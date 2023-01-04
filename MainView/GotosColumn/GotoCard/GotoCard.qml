import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../tools/"
import "../../../"
import "../"

Pane {
  id: root

  signal editRequested
  signal clicked

  contentWidth: cardContent.width
  contentHeight: cardContent.height

  Material.elevation: 4

  Timer {
    id: pendingTimer
    interval: 60000 // 1 min
    repeat: true
    running: true
    triggeredOnStart: false
    onTriggered: elapsed += 1
  }

  property bool isSelected: false

  property string goto_id
  property string courier_id

  property string phone
  property string from
  property string status
  property double start_time
  property double elapsed: Math.floor(
                             ((Date.now(
                                 ) - start_time) / 1000) / 60) //in minutes

  property string address
  property string description

  property string latitude
  property string longitude
  property double accuracy

  property bool sync: false

  Item {
    id: cardContent
    width: Theme.dp(650)
    height: mainColumn.height

    Column {
      id: deletionConfirmation
      spacing: Theme.dp(50)
      anchors.centerIn: parent
      visible: false
      Text {
        text: qsTr("Confirm deletion ?")
        color: ui.color("grey-900")
      }

      Row {
        spacing: Theme.baseMarginSize
        MyButton {
          id: noButton
          text: qsTr("No")
          flat: true

          textColor: ui.color("green-500")
          textColorDown: ui.color("green-900")
          showIcon: false
          onClicked: {
            mainColumn.opacity = 1.0
            mainColumn.enabled = true
            deletionConfirmation.visible = false
          }
        }
        MyButton {
          id: yesButton
          text: qsTr("Yes")
          flat: true
          showIcon: false
          textColor: ui.color("grey-800")
          textColorDown: ui.color("green-900")
          onClicked: {
            couriersMonitor.deleteGoto(courier_id, goto_id)
          }
        }
      }
    }

    Column {
      //Contains all items in the card
      id: mainColumn
      opacity: 1.0
      spacing: Theme.baseMarginSize
      Item {
        width: headerLayout.width
        height: headerLayout.height
        anchors.horizontalCenter: parent.horizontalCenter
        RowLayout {
          //Header
          id: headerLayout
          width: cardContent.width
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.right: parent.right
          spacing: Theme.tinyMarginSize
          Rectangle {
            width: Theme.dp(85)
            height: width
            radius: width / 2
            Layout.alignment: Qt.AlignVCenter
            color: root.isSelected ? ui.color("blue-400") : ui.color("grey-700")
            Icon {
              anchors.centerIn: parent
              width: Theme.dp(45)
              color: "white"
              source: "qrc:/icons/near_me-black-48dp.svg"
            }
          }
          Column {
            Layout.alignment: Qt.AlignVCenter
            Text {
              font.pointSize: 12
              text: beautifyPhone(root.phone)

              function beautifyPhone(str) {
                if (str.length !== 10)
                  return str
                return str[0] + str[1] + str[3] + " " + str[4] + str[5] + " "
                    + str[6] + str[7] + " " + str[8] + str[9]
              }
            }
            Text {
              font.pointSize: 10
              text: root.from
              font.family: "Roboto"
              color: ui.color("grey-500")
            }
          }
          Item {
            Layout.fillWidth: true
            height: 1
          }
          Rectangle {
            //Seen-by-courier icon
            color: ui.color("blue-a400")
            width: Theme.dp(25)
            height: width
            radius: width / 2.0
            Layout.alignment: Qt.AlignVCenter
            visible: !seen_by_courier
          }
          Icon {
            //in-sync icon
            source: "qrc:/icons/sync-black-48dp.svg"
            color: ui.color("red-a400")
            width: Theme.dp(65)
            height: Theme.dp(65)
            Layout.alignment: Qt.AlignTop
            visible: sync
          }

          Icon {
            //has location icon
            source: "qrc:/icons/ic_place_48px.svg"
            color: ui.colorFromHash(courier_id)
            width: Theme.dp(65)
            height: Theme.dp(65)
            Layout.alignment: Qt.AlignTop
            visible: root.latitude.length !== 0 && root.longitude.length !== 0
          }

          Column {
            id: statusColumn
            Layout.alignment: Qt.AlignVCenter
            Icon {
              id: stateIcon
              anchors.horizontalCenter: parent.horizontalCenter
              width: Theme.dp(60)
              height: width
              source: parent.getIcon(root.status)
              color: parent.getColor(root.status)
            }
            Text {
              font.pointSize: 8
              text: root.elapsed.toString() + "min"
              font.family: "Roboto"
              horizontalAlignment: Text.AlignHCenter
              anchors.horizontalCenter: parent.horizontalCenter
              color: parent.getColor()
            }
            function getIcon(statusStr) {
              if (root.status === "pending")
                return "qrc:/icons/schedule-black-48dp.svg"
              if (root.status === "done")
                return "qrc:/icons/done-black-48dp.svg"
              if (root.status === "canceled")
                return "qrc:/icons/cancel_schedule_send-black-48dp.svg"
              return "qrc:/icons/help-black-48dp.svg"
            }
            function getColor(statusStr) {
              if (root.status === "pending")
                return ui.color("orange-a700")
              if (root.status === "done")
                return ui.color("green-a700")
              if (root.status === "cancled")
                return ui.color("red-a700")
              return ui.color("red-a700")
            }
          }
        }
        MouseArea {
          anchors.fill: parent
          cursorShape: "PointingHandCursor"
          acceptedButtons: Qt.LeftButton | Qt.RightButton
          onClicked: {
            root.clicked()
            if (mouse.button === Qt.LeftButton) {
              mainContentItem.expanded = !mainContentItem.expanded
            } else if (mouse.button == Qt.RightButton) {
              statusContextMenu.x = mouse.x
              statusContextMenu.y = mouse.y
              statusContextMenu.open()
            }
          }
        }
        MouseArea {
          x: statusColumn.x
          y: statusColumn.y
          width: statusColumn.width
          height: statusColumn.height
          cursorShape: "PointingHandCursor"
          acceptedButtons: Qt.LeftButton | Qt.RightButton
          onClicked: {
            root.clicked()
            if (mouse.button == Qt.RightButton) {
              statusContextMenu.x = mouse.x
              statusContextMenu.y = mouse.y
              statusContextMenu.open()
            }
          }
        }
        Menu {
          id: statusContextMenu
          MenuItem {
            text: "Resume"
            icon {
              color: ui.color("grey-700")
              source: "qrc:/icons/play_arrow-black-48dp.svg"
            }

            font.pointSize: 10
            onTriggered: {
              if (root.status !== "pending") {
                couriersMonitor.updateGotoStatus(courier_id, goto_id, "pending")
                pendingTimer.start()
              }
            }
          }
          MenuItem {
            text: "Cancel"
            icon {
              color: ui.color("grey-700")
              source: "qrc:/icons/cancel_schedule_send-black-48dp.svg"
            }

            font.pointSize: 10
            onTriggered: {
              if (root.status !== "canceled") {
                couriersMonitor.updateGotoStatus(courier_id, goto_id,
                                                 "canceled")
                pendingTimer.stop()
              }
            }
          }
          MenuItem {
            text: "Mark Done"
            icon {
              color: ui.color("grey-700")
              source: "qrc:/icons/done-black-48dp.svg"
            }

            font.pointSize: 10
            onTriggered: {
              if (root.status !== "done") {
                couriersMonitor.updateGotoStatus(courier_id, goto_id, "done")
                pendingTimer.stop()
              }
            }
          }
        }
      }
      Item {
        //Item that contains the column bellow the header
        id: mainContentItem
        property bool expanded: false
        width: parent.width
        height: expanded ? bellowHeaderColumn.height : 0
        clip: true

        Behavior on height {
          NumberAnimation {
            easing.type: Easing.OutQuad
            duration: 250
          }
        }

        Column {
          id: bellowHeaderColumn
          anchors.top: parent.top
          anchors.right: parent.right
          anchors.left: parent.left
          spacing: Theme.baseMarginSize
          width: parent.width

          Rectangle {
            //Top separation grey line
            width: parent.width
            height: 2
            color: ui.color("grey-400")
          }

          Rectangle {

            id: textRect
            width: parent.width
            height: textColumn.height
            color: "transparent"
            //                    border.width: 1
            Column {
              //Address and Description text
              id: textColumn
              spacing: Theme.smallMarginSize
              anchors {
                left: parent.left
                right: parent.right
                top: parent.top
              }
              Column {
                id: addrColumn
                width: parent.width
                spacing: Theme.tinyMarginSize
                Row {
                  spacing: Theme.tinyMarginSize
                  Icon {
                    anchors.verticalCenter: parent.verticalCenter
                    width: Theme.dp(50)
                    height: width
                    color: ui.color("grey-900")
                    source: "qrc:/icons/ic_place_48px.svg"
                  }
                  Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Address")
                    font.pointSize: 10
                  }
                }

                Text {
                  id: addrText
                  width: parent.width
                  height: (contentHeight / lineCount)
                  leftPadding: Theme.baseMarginSize
                  wrapMode: Text.WordWrap
                  lineHeight: 1.1
                  clip: true
                  text: root.address
                  font.pointSize: 10
                  color: ui.color("grey-700")
                  Behavior on height {
                    NumberAnimation {
                      duration: 200
                    }
                  }
                }
              }

              Column {

                id: descColumn
                width: parent.width
                spacing: Theme.tinyMarginSize
                Row {
                  spacing: Theme.tinyMarginSize
                  Icon {
                    anchors.verticalCenter: parent.verticalCenter
                    width: Theme.dp(50)
                    height: width
                    color: ui.color("grey-900")
                    source: "qrc:/icons/history_edu-black-48dp.svg"
                  }
                  Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Description")
                    font.pointSize: 10
                  }
                }

                Text {
                  id: descText
                  width: parent.width
                  height: lineCount === 1 ? (contentHeight
                                             / lineCount) : (contentHeight / lineCount) * 2
                  leftPadding: Theme.baseMarginSize
                  wrapMode: Text.WordWrap
                  lineHeight: 1.1
                  clip: true
                  text: root.description
                  font.pointSize: 10
                  color: ui.color("grey-700")
                  Behavior on height {
                    NumberAnimation {
                      duration: 200
                    }
                  }
                }
              }
            }
            property bool expanded: false

            MouseArea {
              anchors.fill: parent
              cursorShape: Qt.PointingHandCursor

              onClicked: {
                root.clicked()
                if (!parent.expanded) {
                  parent.expanded = true
                  descText.height = descText.contentHeight
                  addrText.height = addrText.contentHeight
                } else {
                  parent.expanded = false
                  descText.height = descText.lineCount
                      === 1 ? (descText.contentHeight
                               / descText.lineCount) : (descText.contentHeight
                                                        / descText.lineCount) * 2
                  addrText.height = (addrText.contentHeight / addrText.lineCount)
                }
              }
            }
          }

          Row {
            //Action buttons
            id: actionsRow
            spacing: Theme.tinyMarginSize

            MyButton {
              text: qsTr("Edit")
              font.pointSize: 10
              icon.source: "qrc:/icons/edit-black-48dp.svg"
              icon.width: Theme.dp(45)
              icon.height: Theme.dp(45)
              iconColor: ui.color("deep-purple-500")
              iconColorDown: ui.color("deep-purple-900")

              textColor: ui.color("deep-purple-500")
              textColorDown: ui.color("deep-purple-900")

              flat: true
              width: Theme.dp(250)
              onClicked: root.editRequested()
            }

            MyButton {
              text: qsTr("Delete")
              font.pointSize: 10
              icon.source: "qrc:/icons/delete_forever-black-48dp.svg"
              icon.width: Theme.dp(45)
              icon.height: Theme.dp(45)
              iconColor: ui.color("deep-purple-500")
              iconColorDown: ui.color("deep-purple-900")

              textColor: ui.color("deep-purple-500")
              textColorDown: ui.color("deep-purple-900")

              flat: true
              width: Theme.dp(250)

              onClicked: {
                mainColumn.opacity = 0.0
                mainColumn.enabled = false
                deletionConfirmation.visible = true
              }
            }
          }
        }
      }
    }
  }
}
