import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

Rectangle {
    id: anisotropyView
    anchors.fill: parent

    // main ToolBar
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
                iconSource: "qrc:/icons/new.png"
                onClicked: {
                    newAnisotropyDlg.open()
                }
            }

            ToolButton {
                implicitHeight: parent.height
                text: "Open"
                iconSource: "qrc:/icons/open.png"
                onClicked: {
                    openDlg.open()
                }
            }
            ToolButton {
                implicitHeight: parent.height
                text: "Save"
                iconSource: "qrc:/icons/save.png"
                onClicked: {
                    saveDlg.open();
                }

                enabled: (splitView.imageRSrc != "" ) && (splitView.imageTSrc !="")

            }
        }
    }
    // \ TabBar

    // Customizes TabWdiget
    TabBar {
        id:tabBar
        anchors.top: toolBar.bottom
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

        SplitImageView {
            id: splitView
        }

        ChartDisplay {
            id:chartView
        }


    }

    // \ Customizes TabWdiget

    // All Dialogs

    NewInputDialog {
        id: newAnisotropyDlg
        onAccepted: {
            Manager.exeAnisotropy(newAnisotropyDlg.parlBgFile, newAnisotropyDlg.perpBgFile,
                                  newAnisotropyDlg.parlFile, newAnisotropyDlg.perpFile,
                                  newAnisotropyDlg.subtractValue)
        }
    }

    FileDialog {
        id: saveDlg
        title: "Select Save Directory"
        selectExisting: true
        selectMultiple: false
        selectFolder: true

        onAccepted: {
            Manager.saveAnisotropy(fileUrl, splitView.imageRSrc, splitView.imageTSrc);
        }
    }

    FileDialog {
        id: openDlg
        title: "Select Save Directory"
        selectExisting: true
        selectMultiple: false
        selectFolder: false
        nameFilters: ["Anisotropy Metafile (*.ani)"]
        onAccepted: {
            Manager.openAnisotropy(fileUrl);
        }
    }


    // \ All Dialog

    // Connections
    Connections {
        target: Manager
        function onAnisotropyReady(idR, idT){
            splitView.imageRSrc = idR;
            splitView.imageTSrc = idT;
        }

    }
}
