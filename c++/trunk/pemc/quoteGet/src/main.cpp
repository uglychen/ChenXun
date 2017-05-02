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
#include <list>
#include <signal.h>

#include "netClient.h"
#include "mqttClient.h"

using namespace std;

void signal_handler()
{
    std::cout << "get signal" << std::endl;
}

int main()
{
    signal(SIGFPE, signal_handler);
    
    init_quote();

    //connect socket server
    NetClient* ClientHandler = new NetClient();
    ClientHandler->initClient(server_ip, server_prot);
    ClientHandler->netSocket();
    ClientHandler->netConnect();
    ClientHandler->verifyLogin();
    
    //start save db pthread
    //ClientHandler->start_save_thread();

    //start revc data and send msg to topic
    ClientHandler->startSendMsg();

    //destory active mq
    ActiveMqProducer::getInstance()->close();
    activemq::library::ActiveMQCPP::shutdownLibrary();

    //destory zlog
    zlog_fini();

    //delete 
    delete ClientHandler;

    return 0;
}

