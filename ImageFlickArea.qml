import QtQuick 2.0
import QtQuick.Controls 1.4


    Rectangle {
        property real currentZoomLevel: 1.0

        width: 600
        height: 800
        clip: true

        // Create a flickable to view a large image.
        Flickable {
            id: view
            anchors.fill: parent
            contentWidth: image.width
            contentHeight: image.height

            Image {
                id: image
                source: "file:///C:/Users/abhis/OneDrive/Pictures/me.png"
                asynchronous: true
                scale: currentZoomLevel
            }

            // Only show the scrollbars when the view is moving.
            states: State {
                name: "ShowBars"
                when: view.movingVertically || view.movingHorizontally
                PropertyChanges { target: verticalScrollBar; opacity: 1 }
                PropertyChanges { target: horizontalScrollBar; opacity: 1 }
            }

            transitions: Transition {
                NumberAnimation { properties: "opacity"; duration: 400 }
            }
        }

        Rectangle {
            id:footer
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            height: 60
            /*ControlSlider {
            anchors.centerIn: parent
            id: zoomSlider
            title: "Zoom"
            from:0.7
            to:15.0
            value: 1.0
        }*/
            Row{
                x: parent.width/2 - childrenRect.width/2
                y: 10
                spacing: 10
                ToolButton {
                    iconSource: "qrc:/icons/zoom_in.png"
                    onClicked: {
                        currentZoomLevel = Math.min(currentZoomLevel+0.5, 20)
                    }
                }
                ToolButton {
                    iconSource: "qrc:/icons/zoom_out.png"
                    onClicked: {
                        currentZoomLevel -= 0.5
                    }
                }
                ToolButton {
                    iconSource: "qrc:/icons/zoom_reset.png"
                    onClicked: {
                        currentZoomLevel = 1.0
                    }
                }

                //            ToolButton {
                //                iconSource: "qrc:/icons/reset.png"
                //                onClicked: {
                //                    imageR.updateImageLocation()
                //                    imageT.updateImageLocation()
                //                }
                //            }
            }

        }

        // Attach scrollbars to the right and bottom edges of the view.
        ScrollBar {
            id: verticalScrollBar
            width: 12; height: view.height-12
            anchors.right: view.right
            opacity: 0
            orientation: Qt.Vertical
            position: view.visibleArea.yPosition
            pageSize: view.visibleArea.heightRatio
        }

        ScrollBar {
            id: horizontalScrollBar
            width: view.width-12; height: 12
            anchors.bottom: view.bottom
            opacity: 0
            orientation: Qt.Horizontal
            position: view.visibleArea.xPosition
            pageSize: view.visibleArea.widthRatio
        }
    }

