#ifndef _QUOTE_CGI_RESPONSE_H_
#define _QUOTE_CGI_RESPONSE_H_
#include "trade_consumer.h"
#include "trade_producer.h"

class QuoteReal;
class KLineManager;

//CGI请求任务
enum CgiRequestTaskType
{
    //获取行情快照
    REQUEST_TYPE_QUOTE_SNAPSHOT,
    //获取K线
    REQUEST_TYPE_QUOTE_KLINE,
    //获取分时线
    REQUEST_TYPE_QUOTE_SHARELINE
};

class CgiRequestTask 
{
public:
    CgiRequestTask(CgiRequestTaskType taskType):
        _requestType(taskType)
    {

    }

    CgiRequestTaskType _requestType;
};

class CgiQuoteSnapShotRequestTask:
    public CgiRequestTask
{
public:
    CgiQuoteSnapShotRequestTask():
        CgiRequestTask(REQUEST_TYPE_QUOTE_SNAPSHOT)
    {
    }
    
    //产品编号，如果为空则所有商品都要回复
    string _prodcutId;
};

class CgiQuoteKLineRequestTask :
    public CgiRequestTask
{
public:
    CgiQuoteKLineRequestTask() :
        CgiRequestTask(REQUEST_TYPE_QUOTE_KLINE)
    {
    }

    //产品编号
    string _prodcutId;

    //k线种类
    string _kLineType;

    //开始时间
    string _startTime;
};

class CgiQuoteShareLineRequestTask :
    public CgiRequestTask
{
public:
    CgiQuoteShareLineRequestTask() :
        CgiRequestTask(REQUEST_TYPE_QUOTE_SHARELINE)
    {
    }

    //产品编号
    string _prodcutId;

    //分时线品种
    string _shareType;
};



/** 响应CGI网关https请求 
*/
class QuoteCgiResponse :
    public activemq_trade_consumer_callback
{
public:
    QuoteCgiResponse();
    ~QuoteCgiResponse();

    QuoteCgiResponse(const QuoteCgiResponse& Qs);

    virtual void on_recv(const std::string& request, std::string& response, const Message* message);

    bool Init(const std::string& brokerUri, const std::string& queueName);

    void UnInit();

    void SetCallback(QuoteReal* quoteReal, KLineManager* klienManager);


private:
    //处理请求任务
    string ProcessRequestTask(shared_ptr<CgiRequestTask> spRequsetTask);

    //处理快照请求任务
    string ProcessQuoteSnapRequestTask(shared_ptr<CgiRequestTask> spRequsetTask);

    //处理K线请求任务
    string ProcessQuoteKLineRequestTask(shared_ptr<CgiRequestTask> spRequsetTask);

    //处理分时线请求任务
    string ProcessQuoteShareLineRequestTask(shared_ptr<CgiRequestTask> spRequsetTask);

    //获取错误信息
    string GetErrorCode(int codeNum, const string& errorMsg);

private:
    //接收请求消息
    activemq_trade_consumer _ativemqConsumer;

    //回复消息
    activemq_trade_producer _ptr_producer;


    //实时行情处理
    QuoteReal* _quoteReal;

    //k线、分时线处理
    KLineManager* _klineManager;
};

#endif