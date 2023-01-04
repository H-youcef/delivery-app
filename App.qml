import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import "tools/"
import "OrdersColumn/"
import "MainView/"

Item {
  id: root
  anchors.fill: parent

  onWidthChanged: Theme.windowWidth = width
  onHeightChanged: Theme.windowHeight = height
  Component.onCompleted: {
    Theme.windowWidth = width
    Theme.windowHeight = height
  }

  Rectangle {
    anchors.fill: parent
    color: ui.color("indigo-900")
    Column {
      anchors {
        top: parent.top
        left: parent.left
        topMargin: Theme.largeMarginSize
        leftMargin: Theme.largeMarginSize
      }
      spacing: 100
      Text {
        id: appTitle
        anchors.left: parent.left
        color: ui.color("white")
        font.pointSize: Theme.largeTextSize
        font.family: "Roboto"
        text: qsTr("Delivery App")
      }

      Column {
        CButton {
          width: Theme.dp(500)
          text: qsTr("Home")
          iconSource: "qrc:/icons/ic_home_48px.svg"
        }
        CButton {
          width: Theme.dp(500)
          text: qsTr("Search")
          iconSource: "qrc:/icons/ic_search_48px.svg"
        }
      }
    }
  }

  Rectangle {
    anchors {
      top: parent.top
      bottom: parent.bottom
      right: parent.right
      left: parent.left

      topMargin: Theme.baseMarginSize
      bottomMargin: Theme.baseMarginSize
      rightMargin: Theme.baseMarginSize
      //            leftMargin: Theme.dp(550)
      leftMargin: Theme.dp(50)
    }
    radius: Theme.dp(70)
    color: ui.color("pink-100")
    RowLayout {
      anchors.fill: parent
      MainView {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.topMargin: Theme.baseMarginSize
        Layout.bottomMargin: Theme.baseMarginSize
        Layout.leftMargin: Theme.smallMarginSize
        Layout.rightMargin: Theme.baseMarginSize

        radius: Theme.dp(60)
      }
    }
  }

  //TODO: finish notification system; add c++ class/object called notifier
  // that will be connected to this object.
  NotificationsDrawer {
    id: notificationsDrawer
    width: Theme.dp(900)
    height: root.height
    edge: Qt.RightEdge
  }

  NotificationPopup {
    x: parent.width - width - Theme.largeMarginSize
    y: parent.height - height - Theme.dp(220)
    canOpen: !notificationsDrawer.opened
  }
}
