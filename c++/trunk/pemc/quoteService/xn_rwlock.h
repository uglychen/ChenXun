#ifndef XN_RWLOCK_H
#define XN_RWLOCK_H
#include <pthread.h>

class xn_rwlock
{
public:
	xn_rwlock();
	~xn_rwlock();
	
	bool init();
	
	void rdlock();
	void wrlock();
	
	bool try_rdlock();
	bool try_wrlock();
	
	void unlock();
	
private:
	//读写锁
	pthread_rwlock_t _rwlock;
};

#endif // XN_RWLOCK_H
