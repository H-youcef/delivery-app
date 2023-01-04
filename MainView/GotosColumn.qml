import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15

import "../tools/"
import "../"
import "GotosColumn/"
import "GotosColumn/GotoCard/"

Rectangle {
  id: root

  signal gotoSelected(string courierId, string gotoId)

  property string selectedCourierId: ""
  property string selectedGotoId: ""

  onSelectedCourierIdChanged: {
    if (!inCardsView)
      changeView()
  }

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
          source: "qrc:/icons/shop-black-48dp.svg"
        }
      }

      Text {
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: Theme.largeTextSize
        text: qsTr("Tasks")
      }
    }

    ColumnLayout {
      //Layout bellow big title
      Layout.fillWidth: true
      spacing: Theme.largeMarginSize
      Item {
        //Top pink bar with the + Button
        id: addBar
        enabled: selectedCourierId.length > 0
        Layout.fillWidth: true
        height: Theme.dp(50)
        Rectangle {
          //Pink bar
          anchors.centerIn: parent
          width: parent.width
          height: Theme.dp(10)

          gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop {
              position: 0.0
              color: selectedCourierId.length === 0 ? ui.color(
                                                        "grey-200") : ui.color(
                                                        "pink-200")
            }
            GradientStop {
              position: 0.5
              color: selectedCourierId.length === 0 ? ui.color(
                                                        "grey-300") : ui.color(
                                                        "pink-300")
            }
            GradientStop {
              position: 1.0
              color: selectedCourierId.length === 0 ? ui.color(
                                                        "grey-200") : ui.color(
                                                        "pink-200")
            }
          }
          Rectangle {
            id: addButton
            //The add button
            anchors.centerIn: parent
            width: Theme.dp(100)
            height: width
            radius: width / 2
            color: selectedCourierId.length === 0 ? ui.color(
                                                      "grey-400") : ui.color(
                                                      "pink-300")

            Behavior on width {
              NumberAnimation {
                duration: 20
              }
            }

            Icon {
              id: addButtonIcon
              anchors.centerIn: parent
              source: "qrc:/icons/add-black-48dp.svg"
              width: 0.65 * parent.width
              height: width
              color: "white"
            }
            MouseArea {
              anchors.fill: parent
              cursorShape: "PointingHandCursor"
              hoverEnabled: true
              onEntered: {
                addButton.width = Theme.dp(110)
                addButtonShadow.verticalOffset = 3
              }
              onExited: {
                addButton.width = Theme.dp(100)
                addButtonShadow.verticalOffset = 2
              }
              onPressed: {
                addButton.width = Theme.dp(100)
                addButtonShadow.verticalOffset = 2
              }
              onReleased: {
                addButton.width = Theme.dp(110)
                addButtonShadow.verticalOffset = 3
              }

              onClicked: {
                root.changeView()
              }
            }
          }
          DropShadow {
            id: addButtonShadow
            source: addButton
            anchors.fill: addButton
            verticalOffset: 2
            color: ui.color("grey-700")
            Behavior on verticalOffset {
              NumberAnimation {
                duration: 20
              }
            }
          }
        }
      }

      StackView {
        //Bellow the add button stack view
        id: stackView
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.leftMargin: Theme.baseMarginSize

        clip: true
        initialItem: ListView {
          //Cards list view
          id: orderslistView
          highlightMoveDuration: 200
          highlightMoveVelocity: -1
          clip: true
          spacing: Theme.smallMarginSize
          model: filteredGotosModel
          delegate: GotoCard {
            anchors.horizontalCenter: parent.horizontalCenter
            isSelected: orderslistView.currentIndex === index
            goto_id: model._id
            courier_id: model.courier_id
            phone: model.phone
            from: model.from
            status: model.status
            start_time: model.start_time
            address: model.address
            description: model.description
            latitude: model.latitude
            longitude: model.longitude
            accuracy: model.accuracy
            sync: model.sync
            //                        address: "fjdmjfmsdjmsdj\n" +
            //                                 "dqjdsklqsdlkhqsdlhj\n" +
            //                                 "dqsdnlkqsn qdklhj"
            //                        description: "fjdmjfmsdjmsdj\n" +
            //                                     "dqjdsklqsdlkhqsdlhj\n" +
            //                                     "dqsdnlkqsn qdklhj"
            onClicked: {
              orderslistView.currentIndex = index
              root.selectedGotoId = model._id
              root.gotoSelected(courier_id, goto_id)
            }

            onEditRequested: {
              if (inCardsView) {
                inCardsView = false
                addButtonIcon.source = "qrc:/icons/clear-black-48dp.svg"
                stackView.push(addForm, {
                                 "isAnUpdate": true,
                                 "goto_id": goto_id,
                                 "courier_id": courier_id,
                                 "status": status,
                                 "latitude": latitude,
                                 "longitude": longitude,
                                 "accuracy": accuracy,
                                 "phone": phone,
                                 "start_time": start_time,
                                 "from": from,
                                 "address": address,
                                 "description": description
                               })
              }
            }
          }
        }
      }
    }
  }

  Component {
    id: addForm
    CreateGotoForm {
      onCanceled: root.changeView()
      onAccepted: root.changeView()
      courier_id: root.selectedCourierId
    }
  }

  property bool inCardsView: true
  function changeView() {
    if (inCardsView) {
      inCardsView = false
      stackView.push(addForm)
      addButtonIcon.source = "qrc:/icons/clear-black-48dp.svg"
    } else {
      inCardsView = true
      stackView.pop()
      addButtonIcon.source = "qrc:/icons/add-black-48dp.svg"
    }
  }
}
