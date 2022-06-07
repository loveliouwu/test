### 焦点区域 focus scope    
一个焦点区域定义了如果焦点区域接收到焦点，它的最后一个使用focus:true的子元素接收焦点，它将会把焦点传递给最后申请焦点的子元素。


```qml
FocusScope{
    width: 96; height: input.height + 8
    Rectangle{
        anchors.fill: parent
        color: "lightsteelblue"
        border.color: "gray"
    }

    property alias text: input.text
    property alias input: input

    TextInput{
        id: input
        anchors.fill: parent
        anchors.margins: 4
        focus: true
    }
}

```