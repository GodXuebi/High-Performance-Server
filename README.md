# High Performance Server 高性能服务器实现

##### Introduction 简介
本项目利用C++编写了Web服务器，能够解析Get和Post请求，支持HTTP1.0和HTTP1.1长连接。添加了日志功能。

##### Environment 实现环境
1. 编译器 complier：g++
2. 操作系统 Operating system: Ubuntus


##### Model 模型
1. 采用Reactor+NIO+线程池机制，主线程通过消息队列模式分发任务(新的连接请求)。

##### Details 实现细节
1. 利用小根堆实现了定时器功能，控制HTTP连接的释放。
2. 通过主线程开启线程池，主线程负责监听和分发任务,线程池中的子线程负责解析HTTP内容并且发送回送报文。充分利用多核CPU的性能，避免了频繁创建和销毁的花销。分发任务采用消息队列机制。
3. 利用主线程与单个子线程配合实现了日志记录功能，实现过程中使用了双缓冲区技术，每个缓冲区包含两个子缓冲区(first buffer 和 next buffer)。当first buffer写满的时候，数据向next buffer写入。利用条件量condition_wait机制，当wait等待时间超时或者next buffer被使用导致触发condition的notify函数后，交换双缓冲区。负责被写入的缓冲区重新变为空(原来的备用缓冲区), 另一个缓冲区(原来的写入缓冲区)的数据被写入文件。
4. 利用状态机解析HTTP请求。
5. 对于多个线程共享的只读数据，采用thread_once替代了单例模式。
6. 在epoll的两种模式ET(Edge Trigger)和(Level Trigger)中选择了Edge Trigger边缘触发模式。意味着，我们需要在一次性地处理epoll_wait返回的fd的数据。


##### 连接测试工具 WebBench

##### IO部分的实现细节
1. Epoll类实现了epoll_init(利用epoll_create初始化),epoll_add(利用epoll_ctl的EPOLL_CTL_ADD增添监听事件),epoll_mod(利用epoll_ctl的EPOLL_CTL_MOD改变监听事件状态),epoll_del(利用epoll_ctl的EPOLL_CTL_DEL删除事件)，my_epoll_wait(利用epoll_wait)，处理活跃的事件，如果是listen_fd活跃，则调用accept函数获得新的fd，并调用epoll_add把新的fd添加进epoll中，创建对应的RequestData，设置NON_BLOCK, 绑定计时器，Push进消息队列。如果是RequestData，则与Timer解除绑定，并为准备被主线程分发事件。
2. Timer类实现了计时器功能，每个新的连接都会绑定一个计时器，实现判断连接是否过期功能，过期时, Timer的析构函数会利用epoll_ctl的Del功能取消对该端口的继续监听，防止利用TCP三次握手机制导致的洪水攻击(是否调用epoll_del全交给～time_node()决定，充分表现利用计时timer作为判断标准)。同时RequestData类与Timer类使用了设计模式的Oberver模式，当主线程分发RequestData时，通知向对应的Timer解绑该RequestData。
3. TimerManager利用优先队列实现对Timer的管理，值得注意的是，Timer并不是过期的时候马上删除，而是等到程序运行到相应步骤TimerManager::handle_expired_event()的时候，才对过期或者设置为deleted状态的进行删除pop处理。意味着，设定的失效时间只是真正被删除时间的下限，这样的好处时，增加了一定范围的时间容忍区间，当Timers失效(未被真正删除)，该端口来了新的数据，对应Timers被重新激活，省去了一次delete和一次new的时间。另外对TimerManager的优先队列的访问需要加锁以保证线程安全。
4. Threadpool类实现了线程池机制，主线程通过ThreadPool::threadpool_add函数添加任务，添加时候需要加锁，最后调用notify，子线程通过ThreadPool::threadpool_thread循环，从队列中获取任务。
5. RequestData类实现对HTTP数据的解析与处理，以及可以实现HTTP报文回送功能。
6. 对于Keep_alive的RequestData，在对HTTP数据解析与处理完的时，再次调用epoll_add把相应的fd放进epoll中，注意epoll_ctl是线程安全的。

##### LOG日志部分的实现细节(借鉴Ref-2)
与Log相关的类包括FileUtil、LogFile、AsyncLogging、LogStream、Logging。
其中前4个类每一个类都含有一个append函数，Log的设计也是主要围绕这个append函数展开的。

1. FileUtil是最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭文件，底层使用了标准IO，设置了文件的写入缓冲区，该append函数直接向文件写。
2. LogFile进一步封装了FileUtil(LogFile中带有FileUtil指针)，复制管理FileUtil的写入文件描述符，同时在该类利用了锁机制，保证线程安全。设置了一个循环次数，每达到指定次数，就flush一次。
3. AsyncLogging是核心，它负责启动一个log线程，专门用来将log写入LogFile，应用了“双缓冲技术”。交换双缓冲区用到了condition+lock的配合，避免主线程与Log线程竞用复制写入的缓冲区。交换后，log线程继续负责(定时到或被填满时)将缓冲区中的数据写入LogFile中。总的来说，通过AsyncLogging实现异步处理功能。本框架中利用了thread_once保证了AsynLogging只有一个实例
4. LogStream主要用来格式化输出，重载了<<运算符，同时也有自己的一块缓冲区，这里缓冲区的存在是为了缓存一行，把多个<<的结果连成一块。
5. Logging是对外接口，Logging类内涵一个LogStream对象，主要是为了每次打log的时候在log之前和之后加上固定的格式化的信息，比如打log的行、
文件名等信息。

##### Further plan 改进计划
1. 参考muduo库利用eventfd+epoll函数实现线程间同步。
2. 利用Redis保存Session的expire time实现定时器.
3. Muduo库的进一步学习。

##### Reference 参考
1. 陈硕的muduo高性能网络库
2. [Linyacool的网络库](https://github.com/linyacool/WebServer)
