#include "trade_producer.h"
#include "trade_consumer.h"
#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;

class MyTest :
	public activemq_trade_consumer_callback
{
public:

	MyTest() {}
	~MyTest() {}

	virtual void on_recv(const std::string& request, std::string& response, const Message* message)
	{
		cout << "回复结果：" << endl;
		cout << request << endl;
	}


	void SendTest(int h)
	{
		if (m_producer != NULL)
		{
			string msg;

			switch (h)
			{
			case '1':
				msg = "{\"Type\":\"pemc.mid.quote.snapshot\"}";
				break;
			case '2':
				msg = "{\"Type\":\"pemc.mid.quote.kLine\",\"Param\":{\"productId\":\"GDAG\",\"kLineType\":\"5min\",\"startTime\":\"1436864169\"}}";
				break;
			case '3':
				msg = "{\"Type\":\"pemc.mid.quote.shareLine\",\"Param\":{\"productId\":\"GDAG\",\"shareType\":\"1min\"}}";
				break;
			default:
				break;
			}

			m_producer->send_message(topicName, msg);
			cout << "发送请求： " << msg << endl;
		}
	}

	void Test()
	{

		while (1)
		{
			cout << "输入1、2、3 进行测试" << endl;
			int h = std::cin.get();
			switch (h)
			{
			case '1':
			case '2':
			case '3':
				SendTest(h);
				continue;
			case 'q':
				break;

			default:
				sleep(1);
			}
		}
	}

public:
	activemq_trade_producer* m_producer;
	string topicName;
};


int main()
{
	std::string brokerURI = "failover://(tcp://101.37.33.121:61616)";
	bool useTopic = false;
	bool clientAck = false;
	std::string sendTopic = "Quote.Cgi.Request";

	activemq::library::ActiveMQCPP::initializeLibrary();

	activemq_trade_producer producer;
	producer.init(brokerURI);

	MyTest myTest;
	myTest.m_producer = &producer;
	myTest.topicName = sendTopic;

	activemq_trade_consumer consumer(&myTest);
	consumer.init(brokerURI, "Quote.Cgi.Reponse_test");


	myTest.Test();


	activemq::library::ActiveMQCPP::shutdownLibrary();

}