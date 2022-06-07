### 图像元素 (Image Element)  

一个图像元素能够显示不同格式的图像，`source`属性提供了图像文件的连接信息，`fillMode`（文件模式）属性能够控制元素对象的大小调整行为   

```qml
Image{
    x: 12; y: 12
    //width: 50
    //height: 120
    source: "assets/rocket.png"
    fillMode: Image.PreserveAspectCrop  //避免剪裁图像数据被渲染到图像边界外
    clip: true //默认情况下 裁剪属性是被禁用的false，打开后来约束边界矩形的绘制
}

```