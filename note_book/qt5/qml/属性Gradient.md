## Gradient 渐变图形 QML类型 
渐变时由两种或多种颜色定义的，可以无缝混合 
颜色被指定为一组GradientStop子项，每个子项都定义了渐变上从0.0到1.0的位置以及一种颜色，每个GradientStop的位置是通过设置其position属性来定义的，其颜色是使其color属性定义的。 
没有定义任何渐变stops时将呈现白色填充 

例如：
```
Rectangle{
    width:100;height:100
    gradient:Gradient{
        GradientStop{
            position:0.0
            color:"red"
        }
        GradientStop{
            position:0.33
            color:"yellow"
        }
        GradientStop{
            position:1.0
            color:"green"
        }
    }
}

```