#ifndef _PROC_THREAD_H_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
#define _PROC_THREAD_H_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
#include "soapH.h"
#include <pthread.h>
#include <producer/sync_producer.h>

class proc_thread
{
public:
	proc_thread();
	~proc_thread();
	
	bool init(struct soap *ptr_source, int number);
	void destory();

	activemq_sync_producer* get_producer() { return &_sync_producer; }
private:
	static void* proc_thread_func(void *param);
private:
	struct soap *ptr_soap_loop;
	pthread_t _thread_id;
	volatile bool _run;
	activemq_sync_producer _sync_producer;
};

#endif // !_SOAP_BUFFER_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
