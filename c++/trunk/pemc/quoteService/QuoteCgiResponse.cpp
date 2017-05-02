#include "QuoteCgiResponse.h"
#include "QuoteUtils.h"
#include "Product_def.h"
#include "QuoteReal.h"
#include "KLineManager.h"
#include "Sharingline.h"
#include <jsoncpp/json.h>

using namespace Json;

QuoteCgiResponse::QuoteCgiResponse():
    _ativemqConsumer(this),
    _quoteReal(NULL),
    _klineManager(NULL)
{

}

QuoteCgiResponse::QuoteCgiResponse(const QuoteCgiResponse& Qs):
    _ativemqConsumer(this),
    _quoteReal(NULL),
    _klineManager(NULL)
{

}

QuoteCgiResponse::~QuoteCgiResponse()
{

}

void QuoteCgiResponse::on_recv(const std::string& request, std::string& response, const Message* message)
{
    int error_code = 200;
    string error_msg = "成功";

    shared_ptr<CgiRequestTask> spRequestTask;

    try
    {
        Reader reader;
        Value value;
        if (!reader.parse(request, value))
        {
            error_code = 101;
            error_msg = "json 解析失败";
            cout << "json parse fail : " << request << endl;
        }

        string requestType;
        Json::Value js_type = value["Type"];
        if (js_type.isNull() || !js_type.isString())
        {
            error_code = 102;
            error_msg = "Type 类型不存在";
            cout << "error                        01" << endl;
        }
        else
        {
            requestType = js_type.asString();
        }

        if (requestType == CGI_REQUEST_QUOTE_SNAPSHOT)
        {
            string productId;
            Json::Value js_param = value["Param"];
            if (!js_param.isNull())
            {
                Json::Value js_productId = js_param["productId"];
                if (!js_productId.isNull())
                {
                    if (js_productId.isString())
                    {
                        productId = js_productId.asString();
                    }
                    else
                    {
                        //error
                        error_code = 102;
                        error_msg = "productId 类型错误";
                    }
                }
            }

            CgiQuoteSnapShotRequestTask* pTask = new CgiQuoteSnapShotRequestTask();
            pTask->_prodcutId = productId;
            spRequestTask.reset(pTask);

        }
        else if (requestType == CGI_REQUEST_QUOTE_KLINE)
        {
            Json::Value js_param = value["Param"];
            if (js_param.isNull() || !js_param.isObject())
            {
                error_code = 102;
                error_msg = "Param 为空";
            }
            else
            {
                Json::Value js_productId = js_param["productId"];
                if (js_productId.isNull() || !js_productId.isString())
                {
                    error_code = 102;
                    error_msg = "productId 错误";
                }

                Json::Value js_kLineType = js_param["kLineType"];
                if (js_kLineType.isNull() || !js_kLineType.isString())
                {
                    error_code = 102;
                    error_msg = "kLineType 错误";
                }

                CgiQuoteKLineRequestTask* pTask = new CgiQuoteKLineRequestTask();
                pTask->_prodcutId = js_productId.asString();
                pTask->_kLineType = js_kLineType.asString();
                spRequestTask.reset(pTask);
            }
            
        }
        else if (requestType == CGI_REQUEST_QUOTE_SHARELINE)
        {
            Json::Value js_param = value["Param"];
            if (js_param.isNull() || !js_param.isObject())
            {
                error_code = 102;
                error_msg = "Param 为空";
            }
            else
            {
                Json::Value js_productId = js_param["productId"];
                if (js_productId.isNull() || !js_productId.isString())
                {
                    error_code = 102;
                    error_msg = "productId 为空";
                }

                Json::Value js_shareLineType = js_param["shareType"];
                if (js_shareLineType.isNull() || !js_shareLineType.isString())
                {
                    error_code = 102;
                    error_msg = "shareType 为空";
                }

                CgiQuoteShareLineRequestTask* pTask = new CgiQuoteShareLineRequestTask();
                pTask->_prodcutId = js_productId.asString();
                pTask->_shareType = js_shareLineType.asString();
                spRequestTask.reset(pTask);
                dzlog_info(" pTask->_prodcutId:%s", pTask->_prodcutId.c_str());
                dzlog_info(" pTask->_shareType:%s", pTask->_shareType.c_str());
            }
            
        }
        else
        {
            error_code = 103;
            error_msg = "Type 未知类型";
        }
    }
    catch (std::exception &ex)
    {
        cout << "json exception:" << ex.what() << endl;

        error_code = 103;
        error_msg = "json exception:";
    }


    //TODO::业务处理
    string result;
    if (error_code == 200 && spRequestTask != NULL)
    {
        result  = ProcessRequestTask(spRequestTask);
    }
    else
    {
        result = GetErrorCode(error_code, error_msg);
    }
    
    //response
    if (message != NULL)
    {
        //发送到对应的队列里去
        const cms::Destination* dest_send = message->getCMSReplyTo();
        if (NULL != dest_send)
        {
            std::string correlation_id = message->getCMSCorrelationID();
            dzlog_info("correlation_id:%s",correlation_id.c_str());
            _ptr_producer.send_message(dest_send, result, correlation_id);
        }
    }

    //test
    dzlog_info(" send result:%s", result.c_str());
    _ptr_producer.send_message("Quote.Cgi.Reponse_chen", result);
    //test end
}

bool QuoteCgiResponse::Init(const std::string& brokerUri, const std::string& queueName)
{
    if (!_ptr_producer.init(brokerUri))
    {
        return false;
    }

    if (!_ativemqConsumer.init(brokerUri, queueName))
    {
        return false;
    }

    cout << "QuoteCgiResponse 初始化成功！" << endl;

    return true;
}

void QuoteCgiResponse::UnInit()
{
    _ptr_producer.close();
}

void QuoteCgiResponse::SetCallback(QuoteReal* quoteReal, KLineManager* klienManager)
{
    _quoteReal = quoteReal;
    _klineManager = klienManager;
}

string QuoteCgiResponse::ProcessRequestTask(shared_ptr<CgiRequestTask> spRequsetTask)
{
    if (spRequsetTask == NULL)
    {
        return "";
    }

    switch (spRequsetTask->_requestType)
    {
    case REQUEST_TYPE_QUOTE_SNAPSHOT:
        return ProcessQuoteSnapRequestTask(spRequsetTask);
        break;

    case REQUEST_TYPE_QUOTE_KLINE:
        return ProcessQuoteKLineRequestTask(spRequsetTask);
        break;

    case REQUEST_TYPE_QUOTE_SHARELINE:
        return ProcessQuoteShareLineRequestTask(spRequsetTask);
        break;
    }

    return "";
}

string QuoteCgiResponse::ProcessQuoteSnapRequestTask(shared_ptr<CgiRequestTask> spRequsetTask)
{
    CgiQuoteSnapShotRequestTask* pTask = (CgiQuoteSnapShotRequestTask*)spRequsetTask.get();

    if (pTask == NULL)
    {
        cout << "erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        return "";
    }

    Json::Value js_result;
    js_result["code"] = 200;
    js_result["message"] = "成功";
    
    Json::Value js_datalist;

    QuoteSnapshot quoteSnapshot;
    _quoteReal->GetQuoteSnapshot(pTask->_prodcutId, quoteSnapshot);

    for (const QuoteMsg& value : quoteSnapshot)
    {
        //银
        Json::Value js_data;
        js_data["type"] = value._type;
        js_data["exchange"] = value._exchange;
        js_data["market"] = value._market;
        js_data["productId"] = value._symbol;
        js_data["pruductName"] = QuoteUtils::getProductNameById(value._symbol);
        js_data["currency"] = value._currency;
        js_data["bit"] = 2;
        js_data["open"] = value._open;
        js_data["preClose"] = value._close;
        js_data["ask"] = value._ask;
        js_data["bid"] = value._bid;
        js_data["last"] = value._last;
        js_data["high"] = value._high;
        js_data["low"] = value._low;
        js_data["timestamp"] = value._timestamp;
        js_data["tradedate"] = value._tradedate;

        js_datalist.append(js_data);
    }

    js_result["dataList"] = js_datalist;


    Json::FastWriter fastWriter;
    return fastWriter.write(js_result);
}

string QuoteCgiResponse::ProcessQuoteKLineRequestTask(shared_ptr<CgiRequestTask> spRequsetTask)
{
    CgiQuoteKLineRequestTask* pTask = (CgiQuoteKLineRequestTask*)spRequsetTask.get();
    if (pTask == NULL)
    {
        cout << "error !!!!!!!!!!222" << endl;
        return "";
    }

    Json::Value js_data;
    KLineDataList klineDataList;
    _klineManager->GetKLineData(pTask->_prodcutId, pTask->_kLineType, pTask->_startTime, js_data);

    Json::Value js_result;
    js_result["code"] = 200;
    js_result["message"] = "成功";
    js_result["data"] = js_data;

    Json::FastWriter fastWriter;
    return fastWriter.write(js_result);
}

string QuoteCgiResponse::ProcessQuoteShareLineRequestTask(shared_ptr<CgiRequestTask> spRequsetTask)
{
    CgiQuoteShareLineRequestTask* pTask = (CgiQuoteShareLineRequestTask*)spRequsetTask.get();
    if (pTask == NULL)
    {
        cout << "error !!!!!!!!!!222" << endl;
        return "";
    }

    Json::Value js_result;
    js_result["code"] = 200;
    js_result["message"] = "成功";

    Json::Value js_data;
    Json::Value js_datalist;
    Json::Value js_datelist;

    std::string productId  = pTask->_prodcutId;
    std::map<int, ShareLineData> tmp_map;
    tmp_map.clear();
    dzlog_info("--------------------before  GetShareLineDate ------------------------");
    _klineManager->GetShareLineDate(productId, tmp_map);
    dzlog_info("--------------------after   GetShareLineDate ------------------------");

    std::map<int, ShareLineData>::iterator iter = tmp_map.begin();
    for( ; iter!=tmp_map.end(); iter++)
    {
        js_datalist.append(iter->second._closePrice);
        js_datelist.append(iter->first);
    }

    js_data["dataList"] = js_datalist;
    js_data["dateList"] = js_datelist;
    js_result["data"] = js_data;

    Json::FastWriter fastWriter;
    return fastWriter.write(js_result);
}

string QuoteCgiResponse::GetErrorCode(int codeNum, const string& errorMsg)
{
    Json::Value js_result;

    js_result["code"] = codeNum;
    js_result["message"] = errorMsg;


    Json::FastWriter fastWriter;
    return fastWriter.write(js_result);
}

