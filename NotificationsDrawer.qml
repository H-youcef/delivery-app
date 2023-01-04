import QtQuick 2.15
import QtQuick.Controls 2.15

Drawer {
  id: root
  //  Component.onCompleted: open()
  onOpened: notificationsListView.focus = true
  ListView {
    id: notificationsListView
    anchors.fill: parent
    anchors.margins: Theme.baseMarginSize
    clip: true
    spacing: Theme.smallMarginSize
    model: uiNotifier

    delegate: Rectangle {
      id: cantainerRect
      property real padding: Theme.dp(45)
      height: children[0].height + padding
      width: notificationsListView.width
      color: Qt.lighter("lightgrey", 1.15)
      Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: Theme.smallMarginSize
        spacing: Theme.tinyMarginSize
        Row {
          spacing: Theme.smallMarginSize
          Icon {
            //sevirity icon
            anchors.verticalCenter: parent.verticalCenter
            property string pseverity: model.severity
            source: getSource()
            color: getColor()
            width: Theme.dp(75)
            height: Theme.dp(75)
            function getSource() {
              if (pseverity === "info")
                return "qrc:/icons/info-black-48dp.svg"
              if (pseverity === "warning")
                return "qrc:/icons/warning-black-48dp.svg"
              if (pseverity === "error")
                return "qrc:/icons/error-black-48dp.svg"
              return "qrc:/icons/help-black-48dp.svg"
            }
            function getColor() {
              if (pseverity === "info")
                return ui.color("grey-600")
              if (pseverity === "warning")
                return ui.color("yellow-700")
              if (pseverity === "error")
                return ui.color("red-700")
              return ui.color("grey-700")
            }
          }
          Column {
            anchors.verticalCenter: parent.verticalCenter
            Text {
              font.pointSize: Theme.tinyTextSize
              color: ui.color("grey-700")
              text: model.time
            }
            Text {
              font.pointSize: Theme.smallTextSize
              text: model.title
            }
          }
        }
        Text {
          font.pointSize: Theme.smallTextSize
          width: cantainerRect.width
          color: ui.color("grey-700")
          text: model.message
          font.family: "Roboto"
          wrapMode: Text.WordWrap
          elide: Text.ElideRight
        }
      }
    }
  }

  function getCurrentTime() {
    function checkTime(i) {
      if (i < 10) {
        i = "0" + i
      }
      return i
    }
    const today = new Date()
    var h = today.getHours()
    var m = today.getMinutes()
    var s = today.getSeconds()
    // add a zero in front of numbers<10
    h = checkTime(h)
    m = checkTime(m)
    s = checkTime(s)
    return h + ':' + m + ':' + s
  }
}
