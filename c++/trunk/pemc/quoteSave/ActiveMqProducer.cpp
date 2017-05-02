/*
 *  ActiveMqProducer.cpp
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 */

#include "ActiveMqProducer.h"
#include "quoteConsumer.h"

//singleton 
ActiveMqProducer* ActiveMqProducer::getInstance()
{
    static ActiveMqProducer instance_;
    return &instance_;
}

//init producer   
void ActiveMqProducer::initProducer(const std::string& brokerURI, bool useTopic, bool clientAck)
{
    this->m_useTopic= useTopic;
    this->m_clientAck= clientAck;

    zlog_info(zc, "activemq connect ip: %s", brokerURI.c_str());
    zlog_info(zc, "activemq send message use topic or not: %d", useTopic);

    try {
        auto_ptr<ActiveMQConnectionFactory> connectionFactory(new ActiveMQConnectionFactory(brokerURI) );

        try{
            connection = connectionFactory->createConnection();
            connection->start();
        }catch( CMSException& e ){
            e.printStackTrace();
            throw e;
        }  
  
        if( clientAck )
        {  
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        }
        else
        {  
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }
    }catch(CMSException& e){
        e.printStackTrace();
    }
}

//send message
void ActiveMqProducer::sendMessage(std::string& destTopic, const std::string& message)
{
    if(producerMap.find(destTopic) == producerMap.end())
    {
        zlog_info(zc, "ActiveMqProducer::sendMessage create a new topic");

        Destination* destination = NULL;

        if( m_useTopic )   
        {  
            destination = session->createTopic( destTopic );
        }
        else
        {
            destination = session->createQueue( destTopic );
        }

        destinationVec.push_back(destination);

        MessageProducer* producer = session->createProducer( destination );
        producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );
        producerMap[destTopic] = producer;
    }

    //std::string send_message = destTopic + ":" + message;
    std::string send_message = message;

    std::string threadIdStr = Long::toString( Thread::currentThread()->getId() );
    TextMessage* text = session->createTextMessage( send_message );

    zlog_info(zc, "send a message: %s", send_message.c_str());
    //std::cout << "Send message from thread: " << threadIdStr << std::endl;
    //std::cout << "send a message: " << send_message << std::endl; // need modify (log file)
    producerMap[destTopic]->send( text );
    delete text;
}

void ActiveMqProducer::cleanup()
{
    try
    {
        std::map<std::string, MessageProducer*>::iterator iter;
        for(iter=producerMap.begin(); iter!=producerMap.end(); ++iter)
        {
            if(iter->second != NULL)
            {
                delete iter->second;
            }
            iter->second = NULL;
        }
        producerMap.clear();
    }catch( CMSException& e ){
        e.printStackTrace();
    }

    try
    {
        for(size_t i = 0; i<destinationVec.size(); ++i)
        {
            if(destinationVec[i] != NULL) 
            {
                delete destinationVec[i];
            }
            destinationVec[i] = NULL;
        }
        destinationVec.clear();
    }catch( CMSException& e ){
        e.printStackTrace();
    }

    try{
        if( session != NULL) session->close();
        if( connection != NULL) connection->close();
    }catch( CMSException& e){ 
        e.printStackTrace(); 
    }

    try{
        if( session != NULL ) 
        {
            delete session;
        }
        session = NULL;
    }catch( CMSException& e){
        e.printStackTrace();
    }

    try{
        if( connection != NULL ) 
        {
            delete connection;
        }
        connection = NULL;
    }catch( CMSException& e){
        e.printStackTrace();
    }
}


