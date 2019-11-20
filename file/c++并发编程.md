#C++并发编程学习笔记

####参考资料
>
	https://chenxiaowei.gitbook.io/cpp_concurrency_in_action/1.0-chinese/1.1-chinese
	
-- -
#### 何为并发
真正的硬件实现的并行（多处理器）
单核机器的任务切换实现并行
-- -
#### 多线程并发和多进程并发的优劣
- 多进程并发：
	- 通信方式：信号、管道、套接字、文件等等，缺点是速度慢，开销大
- 多线程并发：
	- 所有线程共享地址空间，不便于建立和管理。
	- 开销小于多进程并发，总体性能略强，节省了进程互相切换的消耗
-- -
####何时使用并发
- 应考虑因素
	- 提高性能，平衡各步骤效率
	- 开发难易程度，程序复杂度和稳定性
	- 对资源的消耗，对时效的考虑。
-- -
###C++库的多线程支持
- ####简单demo
```
#include<iostream>
#include<thread>
using namespace std;
void hello()
{
	cout<<"hello world"<<endl;
}
void main(int argc, char *argv[])
{
	std::thread t(hello)
	t.join();	
}
	
```
标准C++库中对多线程支持的声明头文件`<thread>`，声明了管理线程的函数和类，而保护共享数据 的函数在其他头文件中声明
-- -
- ####后台运行线程
使用`detach()`会让线程在后台运行，这就意味着主线程不能与之产生直接交互，也就是说不对等待这个线程结束，分离线程的确在后台运行，所以分离线程不能被加入。
```
std::thread t(do_background_work);
t.detach();
assert(!t.joinable());
```
当一个线程被detach后，其他线程无法再调用`pthread_join()`同步，对已经分离的线程请求`pthread_join()`将返回错误。
并且，`pthread_join()`仅允许一个线程使用。

传递参数可以是指针、变量、引用
当传递的参数是引用时可以有两种方式：
以`string`为例
```
1.string const& str;//传递的参数不能被改变，因此线程中不用改变str
2.string & str1;//传递的参数改变，并且改变都会反映到调用方。但是传递参数时需要ref修饰
thrad t(str,ref(str1));//如果不用ref修饰str1则编译不过，`ref()`返回一个reference_wrapper类型，这个类的源码大概的意思就是维持一个指针，并且重载操作符

``` 
	
```
main()
{
	char b = 'b';
	string str = "lyp";
	std::thread tt(thread_hello,1,'a',&b,str,ref(str));
}
void thread_hello(int i,char a,char *b,string const& str, string &str1)
{
    cout<<"hello world--"<<i<<a<<*b<<str<<str1<<endl;
}

```
-- -

- ####转移线程所有权`move()`
当线程所有权交出之后，将不能在调用`join`,`joinable`,`detach`等操作
```
thread m(thread_move_1);
thread mm(thread_move_2);
thread mmm;
mmm = std::move(m);//转移所有权
mmm = std::move(mm);//当运行到这一步时程序退出，因为试图将两个线程的所有权交给同一个thread，程序自动退出。
```

- ####获取系统能同时并发在一个线程中的线程数量
	一般多核系统中返回值可以是CPU核心的数量，当系统信息无法获取时函数也会返回0.
	
-- -

- ####获取线程ID
使用`thread`成员函数`get_id()`获取当前线程的id。
```
int main(int argc,char *argv[])
{
	cout<<this_thread::get_id()<<endl
	thread t(thread_test);
	thread::id thread_id_num;
	thread_id_num = t.get_id();
}

void thread_test()
{
	cout<<"thread"<<endl;
}
```
	
-- -

- ####线程同步
当一个线程等待另一个线程完成任务时，它会有很多选择：
	1.可以持续检查共享数据标志（用于做保护工作的互斥量），知道另一个线程完成工作时对这个标志进行重设，但是这是一种浪费，线程消耗宝贵的执行时间
	2.线程等待一段时间再检查，使用`std::this_thread::sleep_for()`进行周期性的间歇。
	3.优先的选择，使用标准库中提供的工具去等待事件的发生，通过另一线程触发等待事件的机制是最基本的唤醒方式，称为条件变量。
>
	C++标准库对条件变量有两套实现
	`std::condition_variable`和`std::condition_variable_any`，都包含在`<condition_variable>`头文件中的声明中，两者都需要与一个互斥量一起才能工作，前者仅限于与`std::mutex`一起工作，而后者可以和任何满足最低标准的互斥量一起工作，从而加上`_any`的后缀。
```
this_thread::sleep_for(std::chrono::milliseconds(1000));
```
-- -

- ####C++11的`lock_guard`和`unique_lock`的简单实用
`lock_guard`通常用来管理一个`std::mutex`类型的对象，通过定义一个`lock_guard`对象来管理`std::mutex`的上锁和解锁。注意，`lock_guard`并不管理`std::mutex`对象的声明周期，也就是说在使用`lock_guard`的过程中，如果`std::mutex`的对象被释放了，那么在`lock_guard`析构的时候进行解锁就会出现空指针错误。
`unique_lock`和`lock_guard`一样，但是它的使用更加灵活。
简单的使用互斥量首选`lock_guard`就可以，如果需要添加限制条件则使用`unique_lock`。
