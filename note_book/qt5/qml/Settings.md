## Settings  
Settings类型提供独立于平台的永久性应用程序设置。  

```cpp
//需要在主程序添加以下代码  
QCoreApplication::setOrganizationName("appName.org");
```


- 例子： 随机颜色退出保存
```cpp

import QtQuick 2.0
import Qt.labs.settings 1.0
import QtQuick.Controls 2.5

Rectangle {
    id: root
    width: 320; height: 240
    color: '#000000'
    Settings {
        id: settings
        property alias color: root.color
        
        //设置保存设置类别的名称，可以理解为ini文件里面的section
        category: "My_Setting_name"
        //设置保存设置文件的路径，如果文件尚不存在，则创建文件
        //注： 如果多个qml使用同一个配置文件，只要文件名一样就可以
        fileName: "Setting.ini"
    }
    MouseArea {
        anchors.fill: parent
        onClicked: root.color = Qt.hsla(Math.random(), 0.5, 0.5, 1.0);
    }
    
    onColorChanged:
    {
        //将任何未保存的更改写入永久储存，此函数在析构函数自动调用，并由事件循环定期调用，通常不用自己调用
        settings.sync()
    }
}

```