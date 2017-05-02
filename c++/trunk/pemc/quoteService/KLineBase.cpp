#include "KLineBase.h"
#include "Product_def.h"
#include "QuoteUtils.h"
#include "DBPool.h"
#include "KLineConfig.h"

KLineBase::KLineBase(const KLINE_TYPE klineType, const string& address, const string& clientId):
	_klineType(klineType),
	_mqttPulish(address, clientId),
	_thread_id(0),
	_run(false)
{
}

KLineBase::~KLineBase()
{
	UnInitBase();
}

void KLineBase::runQuoteReal(const QuoteMsg& quoteMsg)
{
	KLineProductList::iterator pos = _klineProductList.find(quoteMsg._symbol);
	if (pos != _klineProductList.end())
	{
		pos->second->ProcessProductQuote(quoteMsg);
	}
}

void KLineBase::InitklineInfo()
{
    _klineProductList.clear();
    const ProductList& productList = KLineConfig::getProductList();
    for (const string& value : productList)
    {
        shared_ptr<KLineProdcut> ptrProudct(new KLineProdcut());
        ptrProudct->InitData(value, _klineType, this);
        _klineProductList[value] = ptrProudct;
    }

}

void KLineBase::GetKLineData(const string& productId, const string& startTime, Json::Value& data)
{
    KLineProductList::iterator pos = _klineProductList.find(productId);
    if (pos != _klineProductList.end())
    {
        pos->second->GetKLineData(startTime, data);
    }
}

void KLineBase::PushQuoteMsg(const QuoteMsg& quoteMsg)
{
	_quoteBuffer.push(quoteMsg);
}

void KLineBase::InitBase()
{
	InitklineInfo();
	_mqttPulish.init();
}

void KLineBase::UnInitBase()
{
	_mqttPulish.uninit();
}

bool KLineBase::ExecuteDb(const string& str)
{
	//TODO:执行入库操作
	if (str.empty())
	{
		return false;
	}
	bool result = true;
	try {
		sql::Connection* con = connpool.GetConnection();
		if (con != NULL)
		{
            sql::Statement* state = con->createStatement();
            if (state != NULL)
            {
                state->execute(str);
                delete state;
            }
            else
            {
                dzlog_error("数据库 state 为空");
                result = false;
            }

            connpool.ReleaseConnection(con);
		}
        else
        {
            dzlog_error("数据库 con 为空");
            result = false;
        }
		
	}
	catch (sql::SQLException &ex) {
        dzlog_error("数据库 异常%s", ex.what());
		result = false;
	}

	return result;
}

void KLineBase::Init()
{
	_run = true;
	if (0 != pthread_create(&_thread_id, NULL, proc_thread_func, (void*)this))
	{
		return;
	}
}

void KLineBase::publishMsg(const string& topicName, const string& msg)
{
	_mqttPulish.publishMsg(topicName, msg);
}

void* KLineBase::proc_thread_func(void *param)
{
	KLineBase* pKlineBase = (KLineBase*)param;
	if (pKlineBase == NULL)
	{
		return NULL;
	}

	pKlineBase->InitBase();

	while (pKlineBase->_run)
	{
		QuoteMsg quoteMsg;
		if (false == pKlineBase->_quoteBuffer.pop(quoteMsg))
		{
			usleep(10);
			continue;
		}

		pKlineBase->runQuoteReal(quoteMsg);
	}

	return NULL;
}

