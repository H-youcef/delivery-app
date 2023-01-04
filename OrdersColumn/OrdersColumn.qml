import QtQuick 2.13
import QtQuick.Layouts 1.13

import "../tools/"
import "../"

Rectangle {
    id: ordersColumn

    ColumnLayout {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            topMargin: Theme.largeMarginSize
            bottomMargin: Theme.largeMarginSize

            rightMargin: Theme.largeMarginSize
        }
        spacing: Theme.largeMarginSize
        Text {
            leftPadding: Theme.largeMarginSize
            font.pointSize: Theme.largeTextSize
            text: qsTr("Orders")
        }

        ColumnLayout {

            Layout.fillWidth: true
            spacing: Theme.baseMarginSize
            ListView {
                //Top menu tab
                id: listView
                width: Layout.maximumWidth
                height: Theme.dp(50)
                Layout.leftMargin: Theme.largeMarginSize
                spacing: Theme.tinyMarginSize
                orientation: ListView.Horizontal
                model: [qsTr("Assigned"), qsTr("Unassigned"), qsTr("All")]
                delegate: Text {
                    width: contentWidth + Theme.largeMarginSize
                    height: contentHeight + Theme.tinyMarginSize
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: ListView.isCurrentItem ? ui.color(
                                                        "pink-a400") : ui.color(
                                                        "black")
                    font.pointSize: Theme.smallTextSize
                    text: modelData
                    MouseArea {
                        anchors.fill: parent
                        onClicked: listView.currentIndex = index
                    }
                }
                highlight: Rectangle {
                    color: ui.color("pink-50")
                    radius: Theme.dp(40)
                }
            }

            ListView {
                id: orderslistView
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.leftMargin: Theme.baseMarginSize + Theme.dp(10)
                highlightMoveDuration: 200
                highlightMoveVelocity: -1
                clip: true
                spacing: Theme.tinyMarginSize
                model: [1, 2, 3, 4, 5, 6]

                //                        model: []
                delegate: Item {
                    width: orderslistView.width - Theme.tinyMarginSize
                    height: Theme.dp(160)

                    Rectangle{
                        id: hoverIndicator
                        anchors.fill: parent
                        visible: false
                        radius: Theme.dp(40)
                        color: ui.color("orange-50")
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.baseMarginSize
                        anchors.rightMargin: Theme.baseMarginSize
//                        anchors.leftMargin: Theme.largeMarginSize

                        spacing: -Theme.dp(90)
                        Rectangle {
                            width: Theme.dp(130)
                            height: width
                            radius: Theme.dp(25)
                            color: getColor()
                            function getColor() {
                                var cols = [ui.color("green-a400"), ui.color(
                                                "blue-a400"), ui.color(
                                                "red-a400"), ui.color(
                                                "yellow-a400"), ui.color(
                                                "purple-a400"), ui.color(
                                                "pink-a400"), ui.color(
                                                "indigo-a400")]
                                var min = 0
                                var max = cols.length
                                var r = Math.floor(Math.random(
                                                       ) * (max - min) + min)
                                return cols[r]
                            }
                        }
                        ColumnLayout {
                            Text {
                                color: orderslistView.currentIndex == index ? ui.color("red-400") : ui.color(
                                                                                                    "black")
                                font.pointSize: Theme.smallTextSize
                                font.family: "roboto-medium"
                                text: qsTr("0697 77 55 60")
                            }
                            Text {
                                font.pointSize: Theme.tinyTextSize
                                text: qsTr("12-03-2020 12:14")
                            }
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                            Text {
                                color: orderslistView.currentIndex == index ? ui.color("red-400") : ui.color(
                                                                    "black")
                                Layout.alignment: Qt.AlignHCenter
                                font.pointSize: Theme.baseTextSize
                                font.bold: true
                                text: index + 1
                            }
                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                font.pointSize: Theme.tinyTextSize
                                text: qsTr("12:14")
                            }
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: orderslistView.currentIndex = index
                        onEntered: hoverIndicator.visible = true
                        onExited: hoverIndicator.visible = false
                    }
                }
                highlight: Rectangle {
                    color: ui.color("pink-50")
                    radius: Theme.dp(40)
                }
            }
        }
    }
}
