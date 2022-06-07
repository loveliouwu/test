### 过渡 Transitions   
一系列的过渡能够被加入任何元素，一个过渡由状态的改变触发执行，你可以使用属性的from和to来定义状态改变的指定过渡。这两个属性就像一个过滤器，当过滤器为true时，过渡生效。你也可以使用`""`来表示任何状态。  


>当我们希望从状态go到状态stop转换时实现一个颜色改变的动画，而对于从stop到go我们希望保持颜色直接改变，不使用过渡，则我们通过使用from和to来限制过渡只在从go到stop时生效
```qml
transitions:[
    Transition{
        from: "stop"; to: "go"  
        ColorAnimation{ target: light1; properties: "color"}
        ColorAnimation{ target: light2; properties: "color"}
    },
    Transition{
        target: text //item's id
        property: "font.pixelSize"
        to: 30
        from: 10
    }
]

```