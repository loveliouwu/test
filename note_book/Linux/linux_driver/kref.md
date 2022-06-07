### kref   内核中最基本的引用计数
kref的定义： 
```C
struct kref{
    atomic_t refcount;
}
//接口
void  kref_init( struct  kref *kref);
void  kref_get( struct  kref *kref);
int  kref_put( struct  kref *kref,  void  (*release) ( struct  kref *kref));
```
一旦使用kref，需要遵循以下三个规则：  
1、如果对一个kref-ed的结构体指针做非局部性拷贝（如复制另外一份做操作），特别是当将指针传递给另一个线程时，必须在传递之前调用kref_get()以增加kref-ed的结构体的引用计数`kref_get(&data->refconut);`     
**解读**：当一个包含kref的结构体指针传递给其他线程使用时，需要提前调用kref_get()增加一个引用计数，在线程使用结束后线程里调用kref_put来减少引用。这样能够保证安全
2、当完成对kref-ed结构体的使用时，必须要调用Kref_put()，`kref_put(&data->refcount,data_release);`，如果这是对结构体最后的引用，那么data_release函数将被调用  
3、如果在没有取得结构体的一个有效的指针时
，尝试去获取kref-ed结构体的引用，则必须串行地访问kref-ed结构体，这样在kref_get时不会发生kref_put，并且在kref_get期间结构体必须保持有效。   
**解读**：可以理解为如果线程里面调用kref_put，则需要保证线程启动之前调用了kref_get，否则线程中Put和get可能无法串行，那么put时数据可能无效，如：    
```C
void data_release(struct kref *kref)
{
　　struct my_data *data = container_of(kref, struct my_data, refcount);
　　kfree(data);
}

void more_data_handling(void *cb_data)
{
　　struct my_data *data = cb_data;
　　…
　　do stuff with data here
　　…
　　kref_put(&data->refcount, data_release);
}

int my_data_handler(void)
{
　　int rv = 0;
　　struct my_data *data;
　　struct task_struct *task;

　　data = kmalloc(sizeof(*data), GFP_KERNEL);
　　if (!data)
　　　　return –ENOMEM;

　　kref_init(&data->refcount);
　　kref_get(&data->refcount);
　　task = kthread_run(more_data_handling, data, “more_data_handling”);
　　if (task == ERR_PTR(-ENOMEM)) {
　　　　rv = -ENOMEM;
　　　　goto out;
　　}
　　…

　　do stuff with data here
　　…

out:
　　kref_put(&data->refcount, data_release);
　　return rv;
}

```


### kref和container_of联合使用    
一般情况下，kref_init和kref_get一起调用，这时程序可以进行内存申请等操作，当需要释放包含kref的结构体时，由于kref_init绑定的函数为kref_release(struct kref *kref)，因此，为了获取kref所在的结构体的指针，可以调用container_of来获取，这时候释放整个结构体的空间，这样使内存管理更加方便和安全：如：    
```C
static void kref_release(struct kref *kref)
{
    struct my_st *tmp = container_of(kref,struct my_st,kref);
    kfree(tmp);
}
```