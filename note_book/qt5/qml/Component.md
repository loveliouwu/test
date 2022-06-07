## 组件（Component） 是QML封好类型 
一个组件是一个可以重复使用的元素。  
Component只能包含一个顶层的Item，而且这个Item之外不能定义任何的数据，除了id 
Component通常用来给一个View提供图形化组件 
Component不是Item的派生类，而是从QQmlComponent继承而来的 

>通常建议使用基础元素对象Item作为根元素，可以提供更多相关控制的API

- 信号 
`onCompleted`在组件启动完成后发出，一旦建立了完整的QML环境，就可以在启动时用来执行脚本代码（可以理解为初始化调用函数） 
```
Item{
    id:test
    Component{
        id:component_1

        onComponent{
            console.log("init")
        }
    }

}
```




- Component 只能包含一个顶层的Item 
```
Component{
    id:root
    Rectangle{ //第一个Item
        id:rect_1
        implicitWidth:200
        implicitHeight:50 
        color: 'red'
    }

    Text{ //第二个Item ，这里会报错，不允许有第二个Item
        id:test
    }
}
```