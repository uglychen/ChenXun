#include "proc_thread.h"
#include "soap_buffer.h"
#include <string>


proc_thread::proc_thread()
	: ptr_soap_loop(NULL)
	, _thread_id(0)
	, _run(true)
{

}

proc_thread::~proc_thread()
{

}

bool proc_thread::init(struct soap *ptr_source, int number)
{
	ptr_soap_loop = soap_copy(ptr_source);
	if (NULL == ptr_soap_loop)
	{
		printf("111111111111\n");
		return false;
	}

	
	static const std::string reques_qeueu = "cgi_pemc_mid_trade_req";
	static const std::string reques_addr = "failover:(tcp://172.18.15.7:61616,tcp://172.18.15.10:61616)?randomize=false";
	std::string response_qeueu = "cgi_pemc_mid_trade_res";
	char buffer[10];
	sprintf(buffer, "_%d", number);
	response_qeueu.append(buffer);
	if (false == _sync_producer.init(reques_addr, reques_qeueu, response_qeueu))
	{
		printf("2222222222222\n");
		return false;
	}

	_run = true;
	if (0 != pthread_create(&_thread_id, NULL, proc_thread_func, (void*)this))
	{
		printf("3333333333\n");
		return false;
	}

	return true;
}

void proc_thread::proc_thread::destory()
{
	//等待线程结束
	_run = false;
	if (0 != _thread_id && 0 != pthread_join(_thread_id, NULL))
	{

	}

	soap_done(ptr_soap_loop);
	free(ptr_soap_loop);
}

void* proc_thread::proc_thread_func(void *param)
{
	proc_thread *ptr_self = (proc_thread*)param;
	soap_buffer& buffer = g_soap_buffer();

	

	while (ptr_self->_run)
	{
		if (false == buffer.pop(ptr_self->ptr_soap_loop->socket))
		{
			printf("usleep\n");
			usleep(10);
			continue;
		}

		if (!soap_valid_socket(ptr_self->ptr_soap_loop->socket))
		{
			printf("process socket %d is not valid\n", ptr_self->ptr_soap_loop->socket);
			continue;
		}

		ptr_self->ptr_soap_loop->user = param;
		soap_serve(ptr_self->ptr_soap_loop);
		soap_destroy(ptr_self->ptr_soap_loop);
		soap_end(ptr_self->ptr_soap_loop);
	}
	return NULL;
}

