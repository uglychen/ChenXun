
/*
 * ActiveMqConsumer.h
 *
 *  Created on: 2017-4-6
 *      Author: chenxun  275076730@qq.com
 */

#ifndef _ACTIVEMQ_CONSUMER_H_
#define _ACTIVEMQ_CONSUMER_H_

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
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

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

class ActiveMqConsumer : public ExceptionListener,  
                            public MessageListener,  
                            public DefaultTransportListener {  
private:  

    cms::Connection* connection;  
    cms::Session* session;  
    std::vector<Destination*> destinationVec;  
    std::vector<MessageConsumer*> consumerVec;  

    bool useTopic;
    bool clientAck;
    std::string brokerURI;
    std::vector<std::string> destURIVec;

private:

    ActiveMqConsumer(){}
    ActiveMqConsumer( const ActiveMqConsumer& );  
    ActiveMqConsumer& operator= ( const ActiveMqConsumer& );  

public:

    //get a single ActiveMq object
    static ActiveMqConsumer* getInstance();

    //init consumer
    void initConsumer( const std::string& brokerURI_, const std::vector<std::string>& destURI_,
                         bool useTopic_ = false,
                         bool clientAck_ = false )
    {
        this->connection = NULL;
        this->session = NULL;
        this->useTopic = useTopic_;
        this->clientAck = clientAck_;
        this->brokerURI = brokerURI_;
        this->destURIVec = destURI_;
    }  

    virtual ~ActiveMqConsumer() {
        this->cleanup();
    }

    //destory the resource
    void close() {  
        this->cleanup();  
    }  

    //start receive message and process message
    void startReceiveMsgFromMq();

    //mq receive message
    virtual void onMessage( const Message* message );

    //ack the received message
    void acknowledge(void* message);

    virtual void onException( const CMSException& ex AMQCPP_UNUSED );
    virtual void transportInterrupted();
    virtual void transportResumed();
private:  
  
    void cleanup();
};  


#endif


