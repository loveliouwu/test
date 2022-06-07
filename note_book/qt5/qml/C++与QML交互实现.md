## QML调用C++的方法 
- 首先C++的类要继承`QObject`类，同时定义`Q_OBJECT`宏 如下所示   
```C++
Class MyTitle : public QObject{
    Q_OBJECT
}
``` 
- 在C++中添加能够在QML中访问的属性  
```C++
    Q_PROPERTY(QString title READ get_title WRITE set_title NOTIFY title_change_signal)

    //其中get_title和set_title是对该属性赋值和读取的方法
    //title_change_signal是一个信号函数，当QML更改该属性的值时候触发该信号
    QString get_title();//READ必须有返回参数
    void set_title(QString str);//Write必须有一个参数

    //其中get_title 和 set_title可以为信号函数
//标准定义为
Q_PROPERTY(type name
             (READ getFunction [WRITE setFunction] |
              MEMBER memberName [(READ getFunction | WRITE setFunction)])
             [RESET resetFunction]
             [NOTIFY notifySignal]
             [REVISION int]
             [DESIGNABLE bool]
             [SCRIPTABLE bool]
             [STORED bool]
             [USER bool]
             [CONSTANT]
             [FINAL])
```  

- 定义接口以方便QML调用   
可以在C++中定义一个方法来对QML对象调用，该方法需要特殊的宏 如：
```QT
Q_INVOKABLE QString settitle_to_qml(QString title);
```

- 在QML中调用C++方法   
```XML
MyTitle{
    id:my_title
    count:100 //用自己定义的属性
    Component.onCompleted: {
            my_title.settitle_to_qml("my_title!")
        }
}

```