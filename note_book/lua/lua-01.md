## lua基础

### 介绍

> lua是一种轻量小巧的脚本预研，用标准C语言编写并以源代码的形式开放，设计目的是为了嵌入应用程序中，从而为应用程序提供灵活的扩展和定制功能
> 特性：轻量级、可扩展
> 应用场景：游戏开发、独立应用脚本、web应用脚本、扩展和数据库插件


### 注释风格

单行注释： -- 两个减号
多行注释： [[ 多行注释 ]]


### 数据类型

1:  nil
2:  boolean   包含两个值，false 和 true
3:  number      表示双精度类型的实浮点数
4:  string      字符串，由一对双引号或单引号来表示
5:  function    由C或Lua编写的函数
6:  userdata    表示任意存储在变量中的C数据结构
7:  thread      表示执行的独立线路，用于执行协同程序
8:  table       Lua中的表其实是一个关联数组，数组的索引可以是数字、字符串或者表类型，在Lua里，table的创建是通过构造表达式来完成，最简单构造表达式是{},用来创建一个空表

### 关键字

#### nil
nil属于Lua的基本数据类型，表示一个无效值(在条件表达式中相当于false)
b = nil
print(b) --> nil


### number
Lua默认只有一种number类型 --doublue(双精度)类型


### string 字符串
- 
str1 = "this is a lua string"
str2 = [[this is also lua string]]

- 
在对一个数字字符串上进行算术操作时，lua会尝试将这个数字字符串转换成一个数字
print("2" + 6)

- 
连接字符串：
print("me" .. "you")
a="me"
b="you"
print(a .. b)
print(a + b) 报错，字符串无法用+号去连接
- 
使用`#`来计算字符串的长度，放在字符串前面
len = "www.lyp.com"
print(#len)
print(#"www.lyp1.com")

### local
lua中的变量全部是全局变量，无论语句块或者函数里，除非用Local显式声明为局部变量，变量默认值均为nil
使用local创建一个局部变量，与全局变量不同，局部变量只在被声明的那个代码块内有效

### table
local table1 = {} --创建一个空表
local table1 = {"apple","peer","orange","grape"} --直接初始表
不同于其他语言的数组把0作为数组的初始索引，在Lua里表的默认索引一般以1开始


### function
在Lua中，函数是被看作是“第一类值”，函数可以存在变量里
定义一个函数，可以将函数直接赋值给另一个变量
function func1(a)
    ...
end
func2 = func1 
可以以匿名函数的方式通过参数传递
function func3(a,func)
    print(func(a))  --将func函数当作一个参数传进来
end

test_a = "llyypp"
func3(test_a,functiong(b)
    return b ;
end
    )


### 循环
lua中的循环有：
while循环 
for循环 
repeat ... until 
嵌套循环 

循环控制语句有： 
break 
goto 

- while 循环 
```lua
while(true)
do
    print("while")
end
```

- for 循环
语法格式
for var=exp1,exp2,exp3 do
    ...
end
var从exp1变化到exp2，每次变化以exp3为步长，步长参数是可选的，如果不指定则默认为1
for i=0,f(x) do
    print(i)
end
注意 for 中的三个表达式在循环开始前一次性求职，即f(x)只执行以此
```lua
function f(x)
    print("function")
    return x*2
end
for i=1,f(3) do
    print(i)
end
```
输出结果为：
1
2
3
4
5
6



- repeat until 循环
- Lua中的repeat until 循环语句不同于for和while循环的条件语句在当前循环执行开始时判断，而repeat until循环的条件语句在当前循环结束后判断。
repeat
    a = a+1
    print("ssss")
until(a>15)


- goto 
```lua
local a = 1
::label:: print("goto label")

a = 2
if a < 4 then
    goto label
end

```


### 调用error函数来显示的抛出一个错误
error("msg")


### tostring() 将任何类型转换成字符串
tostring(100)  --"100"
tostring(100 * 2)  --"200"


### repeat
a = 10
repeat
    a = a + 10
    print(a)
untile a>100

repeat
    line = io.read()
    print(line)
until line ~= ""



### 变量名命名规则
不能用数字作为开头，尽量不要使用下划线开头，因为lua中的一些特殊变量是以下划线开头的。


### 泛型 for 
迭代table元素的（pairs）、迭代数组元素的（ipairs）、迭代字符串中单词的（string.gmatch）等。
a = {}
a[1] = 124
for i,v in ipairs(a) do   --迭代数组元素
    print(i .. v)
end

for i in pairs(a) do  --迭代元素key
    print("key is " .. i)
end