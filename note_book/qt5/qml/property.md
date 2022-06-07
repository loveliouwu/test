# QML自定义属性 
property <propertyType> <propertyName> [:<value>] 
类似于成员变量，不同的时可以初始化，并且没有public、private、protected等限制 

基础类型属性如：int、double、bool、string、variant、color、real、vector3d、url、time、rect、size、enumeration、font、date、point、list、action等

自定义属性完成后会自动为这个属性创建一个属性值改变的信号和响应的信号处理器on****Changed，首字母大写，如下所示： 
```
Rectangle{
    id:root
    property string someText
    onSomeTextChanged: console.log("the someText will be:" + someText)

}
``` 
propertyName以一个小写字母开头，只能包括字母、数字和下划线，propertyType可以是QML基本类型，也可以是QML对象类型 


## 自定义属性列表
```
Item{
    property list<Rectangle> siblingRects 
    property list<Rectangle> childRects:[
        Rectangle{color:"red"},
        Rectangle{color:"green"},
        Rectangle{color:"blue"}
    ]

    Component.onCompleted:{
        for(var i=0;i<childRects.length;i++)
            console.log("color of lable", i ,":",childRects[i].color)
    }

}
```


## 分组属性 
可以用`.`或者愤怒符号对其进行复制
```
Text{
    font.pixelSize:12
    font.bold:true
}
Text{
    font{pixelSize:12;bold:true}
}
```