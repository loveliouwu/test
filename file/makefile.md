- 包含多个源文件
```

SRC := $(wildcard ./*c)
SRC += $(wildcard ./src1/*.c)
INC := $(wildcard ./inc1)

CC      = gcc
TARGET	= main.o
all:$(OBJS)
	$(CC) -I$(INC) -g  $(SRC)  -o $(TARGET)  

```
*wildcard 是makefile关键字
-g 表示编译添加gdb调试
-I （大写的i）表示头文件搜索路径
-L "path/"表示库文件搜索路径
*