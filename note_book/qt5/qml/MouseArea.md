### 鼠标区域元素（MouseArea Element）  
这是一个矩形的非可视化元素对象，你可以通过它来捕捉鼠标事件。通常用来与**可视化元素**对象一起执行命令   

```qml
Rectangle{
    id: rect1  
    x: 12; t: 12
    width: 80; height: 160
    color: "lightsteelblue" 
    MouseArea{
        id: area
        width: parent.width
        height: parent.height
        onClicked: rect2.visible = !rect2.visible  
    }
}

RectangleP{
    id: rect2
    x:50; y: 50
    width: 55; height: 55
    border.color: "lightsteelblue" 
    border.width: 4
    radius: 8  //设置矩阵圆角弧度
}

```