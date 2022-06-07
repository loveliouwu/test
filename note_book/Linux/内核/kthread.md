### Linux内核线程kthread  
Linux内核可以看作一个服务进程，在系统启动后由0号PID的进程创建了1号PID的init进程，1号PID创建了2号PID的`kthreadd`线程，此后一些系统进程都由2号PID来创建。   
内核线程或者叫守护进程，在操作系统中占据相当大的比例，当Linux操作系统启动以后，使用`ps -ef`查看系统中的进程，会发现很多以"d"结尾的进程名，这些进程就是内核线程。   
内核线程与普通进程间的区别在于内核线程没有独立的地址空间，它只在内核空间运行，从来不切换到用户空间去，并且和普通进程一样可以被调度，可以被抢占  

#### task_struct 结构体     
`include/linux/sched.h`  
操作系统进行进程管理是将进程的有效信息提取出来通过管理这些信息来管理进程，而所有的进程信息被存放在一个叫做进程控制块(PCB)的数据结构中。
Linux内核的进程控制块是`task_struct`结构体，主要包含了这些内容：  
**标识符：**描述本进程的唯一标识符，用来区别其他进程。   
**状态：**任务状态，退出代码，推出信号等。    
**优先级**     
**程序计数器：**程序中即将被执行的下一条指令的地址   
**内存指针：**包括程序代码和进程相关数据指针，还有其他进程共享的内存块的指针。   
**上下文数据：**进程执行时处理器的寄存器中的数据。   
**I/O状态信息：**包括显示的I/O请求，分配给进程的I/O设备和被进程使用的文件列表    
**记账信息：**可能包括处理器时间总和，使用的时钟数综合，时间限制，记账号等。    

- 定义和注释
```C
struct task_struct {
volatile long state;  //说明了该进程是否可以执行,还是可中断等信息
unsigned long flags;  //Flage 是进程号,在调用fork()时给出
int sigpending;    //进程上是否有待处理的信号
mm_segment_t addr_limit; //进程地址空间,区分内核进程与普通进程在内存存放的位置不同
                        //0-0xBFFFFFFF for user-thead
                        //0-0xFFFFFFFF for kernel-thread
//调度标志,表示该进程是否需要重新调度,若非0,则当从内核态返回到用户态,会发生调度
volatile long need_resched;
int lock_depth;  //锁深度
long nice;       //进程的基本时间片
//进程的调度策略,有三种,实时进程:SCHED_FIFO,SCHED_RR, 分时进程:SCHED_OTHER
unsigned long policy;
struct mm_struct *mm; //进程内存管理信息
int processor;
//若进程不在任何CPU上运行, cpus_runnable 的值是0，否则是1 这个值在运行队列被锁时更新
unsigned long cpus_runnable, cpus_allowed;
struct list_head run_list; //指向运行队列的指针
unsigned long sleep_time;  //进程的睡眠时间
//用于将系统中所有的进程连成一个双向循环链表, 其根是init_task
struct task_struct *next_task, *prev_task;
struct mm_struct *active_mm;
struct list_head local_pages;       //指向本地页面      
unsigned int allocation_order, nr_local_pages;
struct linux_binfmt *binfmt;  //进程所运行的可执行文件的格式
int exit_code, exit_signal;
int pdeath_signal;     //父进程终止时向子进程发送的信号
unsigned long personality;
//Linux可以运行由其他UNIX操作系统生成的符合iBCS2标准的程序
int did_exec:1; 
pid_t pid;    //进程标识符,用来代表一个进程
pid_t pgrp;   //进程组标识,表示进程所属的进程组
pid_t tty_old_pgrp;  //进程控制终端所在的组标识
pid_t session;  //进程的会话标识
pid_t tgid;
int leader;     //表示进程是否为会话主管
struct task_struct *p_opptr,*p_pptr,*p_cptr,*p_ysptr,*p_osptr;
struct list_head thread_group;   //线程链表
struct task_struct *pidhash_next; //用于将进程链入HASH表
struct task_struct **pidhash_pprev;
wait_queue_head_t wait_chldexit;  //供wait4()使用
struct completion *vfork_done;  //供vfork() 使用
unsigned long rt_priority; //实时优先级，用它计算实时进程调度时的weight值

//it_real_value，it_real_incr用于REAL定时器，单位为jiffies, 系统根据it_real_value
//设置定时器的第一个终止时间. 在定时器到期时，向进程发送SIGALRM信号，同时根据
//it_real_incr重置终止时间，it_prof_value，it_prof_incr用于Profile定时器，单位为jiffies。
//当进程运行时，不管在何种状态下，每个tick都使it_prof_value值减一，当减到0时，向进程发送
//信号SIGPROF，并根据it_prof_incr重置时间.
//it_virt_value，it_virt_value用于Virtual定时器，单位为jiffies。当进程运行时，不管在何种
//状态下，每个tick都使it_virt_value值减一当减到0时，向进程发送信号SIGVTALRM，根据
//it_virt_incr重置初值。
unsigned long it_real_value, it_prof_value, it_virt_value;
unsigned long it_real_incr, it_prof_incr, it_virt_value;
struct timer_list real_timer;   //指向实时定时器的指针
struct tms times;      //记录进程消耗的时间
unsigned long start_time;  //进程创建的时间
//记录进程在每个CPU上所消耗的用户态时间和核心态时间
long per_cpu_utime[NR_CPUS], per_cpu_stime[NR_CPUS]; 
//内存缺页和交换信息:
//min_flt, maj_flt累计进程的次缺页数（Copy on　Write页和匿名页）和主缺页数（从映射文件或交换
//设备读入的页面数）； nswap记录进程累计换出的页面数，即写到交换设备上的页面数。
//cmin_flt, cmaj_flt, cnswap记录本进程为祖先的所有子孙进程的累计次缺页数，主缺页数和换出页面数。
//在父进程回收终止的子进程时，父进程会将子进程的这些信息累计到自己结构的这些域中
unsigned long min_flt, maj_flt, nswap, cmin_flt, cmaj_flt, cnswap;
int swappable:1; //表示进程的虚拟地址空间是否允许换出
//进程认证信息
//uid,gid为运行该进程的用户的用户标识符和组标识符，通常是进程创建者的uid，gid
//euid，egid为有效uid,gid
//fsuid，fsgid为文件系统uid,gid，这两个ID号通常与有效uid,gid相等，在检查对于文件
//系统的访问权限时使用他们。
//suid，sgid为备份uid,gid
uid_t uid,euid,suid,fsuid;
gid_t gid,egid,sgid,fsgid;
int ngroups; //记录进程在多少个用户组中
gid_t groups[NGROUPS]; //记录进程所在的组
//进程的权能，分别是有效位集合，继承位集合，允许位集合
kernel_cap_t cap_effective, cap_inheritable, cap_permitted;
int keep_capabilities:1;
struct user_struct *user;
struct rlimit rlim[RLIM_NLIMITS];  //与进程相关的资源限制信息
unsigned short used_math;   //是否使用FPU
char comm[16];   //进程正在运行的可执行文件名
 //文件系统信息
int link_count, total_link_count;
//NULL if no tty 进程所在的控制终端，如果不需要控制终端，则该指针为空
struct tty_struct *tty;
unsigned int locks;
//进程间通信信息
struct sem_undo *semundo;  //进程在信号灯上的所有undo操作
struct sem_queue *semsleeping; //当进程因为信号灯操作而挂起时，他在该队列中记录等待的操作
//进程的CPU状态，切换时，要保存到停止进程的task_struct中
struct thread_struct thread;
  //文件系统信息
struct fs_struct *fs;
  //打开文件信息
struct files_struct *files;
  //信号处理函数
spinlock_t sigmask_lock;
struct signal_struct *sig; //信号处理函数
sigset_t blocked;  //进程当前要阻塞的信号，每个信号对应一位
struct sigpending pending;  //进程上是否有待处理的信号
unsigned long sas_ss_sp;
size_t sas_ss_size;
int (*notifier)(void *priv);
void *notifier_data;
sigset_t *notifier_mask;
u32 parent_exec_id;
u32 self_exec_id;

spinlock_t alloc_lock;
void *journal_info;
};
```


- 进程状态   
```C
#define TASK_RUNNING    0//进程要么正在执行，要么准备执行  

#define TASK_INTERRUPTIBLE  1//可中断的睡眠，可以通过一个信号唤醒

#define TASK_UNINTERRUPTIBLE 2//不可中断睡眠，不可以通过信号进行唤醒  

#define __TASK_STOPPED  4//进程停止执行  

#define __TASK_TRACED   8//进程被追踪  

#define EXIT_ZOMBIE 16//僵尸状态的进程，标识进程被终止，但是父进程还没有获取它的终止信息，比如进程有没有执行完等信息  

#define EXIT_DEAD   32//进程的最终状态，进程死亡  

#define TASK_DEAD   64//死亡

#define TASK_WAKEKILL 128//唤醒并杀死的进程  

#define TASK_WAKING  256//唤醒进程  
```

- 进程的唯一标识（pid）    
```C
pid_t pid;//进程的唯一标识   
pid_t tgid;//线程组的领头线程的Pid成员值  
``` 
在Linux系统中，一个线程组中的所有线程使用和该线程组的零头线程（该组中的第一个轻量级进程）相同的PID，并被存放在tgid成员中，只有线程组的领头线程的Pid成员才会被设置为与tgid相同的值，注意，`getpid（）`系统调用返回的是当前进程的tgid值而不是pid值。




#### 内核线程函数   
- kthread_run(threadfn,data,namefmt，...)  
创建并唤醒线程，其内部调用了`kthread_create`和`weak_up_process`。  
当一个线程创建后就一直运行下去，直到遇见`kthread_should_stop`或`kthread_stop`函数   
参数包括入口函数threadfn，参数data，线程名称namefmt。 



#### 内核工作组的使用   
```C
struct kthread_worker	kworker;
struct task_struct		*kworker_task;
struct kthread_work		work;
//初始化工作组
kthread_init_worker(&kworker);
//创建一个内核线程并运行kthread_worker_fn 参数是kworker 后面的参数是线程名称
kworker_task = kthread_run(kthread_worker_fn, &kworker,"%s", "xxxx");
//初始化线程
kthread_init_work(&work, kthread_work_func_t fun);
//向工作组添加一个工作，这个接口可能唤醒内核线程，执行完成后这个工作就被删除了
kthread_queue_work(kworker, &work); //old
queue_kthread_work(kworker, &work);//new
//添加一个延时内核工作
kthread_queue_delayed_work(kworker, &work,delay);
flush_kthread_work(struct kthread_work *work);//等待一个work完成
flush_kthread_worker(kworker);//等待worker中的所有work都结束
//停止内核线程，依赖内核线程中调用(kthread_should_stop)检查，否则这个接口会阻塞直到内核线程退出
kthread_stop(kworker_task)
```



#### 其他函数  
```C
//释放进程控制块内存
void free_kthread_struct(struct task_struct *k);
//将线程绑定到指定cpu上
void kthread_bind(struct task_struct *k, unsigned int cpu);
//将线程绑定到几个cpu上用掩码
void kthread_bind_mask(struct task_struct *k, const struct cpumask *mask);
//标记内核线程停止并等待线程停止
int kthread_stop(struct task_struct *k);
//检查线程是否要停止
bool kthread_should_stop(void);
//检查线程是否要park
bool kthread_should_park(void);
//睡眠使用停止模式
bool kthread_freezable_should_stop(bool *was_frozen);
//获取线程data
void *kthread_data(struct task_struct *k);
//
void *kthread_probe_data(struct task_struct *k);
//标记线程park
int kthread_park(struct task_struct *k);
//取消标记park
void kthread_unpark(struct task_struct *k);
// 等待park标记是否清除
void kthread_parkme(void);
//内核线程的管理接口
int kthreadd(void *unused);
```