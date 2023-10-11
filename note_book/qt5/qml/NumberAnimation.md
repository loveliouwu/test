```xml
Item {
    id: id_root

    anchors.fill: parent
    focus: true
    Rectangle{
        id: id_rec
        color: "gray"
        width: 40
        height: 40
        NumberAnimation on x{
            to: parent.width - id_rec.width
            duration: 1000
        }

    }
    Text{
        id: id_text1
        text: "FFFF"
        NumberAnimation on font.pixelSize {
            from: 1
            to: 40
            duration: 2000
        }
    }
    Text{
        id: id_text2
        anchors.top: id_text1.bottom
        text: "text2"
        ParallelAnimation {
            running: true
            NumberAnimation {
                target: id_text2
                properties: "font.pixelSize"
                to: 40
                duration: 1000
            }
            ColorAnimation{
                target: id_text2
                properties: "color"
                from: "blue"
                to: "green"
                duration: 1000
            }
        }
    }
}

```