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


##### LOG日志部分的实现细节


##### Further plan 改进计划
1. 参考muduo库利用eventfd+epoll函数实现线程间同步。
2. 利用Redis保存Session的expire time实现定时器.
3. Muduo库的进一步学习。

##### Reference 参考
1. 陈硕的muduo高性能网络库
2. [Linyacool的网络库](https://github.com/linyacool/WebServer)
