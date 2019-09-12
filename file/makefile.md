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