import QtQuick 2.13
import QtQuick.Layouts 1.13

import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import "../tools/"
import "../"

Rectangle {
  id: root
  color: ui.color("white")
  RowLayout {
    anchors.fill: parent

    CouriersColumn {
      id: couriersColumn
      Layout.fillHeight: true
      Layout.minimumWidth: Theme.dp(600)
      Layout.topMargin: Theme.largeMarginSize
      Layout.bottomMargin: Theme.largeMarginSize
      color: root.color
      //            border.width: 1
      onSelectedCourierIdChanged: {
        gotosColumn.selectedGotoId = ""
      }
    }

    Rectangle {
      Layout.fillHeight: true
      Layout.minimumWidth: Theme.dp(10)
      Layout.maximumWidth: Theme.dp(10)
      color: ui.color("pink-100")
    }

    GotosColumn {
      id: gotosColumn
      Layout.minimumWidth: Theme.dp(800)
      Layout.maximumWidth: Theme.dp(800)
      Layout.fillHeight: true
      Layout.topMargin: Theme.largeMarginSize
      Layout.bottomMargin: Theme.largeMarginSize
      Layout.leftMargin: Theme.tinyMarginSize
      Layout.rightMargin: Theme.smallMarginSize
      radius: Theme.dp(60)
      color: root.color

      selectedCourierId: couriersColumn.selectedCourierId
    }

    MapView {
      id: mapView
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.topMargin: Theme.baseMarginSize
      Layout.bottomMargin: Theme.baseMarginSize
      Layout.rightMargin: Theme.baseMarginSize

      selectedCourierId: couriersColumn.selectedCourierId
      selectedGotoId: gotosColumn.selectedGotoId
    }
    //    ListView {
    //      id: clLv
    //      Layout.fillHeight: true
    //      Layout.fillWidth: true
    //      Layout.topMargin: Theme.largeMarginSize
    //      Layout.bottomMargin: Theme.largeMarginSize
    //      clip: true
    //      model: clientsLocationsModel
    //      spacing: 5
    //      delegate: Pane {
    //        Material.elevation: 4
    //        Column {
    //          spacing: 2
    //          width: Theme.dp(700)
    //          Text {
    //            text: model._id
    //          }
    //          Text {
    //            text: model.phone
    //          }
    //          Text {
    //            text: model.address
    //          }
    //          Text {
    //            text: model.latitude
    //          }
    //          Text {
    //            text: model.longitude
    //          }
    //          Text {
    //            text: model.accuracy
    //          }
    //        }
    //      }
    //    }
  }
}
