import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Mybuttom{
        id: mybut1
        x: parent.x; y:parent.y
        height: 50
        width: parent.width
        onClicked: {
            text_color.text = "Clicked but1"
            text_color.font.pixelSize--
        }
        onPressing: {
            clicked()
        }
        onDoubleclicked: {
            mybut1.scale -= 0.1
            if(mybut1.scale <= 0)
                mybut1.scale = 1
            console.debug("scale = %d",mybut1.scale)
        }
    }

    Mybuttom{
        id: mybut2
        x: parent.x; y:mybut1.y + mybut1.height
        height: 50
        color: "blue"
        width: parent.width
        onClicked: {
            text_color.text = "Clicked but2"
            text_color.font.pixelSize++
        }
        onPressing: {
            clicked()
        }
        onDoubleclicked: {
            mybut2.rotation += 5
        }
    }

    Text{
        id: text_color
        x: parent.width/2 - text_color.width/2
        y: mybut1.height + mybut2.height
        color: "Red"
        text: "Click me !"
    }


    Flow{
        id: flow
        x: parent.x; y: text_color.height + mybut1.height + mybut2.height
        height: 100
        width: parent.width
        spacing: 8
        Repeater{
            model: 8
            Mybuttom{
                width: 100
                height: parent.height
                onClicked: {
                    console.debug("Cell " + index)
                }
            }
        }
    }

    Rectangle{
        anchors.top: flow.bottom
        anchors.topMargin: 5
        border.color: "blue"
        height: 20
        width: parent.width
        TextInput{
            id: input1
            anchors.fill: parent
            font.pixelSize: 18
            color: "red"
            text: "TextInput test"
            selectionColor: "pink"
            selectByMouse: true
            horizontalAlignment: TextInput.AlignHCenter
            maximumLength: 32
            validator: IntValidator{bottom: 0; top: 5555}

            Keys.onRightPressed: {
                console.debug("left press !")
            }
            onLengthChanged: {
                console.debug("length : %d", length)
            }
            onTextChanged: {
                console.debug("val : %d", displayText)
            }
            onAccepted: {
            }

        }
    }


}
