import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls

Item{
    id: root
    property ListModel menumodel
    Rectangle{
        color: "gray"
        anchors.fill: parent

        Menu{
            id: menu
            Instantiator{
                id: ins
                model: menumodel
                delegate: MenuItem{
                    text: name
                    onTriggered: menu.target(name)
                }
                onObjectAdded: function(index, object) { 
                //在 QT6版本之后语法上需要改变 
                //参考 https://stackoverflow.com/questions/69273910/injection-of-parameters-into-signal-handlers-is-deprecated-use-javascript-funct
                                menu.insertItem(index, object)
                               }             
                function target(name){
                    console.debug("triggered: " + name)
                }
            }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                var pos = mapToGlobal(mouseX, mouseY)
                menu.popup(pos.rx, pos.ry)
            }
        }
    }
}
