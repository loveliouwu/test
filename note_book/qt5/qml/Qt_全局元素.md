## Qt Type  


```c++
Component.onCompleted: {
    console.log("application.active:" + Qt.application.active)
    console.log("application.state:" + Qt.application.state)
    console.log("application.layoutDirection:" + Qt.application.layoutDirection)
    console.log("application.font:" + Qt.application.font)
    console.log("application.arguments:" + Qt.application.arguments)
    console.log("application.displayName:" + Qt.application.displayName)
    console.log("application.domain:" + Qt.application.domain)
    console.log("application.name:" + Qt.application.name)
    console.log("application.organization:" + Qt.application.organization)
    console.log("application.screens:" + Qt.application.screens)
    console.log("application.version:" + Qt.application.version)
    console.log("------------------------------------------------------")
    console.log("platform.os:" + Qt.platform.os)
    console.log("platform.pluginName:" + Qt.platform.pluginName)
    console.log("platform.objectName:" + Qt.platform.objectName)
    console.log("uiLanguage:" + Qt.uiLanguage)
    console.log("btoa(0x0a):" + Qt.btoa(0x0a))  //binary to ASCII
    console.log("btoa(\"123456789\"):" + Qt.btoa("123456789"))  //binary to ASCII
}

```

- 输出
```log
qml: application.active:false
qml: application.state:2
qml: application.layoutDirection:0
qml: application.font:QFont(Microsoft YaHei UI,9,-1,5,400,0,0,0,0,0,0,0,0,0,0,1)
qml: application.arguments:E:\code\mytest\build-quick_test-Desktop_Qt_6_2_4_MinGW_64_bit-Debug\appquick_test.exe
qml: application.displayName:appquick_test
qml: application.domain:
qml: application.name:appquick_test
qml: application.organization:
qml: application.screens:[object Object]
qml: application.version:
qml: ------------------------------------------------------
qml: platform.os:windows
qml: platform.pluginName:windows
qml: platform.objectName:
qml: uiLanguage:zh
qml: btoa(0x0a):MTA=
qml: btoa("123456789"):MTIzNDU2Nzg5

```