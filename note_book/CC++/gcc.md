# 编译原理
- 语言处理流程
	- 预处理
	- 编译
	- 汇编
	- 链接




### 多文件编译
每个c文件单独编译成.o可以不用提供头文件，其他文件使用其函数时时只要声明了函数接口，链接工程的时候编译器会自动链接函数接口，


### -Wl,-rpath={LIB_PATH}  
用于指定动态库链接库的位置 
```makefile
LIB_PATH=$(shell pwd)/
gcc -g *.c -o a.out -L . -Wl,-rpaht=$(LIB_PATH)
``` 


### -c  
选项`-c`会明确指示GCC去编译源代码，如果不加`-o`参数则默认输出文件名和输入文件名一样  
```shell 
gcc -c hello.c  
gcc -c hello.c -o hello_newname.o

gcc hello_newname.o -o a.out
```


### -E 预处理  
只进行预处理，并把结果显示在标准输出上。加上-o选项可以将预处理后的代码定位到某个文件上
```shell 
gcc -E main.c
gcc -E main.c -o main.i  

gcc main.i -o a.out
```


### 创建静态库   
静态库是一些`.o`文件的集合，它们的编译程序按照通常的方式生成，最后通过`ar`合并为一个文件。    
```
gcc -c test1.c test2.c 
ar -r libtest.a test1.o test2.o

//使用静态库
gcc main.c libtest.a -o main  
gcc main.c -ltest -o main
```  


### 创建共享库    
共享库是目标文件的集合，但是这些目标文件是由编译程序按照特殊方式生成的，对象模块的每个地址(变量引用和函数调用)都是相对地址，不是绝对地址，因此允许再运行程序的时候，可以动态加载和执行共享模块。
```
gcc -c -fpic test1.c test2.c  
//-fpic使得输出的对象模块是按照可重定位地址方式生成的，缩写pic代表(position independent code)   
gcc -shared test1.o test2.o -o hello.so
//后缀.so告诉GCC该目标文件是要连接到共享库的
``` 

### -x 重写命名约定  
如果需要自己命名C语言的源文件文件名，而不是使用.c作后缀，那么可通过使用-x为该语言重写默认规则  
```
gcc -xc hello.jxxj -o hello.out
```

### 共享库   
共享库位置要求：   
1、环境变量`LD_LIBRARY_PATH`列出的所有用分号分隔的目录;
2、文件/etc/ld.so.cache中找到的库的列表;
3、目录/lib;
4、目录/usr/lib;


### 列举目标文件中的符号名   
工具nm用来列出目标文件、静态文档库或动态库定义的符号，如果命令没有指定文件名，会假设使用a.out。


### strip 去除目标文件中的无用信息   
工具strip命令减少XCOFF对象文件中有选择的去除行号信息、重定位信息、调试段、typchk段、注释段、文件头以及所有或部分符号表，一旦使用该命令，则很难调试文件的符号，因此，通常应该只在已经调试和测试过的生成模块上使用strip命令，使用该命令减小对象文件所需的存储量开销。  
strip的默认选项会去除.symbol节的内容和.debug节的内容，因此尽量只对可执行文件执行strip而不要对静态库或动态库等目标文件strip。


### 列出共享库的依赖关系   
工具ldd会遍历二进制可执行代码中的目标文件，然后列出所有共享库的依赖关系。  
```shell
ldd /bin/bash  
	libtermcap.so.2 => /lib/libtermcap.so.2 (0x40027000) 
```
每一行列出的第一个名字是共享库出现在函数内部的名字，第二个名字就是磁盘上找到的实际运行库的路径名，而库被载入内存的地址出现在行末。   


### 显示目标文件的内部结构  
工具objdump用于展开目标文件、静态库和共享库中的信息，然后将其表示成可读模式，该命令可用于输出来自不同格式目标文件的信息。


### 环境变量   
- C_INCLUDE_PATH:	指定一个或多个目录列表，查找头文件，相似的有：CPATH、CPLUS_INCLUDE、OBJC_INCLUDE_PATH  
- COMPILER_PATH:	该环境变量指定一个或多个目录名列表，相似的有：LIBRARY_PATH、GCC_EXEC_PREFIX    
- CPATH:			编译c\c++\objective-c程序时使用该环境变量，该环境变量指定一个或多个目录名列表，查找头文件，就好像在-l选项一样，会首先查找-l指定的所有目录。    
- CPLUS_INCLUDE_PATH:	同上   
- LD_LIBRARY_PATH:	该环境变量不会影响编译程序，但程序运行的时候会有影响，变量指定一个目录列表，程序会查找该列表定位共享库，只有当未在编译程序的目录中找到共享库的时候，执行程序必须设置该变量。  
- LIBRARY_PATH:		连接程序会搜索该目录，以查找特殊连接程序文件，和由-l命令行选项指定名字的库    


