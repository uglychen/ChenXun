/*
    time: 2017.3.31
    author: chenxun   275076730@qq.coom
    notice: MySql Connector C++
*/
#include <iostream>
#include <stdexcept>  
#include <exception>  
#include <stdio.h>  
#include "DBPool.h"
#include "QuoteUtils.h"

using namespace std;
using namespace sql;

DBPool  connpool = DBPool::GetInstance();
pthread_mutex_t DBPool::lock = PTHREAD_MUTEX_INITIALIZER;

//Singleton: get the single object
DBPool& DBPool::GetInstance()
{  
    static DBPool instance_;
    return instance_;  
}

void DBPool::initPool(std::string url_, std::string user_, std::string password_, int maxSize_)
{  
    user = user_;  
    password = password_;
    url = url_;
    maxSize = maxSize_;  
    curSize = 0; 

    try{  
        driver=sql::mysql::get_driver_instance();  
    }  
    catch(sql::SQLException& e)  
    {
        dzlog_error("初始化连接池异常： %s %d", e.getSQLStateCStr(), e.getErrorCode());

    }  
    catch(std::runtime_error& e)  
    {  
        dzlog_error("runtime_error： %s", e.what());
    }

    InitConnection(maxSize/2);
} 

//init conn pool 
void DBPool::InitConnection(int initSize)
{
    sql::Connection* conn;  
    pthread_mutex_lock(&lock);  
    for(int i =0;i <initSize; i++)  
    {  
        conn= CreateConnection();  
        
        if(conn)
        {  
            connList.push_back(conn);  
            ++curSize;  
        }  
        else  
        {  
            cout << "创建连接 失败！" << endl;
        }  
    }  
    pthread_mutex_unlock(&lock);  

}

sql::Connection* DBPool::CreateConnection()
{
    sql::Connection* conn;  
    try{  
        conn = driver->connect(this->url,this->user,this->password);  //create a conn 
        return conn;  
    }  
    catch(sql::SQLException& e)  
    {  
        dzlog_error("创建连接异常： %s %d", e.getSQLStateCStr(), e.getErrorCode());
        return NULL;  
    }  
    catch(std::runtime_error& e)  
    {  
        cout << __FUNCTION__ << ": 创建连接失败！ runtime_error " << e.what() << endl;

        dzlog_error("runtime_error： %s", e.what());
        return NULL;  
    }  
}

sql::Connection* DBPool::GetConnection()
{  
    sql::Connection* conn;  

    pthread_mutex_lock(&lock);

    if(connList.size()>0)//the pool have a conn 
    {  
        conn = connList.front(); 
        connList.pop_front();//move the first conn 
        if(conn->isClosed())//if the conn is closed, delete it and recreate it
        {  
            dzlog_error("connect is closed!");
            delete conn;  
            conn = CreateConnection();  
        }  

        if(conn == NULL)  
        {  
            --curSize;  
        }

        pthread_mutex_unlock(&lock);

        return conn;  
    }  
    else
    {  
        if(curSize< maxSize)//the pool no conn
        {
            conn = CreateConnection();  
            if(conn)
            {  
                ++curSize;  
                pthread_mutex_unlock(&lock);  
                return conn;  
            }  
            else
            {  
                pthread_mutex_unlock(&lock);  
                dzlog_error("connect is NULL! size is %d", curSize);
                return NULL;  
            }  
        }  
        else //the conn count > maxSize
        { 
            pthread_mutex_unlock(&lock);  
            dzlog_error("connect is NULL! size is %d", curSize);
            return NULL;  
        }  
    }  
}  

//put conn back to pool
void DBPool::ReleaseConnection(Connection *conn)
{
    if(conn)
    {  
        pthread_mutex_lock(&lock);  
        connList.push_back(conn);  
        pthread_mutex_unlock(&lock);  
    }  
}

void DBPool::DestoryConnPool()
{  
    list<sql::Connection*>::iterator iter;  
    pthread_mutex_lock(&lock);  
    for(iter = connList.begin(); iter!=connList.end(); ++iter)  
    {  
        this->DestoryConnection(*iter); 
    }  
    curSize=0;  
    connList.clear();
    pthread_mutex_unlock(&lock);  
}  

void DBPool::DestoryConnection(sql::Connection* conn)  
{  
    if(conn)  
    {  
        try{
            conn->close();  
        }  
        catch(sql::SQLException&e)  
        {  
            perror(e.what());  
        }  
        catch(std::exception& e)  
        {  
            perror(e.what());  
        } 
        delete conn;  
    }  
} 
 
DBPool::~DBPool()  
{  
    DestoryConnPool();  
}
