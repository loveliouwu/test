# 矩形组件Rectangle 

``` 
Rectangle{
    id: rect_buttom
    x:100;y:100  //相对于父窗口的坐标
    width:200;height:200 
    visible:true //可见属性设置，true为可见，默认为true 
    color:"red" // #667788
    radius:5 //设置矩形圆角弧度
    border.color:"green" //边框颜色
    border.width:5 //边框大小
    smooth:true //设置平滑度
    clip:true //剪裁，默认false 如果它的子窗体超过本窗体的显示范围则超出去的部分不显示
    
    implicitWidth:200 //可以理解为推荐宽度 
    implicitHeight:50

    gradient:Gradient{  //渐变属性
        //从上到下颜色渐变，需要从左到右使用属性rotation设置90即可
            GradientStop{
                position:0.0;color:"red"
            }
            GradientStop{
                position:1.0;color:"blue"
            }
    }

    opacity: 0.5  //设置透明度,0-1,0为完全透明，1表示不透明

}
```
- GradientStop（倾斜点）的颜色标记了颜色的位置  
 

