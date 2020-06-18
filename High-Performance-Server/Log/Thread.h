#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <functional>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <memory>
#include "noncopyable.h"
#include "CountDownLatch.h"

class Thread : public noncopyable{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&fun_,const std::string& name_ = std::string());
    ~Thread();
    void start();
    int join();
    bool ifstarted() const { return started;}
    pid_t gettid() const {return tid;}
    const std::string&getname() const { return name;}

private:
    void setDefaultName();
    bool started;
    bool joined;
    pthread_t pthreadId;
    pid_t tid;
    ThreadFunc func;
    std::string name;
    CountDownLatch latch;
};

#endif