SMP  对称多处理器  紧耦合、共享存储，使用共同的系统总线，因此可以访问共同的外设和存储器



1.中断屏蔽：
	local_irq_disable()//屏蔽中断
	//临界区代码
	local_irq_enable()//开中断

	单CPU范文内避免竞态的一种简单方法，由于Linux系统的异步IO 进程调度等很多都依赖于中断，中断对于内核的运行非常重要，在屏蔽中断期间，所有的中断都无法得到处理，因此屏蔽中断不能长时间，索引临界区的代码需要尽快的执行。
	中断屏蔽只能禁止和是能本CPU内的中断，因此并不能解决SMP多CPU引发的竞态
	如果只是想禁止中断的底半部，应使用
	local_bh_disable()
	local_bh_enable()


2.原子操作：
	原子操作是指在执行过程中不会被别的代码路径所中断的操作
	整形原子操作：
		1.设置原子变量的值：
			void atomic_set(atomic_t *v, int i);设置变量值为i
			atomic_t v = ATOMIC_INIT(0)//定义原子变量v并初始化为0
		2.获取原子变量的值
			atomic_read(atomic_t *v)

3.自旋锁
	自旋锁实际上是忙等锁，当锁不可用时，CPU 一直循环执行“测试并设置”该锁直到可用而取得 该锁，CPU 在等待自旋锁时不做任何有用的工作，仅仅是等待。因此，只有在占用锁的时间极短 的情况下，使用自旋锁才是合理的。当临界区很大或有共享设备的时候，需要较长时间占用锁， 使用自旋锁会降低系统的性能。  自旋锁可能导致系统死锁。引发这个问题常见的情况是递归使用一个自旋锁，即如果一个已经 拥有某个自旋锁的 CPU 想第二次获得这个自旋锁，则该 CPU 将死锁。此外，如果进程获得自旋 锁之后再阻塞，也有可能导致死锁的发生。copy_from_user()、copy_to_user()和 kmalloc()等函数 都有可能引起阻塞，因此在自旋锁的占用期间不能调用这些函数
		//定义一个自旋锁 spinlock_t lock; spin_lock_init(&lock); 
		spin_lock (&lock) ; //获取自旋锁，保护临界区 ...//临界区 
		spin_unlock (&lock) ; //解锁


4.读写自旋锁
	rwlock
	写的时候只能由一个进程写，而读的时候可以有多个读执行单元，并且都和写不能同时进行
	rwlock_t lock;   //定义 rwlock rwlock_init(&lock); //初始化 rwlock 
 
	//读时获取锁 read_lock(&lock); ...  //临界资源 read_unlock(&lock); 
	 
	//写时获取锁 write_lock_irqsave(&lock, flags); ... //临界资源 write_unlock_irqrestore(&lock, flags); 

5.顺序锁：
	顺序锁是对写锁的一种优化
	读绝对不会被写阻塞
	写与写之间仍然是互斥的
	但是顺序锁有一个限制，它必须要求被保护的共享资源不含有指针


6.信号
在用户程序中，为了捕获信号，可以使用signal函数来设置对应信号的处理函数
	void (*signal(int signum, void (*handler))(int)(int));
	第一个参数制定信号的值，第二个参数制定针对前面信号值的处理函数，若为SIG_IGN,表示忽略该信号，若为SIG_DFL,表示采用系统默认方式处理信号，若为自定义的函数，则信号被捕获到后，该函数将被执行。
	如果signal()调用成功，它返回最后一次为信号signum绑定的处理函数handler值，失败则返回SIG_ERR。
	例子：
	{
		void sigterm_handler(int signo)
		{
			printf("Have caught sig N.O %d",signo);
		exit(0);	
		}

		int main(void)
		{
			signal(SIGINT,sigterm_handler);
			signal(SIGTERM,sigterm_handler);
			while(1);
			return 0;
		
		}

	}

