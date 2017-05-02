#include "soapH.h"
#include "quote.nsmap"
#include <pthread.h>
#include "accept_thread.h"
#include "proc_thread.h"
#include "soap_buffer.h"
#include <producer/producer.h>
#include <consumer/consumer.h>
#include <string>
#include <signal.h> 


#define BACKLOG (100) // Max. request backlog
#define MAX_THR (20) // Size of thread pool

void sigpipe_handle(int x);



int main(int argc, char **argv)
{
	
	printf("pemc gsoap server start...\n");
	
	printf("----------------------init activemq----------------------\n");
	signal(SIGPIPE, sigpipe_handle);


	//初始化MQ环境
	activemq::library::ActiveMQCPP::initializeLibrary();
	

	int port = 8080;
	struct soap *ptr_soap = soap_new1(SOAP_XML_INDENT);
	accept_thread accept;
	proc_thread proc[MAX_THR];

	while (1)
	{
		//初始化soap
		//soap_init(ptr_soap);
		SOAP_SOCKET m = soap_bind(ptr_soap, NULL, port, BACKLOG);
		if (!soap_valid_socket(m))
		{
			printf("%s(%d):error\n",__FILE__, __LINE__);
			break;
		}

		//初始化处理线程
		bool proc_count = 0;
		for (int i = 0; i < MAX_THR; ++i)
		{
			if (false == proc[i].init(ptr_soap))
			{
				printf("%s(%d): create proc thread error\n", __FILE__, __LINE__);
			}
			else
			{
				++proc_count;
			}
		}

		if (0 == proc_count)
		{
			printf("%s(%d): proc thread error\n", __FILE__, __LINE__);
			break;
		}

		//初始化accept线程
		if (false == accept.init(ptr_soap))
		{
			printf("%s(%d): accept thread error\n", __FILE__, __LINE__);
			break;
		}

		while (1)
		{
			sleep(1);
		}

		break;
	}

	accept.destory();

	for (int i = 0; i < MAX_THR; ++i)
	{
		proc[i].destory();
	}
	//完成
	soap_done(ptr_soap);
	soap_free(ptr_soap);
	activemq::library::ActiveMQCPP::shutdownLibrary();
	return 0;

	printf("pemc gsoap server end...\n");
}

int ns__quote(struct soap *soap, xsd__string request, xsd__string *result)
{
	printf("recv request = %s,  username = %s, passwd = %s\n", request.c_str(), soap->userid, soap->passwd);
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
		printf("send over. exception\n");
		return 408;
	}
}

void sigpipe_handle(int x)
{
	printf("XXXXXXXXXXXXXXXXXXXXXXXXX recv signal = %d\n", x);
}