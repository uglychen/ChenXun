/*
 * main.cpp
 *
 *  Created on: 2013-3-26
 *      Author: holy
 */
 
#include "DBPool.h"
#include <stdio.h> 
 
//初始化连接池
DBPool  *connpool = DBPool::GetInstance();
 
void run() {
 
    Connection *con;
    Statement *state;
    ResultSet *result;
	for(int i = 0; i<10000; i++){    
	con = connpool->GetConnection();
//    for(int i = 0; i<1000; i++)
//	{
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
	
//	}
	delete result;
    delete state;
    connpool->ReleaseConnection(con);
}}
 
int main(int argc, char* argv[]) {
    run();
    return 0;
}
