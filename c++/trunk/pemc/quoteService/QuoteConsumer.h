#ifndef _QUOTE_CONSUMER_H_
#define _QUOTE_CONSUMER_H_
#include "consumer.h"

class QuoteReal;
class KLineManager;

using std::string;

/** 行情获取程序，从mq 上接收原始行情，并分发给各个行情k线处理类
*/
class QuoteConsumer :
	public msg_recv_interface
{
public:
	QuoteConsumer();
	~QuoteConsumer();

	//解析行情数据
	string parseQuote(const string& quote);

	void SetCallback(QuoteReal* quoteReal, KLineManager* klienManager);

	bool init(const string& brokerUri);
	void uninit();

	//消息回调函数
	virtual void on_recv(const std::string& request, std::string& response, const Message* message);
	
    //开始接收数据
    void StartRecvMsg();
private:
	//消息的个数
	long long _msgId;

	//实时行情处理
	QuoteReal* _quoteReal;

	//k线、分时线处理
	KLineManager* _klineManager;

    //active consumer
    ActiveMqConsumer _conumer;
};
#endif