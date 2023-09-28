```
import QtQuick 2.0
import QtQuick.Controls
Item {
    id: id_root

    anchors.fill: parent

    MouseArea{
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: function(mouse){
            if(mouse.button === Qt.RightButton){
                id_menu.popup()
            }
        }


        Menu{
            width: 80
            background: Rectangle{
                anchors.fill: parent
                color: "gray"
            }
            margins: 1
            padding: 1
            spacing: 3
            id: id_menu
            MenuItem{
                text: "Cut"
            }
            MenuItem{
                text: "Copy"
            }
            MenuItem{
                text: "Paste"
            }
            Action {
                property string name: "ac1"
                text: "ac1"
            }
            Action {
                property string name: "ac2"
                text: "ac2"
            }
            Action {
                property string name: "ac3"
                text: "ac3"
            }
            delegate: MenuItem{
                id: id_item
                contentItem:Rectangle{
                    id: id_rec
                    anchors.fill: parent
                    Row{
                        spacing: 0
                        anchors.fill: parent
                        CheckBox{
                            id: id_checkbox
                            height: parent.height
                        }
                        Rectangle{
                            id: id_text_rec
                            color: "pink"
                            height: parent.height
                            width: parent.width - id_checkbox.width
                            Text{
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                anchors.fill: parent
                                text: id_item.text
                            }
                            MouseArea{
                                anchors.fill: parent
                                hoverEnabled: true
                                onContainsMouseChanged: {
                                    if(containsMouse){
                                        id_text_rec.color = "yellow"
                                    }
                                    else{
                                        id_text_rec.color = "pink"
                                    }
                                }
                                onClicked: {
                                    console.log("click " + id_item.text)
                                    id_menu.close()
                                }
                            }
                        }
                    }
                }

            }


        }
    }

}


```