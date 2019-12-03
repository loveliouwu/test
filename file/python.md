#

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

####传递任意数量的实参
def function(*args):
    print(args)

####通过as给函数或模块指定别名
from pizza import make_pizza as mp

import pizza as food_pizza

####倒入模块中所有函数
from os import *


