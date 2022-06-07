### model-view 模型-视图   
在QtQuick中，数据通过model-view分离，对于每个view，每个数据元素的可视化都分给一个代理（delegate）。  
数据通常被称作Model,可视化处理称作view  



#### 使用**Repeater**元素分离   
使用简单的单一属性来作为model
```qml
Repeater{
        id: colors
        model: ["blue", "green", "red", "pink", "yellow"]  //使用javascript序列来替换整形变量model的值
        Rectangle{
            radius: 5
            color: modelData
            y: index*50
            width: 50; height: 50
        }
    }
```  


#### 使用**ListElement**作为模型  

```qml
Column{
    spacing: 3
    Repeater{
        model: ListModel{
            ListElement{//其中的元素都可以自己取名
                name: "Mercury"; surfaceColor:"gray";wid:100
            }
            ListElement{
                name: "Venus"; surfaceColor:"yellow";wid:200
            }
            ListElement{
                name: "Mars"; surfaceColor:"blue";wid:300
            }
        }

        Rectangle{
            width: wid; height: 20
            radius: 3
            color: surfaceColor 
            Text{
                anchors.centerIn: parent
                text: name
            }

        }
    }
}

```
上面的例子中，repeater的内容的每个子项实例化时绑定了默认的属性delegate即：
```qml
    Rectangle{ ... }
    //等同于:
    delegate: Rectangle{ ... }
```

#### 动态视图  
使用Repeater元素适合有限的静态数据，但是真正使用时模型通常更加复杂和庞大，所以我们可以使用ListView和GridView元素，这两个都是基于Flickable(可滑动)区域的元素。  

##### ListView  


