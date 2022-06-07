import QtQuick 2.0
import QtQuick 2.14
import QtQuick.Controls 1.0

Item {
    id:but_amo
    property string but_text: "ButtonAmio"
    property int amo_width: 95
    property int amo_height: 95
    property int amo_count: 10
    property int amo_duration: 100
    property string amo_url: "qrc:/but/images/youhuajiasu_Anima_Hover.png"

    onAmo_widthChanged: {
        console.log("amo_wid changed!");
    }

    signal signalclicked(int num)

    width: amo_width
    height: amo_height
    clip: true

    MouseArea{
        anchors.fill: parent
        parent: but_amo_sprite

        hoverEnabled: true
        onEntered: {
            console.log("in");
            but_amo_sprite.running = true;
            but_amo_sprite.resume()
        }

        onExited: {
            but_amo_sprite.running = false;
            but_amo_sprite.currentFrame = 0;
            but_amo_sprite.pause();
        }

        onClicked: {
            emit: signalclicked(amo_count);
        }
    }

    AnimatedSprite {
        id:but_amo_sprite
        frameCount:amo_count
        frameWidth:amo_width
        frameHeight:amo_height
        frameX:0
        frameY:0
        source:amo_url
        Component.onCompleted:{
            but_amo_sprite.running = false
        }

        onCurrentFrameChanged: {
            if(currentFrame == amo_count-1 ){
                console.log("已结动画完毕，请停止")
                but_amo_sprite.pause();
                but_amo_sprite.running = false;
            }
        }
        onFrameCountChanged:{
            console.log("framCount change" + frameCount);
        }
    }
}
