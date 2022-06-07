import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls
Window {
    width: 640
    height: 480
    visible: true
    color: "white"
    title: qsTr("Hello World")


    ListModel{
        id: filemodel
        ListElement{ name:"Open.." }
        ListElement{ name:"New.."}
    }


/*
创建一个活动菜单，即在一个范围中，点击鼠标可以唤出菜单
*/
    Mymenu{
        id: menu
        menumodel: filemodel
        width: parent.width
        height: 40


    }

}