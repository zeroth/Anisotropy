import QtQuick 2.0
import QtQuick.Controls 2.14
import QtCharts 2.14

RangeSlider {
    id: control
    first.value: 0.25
    second.value: 0.75
    ChartView {
        id:myChart
        title: "Bar series"
        //                   anchors.fill: parent
        //                   legend.alignment: Qt.AlignBottom
        antialiasing: true
        height: control.availableHeight / 2
        width: control.availableWidth
        ValueAxis {
            id: valueAxisX
            min: 0
            max: 255
        }
        ValueAxis {
            id: valueAxisY
            min: 0
            max: 200
        }





    }
    Component.onCompleted:  {

        var line = myChart.createSeries(ChartView.SeriesTypeLine, "Line series", valueAxisX, valueAxisY);

        line.append(0,0)
        line.append(127, 200)
        line.append(250, 170)
    }

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: control.availableWidth
        height: implicitHeight
        radius: 2
        color: "#bdbebf"
        //              border.color: "red"
        Rectangle {
            x: control.first.visualPosition * parent.width
            width: control.second.visualPosition * parent.width - x
            height: parent.height
            color: "#21be2b"
            radius: 2
            //                  border.color: "black"
        }


    }

    first.handle: Rectangle {
        x: control.leftPadding + first.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 26
        implicitHeight: 26
        radius: 13
        color: first.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
        Text {
            width: 32
            height: 32
            y: parent.height + 5
            text: control.first.value.toFixed(2)
        }
    }

    second.handle: Rectangle {
        x: control.leftPadding + second.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 26
        implicitHeight: 26
        radius: 13
        color: second.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
        Text {
            width: 32
            height: 32
            y: parent.height + 5
            text: control.second.value.toFixed(2)
        }
    }
}
