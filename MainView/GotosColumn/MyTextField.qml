import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../"

Item {
    id: root
    width: 300
    height: 40

    property alias text: control.text
    property alias title: title.text
    property bool controlActiveFocus: control.activeFocus || control.usingContextMenu

    property alias validator: control.validator
    property alias acceptableInput: control.acceptableInput

    TextField {
        id: control
        anchors.centerIn: parent
        topPadding: 16
        padding: 15

        selectByMouse: true
        persistentSelection: true
        property bool usingContextMenu: false

        background: Rectangle {
            id: back
            implicitWidth: root.width
            implicitHeight: 40
            color: "transparent"
            border.width: 2
            border.color: control.activeFocus || control.usingContextMenu ? ui.color("blue-a400") : "grey"
            radius: 8
        }
        onActiveFocusChanged: {
            if (activeFocus || text.length > 0 || usingContextMenu) {
                parent.setState("ACTIVE_FOUCUS")
            } else {
                parent.setState("")
            }
            if(!activeFocus && !usingContextMenu){
                control.deselect();
            }
        }
        onTextChanged: {
            if (activeFocus || text.length > 0 || usingContextMenu) {
                parent.setState("ACTIVE_FOUCUS")
            } else {
                parent.setState("")
            }
        }

        onUsingContextMenuChanged: {
            if (activeFocus || text.length > 0 || usingContextMenu) {
                parent.setState("ACTIVE_FOUCUS")
            } else {
                parent.setState("")
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            hoverEnabled: true
            cursorShape: "IBeamCursor"
            onClicked: {
                var selectStart = control.selectionStart
                var selectEnd = control.selectionEnd
                var curPos = control.cursorPosition
                contextMenu.x = mouse.x
                contextMenu.y = mouse.y
                control.usingContextMenu = true
                contextMenu.open()
                control.cursorPosition = curPos
                control.select(selectStart, selectEnd)
            }

            Menu {
                id: contextMenu

                onClosed: {
                    control.usingContextMenu = false
                }

                MenuItem {
                    text: "Cut"
                    icon {
                        color: ui.color("grey-700")
                        source: "qrc:/icons/content_cut-black-48dp.svg"
                    }

                    font.pointSize: 10
                    onTriggered: {
                        control.cut()
                        control.focus = true
                    }
                }
                MenuItem {
                    text: "Copy"
                    icon {
                        color: ui.color("grey-700")
                        source: "qrc:/icons/content_copy-black-48dp.svg"
                    }
                    font.pointSize: 10
                    onTriggered: {
                        control.copy()
                        control.focus = true

                    }
                }
                MenuItem {
                    text: "Paste"
                    icon {
                        color: ui.color("grey-700")
                        source: "qrc:/icons/content_paste-black-48dp.svg"
                    }
                    font.pointSize: 10
                    onTriggered: {
                        control.paste()
                        control.focus = true
                    }
                }
            }
        }
    }

    Rectangle {
        id: titleTopBackground
        color: "transparent"
        x: control.x + control.padding - 2
        y: control.y - control.topPadding * 0.5
        width: title.contentWidth + 4
        height: title.contentHeight + 2
    }

    Text {
        id: title
        x: control.x + control.padding
        y: control.y + control.topPadding

        verticalAlignment: Text.AlignVCenter
        color: "grey"
        font.pointSize: 12
        Behavior on y {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutQuart
            }
        }
        Behavior on x {
            NumberAnimation {
                duration: 70
                easing.type: Easing.OutQuart
            }
        }
        Behavior on font.pointSize {
            NumberAnimation {
                duration: 70
                easing.type: Easing.OutQuart
            }
        }
    }

    function setState(s){
        if(s !== root.state){
            root.state = s;
        }
    }

    states: [
        State {
            name: "ACTIVE_FOUCUS"
            PropertyChanges {
                target: title
                y: control.y - control.topPadding * 0.5
                color: back.border.color
                font.pointSize: 10
                font.family: "Roboto medium"
            }
            PropertyChanges {
                target: titleTopBackground
                color: "white"
            }
        }
    ]
}
