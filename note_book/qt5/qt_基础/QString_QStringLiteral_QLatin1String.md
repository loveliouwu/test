
## QLatin1String

- 说明
```C
if (str == "auto" || str == "extern"
        || str == "static" || str == "register") {
    ...
}
```
一般来说上面这段代码比下面这段代码执行效率要高很多
```C
if (str == QString("auto") || str == QString("extern")
        || str == QString("static") || str == QString("register")) {
    ...
}
```
因为下面这段需要申请四个`QString`对象来存放字符串，如果在程序中定义了`QT_NO_CAST_FROM_ASCII`的应用程序是无法使用`QString`的`const char*`相关的API函数，
```C
QString str = "abc";
```
因此Qt提供了`QLatin1String`类来更高效的利用`const char* `的类型，它就是一个关于`const char*`的一个浅封装

如果利用QLatin1String类来写上述的程序就是
```C

if (str == QLatin1String("auto")
        || str == QLatin1String("extern")
        || str == QLatin1String("static")
        || str == QLatin1String("register") {
    ...
    }
```

得益于QString(const QLatin1String&)这个构造函数，QLatin1String可以在任何需要QString对象的地方使用，比如：

```C
QLabel *label = new QLabel(QLatin1String("MMD",this));
```



## QStringLiteral

#### 说明
QStringLiteral 是Qt5中新引入的一个用来从“字符串常量”创建QString对象的**宏**, 展开来就是一个lambda函数的调用
临时的字符串将会作为一个属性功能参数，这样可能开销很大，这样需要一些空间支持和且创建了一个的数据向Qt内部编码格式的复制传递
这些不必要的开销可以使用QStringLiteral来代替

在只接受QString参数的函数，无论我们给一个字面字符串或QLatin1String，都会隐式构造一个临时的QString对象，构造这个对象需要在栈上申请一定的内存空间，然后把字符串拷贝过去，如果这样的调用比较多，那还是一笔不小的开销。此时，我们可以使用QStringLiteral来减少这个开销。

在这种情况中，QString内部的数据将会在编译的时候才生成，不需要传值或者在运行的时候分配空间
使用QStringLiteral来代替C++中的双重数值传递将会在编译的时候显著的提升时间

#### 使用场景
大多数情况：使用QStringLiteral(“某字符串”) －－如果它最终转会换成QString的话。

#### 使用方法
```
o->setObjectName(QStringLiteral("MyObject"));
```
