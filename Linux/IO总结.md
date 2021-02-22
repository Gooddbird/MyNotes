# I/O复用

- I/O复用：内核一旦发现一个或多个I/O条件准备就绪，他就通知进程。
- 当客户处理多个描述符时必须复用。
- 当一个TCP服务器既要处理监听套接字，又要处理已连接套接字。
- 一个服务器既要处理TCP，又要处理UDP。
- 一个服务器要处理多个协议或者多个服务。
- https://blog.csdn.net/sqsltr/article/details/92762279
## UNIX五种I/O模型


- IO分为两个阶段
1. 内核从设备读取数据
2. 内核将数据复制回用户空间

##### 五种IO模型
- (1) 阻塞式 ：第一阶段和第二阶段均为阻塞
- (2) 非阻塞式 ： 第一阶段不阻塞，如果IO没有准备好会立即返回错误，第二阶段阻塞。
- (3) I/O复用
- (4) 信号驱动I/O
- (5) 异步I/O ：第一阶段和第二阶段都不阻塞，调用后会等待第一阶段和第二阶段全部完成来来通知调用者，在这个时间里面调用者可以做其他的事情。

## 管道的阻塞和非阻塞
![](images/pipe.jpg)

## select函数

```
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfpl, fd_set *readset, fd_set *writeset, fd_set *exceptset, 
            const struct timeval * timeout);
//若有就绪描述符返回其数目，超时返回0，出错-1
struct timeval {
    long tv_sec;
    long tv_usec;
};

```
- 一直等待：timeout设为NULL，则一直等待到一个描述符就绪为止。
- 等待一段固定时间：在有一个描述符就绪时返回，但不超过timeout。
- 不等待：检查描述符后立刻返回，timeout设为0。
- 前两种情况等待通常被进程在等待期间的捕获到信号中断，并从信号处理函数返回。
- timeout设为const代表他不会被更新。
```
void FD_ZERO(fd_set *fdset);            //清零
void FD_SET(int fd, fd_set *fdset);     //打开fd
void FD_CLR(int fd, fd_set *fdset);     //关闭fd
void FD_ISSET(int fd, fd_set *fdset);   //检查fd是否打开
```
- maxfpl为测定描述符总个数，它的值是最大描述符加1(因为描述符从0开始)。
- select函数返回时将指定哪些描述符已经就绪。描述符集内任何未就绪的描述符对应位返回时清0，因此每次重新调用select时要再次把所关心的位置1。

## 描述符就绪条件
- 注意当套接字上有错误是，他被select标记为即可读又可写。

### 套接字可读
- 该套接字接收缓冲区的数据字节数大于等于套接字接收缓冲区低水位标记。对该套接字读不会阻塞并返回读入的字节数。使用SO_RCVLOWAT套接字选项设置低水位标记，默认为1.
- 该套接字的读半步关闭（即收到了FIN的TCP连接）。对该套接字读不阻塞且返回0（即EOF）.
- 已完成连接数不为0的监听套接字，对该套接字accept通常不会阻塞。
- 套接字上有一个错误待处理。读不阻塞返回-1（即返回一个错误），同时把error设置为错误确切条件。这些错误可以指定SO_ERROR套接字选项调用getsockopt获取并清除。


### 套接字可写
- 该套接字发送缓冲区的可用空间字节数大于等于发送缓冲区低水位标记，且该套接字已连接（UDP可不需要连接）。若将该套接字设为非阻塞，写操作将返回正值（如传输层接受的字节数）。可以使用SO_SNDLOWAT设置低水位标记线，默认2048.
- 该连接的写半步关闭，对该套接字写将产生SIGPIPE信号。
- 使用非阻塞connect的套接字已建立连接，或者connect已经失败告终。
- 其上有错误待处理。写操作不阻塞返回-1，同读。
### 对于UDP只要发送低水位标记低于发送缓冲区，就可写。
## shutdown函数

- close把描述符的引用计数-1，仅在为0时才关闭套接字。而shutdown不论计数多少都可以直接激发TCP的正常终止序列。
- close终止读和写两个方向的数据传送。
```
#include <sys/socket.h>
int shutdown(int sockfd, int howto);
```
- howto如下：
- SHUT_RD（0） 关闭连接的读这一半————套接字中不再有数据可接收，缓冲区数据被丢弃。进程不能再对该套接字读，对该套接字接收的来自对端的数据都被确认后悄然丢弃。
- SHUT_WR（1） 关闭写这一半————对于TCP，这称为半关闭。当前留在缓冲区的数据将被发送掉，之后TCP连接正常终止。进程不能再对该套接字写。
- SHUT_RDWR（2） 关闭读写，这与依次调用以上两个等效。

## poll函数

```
#include <poll.h>

struct pollfd {
    int fd;
    short events;       //测试条件
    short revents;      //上述测试条件的状态
};
//若有就绪描述符返回其数目 超时返回0 失败-1
int poll(struct pollfd *fdarray, unsigned long nfds, int timeout);
```
- poll识别三种数据：普通 优先级带 高优先级
- 以下条件poll返回特定的revent</br>
  1. TCP带外数据被认为是优先级带数据
  2. TCP的读半部关闭（如收到对方的FIN），也被认为是普通数据，随后读将返回0。
  3. TCP连接错误被认为是普通数据，返回-1。
  4. 监听套接字上有新的连接被认为是普通数据（大多数情况），也可认为是优先级数据。
  5. 非阻塞式connect的完成被认为是相应套接字可写。
- 结构数组中元素个数由nfds参数指定。
- timeout参数指定poll返回前等待多少时间。指向一个毫秒数的正值。可能取值：</br>
  1. INFTIM 永远等待，常定义于<sys/stropts.h>头文件中
  2. 0 立即返回，不阻塞进程
  3. 大于0 等待指定毫秒数

## epoll
- 相比select优点:
    1. 无需针对所有文件描述符循环检查某些描述符变化。
    2. 无需每次传递监视对象信息。


- epoll_create:创建空间保持需要监听的描述符
- epoll_ctl: 注册或注销文件描述符
- epoll_wait：功能相当于select
- epoll通过以下结构把发生变化的了的描述符集中到一起：
```
typedef uoion epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;

struct epoll_event {
    __uint32_t events;
    epoll_data_t data;
};
```
- epoll_event的events成员有如下取值：
 1. EPOLLIN: 需要读的情况 0X1
 2. EPOLLOUT: 输出缓存为空 需要立刻发送数据 0X4
 3. EPOLLPRI: 收到OOB数据
 4. EPOLLRDHUP: 断开连接或者半关闭情况下 常用于边缘触发模式
 5. EPOLLERR: 出错情况
 6. EPOLLLET：以边缘触发方式得到事件通知
 7. EPOLLONESHOT: 发生一次事件后相应文件描述符不在收到事件通知 需要设置epoll_ctl第二个参数为EPOLL_CTL_MOD 再次设置事件
 

##### epoll_create
- 创建一个epoll句柄，用于区分不同的epoll例程
```
#include <sys/epoll.h>

int epoll_create(int size);
//成功返回epoll文件描述符 此文件描述符用于区分epoll例程 终止后也需要调用close 失败返回-1


```

##### epoll_ctl
- 用于增加 删除 更改事件。
```
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

// 向epoll例程epfd中对文件描述符fd执行op操作 目的是监视event中的事件
// epoll_ctl(A, EPOLL_CTL_ADD, B, C); 在A例程中注册描述符B 监视C中的事件
// epoll_ctl(A, EPOLL_CTL_DEL, B. NULL); 在A例程中删除文件描述符B 即不再对B这个描述符监视
```
- epfd：即是epoll_create返回的文件描述符
- op：用于指定监视对象的添加 删除等，如下：
    1. EPOLL_CTL_ADD 注册文件描述符
    2. EPOLL_CTL_DEL 从epoll例程中删除文件描述符
    3. EPOLL_CTL_MOD 更改已注册描述符的关注对象发生情况
- 使用流程：
```
struct epoll_event event;
event.events = EPOLLIN;
event.data.fd = sockfd;
epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
```
##### epoll_wait
- 监听注册的epoll事件
```
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);

```
- 成功时返回值代表已经准备好的套接字个数 注意只返回已经发生事件了呃套接字 而不是注册的所有套接字。出错返回-1
- events参数返回时会存储所有准备好的事件
- timeout参数为最大超时时间 若到达这个时间epoll_wait还是阻塞的话则会使其返回。 当timeout设置为-1代表epoll_wait会一直阻塞直至有事件发生返回。timeout设置为0代表epoll_wait不论成功与否都会会立即返回。

#### epoll的水平触发(LT)和边缘触发(ET)

- epoll默认使用条件触发方式。
#### 水平触发LT
- epoll原理详解
- https://cloud.tencent.com/developer/article/1005481
#### epoll惊群效应
多个线程(或进程)同时调用epoll_wait等待套接字就绪，当套接字就绪时候，由于操作系统无法决定由哪一个线程(或进程)来执行，因此就会环形所有的线程，其中一个线程(或进程)得到就绪套接字，其余线程失败，错误码为EAGAIN。
解决方法：

- 多线程：不建议多个线程同时调用epoll_wait等待，只需要一个线程来等待即可。
- 多进程：在同一时刻，永远都只有一个子进程在监听的socket上epoll_wait，其做法是，创建一个全局的pthread_mutex_t，在子进程进行epoll_wait前，则先获取锁。

#### LT下当socket可写时，会不停的触发socket可写的事件，如何处理？

- 开始不把socket加入epoll，需要向socket写数据的时候，直接调用write或者send发送数据。如果返回EAGAIN(注意为非阻塞IO)，把socket加入epoll，在epoll的驱动下写数据，全部数据发送完毕后，再移出epoll。这种方式的优点是：数据不多的时候可以避免epoll的事件处理，提高效率。