
### 用 std::move() 实现对象“移动” 
进行了左右值的转换，将一个左值转换成右值，从而避免内存拷贝
```c++
void setstring(std::string version)
{
    _version = std::move(version);//
}
```

### 使用 string::c_str() 获取string中的char *对象   
c_str()返回的是一个临时指针，不能对其进行操作
```c++
using namespace std;
string add = "hello!";
const string add_on = ""baby;
const char *cfirst = add.c_str();
const char *csecond = add_on.cstr();
char *copy = new char[strlen(cfirst) + strlen(csecond) + 1];
strcpy(copy, cfirst);
strcat(copy, csecond);

add = copy;
cout<<copy<<endl;
delete [] copy;  
cout<<add<<endl;
```


### sscanf读取格式化的字符串中的数据  
`sscanf(const char *buffer, const char *format, [argument] ...)`   
buffer需要操作的数据地址 
```c
char buf[512];

sscanf("123456","%s",buf);
printf("%s", buf);  //123456

sscanf("123456 ", "%4s"， buf); //1234

sscanf("")

```

### %hu %hx 
%hu代表以 unsigned short格式输出整数
%hx 代表以16进制的 输出short类型的整数
比如
printf("%hu\n",-30); 会输出 65506
printf("%hx\n",-30); 会输出FFE2


### using  
c++ 11之前的用法为限定 空间名称  
`using std::cout;`  

c++11中用于取代typedef 
using intvec = std::vector<int>;   
```c++
using func = std::function<void (int a,char *b)>;
using ptr = std::shared_ptr<session>;

```


### 构造函数的 default 和 delete
C++ 11中，当类中含有不能默认初始化的成员变量时， 可以禁止默认构造函数  
```cpp
myClass() = delete;//表示删除默认构造函数
myClass() = default; //表示默认存在构造函数
myClass(const myClass&) = delete;//删除默认拷贝构造函数
myClass & operator=(const myClass &) = delete;//删除默认拷贝构造函数
```


### lock_guard 
位于<mutex>头文件下
相比于mutex功能，lock_guard具有创建时加锁，析构时解锁功能，类似于智能指针，为了防止在线程使用Mutex加锁后异常退出导致死锁的问题，建议使用lock_guard代替mutex  
保证了在异常情况下，其他线程也能获得mutex，不会导致阻塞
```cpp
mutex tex;
int times = 100;
void main()
{
    thread t1(func1);
    thread t2(func2);
    t1.join();
    t2.join();
    return ;
}

void func1()
{
    while(times > 0)//... do something
    {
        lock_guard<mutex> lck(tex);
        //tex.lock();
        if(times < 50)
            return; //结束线程
        times -= 1;
        //tex.unlock();
    }//每次运行之后智能指针释放，则释放tex
}

void func2()
{
    while(times > 0)//... do something
    {
        lock_guard<mutex> lck(tex);
        //tex.lock();
        times -= 1;
        //tex.unlock();
    }
}
//假如func1()先运行，则lock_guard获取了锁，只有当函数运行完，自动释放了lck时，线程func2()才能运行。

```

### c++11 once_flag call_once() 
`#include<mutex>`
当某个任务只需要执行一次使用， 实际上once_flag相当于一个锁，使用它的线程都会在上面等待，只有一个线程允许执行，如果该线程抛出异常，那么从等待的线程中选择一个，重复上面的流程
- once_flag  
std::once_flag实例的状态，指示所关联的函数尚未被调用



### c++11 random_device  
只有在linux下为真随机数  
```cpp
std::random_devide rd();
std::cour<<"random "<<rd();
```