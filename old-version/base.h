#pragma once
#include <functional>
class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};

template<typename F,typename...Args>
std::function<void()> mybind(F&&f,Args&&...args);


class MutexLock: noncopyable
{
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex, NULL);
    }
    ~MutexLock()
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex);
    }
private:
    pthread_mutex_t mutex;
};


class MutexLockGuard: noncopyable
{
public:
    explicit MutexLockGuard(MutexLock &_mutex):
    mutex(_mutex)
    {
        mutex.lock();
    }
    ~MutexLockGuard()
    {
        mutex.unlock();
    }
private:
    MutexLock &mutex;
};
