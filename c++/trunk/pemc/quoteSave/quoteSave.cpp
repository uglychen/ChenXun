/*
*   time:2017.4.13
*/
#include <iostream>
#include "quoteConsumer.h"

using namespace std;

int main() 
{
    cout<<"main begin!"<<endl;

    //init zlog
    zlogInit(log_config_file_path);

    //init db db pool
    connpool.initPool("tcp://116.62.176.131:3306/calf_middle_gg", "user1", "123456", 100);

    activemq::library::ActiveMQCPP::initializeLibrary();

    std::string brokerURI = "failover://(tcp://101.37.33.121:61616)";
    bool useTopic = true;
    bool clientAck = false;
    std::vector<std::string> destURIVec;
    destURIVec.push_back("QUOTE.ORG");

    ActiveMqConsumer* consumer = ActiveMqConsumer::getInstance();
    consumer->initConsumer(brokerURI, destURIVec, useTopic, clientAck);
    ActiveMqProducer* producer = ActiveMqProducer::getInstance();
    producer->initProducer(brokerURI, useTopic, clientAck);

    quoteConsumer quoteConsumer;
    consumer->set_notify(&quoteConsumer);
    consumer->startReceiveMsgFromMq();

    while (std::cin.get() != 'q') {}

    consumer->close();
    delete consumer;
    activemq::library::ActiveMQCPP::shutdownLibrary();
    zlog_fini();
    return 0;
}