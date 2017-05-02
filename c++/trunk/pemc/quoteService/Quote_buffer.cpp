#include "Quote_buffer.h"

#define MAX_QUEUE (10000) // Max. size of request queue

Quote_buffer::Quote_buffer()
	:_run(true)
{
	pthread_mutex_init(&_queue_cs, NULL);
	pthread_cond_init(&_queue_cv, NULL);
}
Quote_buffer::~Quote_buffer()
{
	pthread_mutex_destroy(&_queue_cs);
	pthread_cond_destroy(&_queue_cv);
}

bool Quote_buffer::push(const QuoteMsg& quoteMsg)
{
	bool ret = false;
	pthread_mutex_lock(&_queue_cs);
	if (_soap_map.size() < MAX_QUEUE) //最大1万个缓冲
	{
		_soap_map.push_back(quoteMsg);
		ret = true;
	}
	pthread_cond_signal(&_queue_cv);
	pthread_mutex_unlock(&_queue_cs);
	return ret;
}
bool Quote_buffer::pop(QuoteMsg& quoteMsg)
{
	bool ret = false;
	pthread_mutex_lock(&_queue_cs);
	while (_soap_map.empty())
	{
		pthread_cond_wait(&_queue_cv, &_queue_cs);
	}

	if (!_soap_map.empty())
	{
		quoteMsg = _soap_map.front();
		_soap_map.pop_front();
		ret = true;
	}

	pthread_mutex_unlock(&_queue_cs);
	return ret;
}