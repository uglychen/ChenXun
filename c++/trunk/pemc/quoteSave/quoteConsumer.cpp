/*
 *  quoteConsumer.cpp
 *
 *  Created on: 2017-4-16
 *  Author: chenxun 
 *
 */

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <map>
#include <string>
#include <memory>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/metadata.h>
#include <cppconn/exception.h>
#include <json/json.h>
#include <uuid/uuid.h>  
#include <time.h>

#include "quoteConsumer.h"

const std::string DBHOST = "tcp://116.62.176.131:3306/calf_middle_gg";
const std::string USER =  "user1";
const std::string PASSWORD = "123456";

using namespace Json;

int rc;
zlog_category_t *zc;

void zlogInit(std::string path)
{
    rc =zlog_init(path.c_str());
    if (rc)
    {
        printf("init failed\n");
        zlog_fini();
        return;
    }

    zc = zlog_get_category("my_cat");
}

//get linux time usec
std::string getStrTime()  
{  
    struct  timeval    tv;
    struct  timezone   tz;
    gettimeofday(&tv,&tz);

    time_t t = tv.tv_sec;
    struct tm* p= localtime(&t); 
    char s[100];  
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);

    std::ostringstream str;
    str << s;
    str << tv.tv_usec;

    return str.str();
}

quoteConsumer::quoteConsumer()
{
    count = 0;
    con = NULL;
    state = NULL;
    tableName = "";
    m_timestamp = 0;;
    m_sequence = "";
    quote_data= "";

    initMap();

    //json parse
    std::string id = "";
    std::string symbol = "";
    std::string name = "";
    timestamp = 0;
    std::string currency;
    type = 0;
    std::string exchange = "";
    std::string market = "";
    bit = 0;
    std::string open = "";  //开盘价
    std::string close = ""; //昨收价
    std::string ask = "";   //买入价
    std::string bid = "";   //卖出价
    std::string last = "";
    std::string high = "";
    std::string low = "";
    tradedate = 0; //交易日
    std::string createman = "";
    modifytime = 0;
    std::string sequence = "";
}

void quoteConsumer::initMap()
{
    time_map["GDAG"] = 0;
    time_map["GDPD"] = 0;
    time_map["GDPT"] = 0;

    seq_map["GDAG"] = "";
    seq_map["GDPD"] = "";
    seq_map["GDPT"] = "";
}

void quoteConsumer::on_recv(const std::string& request, std::string& response, const Message* message)
{
    zlog_info(zc, "msgId: %d", ++count);
    zlog_info(zc, "recv msg:%s", request.c_str());
    parseJson(request);
    string sqlstate = getExcSql();
    executedb(sqlstate);
}

void quoteConsumer::parseJson(const string& quote)
{
    if ( quote.empty() )
    {
        zlog_info(zc, " quoteConsumer::parseJson recv msg is empty ");
        return;
    }

    zlog_info(zc, "quoteConsumer::parseJson begin parse json data");
    try 
    {
        Reader reader;
        Value value;
        if (!reader.parse(quote, value))
        {
            zlog_info(zc, "quoteConsumer::parseJson parse json data failed");
            return;
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
        sequence = value["SEQUENCE"].asString();
        zlog_info(zc, "quoteConsumer::parseJson [sequence: %s]", sequence.c_str());

        quote_data = quote;
    }
    catch (std::exception &ex)
    {
        zlog_info(zc, "json exception: %s", ex.what());
        return;
    }

    zlog_info(zc, " quoteConsumer::parseJson json data end ");

}

void quoteConsumer::getTableName()
{
    if (symbol == "GDAG")
    {
        product_name = "粤贵银";
        tableName = "gg_market_silver_his";
    }
    else if (symbol == "GDPD")
    {
        product_name = "粤贵钯";
        tableName = "gg_market_palladium_his";
    }
    else if (symbol == "GDPT")
    {
        product_name = "粤贵铂";
        tableName = "gg_market_platinum_his";
    }
    else
    {
        cout << "can't find symbol and talbes" << endl;
    }
}

std::string quoteConsumer::getExcSql()
{
    char buffer[1024] = {0};

    //id = getUuid();
    getTableName();

    if( timestamp > time_map[symbol])
    {
        zlog_info(zc,"timestamp%d > time_map[symbol]:%d", timestamp, time_map[symbol]);
        time_map[symbol] = timestamp;
        seq_map[symbol] = "1";
        std::string topic = "Quote.Single.Org";
        zlog_info(zc, "send message[topic--->Quote.Single.Org]:%s", quote_data.c_str());
        ActiveMqProducer::getInstance()->sendMessage(topic, quote_data);
    }
    else if( timestamp == time_map[symbol] && sequence > seq_map[symbol])
    {
        zlog_info(zc, "timestamp == m_timestamp && sequence > seq_map[symbol]");
        time_map[symbol] = timestamp;
        seq_map[symbol] = sequence;
        std::string topic = "Quote.Single.Org";
        zlog_info(zc, "send message[topic--->Quote.Single.Org]:%s", quote_data.c_str());
        ActiveMqProducer::getInstance()->sendMessage(topic, quote_data);
    }
    else
    {
        zlog_info(zc, "--------The data have save into DB-------------");
        return "";
    }

    char temp[32] = { 0 };
    sprintf(temp,"FROM_UNIXTIME(%d)", timestamp);
    string s_timestamp(temp);

    memset(temp, 0, 32);
    sprintf(temp, "FROM_UNIXTIME(%d)", tradedate);
    string s_tradedate(temp);

    time_t now;
    time(&now);
    memset(temp, 0, 32);
    sprintf(temp, "FROM_UNIXTIME(%d)", now);
    string s_cratetime(temp);

    memset(temp, 0, 32);
    sprintf(temp, "%d", type);
    string s_type(temp);

    memset(temp, 0, 32);
    sprintf(temp, "%d", bit);
    string s_bit(temp);

    sprintf(buffer, "INSERT INTO %s VALUES(%s, \"%s\", \"%s\", %s, \"%s\", %d, \"%s\", \"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %s, %s, NULL, NULL,\"%s\")",
        tableName.c_str(),
        "replace(uuid(),'-','')",
        symbol.c_str(),
        product_name.c_str(),
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
        s_cratetime.c_str(),
        sequence.c_str()
        );

    string sqlstatement(buffer);
    return sqlstatement;
}

//get the uuid 
std::string quoteConsumer::getUuid()
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

// execut sql
void quoteConsumer::executedb(const string& str)
{
    if ( str.empty() )
    {
        return;
    }

    try{
        con = connpool.GetConnection();
        state = con->createStatement();
        state->execute(str);
        connpool.ReleaseConnection(con);
        delete state;
        state = NULL;
        zlog_info(zc,"save data:%s", str.c_str());
    }catch(sql::SQLException &e){
        std::cout << e.what() <<std::endl;
    }
}