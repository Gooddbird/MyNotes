## 1.编译出现 undefined reference to 问题
编译charon项目时，出现undefined reference to charon::TimerManager...问题。 
```
[ 55%] Linking CXX executable ../bin/test_io_scheduler
/usr/local/bin/cmake -E cmake_link_script CMakeFiles/test_io_scheduler.dir/link.txt --verbose=1
/usr/bin/c++   -rdynamic -O2 -g -std=c++11 -lpthread -Wall -Wno-deprecated -Werror -Wno-dev -Wno-unused-function  -rdynamic CMakeFiles/test_io_scheduler.dir/tests/test_io_scheduler.cc.o  -o ../bin/test_io_scheduler -Wl,-rpath,/home/hd/workspace/charon/lib ../lib/libcharon.so 
../lib/libcharon.so: undefined reference to `typeinfo for charon::TimerManager'
../lib/libcharon.so: undefined reference to `charon::TimerManager::TimerManager()'
../lib/libcharon.so: undefined reference to `charon::TimerManager::~TimerManager()'
```
分析上面可知 libcharon.so文件里面找不到TimerManager的定义，初次检查代码没发现问题。

由于TimerManager类在 timer.cc/h 文件里，编译时生成临时库timer.cc.o
```
[ 33%] Building CXX object CMakeFiles/charon.dir/charon/timer.cc.o
/usr/bin/c++  -Dcharon_EXPORTS  -rdynamic -O2 -g -std=c++11 -lpthread -Wall -Wno-deprecated -Werror -Wno-dev -Wno-unused-function -fPIC   -o CMakeFiles/charon.dir/charon/timer.cc.o -c /home/hd/workspace/charon/charon/timer.cc

```
去到 CMakeFiles/charon.dir/charon/timer.cc.o 目录下找到这个o文件，现在问题是查看 TimerManager定义是否在这个文件里。

使用 nm 命令可以帮助我们查看, 注意 -C选项，能帮我们看到函数名字修饰之前的名字
```
[hd@localhost charon]$ nm -C timer.cc.o | grep TimerManager
0000000000000780 T charon::TimerManager::insertTimer(std::shared_ptr<charon::Timer>, charon::WriteScopedLockImpl<charon::RWMutex>&)
00000000000001a0 T charon::TimerManager::getNextTimer()
0000000000000f40 T charon::TimerManager::listExpiredCb(std::vector<std::function<void ()>, std::allocator<std::function<void ()> > >&)
0000000000000db0 T charon::TimerManager::addConditionTimer(unsigned long, std::function<void ()>, std::weak_ptr<void>, bool)
0000000000000170 T charon::TimerManager::empty()
0000000000000b00 T charon::TimerManager::addTimer(unsigned long, std::function<void ()>, bool)
00000000000002d0 T charon::Timer::Timer(unsigned long, std::function<void ()>, bool, charon::TimerManager*)
00000000000002d0 T charon::Timer::Timer(unsigned long, std::function<void ()>, bool, charon::TimerManager*)
```
可以看到缺少了两个重要函数TimerManager::TimerManager() 和 TimerManager::~TimerManager()

再次查看代码看到只声明了构造和析构函数，未在.cc文件实现。修改后问题解决。

总结： 使用 nm命令可以看到文件里面的各个符号，帮助排查问题。

## 2.thread_local变量和全局变量导致的段错误

封装协程类Coroutine是，定义了一个变量作为主协程的指针，方便来回切换。

```
// 当前线程主协程   
static thread_local Coroutine::ptr t_MainCoroutine = nullptr;
```
当去掉 thread_local关键字，把这个作为一个全局变量时候，执行时出现了段错误。

排查后发现，协程的析构函数在析构时会打印日志，日志中某个变量为线程名称，而这个线程名称是一个thread_local变量。
```
static thread_local std::string t_thread_name = "default";
```
由于t_MainCoroutine是一个全局变量，它在整个程序main函数执行完后才析构，此时打印日志，转而去获取线程名称这个thread_local变量，但此时由于程序都结束了，thread_local变量会早于全局变量销毁，因此就触发了段错误.