### ListView

ListView同样使用了一个model，使用delegate来实例化，并且在两个delegate之间能够设置间隔spacing。  

```qml
Rectangle{
    color: "white"

    ListView{
        anchors.fill: parent
        anchors.margins: 20
        clip: true 
        model: 100
        delegate: numberDelegate
        spacing: 5 
        orientation: ListView.Horizontal

        Component{
            id: numberDelegate
            Rectangle{
                width: 40; height: 40 
                color: "lightGreen"  
                Text{
                    anchors.centerIn: parent
                    font.pixelSize: 10
                    text: index
                }
            }
        }
    }
}

```  
- 默认的链表视图只提供了一个垂直方向的滚动条。链表视图的方向由属性`orientation`控制，可以取`ListView.Vertical`、`ListView.Horizontal`    

- 水平链表视图的默认元素顺序方向时由左到右，可以通过设置`layoutDirection`属性来控制元素顺序方向，它可以设置为`Qt.LeftToRight`或`Qt.RightToLeft`  

- hightlight  设置高亮   


- 链表的页眉页脚  
页眉`header`和页脚`footer`  

```qml
Rectangle{
    anchors.fill: parent
    color: "white"  

    ListView{
        anchors.fill: parent 
        anchors.margins: 20 
        clip: true //可以被边界切割  
        model: 4
        delegate: numberDelegate //设置代理  
        spacing: 5
        
        header: headerComponent 
        footer: footerComponent 
    }

    Component{
        id: headerComponent 
        Rectangle{
            width: 40; height: 20;
            color: "red"
        }
    }

    Component{
        id: footerComponent 
        Rectangle{
            width: 40; height: 20;
            color: "yellow"
        }
    }

    Component{
        id: numberDelegate
        Rectangle{
            width: 40; height: 40;
            color: "lightGreen" 

            Text{
                anchors.centerIn: parent
                font.pixelSize: 10
                text: index
            }
        }
    }
}

```  


#### 链表分段  Lists with Sections   
为了使用分段，section.property与section.criteria必须安装。  
