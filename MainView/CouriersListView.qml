import QtQuick 2.13
import QtQuick.Layouts 1.13

import "../tools/"
import "../"

ListView {
  id: root

  property string selectedCourierId: ""

  highlightMoveDuration: 200
  highlightMoveVelocity: -1
  clip: true
  spacing: Theme.tinyMarginSize

  currentIndex: -1

  onCurrentIndexChanged: {
    if (currentIndex === -1)
      selectedCourierId = ""
  }

  delegate: Item {
    width: root.width - Theme.tinyMarginSize
    height: Theme.dp(160)
    Rectangle {
      id: hoverIndicator
      anchors.fill: parent
      visible: false
      radius: Theme.dp(40)
      color: ui.color("orange-50")
    }

    Row {
      anchors.fill: parent
      anchors.leftMargin: Theme.baseMarginSize
      anchors.rightMargin: Theme.baseMarginSize

      //                        anchors.leftMargin: Theme.largeMarginSize
      spacing: Theme.baseMarginSize
      Rectangle {
        //The colored circle
        width: Theme.dp(100)
        height: width
        radius: width / 2
        anchors.verticalCenter: parent.verticalCenter

        color: ui.colorFromHash(model._id)
        Rectangle {
          width: Theme.dp(35)
          height: width
          radius: width / 2
          anchors.top: parent.top
          anchors.right: parent.right
          color: "white"
          Rectangle {
            //Connection status indicator
            width: Theme.dp(20)
            height: width
            radius: width / 2
            anchors.centerIn: parent
            color: status === "online" ? ui.color("green-a400") : ui.color(
                                           "red-a400")
          }
        }
        Text {
          //Text inside the circle
          text: username[0].toUpperCase()
          verticalAlignment: Text.AlignVCenter
          horizontalAlignment: Text.AlignHCenter
          anchors.centerIn: parent
          color: "white"
        }
        function getColor(str) {
          var cols = [ui.color("green-a400"), ui.color("blue-a400"), ui.color(
                        "red-a400"), ui.color("yellow-a400"), ui.color(
                        "purple-a400"), ui.color("pink-a400"), ui.color(
                        "indigo-a400")]

          const h = Math.abs(ui.hash(str))
          const r = h % cols.length
          return cols[r]
        }
      }
      Text {
        //Username text
        color: root.currentIndex == index ? ui.color(
                                              "red-400") : ui.color("black")
        font.pointSize: Theme.tinyTextSize
        font.family: "roboto-medium"
        text: ui.toTitleCase(username)
        anchors.verticalCenter: parent.verticalCenter
      }
    }
    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      onClicked: {
        root.currentIndex = index
        root.selectedCourierId = model._id
        couriersMonitor.filterGotoModelByCourierId(model._id)
      }
      onEntered: hoverIndicator.visible = true
      onExited: hoverIndicator.visible = false
    }
  }
  highlight: Rectangle {
    color: ui.color("pink-50")
    radius: Theme.dp(40)
  }
}
