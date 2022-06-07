### 锚点布局anchor  
锚点分为锚线、边距、偏移、边界用法    
anchoring（锚定）是基础元素对象的基本属性，可以被所有的可视化QML元素使用，

- 锚线  
    - left 
    - right
    - top
    - bottom
    - horizontalCenter 水平居中
    - verticalCenter   垂直居中
    - baseline 
- 边距
    - leftMargin
    - rightMargin
    - topMargin
    - bottomMargin

- 偏移
    - horizontalCenterOffset
    - verticalCenterOffset

- 便捷用法  
    - centerIn
    - margins


- 锚线都是不可见的  
```qml
anchors.left: parent.left//表示本元素的左侧锚线在父级的左侧锚线  

anchors.centerIn: parent//表示本元素在父级的中心 

anchors.horizontalCenter: parent.horizontalCenter
anchors.verticalCenter: parent.verticalCenter//这两句的效果等同于上一句
```