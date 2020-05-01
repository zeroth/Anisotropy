import QtQuick 2.6
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12


Item {
    id: rootImageScrollArea
    anchors.fill: parent
    property var currentFrame: undefined
    property real surfaceViewportRatio: 1.5
    property alias zoomController: zoomSlider
    property string imageSrc:""
    property real zoomScale: zoomSlider.value
    signal imageScaled()

//    onImageSrcChanged: {
//        image.source = `image://Bi/img/${imageSrc}/cmap/${colorMapCmb.currentText}/min/${thresholdSlider.first.value}/max/${thresholdSlider.second.value}/t/1`;
//    }

    Flickable {
        id: flick

        anchors.fill: parent

        contentWidth: width
        contentHeight: height

        Rectangle {
            id: imgRect
            //            anchors.centerIn: parent
            width: image.width
            height: image.height
            color: "transparent"

            smooth: true
            antialiasing: true
            Component.onCompleted: {
                x =  (rootImageScrollArea.width/2) - (image.width / 2)
                y = (rootImageScrollArea.height/2) - (image.height / 2)
            }

            scale: zoomScale
            Image {
                id: image
                cache: false
                source: imageSrc
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                visible: true
            }
//            BrightnessContrast {
//                id: image
//                anchors.fill: image_src
//                source: image_src
//                brightness: 0.0
//                contrast: 0.0
//                antialiasing: true
//            }


            /*MouseArea {
                id: dragArea
                hoverEnabled: true
                anchors.fill: parent
                drag.target: imgRect
                scrollGestureEnabled: true  // 2-finger-flick gesture should pass through to the Flickable

                onPositionChanged: {
                    let x = Math.round(mouse.x);
                    let y = Math.round(mouse.y);
                    let val = Manager.intensity(x, y);
//                    let posInfo = ` X: ${x}, Y: ${y},  Value: ${val}`;
//                    appRoot.statusBar.positionInfo = posInfo;
                }

                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        if(wheel.angleDelta.y < 0) {
                            zoomSlider.decrease();
//                            rootImageScrollArea.imageScaled();
                        } else {
                            zoomSlider.increase();
//                            rootImageScrollArea.imageScaled();
                        }
                    }
                }


            }*/


        }


        Row {
            x: (parent.width  - childrenRect.width) -50
            y: parent.height - 60
            spacing: 10
            ControlSlider {
                id: zoomSlider
                from: 0.7
                value: 1.0
                to: 15.0

                title:"Zoom"
            }

            ThresholdSlider {
                id: thresholdSlider
                from:0
                to: 255

                first.value: 0
//                first.onMoved: thresholdImage();

                second.value: 255
//                second.onMoved: thresholdImage();

            }

            ComboBox {
                id: colorMapCmb
                model: Manager.availableColorMaps();
                onActivated: changeColorMap()

                background: Rectangle {
                    implicitWidth: 120
                    implicitHeight: 40
                    radius: 2
                }

                function updateDefaultSelection() {
                    colorMapCmb.currentIndex = colorMapCmb.find("Greys", Qt.MatchExactly)
                }

                Component.onCompleted:  updateDefaultSelection()
                onModelChanged: updateDefaultSelection()
            }

        }


    }
}

