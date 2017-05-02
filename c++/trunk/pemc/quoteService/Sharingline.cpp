#include "Sharingline.h"
#include "Product_def.h"
#include "QuoteUtils.h"
#include "DBPool.h"
#include "KLineConfig.h"
#include "jsoncpp/json.h"

SharingLine::SharingLine(ShareLineType shareLineType, const string& address, const string& clientId):
    _shareLineType(shareLineType),
    _mqttPulish(address, clientId),
    _thread_id(0),
    _run(false),
    _curTradeDate(0)
{
}

SharingLine::~SharingLine()
{
    UnInitBase();
}

void SharingLine::runQuoteReal(const QuoteMsg& quoteMsg)
{
    if (_curTradeDate != quoteMsg._tradedate)
    {
        _curTradeDayData.clear();
        _curTradeDate = quoteMsg._tradedate;
    }

    _rwlock.wrlock();
    insertMap(quoteMsg);
    _rwlock.unlock();
}

void SharingLine::insertMap(const QuoteMsg& quoteMsg)
{
    ProdcutShareLineDataList::iterator pos =  _curTradeDayData.find(quoteMsg._symbol);
    //时间戳分钟取整
    int roundTime = QuoteUtils::roundTimeMimute(quoteMsg._timestamp, _flag);
    bool isUpdate = false;
    if(pos == _curTradeDayData.end())
    {
        ShareLineData tmpData;
        tmpData._stockTime = roundTime;
        tmpData._closePrice = quoteMsg._last;

        SharingLineDataMap tmp;
        tmp.clear();
        tmp[roundTime] = tmpData;
        _curTradeDayData[quoteMsg._symbol] = tmp;
        isUpdate = true;
        InsertIntoDataBase(quoteMsg._symbol, tmpData);  //程序启动数据入库
    }
    else
    {
        SharingLineDataMap& dataMap = pos->second;
        SharingLineDataMap::iterator iter = dataMap.find(roundTime);
        if( iter == dataMap.end() )
        {
            ShareLineData tmpData;
            tmpData._stockTime = roundTime;
            tmpData._closePrice = quoteMsg._last;
            dataMap[roundTime] = tmpData;
            isUpdate = true;
            InsertIntoDataBase(quoteMsg._symbol, tmpData);  //时间戳更新，接受到新的数据的入库
        }
        else
        {
            if( iter->second._stockTime == 0 )
            {
                iter->second._stockTime = roundTime;
                iter->second._closePrice = quoteMsg._last;
                isUpdate = true;
            }
            else 
            {
                if (iter->second._closePrice != quoteMsg._last)
                {
                    iter->second._closePrice = quoteMsg._last;
                    isUpdate = true;
                    ShareLineData tmpData = iter->second;
                    updateShareLineDB(quoteMsg._symbol, tmpData);
                }
            }
        }
    }

    //行情数据有变化就推送给客户端
    if (isUpdate == true)
    {
        ShareLineData tmp_send_mqtt; 
        tmp_send_mqtt._stockTime = ((_curTradeDayData[quoteMsg._symbol])[roundTime])._stockTime;
        tmp_send_mqtt._closePrice = ((_curTradeDayData[quoteMsg._symbol])[roundTime])._closePrice;
        SendUpdate(quoteMsg._symbol, tmp_send_mqtt);
        isUpdate = false;
    }
}

void SharingLine::GetShareLineData(const std::string& productId, std::map<int, ShareLineData>& data_map)
{
    _rwlock.rdlock();
    ProdcutShareLineDataList::iterator p_iter =  _curTradeDayData.find(productId);
    if( p_iter != _curTradeDayData.end())
    {
        SharingLineDataMap& dataMap = p_iter->second;
        if(dataMap.size() > 0)
        {
            SharingLineDataMap::iterator iter = dataMap.begin();
            for ( ; iter!= dataMap.end(); iter++)
            {
                ShareLineData tmpData;
                tmpData._stockTime = iter->second._stockTime;
                tmpData._closePrice = iter->second._closePrice;
                data_map[iter->first] = tmpData;
                dzlog_info("stock_time:%d  _closePrice:%s", iter->second._stockTime, iter->second._closePrice.c_str());
            }
        }
    }
    _rwlock.unlock();
}

void SharingLine::InitShareLineInfo()
{
    ProductInfoParm productInfoParm;
    KLineConfig::getProductInfoParm(_shareLineType, productInfoParm);

    ProductInfoParm::const_iterator ibeg = productInfoParm.begin();
    ProductInfoParm::const_iterator iend = productInfoParm.end();

    //初始化产品列表
    _prodcutList.clear();

    //初始化缓冲区
    for (; ibeg != iend; ibeg++)
    {
        _prodcutList[ibeg->_productId] = *ibeg;
        time_map[ibeg->_productId] = 0;
    }
     _curTradeDayData.clear();
}

void SharingLine::PushQuoteMsg(const QuoteMsg& quoteMsg)
{
    _quoteBuffer.push(quoteMsg);
}

void SharingLine::InitBase()
{
    InitShareLineInfo();
    _mqttPulish.init();
}

void SharingLine::UnInitBase()
{
    _mqttPulish.uninit();
}

void SharingLine::ExecuteDb(const string& str)
{
    //TODO:执行入库操作
    if (str.empty())
    {
        return;
    }

    try {
        sql::Connection* con = connpool.GetConnection();
        //if (con != NULL)
        if( con == NULL )
        {
            connpool.ReleaseConnection(con);
            dzlog_info("-------------connpool error! con == NULL---------------");
        }
        else
        {
            sql::Statement* state = con->createStatement();
            if (state != NULL)
            {
                state->execute(str);
                delete state;
            }
            else
            {
                dzlog_info("-------------connpool error! state== NULL---------------");
            }

            connpool.ReleaseConnection(con);
        }
    }
    catch (sql::SQLException &ex) {
        std::cout << ex.what() << std::endl;
    }
}

void SharingLine::Init()
{
    _run = true;
    if (0 != pthread_create(&_thread_id, NULL, proc_thread_func, (void*)this))
    {
        return;
    }
}

void SharingLine::SendUpdate(const string& productId, const ShareLineData& klinedata)
{
    ProductInfoList::iterator pos = _prodcutList.find(productId);
    if (pos != _prodcutList.end())
    {
        //publish msg to mqtt
        const ProdcutInfo& productInfo = pos->second;

        Json::Value js_result;
        js_result["code"] = "200";
        js_result["message"] = "成功";

        Json::Value js_data;
        js_data["last"] = klinedata._closePrice;
        js_data["timestamp"] = klinedata._stockTime;

        js_result["data"] = js_data;
        Json::FastWriter fastWriter;
        string text = fastWriter.write(js_result);
        dzlog_info("推送分时线:%s", productInfo._topicName.c_str());
        //cout << "推送分时线:  " << productInfo._topicName << endl;
        dzlog_info("推送分时线数据:%s", text.c_str());

        _mqttPulish.publishMsg(productInfo._topicName, text);
    }
}

void SharingLine::InsertIntoDataBase(const string& productId, const ShareLineData& data)
{
    //TODO:
    dzlog_info("productId: %s", productId.c_str());
    ProductInfoList::iterator pos = _prodcutList.find(productId);
    if (pos != _prodcutList.end())
    {
        ProdcutInfo& productInfo = pos->second;

        string tableName = productInfo._tableName;
        string uuid = QuoteUtils::getUuid();
        string Id = productId;
        string productName = QuoteUtils::getProductNameById(productId);
        string createTime = QuoteUtils::getSqlUnixTime();
        string stockTime = QuoteUtils::getSqlUnixTime(data._stockTime);
        char buffer[1024] = { 0 };
        sprintf(buffer, "INSERT INTO calf_middle_gg.%s (ID, invest_product_name, invest_product_id, stock_time, closing_price, create_time, opening_price,highest_price,floor_price,remark) VALUES (\"%s\", \"%s\", \"%s\", %s, \"%s\",%s,\"1\",\"2\",\"3\" ,\"4\");",
            tableName.c_str(),
            uuid.c_str(),
            productName.c_str(),
            Id.c_str(),
            stockTime.c_str(),
            data._closePrice.c_str(),
            createTime.c_str()
        );
        string statement(buffer);
        dzlog_info("sqlstatment is: %s", statement.c_str());
        ExecuteDb(statement);
    }
}

void SharingLine::updateShareLineDB(const string& productId, ShareLineData& data)
{
    dzlog_info("productId: %s", productId.c_str());
    ProductInfoList::iterator pos = _prodcutList.find(productId);
    if (pos != _prodcutList.end())
    {
        ProdcutInfo& productInfo = pos->second;
        string tableName = productInfo._tableName;
        char buffer[512] = { 0 };
        string createTime = QuoteUtils::getSqlUnixTime();
        sprintf(buffer, "UPDATE calf_middle_gg.%s SET closing_price = '%s', modify_time = %s where stock_time = FROM_UNIXTIME(%d)",
            tableName.c_str(), 
            data._closePrice.c_str(), 
            createTime.c_str(), 
            data._stockTime);

        string statement(buffer);
        dzlog_info("sqlstatment is: %s", statement.c_str());
        ExecuteDb(statement);
    }
}

void* SharingLine::proc_thread_func(void *param)
{

    SharingLine* p_ShareLine = (SharingLine*)param;
    if (p_ShareLine == NULL)
    {
        return NULL;
    }

    p_ShareLine->InitBase();

    while ( true )
    {
        QuoteMsg quoteMsg;
        if (false == p_ShareLine->_quoteBuffer.pop(quoteMsg))
        {
            usleep(10);
            continue;
        }

        p_ShareLine->runQuoteReal(quoteMsg);
    }

    return NULL;
}

