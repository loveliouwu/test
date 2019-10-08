#GDB调试学习

- 常用的命令
```
    - s step 进入函数
    - n next 行执行
    - c finish 执行完当前函数闭并返回
    - p exp 
    - b func 函数断点
    - b 10  行号断点
    - b file.c:6 指定文件的断点行
    - b a/file.c:6 指定路径指定文件断点行
```


- 退出正在调试的函数
   
   1. `finish`
    ```
    函数会继续执行完，并且打印返回值，然后等待接下来的命令
    ```
   2. `return`
   ```
   函数不会执行下面的语句，并且可以用 `return expression` 命令返回指定的返回值，取代未执行代码的返回值
   ```

- 直接执行函数
  
```
使用gdb调试程序时，可以使用`call`或`print`命令直接调用函数执行，执行函数后会对函数使用的全局变量改变，之后运行其他涉及该全局变量的函数时会改变原有的函数逻辑
> 例如
    -->test.c
    int global = 1
    int func(void)
    {
        return (++global);
        ···
    }
    void mai()
    {
        func();
        printf("%d\n", global);
    }

    gdb-->
    (gdb) start
    (gdb) call func()
    $1 = 2
    (gdb) print func()
    $2 = 3
    (gdb) n
    3
```

- 打印函数堆栈帧信息

```
    - i frame
    i是info命令缩写，显示函数堆栈帧信息
    - i registers
    显示寄存器值

```

- 设置条件断点

```
条件断点可以常用在循环体中
> 例如：
test.c
int main()
{
    int i = 0;
    int sum = 0;
    for(i = 0;i<=200;i++)
    {
        sum += i;
    }
}

(gdb) start
(gdb) b 10 if i==101
(gdb) r
...breakpoint at test.c:10
(gdb) p sum
$1 = 5050
```

- 忽略断点
  
```
设置断点以后，可以忽略断点，命令是`ignore hnum count`意思是借来来count次编号为hnum的断点都不会让程序中断，只有当第count+1次断点出发才会让程序中断。

(gdb) b 10 
breakpoint 1 at 0x4004e3: file a.c line 10
(gdb) ignore 1 5
(gdb) r
breakpoint 1
10
(gdb) p i
$i = 6

```


- 设置观察点
```
使用命令`watch`命令设置观察点，也就是当一个变量值发生变化时，程序会停下来
列出当前所设置的所有观察点 info watchpoints
(gdb) watch a
```

- 设置观察点只对指定线程有效

```
wa(watch) expr thread threadnum
只有编号为threadnum的线程改变了变量expr的值时，程序才会停下来，其他编号的线程改变变量的值时不会让线程停住

```


- 设置读观察点

```
使用命令`rwatch`设置读观察点，也就是当发生读取变量行为时，程序就会暂停住

rw(rwatch) a
每次访问a的值都会让程序停下来
```

- 设置读写观察点
```
可以使用`awatch`命令设置读写观察点，
当发生读取变量或改变变量值的行为时，程序就会暂停住
aw(awatch) a
```

- 打印信息
```
    - 打印字符串
    命令`x/s str`
    x/ws str  打印一个宽字节长度
    x/hs str  打印两个字节长度

    - 打印大数组内的内容
    p array 缺省最多会显示200个元素
    可以设置这个最大限制数
    set print elements number-of-elements
    例如
    set print elements 0  设置为没有限制

    - 打印数组中任意连续元素的值
    p array[60]&10  打印第60~69个元素

    - 设置带引索引下标
    set print array-indexes on

    - 查看函数局部变量的值
    bt full  显示各个函数的局部变量的值
    如果只想打印当前函数的局部变量的值
    info locals
```