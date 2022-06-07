import QtQuick 2.0

Rectangle{
    id: rect1
    color: 'black'
    signal clicked
    signal pressing
    signal doubleclicked
    smooth: true
    Text {
        id: text
        text: qsTr("objname")
        anchors.fill: parent.Center
        anchors.centerIn: parent
        font.pixelSize: 20
        color: "green"
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            rect1.clicked()
        }

        onPressAndHold: {
            timer.running = true
        }
        onReleased: {
            if(timer.running == true)
                timer.running = false
            else
                console.debug("just clicked once!")
        }
        onDoubleClicked: {
            rect1.doubleclicked()
        }
    }

    Timer{
        id: timer
        interval: 100
        repeat: true
        running: false
        onTriggered: {
            rect1.pressing()
        }
    }
}
