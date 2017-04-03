#include <iostream>
#include "ActiveMqConsumer.h"

int main()
{
    activemq::library::ActiveMQCPP::initializeLibrary();

    std::string brokerURI = "failover:(tcp://127.0.0.1:61616)";
    bool useTopic = false;
    bool clientAck = false;
    std::vector<std::string> destURIVec;
    destURIVec.push_back("test.chenxun");
    destURIVec.push_back("test.chen");

    ActiveMqConsumer* consumer = ActiveMqConsumer::getInstance();
    consumer->initConsumer(brokerURI, destURIVec, useTopic, clientAck);
    consumer->startReceiveMsgFromMq();

    while( std::cin.get() != 'q'){}
    consumer->close();    

    activemq::library::ActiveMQCPP::shutdownLibrary();
    return 0;
}
