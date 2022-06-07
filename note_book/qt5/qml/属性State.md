### 状态 State  
在QML中，使用State元素来定义状态，需要与基础元素对象Item的states序列属性连接，状态通过它的状态名来鉴别。  

```qml
Rectangle{
    id: root
    color: "white"
    Text{
        anchor.fill: parent
        id: text
        text: "defalut text"
    }

    states:[
        State{
            name: "go"
            PropertyChanges{
                target: text  //将元素对象与属性绑定
                text: "state-go"  //元素的状态
            }
            PropertyChanges{ //如果有多个target
                target: root
                color: "red"
            }
        },
        State{
            name: "stop"
            PropertyChanges{
                target: text
                text: "state-stop"
            }
            PropertyChanges{ //如果有多个target
                target: root
                color: "yellow"
            }
        }
    ]

    MouseArea{
        id: changeState
        onClicked: root.state = "go" //通过这种方式改变状态
    }

}

```