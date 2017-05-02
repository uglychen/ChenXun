/*
 * ActiveMqProducer.h
 *
 *  Created on: 2017-4-6
 *      Author: chenxun  275076730@qq.com
 */

#ifndef _ACTIVEMQ_PRODUCER_H_
#define _ACTIVEMQ_PRODUCER_H_

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

class ActiveMqProducer : public Runnable 
{
private:

    cms::Connection* connection;
    cms::Session* session;

    bool m_useTopic;          // if use topic mode (or queue)
    bool m_clientAck;
    std::string m_brokerURI;  // ip

    std::vector<Destination*> destinationVec;
    std::map<std::string, MessageProducer*> producerMap; // key:destTopic  value:MessageProducer*

private:
    ActiveMqProducer() { }
    ActiveMqProducer( const ActiveMqProducer& );  
    ActiveMqProducer& operator= ( const ActiveMqProducer& );  

public:

    //get a single ActiveMq object
    static ActiveMqProducer* getInstance();

    //init producer
    void initProducer( const std::string& brokerURI,bool useTopic = false, bool clientAck = false );

    //send message
    void sendMessage(std::string& destTopic, const std::string& message);

    virtual ~ActiveMqProducer()
	{
        cleanup();  
    }

    void close() 
	{
        this->cleanup();
    }

    virtual void run() { }

private:

    void cleanup();

};

#endif


