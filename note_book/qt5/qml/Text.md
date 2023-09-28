### 显示文本元素  
```qml
Text{
    text: "test"
    color: "#303030"
    font.family: "Ubuntu"
    font.pixelSize: 28
    elide: Text.ElideMiddle //设置省略号在文本左边、右、中间。  
    style: Text.Sunken  
    styleColor: '#FF4444'
    //verticalAlignment: Text.Aligntop
    //水平靠左
    horizontalAlignment: Text.AlignLeft
    //垂直居中
    verticalAlignment: Text.AlignVCenter
}
```  

- 可以使用`horizontalAlignment`与`verticalAlignment`属性来设置它的对齐效果。  

- 为了提高文本的渲染效果，可以使用`style`和`styleColor`属性来配置文字的外框效果，浮雕效果或者凹陷效果。对于过长的文本，你可能需要使用省略号来表示，例如A very ... long text，你可以使用`elide`属性来完成这个操作。   

- 一个text元素只显示的文本，它不会渲染任何背景修饰，除了显示的文本，text元素背景是透明的，为一个文本元素提供背景是你自己需要考虑的问题。  