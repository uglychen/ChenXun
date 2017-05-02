/*
 *  quoteConsumer.h
 *
 *  Created on: 2017-4-13
 *  Author: chenxun 
 *
 */

#ifndef _QUOTE_CONSUMER_H_
#define _QUOTE_CONSUMER_H_

#include <sstream>
#include <sys/time.h>
#include <time.h>

#include "ActiveMqConsumer.h"
#include "ActiveMqProducer.h"
#include "DBPool.h"
#include "zlog.h"

const std::string log_config_file_path = "./log.conf";

//zlog
extern int rc;
extern zlog_category_t *zc;
void zlogInit(std::string path);

std::string getStrTime();

class QuoteData
{
public:
    QuoteData(){}
    //json parse
    std::string id;
    std::string symbol;
    std::string product_name ;
    int timestamp;
    std::string currency;
    int type;
    std::string exchange;
    std::string market;
    int bit;
    std::string open;  //开盘价
    std::string close; //昨收价
    std::string ask;   //买入价
    std::string bid;   //卖出价
    std::string last;
    std::string high;
    std::string low;
    int tradedate; //交易日
    std::string createman;
    int modifytime;
    std::string sequence;
};

class quoteConsumer : public msg_recv_interface, public QuoteData
{
public:
    quoteConsumer();

    virtual ~quoteConsumer()
    {
        if( state != NULL )
        {
            delete state;
        }
        state = NULL;

        if( con != NULL )
        {
            delete con;
        }
        
        connpool.ReleaseConnection(con);
    }

    //init map
    void initMap();

    //parse json data 
    void parseJson(const string& quote);

    //get sql
    std::string getExcSql();

    //get table name
    void getTableName();

    //uuid
    std::string getUuid();

    //save data
    void executedb(const string& str);

    //recv msg
    virtual void on_recv(const std::string& request, std::string& response, const Message* message);

private:
    long long count;                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    std::string tableName;

    std::map<std::string,int> time_map;
    std::map<std::string, std::string> seq_map;
    std::string quote_data;

    int m_timestamp;
    std::string m_sequence;

private:
    sql::Connection *con;
    sql::Statement *state;
};

#endif