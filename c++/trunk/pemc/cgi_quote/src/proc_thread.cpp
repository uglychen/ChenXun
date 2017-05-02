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

bool proc_thread::init(struct soap *ptr_source)
{
	ptr_soap_loop = soap_copy(ptr_source);
	if (NULL == ptr_soap_loop)
	{
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

	soap_done(ptr_soap_loop);
	free(ptr_soap_loop);
}

void* proc_thread::proc_thread_func(void *param)
{
	proc_thread *ptr_self = (proc_thread*)param;
	soap_buffer& buffer = g_soap_buffer();

	char buf_pthread_id[128];
	sprintf(buf_pthread_id, "%lu", pthread_self());
	static const std::string reques_qeueu = "Quote.Cgi.Request";
	std::string response_qeueu = reques_qeueu;
	response_qeueu += "_";
	response_qeueu += buf_pthread_id;


	//172.18.15.7
	if (false == ptr_self->_sync_producer.init("failover://(tcp://101.37.33.121:61616)", reques_qeueu/*, response_qeueu*/))
	//if (false == ptr_self->_sync_producer.init("failover://(tcp://172.18.15.7:61616)", reques_qeueu/*, response_qeueu*/))
	{
		printf("2222222222222");
		return NULL;
	}
	printf("3333333333333333333\n");

	while (ptr_self->_run)
	{
		if (false == buffer.pop(ptr_self->ptr_soap_loop->socket))
		{
			usleep(10);
			continue;
		}

		if (!soap_valid_socket(ptr_self->ptr_soap_loop->socket))
		{
			printf("process socket %d is not valid\n", ptr_self->ptr_soap_loop->socket);
			continue;
		}
		printf("44444444444444444\n");
		ptr_self->ptr_soap_loop->user = param;
		soap_serve(ptr_self->ptr_soap_loop);
		soap_destroy(ptr_self->ptr_soap_loop);
		soap_end(ptr_self->ptr_soap_loop);
	}
	return NULL;
}

