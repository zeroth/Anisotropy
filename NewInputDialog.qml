import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Dialog {
    title: "New Anisotropy"

    property url parlBgFile : ""
    property url perpBgFile : ""
    property url parlFile : ""
    property url perpFile : ""
    property double subtractValue: 0

    standardButtons: Dialog.Ok | Dialog.Cancel

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
            validator: DoubleValidator{bottom: 0; top: Math.pow(2, 32) -1;}
        }

    }

    // supporting FileDialog

    // dialogs
    /*
    url parlBgFile;
    url perpBgFile;
    url parlFile;
    url perpFile;
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
            parlBgFile = "";
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
            perpBgFile = "";
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
            parlFile = "";
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
            perpFile = "";
        }
    }

}
