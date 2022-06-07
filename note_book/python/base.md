### 数据类型  
数字： 1234，3.1314
字符串： 'spam',"guido's"
列表： [1,[2,'three'],4]
字典： {'food':'spam','taste':'yum'}
元组： （1，'spam'，4，'U'）
文件： myfile=open('eggs', 'r')

>>数字、字符串、元组是不可改变的
>>dir(s)  查看s支持的属性列表

#### 字符串  
```python
s = "123456"
s[1]
s[1:3]
s[2:]
s[:3]
s[-4:-1]
s[-4:]
s+'xyz'
s*2

#func
s.fund('pa')
s.replace('pa','XYZ')
s.split(',')
s.upper()
s.isalpha()
s.rstrip() #remove whitespace characters on the right side

```


#### 列表  
```python 
L = ['test',10,[1,2,3]]
L[0]
L + [4,5,6]
dir(L)
L.count(1)
c={'name':'lyp'}
L.count(c)

```