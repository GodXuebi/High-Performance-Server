#ifndef __COUNTDOWNLATCH_H__
#define __COUNTDOWNLATCH_H__
#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

class CountDownLatch : public noncopyable
{
private:
    int count;
    mutable MutexLock mutex;
    Condition cond;

public:
    explicit CountDownLatch(int _count);
    void wait();
    void countDown();
};




#endif