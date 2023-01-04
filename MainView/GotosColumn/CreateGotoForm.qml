import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls.Material 2.15

import QtPositioning 5.13
import QtLocation 5.13

import "../../"

ScrollView {
  id: root

  signal canceled
  signal accepted

  property bool isAnUpdate: false
  property string courier_id

  //Used when updating
  property string goto_id
  property string status
  property string latitude: ""
  property string longitude: ""
  property real accuracy: 0
  property string phone
  property real start_time
  property string from
  property string address
  property string description

  Column {
    spacing: Theme.smallMarginSize
    Row {
      spacing: Theme.baseMarginSize
      Icon {
        width: Theme.dp(70)
        height: width
        color: tfStore.controlActiveFocus ? ui.color("blue-400") : ui.color(
                                              "grey-700")
        source: "qrc:/icons/store-24px.svg"
        anchors.verticalCenter: parent.verticalCenter
      }
      MyTextField {
        id: tfStore
        text: root.from
        width: Theme.dp(500)
        height: Theme.dp(150)
        title: "From Store"
      }
    }
    Row {
      spacing: Theme.baseMarginSize
      Icon {
        width: Theme.dp(70)
        height: width
        color: tfDesc.controlActiveFocus ? ui.color("blue-400") : ui.color(
                                             "grey-700")
        source: "qrc:/icons/history_edu-black-48dp.svg"
      }
      MyTextArea {
        id: tfDesc
        text: root.description
        width: Theme.dp(600)
        height: Theme.dp(300)
        title: "Description"
      }
    }

    Row {
      spacing: Theme.baseMarginSize
      Icon {
        width: Theme.dp(70)
        height: width
        color: tfPhone.controlActiveFocus ? ui.color("blue-400") : ui.color(
                                              "grey-700")
        source: "qrc:/icons/local_phone-24px.svg"
        anchors.verticalCenter: parent.verticalCenter
      }
      Column {
        spacing: -Theme.dp(10)
        MyTextField {
          id: tfPhone
          text: root.phone
          width: Theme.dp(500)
          height: Theme.dp(150)
          title: "Phone"
          validator: RegularExpressionValidator {
            regularExpression: /0[5,6,7]\d{8}$/
          }
          onTextChanged: {
            if (text.length === 10) {
              const clientLocation = clientsLocationsModel.getClientLocationByPhone(
                                     text)
              console.log(clientLocation.latitude, clientLocation.longitude,
                          clientLocation.accuracy)
              if (clientLocation.latitude && clientLocation.longitude) {

                root.setLocation(clientLocation.latitude,
                                 clientLocation.longitude,
                                 clientLocation.accuracy)

                if (clientLocation.address)
                  tfAddr.text = clientLocation.address
                else
                  tfAddr.text = ""
              }
            }
          }
        }
        Text {
          text: qsTr("*Required")
          font.family: "Roboto Medium"
          color: ui.color("red-400")
          font.pointSize: Theme.tinyTextSize
        }
      }
    }
    Row {
      spacing: Theme.smallMarginSize
      Icon {
        width: Theme.dp(70)
        height: width
        color: tfAddr.controlActiveFocus ? ui.color("blue-400") : ui.color(
                                             "grey-700")
        source: "qrc:/icons/location_on-black-48dp.svg"
        anchors.verticalCenter: parent.verticalCenter
      }
      MyTextField {
        id: tfAddr
        text: root.address
        width: Theme.dp(500)
        height: Theme.dp(150)
        title: "Address"
      }
    }
    Item {
      //the map image button
      width: Theme.dp(550)
      height: Theme.dp(250)
      anchors.horizontalCenter: parent.horizontalCenter

      Image {
        id: mapImg
        anchors.fill: parent
        source: "qrc:/assets/map_img.png"
        visible: false
      }
      Rectangle {
        id: mask
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        radius: Theme.dp(40)
        visible: false
        color: "red"
      }
      OpacityMask {
        anchors.fill: mapImg
        source: mapImg
        maskSource: mask
      }
      Item {
        width: Theme.dp(160)
        height: width
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: -Theme.dp(50)
        anchors.rightMargin: -Theme.dp(50)
        Icon {
          id: mapIcon
          width: Theme.dp(100)
          height: width
          anchors.centerIn: parent
          color: (root.latitude.length !== 0
                  && root.longitude.length !== 0) ? ui.color(
                                                      "red-a400") : ui.color(
                                                      "grey-700")
          source: "qrc:/icons/ic_place_48px.svg"
        }
      }
      SequentialAnimation {
        id: mapAnim
        PropertyAnimation {
          target: mapIcon
          easing.type: Easing.InExpo
          properties: "width"
          from: Theme.dp(100)
          to: Theme.dp(160)
        }
        PropertyAnimation {
          target: mapIcon
          easing.type: Easing.OutExpo
          properties: "width"
          from: Theme.dp(160)
          to: Theme.dp(100)
        }
      }

      MouseArea {
        anchors.fill: parent
        cursorShape: "PointingHandCursor"
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
          if (mouse.button == Qt.RightButton) {
            locationContextMenu.open()
          } else {
            console.log("left")
            setlocationPopup.open()
          }
        }
        Menu {
          id: locationContextMenu
          MenuItem {
            text: "Clear"
            icon {
              color: ui.color("grey-700")
              source: "qrc:/icons/delete_forever-black-48dp.svg"
            }

            font.pointSize: 10
            onTriggered: {
              root.clearLocation()
            }
          }
        }
      }
      Icon {
        //clear location icon
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: -Theme.dp(20)
        width: Theme.dp(100)
        height: width
        color: ui.color("grey-700")
        source: "qrc:/icons/delete_forever-black-48dp.svg"
        MouseArea {
          anchors.fill: parent
          cursorShape: "PointingHandCursor"
          onClicked: {
            root.clearLocation()
          }
        }
      }
    }

    Row {
      //Action buttons
      topPadding: Theme.baseMarginSize
      spacing: Theme.baseMarginSize
      anchors.horizontalCenter: parent.horizontalCenter

      MyButton {
        text: qsTr("Cancel")
        font.pointSize: 10
        icon.source: "qrc:/icons/delete_forever-black-48dp.svg"
        icon.width: Theme.dp(65)
        icon.height: Theme.dp(65)
        iconColor: ui.color("deep-purple-500")
        iconColorDown: ui.color("deep-purple-900")

        textColor: ui.color("deep-purple-500")
        textColorDown: ui.color("deep-purple-900")

        flat: true
        width: Theme.dp(250)
        onClicked: {
          root.canceled()
        }
      }

      MyButton {
        text: qsTr("Save")
        font.pointSize: 10
        icon.source: "qrc:/icons/save_alt-black-48dp.svg"
        icon.width: Theme.dp(55)
        icon.height: Theme.dp(55)
        iconColor: ui.color("white")
        iconColorDown: ui.color("grey-100")

        textColor: ui.color("white")
        textColorDown: ui.color("grey-100")
        Material.background: ui.color("deep-purple-900")
        flat: true
        width: Theme.dp(230)
        enabled: tfPhone.acceptableInput
        onClicked: {
          if (isAnUpdate) {
            couriersMonitor.updateGoto(courier_id, goto_id, {
                                         "from": tfStore.text,
                                         "description": tfDesc.text,
                                         "address": tfAddr.text,
                                         "phone": tfPhone.text,
                                         "latitude": root.latitude,
                                         "longitude": root.longitude,
                                         "accuracy": root.accuracy
                                       })
            root.accepted()
          } else {

            couriersMonitor.addGoto(courier_id, {
                                      "from": tfStore.text,
                                      "description": tfDesc.text,
                                      "address": tfAddr.text,
                                      "phone": tfPhone.text,
                                      "latitude": root.latitude,
                                      "longitude": root.longitude,
                                      "accuracy": root.accuracy
                                    })
            root.accepted()
          }
        }
      }
    }
  }
  Popup {
    id: setlocationPopup
    x: mapFromGlobal((window.width - width) / 2, (window.height - height) / 2).x
    y: mapFromGlobal((window.width - width) / 2, (window.height - height) / 2).y
    width: Theme.dp(1500)
    height: Theme.dp(1200)
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    padding: 0
    onOpened: mapLoader.sourceComponent = mapComponent
    onClosed: mapLoader.sourceComponent = undefined

    property var markerCoordinate: getMarkerCoordinate()
    property double accuracy: 0.0

    Loader {
      id: mapLoader
      anchors.fill: parent
      anchors.margins: 10
    }
    Component {
      id: mapComponent
      Map {
        copyrightsVisible: false
        plugin: Plugin {
          id: osmPlugin
          name: "osm"
          locales: ["ar_AR"]
          PluginParameter {
            name: "osm.mapping.host"
            value: "https://a.tile.openstreetmap.org/"
          }
          PluginParameter {
            name: "osm.mapping.cache.directory"
            value: "/home/youcef/workspace/c++/delivery_app/map_cache"
          }
        }
        center: getCenterCoordinate()
        zoomLevel: 15.5
        Component.onCompleted: {
          activeMapType = supportedMapTypes[supportedMapTypes.length - 1]
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            const coordinate = parent.toCoordinate(Qt.point(mouseX, mouseY))
            marker.coordinate = coordinate
            setlocationPopup.markerCoordinate = coordinate
            saveButton.enabled = true
          }
        }
        MapCircle {
          id: accuracyCircle
          center: marker.coordinate
          radius: calcRadius(accuracyComboBox.currentIndex)
          onRadiusChanged: setlocationPopup.accuracy = radius
          border.width: 0
          color: ui.colorFromHash(root.courier_id, 0.3)
          function calcRadius(idx) {
            if (idx === 0)
              return 0
            if (idx === 1)
              return 50
            if (idx === 2)
              return 100
            if (idx === 3)
              return 200
            return 0
          }
        }
        MapQuickItem {
          id: marker
          anchorPoint.x: markerImg.width / 2
          anchorPoint.y: markerImg.height
          coordinate: setlocationPopup.getMarkerCoordinate()
          sourceItem: Icon {
            id: markerImg
            width: Theme.dp(100)
            height: Theme.dp(130)
            source: "qrc:/icons/ic_place_48px.svg"
            color: ui.colorFromHash(root.courier_id)
          }
        }

        function getCenterCoordinate() {
          if (root.latitude.length > 0 && root.longitude.length > 0) {
            const lat = parseFloat(root.latitude)
            const longt = parseFloat(root.longitude)

            return QtPositioning.coordinate(lat, longt)
          }
          return QtPositioning.coordinate(35.186365358672745,
                                          -0.6319964944472076)
        }
      }
    }
    Pane {
      anchors.top: parent.top
      anchors.left: parent.leftInset
      anchors.margins: Theme.baseMarginSize
      Text {
        text: qsTr("Click anywhere to slelect the destination location.")
        font.family: "Roboto"
        verticalAlignment: Text.AlignVCenter
      }
    }

    Pane {
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: Theme.dp(100)
      topPadding: 0
      bottomPadding: 0
      leftPadding: Theme.tinyMarginSize
      rightPadding: Theme.tinyMarginSize
      Row {
        //Action buttons
        spacing: Theme.baseMarginSize
        ComboBox {
          id: accuracyComboBox
          model: ["Exacte", "<50m", "<100m", "<200m"]
          currentIndex: {
            if (root.accuracy === 0)
              return 0
            if (root.accuracy <= 50)
              return 1
            if (root.accuracy <= 100)
              return 2
            if (root.accuracy <= 200)
              return 3
            return 0
          }
          onCurrentIndexChanged: saveButton.enabled = true
        }
        Row {
          spacing: Theme.tinyMarginSize
          MyButton {
            text: qsTr("Cancel")
            font.pointSize: 10
            icon.source: "qrc:/icons/delete_forever-black-48dp.svg"
            icon.width: Theme.dp(65)
            icon.height: Theme.dp(65)
            iconColor: ui.color("deep-purple-500")
            iconColorDown: ui.color("deep-purple-900")

            textColor: ui.color("deep-purple-500")
            textColorDown: ui.color("deep-purple-900")

            flat: true
            width: Theme.dp(250)
            onClicked: setlocationPopup.close()
          }

          MyButton {
            id: saveButton
            text: qsTr("Save")
            font.pointSize: 10
            icon.source: "qrc:/icons/save_alt-black-48dp.svg"
            icon.width: Theme.dp(55)
            icon.height: Theme.dp(55)
            iconColor: ui.color("white")
            iconColorDown: ui.color("grey-100")

            textColor: ui.color("white")
            textColorDown: ui.color("grey-100")
            Material.background: ui.color("deep-purple-900")
            flat: true
            width: Theme.dp(230)
            enabled: false
            onClicked: {
              setlocationPopup.close()
              root.setLocation(
                    setlocationPopup.markerCoordinate.latitude.toString(),
                    setlocationPopup.markerCoordinate.longitude.toString(),
                    setlocationPopup.accuracy)
            }
          }
        }
      }
    }
    function getMarkerCoordinate() {
      if (root.latitude.length > 0 && root.longitude.length > 0) {
        const lat = parseFloat(root.latitude)
        const longt = parseFloat(root.longitude)
        return QtPositioning.coordinate(lat, longt)
      }
      return QtPositioning.coordinate()
    }
  }
  function clearLocation() {
    latitude = ""
    longitude = ""
    accuracy = 0.0
    mapIcon.color = ui.color("grey-700")
  }

  function setLocation(lat, lngt, accr) {
    if (lat.length === 0 || lngt.length === 0) {
      clearLocation()
      return
    }
    console.log("Setting location")
    latitude = lat
    longitude = lngt
    accuracy = accr
    mapIcon.color = ui.color("red-400")
    mapAnim.start()
  }
}
