import QtQuick.Controls 2.15
import QtQuick 2.15

import "../../tools/"
import "../../"

Button {
    id: control

    property color textColor: "black"
    property color textColorDown: "black"

    property color iconColor: "black"
    property color iconColorDown: "black"

    property bool showIcon: true

    flat: true

    contentItem: Row {
        spacing: Theme.tinyMarginSize
        Icon {
            anchors.verticalCenter: parent.verticalCenter
            width: control.icon.width
            height: control.icon.height
            source: control.icon.source
            color: control.down ? control.iconColor : control.iconColorDown
            visible: control.showIcon
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            font: control.font
            opacity: enabled ? 1.0 : 0.3
            color: control.down ? control.textColorDown : control.textColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

}
