#include "QuoteConsumer.h"
#include "QuoteReal.h"
#include "KLineManager.h"
#include "QuoteCgiResponseManager.h"
#include "DBPool.h"
#include "Product_def.h"
#include "QuoteUtils.h"
 
int main() 
{
    if (!MyLog::Instance().Init())
    {
        return 0;
    }
	
	connpool = DBPool::GetInstance();
	//初始化连接，创建参数中maxSize一半的连接  
	connpool.initPool(DBHOST, USER, PASSWORD, 100);
    dzlog_info("数据库连接池数据化！");

	///////////////////////////////////////////
	activemq::library::ActiveMQCPP::initializeLibrary();
    dzlog_info("初始化ActiveMQCPP库");

    //实时行情处理类
	QuoteReal quoteReal;
	quoteReal.init();

    //k线分时线管理类
	KLineManager klineManager;
    klineManager.Init();

    //原始行情接收类
	QuoteConsumer quoteConsumer;
	quoteConsumer.SetCallback(&quoteReal, &klineManager);
	quoteConsumer.init(BROKER_URI);
    quoteConsumer.StartRecvMsg();

    //cgi请求管理类
	QuoteCgiResponseManager quoteCgiResponseManager;
	quoteCgiResponseManager.SetCallback(&quoteReal, &klineManager);
	quoteCgiResponseManager.Init();

	while (std::cin.get() != 'q') {}
	quoteConsumer.uninit();

	activemq::library::ActiveMQCPP::shutdownLibrary();
	///////////////////////
    dzlog_info("main end!");
	return 0;
}