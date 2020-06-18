#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include "CurrentThead.h"
#include <iostream>

namespace CurrentThread
{
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";    
}

pid_t gettid() {return static_cast<pid_t>(syscall(SYS_gettid));}

void CurrentThread::cacheTid()
{
    if(t_cachedTid == 0)
    {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
}

struct ThreadData{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

  ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid,
             CountDownLatch* latch): func_(func), name_(name), tid_(tid), latch_(latch) {}

  void runInThread()
  {
    *tid_ = CurrentThread::tid();
    tid_ = NULL; 
    latch_->countDown(); //wakeup the outside start function
    latch_ = NULL;

    CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
    prctl(PR_SET_NAME, CurrentThread::t_threadName);

    func_();
    CurrentThread::t_threadName = "finished";
  }
};


void* startThread(void*obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

Thread::Thread(const ThreadFunc&func_,const std::string&name_):started(false),joined(false),
pthreadId(0),tid(0),func(func_),name(name_),latch(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started && !joined) pthread_detach(pthreadId);
}

void Thread::setDefaultName()
{
    if(name.empty())
        name = "Thread";
}

void Thread::start()
{
    assert(!started);
    started = true;
    ThreadData* data = new ThreadData(func,name,&tid, &latch); //latch is important
    if(pthread_create(&pthreadId,NULL,&startThread,data))
    {
        started = false;
        delete data;
    }
    else
    {
        latch.wait();
        assert(tid > 0);
    }
}

int Thread::join()
{
    assert(started);
    assert(!joined);
    joined = true;
    return pthread_join(pthreadId,NULL);
}


