#include "soapH.h"
#include "trade.nsmap"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

void *thread_func(void* param);
void *thread_func_1(void* param);
void *thread_func_2(void* param);
void *thread_func_3(void* param);
void *thread_func_4(void* param);
void *thread_func_5(void* param);
void *thread_func_6(void* param);
void *thread_func_7(void* param);
void *thread_func_8(void* param);
void *thread_func_9(void* param);

//ssl mutex
int CRYPTO_thread_setup();
void CRYPTO_thread_cleanup();

void send(const std::string& message)
{
	struct soap account_info_soap;
	

	soap_init(&account_info_soap);

	if (soap_ssl_client_context(&account_info_soap,
		SOAP_SSL_DEFAULT | SOAP_SSL_ALLOW_EXPIRED_CERTIFICATE,
		"client.key", /* keyfile: required only when client must authenticate to server (see SSL docs on how to obtain this file) */
		NULL, /* password to read the key file (not used with GNUTLS) */
		"server.crt", /* cacert file to store trusted certificates (needed to verify server) */
		NULL, /* capath to directory with trusted certificates */
		NULL /* if randfile!=NULL: use a file with random data to seed randomness */
	))
	{
		printf("Init soap ssl client context error \n");
		return;
	}


	std::string server = "https://172.18.15.7/pmec/mid/trade/";
	std::string result;
	account_info_soap.userid = "1";
	account_info_soap.passwd = "cpp";
	account_info_soap.connect_timeout = 60;	/* try to connect for 1 minute */
	account_info_soap.send_timeout = account_info_soap.recv_timeout = 30;	/* if I/O stalls, then timeout after 30 seconds */

	if (soap_call_ns__trade(&account_info_soap, server.c_str(), NULL, message, &result) == SOAP_OK)
	{
		printf("result is %s, thread id = %lu\n", result.c_str(), pthread_self());
	}
	else
	{
		soap_print_fault(&account_info_soap, stderr);
	}

	soap_destroy(&account_info_soap); /* C++ */
	soap_end(&account_info_soap);
	soap_done(&account_info_soap);
	
	//soap_free(&account_info_soap);
	
	usleep(1);
}

//ssl
struct CRYPTO_dynlock_value
{
	pthread_mutex_t mutex;
};

static pthread_mutex_t *mutex_buf;

static struct CRYPTO_dynlock_value *dyn_create_function(const char *file, int line)
{
	//dzlog_info("file = %s, line = %d", file, line);
	//struct CRYPTO_dynlock_value *value;
	//(void)file; (void)line;
	struct CRYPTO_dynlock_value *value = (struct CRYPTO_dynlock_value*)malloc(sizeof(struct CRYPTO_dynlock_value));
	if (value)
	{
		pthread_mutex_init(&value->mutex, NULL);
	}
	return value;
}

static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l, const char *file, int line)
{
	//dzlog_info("mode = %d, file = %s, line = %d", mode, file, line);
	//(void)file; (void)line;
	if (mode & CRYPTO_LOCK)
	{
		pthread_mutex_lock(&l->mutex);
	}
	else
	{
		pthread_mutex_unlock(&l->mutex);
	}
}

static void dyn_destroy_function(struct CRYPTO_dynlock_value *l, const char *file, int line)
{
	//dzlog_info("file = %s, line = %d", file, line);
	//(void)file; (void)line;
	pthread_mutex_destroy(&l->mutex);
	free(l);
}

static void locking_function(int mode, int n, const char *file, int line)
{
	//dzlog_info("mode = %d, n = %d, file = %s, line = %d", mode, n, file, line);
	//(void)file; (void)line;
	if (mode & CRYPTO_LOCK)
	{
		pthread_mutex_lock(&mutex_buf[n]);
	}
	else
	{
		pthread_mutex_unlock(&mutex_buf[n]);
	}
}

static unsigned long id_function()
{
	//dzlog_info("thread = %ld", pthread_self());
	return (unsigned long)pthread_self();
}

int CRYPTO_thread_setup()
{
	//dzlog_info("thread = %ld", pthread_self());
	int i;
	mutex_buf = (pthread_mutex_t*)malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
	if (!mutex_buf)
		return SOAP_EOM;
	for (i = 0; i < CRYPTO_num_locks(); i++)
	{
		pthread_mutex_init(&mutex_buf[i], NULL);
	}
	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(locking_function);
	CRYPTO_set_dynlock_create_callback(dyn_create_function);
	CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
	CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
	return SOAP_OK;
}

void CRYPTO_thread_cleanup()
{
	//dzlog_info("thread = %ld", pthread_self());
	int i;
	if (!mutex_buf)
		return;
	CRYPTO_set_id_callback(NULL);
	CRYPTO_set_locking_callback(NULL);
	CRYPTO_set_dynlock_create_callback(NULL);
	CRYPTO_set_dynlock_lock_callback(NULL);
	CRYPTO_set_dynlock_destroy_callback(NULL);
	for (i = 0; i < CRYPTO_num_locks(); i++)
	{
		pthread_mutex_destroy(&mutex_buf[i]);
	}
	free(mutex_buf);
	mutex_buf = NULL;
}


int main(int argc, char **argv)
{
	static const int count = 10;
	pthread_t thread[count];

	soap_ssl_init();

	if (CRYPTO_thread_setup())
	{
		fprintf(stderr, "Cannot setup thread mutex for OpenSSL\n");
		exit(1);
	}
	

	/*for (int i = 0; i < count; ++i)
	{
		 pthread_create(&thread[i], NULL, thread_func, NULL);
	}*/

	pthread_create(&thread[0], NULL, thread_func, NULL);
	pthread_create(&thread[1], NULL, thread_func_1, NULL);
	pthread_create(&thread[2], NULL, thread_func_2, NULL);
	pthread_create(&thread[3], NULL, thread_func_3, NULL);
	pthread_create(&thread[4], NULL, thread_func_4, NULL);
	pthread_create(&thread[5], NULL, thread_func_5, NULL);
	pthread_create(&thread[6], NULL, thread_func_6, NULL);
	pthread_create(&thread[7], NULL, thread_func_7, NULL);
	pthread_create(&thread[8], NULL, thread_func_8, NULL);
	pthread_create(&thread[9], NULL, thread_func_9, NULL);

	while (1)
	{
		sleep(1);
	}


	CRYPTO_thread_cleanup();
	return 0;
}


void *thread_func(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"180000000000003\",\"Passwd\":\"12345678\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"180000000000003\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"180000000000003\"}}";
		send(request);

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
	send(request);

	return NULL;
}

void *thread_func_1(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000001\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000001\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000001\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000001\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000001\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000001\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000001\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000001\"}}";
	send(request);

	return NULL;
}


void *thread_func_2(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000002\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000002\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000002\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000002\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000002\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000002\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000002\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000002\"}}";
	send(request);

	return NULL;
}


void *thread_func_3(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000003\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000003\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000003\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000003\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000003\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000003\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000003\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000003\"}}";
	send(request);

	return NULL;
}


void *thread_func_4(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000004\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000004\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000004\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000004\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000004\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000004\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000004\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000004\"}}";
	send(request);

	return NULL;
}


void *thread_func_5(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000005\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000005\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000005\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000005\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000005\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000005\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000005\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000005\"}}";
	send(request);

	return NULL;
}


void *thread_func_6(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000006\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000006\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000006\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000006\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000006\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000006\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000006\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000006\"}}";
	send(request);

	return NULL;
}


void *thread_func_7(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000007\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000007\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000007\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000007\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000007\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000007\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000007\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000007\"}}";
	send(request);

	return NULL;
}


void *thread_func_8(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000008\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000008\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000008\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000008\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000008\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000008\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000008\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000008\"}}";
	send(request);

	return NULL;
}


void *thread_func_9(void* param)
{

	//"CommodityID":100000000, "CommodityClassName":"粤贵银",
	//"CommodityID":100000001, "CommodityClassName":"粤贵铂",
	//"CommodityID":100000002, "CommodityClassName":"粤贵钯",

	//登录
	std::string request = "{\"Type\":\"pemc.mid.trade.login\",\"Param\":{\"Account\":\"100000000000009\",\"Passwd\":\"123456\"}}";
	send(request);


	while (1)
	{
		//获取用户信息
		request = "{\"Type\":\"pemc.mid.trade.get.accountinfo\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//获取市场状态
		request = "{\"Type\":\"pemc.mid.trade.get.marketstatus\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//根据商品ID，获取其市价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openmarketconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其市价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closemarketconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价建仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.openlimitconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取其限价平仓的配置信息
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000000}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000001}}";
		send(request);
		request = "{\"Type\":\"pemc.mid.trade.get.closelimitconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000002}}";
		send(request);

		//根据商品ID，获取交割的配置信息
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000000}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000001}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.get.opendeliverconf\",\"Param\":{\"Account\":\"100000000000009\", \"CommodityID\":100000002}}";
		//send(request);

		//请求商品行情
		request = "{\"Type\":\"pemc.mid.trade.req.quote\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//请求请求持仓信息
		request = "{\"Type\":\"pemc.mid.trade.req.hodeposition\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//请求限价单信息
		request = "{\"Type\":\"pemc.mid.trade.req.limitorders\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//请求平仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.closeorders\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//请求所有持仓单信息
		request = "{\"Type\":\"pemc.mid.trade.req.holdpositiontotal\",\"Param\":{\"Account\":\"100000000000009\"}}";
		send(request);

		//修改登录密码MarketType
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000009\", \"OldPasswd\":\"12345678\", \"NewPasswd\":\"123456\", \"MarketType\":1}}";
		//send(request);
		//request = "{\"Type\":\"pemc.mid.trade.req.loginpwdupd\",\"Param\":{\"Account\":\"100000000000009\", \"OldPasswd\":\"123456\", \"NewPasswd\":\"12345678\", \"MarketType\":1}}";
		//send(request);

		//修改资金密码
		//request = "{\"Type\":\"pemc.mid.trade.req.fundpwdupd\",\"Param\":{\"Account\":\"100000000000009\"}}";
		//send(request);

		//退出
		//request = "{\"Type\":\"pemc.mid.trade.exit\",\"Param\":{\"Account\":\"100000000000009\"}}";
		//send(request);
	}

	//登出
	request = "{\"Type\":\"pemc.mid.trade.logout\",\"Param\":{\"Account\":\"100000000000009\"}}";
	send(request);

	return NULL;
}