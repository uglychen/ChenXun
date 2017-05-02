#include "xn_rwlock.h"

xn_rwlock::xn_rwlock() 
{
	init();
}

xn_rwlock::~xn_rwlock() 
{ 
	pthread_rwlock_destroy(&_rwlock);
}

bool xn_rwlock::init() 
{ 
	return pthread_rwlock_init(&_rwlock, NULL) == 0; 
}

void xn_rwlock::rdlock()
{ 
	pthread_rwlock_rdlock(&_rwlock); 
}

void xn_rwlock::wrlock()
{ 
	pthread_rwlock_wrlock(&_rwlock); 
}

bool xn_rwlock::try_rdlock()
{
	return 0 == pthread_rwlock_tryrdlock(&_rwlock);
}

bool xn_rwlock::try_wrlock()
{
	return 0 == pthread_rwlock_trywrlock(&_rwlock);
}

void xn_rwlock::unlock()
{ 
	pthread_rwlock_unlock(&_rwlock); 
}
