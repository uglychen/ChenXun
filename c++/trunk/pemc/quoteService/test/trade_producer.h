#ifndef _ACTIVEMQ_TRADE_PRODUCER_H_D024EE2A_7887_4265_89DF_92D55CA1E3AB_
#define _ACTIVEMQ_TRADE_PRODUCER_H_D024EE2A_7887_4265_89DF_92D55CA1E3AB_

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
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
#include <memory>
  
using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

class activemq_trade_producer : public Runnable
	, public cms::ExceptionListener
{
public:
	activemq_trade_producer();
	virtual ~activemq_trade_producer();
	void close();

	bool init(const std::string& broker_url);

	void send_message(const std::string& queueName, const std::string& msg);
	void send_message(const Destination* destination, const std::string& msg, const std::string& id);

	virtual void run() {}
	virtual void onException(const cms::CMSException& ex AMQCPP_UNUSED);
	void cleanup();

private:
	activemq_trade_producer(const activemq_trade_producer&) {}
	activemq_trade_producer& operator= (const activemq_trade_producer&) { return *this; }

private:
	/*activemq*/
	std::unique_ptr<cms::Connection>			_ptr_connection;
	std::unique_ptr<cms::Session>				_ptr_session;
};

#endif

