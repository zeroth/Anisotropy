import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: anisotropyView
    anchors.fill: parent
    ToolBar {
        id: toolBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 50

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            height: 55
            ToolButton {
                implicitHeight: parent.height
                text: "New"
                onClicked: {
                    console.log("New clicked")
                }
            }
            ToolButton {
                implicitHeight: parent.height
                text: "Open"
                onClicked: {
                    console.log("Open clicked")
                }
            }
            ToolButton {
                implicitHeight: parent.height
                text: "Save"
                onClicked: {
                    console.log("Save clicked")
                }

            }
        }
    }


    TabBar {
        id:tabBar
        anchors.top: toolBar.bottom
//        anchors.left: parent.left
//        anchors.right: parent.right
        width: parent.width * (20/100)
        TabButton {
            text: "Images"
        }

        TabButton {
            text: "Chart"
        }
    }

    StackLayout {
        width: parent.width
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        currentIndex: tabBar.currentIndex
        SplitImageView{
//            anchors.fill: parent
        }

        Rectangle {
            id: greenRect
            color: "green"
        }
    }
}
