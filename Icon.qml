import QtQuick 2.13
import QtGraphicalEffects 1.13

Item {
    id: root

    property alias source: image.source
    property alias color: colorOverlay.color

    Image {
        id: image
        sourceSize{
            width: root.width
            height: root.height
        }
        anchors.centerIn: parent
        visible: false
    }
    ColorOverlay{
        id: colorOverlay
        source: image
        anchors.fill: image
    }
}
