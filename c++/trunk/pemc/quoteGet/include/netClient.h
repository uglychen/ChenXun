/*
 *  netClient.h
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 *
 */

#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "DBPool.h"
#include "ActiveMqProducer.h"
#include "tool.h"
#include "Common.h"

#define NET_OK 0;
#define NET_FALSE -1;

class NetClient
{
public:
    NetClient(){}
    ~NetClient(){ close(fd); }

public:
    void initClient(std::string server_ip, int server_port);

    int netSocket();

    int netConnect();

    ssize_t netReadn(int fd, char *buf, size_t n );

    ssize_t netWriten(int fd, const void *buf, size_t n);

    bool verifyLogin();

    void recvData(std::string& data);

    std::string parseJsonAddSeq(std::string& quote_json);

    void startSendMsg();

public:
    static std::string getUuid();

    static std::string parseJson(const string& quote);
    
    static void executedb(const string& str);

    static void* save_data_thread(void *arg);

    void start_save_thread();

private:
    int fd;
    int protofamily;
    int type;
    int protocol;
    int port;
    std::string ip;

    bool login_flag;//是否登陆
    //std::string timestamp;//时间戳
    bool first_time;

    std::string product_symbol;
    std::map<std::string, int> time_map;
    std::map<std::string, int> seq_map;

private:
    static sql::Connection *con;
    static sql::Statement *state;
    static sql::ResultSet *result;
};

#endif
