###lambda匿名函数

- 基本格式
[capture](parameters) mutable ->return -type {
    statement
}

1.[]
标识一个lambda的开始，这部分必须邨彩，不能省略
1.1 空
    没有使用任何函数对象参数
1.2 =
    函数体内可以使用lambda所在作用范围内所有可见的局部变量（包括lambda所在类的this），并且是值传递方式
1.3 &
    函数体内可以使用lambda所在作用范围内所有可见的局部变量（包括lambda所在类的this），并且是引用传递方式
1.4 this
    函数体内可以使用lambda所在类中的成员变量
1.5 a
    将a按值传递，按值传递时，函数体内不能修改传递进来的a的拷贝，因为默认情况下是const的，要修改传递进来的a的拷贝，可以添加mutable修饰符
    [a]() mutable { a = 100;}
1.6 &a
    将a按引用进行传递
1.7 a,&b
    混合使用
1.8 =，&a
    除a用引用传递外，其他值按值传递
1.9 &，a
    与上一个相反


2.()
    表示重载的()操作符的参数，没有参数时这部分可以省略不填充

3.可修改标示符
    mutable声明，可以省略，按值传递时加上mutable修饰符后，可以修改按值传递进来的拷贝，注意只能修改拷贝，值本身没有变

- 通常配合信号使用，非常方便
```
//定义信号和槽
signals:
    void lambda_signal(int num)
public slots:
    void num_print(int num);

//连接
QObject::connect(this,pe100::lambda_signal,[](int num){
    qDebug()<<num;
})
```