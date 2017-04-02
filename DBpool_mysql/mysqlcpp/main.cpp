/*
 * main.cpp
 *
 *  Created on: 2013-3-26
 *      Author: holy
 */
 
#include "pool.h"
#include <stdio.h> 
namespace ConnectMySQL {
 
//初始化连接池
ConnPool *connpool = ConnPool::GetInstance();
 
void run() {
 
    Connection *con;
    Statement *state;
    ResultSet *result;
 
    // 从连接池中获取mysql连接
    con = connpool->GetConnection();
 
    state = con->createStatement();
    state->execute("use mysql");
 
    // 查询
    result = state->executeQuery("select host,user from user");
 
    // 输出查询
    while (result->next()) {
        try{
		string user = result->getString("user"); 
        string name = result->getString("host");
        cout << user << " : " << name << endl;
		}catch(sql::SQLException& e){
			std::cout<< e.what() <<std::endl;
		}
	}

    delete state;
    connpool->ReleaseConnection(con);
}
}
 
int main(int argc, char* argv[]) {
    ConnectMySQL::run();
    return 0;
}
