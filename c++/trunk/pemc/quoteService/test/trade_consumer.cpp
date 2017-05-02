#include <iostream>
#include "trade_consumer.h"

activemq_trade_consumer::activemq_trade_consumer(activemq_trade_consumer_callback* ptr)
	:_ptr_msg_interface(ptr)
{

}

activemq_trade_consumer::~activemq_trade_consumer()
{
	cleanup();
}

bool activemq_trade_consumer::init(const std::string& url, const std::string& queue)
{
    try 
	{
		std::unique_ptr<cms::ConnectionFactory> ptr_connectionFactory(cms::ConnectionFactory::createCMSConnectionFactory(url));
		ptr_connectionFactory->setExceptionListener(this);

		_ptr_connection = std::unique_ptr<cms::Connection>(ptr_connectionFactory->createConnection(/*可以添加用户名和密码*/));
		_ptr_connection->setExceptionListener(this);
		_ptr_connection->start();

		_ptr_session = std::unique_ptr<cms::Session>(_ptr_connection->createSession(/*Session::CLIENT_ACKNOWLEDGE*/));

		_ptr_recv_queue = std::unique_ptr<cms::Queue>(_ptr_session->createQueue(queue));

		_ptr_recv_message = std::unique_ptr<cms::MessageConsumer>(_ptr_session->createConsumer(_ptr_recv_queue.get()));
		_ptr_recv_message->setMessageListener(this);

		return true;
    }
    catch (CMSException& e)
    {
        e.printStackTrace();
		return false;
    } 
}

void activemq_trade_consumer::close()
{
	this->cleanup();
}

void activemq_trade_consumer::onMessage( const Message* message )
{
    try
	{
        const TextMessage* textMessage =  dynamic_cast< const TextMessage* >( message );
        if( textMessage != NULL ) 
		{
			string  text = textMessage->getText();
			if(NULL != _ptr_msg_interface)
			{
				std::string response;
				_ptr_msg_interface->on_recv(text, response, message);
			}
        } 
    } 
	catch (CMSException& e) 
	{
        e.printStackTrace();
    }
}

//after process the message we ack the message
void activemq_trade_consumer::acknowledge(void* message)
{
    try{
        Message* clientAckMsg = (Message*)message;
        clientAckMsg->acknowledge();
        delete clientAckMsg;
        
    }catch( CMSException& e){
        e.printStackTrace();
    }
}

void activemq_trade_consumer::onException( const CMSException& ex AMQCPP_UNUSED )
{
    std::cout << "CMS Exception occurred.  Shutting down client." << std::endl;
    ex.printStackTrace();
    exit(1);
}

void activemq_trade_consumer::transportInterrupted()
{
    //std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void activemq_trade_consumer::transportResumed()
{
    //std::cout << "The Connection's Transport has been Restored." << std::endl;
}

//clean resource
void activemq_trade_consumer::cleanup()
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

