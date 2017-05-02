#ifndef _K_LINE_BASE_H_
#define _K_LINE_BASE_H_
#include "QuoteDataDef.h"
#include "Quote_buffer.h"
#include "KLineProdcut.h"
#include "MqttPublish.h"
#include "xn_rwlock.h"
#include "KLineProdcut.h"
#include <jsoncpp/json.h>
#include <vector>
#include <map>
#include <string>
#include <pthread.h>
#include <unistd.h>


using namespace std;
class KLineBase:
	public IKLineCallBack
{
public:
	KLineBase(const KLINE_TYPE klineType, const string& address, const string& clientId);
	virtual ~KLineBase();

	//更新K线数据
	virtual void runQuoteReal(const QuoteMsg& quoteMsg);

	//初始化k线产品数据
	virtual void InitklineInfo();

	//获取k线数据
    virtual void GetKLineData(const string& productId, const string& startTime, Json::Value& data);

	void PushQuoteMsg(const QuoteMsg& quoteMsg);

	void InitBase();
	void UnInitBase();

	void Init();

	//mqtt 发布消息
	virtual void publishMsg(const string& topicName, const string& msg);

	//执行数据库操作
	virtual bool ExecuteDb(const string& str);

protected:
	static void* proc_thread_func(void *param);

protected:
	//产品列表
	typedef map<string, shared_ptr<KLineProdcut>> KLineProductList;
	KLineProductList _klineProductList;

	//k线种类
	KLINE_TYPE _klineType;
	
	//mqtt 消息发布对象
	MqttPublish _mqttPulish;

	//行情缓冲区
	Quote_buffer _quoteBuffer;

	pthread_t _thread_id;
	volatile bool _run;
};

#endif
