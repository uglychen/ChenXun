#include "trade_producer.h"

activemq_trade_producer::activemq_trade_producer()
{

}

activemq_trade_producer::~activemq_trade_producer()
{
	cleanup();
}

void activemq_trade_producer::close()
{
	cleanup();
}

bool activemq_trade_producer::init(const std::string& broker_url)
{
	try
	{
		std::auto_ptr<cms::ConnectionFactory> ptr_connectionFactory(cms::ConnectionFactory::createCMSConnectionFactory(broker_url));
		ptr_connectionFactory->setExceptionListener(this);

		_ptr_connection = std::auto_ptr<cms::Connection>(ptr_connectionFactory->createConnection(/*可以添加用户名和密码*/));
		_ptr_connection->setExceptionListener(this);
		_ptr_connection->start();

		_ptr_session = std::auto_ptr<cms::Session>(_ptr_connection->createSession(/*Session::CLIENT_ACKNOWLEDGE*/));

		return true;
	}
	catch (cms::CMSException& e)
	{
		return false;
	}
}

//send message
void activemq_trade_producer::send_message(const Destination* destination, const std::string& msg, const std::string& id)
{
	try
	{
		std::auto_ptr<cms::MessageProducer> ptr_send_message = std::auto_ptr<cms::MessageProducer>(_ptr_session->createProducer(destination));
		ptr_send_message->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

		//发送消息
		std::auto_ptr<cms::TextMessage> ptr_message(_ptr_session->createTextMessage(msg));
		ptr_message->setCMSCorrelationID(id);
		ptr_send_message->send(ptr_message.get());
	}
	catch (cms::CMSException& e)
	{
		printf("Producer run() CMSException \n");
		e.printStackTrace();
	}
}

void activemq_trade_producer::cleanup()
{
	try
	{
		if (_ptr_session.get() != NULL) _ptr_session->close();
		if (_ptr_connection.get() != NULL) _ptr_connection->close();
	}
	catch (cms::CMSException& e)
	{
		e.printStackTrace();
	}
}

// If something bad happens you see it here as this class is also been
// registered as an ExceptionListener with the connection.
void activemq_trade_producer::onException(const cms::CMSException& ex AMQCPP_UNUSED)
{
	//printf("Producer onException() CMS Exception occurred.  Shutting down client. \n");
	ex.printStackTrace();
	exit(1);
}
