#ifndef _QUOTE_REAL_H_
#define _QUOTE_REAL_H_
#include "QuoteDataDef.h"
#include "Quote_buffer.h"
#include "MqttPublish.h"
#include "xn_rwlock.h"
#include <pthread.h>
#include <unistd.h>
#include <map>

using namespace std;

namespace sql{
	class Connection;
	class Driver;
	class Statement;
}

/** 实时行情状态
*/
class QuoteRealState 
{
public:
	QuoteRealState()
	{
	}
	QuoteRealState(const string& productId, const string& lastPrice, const string& topicName):
		_prodcutId(productId),
		_lastPrice(lastPrice),
		_topicName(topicName)
	{

	}

	//产品ID
	string _prodcutId;
	//最新价
	string _lastPrice;
	//主题名字
	string _topicName;
};

/** 实时行情服务
*/
class QuoteReal
{
public:
	QuoteReal();
	~QuoteReal();

	//添加到消息队列
	void PushQuoteMsg(const QuoteMsg& quoteMsg);

	//运行实时行情
	string runQuoteReal(const QuoteMsg& quoteMsg);

	//初始化
	bool init();

	//反初始化
	void uninit();

	//获取行情快照
	void GetQuoteSnapshot(const string& prudctId, QuoteSnapshot& quoteSnapshot);

	//更新快照
	void UpdateCurQuote(const QuoteMsg& quoteMsg);

	bool InitData();

private:
	static void* proc_thread_func(void *param);

private:
	//mqtt 消息发布对象
	MqttPublish _mqttPulish;

	//消息的个数
	long long _msgId;

	//实时行情状态列表
	typedef map<string, QuoteRealState> QuoteRealStateList;
	QuoteRealStateList _quoteRealStateList;

	//行情快照
	typedef map<string, QuoteMsg> CurQuoteMsg;
	CurQuoteMsg _curQuoteMsg;

	//行情缓冲区
	Quote_buffer _quoteBuffer;

	pthread_t _thread_id;
	volatile bool _run;

	//读写锁
	xn_rwlock _rwlock;
};
#endif
