
```
Item {
    id: id_root

    anchors.fill: parent
    focus: true

    Keys.onPressed: function(event){
        if(event.key === Qt.Key_0){
            console.log("0 pressed")
        }
    }
    Keys.onDigit1Pressed: function(event){
        console.log("1 pressed")
    }
    

}

```