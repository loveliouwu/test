### Repeater重复元素    
通常与定位器共同使用  

- index属性，表示当前的循环索引值，从0开始

```qml
Grid{
    anchors.fill: parent
    anchors.margins: 8
    spacing: 4
    Repeater{
        model: 16
        Rectangle{
            width: 50; height: 50
            color: 8

            Text{
                text: "Cell " + index //index是Repeater的属性值
            }
        }
    }

}

```