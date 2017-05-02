/*
 * ActiveMqConsumer.cpp
 *
 *  Created on: 2017-4-6
 *      Author: chenxun  275076730@qq.com
 */

#include <iostream>
#include "consumer.h"

//start get message and process message
void ActiveMqConsumer::startReceiveMsgFromMq( )
{
    try {
        ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory( brokerURI );
        connection = connectionFactory->createConnection();
        delete connectionFactory;

        ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
        if( amqConnection != NULL )
        {
            amqConnection->addTransportListener( this );
            amqConnection->setExclusiveConsumer( true );
        }
		
        //start the asyn process and listen message
        connection->start();
        connection->setExceptionListener(this);

        if( clientAck ) {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        } else {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }

        for(size_t i = 0; i < destURIVec.size(); ++i)
        {
            Destination *destination = NULL;
            if( useTopic ) {
                destination = session->createTopic( destURIVec[i] );
            } else {
                destination = session->createQueue( destURIVec[i] );
            }

            destinationVec.push_back(destination);

            MessageConsumer* consumer = session->createConsumer( destination );
            consumer->setMessageListener( this );
            consumerVec.push_back( consumer );
        }
    }
    catch (CMSException& e)
    {
        e.printStackTrace();
    } 
}

void ActiveMqConsumer::onMessage( const Message* message )
{
    try{
        const TextMessage* textMessage =  dynamic_cast< const TextMessage* >( message );
        string text = "";

        if( textMessage != NULL ) 
		{
            text = textMessage->getText();
			if(NULL != _ptr_msg_interface)
			{
				std::string response;
				_ptr_msg_interface->on_recv(text, response, message);
			}
        } 
		//else 
		//{
           // text = "NOT A TEXTMESSAGE!";
        //}

        if( clientAck ) 
		{
            message->acknowledge();
        }

        //std::cout << "Message Received:" << text << std::endl;
        /*
            at here we can put the message into list that the globale variable
        */

    	}
		catch (CMSException& e) 
		{
        	e.printStackTrace();
    	}
}

//after process the message we ack the message
void ActiveMqConsumer::acknowledge(void* message)
{
    try{
        Message* clientAckMsg = (Message*)message;
        if( clientAck != false)
        {
            clientAckMsg->acknowledge();
            delete clientAckMsg;
        }
    }catch( CMSException& e){
        e.printStackTrace();
    }
}

void ActiveMqConsumer::onException( const CMSException& ex AMQCPP_UNUSED )
{
    std::cout << "CMS Exception occurred.  Shutting down client." << std::endl;
    ex.printStackTrace();
    exit(1);
}

void ActiveMqConsumer::transportInterrupted()
{
    std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void ActiveMqConsumer::transportResumed()
{
    std::cout << "The Connection's Transport has been Restored." << std::endl;
}

//clean resource
void ActiveMqConsumer::cleanup()
{
    try{
        for(size_t i = 0; i < consumerVec.size(); ++i)
        {
            if( consumerVec[i] != NULL)
            {
                delete consumerVec[i];
            }
            consumerVec[i] = NULL;
        }
    }catch(CMSException& e){
        e.printStackTrace();
    }
    consumerVec.clear();

    try{
        for(size_t i = 0; i < destinationVec.size(); ++i)
        {
            if( destinationVec[i] != NULL)
            {
                delete destinationVec[i];
            }
            destinationVec[i] = NULL;
        }
    }catch(CMSException& e){
        e.printStackTrace();
    }
    destinationVec.clear();

    try{
        if( session != NULL ) session->close();  
        if( connection != NULL ) connection->close();  
    }catch (CMSException& e) {
        e.printStackTrace();
    }

    try{
        if( session != NULL ) delete session;  
    }catch (CMSException& e) {
        e.printStackTrace();
    }
    session = NULL;

    try{
        if( connection != NULL ) delete connection;
    }catch (CMSException& e) {
        e.printStackTrace();
    }
    connection = NULL;  
}

