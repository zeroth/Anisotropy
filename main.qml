import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.2
import QtQuick.Controls 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import BioImages 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0

import "Controller.js" as Ctrl

Rectangle {
    id: appRoot
    visible: true
    width: 1200
    height: 800
    color: "red"

    AnisotropyView {
        anchors.fill: parent
    }
}

/*ApplicationWindow {
    id: appRoot
    visible: true
    width: 1200
    height: 800
    title: qsTr("Anisotropy")
    property alias statusBar: statusBar

    AnisotropyView {
        id: rootView
        anchors.fill: parent
    }


    footer: StatusBar {
        id: statusBar
        property string positionInfo: ""
        style: StatusBarStyle {
            padding {
                left: 8
                right: 8
                top: 3
                bottom: 3
            }
            background: Rectangle {
                implicitHeight: 16
                implicitWidth: 200
                gradient: Gradient{
                    GradientStop{color: "#eee" ; position: 0}
                    GradientStop{color: "#ccc" ; position: 1}
                }
                color:  "#ccc"

                Rectangle {
                    anchors.top: parent.top
                    width: parent.width
                    height: 1
                    color: "#ababab"
                }
            }
        }
        RowLayout {
            anchors.fill: parent
            Label {
                id: posInfo
                text: statusBar.positionInfo
            }
        }
    }

    function addSingleImage(fileUrl) {
        Manager.createImage(fileUrl);
    }

    FileDialog {
        id: inputDialog
        title: "Input Dialog box"
        selectExisting: true
        selectMultiple: false
        selectFolder: false
        onAccepted: addSingleImage(fileUrl);
    }

    Shortcut {
        sequences: [StandardKey.Open]
        onActivated: inputDialog.open()
    }

    Connections {
        target: Manager
        function onImageLoaded(imageid) {
            rootView.addImage(imageid);
        }
    }
}*/
