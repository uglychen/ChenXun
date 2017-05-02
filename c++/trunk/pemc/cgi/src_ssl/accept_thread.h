#ifndef _ACCEPT_THREAD_H_AC195400_DEA0_4376_8A82_DD4B0A5721D2_
#define _ACCEPT_THREAD_H_AC195400_DEA0_4376_8A82_DD4B0A5721D2_
#include "soapH.h"
#include <pthread.h>

class accept_thread
{
public:
	accept_thread();
	~accept_thread();
	
	bool init(struct soap *ptr_source);
	void destory();

private:
	static void* accept_thread_func(void *param);
private:
	struct soap *ptr_soap_source;
	pthread_t _thread_id;
	volatile bool _run;
};

#endif // !_SOAP_BUFFER_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
