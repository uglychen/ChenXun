#include "soap_buffer.h"

#define MAX_QUEUE (100000) // Max. size of request queue

soap_buffer& g_soap_buffer()
{
	static soap_buffer data;
	return data;
}

soap_buffer::soap_buffer()
	:_run(true)
{
	pthread_mutex_init(&_queue_cs, NULL);
	pthread_cond_init(&_queue_cv, NULL);
}
soap_buffer::~soap_buffer()
{
	pthread_mutex_destroy(&_queue_cs);
	pthread_cond_destroy(&_queue_cv);
}

bool soap_buffer::push(struct soap* ptr)
{
	bool ret = false;
	pthread_mutex_lock(&_queue_cs);
	if (_soap_map.size() < MAX_QUEUE) //最大1万个连接
	{
		_soap_map.push_back(ptr);
		ret = true;
	}
	pthread_cond_signal(&_queue_cv);
	pthread_mutex_unlock(&_queue_cs);
	return ret;
}
bool soap_buffer::pop(struct soap** ptr)
{
	bool ret = false;
	pthread_mutex_lock(&_queue_cs);
	while (_soap_map.empty())
	{
		pthread_cond_wait(&_queue_cv, &_queue_cs);
	}

	if (!_soap_map.empty())
	{
		*ptr = _soap_map.front();
		_soap_map.pop_front();
		ret = true;
	}

	pthread_mutex_unlock(&_queue_cs);
	return ret;
}