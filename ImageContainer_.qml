import QtQuick 2.0
import BioImages 1.0
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.12

Rectangle {
    id: imageWindow
    //    property url source: null;

    property alias imgBrightnessControl: brightnessControl
    property alias imageViewer: imgViewer
    anchors.fill: parent
    color: "#eee"
    anchors.centerIn: parent
    //     Create a flickable to view a large image.

    Image {
        id: imgViewer
        source: ""
        visible: false
        anchors.centerIn: parent
        cache: false
        MouseArea {
            enabled: true
            anchors.fill: parent
            hoverEnabled: true

            // We have to manually trigger a zoom since we have
            // our own mouse area.
            // onDoubleClicked: scaleArea.zoomAtPosition(mouse)
            onPositionChanged: {

                let x = mouse.x;
                let y = mouse.y;
                let val = Manager.intensity(x, y);
                let posInfo = ` X: ${x}, Y: ${y},  Value: ${val}`;
                appRoot.statusBar.positionInfo = posInfo;
            }

            onWheel: {

                scaleArea.zoom(wheel, 0.1)

            }
        }
    }

    BrightnessContrast {
         id: brightnessControl
         property real scaleFactor: 0.1
         anchors.fill: imgViewer
         source: imgViewer
         brightness: 0.0
         contrast: 0.0
//             scale: 5

    }

    Connections{
        target: Manager
        function onImageLoaded() {
            imgViewer.source= ""
            imgViewer.source= "image://Bi/a"
        }
    }




    /*ScrollView {
        id: view
        clip: true
        anchors.centerIn: parent

        width: parent.width < imgViewer.width ? parent.width : imgViewer.width > 0 ? imgViewer.width : parent.width
        height: parent.height < imgViewer.height ? parent.height : imgViewer.height > 0 ? imgViewer.height : parent.height

        background: Rectangle {
            width: parent.width
            height: parent.height
            color: "#eee"
        }

        Image {
            id: imgViewer

            source: ""
//            scale: 5
            visible: false

        }

        Connections{
            target: Manager
            function onImageLoaded() {
                imgViewer.source= "image://Bi/a"
            }
        }

//        BioImage2DViewer {
//            id: imgViewer
//            anchors.centerIn: parent
//            property real scaleFactor: 0.1
//            MouseArea {
//                hoverEnabled: true
//                anchors.fill: parent
//                onPositionChanged: {

//                    let x = mouse.x;
//                    let y = mouse.y;
//                    let val = imgViewer.pixelValue(x, y);
//                    let posInfo = ` X: ${x}, Y: ${y},  Value: ${val}`;
//                    appRoot.statusBar.positionInfo = posInfo;
//                }
//                onWheel: {
//                    console.log("Button pressed : ", wheel.modifiers == Qt.ControlModifier)
//                    console.log("Angle Delta : ", wheel.angleDelta)
//                    console.log("Pixel Delta : ", wheel.pixelDelta)
//                    if(wheel.modifiers == Qt.ControlModifier) {
//                        if(wheel.angleDelta.y > 1) {
//                            console.log("scalling : ", imgViewer.scale, " to : ", imgViewer.scale+ imgViewer.scaleFactor)
//                            imgViewer.scale = 5;//imgViewer.scale+ imgViewer.scaleFactor
//                        } else {
//                            imgViewer.scale = 1; //imgViewer.scale - imgViewer.scaleFactor
//                        }
//                    }
//                }

//            }


//        }

        BrightnessContrast {
             id: brightnessControl
             property real scaleFactor: 0.1
             anchors.fill: imgViewer
             source: imgViewer
             brightness: 0.0
             contrast: 0.0
//             scale: 5
             MouseArea {
                 hoverEnabled: true
                 anchors.fill: parent
                 onPositionChanged: {

                     let x = mouse.x;
                     let y = mouse.y;
                     let val = Manager.intensity(x, y);
                     let posInfo = ` X: ${x}, Y: ${y},  Value: ${val}`;
                     appRoot.statusBar.positionInfo = posInfo;
                 }
                 transform:[ Scale {
                     id: zoomer
                     origin.x : 0
                     origin.y : 0

                     xScale : 2.0
                     yScale : 2.0
                 }]

                 onWheel: {
//                     console.log("Button pressed : ", wheel.modifiers == Qt.ControlModifier)
//                     console.log("Angle Delta : ", wheel.angleDelta)
//                     console.log("Pixel Delta : ", wheel.pixelDelta)
                     if(wheel.modifiers == Qt.ControlModifier) {
                         if(wheel.angleDelta.y > 1) {
                             console.log("scalling : ", zoomer.xScale, " to : ", zoomer.xScale + brightnessControl.scaleFactor)
                             zoomer.origin.x = wheel.x
                             zoomer.origin.y = wheel.y

                             zoomer.xScale -= brightnessControl.scaleFactor
                             zoomer.yScale -= brightnessControl.scaleFactor
                         } else {
                             zoomer.origin.x = wheel.x
                             zoomer.origin.y = wheel.y
                             zoomer.xScale += brightnessControl.scaleFactor
                             zoomer.yScale += brightnessControl.scaleFactor

                         }
                     }
                 }

             }

         }

    }*/

}

