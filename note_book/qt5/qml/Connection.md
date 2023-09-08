## Connection  
用于绑定信号  

```c++
Rectangle{
    property int size: 50
    signal signal_close()
    id: id_rec
    width: size
    height: size
    color: "blue"
    Component.onDestruction: {
        signal_close()
    }
}
Connections{
    target: id_rec
    //基本属性的槽函数
    function onColorChanged(){
        console.log("Color change :" + id_rec.color)
    }
    //自定义属性的信号槽
    function onSizeChanged(){
        console.log("size change :" + id_rec.size)
    }
    //自定义信号的槽函数
    function onSignal_close(){
        console.log("signal close emit")
    }
}

```