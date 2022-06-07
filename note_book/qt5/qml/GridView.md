### GridView   
网格视图不依赖于元素间隔和大小来配置元素，它使用单元宽度`cellWidth`与单元高度`cellHeight`属性来控制数组内的二维元素的内容。  
```qml
GridView{
    anchors.fill: parent
    anchors.margins: 20
    clip: true
    model: 100

    cellWidth: 45
    cellHeight: 45

    delegate: numberDelegate 
}
Component{
    id: numberDelegate 
    Rectangle{
        width: 40; height: 40
        color: "lightGreen" 
        Text{
            anchors.centerIn: parent 
            font.pixelSize: 20
            text: index
        }
    }
}

```