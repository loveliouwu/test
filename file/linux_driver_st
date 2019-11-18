使用register_chrdev注册字符设备

注册字符设备可以使用register_chrdev函数。

    int register_chrdev (unsigned int major, const  char *name, struct file_operations*fops); 

register_chrdev函数的major参数如果等于0，则表示采用系统动态分配的主设备号。

注销字符设备可以使用unregister_chrdev函数。

    int unregister_chrdev(unsigned int major, const char *name); 

例1.3  register_chrdev注册字符设备实例

代码见光盘\src\1drivermodel\1-3register_chrdev。核心代码如下所示：

    static unsigned char simple_inc=0;  
    static unsigned char demoBuffer[256];  
    int simple_open(struct inode *inode, struct file *filp)  
    {  
        if(simple_inc>0)return -ERESTARTSYS;  
        simple_inc++;  
        return 0;  
    }  
    int simple_release(struct inode *inode, struct file *filp)  
    {  
        simple_inc--;  
        return 0;  
    }  
    ssize_t simple_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)  
    {  
        /* 把数据复制到应用程序空间 */  
        if (copy_to_user(buf,demoBuffer,count))  
        {  
           count=-EFAULT;   
        }  
        return count;  
    }  
    ssize_t simple_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)  
    {  
        /* 把数据复制到内核空间 */  
        if (copy_from_user(demoBuffer+*f_pos, buf, count))  
        {  
            count = -EFAULT;  
        }  
        return count;  
    }  
    struct file_operations simple_fops = {  
        .owner =    THIS_MODULE,  
        .read =     simple_read,  
        .write =    simple_write,  
        .open =     simple_open,  
        .release =  simple_release,  
    };  
    /*******************************************************  
                    MODULE ROUTINE  
    *******************************************************/  
    void simple_cleanup_module(void)  
    {  
        unregister_chrdev(simple_MAJOR,  "simple");   
        printk("simple_cleanup_module!\n");  
    }  
    int simple_init_module(void)  
    {  
        int ret;  
        //根据设备号与设备名注册字符设备  
        ret = register_chrdev(simple_MAJOR, "simple", &simple_fops);   
        if (ret < 0)  
        {  
            printk("Unable to register character device %d!\n",simple_MAJOR);  
            return ret;  
        }  
        return 0;  
    }  
    module_init(simple_init_module);  
    module_exit(simple_cleanup_module);  

应用程序的代码如下：

    void main(void)  
    {  
        int fd;  
        int i;  
        char data[256];  
        int retval;  
        fd=open("/dev/fgj",O_RDWR);  
        if(fd==-1)  
        {  
            perror("error open\n");  
            exit(-1);  
        }  
        printf("open /dev/fgj successfully\n");  
        //写数据  
        retval=write(fd,"fgj",3);  
        if(retval==-1)  
        {  
            perror("write error\n");  
            exit(-1);  
        }  
        //读数据  
        retval=read(fd,data,3);  
        if(retval==-1)  
        {  
            perror("read error\n");  
            exit(-1);  
        }  
        data[retval]=0;  
        printf("read successfully:%s\n",data);  
        //关闭设备  
        close(fd);  
    }  




×××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××




使用cdev_add注册字符设备

在Linux 2.6内核中的字符设备用cdev结构来描述，其定义如下：

    struct cdev   
    {  
        struct kobject kobj;  
        struct module *owner; //所属模块  
        const struct file_operations *ops; //文件操作结构  
        struct list_head list;  
        dev_t dev; //设备号，int 类型，高12位为主设备号，低20位为次设备号  
        unsigned int count;  
    };  

下面一组函数用来对cdev结构进行操作：

    struct cdev *cdev_alloc(void);//分配一个cdev  
    void cdev_init(struct cdev *, const struct file_operations *);//初始化cdev的file_operation  
    void cdev_put(struct cdev *p);// //减少使用计数  
    //注册设备，通常发生在驱动模块的加载函数中  
    int cdev_add(struct cdev *, dev_t, unsigned);   
    //注销设备，通常发生在驱动模块的卸载函数中  
    void cdev_del(struct cdev *);   

使用cdev_add注册字符设备前应该先调用register_chrdev_region或alloc_chrdev_region分配设备号。register_chrdev_region函数用于指定设备号的情况，alloc_chrdev_region函数用于动态申请设备号，系统自动返回没有占用的设备号。

    int register_chrdev_region(dev_t from, unsigned count, const char *name) ;  
    int alloc_chrdev_region(dev_t *dev,unsigned baseminor,unsigned count,const char *name); 

alloc_chrdev_region申请一个动态主设备号，并申请一系列次设备号。baseminor为起始次设备号，count为次设备号的数量。注销设备号(cdev_del)后使用unregister_chrdev_region：

    void unregister_chrdev_region(dev_t from,unsigned count) ; 

例1.4  cdev_add注册字符设备实例

代码见光盘\src\1drivermodel\1-4cdev。核心代码如下所示：

    struct file_operations simple_fops = {  
        .owner =    THIS_MODULE,  
        .read =     simple_read,  
        .write =    simple_write,  
        .open =     simple_open,  
        .release =  simple_release,  
    };  
    /*******************************************************  
                    MODULE ROUTINE  
    *******************************************************/  
    void simple_cleanup_module(void)  
    {  
        dev_t devno = MKDEV(simple_MAJOR, simple_MINOR);  
        if (simple_devices)   
        {  
            cdev_del(&simple_devices->cdev);  
            kfree(simple_devices);  
        }  
        unregister_chrdev_region(devno,1);  
    }  
    //模块初始化  
    int simple_init_module(void)  
    {  
        int result;  
        dev_t dev = 0;  
        dev = MKDEV(simple_MAJOR, simple_MINOR);  
        result = register_chrdev_region(dev, 1, "DEMO");//申请设备号  
        if (result < 0)   
        {  
            printk(KERN_WARNING "DEMO: can't get major %d\n", simple_MAJOR);  
            return result;  
        }  
        simple_devices = kmalloc(sizeof(struct simple_dev), GFP_KERNEL);  
        if (!simple_devices)  
        {  
            result = -ENOMEM;  
            goto fail;  
        }  
        memset(simple_devices, 0, sizeof(struct simple_dev));  
        //初始化设备结构  
        cdev_init(&simple_devices->cdev, &simple_fops);  
        simple_devices->cdev.owner = THIS_MODULE;  
        simple_devices->cdev.ops = &simple_fops;  
        result = cdev_add (&simple_devices->cdev, dev, 1);//添加字符设备  
        if(result)  
        {  
            printk(KERN_NOTICE "Error %d adding DEMO\n", result);  
            goto fail;  
        }  
        return 0;  
    fail:  
        simple_cleanup_module();  
        return result;  
    }  
    module_init(simple_init_module);  
    module_exit(simple_cleanup_module);  


××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××


在驱动程序中用  
	module_param(参数名，参数类型，参数读写权限)；

	例如：
		static int num = 4000;
		static char * book_num = "三国"
		module_param(num,int,S_IRUGO);
		module_param(book_num,charp,S_IRUGO);

	当使用一般加载驱动的命令
		insmod test.ko
	时，num和book_num使用驱动中定义的值
	当使用以下命令加载驱动时，这两个参数使用传递进的值
		insmod test.ko book_name='西游' num=5000
	

PS：
	驱动程序是必须加上
		MODULE_LICENSE("Dual BSD/GPL");
	否自系统会警告内核被污染

××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××××


GCC 支持复杂的宏，它使用一种不同的语法，使你可以给可变参数一个名字，如同其它参数一样，比如：

    引用
    #define debug(format, args...) fprintf(stderr, format, args)


这种定义可读性更强，也更容易描述。完整测试代码：

    引用
    #include <stdio.h>

    #define debug(format, args...) fprintf(stderr, format, args)

    int main()
    {
        char a[20] = "hello world\n";
        int i = 10;
        debug("i = %d, %s", i, a);

        return 0;
    }


运行输出：

    引用
    beyes@linux-beyes:~/C/micro> ./mic.exe 
    i = 10, hello world


但是上面的定义仍存在一点问题，如果把上面的代码改为下面的：

    引用
    #include <stdio.h>

    #define debug(format, args...) fprintf(stderr, format, args)

    int main()
    {

        debug("hello world\n");

        return 0;
    }


那么在编译时会提示以下错误：

    引用
    beyes@linux-beyes:~/C/micro> gcc -g mic.c -o mic.exe
    mic.c: In function ‘main’:
    mic.c:10: error: expected expression before ‘)’ token


提示缺少右边括号。这是因为，当宏展开后，"hello world\n" 代入 format，然而，在其后还紧跟着一个逗号，但是这个逗号后面是期望有 args 参数的，但这里却没有，所以宏不能展开完全，故而无法编译通过。那么，再改一下宏定义：

    引用
    #include <stdio.h>

    #define debug(format, args...) fprintf(stderr, format, ##args)

    int main()
    {
        debug("hello world\n");

        return 0;
    }


这时候，再编译运行及输出：

    引用
    beyes@linux-beyes:~/C/micro> gcc -g mic.c -o mic.exe
    beyes@linux-beyes:~/C/micro> ./mic.exe
    hello world


编译通过，并正常输出。上面的代码，在 fprintf() 中的 args 前面加了两个 # 号 ##。
## 号的作用是：
如果可变参数部分( args...) 被忽略或为空，那么 "##" 操作会使预处理器 (preprocessor) 去掉它前面的那个逗号。如果在调用宏时，确实提供了一些可变参数，GNU C 也会正常工作，它会把这些可变参数放在逗号的后面；如果没有提供，它就会自动去掉前面的逗号，使宏结束展开 ---- 补充完右边括号。

另外，假如按照 C99 的定义来用，改宏为：

    引用
    #define debug(format, args...) fprintf(stderr, format, ##__VA_ARGS__)


那么编译会出错：

    引用
    beyes@linux-beyes:~/C/micro> gcc -g mic.c -o mic.exe
    mic.c:3:58: warning: __VA_ARGS__ can only appear in the expansion of a C99 variadic macro
    mic.c:9:1: error: pasting "," and "__VA_ARGS__" does not give a valid preprocessing token
    mic.c: In function ‘main’:
    mic.c:9: error: ‘__VA_ARGS__’ undeclared (first use in this function)
    mic.c:9: error: (Each undeclared identifier is reported only once
    mic.c:9: error: for each function it appears in.)


原因在于，args... 和 ##__VA_ARGS__ 是不匹配的，正确的匹配应该是：

    引用
    #define debug(format, ...) fprintf(stderr, format, ##__VA_ARGS__)


注意，... 省略号对应的就是 __VA_ARGS__

一般的，定义可变参数宏的一个流行方法，形如：

    引用
    #define DEBUG(args) (printf("DEBUG: "), printf args)
    if(n != 0) DEBUG(("n is %d\n", n));


这个方法的一个缺点是，要记住一对额外的括弧。





通过编译器字符串替换注册函数：

#define show_set(value)	\
static ssize_t show_##value(struct device *dev, struct device_attribute *attr, char *buf)		\
{									\
	struct usb_interface *intf = to_usb_interface(dev);		\
	struct usb_led *led = usb_get_intfdata(intf);			\
									\
	return sprintf(buf, "%d\n", led->value);			\
}									\
static ssize_t set_##value(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)	\
{									\
	struct usb_interface *intf = to_usb_interface(dev);		\
	struct usb_led *led = usb_get_intfdata(intf);			\
	int temp = simple_strtoul(buf, NULL, 10);			\
									\
	led->value = temp;						\
	change_color(led);						\
	return count;							\
}									\
static DEVICE_ATTR(value, S_IWUGO | S_IRUGO, show_##value, set_##value);
show_set(blue);
show_set(red);
show_set(green);
