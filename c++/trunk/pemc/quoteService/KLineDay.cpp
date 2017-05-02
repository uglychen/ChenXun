#include "KLineDay.h"
#include "Product_def.h"
#include "QuoteUtils.h"
#include <jsoncpp/json.h>

using namespace Json;
//日缓冲大小 1
#define BUF_SIZE 1

KLineDay::KLineDay():
	KLineBase(KLINE_TYPE_1DAY, ADDRESS, CLIENTID_KLINE_1DAY)
{
}

KLineDay::~KLineDay()
{

}
#if 0

void KLineDay::runQuoteReal(const QuoteMsg& quoteMsg)
{
	cout << "日线在运行！！！！！！！！！！" << endl;
	if (_curTradeDate != quoteMsg._tradedate)
	{
		_curTradeDate = quoteMsg._tradedate;
	}

	ProdcutKLineDataList::iterator pos = _curTradeDayData.find(quoteMsg._symbol);
	if (pos != _curTradeDayData.end())
	{
		KLineDataList& datalist = pos->second;

		int index = 0;
		KLINE_DATA& lineData = datalist[index];
		if (CheckUpdateData(lineData, quoteMsg._timestamp, quoteMsg._last, quoteMsg._high, quoteMsg._low))
		{
			SendUpdate(quoteMsg._symbol, lineData);
		}

		//收盘之后保存数据库
		if (IsClose(quoteMsg))
		{
			InsertIntoDataBase(quoteMsg._symbol, lineData);
		}
	}
}

void KLineDay::ReadHistoryKLineData()
{
	// 日期
}

bool KLineDay::CheckUpdateData(KLINE_DATA& klinedata, const int stockTime, const string& lastPrice, const string& hightPrice, const string& lowPrice)
{
	bool isUpdate = false;
	cout << "CheckUpdateData day!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << endl;
	if (klinedata._stockTime == 0)
	{
		int roundTime = QuoteUtils::roundTime(stockTime, _klineType);
		if (roundTime != -1)
		{
			klinedata._stockTime = roundTime;
			klinedata._open = lastPrice;
			klinedata._close = lastPrice;
			klinedata._high = hightPrice;
			klinedata._low = lowPrice;
			isUpdate = true;
		}
	}
	else
	{
		if (klinedata._close != lastPrice)
		{
			klinedata._close = lastPrice;
			isUpdate = true;
		}

		if (QuoteUtils::cmpPrice(klinedata._low, lowPrice) == 1)
		{
			klinedata._low = lowPrice;
			isUpdate = true;
		}

		if (QuoteUtils::cmpPrice(klinedata._high, hightPrice) == -1)
		{
			klinedata._high = hightPrice;
			isUpdate = true;
		}
	}

	return isUpdate;
}

void KLineDay::SendUpdate(const string& productId, const KLINE_DATA& klinedata)
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
		js_data["open"] = klinedata._open;
		js_data["close"] = klinedata._close;
		js_data["high"] = klinedata._high;
		js_data["low"] = klinedata._low;
		js_data["timestamp"] = klinedata._stockTime;

		js_result["data"] = js_data;
		Json::FastWriter fastWriter;
		string text =  fastWriter.write(js_result);
		cout << "推送k线:  " << productInfo._topicName << endl;
		cout << text << endl;

		_mqttPulish.publishMsg(productInfo._topicName, text);
	}
}

void KLineDay::InsertIntoDataBase(const string& productId, const KLINE_DATA& klinedata)
{
	cout << "productId:  " << productId << endl;
	ProductInfoList::iterator pos = _prodcutList.find(productId);
	if (pos != _prodcutList.end())
	{
		ProdcutInfo& productInfo = pos->second;

		string tableName = productInfo._tableName;
		string uuid = QuoteUtils::getUuid();
		string Id = productId;
		string productName = QuoteUtils::getProductNameById(productId);
		cout << "productName:  " << productName << endl;
		string createTime = QuoteUtils::getSqlUnixTime();
		string stockTime = QuoteUtils::getSqlUnixTime(klinedata._stockTime);
		char buffer[4096] = { 0 };
		sprintf(buffer, "INSERT INTO %s (ID, invest_product_id, invest_product_name, stock_time, opening_price, closing_price, highest_price, floor_price, create_time, state) VALUES(\"%s\", \"%s\", \"%s\", %s, \"%s\",\"%s\", \"%s\", \"%s\", %s, %d)",
			tableName.c_str(),
			uuid.c_str(),
			Id.c_str(),
			productName.c_str(),
			stockTime.c_str(),
			klinedata._open.c_str(),
			klinedata._close.c_str(),
			klinedata._high.c_str(),
			klinedata._low.c_str(),
			createTime.c_str(),
			1
		);
		string statement(buffer);
		cout << "sqlstatment is " << statement << endl;
		ExecuteDb(statement);
	}
}

bool KLineDay::IsClose(const QuoteMsg& quoteMsg)
{
	return false;
}

#endif