#include "proc_thread.h"
#include "soap_buffer.h"
#include <string>
#include "zlog.h"

proc_thread::proc_thread()
	: _thread_id(0)
	, _run(true)
{
}

proc_thread::~proc_thread()
{

}

bool proc_thread::init()
{
	//static const std::string reques_qeueu = "cgi_pemc_mid_trade_req";
	static const std::string reques_qeueu = "Quote.Cgi.Request";
	static const std::string uri = "failover://(tcp://172.18.15.7:61616)";
	if (false == _sync_producer.init(uri, reques_qeueu))
	{
		dzlog_error("Initial activemq error");
		return false;
	}

	_run = true;
	if (0 != pthread_create(&_thread_id, NULL, proc_thread_func, (void*)this))
	{
		return false;
	}

	return true;
}

void proc_thread::proc_thread::destory()
{
	//等待线程结束
	_run = false;
	if (0 != pthread_join(_thread_id, NULL))
	{

	}
}

void* proc_thread::proc_thread_func(void *param)
{
	proc_thread *ptr_self = (proc_thread*)param;
	soap_buffer& buffer = g_soap_buffer();

	struct soap* ptr_soap = NULL;
	while (ptr_self->_run)
	{
		if (false == buffer.pop(&ptr_soap))
		{
			usleep(10);
			continue;
		}

		if (soap_ssl_accept(ptr_soap) != SOAP_OK)
		{
			dzlog_error("Soap ssl accept error");
		}
		else
		{
			ptr_soap->user = param;
			soap_serve(ptr_soap);
		}

		
		soap_destroy(ptr_soap);
		soap_end(ptr_soap);
		soap_free(ptr_soap);
	}
	return NULL;
}

