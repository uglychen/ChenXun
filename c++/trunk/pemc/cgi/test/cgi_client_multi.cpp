#include "soapH.h"
#include "trade.nsmap"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

void *thread_func(void* param);
void *thread_func_1(void* param);

std::string server = "http://172.18.15.7:8090/pmec/mid/trade/";

void send(struct soap * account_info_soap, const std::string& message)
{
	std::string result;
	if (soap_call_ns__trade(account_info_soap, server.c_str(), NULL, message, &result) == SOAP_OK)
	{
		//printf("result is %s, thread id = %lu\n", result.c_str(), pthread_self());
	}
	else
	{
		soap_print_fault(account_info_soap, stderr);
		printf("error, thread id = %lu\n", pthread_self());
	}

}

int main(int argc, char **argv)
{
	static const int count = 20;
	pthread_t thread[count];

	for (int i = 0; i < count; ++i)
	{
		 pthread_create(&thread[i], NULL, thread_func, NULL);
	}

	while (1)
	{
		sleep(1);
	}

	return 0;
}


void *thread_func(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	struct soap *account_info_soap = soap_new();
	
	
	account_info_soap->userid = "1";
	account_info_soap->passwd = "1";

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"180000000000003\",\"Passwd\":\"12345678\"}}";
	//std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"180000000000003\",\"Passwd\":\"11\"}}";
	send(account_info_soap, request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(account_info_soap, request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(account_info_soap, request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(account_info_soap, request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(account_info_soap, request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(account_info_soap, request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(account_info_soap, request);





		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"180000000000003\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"180000000000003\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"180000000000003\"}}";
		//send(request);

		

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"180000000000003\"}}";
		//send(request);

	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"180000000000003\"}}";
	send(account_info_soap, request);

	soap_end(account_info_soap);
	soap_free(account_info_soap);

	return NULL;
}




//void *thread_func_1(void* param)
//{
//
//	struct soap *login_soap = soap_new();
//	std::string server = "http://localhost:8090";
//	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"520147896369547\",\"Passwd\":\"123456\"}}";
//	std::string result;
//	login_soap->userid = "1";
//	login_soap->passwd = "1";
//
//	if (soap_call_ns__trade(login_soap, server.c_str(), NULL, request, &result) == SOAP_OK)
//	{
//		printf("result is %s\n", result.c_str());
//	}
//	else
//	{
//		soap_print_fault(login_soap, stderr);
//	}
//
//	soap_end(login_soap);
//	soap_free(login_soap);
//
//	while (1)
//	{
//		struct soap *account_info_soap = soap_new();
//		std::string account_info_request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"520147896369547\"}}";
//
//		account_info_soap->userid = "1";
//		account_info_soap->passwd = "1";
//
//		if (soap_call_ns__trade(account_info_soap, server.c_str(), NULL, account_info_request, &result) == SOAP_OK)
//		{
//			printf("result is %s, thread id = %lu\n", result.c_str(), pthread_self());
//		}
//		else
//		{
//			soap_print_fault(account_info_soap, stderr);
//		}
//
//		soap_end(account_info_soap);
//		soap_free(account_info_soap);
//
//		sleep(10);
//	}
//
//	return 0;
//}