
#include <iostream>
#include "ActiveMqProducer.h"

void test_send()
{
    std::string topic = "test.chenxun";
    string str =  "this is a test";
    std::cout << "send a message: " << str << std::endl;
    ActiveMqProducer::getInstance()->sendMessage(topic, str);
}

int main(int argc , char* argv[])
{
    std::string brokerURI = "failover:(tcp://localhost:61616)";
    bool useTopic = false;
    bool clientAck = false;
    std::string topic = "test.chen";
    
    activemq::library::ActiveMQCPP::initializeLibrary();
    ActiveMqProducer::getInstance()->initProducer(brokerURI, useTopic, clientAck);

    string str;

    test_send();
    
    while(getline(std::cin, str))
    {
        ActiveMqProducer::getInstance()->sendMessage(topic, str);
    }
	
	ActiveMqProducer::getInstance()->close();
    activemq::library::ActiveMQCPP::shutdownLibrary();

    return 0;
}
