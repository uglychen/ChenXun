#include "sync_producer.h"


activemq_sync_producer::activemq_sync_producer()
{

}

activemq_sync_producer::~activemq_sync_producer()
{
	cleanup();
}

void activemq_sync_producer::close()
{
	cleanup();
}

bool activemq_sync_producer::init(const std::string& broker_url, const std::string& request_queue, const std::string& response_queue)
{
	try
	{
		std::auto_ptr<cms::ConnectionFactory> ptr_connectionFactory(cms::ConnectionFactory::createCMSConnectionFactory(broker_url));
		ptr_connectionFactory->setExceptionListener(this);

		_ptr_connection = std::auto_ptr<cms::Connection>(ptr_connectionFactory->createConnection(/*可以添加用户名和密码*/));
		_ptr_connection->setExceptionListener(this);
		_ptr_connection->start();

		_ptr_session = std::auto_ptr<cms::Session>(_ptr_connection->createSession(/*Session::CLIENT_ACKNOWLEDGE*/));

		_ptr_send_queue = std::auto_ptr<cms::Queue>(_ptr_session->createQueue(request_queue));

		_ptr_recv_queue = std::auto_ptr<cms::Queue>(_ptr_session->createQueue(response_queue));
		
		_ptr_send_message = std::auto_ptr<cms::MessageProducer>(_ptr_session->createProducer(_ptr_send_queue.get()));
		_ptr_send_message->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

		_ptr_recv_message = std::auto_ptr<cms::MessageConsumer>(_ptr_session->createConsumer(_ptr_recv_queue.get()));


		return true;
	}
	catch (cms::CMSException& e)
	{
		return false;
	}
}

//send message
bool activemq_sync_producer::send_message(const std::string& request, std::string& response, int time)
{
	try
	{
		char uuid_str[37];
		uuid_str[36] = 0;
		uuid_clear(_uuid);
		uuid_generate_time_safe(_uuid);
		uuid_unparse(_uuid, uuid_str);

		//发送消息
		std::auto_ptr<cms::TextMessage> ptr_message(_ptr_session->createTextMessage(request));
		ptr_message->setCMSReplyTo(_ptr_recv_queue.get());
		ptr_message->setCMSCorrelationID(uuid_str);
		_ptr_send_message->send(ptr_message.get());


		bool recv_data = false;
		while (1)
		{
			std::auto_ptr<cms::TextMessage> recv_message(dynamic_cast<cms::TextMessage*>(_ptr_recv_message->receive(time)));
			if (NULL != recv_message.get())
			{
				if (recv_message->getCMSCorrelationID().compare(uuid_str) == 0)
				{
					response = recv_message->getText();
					recv_data = true;
					break;
				}
				else
				{
					
				}				
			}
			else
			{
				break;
			}	
		}
		
		return recv_data;
	}
	catch (cms::CMSException& e)
	{
		printf("Producer run() CMSException \n");
		e.printStackTrace();

		return false;
	}
}

void activemq_sync_producer::cleanup()
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
void activemq_sync_producer::onException(const cms::CMSException& ex AMQCPP_UNUSED)
{
	//printf("Producer onException() CMS Exception occurred.  Shutting down client. \n");
	ex.printStackTrace();
	exit(1);
}
