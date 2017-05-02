/*
 *  netClient.cpp
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 *
 */

#include <iostream>
#include <stdio.h>
#include "netClient.h"
#include "zlog.h"
#include "tool.h"

sql::Connection* NetClient::con = NULL;
sql::Statement* NetClient::state = NULL;
sql::ResultSet* NetClient::result = NULL;

void NetClient::initClient(std::string server_ip, int server_port)
{
    this->protofamily = AF_INET;
    this->type = SOCK_STREAM;
    this->protocol = 0;
    this->port = server_port;
    this->ip = server_ip;

    this->login_flag = 0;

    this->first_time = true;
    this->product_symbol = "";
    this->seq_map["GDAG"] = 0;
    this->seq_map["GDPD"] = 0;
    this->seq_map["GDPT"] = 0;
    this->time_map["GDAG"] = 0;
    this->time_map["GDPD"] = 0;
    this->time_map["GDPT"] = 0;
    
    zlog_info(zc, "NetClient::initClient server ip:%s", ip.c_str());
    zlog_info(zc, "NetClient::initClient server port:%d", port);
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
        zlog_info(zc,"connect error: %s , errno:%d", strerror(errno), errno);
        return NET_FALSE;
    }
    
    zlog_info(zc, "NetClient::netSocket  success!");
    
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
            zlog_info(zc,"connect error: %s , errno:%d", strerror(errno), errno);
            close(fd);
            return NET_FALSE;
        }
        zlog_info(zc,"NetClient::netConnect");
    }
    
    zlog_info(zc,"NetClient::netConnect success!");
   
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
                zlog_info(zc, "NetClient::netReadn failed:%s, errno:%d", strerror(errno), errno);
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
                zlog_info(zc,"NetClient::netWriten failed: %s , errno:%d", strerror(errno), errno);
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
    memcpy(login_buffer + 11, "PMEC180", 7);// passwd: PMEC180
    memset(login_buffer + 19, 0x00, 1);

    zlog_info(zc, "NetClient::verifyLogin login_buffer:%s, size:%d", login_buffer, sizeof(login_buffer));
    
    size_t writeCount = netWriten(fd, login_buffer, sizeof(login_buffer));
    if( writeCount < sizeof(login_buffer) )
    {
        zlog_info(zc,"NetClient::verifyLogin failed");
    }
   
    char recv_login[13] ={ 0 };
    //recv(fd, buffer, sizeof(buffer), 0);
    netReadn(fd, recv_login, sizeof(recv_login)-1);  // 这儿从fd中拿出12字节的
    zlog_info(zc,"NetClient::verifyLogin recv_login data: %s",recv_login);    
 
    char login_cmd[5]={ 0 };
    memcpy(login_cmd, recv_login + 1, 4);
    zlog_info(zc,"NetClient::verifyLogin login_cmd:%s",login_cmd);
    
    if( strcmp(login_cmd, "0200") == 0 )
    {
        zlog_info(zc,"NetClient::verifyLogin success!");
        login_flag = true;
        return true;
    }else{
        zlog_info(zc,"NetClient::verifyLogin failed!");
        login_flag = false;
        return false;
    } 
}

void  NetClient::recvData( std::string& data )
{
    char head_buffer[12] = { 0 };
    netReadn(fd, head_buffer, 11);
    zlog_info(zc, "----------------------------------------------");
    zlog_info(zc, "NetClient::recvData head: %s", head_buffer); 
    
    char commd[5] = { 0 };
    char m_len[5]={ 0 };
    memcpy(commd, head_buffer+1, 4);
    memcpy(m_len, head_buffer+5, 4);
    int message_len = atoi(m_len);
    zlog_info(zc, "the message length: %d", message_len); 

    if( strcmp(commd, "0201") == 0)
    {
        char message_body[2480] =  { 0 }; 
        netReadn(fd, message_body,  message_len+1);
        //std::cout << "message body: " << message_body << std::endl;
        std::string tmp_str =  message_body;  
        data = tmp_str.substr(0, tmp_str.length()-1);
        zlog_info(zc, "message body: %s", data.c_str());
    }
    else if( strcmp(commd, "0203") == 0)//心跳包处理
    {
        char a[2] = { 0 };
        //无行情的时候头部是12字节的，先接受的了11，这里追加消费1字节的消息
        netReadn(fd, head_buffer, 1);
        zlog_info(zc, "NetClient::recvData heartbeat");
    }
    else
    {
        zlog_info(zc, "NetClient::recvData error");
    }   

    return;
}

void NetClient::start_save_thread()
{
    //start save date p_thread
    pthread_t tid;
    int err = pthread_create(&tid, NULL, save_data_thread, 0);
    if (err != 0)
    {
        zlog_info(zc,"Create new thread failed: %s\n", strerror(err));
        exit(1);
    }
}

//thread fuction
void* NetClient::save_data_thread(void *arg)
{
    while( true )
    {
        std::string str = "";
        g_list.pop(str);
        if( !str.empty() )
        {
            zlog_info(zc,"str size():%d",str.length());
            zlog_info(zc,"revc json data:%s",str.c_str());
            std::string sqlstate = parseJson(str);
            executedb(sqlstate);
        }
        else
        {
            zlog_info(zc, "NetClient::save_data_thread recv no data");
        }
    }
}

std::string NetClient::parseJson(const string& quote)
{
    if (quote.empty())
    {
        return "";
    }
    
    //json parse
    string id = getUuid();
    string symbol;
    string name = "粤贵银";
    int timestamp = 0;
    string currency;
    int type = 0;
    string exchange;
    string market;
    int bit;
    string open;  //开盘价
    string close; //昨收价
    string ask;   //买入价
    string bid;   //卖出价
    string last;
    string high;
    string low;
    int tradedate; //交易日
    string createman;
    int modifytime;

    char buffer[4096] = {0};

    zlog_info(zc, "******begin parse json data******");
    try 
    {
        Json::Reader reader;
        Json::Value value;
        if (!reader.parse(quote, value))
        {
            zlog_info(zc, "****parse json data failed****");
            return "";
        }

        type = value["TYPE"].asInt();
        exchange = value["EXCHANGE"].asString();
        market = value["MARKET"].asString();
        symbol = value["SYMBOL"].asString();
        currency = value["CURRENCY"].asString();
        bit = value["BIT"].asInt();
        open = value["OPEN"].asString();
        close = value["PRECLOSE"].asString();

        ask = value["ASK"].asString();
        bid = value["BID"].asString();
        last = value["LAST"].asString();
        high = value["HIGH"].asString();
        low = value["LOW"].asString();
        timestamp = value["TIMESTAMP"].asInt();
        tradedate = value["TRADEDATE"].asInt();
    }
    catch (std::exception &ex)
    {
        zlog_info(zc, "json exception: %s", ex.what());
        return "";
    }
 
    zlog_info(zc, "*****parse json data end******");

    string tableName;
    if (symbol == "GDAG")
    {
        name = "粤贵银";
        tableName = "gg_market_silver_his";
    }
    else if (symbol == "GDPD")
    {
        name = "粤贵钯";
        tableName = "gg_market_palladium_his";
    }
    else if (symbol == "GDPT")
    {
        name = "粤贵铂";
        tableName = "gg_market_platinum_his";
    }
    else
    {
        cout << "can't find symbol and talbes" << endl;
    }

    char temp[1024] = { 0 };
    sprintf(temp,"FROM_UNIXTIME(%d)", timestamp);
    string s_timestamp(temp);

    memset(temp, 0, 1024);
    sprintf(temp, "FROM_UNIXTIME(%d)", tradedate);
    string s_tradedate(temp);

    time_t now;
    time(&now);
    memset(temp, 0, 1024);
    sprintf(temp, "FROM_UNIXTIME(%d)", now);
    string s_cratetime(temp);

    memset(temp, 0, 1024);
    sprintf(temp, "%d", type);
    string s_type(temp);

    memset(temp, 0, 1024);
    sprintf(temp, "%d", bit);
    string s_bit(temp);

    sprintf(buffer, "INSERT INTO %s VALUES(\"%s\", \"%s\", \"%s\", %s, \"%s\", %d, \"%s\", \"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %s, %s, NULL, NULL)",
        tableName.c_str(),
        id.c_str(),
        symbol.c_str(),
        name.c_str(),
        s_timestamp.c_str(),
        currency.c_str(),
        type,
        exchange.c_str(),
        market.c_str(),
        bit,
        open.c_str(),
        close.c_str(),
        ask.c_str(),
        bid.c_str(),
        last.c_str(),
        high.c_str(),
        low.c_str(),
        s_tradedate.c_str(),
        s_cratetime.c_str()
        );

    string sqlstatement(buffer);
    return sqlstatement;
}

// execut sql
void NetClient::executedb(const string& str)
{
    if (str.empty())
    {
        return;
    }

    try{
        con = connpool.GetConnection();
        state = con->createStatement();
        state->execute(str);
        connpool.ReleaseConnection(con);
        zlog_info(zc, "save data:%s", str.c_str());
        zlog_info(zc, "----------------------------------------------");
        delete state;
        state = NULL;
    }catch(sql::SQLException &e){
        std::cout << e.what() <<std::endl;
    }
}

//get the uuid 
std::string NetClient::getUuid()
{
    char cc[33] = {0};
    uuid_t uu;
    uuid_generate(uu);

    for (int i = 0; i<16; i++)
    {
        sprintf(&cc[i*2],"%02x", uu[i]);
    }

    return string(cc);
}

std::string NetClient::parseJsonAddSeq(std::string& quote_json)
{
    Json::Reader reader;
    Json::Value root;
    Json::FastWriter writer;

    if (!reader.parse(quote_json, root))
    {
        zlog_info(zc, "NetClient::parseJsonAddSeq parse json data Failed");
        return "";
    }

    product_symbol = root["SYMBOL"].asString(); //获取产品名称作为map的key
    int tmp_time = root["TIMESTAMP"].asInt();
    if( tmp_time == time_map[product_symbol] )
    {
        seq_map[product_symbol] = seq_map[product_symbol] + 1;
        root["SEQUENCE"] = seq_map[product_symbol] + 1;
    }
    else
    {
        root["SEQUENCE"] = 1;
        seq_map[product_symbol] = 0;
    }
    time_map[product_symbol] = root["TIMESTAMP"].asInt();

    std::string json_str = writer.write(root);
    //std::cout << "json_str:" <<json_str <<std::endl;

    std::string ret_str = json_str.substr(0, json_str.length()-1);
    //std::cout << "ret_str:" <<ret_str <<std::endl;

    return ret_str;
}

void NetClient::startSendMsg()
{
    std::string str="";

    while( login_flag )
    {
        recvData( str );//get data from socket server
        if( !str.empty() )
        {
            std::string tmp = parseJsonAddSeq(str);
            //std::cout << tmp <<std::endl;
            ActiveMqProducer::getInstance()->sendMessage(topic, tmp);
            //g_list.push(str);
            saveFile(tmp);
            str="";
        }
    }
}