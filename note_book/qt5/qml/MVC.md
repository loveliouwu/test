```
Item {
    id: id_root

    anchors.fill: parent
    focus: true

    Component{
        id: id_delegate
        Rectangle{
            width: 150
            height: 50
            Row{
                spacing: 2
                anchors.fill: parent
                Label{
                    text: name
                    color: textcolor
                }
                Label{
                    text: age
                    color: textcolor
                }
            }
            border.width: 2
            border.color: "gray"
        }
    }

    ListModel{
        id: id_model
        ListElement{
            name: "ele1"
            age: 10
            textcolor: "blue"
        }
        ListElement{
            name: "ele2"
            age: 10
            textcolor: "black"
        }
    }

    ListView{
        id: id_view
        width: 300
        height: parent.height
        model: id_model
        delegate: id_delegate
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            console.log(Qt.hsla(Math.random(), 0.5, 0.5, 1.0))
            var data = {'name': "ele" + id_model.count, 'age':10, textcolor: Qt.hsla(Math.random(), 0.5, 0.5, 1.0).toString() }
            id_model.insert(0,data)
        }
        onDoubleClicked: {
            var delname = "ele" + Math.round(Math.random()*10+1)
            console.log(delname)
            for(var i = 0; i < id_model.count; i++){
                var data = id_model.get(i)
                if(data.name === delname){
                    id_model.remove(i,1)
                }
            }
        }
    }
}


```