import QtQuick 2.0
import QtQuick 2.14
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import BioImages 1.0
import "Controller.js" as Ctrl

RowLayout {
    id: layout
    property url parlBgFile : ""
    property url perpBgFile : ""
    property url parlFile : ""
    property url perpFile : ""
    property double subtractValue: 0

    anchors.fill: parent
    spacing: 0
    Rectangle {
        id:left
        color: "#eee"

        Layout.fillHeight: true
        Layout.preferredWidth: parent.width * (70/100)

        ImageVisualizer {
            id: imgContainer
            anchors.fill: parent
        }
    }

    Rectangle {
        id:right

        Layout.fillHeight: true
        Layout.preferredWidth:parent.width * (30/100)
        GridLayout{
            id: gridLayout
            anchors.centerIn: parent
            rows: 6
            columns: 6

            Label {
                text: "Anisotropy"
                Layout.columnSpan: 6
                font.pointSize: 20
            }

            TextField {
                id: parlBgTxt
                placeholderText: "Parallel Background"
                Layout.columnSpan: 3
                text: parlBgFile

            }
            Button {
                id:parlBgButton
                text: "Select"
                Layout.columnSpan: 3
                onClicked: {
                    parlBgFd.open();
                }
            }

            TextField {
                id: perpBgTxt
                placeholderText: "Perpendicular Background "
                Layout.columnSpan: 3
                text: perpBgFile
            }
            Button {
                id:perpBgButton
                text: "Select"
                Layout.columnSpan: 3
                onClicked: {
                    perpBgFd.open();
                }
            }

            TextField {
                id: parlTxt
                placeholderText: "Parallel"
                Layout.columnSpan: 3
                text: parlFile
            }
            Button {
                id:parlButton
                text: "Select"
                Layout.columnSpan: 3
                onClicked: {
                    parlFd.open();
                }
            }

            TextField {
                id: perpTxt
                placeholderText: "Perpendicular "
                Layout.columnSpan: 3
                text: perpFile
            }
            Button {
                id:perpButton
                text: "Select"
                Layout.columnSpan: 3
                onClicked: {
                    perpFd.open();
                }
            }

            TextField {
                id: subtractTxt
                placeholderText: "Subtract Value"
                Layout.columnSpan: 6
                text: subtractValue
                validator: DoubleValidator{bottom: 0; top: 4294967295;}
            }


            Button {
                id:anApply
                text: "Apply"
                Layout.columnSpan: 6
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: parent.width / 2
                onClicked: {
                    Ctrl.applyAnisotropy( parlBgFile, perpBgFile, parlFile, perpFile, subtractValue);
                }

            }

            Rectangle {
                Layout.fillWidth: true;
                Layout.columnSpan: 6
                height: 2
                color: "black"
            }


            Button {
                id:saveBtn
                text: "Save"
                Layout.columnSpan: 6
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: parent.width / 2
                onClicked: {
                    saveFd.open();
                }

            }
        }
    }


    // dialogs
    /*
    var parlBgFile;
    var perpBgFile;
    var parlFile;
    var perpFile;
    */
    FileDialog {
        id: parlBgFd
        selectExisting: true
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            parlBgFile = parlBgFd.fileUrl;
        }
        onRejected: {
            parlBgFile = null;
        }
    }

    FileDialog {
        id: perpBgFd
        selectExisting: true
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            perpBgFile = perpBgFd.fileUrl;
        }
        onRejected: {
            perpBgFile = null;
        }
    }

    FileDialog {
        id: parlFd
        selectExisting: true
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            parlFile = parlFd.fileUrl;
        }
        onRejected: {
            parlFile = null;
        }
    }

    FileDialog {
        id: perpFd
        selectExisting: true
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            perpFile = perpFd.fileUrl;
        }
        onRejected: {
            perpFile = null;
        }
    }

    FileDialog {
        id: saveFd
        selectExisting: false
        selectMultiple: false
        selectFolder: false
        onAccepted: {
            Manager.saveImage(saveFd.fileUrl);
        }

    }

    Anisotropy {
        id: ani
    }

}
