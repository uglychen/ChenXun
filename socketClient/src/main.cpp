/*
 *  main.cpp
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 *
 */


#include <iostream>
#include <string>
#include <fstream>

#include "netClient.h"
#include "mqttClient.h"
#include "ActiveMqProducer.h"
#include "tool.h"

using namespace std;

int main()
{
    int port = 10088;
    int port2 = 5188;
    std::string ip = "183.62.250.18";

    NetClient* ClientHandler = new NetClient();
    ClientHandler->initClient(ip, port);
    ClientHandler->netSocket();
    ClientHandler->netConnect();

    //init mqtt client
    mqtt::async_client client(ADDRESS, CLIENTID);
    callback cb;
    client.set_callback(cb);
    mqtt::itoken_ptr conntok = client.connect();

    try{
        std::cout << "the mqtt waiting for the connection..." << std::flush;
        conntok->wait_for_completion();
        std::cout << "OK" << std::endl;
    }catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
    
    //test mqtt
    std::string test_message = "this is a test meesage...";
    mqtt::message_ptr pubmsg = std::make_shared<mqtt::message>(test_message);
    pubmsg->set_qos(QOS);
    client.publish(TOPIC, pubmsg)->wait_for_completion(TIMEOUT);

    //init active mq
    std::string brokerURI = "failover:(tcp://101.37.33.121:61616)";
    bool useTopic = true;
    bool clientAck = false;
    std::string topic = "QUOTE.ORG";
 
    activemq::library::ActiveMQCPP::initializeLibrary();
    ActiveMqProducer::getInstance()->initProducer(brokerURI, useTopic, clientAck);

    //init write file
    std::string date = getDate();
    std::string filename = date +".log";
    std::ofstream out_file(filename, ios::app);

    //start recv json data 
    bool log_flag = ClientHandler->verifyLogin();
    std::string str="";
    while( log_flag )
    {
        ClientHandler->recvData( str );

        if(!str.empty())
        {
            std::string tmep_send = str.substr(0, str.length()-1);
            ActiveMqProducer::getInstance()->sendMessage(topic, tmep_send);
            //write the message to another file
            out_file << getDateAndTime() << "[The received data:]" << tmep_send << std::endl << std::endl;

            str="";
        }
    }

    std::vector<mqtt::idelivery_token_ptr> toks = client.get_pending_delivery_tokens();
    if (!toks.empty())
    {
        std::cout << "Error: There are pending delivery tokens!" << std::endl;
    }
    
    // mqtt disconnect
    std::cout << "Disconnecting..." << std::flush;
    conntok = client.disconnect();
    conntok->wait_for_completion();
    std::cout << "OK" << std::endl;


    //destory active mq
    ActiveMqProducer::getInstance()->close();
    activemq::library::ActiveMQCPP::shutdownLibrary();

    //delete 
    delete ClientHandler;

    return 0;
}

