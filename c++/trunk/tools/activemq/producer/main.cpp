/* #include <iostream>
#include "ActiveMqProducer.h"

void test_send()
{
    std::string topic = "zjz.test";
    string str =  "this is a test";
    std::cout << "send a message: " << str << std::endl;
    ActiveMqProducer::getInstance()->sendMessage(topic, str);
} */

// 为防止引用该工具编译出错，注释掉主函数
/* int main(int argc , char* argv[])
{
    std::string brokerURI = "failover://(tcp://101.37.33.121:61616)";
    bool useTopic = false;
    bool clientAck = false;
    std::string topic = "zjz.test";
    
    activemq::library::ActiveMQCPP::initializeLibrary();
    ActiveMqProducer::getInstance()->initProducer(brokerURI, useTopic, clientAck);

    string str;
    while (1)
    {
       test_send(); 
    }
    
    
    while(getline(std::cin, str))
    {
        ActiveMqProducer::getInstance()->sendMessage(topic, str);
    }
	
	ActiveMqProducer::getInstance()->close();
    activemq::library::ActiveMQCPP::shutdownLibrary();

    return 0;
} */
