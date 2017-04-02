/*
 * main.cpp
 *
 *  Created on: 2013-3-26
 *      Author: chenxun
 */
 
#include "DBPool.h"
#include <stdio.h> 

/*--------------------------------------------------------------
    单例模式，全局唯一 db pool，程序中从connpool中获取一个
    db连接使用，使用完之后调用ReleaseConnection把conn放回pool中去.
----------------------------------------------------------------*/
DBPool  connpool = DBPool::GetInstance();


int main(int argc, char* argv[]) 
{
    //初始化连接，创建参数中maxSize一半的连接
    connpool.initPool("tcp://127.0.0.1:3306", "root", "123456", 100);

    Connection *con;
    Statement *state;
    ResultSet *result;
    con = connpool.GetConnection();
    for(int i = 0; i<2; i++)
    {
        state = con->createStatement();
        state->execute("use mysql");
 
        // 查询
        result = state->executeQuery("select host,user from user");
 
        // 输出查询
        while (result->next()) 
        {
            try{
                string user = result->getString("user"); 
                string name = result->getString("host");
                cout << user << " : " << name << endl;
            }catch(sql::SQLException& e){
                std::cout<< e.what() <<std::endl;
            }
        }

        result = state->executeQuery("select cust_id,cust_name from customers");
        while (result->next()) 
        {
            try{
                string user = result->getString("cust_id");
                string name = result->getString("cust_name");
                cout << user << " : " << name << endl;
            }catch(sql::SQLException& e){
              std::cout<< e.what() <<std::endl;
            }
        }
    
        std::cout << i << std::endl;
    
    }

    delete result;
    delete state;
    connpool.ReleaseConnection(con);

    return 0;
}
