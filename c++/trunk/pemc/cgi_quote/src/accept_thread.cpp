#include "accept_thread.h"
#include "soap_buffer.h"


accept_thread::accept_thread()
	: ptr_soap_source(NULL)
	, _thread_id(0)
	, _run(true)
{

}

accept_thread::~accept_thread()
{

}

bool accept_thread::init(struct soap *ptr_source)
{
	ptr_soap_source = ptr_source;
	if (NULL == ptr_soap_source)
	{
		return false;
	}

	_run = true;
	if (0 != pthread_create(&_thread_id, NULL, accept_thread_func, (void*)this))
	{
		return false;
	}

	return true;
}

void accept_thread::accept_thread::destory()
{
	//等待线程结束
	_run = false;
	if (0 != pthread_join(_thread_id, NULL))
	{

	}

}

void* accept_thread::accept_thread_func(void *param)
{
	accept_thread *ptr_self = (accept_thread*)param;
	soap_buffer& buffer = g_soap_buffer();

	while (ptr_self->_run)
	{
		SOAP_SOCKET s = soap_accept(ptr_self->ptr_soap_source);
		if (!soap_valid_socket(s))
		{
			if (ptr_self->ptr_soap_source->errnum)
			{
				soap_print_fault(ptr_self->ptr_soap_source, stderr);
				continue; // retry
			}
			else
			{
				fprintf(stderr, "Server timed out\n");
			}
		}

		fprintf(stderr, "Thread %ld accepts socket %d connection from IP %d.%d.%d.%d\n", pthread_self(), s, 
			(int)(ptr_self->ptr_soap_source->ip >> 24) & 0xFF, (int)(ptr_self->ptr_soap_source->ip >> 16) & 0xFF, (int)(ptr_self->ptr_soap_source->ip >> 8) & 0xFF, (int)ptr_self->ptr_soap_source->ip & 0xFF);

		while (false == buffer.push(s))
		{
			sleep(1);
		}
	}
	return NULL;
}

