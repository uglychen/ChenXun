#include "QuoteReal.h"
#include "Product_def.h"
#include "QuoteUtils.h"
#include "jsoncpp/json.h"
#include <iostream>

using std::string;
QuoteReal::QuoteReal():
	_msgId(0),
	_mqttPulish(ADDRESS, CLIENTID_QUOTE),
	_thread_id(0),
	_run(false)
{
}

QuoteReal::~QuoteReal()
{
	uninit();
}


void QuoteReal::PushQuoteMsg(const QuoteMsg& quoteMsg)
{
	_quoteBuffer.push(quoteMsg);
}

bool QuoteReal::init()
{
	_run = true;
	if (0 != pthread_create(&_thread_id, NULL, proc_thread_func, (void*)this))
	{
		return false;
	}

	return true;
}

void QuoteReal::uninit()
{
	_mqttPulish.uninit();
	_quoteRealStateList.clear();

	_run = false;
	if (0 != pthread_join(_thread_id, NULL))
	{

	}
}


void QuoteReal::GetQuoteSnapshot(const string& prudctId, QuoteSnapshot& quoteSnapshot)
{
	quoteSnapshot.clear();
	_rwlock.rdlock();
	if (prudctId.empty())
	{
		//全部
		CurQuoteMsg::iterator pos = _curQuoteMsg.begin();
		for (; pos != _curQuoteMsg.end(); pos++)
		{
			quoteSnapshot.push_back(pos->second);
		}
	}
	else
	{
		CurQuoteMsg::iterator pos = _curQuoteMsg.find(prudctId);
		if (pos != _curQuoteMsg.end())
		{
			quoteSnapshot.push_back(pos->second);
		}
		
	}
	_rwlock.unlock();
}


void QuoteReal::UpdateCurQuote(const QuoteMsg& quoteMsg)
{
	_rwlock.wrlock();
	_curQuoteMsg[quoteMsg._symbol] = quoteMsg;
	_rwlock.unlock();
}

bool QuoteReal::InitData()
{
	_mqttPulish.init();
	_quoteRealStateList.clear();
	_quoteRealStateList[PRODUCT_ID_SLIVER] = QuoteRealState(PRODUCT_ID_SLIVER, "", TOPIC_QUOTE_SLIVER);
	_quoteRealStateList[PRODUCT_ID_PALLADIUM] = QuoteRealState(PRODUCT_ID_PALLADIUM, "", TOPIC_QUOTE_PALLADIUM);
	_quoteRealStateList[PRODUCT_ID_PLATINUM] = QuoteRealState(PRODUCT_ID_PLATINUM, "", TOPIC_QUOTE_PLATINUM);
	return true;
}

void* QuoteReal::proc_thread_func(void *param)
{
	QuoteReal* quoteRealSef = (QuoteReal*)param;
	if (quoteRealSef == NULL)
	{
		return NULL;
	}

	quoteRealSef->InitData();

	while (quoteRealSef->_run)
	{
		QuoteMsg quoteMsg;
		if (false == quoteRealSef->_quoteBuffer.pop(quoteMsg))
		{
			usleep(10);
			continue;
		}

		quoteRealSef->runQuoteReal(quoteMsg);
	}

	return NULL;
}

string QuoteReal::runQuoteReal(const QuoteMsg& quoteMsg)
{
    dzlog_info("实时行情： %s", quoteMsg._symbol.c_str());
	UpdateCurQuote(quoteMsg);
	//价格发生变化之后才推送
	QuoteRealStateList::iterator pos = _quoteRealStateList.find(quoteMsg._symbol);
	if (pos != _quoteRealStateList.end())
	{
		QuoteRealState& quoteRealState = pos->second;
	//	if (quoteRealState._lastPrice != quoteMsg._last)
		{
			quoteRealState._lastPrice = quoteMsg._last;
			Json::Value js_result;
			js_result["code"] = 200;
			js_result["message"] = "成功";

			Json::Value js_data;
			js_data["type"] = quoteMsg._type;
			js_data["exchange"] = quoteMsg._exchange;
			js_data["market"] = quoteMsg._market;
			js_data["productId"] = quoteMsg._symbol;
			js_data["pruductName"] = QuoteUtils::getProductNameById(quoteMsg._symbol);
			js_data["currency"] = quoteMsg._currency;
			js_data["bit"] = quoteMsg._bit;
			js_data["open"] = quoteMsg._open;
			js_data["preClose"] = quoteMsg._close;
			js_data["ask"] = quoteMsg._ask;
			js_data["bid"] = quoteMsg._bid;
			js_data["last"] = quoteMsg._last;
			js_data["high"] = quoteMsg._high;
			js_data["low"] = quoteMsg._low;
			js_data["timestamp"] = quoteMsg._timestamp;
			js_data["tradedate"] = quoteMsg._tradedate;

			js_result["data"] = js_data;
			Json::FastWriter fastWriter;
			string resultText =  fastWriter.write(js_result);
			
			_mqttPulish.publishMsg(quoteRealState._topicName, resultText);
		}
	}

	return "";
}


