import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.15

import "../tools/"
import "../"

Rectangle {
  id: root
  property string selectedCourierId: ""
  ColumnLayout {
    anchors {
      top: parent.top
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }
    spacing: Theme.largeMarginSize
    Row {
      leftPadding: Theme.largeMarginSize
      spacing: Theme.smallMarginSize
      Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.dp(95)
        height: width
        radius: width / 2
        color: ui.color("blue-400")
        Icon {
          anchors.centerIn: parent
          width: Theme.dp(55)
          color: "white"
          source: "qrc:/icons/moped-black-48dp.svg"
        }
      }

      Text {
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: Theme.largeTextSize
        text: qsTr("Couriers")
      }
    }
    ColumnLayout {

      Layout.fillWidth: true
      spacing: Theme.baseMarginSize
      ListView {
        //Top menu tab
        id: pageIndicatorListView
        width: Layout.maximumWidth
        height: Theme.dp(50)
        Layout.leftMargin: Theme.baseMarginSize
        spacing: Theme.tinyMarginSize
        orientation: ListView.Horizontal
        onCurrentIndexChanged: swipeView.currentIndex = currentIndex
        model: [qsTr("Online"), qsTr("All")]
        delegate: Text {
          width: contentWidth + Theme.largeMarginSize
          height: contentHeight + Theme.tinyMarginSize
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          color: ListView.isCurrentItem ? ui.color(
                                            "pink-a400") : ui.color("black")
          font.pointSize: Theme.smallTextSize
          text: modelData
          MouseArea {
            anchors.fill: parent
            onClicked: pageIndicatorListView.currentIndex = index
          }
        }
        highlight: Rectangle {
          color: ui.color("pink-50")
          radius: Theme.dp(40)
        }
      }
      SwipeView {
        id: swipeView
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.leftMargin: Theme.baseMarginSize

        clip: true

        onCurrentIndexChanged: {
          onlineCouriersListView.currentIndex = -1
          allCouriersListView.currentIndex = -1
          pageIndicatorListView.currentIndex = currentIndex
        }

        CouriersListView {
          id: onlineCouriersListView
          model: onlineCouriersModel
          onSelectedCourierIdChanged: root.selectedCourierId = selectedCourierId
        }

        CouriersListView {
          id: allCouriersListView
          model: allCouriersModel
          onSelectedCourierIdChanged: root.selectedCourierId = selectedCourierId
        }
      }
    }
  }

  onSelectedCourierIdChanged: {
    console.log("selected cid: ", selectedCourierId)
  }

  ListModel {
    id: allCouriersLModel
    ListElement {
      username: "Bill Smith"
      courier_id: "5553264"
      status: "online"
    }
    ListElement {
      username: "John Brown"
      courier_id: "5558426"
      status: "online"
    }
    ListElement {
      username: "Sam Wise"
      courier_id: "555 0473"
      status: "offline"
    }
    ListElement {
      username: "Bill Smith"
      courier_id: "5553287"
      status: "online"
    }
    ListElement {
      username: "John Brown"
      courier_id: "5558454"
      status: "offline"
    }
    ListElement {
      username: "Sam Wise"
      courier_id: "555 0411"
      status: "offline"
    }
  }
  ListModel {
    id: onlineCouriersLModel
    ListElement {
      username: "Bill Smith"
      courier_id: "5553264"
      status: "online"
      latitude: "35.188281056868490"
      longitude: "-0.633652915929360"
    }
    ListElement {
      username: "John Brown"
      courier_id: "5558426"
      status: "online"
      latitude: "35.193262603743293"
      longitude: "-0.634428648285905"
    }
    ListElement {
      username: "Bill Smith"
      courier_id: "5553287"
      status: "online"
      latitude: "35.189341961181199"
      longitude: "-0.629423302910482"
    }
  }
}
