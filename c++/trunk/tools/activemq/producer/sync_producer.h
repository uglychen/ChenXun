#ifndef _SYNC_ACTIVEMQ_PRODUCER_H_1B17AD24_FAFB_4712_A57D_03E2B18E0EF8_
#define _SYNC_ACTIVEMQ_PRODUCER_H_1B17AD24_FAFB_4712_A57D_03E2B18E0EF8_

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
#include <uuid/uuid.h>
  
using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;

class activemq_sync_producer : public Runnable
	, public cms::ExceptionListener
{
public:
	activemq_sync_producer();
	virtual ~activemq_sync_producer();
	void close();

	bool init(const std::string& broker_url, const std::string& request_queue, const std::string& response_queue);
	bool send_message(const std::string& request, std::string& response, int time = 1000);

	virtual void run() {}
	virtual void onException(const cms::CMSException& ex AMQCPP_UNUSED);
	void cleanup();

private:
	activemq_sync_producer( const activemq_sync_producer& ) {}
	activemq_sync_producer& operator= (const activemq_sync_producer&) { return *this; }

private:
	/*activemq*/
	std::auto_ptr<cms::Connection>			_ptr_connection;
	std::auto_ptr<cms::Session>				_ptr_session;
	std::auto_ptr<cms::Queue>				_ptr_send_queue;
	std::auto_ptr<cms::Queue>		_ptr_recv_queue;
	std::auto_ptr<cms::MessageProducer>		_ptr_send_message;
	std::auto_ptr<cms::MessageConsumer>		_ptr_recv_message;
	/*uuid*/
	uuid_t _uuid;
};



#endif

