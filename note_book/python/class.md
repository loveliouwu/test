## property 装饰器 
针对私有属性（以'_'开头的变量），可以设置property包装器来包装getter和setter方法，使得对属性的访问即安全又方便：
```python
class person():
    def __init__(self,name,age):
        self._name = name
        self._age = age
    
    # 一般的访问函数
    def get_name(self):
        return self._name
    def get_age(self):
        return self._age
    # 使用的时候：
    # name = person1.get_name()

    # 访问器 - getter方法
    @property
    def name(self): # 方法名与变量名一致
        return self._name
    @property
    def age(self):
        return self._age
    # 使用的时候：
    # name = person1.name

    #setter
    @name.setter
    def name(self,new_name):
        self._name = new_name
    # 使用：
    # person1.name = "new name"
    

```


## 静态方法 
意义与C++中的几乎一样,静态方法不需要创建对象即可调用
```python
import os
class myfile(object):
    def __init__(self,filename):
        self._filename = filename
    
    @staticmethod
    def file_is_exist(filename):
        return os.file


if __name__ == '__main__':
	filepath = ''
	fp = myfile.file_is_exist(filepath)
```