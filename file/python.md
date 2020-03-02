#

####word
https://docs.python.org/zh-cn/3.7/tutorial/index.html


####几个pip安装源以解决install慢
http://pypi.douban.com/ 豆瓣 
http://pypi.hustunique.com/ 华中理工大学 
http://pypi.sdutlinux.org/ 山东理工大学 
http://pypi.mirrors.ustc.edu.cn/ 中国科学技术大学 
http://mirrors.aliyun.com/pypi/simple/ 阿里云 
https://pypi.tuna.tsinghua.edu.cn/simple/ 清华大学

1.临时使用：
可以使用pip时添加参数 -i[url]，如：

pip install -i http://pypi.douban.com/simple/ django

- 语法创建链表
```
f = [ i for i in range(0,10) ]
f = [ x + y for x in range(0,10) for y in range(0,20) ]
```

- 类的继承
```
#!/bin/bash

import os
import time


class clock(object):
    '''time clock'''
    def __init__(self,hour=0,minute=0,second=0):
        self.hour=hour
        self.minute=minute
        self.second=second
    
    def run(self):
        self.second += 1
        if self.second == 60:
            self.second = 0
            self.minute += 1
            if self.minute == 60:
                self.minute = 0
                self.hour += 1
                if self.hour == 24:
                    self.hour = 0

    def show(self):
        print("%02d:%02d:%02d" % (self.hour,self.minute,self.second))

class clock1(clock):
    def __init__(self,year,month,day):
        #self.hour = 10
        #self.minute = 46
        #self.second = 4
        #super().__init__(self.hour,self.minute,self.second)
        #注：上下两种方式都可以，但是这都是python3的格式
        super().__init__(10,46,4)
        self.year = year
        self.month = month
        self.day = day

    def run(self):
        super().run()


    def show(self):
    	#如果要调用子类的show则
    	super().show()
        print("%04d-%02d-%02d %02d:%02d:%02d" % (self.year,self.month,self.day,self.hour,self.minute,self.second))



def main():
    clocka = clock(23,59,50)
    clockb = clock1(2019,8,28)
    while True:
        clocka.show()
        clockb.show()
        time.sleep(1)
        clocka.run()
        clockb.run()

if __name__ == '__main__':
    main()

```


####禁止函数修改列表
1.传递列表的副本
function(list_name[:])
切片表示法[:]创建列表的副本

"""
    字符串的基本操作, python中的字符串是不可变的，会拷贝一份进行修改，原string不变
    切割,并没有slice函数直接中括号，中间使用**冒号**隔开 [起始：结束：步长]
"""
a="123456789"
a[::2] #"13579"
a[::3] #"147"
a[1:4:2] #"24"

"""
    self参数不用管，不填即可
    a.find(self, subStr, start, end) 检测a在start,end区间内是否包含subStr,返回值是subStr的index
    a.rfind(self, subStr, start, end) 从右边倒叙查找，但是没有lfind因为find()默认就是左边开始
    a.index(self, subStr, start, end) 方法和find一样，不建议使用，如果index找不到报错异常
    a.count("b") 检测b在a字符串中出现的次数
    a.replace(self, old, new, count) 该方法返回新的str，count指定的话，则替换不超过count次
    a.split(self,sep,maxSplit) 去除sep后的a返回一个[]
"""
print(a.find("c"))
print(a.rfind("c"))
print(a.index("c"))
print(a.count("c"))
print(a.replace("c","C",2))
print(a.split("c")) 

"""
    a.capitalize()  字符串**首字母**进行大写
    a.title()  字符串**每个单词的首字母**进行大写
    a.startswith()  字符串是否已某个字串开始
    a.endswith()  字符串是否已某个字串结尾
    a.lower()  字符串所有字母进行小写
    a.upper()  字符串所有字母进行大写
"""
b = "hello world"
print(a.capitalize())
print(b.title())
print(a.startswith("ab"))
print(a.endswith("ab"))
print(a.lower())
print(a.upper())
print(a.islower())
print(a.isnumeric())

####传递任意数量的实参
def function(*args):
    print(args)

####通过as给函数或模块指定别名
from pizza import make_pizza as mp

import pizza as food_pizza

####倒入模块中所有函数
from os import *


####集合的操作
"""
    多个set集合的操作, 交集并集主要用在多个set的去重
    & -> 交集
    | -> 并集
"""
set1 = {1,2,3,4}
set2 = {3,4,5,6}
print(set1 & set2)
print(set1 | set2)

####多个返回值的方式
"""
    处理多个返回值的方式
    1- return ["小明"， 28]
    2- return {"name":"小明"，"age"：28]
    2- return 返回值1， 返回值2
"""
def my_func04(name, age):
    return name, age

print(my_func04("张三",28)[0])
print(my_func04("张三",28)[1])
name, age = my_func04("李四", 28)

