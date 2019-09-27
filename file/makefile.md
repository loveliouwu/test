- ####makefile包含5个内容
	- **显示规则**
	> 显示规则说明了如何生成一个或多个目标文件
	- **隐晦规则**
	> 由于我们的make有自动推导的功能，所以隐晦规则可以更为简略的书写makefile
	- **变量的定义**
	> 变量一般都是字符串，类似C语言中的宏，执行时被替换扩展到相应引用位置
	- **文件指示**
		- 一个makefile文件中引用另一个makefile，就像C语言中的include一样
		> 
		```
		include ./inc/makefile  *.mk
		```
		- 根据某些情况指定makefile中的有效部分，就像C语言中的预编译
		`if eq` `if neq` `ifdef` `ifndef`
		>
		ifxxx  (arg1,arg2)
		else
		endif
		>
		ifdef  var1
		else
		endif
			>
			```
			var1:=A
			var2:=$(var1)
			var3:= 
			test:
			ifeq ($(var1),$(var2))
				@echo "var1 == var2"
			else
				@echo "var1 != var2"
			endif
			ifneq ($(var3),)
				@echo "var3 is not empty"
			else
				@echo "var3 is empty"
			endif
			ifndef var3
				@echo "var3 is empty"
			else
				@echo "var3 is not empty"
			endif
			```
			
		>
		当使用判断语句时，伪目标之后不能添加tab
		test:
		ifeq ($(var1),$(var2))
		另外注意`@`符号修饰的命令的作用：
		通常make会把其要执行的命令行在命令执行前输出到屏幕上，当我们用`@`字符在命令前，那么，这个命令将不会被make显示出来，通常我们用这个功能显示一些信息

	- **注释**
	>
	makefile中只有行注释，和UNIX和Shell脚本一样，其注释使用`#`字符，类似C语言中的`//`，如果你要使用`#`字符，则可以用反斜杠转义。	
	
- ####make参数
	- `-n`、`--just-print`
	如果make执行时，带入参数`-n` `--just-print`，那么其只显示命令，但是不会执行，这样我们可以调试makefile，看我们的命令执行起来是怎样的顺序
	- `I`、`--include-dir`
	指定make寻找文件的范围，寻找文件可以是头文件也可以是makefile文件
	如果makefile没有找到文件或者不能读取的文件，make会出现致命信息，如果你想让make不理会那些无法读取的文件而继续执行，你可以在include前加一个减号`-`如：`-include <filename>`，其表示Include过程中出现什么错误都不要报错继续执行。
	- `-s`、`--silent`、`--quiet`
	全面禁止命令的显示
			
- 文件搜索路径
在一些大工程中，有大量的源文件，当make需要去寻找文件的依赖关系时，我们可以在文件前加上路径，但最好的办法是把一个路径告诉make，让make再自动去找
	**`VPATH`**特殊变量就是用来完成这个功能的，如果没有指明这个变量，make只会在当前的目录中去寻找依赖文件和目标文件，如果指定了这个变量，那么make就会在当前目录找不到的情况下，到指定的目录中去寻找文件
	`VPATH = src:../headers`
	上面的定义指定两个目录，`src`和`../headers`，目录由分号`:`来分隔

- ####包含多个源文件
```

SRC := $(wildcard ./*c)
SRC += $(wildcard ./src1/*.c)
INC := $(wildcard ./inc1)

CC      = gcc
TARGET	= main.o
all:$(OBJS)
	$(CC) -I$(INC) -g  $(SRC)  -o $(TARGET) 
	#或者 
	$(CC) main.c ./src1/*.c -o $(TARGET) -I ./inc1

```
*wildcard 是makefile关键字
-g 表示编译添加gdb调试
-I （大写的i）表示头文件搜索路径
-L "path/"表示库文件搜索路径
*
- --
- ####自动推导
一般我们要编译目标需要这样写：
```
edit: main.o
main.o: main.c main.h def.h
	cc -c main.c
```

但是make自动推导功能可以自动推导文件以及文件依赖关系后面的命令，于是我们无需在每个`.o`文件后面都添加上类似命令
```
edit: main.o
main.o:main.h def.h
```
*#已经自动推导，main.o编译时会自动添加main.c到编译依赖文件，并且自动编译*

- --
- ####清空目标文件的规则
每个makefile都应该有一个清空目标文件的规则
一般的风格都是：
```
objects = main.o file.o
clean:
	rm exit $(objects)
```
但是更为稳健的做法是用伪目标
```
.PHONY:clean
clean:
	-rm edit $(objects)
```
`rm`前面加上`-`号的意思是也许某些文件出现问题，但是不要管，继续做后面的事

- --
- ####伪目标
伪目标一般没有依赖的文件，但是我们也可以为伪目标指定所依赖的文件。伪目标同样也可以作为默认膜表，只要将其放在第一个，如：
```
all: prog1  prog2  prog3
.PHONY : all
prog1:prog1.o utils.o
	cc -o prog1 prog1.o uitils.o
...
```
指定`make`时会将伪目标作为默认目标，这样执行一次命令就会生成三个可执行文件。
这里注意，第一句直接就告诉`make`这个`all`是个伪目标，这是由`make`隐式推导出来的，但是写第二句是显式告诉`make`这是个伪目标。这样执行`make`不会生成`all`文件，而是执行后面的多个目标。

####gcc
gcc and g++分别是gnu的c & c++编译器   
gcc/g++在执行编译工作的时候，总共需要4步   
1.预处理,生成.i的文件  

    预处理器cpp
2.将预处理后的文件不转换成汇编语言,生成文件.s  

    编译器egcs
3.有汇编变为目标代码(机器代码)生成.o的文件

    汇编器as
4.连接目标代码,生成可执行程序

    连接器ld

1.总体选项
>    -E   
只激活预处理,这个不生成文件,你需要把它重定向到一个输出文件里   
面.   
例子用法:   
gcc -E hello.c > pianoapan.txt   
gcc -E hello.c | more   
慢慢看吧,一个hello word 也要与处理成800行的代码      
    -S   
只激活预处理和编译，就是指把文件编译成为汇编代码。   
例子用法   
gcc -S hello.c   
他将生成.s的汇编代码，你可以用文本编辑器察看     
    -c    
只激活预处理,编译,和汇编,也就是他只把程序做成obj文件   
例子用法:   
gcc -c hello.c   
他将生成.o的obj文件  

2.目录选项
>
 -Idir   
在你是用#include"file"的时候,gcc/g++会先在当前目录查找你所制定的头   
文件,如果没有找到,他回到缺省的头文件目录找,如果使用-I制定了目录,他   
回先在你所制定的目录查找,然后再按常规的顺序去找.   
对于#include,gcc/g++会到-I制定的目录查找,查找不到,然后将到系   
统的缺省的头文件目录查找    

-include file
-i

相当于“#include”
包含某个代码,简单来说,就是便以某个文件,需要另一个文件的时候,就可以   
用它设定,功能就相当于在代码中使用#include   
例子用法:   
gcc hello.c -include /root/pianopan.h   
    -I-   
就是取消前一个参数的功能,所以一般在-Idir之后使用   
   -idirafter dir   
在-I的目录里面查找失败,讲到这个目录里面查找.   
    -iprefix prefix   
    -iwithprefix dir   
一般一起使用,当-I的目录查找失败,会到prefix+dir下查找    
    -Ldir   
制定编译的时候，搜索库的路径。比如你自己的库，可以用它制定目录，不然   
编译器将只在标准库的目录找。这个dir就是目录的名称。  
    -llibrary    
制定编译的时候使用的库   
例子用法   
gcc -lcurses hello.c   
使用ncurses库编译程序   

3.调试选项
     -g   
只是编译器，在编译的时候，产生调试信息。   
    -gstabs   
此选项以stabs格式声称调试信息,但是不包括gdb调试信息.   
    -gstabs+   
此选项以stabs格式声称调试信息,并且包含仅供gdb使用的额外调试信息.   
    -ggdb    
此选项将尽可能的生成gdb的可以使用的调试信息.

    -glevel

请求生成调试信息，同时用level指出需要多少信息，默认的level值是2

4.链接方式选项：
    -static 此选项将禁止使用动态库。

优点：程序运行不依赖于其他库

缺点：文件比较大

    -shared (-G) 此选项将尽量使用动态库，为默认选项

优点：生成文件比较小

缺点：运行时需要系统提供动态库

    -symbolic 建立共享目标文件的时候,把引用绑定到全局符号上.

对所有无法解析的引用作出警告(除非用连接编辑选项 `-Xlinker -z -Xlinker defs'取代)。

注：只有部分系统支持该选项.

5.错误与告警选项
    -Wall 一般使用该选项，允许发出GCC能够提供的所有有用的警告。也可以用-W{warning}来标记指定的警告。

    -pedantic 允许发出ANSI/ISO C标准所列出的所有警告

    -pedantic-errors 允许发出ANSI/ISO C标准所列出的错误

    -werror 把所有警告转换为错误，以在警告发生时中止编译过程

    -w 关闭所有警告,建议不要使用此项

6.预处理选项
    -Dmacro   
相当于C语言中的#define macro   
    -Dmacro=defn   
相当于C语言中的#define macro=defn   
    -Umacro   
相当于C语言中的#undef macro   
    -undef   
取消对任何非标准宏的定义 

7.其他选项
    -o

制定目标名称,缺省的时候,gcc 编译出来的文件是a.out,很难听,如果你和我有同感，改掉它,哈哈

例子用法   
gcc -o hello.exe hello.c (哦,windows用习惯了)   
gcc -o hello.asm -S hello.c   
    -O0   
    -O1   
    -O2   
    -O3   
编译器的优化选项的4个级别，-O0表示没有优化,-O1为缺省值，-O3优化级别最高

    -fpic 编译器就生成位置无关目标码.适用于共享库(shared library).

    -fPIC 编译器就输出位置无关目标码.适用于动态连接(dynamic linking),即使分支需要大范围转移.

    -v 显示详细的编译、汇编、连接命令
    -pipe   
使用管道代替编译中临时文件,在使用非gnu汇编工具的时候,可能有些问题   
gcc -pipe -o hello.exe hello.c   
    -ansi   
关闭gnu c中与ansi c不兼容的特性,激活ansi c的专有特性(包括禁止一些asm inline typeof关键字,以及UNIX,vax等预处理宏,   
    -fno-asm   
此选项实现ansi选项的功能的一部分，它禁止将asm,inline和typeof用作关键字。   
    -fno-strict-prototype   
只对g++起作用,使用这个选项,g++将对不带参数的函数,都认为是没有显式的对参数的个数和类型说明,而不是没有参数.而gcc无论是否使用这个参数,都将对没有带参数的函数,认为城没有显式说明的类型   
    -fthis-is-varialble   
就是向传统c++看齐,可以使用this当一般变量使用.   
    -fcond-mismatch   
允许条件表达式的第二和第三参数类型不匹配,表达式的值将为void类型   
    -funsigned-char   
    -fno-signed-char   
    -fsigned-char   
    -fno-unsigned-char   
这四个参数是对char类型进行设置,决定将char类型设置成unsigned char(前   
两个参数)或者 signed char(后两个参数) 
    -imacros file   
将file文件的宏,扩展到gcc/g++的输入文件,宏定义本身并不出现在输入文件中       
    -nostdinc   
使编译器不再系统缺省的头文件目录里面找头文件,一般和-I联合使用,明确限定头文件的位置   
    -nostdin C++   
规定不在g++指定的标准路经中搜索,但仍在其他路径中搜索,.此选项在创建libg++库使用   
    -C   
在预处理的时候,不删除注释信息,一般和-E使用,有时候分析程序，用这个很方便的   
    -M   
生成文件关联的信息。包含目标文件所依赖的所有源代码你可以用gcc -M hello.c来测试一下，很简单。   
    -MM   
和上面的那个一样，但是它将忽略由#include造成的依赖关系。   
    -MD   
和-M相同，但是输出将导入到.d的文件里面   
    -MMD   
和-MM相同，但是输出将导入到.d的文件里面   
    -Wa,option   
此选项传递option给汇编程序;如果option中间有逗号,就将option分成多个选项,然后传递给会汇编程序   
    -Wl.option   
此选项传递option给连接程序;如果option中间有逗号,就将option分成多个选项,然后传递给会连接程序.

    -x language filename   

设定文件所使用的语言,使后缀名无效,对以后的多个有效.也就是根   
据约定C语言的后缀名称是.c的，而C++的后缀名是.C或者.cpp,如果   
你很个性，决定你的C代码文件的后缀名是.pig 哈哈，那你就要用这   
个参数,这个参数对他后面的文件名都起作用，除非到了下一个参数   
的使用。   
可以使用的参数吗有下面的这些   
`c’, `objective-c’, `c-header’, `c++’, `cpp-output’,    
`assembler’, and `assembler-with-cpp’.   
看到英文，应该可以理解的。   
例子用法:   
gcc -x c hello.pig   
    -x none filename   
关掉上一个选项，也就是让gcc根据文件名后缀，自动识别文件类型   
例子用法:   
gcc -x c hello.pig -x none hello2.c