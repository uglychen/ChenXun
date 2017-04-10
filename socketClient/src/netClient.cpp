
#include <iostream>
#include "netClient.h"


void NetClient::initClient(std::string server_ip, int server_port)
{
    this->protofamily = AF_INET;
    this->type = SOCK_STREAM;
    this->protocol = 0;
    this->port = server_port;
    this->ip = server_ip.c_str();
    
    std::cout << "**server ip:" << ip << std::endl;
    std::cout << "**server port:" << port << std::endl;

}

int NetClient::netSocket()
{
    if( (this->fd = socket(AF_INET , SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        return NET_FALSE;
    }

    int on = 1;  
    if ( setsockopt(this->fd, SOL_SOCKET,  SO_REUSEADDR, &on, sizeof(on)) < 0 )  
    {
        perror("setsockopt error ");
        return NET_FALSE;
    }
    
    std::cout << "NetClient::netSocket  success!" << std::endl;
    
    return NET_OK;
}

int NetClient::netConnect()
{
    struct sockaddr_in servaddr;  
    memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( 10088 );
    inet_pton( AF_INET, ip.c_str(), &servaddr.sin_addr );
    //std::cout << ip.c_str() << std::endl;
    //const char *tmp = ip.c_str();
    //servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    //servaddr.sin_addr.s_addr = inet_addr("183.62.250.18");

    if( connect(this->fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
    {
        if ( errno != EINPROGRESS )
        {
            std::cout << "connect error:" << strerror(errno) << "errno:" << errno <<std::endl;
            close(fd);
            return NET_FALSE;
        }
        std::cout << "NetClient::netConnect 1111" << std::endl;
    }
    
    std::cout << "NetClient::netConnect success!" << std::endl;    

    return NET_OK;
}

ssize_t NetClient::netReadn(int fd, char *buf, size_t n)
{
    size_t  nleft;
    ssize_t nread;
    char   *ptr;
    
    ptr = buf;
    nleft = n;
    while (nleft > 0) 
    {
        if ( (nread = read(fd, ptr, nleft)) < 0) 
        {
            if ( errno == EINTR )
            {
                nread = 0; /* and call read() again */
            }      
            else 
            {
                perror("NetClient::netReadn failed");
                return NET_FALSE;   /* error */
            }
        } 
        else if (nread == 0) // EOF
        {
            close(fd);
            break;
        }

        nleft -= nread;
        ptr += nread;
    }

    return (n - nleft);         /* return >= 0 */
}

ssize_t NetClient::netWriten(int fd, const void *buf, size_t n)  
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (const char*)buf;
    nleft = n;
    while (nleft > 0) 
    {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) 
        {
            if (nwritten < 0 && errno == EINTR)
            {   
                nwritten = 0;   /* and call write() again */
            }
            else
            {
                perror("NetClient::netWriten failed");
                return NET_FALSE; /* error */
            }
         }

         nleft -= nwritten;
         ptr += nwritten;
    }
    return (n);
} 

bool NetClient::verifyLogin()
{
    char login_buffer[19] = {0}; 
    memset(login_buffer, 0xff, 1); 
    memcpy(login_buffer + 1, "0204", 4);
    memcpy(login_buffer + 5, "0007", 4);
    memcpy(login_buffer + 9, "02", 2);
    memcpy(login_buffer + 11, "PMEC180", 7);
    memset(login_buffer + 19, 0x00, 1);

    std::cout << "----login_buffer:----" << login_buffer << " size:" << sizeof(login_buffer) << std::endl;

    size_t writeCount = netWriten(fd, login_buffer, sizeof(login_buffer));

    if( writeCount < sizeof(login_buffer) )
    {
        std::cout << "NetClient::verifyLogin failed" << std::endl;
    }
   
    char recv_login[12] ={ 0 };
    //recv(fd, buffer, sizeof(buffer), 0);
    netReadn(fd, recv_login, sizeof(recv_login));  // 这儿从fd中拿出12字节的
    std::cout << " recv_login data:" << recv_login << std::endl;       
 
    char login_cmd[5]={ 0 };
    memcpy(login_cmd, recv_login + 1, 4);
    std::cout << "---login_cmd:---" << login_cmd << std::endl;
    
    if( strcmp(login_cmd,"0200") == 0 )
    {
        std::cout << "NetClient::verifyLogin success!" << std::endl;
        return true;
    }else{
        std::cout << "NetClient::verifyLogin failed!" << std::endl;
        return false;
    } 
}

void  NetClient::recvData(std::string& data, int n)
{
    char head_buffer[12] = { 0 };
    netReadn(fd, head_buffer, 11);
    std::cout << "The reev head  data:" << head_buffer << std::endl;    
    
    char len[5]={ 0 };
    memcpy(len, head_buffer+5, 4);
    int message_len = atoi(len);
    std::cout << "the message length: "  << message_len << std::endl;
   
    char message_body[2480] =  { 0 }; 
    netReadn(fd, message_body,  message_len);
    //std::cout << "message body: " << message_body << std::endl;    
    data = message_body;
    std::cout << "message body: " << data  << std::endl;    

    return;
}


