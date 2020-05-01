import QtQuick 2.6
import QtQuick.Dialogs 1.0
//import QtQuick.Window 2.1
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

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
            id: imgRect
            //            anchors.centerIn: parent
            width: image.width
            height: image.height
            color: "transparent"

            smooth: true
            antialiasing: true
            Component.onCompleted: {
                x =  (rootImageViewer.width/2) - (image.width / 2)
                y = (rootImageViewer.height/2) - (image.height / 2)
            }
            scale: zoomSlider.value
            Image {
                id: image_src
                cache: false
                source: ""
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                visible: false
            }
            BrightnessContrast {
                id: image
                anchors.fill: image_src
                source: image_src
                brightness: 0.0
                contrast: 0.0
                antialiasing: true
            }


            MouseArea {
                id: dragArea
                hoverEnabled: true
                anchors.fill: parent
                drag.target: imgRect
                scrollGestureEnabled: true  // 2-finger-flick gesture should pass through to the Flickable

                onPositionChanged: {
                    let x = Math.round(mouse.x);
                    let y = Math.round(mouse.y);
                    let val = Manager.intensity(x, y);
                    let posInfo = ` X: ${x}, Y: ${y},  Value: ${val}`;
                    appRoot.statusBar.positionInfo = posInfo;
                }

                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        if(wheel.angleDelta.y < 0) {
                            zoomSlider.decrease();
                        } else {
                            zoomSlider.increase();
                        }
                    }
                }


            }


        }

        // histogram
        //        HistogramView {
        //            x: (parent.width - availableWidth) -50
        //            y: 20
        //            height: 300
        //        }


        // basic control slider
        Row {

            x: parent.width  - childrenRect.width
            y: parent.height - 50

            ControlSlider {
                id: zoomSlider
                from: 0.7
                value: 1.0
                to: 10.0

                title:"Zoom"
            }

            ControlSlider {
                id:contrastSlider
                from: -1.0
                value: 0.0
                to: 1.0
                stepSize: 0.1

                title: "Contrast"

                onMoved: {
                    image.contrast = contrastSlider.value
                }
            }
            ControlSlider {
                id:brightnessSlider
                from: -1.0
                value: 0.0
                to: 1.0
                stepSize: 0.1

                title: "Brightness"

                onMoved: {
                    image.contrast = brightnessSlider.value
                }
            }

            ThresholdSlider {
                id: thresholdSlider
                from:0
                to: 255
//                stepSize: 1.0

                first.value: 0
                first.onMoved: thresholdImage();

                second.value: 255
                second.onMoved: thresholdImage();

            }

            ComboBox {
                id: colorMapCmb
                model: Manager.availableColorMaps();
                onActivated: changeColorMap()

                background: Rectangle {
                    implicitWidth: 120
                    implicitHeight: 40
                    border.color: colorMapCmb.pressed ? "#17a81a" : "#21be2b"
                    border.width: colorMapCmb.visualFocus ? 2 : 1
                    radius: 2

                }
            }

            Rectangle {
                id: resetButton
                color: "transparent"
                radius: 15
                height: 64
                width: 64
                transformOrigin: Item.Center
                Image {
                    id: resetIcon
                    source: "qrc:/icons/reset_icon.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        reset();
                    }
                    onPressed: {
                        resetButton.scale = 0.7
                    }
                    onReleased: {
                        resetButton.scale = 1.0
                    }
                }
            }

        }

        // Get the Image
        Connections {
            target: Manager
            function onImageLoaded() {
                reset();
            }
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


    function changeColorMap() {
        image_src.source = ""
        image_src.source= `image://Bi/img/a/cmap/${colorMapCmb.currentText}/min/${thresholdSlider.first.value}/max/${thresholdSlider.second.value}/t/1`
    }

    function thresholdImage() {
        image_src.source = ""
        image_src.source= `image://Bi/img/a/cmap/${colorMapCmb.currentText}/min/${thresholdSlider.first.value}/max/${thresholdSlider.second.value}/t/1`
    }

    function reset() {


        thresholdSlider.from = Manager.imgMin();
        thresholdSlider.to = Manager.imgMax();
        thresholdSlider.first.value = Manager.imgMin();
        thresholdSlider.second.value = Manager.imgMax();
        colorMapCmb.currentIndex = colorMapCmb.find("Greys", Qt.MatchExactly)
//        console.log(Manager.colorTable("Jet"));

        image_src.source= ""
        image_src.source= `image://Bi/img/a/cmap/${colorMapCmb.currentText}/min/${thresholdSlider.first.value}/max/${thresholdSlider.second.value}/t/1`
//        thresholdSlider.stepSize = (Manager.imgMax() - Manager.imgMin())/256
        imgRect.x =  (rootImageViewer.width/2) - (image.width / 2)
        imgRect.y = (rootImageViewer.height/2) - (image.height / 2)
        zoomSlider.value = 1.0;
        contrastSlider.value =0.0
        brightnessSlider.value = 0.0
    }
    function updatePosition() {
        imgRect.x =  (rootImageViewer.width/2) - (image.width / 2)
        imgRect.y = (rootImageViewer.height/2) - (image.height / 2)
    }

    onWidthChanged: updatePosition()
    onHeightChanged: updatePosition()

}
