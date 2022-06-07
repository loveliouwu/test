

- 定义一个Item，通过MouseArea和AnimatedSprite来实现一个按钮的动画
```XML
import QtQuick 2.0
import QtQuick.Controls 1.0

Item {
    //定义一些全局属性，便于设置
    property int framewidth: 950/10  //帧的宽度
    property int frameheight: 95    //帧的高度
    property int framecount: 10 //帧数
    property int frameduration: 100 //帧的停留时间
    property url btnurl: "qrc:/images/360/btm/qinglilaji_yijianqingli_Anima_Hover.png"  //帧的地址
    property string btmTitle: "电脑清理"
    property int clickType: 0
    signal signalClickedBtn(int clicktype); //0 close 1:min 2:menu 3:feedback
    clip: true

    width: framewidth
    height: frameheight+txtId.height

    MouseArea{
        id:ma
        anchors.fill: animated
        hoverEnabled: true
        onEntered: {
            console.log("onEntered")
            animated.running = true;  //开始播放动画
            animated.resume();
        }
        onExited: {
            console.log("onExited")
            animated.running = true;
            animated.currentFrame = 0
            animated.pause()
        }
    }
    AnimatedSprite {
        id: animated;
        width:framewidth ;
        height: frameheight;
        source: btnurl;
        frameWidth:framewidth;
        frameHeight: frameheight;
        frameDuration: frameduration;
        frameCount: framecount;
        frameX: 0;
        frameY: 0;
        currentFrame: 0
        onCurrentFrameChanged: {
            //console.log("%1/%2".arg(animated.currentFrame).arg(animated.frameCount));
            if(currentFrame == frameCount-1 ){
                console.log("已结动画完毕，请停止")
                animated.pause();
                animated.running = false;
            }
        }
        Component.onCompleted: {
            running = false;  //初始时不播放
        }
    }
    DDText{
        id:txtId
        text: btmTitle  //设置按钮的文字
        width: parent.width
        height: 30
        anchors.bottom: parent.bottom
    }
}
```