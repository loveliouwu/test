


```
import QtQuick
import QtQuick.Controls 2.15
Item {
    id: id_root
    anchors.fill: parent

    Rectangle{
        width: 50
        height: 50
        color: "gray"
        TapHandler{
            id: id_tap
            // 默认acceptedButtons的值设置为Qt.LeftButton
            acceptedModifiers: Qt.NoModifier  //限定只有当左键按的时候触发
            onTapped: {
                console.log("tapCount: " + tapCount)
                console.log("left clicked")
            }


            onSingleTapped: {
                console.log("single tapped")
            }
            onDoubleTapped: {
                console.log("double tapped")
            }


        }
        TapHandler{
            id: id_tap1
            acceptedButtons: Qt.RightButton
            acceptedModifiers: Qt.NoModifier  //限定只有当右键按的时候触发
            onTapped: {
                console.log("right clicked")
            }
            onEnabledChanged: {
                if(enabled === true){
                    console.log("right enabled")
                }else
                {
                    console.log("right disabled")
                }
            }
        }
        TapHandler{
            id: id_tap2
            enabled: true //如果关闭enable，则所有的事件都不会被触发
            // 默认acceptedButtons的值设置为Qt.LeftButton
            acceptedModifiers: Qt.ControlModifier //限定ctrl和左键同时按的时候触发
            onTapped: {
                console.log("ctrl-tapped")
                //id_tap1.enabled = id_tap1.enabled === true ? false : true
                id_tap1.enabled = !id_tap1.enabled
            }
        }
    }
}


```