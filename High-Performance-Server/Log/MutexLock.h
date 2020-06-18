#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__
#include <pthread.h>
#include <cstdio>
#include "noncopyable.h"


class MutexLock : public noncopyable
{
private:
    friend class Condition;
    pthread_mutex_t mutex;
public:
    MutexLock(){pthread_mutex_init(&mutex,NULL);}
    ~MutexLock()
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }
    void lock(){pthread_mutex_lock(&mutex);}
    void unlock(){pthread_mutex_unlock(&mutex);}
    pthread_mutex_t*get(){return &mutex;}
};

class MutexLockGuard : public noncopyable
{
private:
    MutexLock&mutex;
public:
    explicit MutexLockGuard(MutexLock&_mutex):mutex(_mutex){mutex.lock();}
    ~MutexLockGuard(){mutex.unlock();}
};
#endif