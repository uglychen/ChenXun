#include "soapH.h"
#include "quote.nsmap"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

void *thread_func(void* Param);
void *thread_func_1(void* Param);

void send(const std::string& message)
{
	struct soap *account_info_soap = soap_new();
	//std::string server = "http://localhost:8080";
	std::string server = "http://101.37.33.121:8080";
	std::string result;
	account_info_soap->userid = "1";
	account_info_soap->passwd = "1";

	if (soap_call_ns__quote(account_info_soap, server.c_str(), NULL, message, &result) == SOAP_OK)
	{
		printf("result is %s, thread id = %lu\n", result.c_str(), pthread_self());
	}
	else
	{
		soap_print_fault(account_info_soap, stderr);
	}

	soap_end(account_info_soap);
	soap_free(account_info_soap);
	
	usleep(200);
}

int main(int argc, char **argv)
{
	

	static const int count = 2;
	pthread_t thread[count];

	/*for (int i = 0; i < count; ++i)
	{
		 pthread_create(&thread[i], NULL, thread_func, NULL);
	}*/

	pthread_create(&thread[0], NULL, thread_func, NULL);
	//pthread_create(&thread[1], NULL, thread_func_1, NULL);

	while (1)
	{
		sleep(1);
	}

	return 0;
}


void *thread_func(void* Param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.quote.login\",\"Param\":{\"Account\":\"520147896369547\",\"Passwd\":\"123456\"}}";
	//std::string request = "{\"Type\":\"pemc.mid.quote.login\",\"Param\":{\"Account\":\"520147896369547\",\"Passwd\":\"11\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.quote.get.accountinfo\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.quote.get.marketstatus\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.quote.get.openmarketconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.openmarketconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.openmarketconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.quote.get.closemarketconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.closemarketconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.closemarketconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.quote.get.openlimitconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.openlimitconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.openlimitconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.quote.get.closelimitconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.closelimitconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.get.closelimitconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.quote.get.opendeliverconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.quote.get.opendeliverconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.quote.get.opendeliverconf\",\"Param\":{\"Account\":\"520147896369547\", \"CommodityID\":100000002}}";
		//send(request);

		//根据商品ID，获取交割的配置信息
		request = "{\"Type\":\"pemc.mid.quote.get.opendeliverconf\",\"Param\":{\"Account\":\"520147896369547\",\"CommodityID\":100000000,\"HoldWeight\":2,\"HoldPrice\":3000}}";
		send(request);


		//请求商品行情
		request = "{\"Type\":\"pemc.mid.quote.req.quote\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.quote.req.hodeposition\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.quote.req.limitorders\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.quote.req.closeorders\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.quote.req.holdpositiontotal\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//市价平仓
		request = "{\"Type\":\"pemc.mid.quote.req.closemarket\",\"Param\":{\"Account\":\"520147896369547\",\"HoldPositionID\":10000012120,\"CommodityID\":100000000,\"Weight\":80,\"Quantity\":3,\"quoteRange\":50}}";
		send(request);

		//批量平仓
		request = "{\"Type\":\"pemc.mid.quote.req.closemaretmany\",\"Param\":{\"Account\":\"520147896369547\",\"CommodityID\":100000000,\"Weight\":80,\"Quantity\":3,\"quoteRange\":50,\"CloseDirector\":1}} ";
		send(request);

		//限价建仓
		request = "{\"Type\":\"pemc.mid.quote.req.openlimit\",\"Param\":{\"Account\":\"520147896369547\",\"CommodityID\":100000000,\"ExpireType\":1,\"OpenDirector\":1,\"Weight\":80,\"Quantity\":3,\"OrderPrice\":100,\"SLPrice\":99,\"TPPrice\":98}}";
		send(request);

		//限价平仓
		request = "{\"Type\":\"pemc.mid.quote.req.closelimit\",\"Param\":{\"Account\":\"520147896369547\",\"CommodityID\":100000000,\"ClosePrice\":2,\"ExpireType\":1,\"HoldPositionID\":10000012120,\"SLPrice\":99,\"TPPrice\":98}}";
		send(request);

		//撤销限价单接口
		request = "{\"Type\":\"pemc.mid.quote.req.limitrevoke\",\"Param\":{\"Account\":\"520147896369547\",\"LimitOrderID\":12,\"LimitType\":1}}";
		send(request);

		//查询客户可出资金，用于出金操作
		request = "{\"Type\":\"pemc.mid.quote.req.moneyquery\",\"Param\":{\"Account\":\"520147896369547\"}}";
		send(request);

		//出入金
		request = "{\"Type\":\"pemc.mid.quote.req.moneyinout\",\"Param\":{\"Account\":\"520147896369547\",\"OperateType\":1,\"Currency\":100,\"Amount\":888,\"FundPsw\":\"honglu\",\"BankPsw\":\"ibcc\",\"Reversed\":\"abd\",\"PayType\":1,\"OperateFlag\":1}}";
		send(request);

		//修改资金密码
		request = "{\"Type\":\"pemc.mid.quote.req.fundpwdupd\",\"Param\":{\"Account\":\"520147896369547\",\"OldPasswd\":\"old\",\"NewPasswd\":\"news\"}}";
		send(request);

		//客户资金流水查询接口 
		request = "{\"Type\":\"pemc.mid.quote.req.fundflowquery\",\"Param\":{\"Account\":\"520147896369547\",\"QueryType\":1,\"BeginDate\":111,\"EndDate\":1,\"BeginRow\":11,\"EndRow\":2,\"OperType\":1}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		request = "{\"Type\":\"pemc.mid.quote.req.opendeliverorder\",\"Param\":{\"Account\":\"520147896369547\",\"CommodityID\":100000000,\"HoldPositionID\":10000012120,\"DeliveryCommodityID\":121,\"AppDesc\":\"AppDesc\"}}";
		send(request);

		//银行签约成功通知接口
		request = "{\"Type\":\"pemc.mid.quote.req.signresultnotifyquery\",\"Param\":{\"Account\":\"520147896369547\",\"BankID\":1,\"BankAccount\":\"ICBC\"}}";
		send(request);

		//支付推进接口
		request = "{\"Type\":\"pemc.mid.quote.req.payforward\",\"Param\":{\"Account\":\"520147896369547\",\"Ticket\":\"abc\",\"IdentifyCode\":\"abci\",\"Reversed\":\"call\"}}";
		send(request);

		//修改或查询客户是否可以登录
		request = "{\"Type\":\"pemc.mid.quote.req.customerallowlogin\",\"Param\":{\"Account\":\"520147896369547\",\"AllowLogin\":222}}";
		send(request);

		



		//修改登录密码MarketType
		request = "{\"Type\":\"pemc.mid.quote.req.loginpwdupd\",\"Param\":{\"Account\":\"520147896369547\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.quote.req.loginpwdupd\",\"Param\":{\"Account\":\"520147896369547\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.quote.req.fundpwdupd\",\"Param\":{\"Account\":\"520147896369547\"}}";
		//send(request);

		

		//退出
		//request = "{\"Type\":\"pemc.mid.quote.exit\",\"Param\":{\"Account\":\"520147896369547\"}}";
		//send(request);

	}

	//登出
	request = "{\"Type\":\"pemc.mid.quote.logout\",\"Param\":{\"Account\":\"520147896369547\"}}";
	send(request);

	return NULL;
}




//void *thread_func_1(void* Param)
//{
//
//	struct soap *login_soap = soap_new();
//	std::string server = "http://localhost:8080";
//	std::string request = "{\"Type\":\"pemc.mid.quote.login\",\"Param\":{\"Account\":\"520147896369547\",\"Passwd\":\"12345678\"}}";
//	std::string result;
//	login_soap->userid = "1";
//	login_soap->Passwd = "1";
//
//	if (soap_call_ns__quote(login_soap, server.c_str(), NULL, request, &result) == SOAP_OK)
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
//		std::string account_info_request = "{\"Type\":\"pemc.mid.quote.get.accountinfo\",\"Param\":{\"Account\":\"520147896369547\"}}";
//
//		account_info_soap->userid = "1";
//		account_info_soap->Passwd = "1";
//
//		if (soap_call_ns__quote(account_info_soap, server.c_str(), NULL, account_info_request, &result) == SOAP_OK)
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