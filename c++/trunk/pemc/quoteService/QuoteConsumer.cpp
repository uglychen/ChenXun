#include "QuoteConsumer.h"
#include "QuoteDataDef.h"
#include "Product_def.h"
#include "QuoteReal.h"
#include "KLineManager.h"
#include <iostream>
#include <stdexcept>
#include <jsoncpp/json.h>

using namespace Json;
using std::string;


QuoteConsumer::QuoteConsumer():
    _msgId(0),
    _quoteReal(NULL),
    _klineManager(NULL)
{

}


QuoteConsumer::~QuoteConsumer()
{

}

bool QuoteConsumer::init(const string& brokerUri)
{
    bool useTopic = true;
    bool clientAck = false;
    std::vector<std::string> destURIVec;
    destURIVec.push_back(QUOTE_SINGLE_ORG);
    _conumer.initConsumer(BROKER_URI, destURIVec, useTopic, clientAck);
    _conumer.set_notify(this);

    return true;
}

void QuoteConsumer::uninit()
{
    _conumer.close();
}


void QuoteConsumer::on_recv(const std::string& request, std::string& response, const Message* message)
{
    dzlog_info("收到行情数目：%ld 内容：%s", ++_msgId, request.c_str());
    parseQuote(request);
}


void QuoteConsumer::StartRecvMsg()
{
    _conumer.startReceiveMsgFromMq();
}

string QuoteConsumer::parseQuote(const string& quote)
{
    if (quote.empty())
    {
        return "";
    }
    
    try 
    {
        Reader reader;
        Value value;
        if (!reader.parse(quote, value))
        {
            dzlog_error("json 解析失败:%s", quote.c_str());
            return "";
        }

        QuoteMsg quoteMsg;
        quoteMsg._quoteOrg = quote;
        quoteMsg._type = value["TYPE"].asInt();
        quoteMsg._exchange = value["EXCHANGE"].asString();
        quoteMsg._market = value["MARKET"].asString();
        quoteMsg._symbol = value["SYMBOL"].asString();
        quoteMsg._currency = value["CURRENCY"].asString();
        quoteMsg._bit = value["BIT"].asInt();
        quoteMsg._open = value["OPEN"].asString();
        quoteMsg._close = value["PRECLOSE"].asString();
        quoteMsg._ask = value["ASK"].asString();
        quoteMsg._bid = value["BID"].asString();
        quoteMsg._last = value["LAST"].asString();
        quoteMsg._high = value["HIGH"].asString();
        quoteMsg._low = value["LOW"].asString();
        quoteMsg._timestamp = value["TIMESTAMP"].asInt();
        quoteMsg._tradedate = value["TRADEDATE"].asInt();

        if (_quoteReal != NULL)
        {
            _quoteReal->PushQuoteMsg(quoteMsg);
        }

        if (_klineManager != NULL)
        {
            _klineManager->PushQuoteMsg(quoteMsg);
        }
    }
    catch (std::exception &ex)
    {
        dzlog_error("json 异常 %s", ex.what());
        return "";
    }

    return "";
}

void QuoteConsumer::SetCallback(QuoteReal* quoteReal, KLineManager* klienManager)
{
    _quoteReal = quoteReal;
    _klineManager = klienManager;
}

