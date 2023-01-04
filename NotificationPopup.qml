import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
  id: popup

  //  Component.onCompleted: open()
  width: cantainerRect.width
  height: cantainerRect.height
  modal: false
  focus: false
  closePolicy: Popup.NoAutoClose

  property bool canOpen: true
  property string severity_p: ""
  property string title_p: ""
  property string message_p: ""

  onCanOpenChanged: {
    if (!canOpen)
      popup.close()
  }

  Connections {
    target: uiNotifier
    function onNotificationInserted(severity, title, message, duration) {
      if (!canOpen)
        return
      popup.severity_p = severity
      popup.title_p = title
      popup.message_p = message
      timer.interval = duration
      timer.restart()
      popup.open()
      indicatorRect.show()
    }
  }

  onClosed: {
    timer.stop()
  }

  Timer {
    id: timer
    repeat: false
    onTriggered: popup.close()
  }

  Pane {
    id: cantainerRect
    anchors.centerIn: parent
    Column {
      width: Theme.dp(800)
      spacing: Theme.tinyMarginSize
      Row {
        spacing: Theme.smallMarginSize
        Icon {
          anchors.verticalCenter: parent.verticalCenter
          property string pseverity: popup.severity_p
          source: getSource()
          color: getColor()
          width: Theme.dp(65)
          height: Theme.dp(65)
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

        Text {
          anchors.verticalCenter: parent.verticalCenter
          font.pointSize: Theme.smallTextSize
          text: popup.title_p
        }
      }
      Text {
        font.pointSize: Theme.smallTextSize
        width: parent.width
        color: ui.color("grey-700")
        text: popup.message_p
        font.family: "Roboto"
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
      }
    }
  }
  Button {
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.rightMargin: -Theme.smallMarginSize
    anchors.topMargin: -Theme.baseMarginSize
    width: Theme.dp(100)
    height: width
    flat: true
    icon.source: "qrc:/icons/clear-black-48dp.svg"
    onClicked: popup.close()
  }
  Rectangle {
    id: indicatorRect
    anchors.fill: parent
    color: ui.color("grey-400")
    opacity: 0.0
    visible: false
    function show() {
      visible = true
      indicatorAnim.start()
    }

    SequentialAnimation {
      id: indicatorAnim
      loops: 3
      onFinished: indicatorRect.visible = false
      NumberAnimation {
        target: indicatorRect
        properties: "opacity"
        duration: 100
        from: 0.0
        to: 0.5
      }
      NumberAnimation {
        target: indicatorRect
        duration: 100
        properties: "opacity"
        from: 0.5
        to: 0.0
      }
    }
  }
}
