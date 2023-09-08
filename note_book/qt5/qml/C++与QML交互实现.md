### QML调用C++的方法 
- 首先C++的类要继承`QObject`类，同时定义`Q_OBJECT`宏   

- 在C++中添加能够在QML中访问的属性 
```c++
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
可以在C++中定义一个方法来对QML对象调用，该方法需要特殊的宏`Q_INVOKABLE`

## 实例1
- c++.h
```C++
Class MyTitle : public QObject{
    Q_OBJECT
    //定义属性 title 以及读写和信号函数
    Q_PROPERTY(QString title READ get_title WRITE set_title NOTIFY title_change_signal)
    //其中get_title和set_title是对该属性赋值和读取的方法
    //title_change_signal是一个信号函数，当QML更改该属性的值时候触发该信号
    //其中get_title 和 set_title也可以为信号函数
    QString get_title();//READ必须有返回参数
    void set_title(QString str);//Write必须有一个参数
public:
    QString title_text = "NULL";
    Q_INVOKABLE void sendMsg(QString msg);
    Q_INVOKABLE QString settitle_from_qml(QString title);
signals:
    void title_change_signal(); //属性 title 的信号函数
public slots:
    void text_change_slot_calledByQml();  //给QML触发的槽函数
    void text_change_slot_callebByCpp();  //传统的C++槽函数
}
```  
- c++.cpp
```c++
MyTitle::MyTitle(){
    qDebug()<<"myTitle init";
    QObject::connect(this, SIGNAL(title_change_signal()), this, SLOT(text_change_slot_calledByCpp()));
}
//通过Q_PROPERTY属性来访问的函数接口
QString MyTitle::get_title(){
    return title_text;
}
void MyTitle::set_title(QString str){
    title_text = str;
}
//通过Q_INVOKABLE属性来访问的函数接口
void MyTitle::sendMsg(QString msg){
    qDebug()<<"sendMsg:"<<msg;
}

void MyTitle::text_change_slot_calledByQml(){
    qDebug()<<"text_change_slot_calledByQml";
}

void MyTitle::text_change_slot_calledByCpp(){
    qDebug()<<"text_change_slot_calledByCpp";
}

```
- main.cpp
```c++
int main(){
    ...
    qmlRegisterType<MyTitle>("my.cpp", 1, 0,"MyTitle"); //将C++类注册为QML元素
    ...
}
```


- qml

```c++
import my.cpp  //将注册的C++类元素导入

Text{
    id: id_text
    //自定义定义信号
    signal signal_from_qml_textChanged()
    text: "text's text"
    //使用默认的槽函数来测试C++函数调用
    onTextChanged: {
        console.log("text's text changed")
    }
    onAccepted: {  //Enter 键触发的槽函数
        my_title.sendMsg("send msg to MyTitle");
    }
    //自定义信号的默认slot实现
    onSignal_from_qml_textChanged: { 
        console.log("default slot called")
    }

}

//绑定信号和槽
Connection{
    target: id_text //目标，指的是发送信号的对象
    //调用方式1【不推荐】
    //onSignal_from_qml_textChanged : my_title.text_change_slot_calledByQml()
    //调用方式2【推荐】
    function onSignal_from_qml_textChanged() {
        console.log("connect slot called")
        my_title.text_change_slot_calledByQml()
    }
}

//实例化一个C++类
MyTitle{
    id:my_title
    count:100 //用自己定义的属性
    Component.onCompleted: {
            title: "set title"
            my_title.settitle_from_qml("my_title!")
            id_text.onSignal_from_qml_textChanged()  //发出信号
            //另外一种绑定qml信号和c++槽函数的方法 格式为 id.signal.connect(id.slot)
            id_text.signal_from_qml_textChanged.connect(my_title.text_change_slot_calledByQml)
    }

}

```