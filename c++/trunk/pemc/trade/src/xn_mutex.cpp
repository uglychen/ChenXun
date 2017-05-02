#include "xn_mutex.h"

xn_mutex::xn_mutex() 
{
}

xn_mutex::~xn_mutex() 
{ 
	pthread_mutex_destroy(&_mutex);
}

bool xn_mutex::init() 
{ 
	return pthread_mutex_init(&_mutex, NULL) == 0; 
}

void xn_mutex::lock()
{ 
	pthread_mutex_lock(&_mutex); 
}

bool xn_mutex::try_lock()
{
	return 0 == pthread_mutex_trylock(&_mutex);
}

void xn_mutex::unlock()
{ 
	pthread_mutex_unlock(&_mutex); 
}
