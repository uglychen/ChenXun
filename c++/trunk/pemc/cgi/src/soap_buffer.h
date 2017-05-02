#ifndef _SOAP_BUFFER_H_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
#define _SOAP_BUFFER_H_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
#include "soapH.h"
#include <pthread.h>
#include <deque>

class soap_buffer
{
private:
	soap_buffer();
public:
	~soap_buffer();


	bool push(SOAP_SOCKET);
	bool pop(SOAP_SOCKET&);

private:
	pthread_mutex_t _queue_cs;
	pthread_cond_t _queue_cv;
	std::deque<SOAP_SOCKET> _soap_map;
	volatile bool _run;

	friend soap_buffer& g_soap_buffer();
};

soap_buffer& g_soap_buffer();
#endif // !_SOAP_BUFFER_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
