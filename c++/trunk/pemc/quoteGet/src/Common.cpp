
#include <stdlib.h>
#include "Config.h"
#include "Common.h"
#include "zlog.h"
#include "tool.h"
#include "ActiveMqProducer.h"
#include "DBPool.h"
#include "libxml/parser.h"
#include "tinyxml.h"

const std::string log_config_file_path;
int server_prot;
const std::string  server_ip;
const std::string brokerURI;
bool useTopic;
bool clientAck;
std::string topic;
const std::string db_url;
const std::string user;
const std::string passwd;
int connect_num;

void read_config()
{
    const char ConfigFile[]= "./configFile";
    Config configSettings(ConfigFile);
    log_config_file_path = configSettings.Read("log_config_file_path", log_config_file_path);
    server_prot = configSettings.Read("server_prot", server_prot);
    server_ip = configSettings.Read("server_ip", server_ip);
    brokerURI = configSettings.Read("brokerURI", brokerURI);
    useTopic = configSettings.Read("useTopic", useTopic);
    clientAck = configSettings.Read("clientAck", clientAck);
    topic = configSettings.Read("topic", topic);
    db_url = configSettings.Read("db_url", db_url);
    user = configSettings.Read("user", user);
    passwd = configSettings.Read("passwd", passwd);
    connect_num = configSettings.Read("connect_num", connect_num);
}

void read_config_xml()
{
    const char* file_name = "config.xml";
    
    xmlDocPtr doc;
    xmlNodePtr cur;
    
    std::cout << "111111111" << std::endl;
    doc = xmlParseFile(file_name);
    std::cout << "222222222" << std::endl;
    if (doc == NULL) 
    {
        std::cout << "Failed to pares xml file:" << std::endl;
        return 0;
    }
    
    cur = xmlDocGetRootElement(doc);
    xmlChar *key;
    
    cur = cur->xmlChildrenNode;
    while( cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"chenxun")))
        {
            key = xmlNodeGetContent(cur);   
            const std::string str((char*)key);
            std::cout << str << std::endl;
            xmlFree(key);
        }
        
        cur = cur->next;
    }
    /*const char* file_name = "chenxun.xml";
    
    std::cout << "get cur Failed11" <<std::endl;
    xmlDocPtr doc = xmlParseFile(file_name);
    std::cout << "get cur Failed" <<std::endl;

    if (doc == NULL)
    {
        std::cout << "Failed to parse xml file:" << std::endl;
        return 0;
    }
    
    std::cout << "get cur Failed" <<std::endl;
    xmlNodePtr cur = xmlDocGetRootElement(doc);

    if (cur == NULL)
    {
        std::cout << "get cur Failed" <<std::endl;
    }

    xmlChar *key;
    cur = cur->xmlChildrenNode;
    while( cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"chenxun")))
        {
            key = xmlNodeGetContent(cur);   
            log_config_file_path = (char*)key;
            std::cout << log_config_file_path << std::endl;
            xmlFree(key);
        }
        
        /*if ((!xmlStrcmp(cur->name, (const xmlChar *)"server_prot")))
        {
            key = xmlNodeGetContent(cur);   
            server_prot = atol((char*)key);
            std::cout << server_prot << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"server_ip")))
        {
            key = xmlNodeGetContent(cur);   
            server_ip = (char*)key;
            std::cout << server_ip << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"brokerURI")))
        {
            key = xmlNodeGetContent(cur);   
            brokerURI = (char*)key;
            std::cout << brokerURI << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"useTopic")))
        {
            key = xmlNodeGetContent(cur);   
            useTopic = atoi((char*)key);
            std::cout << useTopic << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"clientAck")))
        {
            key = xmlNodeGetContent(cur);   
            clientAck = atoi((char*)key);
            std::cout << clientAck << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"topic")))
        {
            key = xmlNodeGetContent(cur);   
            topic = (char*)key;
            std::cout << topic << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"db_url")))
        {
            key = xmlNodeGetContent(cur);   
            db_url = (char*)key;
            std::cout << db_url << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"user")))
        {
            key = xmlNodeGetContent(cur);   
            user = (char*)key;
            std::cout << user << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"passwd")))
        {
            key = xmlNodeGetContent(cur);   
            passwd = (char*)key;
            std::cout << passwd << std::endl;
            xmlFree(key);
        }

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"connect_num")))
        {
            key = xmlNodeGetContent(cur);   
            connect_num = atoi((char*)key);
            std::cout << connect_num << std::endl;
            xmlFree(key);
        }

        cur = cur->next;
    }*/

}

void read_config_tinyxml()
{
    const char* filepath = "config.xml";
    TiXmlDocument doc(filepath);
    bool loadOkay = doc.LoadFile();
    TiXmlElement* root = doc.RootElement();

    std::string tmp = "";

    TiXmlElement *item = root->FirstChildElement();
    tmp = (item->GetText());
    log_config_file_path = tmp;
    //std::cout << log_config_file_path << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    server_prot = atoi(tmp.c_str());
    //std::cout << server_prot << std::endl;
    
    item = item->NextSiblingElement();
    tmp = (char*)(item->GetText());
    server_ip = tmp;
    //std::cout << server_ip << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    brokerURI = tmp;
    //std::cout << brokerURI << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    useTopic = atoi(tmp.c_str());
    //std::cout << useTopic << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    clientAck = atoi(tmp.c_str());
    //std::cout << clientAck << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    topic =  tmp;
    //std::cout << topic << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    db_url = tmp;
    //std::cout << db_url << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    user = tmp;
    //std::cout << user << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    passwd = tmp;
    //std::cout << passwd << std::endl;

    item = item->NextSiblingElement();
    tmp = (item->GetText());
    connect_num = atoi(tmp.c_str());
    //std::cout << connect_num << std::endl;

    //item = item-> NextSiblingElement();
    //const char* name2 = item->ToElement()->GetText();
    //std::cout << "name2:" << name2 << std::endl;

   /* for (; item != NULL; item = item->NextSiblingElement() )
    {
        const char* name1 = item->GetText();
        std::cout << "name1:" << name1 << std::endl;
    }*/
}

void init_quote()
{
    read_config_tinyxml();  //read config
    zlogInit(log_config_file_path); //init log
    zlog_info(zc, "log_config_file_path:%s", log_config_file_path.c_str());
    zlog_info(zc, "server_prot:%d", server_prot);
    zlog_info(zc, "server_ip:%s", server_ip.c_str());
    zlog_info(zc, "brokerURI:%s", brokerURI.c_str());
    zlog_info(zc, "useTopic:%d", useTopic);
    zlog_info(zc, "clientAck:%d", clientAck);
    zlog_info(zc, "topic:%s", topic.c_str());
    zlog_info(zc, "db_url:%s", db_url.c_str());
    zlog_info(zc, "user:%s", user.c_str());
    zlog_info(zc, "passwd:%s", passwd.c_str());
    zlog_info(zc, "connect_num:%d", connect_num);

    //init db pool
    connpool.initPool(db_url, user, passwd, connect_num);

    //init active mq
    activemq::library::ActiveMQCPP::initializeLibrary();
    ActiveMqProducer::getInstance()->initProducer(brokerURI, useTopic, clientAck);
}

