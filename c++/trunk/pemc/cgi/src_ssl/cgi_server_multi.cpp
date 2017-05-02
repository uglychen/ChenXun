#include "soapH.h"
#include "trade.nsmap"
#include <pthread.h>
#include "accept_thread.h"
#include "proc_thread.h"
#include "soap_buffer.h"
#include <producer/producer.h>
#include <consumer/consumer.h>
#include <string>
#include <signal.h> 
#include "zlog.h"
#include <unistd.h> /* defines POSIX THREADS if pthreads are available */
#include <openssl/crypto.h>


#define BACKLOG (1000) // Max. request backlog
#define MAX_THR (20) // Size of thread pool


//socket出错信号
void sigpipe_handle(int x);

//初始化日志
bool init_log();

//ssl mutex
bool CRYPTO_thread_setup();
void CRYPTO_thread_cleanup();


int main(int argc, char **argv)
{
	//初始化日志
	if (false == init_log())
	{
		return -1;
	}
	dzlog_info("Initial log success...");

	//初始化MQ环境
	activemq::library::ActiveMQCPP::initializeLibrary();
	dzlog_info("Initial activemq library success...");
	
	struct soap gsoap;
	accept_thread soap_accept_thread;
	proc_thread proc[MAX_THR];
	while (1)
	{
		int port = 443;
		
		/* init OpenSSL (just once) */
		soap_ssl_init();	
		dzlog_info("Initial soap ssl success...");

		//gsoap keep alive
		signal(SIGPIPE, sigpipe_handle);

		/* Note: CRYPTO_thread_setup() must be called in multi-thread environment */
		if (false == CRYPTO_thread_setup())
		{
			dzlog_error("Initial soap ssl server context error");
			break;
		}
		dzlog_info("Initial crypto thread success...");


		//配置超时和keep alive属性
		//soap_init2(&gsoap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
		//gsoap.keep_alive = 1;
		//gsoap.connect_timeout = 3;
		//gsoap.accept_timeout = 5;
		//gsoap.send_timeout = 3;
		//gsoap.recv_timeout = 5;
		soap_init(&gsoap);

		dzlog_info("Initial soap success...");

		if (soap_ssl_server_context(&gsoap,
			SOAP_SSL_DEFAULT,
			"server.pem", /* keyfile: required when server must authenticate to clients (see SSL docs on  how to obtain this file) */
			"password", /* password to read the key file (not used with GNUTLS) */
			"cacert.pem", /* optional cacert file to store trusted certificates */
			NULL, /* optional capath to directory with trusted certificates */
			"dh2048.pem", /* DH file name or DH key len bits (minimum is 512, e.g. ”512”) to generate DH param, if NULL use RSA */
			NULL, /* if randfile!=NULL: use a file with random data to seed randomness */
			"sslserver" /* optional server identification to enable SSL session cache (must be a unique name)*/
		))
		{
			//soap print fault(&soap, stderr);
			dzlog_error("Initial soap ssl server context error");
			break;
		}
		dzlog_info("Initial soap ssl context success...");

		//初始化soap
		//gsoap.accept_timeout = 60;	/* server times out after 1 minute inactivity */
		gsoap.send_timeout = gsoap.recv_timeout = 30;	/* if I/O stalls, then timeout after 30 seconds */
		SOAP_SOCKET main_socket = soap_bind(&gsoap, NULL, port, BACKLOG);
		if (!soap_valid_socket(main_socket))
		{
			dzlog_error("Check main socket error");
			break;
		}
		dzlog_info("Bind soap success...");
		
		//初始化处理线程
		bool proc_count = 0;
		for (int i = 0; i < MAX_THR; ++i)
		{
			if (false == proc[i].init())
			{
				dzlog_error("Create proc thread error");
			}
			else
			{
				++proc_count;
			}
		}
		dzlog_info("Initial process thread success...");

		if (0 == proc_count)
		{
			dzlog_error("Proc thread error");
			break;
		}

		//初始化accept线程
		if (false == soap_accept_thread.init(&gsoap))
		{
			dzlog_error("Accept thread error");
			break;
		}
		dzlog_info("Initial accept thread success...");

		dzlog_info("Pemc gsoap server start");
		while (1)
		{
			sleep(1);
		}

		break;
	}


	dzlog_info("Pemc gsoap server ending");
	soap_accept_thread.destory();

	for (int i = 0; i < MAX_THR; ++i)
	{
		proc[i].destory();
	}
	//完成
	soap_destroy(&gsoap);
	soap_end(&gsoap);
	soap_done(&gsoap);
	CRYPTO_thread_cleanup();
	activemq::library::ActiveMQCPP::shutdownLibrary();
	dzlog_info("Pemc gsoap server end");
	return 0;

}

int ns__trade(struct soap *soap, xsd__string request, xsd__string *result)
{
	dzlog_info("Recv request = %s,  username = %s, passwd = %s\n", request.c_str(), soap->userid, soap->passwd);
	proc_thread *ptr_self = (proc_thread*)soap->user;

	try
	{
		if (ptr_self->get_producer()->send_message(request, *result, 3000))
		{
			return SOAP_OK;
		}
		else
		{
			return 408;
		}
	}
	catch (...)
	{
		return 408;
	}
}

void sigpipe_handle(int x)
{
	dzlog_warn("signal, x = %d", x);
}

bool init_log()
{
	printf("file:%s, line:%d, initial log\n", __FILE__, __LINE__);
	// zlog init
	int zlog = dzlog_init("zlog.conf", "cgi");
	if (zlog)
	{
		printf("file:%s, line:%d, dzlogrt init failed\n", __FILE__, __LINE__);
		return false;
	}

	zlog_category_t* ptr_category = zlog_get_category("my_cat");
	if (NULL == ptr_category)
	{
		printf("file:%s, line:%d, get cat fail\n", __FILE__, __LINE__);
		zlog_fini();
		return false;
	}

	dzlog_info("ZLOG initial fail");
	return true;
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

bool CRYPTO_thread_setup()
{
	//dzlog_info("thread = %ld", pthread_self());
	mutex_buf = (pthread_mutex_t*)malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
	if (!mutex_buf)
	{
		return false;
	}

	for (int i = 0; i < CRYPTO_num_locks(); i++)
	{
		pthread_mutex_init(&mutex_buf[i], NULL);
	}
	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(locking_function);
	CRYPTO_set_dynlock_create_callback(dyn_create_function);
	CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
	CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
	return true;
}

void CRYPTO_thread_cleanup()
{
	//dzlog_info("thread = %ld", pthread_self());
	int i;
	if (!mutex_buf)
	{
		return;
	}
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
