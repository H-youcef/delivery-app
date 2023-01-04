import QtQuick 2.13
import QtLocation 5.13
import QtPositioning 5.13
import "../tools/"
import "../"

MapQuickItem {
  id: root
  property alias iconSource: icon.source
  property alias iconColor: icon.color
  property bool enableMovementRotation: false
  property real iconScale: 1.0
  property alias font: text.font

  property var lastCoordinate: null
  onEnableMovementRotationChanged: {
    if (enableMovementRotation === false)
      rotation = 0
  }

  onCoordinateChanged: {
    if (enableMovementRotation === false)
      return
    if (lastCoordinate !== null) {
      const oldPos = Qt.vector2d(lastCoordinate.longitude,
                                 lastCoordinate.latitude)
      const newPos = Qt.vector2d(coordinate.longitude, coordinate.latitude)

      const newOrientationVec = newPos.minus(oldPos).normalized()

      const oldOrientationVec = Qt.vector2d(0, 1)
      const radAngle = Math.atan2(oldOrientationVec.y,
                                  oldOrientationVec.x) - Math.atan2(
                       newOrientationVec.y, newOrientationVec.x)

      const degAngle = radAngle * (180 / Math.PI)
      icon.rotation = degAngle
    }
    lastCoordinate = QtPositioning.coordinate(coordinate.latitude,
                                              coordinate.longitude)
  }

  anchorPoint: Qt.point(
                 markerItem.width / 2.0,
                 (descRect.height + markerItem.spacing) + (icon.height / 2.0))
  sourceItem: Column {
    id: markerItem
    //    spacing: Theme.dp(5)
    padding: 0
    Rectangle {
      id: descRect
      anchors.horizontalCenter: parent.horizontalCenter
      property real padding: Theme.dp(40)
      width: children[0].width + padding
      height: children[0].height + padding
      radius: Theme.dp(20)
      opacity: 0.8
      Text {
        id: text
        anchors.centerIn: parent
        text: username
        font.family: "Roboto"
        font.pointSize: 8
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
      }
    }
    Icon {
      id: icon
      anchors.horizontalCenter: parent.horizontalCenter
      width: Theme.dp(100) * iconScale
      height: Theme.dp(100) * iconScale
      source: "qrc:/icons/ic_place_48px.svg"
    }
  }
}
