## Window  
Window对象为Qt Quick场景常见一个顶级窗口。  


## 信号函数 
```qml

 Connections {
    target: id_root
    function onVisibleChanged() {
        console.log("windows close")
        signalexit()
    }
}

Connections{
    target: id_root
    function onActiveChanged(){
        console.log("active changed " + id_root.active)
    }
}

Connections{
    target: id_root
    function onWidthChanged(){
        console.log("width changed " + id_root.width)
    }
}

Connections{
    target: id_root
    function onXChanged(){
        console.log("window x changed " + id_root.x)
    }
}

Connections{
    target: id_root
    function onClosing(){
        console.log("close")
        signalexit()
    }
}


``` 


### 当有多个窗口需要跳转时    
使用`Loader`可以在有需要的时候的再创建元素
```xml
Window{
    id: id_mainwindow
    
    function showlogin(){
        id_loder.sourceComponent = id_login_page
    }

    function showuser(){
        id_loder.sourceComponent = id_user_page
    }

    Rectangle{
        width: 200, height: 200
        Loader{
            id: id_loder
            anchors.fill: parent
            sourceComponent: id_login_page
        }
    }

    Component{
        id: id_blue
        Rectangle{
            anchor.fill: parent
            color:"blue"
            
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    id_loader.sourceComponent = id_black
                    console.log("click blue")
                }
            }
        }
    }

    Component{
        id: id_black
        Rectangle{
            anchor.fill: parent
            color:"black"
            
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    id_loader.sourceComponent = id_blue
                    console.log("click black")
                }
            }
        }
    }
}
```