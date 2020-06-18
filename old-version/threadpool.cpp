#include "threadpool.h"

pthread_mutex_t ThreadPool::lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::notify = PTHREAD_COND_INITIALIZER;
std::vector<pthread_t> ThreadPool::threads;
std::queue<std::function<void()>> ThreadPool::taskqueue;
int ThreadPool::queue_size = 0;
int ThreadPool::shutdown = 0;
int ThreadPool::taskcount;//Number of current tasks in the queue
int ThreadPool::threadcount;
int ThreadPool::thread_num;



int ThreadPool::threadpool_create(int _thread_num, int _queue_size)
{
    bool err=false;
    do
    {
        if(_thread_num <= 0 || _thread_num > MAX_THREADS || _queue_size <= 0 || _queue_size > MAX_QUEUE) 
        {
            _thread_num = 4;
            _queue_size = 1024;
        }

        queue_size = _queue_size;
        thread_num = _thread_num;
        taskcount = threadcount = 0;
        shutdown = 0;
        //set the number of threads in the threadpool and the maximum number of tasks in the queue
        threads.resize(thread_num);

        for(int i=0;i<thread_num;i++)
        {//threadpool_thread: void *threadpool_thread(void *args);
            if(pthread_create(&threads[i], NULL, threadpool_thread, (void*)(0)) != 0) 
            {
                //threadpool_destroy(pool, 0);
                return -1;
            }
            threadcount ++;
        }
    } while (false);

    if (err) 
    {
        //threadpool_free(pool);
        return -1;
    }
    return 0;
}

int ThreadPool::threadpool_add(std::function<void()>&&fun)
{//robbin method for distribution of tasks
    int modcount,err = 0;
    if(pthread_mutex_lock(&lock) != 0)
        return THREADPOOL_LOCK_FAILURE;
    //add the task to the queue --> lock
    do
    {
        if(taskcount == queue_size) //
        {
            err = THREADPOOL_QUEUE_FULL;
            break;
        }
        if(shutdown)
        {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        taskqueue.emplace(fun);
        taskcount++;

        if(pthread_cond_signal(&notify) != 0) 
        {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }

    } while (false); 

    if(pthread_mutex_unlock(&lock) != 0)
        err = THREADPOOL_LOCK_FAILURE;    
 

    return err;
}

int ThreadPool::threadpool_destroy(ShutDownOption shutdown_option) 
{
    printf("Thread pool destroy !\n");
    int i, err = 0;

    if(pthread_mutex_lock(&lock) != 0) 
    {
        return THREADPOOL_LOCK_FAILURE;
    }
    do 
    {
        if(shutdown) {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        shutdown = shutdown_option;

        if((pthread_cond_broadcast(&notify) != 0) ||
           (pthread_mutex_unlock(&lock) != 0)) {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }

        for(i = 0; i < thread_num; ++i)
        {
            if(pthread_join(threads[i], NULL) != 0)
            {
                err = THREADPOOL_THREAD_FAILURE;
            }
        }
    } while(false);

    if(!err) 
    {
        threadpool_free();
    }
    return err;
}

int ThreadPool::threadpool_free()
{
    if(threadcount > 0)
        return -1;
    pthread_mutex_lock(&lock);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&notify);
    return 0;
}

void *ThreadPool::threadpool_thread(void *args)
{
    while (true)
    {
        std::function<void()> task;
        pthread_mutex_lock(&lock);
        while((taskcount == 0) && (!shutdown))  //the task queue is empty and threadpool is still open
        {
            pthread_cond_wait(&notify, &lock); //wait for the wakeup
        }
        if((shutdown == immediate_shutdown) ||
           ((shutdown == graceful_shutdown) && (taskcount == 0)))
        {
            break;
        }
        task=taskqueue.front();
        taskqueue.pop();
        --taskcount;
        pthread_mutex_unlock(&lock);
        task();
    }
    --threadcount;
    pthread_mutex_unlock(&lock);
    printf("This threadpool thread finishs!\n");
    pthread_exit(NULL);
    return(NULL);
}





