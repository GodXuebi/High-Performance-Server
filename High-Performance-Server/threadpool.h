#pragma once
#include <memory>
#include <functional>
#include <condition_variable>
#include <vector>
#include <queue>
#include <pthread.h>
#include "Log/noncopyable.h"
#include "Log/Logging.h"
#include "Log/MutexLock.h"

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

typedef enum
{
    immediate_shutdown = 1,
    graceful_shutdown  = 2
} ShutDownOption;

class ThreadPool:public noncopyable
{
private:
    static pthread_mutex_t lock;
    static pthread_cond_t notify;
    static std::vector<pthread_t> threads;
    static std::queue<std::function<void()>> taskqueue;
    static int queue_size;   
    static int taskcount;//Number of current tasks in the queue
    static int threadcount;
    static int shutdown;
    static int thread_num;

public:
    static int threadpool_create(int _thread_num,int _queue_size);
    static int threadpool_add(std::function<void()>&&task);
    static int threadpool_destroy(ShutDownOption shutdown_option = graceful_shutdown);
    static int threadpool_free();
    static void *threadpool_thread(void *args);
};


