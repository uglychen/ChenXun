#ifndef _ACTIVEMQ_TRADE_CONSUMER_H_46E3E7E7_BD6A_4DE9_B479_73B856C82F3B_
#define _ACTIVEMQ_TRADE_CONSUMER_H_46E3E7E7_BD6A_4DE9_B479_73B856C82F3B_

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

class activemq_trade_consumer_callback
{
public:
	virtual void on_recv(const std::string& request, std::string& response, const Message* message) = 0;
};


class activemq_trade_consumer : public ExceptionListener,  
                            public MessageListener,  
                            public DefaultTransportListener 
{  

public:
	activemq_trade_consumer(activemq_trade_consumer_callback* ptr);
	virtual ~activemq_trade_consumer();
	void close();
	bool init(const std::string& url, const std::string& queue);
	virtual void onMessage(const Message* message);
	void acknowledge(void* message);

	virtual void onException(const CMSException& ex AMQCPP_UNUSED);
	virtual void transportInterrupted();
	virtual void transportResumed();

private:

    activemq_trade_consumer( const activemq_trade_consumer& );  
    activemq_trade_consumer& operator= ( const activemq_trade_consumer& );  

    void cleanup();

private:
	/*activemq*/
	std::unique_ptr<cms::Connection>			_ptr_connection;
	std::unique_ptr<cms::Session>				_ptr_session;
	std::unique_ptr<cms::Queue>				_ptr_recv_queue;
	std::unique_ptr<cms::MessageConsumer>		_ptr_recv_message;
	/*回调*/
	activemq_trade_consumer_callback* _ptr_msg_interface;
};  


#endif


