import QtQuick 2.6
import QtQuick.Dialogs 1.0
//import QtQuick.Window 2.1
import QtQuick.Controls 2.12

Item {
    anchors.fill: parent
    id: rootImageViewer
    property var currentFrame: undefined
    property real surfaceViewportRatio: 1.5
    Flickable {
        id: flick

        anchors.fill: parent

        contentWidth: width
        contentHeight: height

        Rectangle{
            id: redRect
//            anchors.centerIn: parent
            width: image.width
            height: image.height
            color: "red"
            border.color: "black"
            border.width: 2
            smooth: true
            antialiasing: true
            Component.onCompleted: {
                x =  (rootImageViewer.width/2) - width / 2
                y = (rootImageViewer.height/2) - height / 2
            }
            scale: redRectSlider.value
            Image {
                id: image
                source: ""
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                antialiasing: true
            }


            MouseArea {
                id: dragArea
                hoverEnabled: true
                anchors.fill: parent
                drag.target: redRect
                scrollGestureEnabled: true  // 2-finger-flick gesture should pass through to the Flickable

                onPositionChanged: {
                    console.log(mouse.x, mouse.y);
                }

                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        if(wheel.angleDelta.y < 0) {
                            redRectSlider.decrease();
                        } else {
                            redRectSlider.increase();
                        }
                    }
                }
            }


        }

        Slider {
            id: redRectSlider
            from: 0.7
            value: 1.0
            to:3.0
            x:(parent.width /2) - (width/2)
            y: parent.height - 40
        }

    }

    Rectangle {
        id: verticalScrollDecorator
        anchors.right: parent.right
        anchors.margins: 2
        color: "lightgray"
        border.color: "black"
        border.width: 1
        width: 5
        radius: 2
        antialiasing: true
        height: flick.height * (flick.height / flick.contentHeight) - (width - anchors.margins) * 2
        y: (flick.contentY - flick.originY) * (flick.height / flick.contentHeight)
        NumberAnimation on opacity { id: vfade; to: 0; duration: 500 }
        onYChanged: { opacity = 1.0; scrollFadeTimer.restart() }
    }

    Rectangle {
        id: horizontalScrollDecorator
        anchors.bottom: parent.bottom
        anchors.margins: 2
        color: "lightgray"
        border.color: "black"
        border.width: 1
        height: 5
        radius: 2
        antialiasing: true
        width: flick.width * (flick.width / flick.contentWidth) - (height - anchors.margins) * 2
        x: (flick.contentX - flick.originY) * (flick.width / flick.contentWidth)
        NumberAnimation on opacity { id: hfade; to: 0; duration: 500 }
        onXChanged: { opacity = 1.0; scrollFadeTimer.restart() }
    }

    Timer { id: scrollFadeTimer; interval: 1000; onTriggered: { hfade.start(); vfade.start() } }

}
