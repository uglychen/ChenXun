
#include <iostream>
#include "netClient.h"

using namespace std;

int main()
{
    int port = 10088;
    int port2 = 5188;
    std::string ip = "183.62.250.18";
    std::string ip2 = "172.0.0.1";

    NetClient* ClientHandler = new NetClient();
    ClientHandler->initClient(ip, port);
    ClientHandler->netSocket();
    ClientHandler->netConnect();

    bool log_flag = ClientHandler->verifyLogin();

    std::string str="";
    while(log_flag)
    {
        ClientHandler->recvData( str, 100);
        str="";
    }


    delete ClientHandler;
    return 0;


}

