import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls 1.4
import QtCharts 2.14
import QtQuick.Layouts 1.12

Item {
    id: chartDisplay
    Rectangle {
        id: chartContainer
        anchors.fill: parent
        ToolBar {
            id: chartToolBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: 50

            RowLayout {
                anchors.verticalCenter: parent.verticalCenter
                height: parent.height
                TextField {
                    id: binSize
                    width: 100
                    validator: IntValidator{
                        bottom: 1
                    }
                    placeholderText: "Bin Size"
                    Keys.onEnterPressed: {
                        Manager.setModelBinSize(parseInt(binSize.text))
                    }
                }


                ToolButton {
                    implicitHeight: parent.height
                    text: "Apply"
                    onClicked: {
                        Manager.setModelBinSize(parseInt(binSize.text))
                    }
                }

                ToolButton {
                    implicitHeight: parent.height
                    text: "Toggle view"
                    onClicked: {
                        tableView.visible = !tableView.visible
                    }
                    iconSource: "qrc:/icons/toggle.png"

                }

                ToolButton {
                    iconSource: "qrc:/icons/settings.png"
                }
            }
        }


        TableView {
            id:tableView
            anchors.top: chartToolBar.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            model: Manager.mode()
            width: parent.width/2
            TableViewColumn {
                role: "I"
                title: "Index"
                width: 200
            }
            TableViewColumn {
                role: "X"
                title: "Bin"
                width: 200
            }
            TableViewColumn {
                role: "Y"
                title: "Mean"
                width: 200
            }
            visible: false
        }

        ChartView {
            id: chartView
            anchors.top: chartToolBar.bottom
            anchors.left: tableView.visible ? tableView.right : parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.width/2
            legend.visible: false
            animationOptions: ChartView.SeriesAnimations
//            theme: ChartView.ChartThemeHighContrast



            ValueAxis {
                id: chartViewX
                min:0
                max: Manager.modelBinCount()
                tickCount: 20
                tickType: ValueAxis.TicksFixed
            }
            ValueAxis{
                id: chartViewY
                min: Manager.modelYMin()
                max: Manager.modelYMax();
//                tickInterval: 0.001
                tickCount: 20
                tickType: ValueAxis.TicksFixed
            }

            LineSeries {
                VXYModelMapper {
                    model: Manager.mode()
                    xColumn: 0
                    yColumn: 2
                }

                axisY: chartViewY
                axisX: chartViewX
            }

            Connections {
                target: Manager
                function onDataModelUpdated() {

                    chartViewX.max = Manager.modelBinCount();
                    chartViewY.min = Manager.modelYMin();
                    chartViewY.max = Manager.modelYMax();
                }

            }
        }
    }

}
