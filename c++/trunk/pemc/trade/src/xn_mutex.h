#ifndef XN_MUTEX_H_D517F3AF_1E11_496E_8020_5ACC5BD02FA3_
#define XN_MUTEX_H_D517F3AF_1E11_496E_8020_5ACC5BD02FA3_
#include <pthread.h>

class xn_mutex
{
public:
	xn_mutex();
	~xn_mutex();
	bool init();
	void lock();
	void unlock();
	bool try_lock();
	
private:
	//ª•≥‚¡ø
	pthread_mutex_t _mutex;
};

#endif // XN_MUTEX_H
