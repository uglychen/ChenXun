/*
 *  blockQueue.h
 *
 *  Created on: 2017-4-6
 *  Author: chenxun 275076730@qq.com
 *
 */

#ifndef _BLOCKED_QUEUE_H_
#define _BLOCKED_QUEUE_H_

#include <deque>
#include <list>
#include <queue>
#include <pthread.h>
#include <time.h>

using namespace std;

template<class T, class C = list<T> >
class BlockedQueue
{
public:
    BlockedQueue()
    {
        _queue = std::queue<T, C>();
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_cond_ne, NULL);
    }

    ~BlockedQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond_ne);
    }

    bool push(T &val, unsigned int timeout = 3000)
    {
        
        pthread_mutex_lock(&_mutex);
        
        if (_queue.empty())
        {
            _queue.push(val);
            pthread_cond_signal(&_cond_ne);
        }
        else
            _queue.push(val);

        pthread_mutex_unlock(&_mutex);

        return true;

    }
    bool pop(T &val, unsigned int timeout = 3000)
    {

        pthread_mutex_lock(&_mutex);
        if (_queue.empty())
        {
            
            struct timespec ts;
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            {
                pthread_mutex_unlock(&_mutex);
                return false;
            }
            ts.tv_sec += timeout / 1000;
            ts.tv_nsec = 0L;

            pthread_cond_timedwait(&_cond_ne, &_mutex, &ts);


            if (_queue.empty())
            {
                pthread_mutex_unlock(&_mutex);
                return false;
            }
        }

        val = _queue.front();
        _queue.pop();
        
        pthread_mutex_unlock(&_mutex);
        return true;
    }

    bool empty()
    {
        return _queue.empty();
    }
    
    unsigned int size()
    {
        return _queue.size();
    }
protected:
    queue<T, C>     _queue;
    pthread_mutex_t _mutex;
    pthread_cond_t  _cond_ne;
};

#endif /* _BLOCKED_QUEUE_H */