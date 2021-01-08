import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: rootImageScrollArea

    property alias zoomController: zoomSlider
    property string imageSrc: ""
    property real zoomScale: 1.0
    property var controlAlignment: Qt.AlignLeft
    clip: true
    Rectangle {
        id: imageContainer
        Image {
            id: image
//            source: imageSrc
        }

        width: image.width
        height: image.height
        color: "transparent"
        smooth: false
        antialiasing: false
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            hoverEnabled: true
            onPositionChanged: {
                let x = Math.round(mouse.x);
                let y = Math.round(mouse.y);
                let val = Manager.intensity(imageSrc, x, y);
//                let posInfo = ` X: ${x}, Y: ${y},  Value: ${val}`;
                localtionInfo.xPos = x;
                localtionInfo.yPos = y;
                localtionInfo.intensity = val;

            }
        }

        scale: zoomScale

        Component.onCompleted: updateImageLocation()
    }

    onWidthChanged: {
        updateImageLocation()
    }
    onHeightChanged: {
        updateImageLocation()
    }

    onImageSrcChanged: {
        thresholdSlider.from = Manager.imgMin(imageSrc)
        thresholdSlider.first.value = Manager.imgMin(imageSrc)
        thresholdSlider.to = Manager.imgMax(imageSrc)
        thresholdSlider.second.value = Manager.imgMax(imageSrc)
        updateImage();
        updateImageLocation();
    }

    function updateImage() {
        var imageUrl = `image://Bi/img/${imageSrc}/cmap/${colorMapCmb.currentText}/min/${thresholdSlider.first.value}/max/${thresholdSlider.second.value}/t/1`;
        image.source = ""
        image.source = imageUrl;
    }
    function updateImageLocation() {
        imageContainer.x =  (rootImageScrollArea.width/2) - (image.width / 2)
        imageContainer.y = (rootImageScrollArea.height/2) - (image.height / 2)
    }

    RowLayout {
        x: controlAlignment === Qt.AlignLeft ? 50 : (parent.width  - childrenRect.width) -50
        y: parent.height - 60
        spacing: 10
        opacity: 1

        Rectangle {
            id:localtionInfo
            implicitWidth: 100
            implicitHeight: 4
            property real xPos: 0
            property real yPos: 0
            property real intensity: 0
            color: "transparent"
            Text {
                anchors.fill: parent
                text: `X : ${localtionInfo.xPos}, Y: ${localtionInfo.yPos}, \nValue: ${localtionInfo.intensity.toFixed(3)}`
            }
        }

        ControlSlider {
            id: zoomSlider
            from: 0.7
            value: 1.0
            to: 15.0

            title:"Zoom"
            onMoved: {
                zoomScale = value
            }
        }

        ThresholdSlider {
            id: thresholdSlider
            from:0
            to: 255

            first.value: 0
            first.onMoved: updateImage();

            second.value: 255
            second.onMoved: updateImage();

        }

        ComboBox {
            id: colorMapCmb
            model: Manager.availableColorMaps();
            onActivated: updateImage()

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

        /*MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onEntered: {
                parent.opacity = 1
            }
            onExited: {
                parent.opacity = 0.3
            }
        }*/

    }

}
