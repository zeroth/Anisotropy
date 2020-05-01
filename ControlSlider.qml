import QtQuick 2.0
import QtQuick.Controls 2.12

Slider {
    id:mainSlider
    property string title: "slider"

    handle: Rectangle {
        x: mainSlider.leftPadding + mainSlider.visualPosition * (mainSlider.availableWidth - width)
        y: mainSlider.topPadding + mainSlider.availableHeight / 2 - height / 2
        implicitWidth: 26
        implicitHeight: 26
        radius: 13
        color: mainSlider.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
    }
    background: Rectangle {
        x: mainSlider.leftPadding
        y: mainSlider.topPadding + mainSlider.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: mainSlider.availableWidth
        height: implicitHeight
        radius: 2
        color: "#bdbebf"

        Rectangle {
            width: mainSlider.visualPosition * parent.width
            height: parent.height
            color: "#21be2b"
            radius: 2
        }
        Text {

            text: qsTr(title)
            x: mainSlider.width/2 - (width/2)
            y: 15
        }
    }
}
