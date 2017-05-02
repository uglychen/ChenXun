#include "KLineProdcut.h"
#include "QuoteUtils.h"
#include "KLineConfig.h"
#include "DBPool.h"
#include <iostream>

KLineProdcut::KLineProdcut():
	_curTradeDate(0),
	_pKlineCallback(NULL),
    _dayInterval(0)
{

}

KLineProdcut::~KLineProdcut()
{

}

void KLineProdcut::InitData(const string& productId, const KLINE_TYPE& klineType, IKLineCallBack* pKlineCallBack)
{
	_productId = productId;
	_klineType = klineType;
	_pKlineCallback = pKlineCallBack;
	_topicName = KLineConfig::getMqttTopicName(productId, klineType);
	_tableName = KLineConfig::getTableName(productId, klineType);
    _dayInterval = KLineConfig::getIntervalDay(klineType);
	Reset();
}

void KLineProdcut::ProcessProductQuote(const QuoteMsg& quoteMsg)
{
	//检查交易日是否发生变化
	CheckTradeDate(quoteMsg._tradedate);

	//生成k线数据
	ProcessKlineData(quoteMsg);

	//检查行情是否发生变化
	CheckQuoteChange(quoteMsg);
}

void KLineProdcut::GetKLineData(const string& startTime, Json::Value& data)
{
    _rwlock.rdlock();
    Json::Value js_datalist;
    Json::Value js_datelist;

    KlineInfoList::iterator ipos = _klineInfolist.begin();
    KlineInfoList::iterator iend = _klineInfolist.end();

    for (; ipos != iend; ipos++)
    {
        Json::Value object;
        //开盘价
        object.append(ipos->second._open);
        //收盘价
        object.append(ipos->second._close);
        //最高价
        object.append(ipos->second._high);
        //最低价
        object.append(ipos->second._low);
        js_datalist.append(object);
        js_datelist.append(QuoteUtils::timeToFmt(ipos->first));
    }

    data["dataList"] = js_datalist;
    data["dateList"] = js_datelist;

    _rwlock.unlock();
}

void KLineProdcut::CheckTradeDate(const int tradeDate)
{
	//init tradedate
	if (_curTradeDate == 0)
	{
		_curTradeDate = tradeDate;
	}
	else if (_curTradeDate != tradeDate)
	{
		// tradedate change
		_curTradeDate = tradeDate;
		OnTradeDateChange();
	}
}

void KLineProdcut::OnTradeDateChange()
{
	//TODO:
	//1. 日线入库
	//2. 重新加载内存和数据库数据等操作
	//3. 重新初始化其他数据等等
	//4. 重新初始化数据等等
	Reset();
}

void KLineProdcut::ProcessKlineData(const QuoteMsg& quoteMsg)
{
	/* 规则：
	前一根的收盘价是下一根的开盘价
	交易日第一根暂时按照开盘价来处理，或者昨收
	*/

	//获取k线的时间
	int klineTime = QuoteUtils::roundTime(quoteMsg._timestamp, _klineType);


	//开盘第一根初始化
	if (_curKlineInfo._stockTime == 0)
	{
		_curKlineInfo._stockTime = klineTime;
		_curKlineInfo._open = quoteMsg._last;
		_curKlineInfo._close = quoteMsg._last;
		_curKlineInfo._high = quoteMsg._last;
		_curKlineInfo._low = quoteMsg._last;
		//插入数据库数据
		InsertIntoData(_curKlineInfo);
	}
	else
	{
		//产生新的k线
		if (_curKlineInfo._stockTime != klineTime)
		{
			//插入新的K线
			_curKlineInfo._stockTime = klineTime;
			_curKlineInfo._open = _curKlineInfo._close;
			_curKlineInfo._close = quoteMsg._last;
			_curKlineInfo._high = quoteMsg._last;
			_curKlineInfo._low = quoteMsg._last;
			InsertIntoData(_curKlineInfo);
		}
		else
		{
			//更新当前k线数据
			_curKlineInfo._close = quoteMsg._last;
			if (QuoteUtils::cmpPrice(_curKlineInfo._low, quoteMsg._last) == 1)
			{
				_curKlineInfo._low = quoteMsg._last;
			}

			if (QuoteUtils::cmpPrice(_curKlineInfo._high, quoteMsg._last) == -1)
			{
				_curKlineInfo._high = quoteMsg._last;
			}

			UpdateData(_curKlineInfo);
		}
	}
}

void KLineProdcut::CheckQuoteChange(const QuoteMsg& quoteMsg)
{
	bool quoteChange = false;
	if (_lastStockTime != quoteMsg._timestamp)
	{
		_lastStockTime = quoteMsg._timestamp;
		quoteChange = true;
	}

	if (_lastPrice != quoteMsg._last)
	{
		_lastPrice = quoteMsg._last;
		quoteChange = true;
	}

	if (quoteChange)
	{
		OnQuoteChange();
	}
}

void KLineProdcut::OnQuoteChange()
{
	//发送行情到MQTT主题上面

	if (_pKlineCallback != NULL)
	{
		Json::Value js_result;
		js_result["code"] = "200";
		js_result["message"] = "成功";

		Json::Value js_data;
		js_data["open"] = _curKlineInfo._open;
		js_data["close"] = _curKlineInfo._close;
		js_data["high"] = _curKlineInfo._high;
		js_data["low"] = _curKlineInfo._low;
		js_data["timestamp"] = _curKlineInfo._stockTime;

		js_result["data"] = js_data;
		Json::FastWriter fastWriter;
		string text = fastWriter.write(js_result);

		_pKlineCallback->publishMsg(_topicName, text);
	}

}

void KLineProdcut::InsertIntoData(const KlineInfo2& klineInfo)
{
	//对数据进行加锁处理
	_rwlock.wrlock();
	KlineInfoList::iterator pos = _klineInfolist.find(klineInfo._stockTime);
	if (pos != _klineInfolist.end())
	{
		//error！！！！！1
		cout << "error!!!!!1" << endl;
	}
	else
	{
		//插入数据
		KlineInfo info;
		info._open = klineInfo._open;
		info._close = klineInfo._close;
		info._high = klineInfo._high;
		info._low = klineInfo._low;
		_klineInfolist[klineInfo._stockTime] = info;
	}
	_rwlock.unlock();

	InsertIntoDataBase(klineInfo);
}

void KLineProdcut::UpdateData(const KlineInfo2& klineInfo)
{
	//对数据进行加锁处理
	_rwlock.wrlock();
	KlineInfoList::iterator pos = _klineInfolist.find(klineInfo._stockTime);
	if (pos != _klineInfolist.end())
	{
		pos->second._open = klineInfo._open;
		pos->second._close = klineInfo._close;
		pos->second._high = klineInfo._high;
		pos->second._low = klineInfo._low;
	}
	else
	{
        dzlog_warn("插入数据失败！%s", QuoteUtils::timeToFmt(klineInfo._stockTime).c_str());
	}
	_rwlock.unlock();

	UpdateDataBase(klineInfo);
}

void KLineProdcut::Reset()
{
    _rwlock.wrlock();
	_lastStockTime = 0;
	_lastPrice = "";
	_klineInfolist.clear();
	_curKlineInfo = KlineInfo2();
    LoadHisKlineData();
    _rwlock.unlock();
    dzlog_info("%s 重置数据个数：%d", _productId.c_str(), _klineInfolist.size());
}

void KLineProdcut::InsertIntoDataBase(const KlineInfo2& klineInfo)
{
	string uuid = QuoteUtils::getUuid();
	string productName = QuoteUtils::getProductNameById(_productId);
	cout << "productName:  " << productName << endl;
	string createTime = QuoteUtils::getSqlUnixTime();
	string stockTime = QuoteUtils::getSqlUnixTime(klineInfo._stockTime);
	char buffer[4096] = { 0 };
	sprintf(buffer, "INSERT INTO %s (ID, invest_product_id, invest_product_name, stock_time, opening_price, closing_price, highest_price, floor_price, create_time, state) VALUES(\"%s\", \"%s\", \"%s\", %s, \"%s\",\"%s\", \"%s\", \"%s\", %s, %d)",
		_tableName.c_str(),
		uuid.c_str(),
		_productId.c_str(),
		productName.c_str(),
		stockTime.c_str(),
		klineInfo._open.c_str(),
		klineInfo._close.c_str(),
		klineInfo._high.c_str(),
		klineInfo._low.c_str(),
		createTime.c_str(),
		1
	);
	string statement(buffer);

	if (_pKlineCallback != NULL)
	{
		_pKlineCallback->ExecuteDb(statement);
	}
}

void KLineProdcut::UpdateDataBase(const KlineInfo2& klineInfo)
{
	string modifyTime = QuoteUtils::getSqlUnixTime();
	string stockTime = QuoteUtils::getSqlUnixTime(klineInfo._stockTime);
	char buffer[4096] = { 0 };
	sprintf(buffer, "UPDATE %s SET opening_price=\"%s\",closing_price=\"%s\",highest_price=\"%s\", floor_price=\"%s\",modify_time=%s WHERE stock_time=%s",
		_tableName.c_str(),
		klineInfo._open.c_str(),
		klineInfo._close.c_str(),
		klineInfo._high.c_str(),
		klineInfo._low.c_str(),
		modifyTime.c_str(),
		stockTime.c_str()
	);
	string statement(buffer);

	if (_pKlineCallback != NULL)
	{
		_pKlineCallback->ExecuteDb(statement);
	}
}

bool KLineProdcut::LoadHisKlineData()
{
    bool result = true;
	_klineInfolist.clear();

	char buffer[4096] = { 0 };
	sprintf(buffer, "SELECT stock_time,opening_price,closing_price,highest_price,floor_price FROM %s \
	where stock_time > DATE_SUB(NOW(), INTERVAL %d DAY) ORDER BY stock_time ASC",
		_tableName.c_str(),
        _dayInterval
	);

	string str(buffer);
	try
	{
		sql::Connection* con = connpool.GetConnection();
		if (con != NULL)
		{
            sql::Statement* state = con->createStatement();
            if (state != NULL)
            {
                sql::ResultSet *res = state->executeQuery(str);
                if (res != NULL)
                {
                    while (res->next())
                    {

                        KlineInfo klineInfo;
                        klineInfo._open = res->getString("opening_price");
                        klineInfo._close = res->getString("closing_price");
                        klineInfo._high = res->getString("highest_price");
                        klineInfo._low = res->getString("floor_price");
                        string stockTime = res->getString("stock_time");
                        int nTime = QuoteUtils::fmtToTime(stockTime);
                        _klineInfolist[nTime] = klineInfo;
                    }

                    delete res;
                }
                else
                {
                    dzlog_error("数据库ResultSet为空！");
                    result = false;
                }
                delete state;
                
            }
            else
            {
                dzlog_error("数据库Statement为空！");
                result = false;
            }

            connpool.ReleaseConnection(con);
		}
        else
        {
            dzlog_error("数据库连接为空！");
            return false;
        }

		
	}
	catch (sql::SQLException &ex) 
    {
        dzlog_error("数据库异常！ state:%s errorcode:%d", ex.getSQLStateCStr(), ex.getErrorCode());
        result = false;
	}

	return result;
}
