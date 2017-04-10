#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_

/*
*   Time: 2017.4.7
*   Author: chen
*
*/


#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    ssize_t netReadn(int fd, char *buf, size_t n);
    ssize_t netWriten(int fd, const void *buf, size_t n);
    bool verifyLogin();
    void recvData(std::string& data, int n);

private:
    int fd;
    int protofamily;
    int type;
    int protocol;
    int port;
    std::string ip;
};

#endif
