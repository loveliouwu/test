# Linux驱动开发详解笔记

### **模块加载函数** 
>Linux内核模块加载函数一般以__init标识声明，卸载函数以__exit声明，典型的模块加载卸载函数的形式如：
```C
static int __init driver_init_func(void)
{
    /*初始化代码*/
}

static void __exit driver_exit_func(void)
{
    /*释放代码*/
}

module_init(driver_init_func);
module_exit(driver_exit_func);
```
模块加载函数返回整型值，若初始化成功，应返回0，而在初始化失败时，应该返回错误码，错误码是一个负值，在<linux/errno.h>中定义。  

此外还可以通过调用`request_module(module_name)`来加载内核模块 

### **模块参数** 

我们可以用`module_param(参数名，参数类型，参数读写权限)`为模块定义一个参数，如： 
```C
static char  *book_name = "Linux驱动";
static int num = 400;
module_param(num,int,S_IRUGO);
module_param(book_name,charp,S_IRUGO);
```
在装载内核模块时，用户可以向模块传递参数，形式为`insmode 模块名 参数名=参数值`,如果不传递，参数将使用模块内定义的默认值。 

参数的类型可以是： 
`
byte short ushort int uint long ulong charp bool invbool
`
 
 模块被加载后，在`/sys/modules/`目录下将会出现以此模块明明的目录，在此目录下还将出现`parameters`目录，包含一系列以 参数名命名的文件节点，这些文件的权限就是传入module_param的参数读写权限，文件的内容为参数的值。 

 例子： 
 ```C
#include<linux/init.h>
#include<linux/modules.h>

MODULE_LICENSE("Dual BSD/GPL");

static char *book_name = "Linux Driver";
static int num = 4000;

moudle_init(...);
module_exit(...);
module_param(num,int,S_IRUGO)；
module_param(book_name,charp,S_IRUGO)；
 ```



## 模块的编译 
最简单的模块编译Makefile如下：
```C
KERN_DIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m    += demo.o
``` 
其中`-C`后指定的是Linux内核源代码的目录，而`M=`后指定的是hello.c和Makefile的目录 

如果一个模块包含多个`.c`文件，则可以通过`module-objs := 源文件.o`的方式编写：
```C
obj-m := demo.o
module-objs := file1.o  file2.o
```
`module-objs`用在多个.o文件编译的时候

## 属性和宏定义 
- 宏定义 
可变参数的宏 #define pr_debug(fmt,arg...)  printk(fmt,##arg)
- 当前函数名
__FUNCTION__ 保存函数在源码中的名字
- 特殊属性说明 
__attribute__((ATTRIBUTE)) 其中ATTRIBUTE为属性说明，如果存在多个属性，则以逗号分隔，GUNC支持noreturn、format、section、saligned、packed等十多个属性。 
其中： 
> 
unused属性作用于函数和变量，表示该函数或变量可能不会被用到，这个属性可以避免编译器产生警告信息。 
aligned属性用域变量、结构体或联合体，指定对界方式，以字节为单位，例如： 
```C 
struct example_struct{
	char a;
	int b;
	long c;
}__arrtibute__((aligned(4)))
 ```
