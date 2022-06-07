
### tasklet  
tasklet在很多方面类似内核定时器，他们始终在中断期间运行，时钟会在调度它们的同一CPU上运行，而且都接收一个unsigned long参数   
```C
#include<linux/interrupt.h>
struct tasklet_struct {
    void (*func)(unsigned long);
    unsigned long data;
}
void tasklet_init(struct tasklet_struct *t,
        void (*func)(unsigned long),
        unsigned long data
;
DECLARE_TASKLET(name, func, data);
DECLARE_TASKLET_DISABLED(name, func, data);

void tasklet_disable(struct tasklet_struct *t);
void tasklet_disable_nosync(struct tasklet_struct *t);
void tasklet_schedule(struct tasklet_struct *t);
void tasklet_hi_schedule(struct tasklet_struct *t);
void tasklet_kill(struct tasklet_struct *t);
```
- 使用方式： tasklet对于中断处理特别有用，硬件中断必须尽快处理，但大部分的数据管理可以延后到以后安全的时间执行，此时，在驱动中断处理函数中调用`tasklet_schedule`来将后续的操作放在tasklet任务中，这样能够尽快的处理硬件中断。其中中断处理程序属于上半部，tasklet中的任务属于下半部，中断不能被相同类型的中断打断，但是下半部依然可以被中断打断。   
1、如果一个任务对事件非常敏感，将其放在中断处理程序中执行；   
2、如果一个任务和硬件有关，将其放在中断处理程序中执行； 
3、如果一个任务要保证不被其他终端打断，将其放在中断处理程序中执行；   
4、其他所有任务，考虑放在下半部去执行。
内核引入tasklet机制主要是由于软中断的pending标志位也就32位，内核也没有提供通用的增加软中断的接口   
特点：tasklet不能休眠，同一个tasklet不能在两个CPU上同时运行，但是不同tasklet可能在不同CPU上同时运行。  


### 工作队列  
```C
struct workqueue_struct *create_workqueue(const char *name);
struct workqueue_struct *create_singlethread_worequeue(const char *name);
```
每个工作队列有一个或多个专用的进程，这些进程运行提交到该队列的函数，如果我们使用create_workqueue，则内核会在系统中的每个处理器上为该工作队列创建专用的线程，如果单个工作线程足够使用，那么应该使用create_singlethread_workqueue创建工作队列。   
```C
struct workqueue_struct * create_workqueue(const char *name);
struct workqueue_struct * create_singlethread_workqueue(const char *name);
//name是要声明的结构名称
//如果要在运行的时候构造work_sturct结构，可以使用下面两个宏：
INIT_WORK(struct work_struct *work, void (*function)(void *), void *data);
PREPARE_WORK(struct work_struct *work, void(*function)(void *), void *data);
//如果需要将工作提交到工作队列，则可使用下面的函数： 
int queue_work(struct workqueu_struct *queue, struct work_struct *work);
int queue_delayed_work(struct workqueue_struct *queue, struct work_struct *work, unsigned long delay); 
//它们都会将work添加到给定的queue,但是如果使用queue_delayed_work，则实际的工作至少会经过指定的jiffies之后才会被执行(即delay值)。如果工作被成功添加到队列，则上述函数的返回值为1，返回值为非零时意味着给定的work_struct结构已经等待在该队列中，从而不能两次加入到该队列。
//取消某个挂起的工作队列： 
int cancel_delayed_work(struct work_struct *work);
//释放资源  
void destory_workqueue(struct workqueue_struct *queue);
```
要向一个工作队列提交一个任务，需要填充一个work_struct结构：  `DECLARE_WORK(name, void (*function)(void *), void *data);`