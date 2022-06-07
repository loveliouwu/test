table类型实现了“关联数组”，关联数组是一种具有特殊索引方式的数组，不仅可以通过整数来索引，还可以使用字符串或其他类型的值来索引 

table没有固定的大小，可以动态地添加任意数量的元素到一个table中 

在lua中，table既不是“值”也不是“变量”，而是“对象” 

通过 `a = {}` 创建table, table永远是一个匿名的，一个持有table的变量与table自身没有固定的关联性，当一个table再也没有被引用时，系统会回收并删除该table
```lua
    a = {}
    a[1] = "sss1"
    k = 3
    a[k] = "sss3"
    b = a --这是a b都引用了同一个table
    a = nil
    b = nil
    --这时 再也没有对table的引用了，lua的垃圾收集器会删除该table
```


### table.key 语法糖
lua实现了诸如 
```lua  
    a["name"] = "lyp"
    print(a.name)  --直接输出该索引的值
    -- a.x 等同于 a["x"]
    a[1] = "abc"
    print(a[1])

```

### 通过 # 获取table的最后一个元素索引
```lua
    a[1] = 1
    a['2'] = 2
    a['last'] = 3
    print(#a) -- last

```

### unpack
从下标1开始返回所有的元素
```lua
a = {10,20,30}
b,c,d = table.unpack(a)

为了兼容5.1以前的版本

if unpack == nil then
    b,c,d = unpack(a)
else
    b,c,d = table.unpack(a)
end
```