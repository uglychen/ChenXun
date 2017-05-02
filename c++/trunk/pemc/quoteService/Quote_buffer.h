#ifndef _QUOTE_BUFFER_H_
#define _QUOTE_BUFFER_H_
#include "QuoteDataDef.h"
#include <pthread.h>
#include <deque>

class Quote_buffer
{
public:
	Quote_buffer();

	~Quote_buffer();


	bool push(const QuoteMsg& quoteMsg);
	bool pop(QuoteMsg& quoteMsg);

private:
	pthread_mutex_t _queue_cs;
	pthread_cond_t _queue_cv;
	std::deque<QuoteMsg> _soap_map;
	volatile bool _run;
};

#endif // !_SOAP_BUFFER_FD47A1F9_FEF2_453F_9A5A_4A0353C296F4_
