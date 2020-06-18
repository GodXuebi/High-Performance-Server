#include "CountDownLatch.h"

CountDownLatch :: CountDownLatch(int _count) : count(_count),mutex(),cond(mutex){}

void CountDownLatch :: wait()
{
    MutexLockGuard lock(mutex);
    while(count > 0){cond.wait();}
}

void CountDownLatch :: countDown()
{
    MutexLockGuard lock(mutex);
    count--;
    if(count == 0){cond.notify_all();}
}
