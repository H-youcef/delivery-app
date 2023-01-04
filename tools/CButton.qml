import QtQuick 2.13
import QtQuick.Controls 2.13

import "../"

Button {
    id: control
    flat: true

    font.family: "Roboto"
    font.capitalization: Font.MixedCase
    font.bold: false
    font.pointSize: Theme.smalTextlSize

    property color backgroundColor: ui.color("grey-400")
    property color textColor: ui.color("white")

    property color iconColor: ui.color("white")
    property url iconSource
    property real iconWidth: Theme.baseIconSize
    property real iconHeight: Theme.baseIconSize

    contentItem: Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.baseMarginSize
        Icon{
            width: control.iconWidth
            height: control.iconHeight
            anchors.verticalCenter: parent.verticalCenter
            color: control.down ? Qt.darker(control.iconColor, 1.2) : control.iconColor
            source: control.iconSource
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            font: control.font
            opacity: enabled ? 1.0 : 0.3
            color: control.down ? Qt.darker(control.textColor, 1.2) : control.textColor
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40

        color: getColor()
        opacity: enabled ? 1 : 0.3
        radius: 2

        function getColor(){
            if(control.flat){
                if(control.hovered)
                    return Qt.rgba(0, 0, 0, 0.1)
                else
                    return "transparent"
            }else{
                if(control.hovered)
                    return Qt.darker(control.backgroundColor, 1.2)
                else
                    return control.backgroundColor
            }
        }
    }
}
