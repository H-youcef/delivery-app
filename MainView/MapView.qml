import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtPositioning 5.15
import QtLocation 5.15

import "../tools/"
import "../"

Item {
  id: root

  property string selectedCourierId: ""
  property string selectedGotoId: ""

  property bool markerRotationEnabled: false

  onSelectedGotoIdChanged: {
    mapRoute.setGotoId(selectedGotoId)
    fitFovForSelectedCourierId()
  }

  RouteModel {
    id: routeModel
    autoUpdate: false
    plugin: osmPlugin
    query: RouteQuery {
      id: routeQuery
      property var routeEnds: mapRoute.routeEnds

      onRouteEndsChanged: {
        resetWayPoints()
      }

      function resetWayPoints() {
        clearWaypoints()
        if (routeEnds.courier_latitude && routeEnds.courier_longitude
            && routeEnds.goto_latitude && routeEnds.goto_longitude) {
          addWaypoint(QtPositioning.coordinate(
                        parseFloat(routeEnds.courier_latitude),
                        parseFloat(routeEnds.courier_longitude)))
          addWaypoint(QtPositioning.coordinate(
                        parseFloat(routeEnds.goto_latitude),
                        parseFloat(routeEnds.goto_longitude)))
        } else {
          routeModel.reset()
        }

        routeModel.update()
      }
    }
  }

  //TODO: add support for sattelaite view
  onSelectedCourierIdChanged: {
    fitFovForSelectedCourierId()
  }

  function fitFovForSelectedCourierId() {
    routeQuery.clearWaypoints()
    routeModel.update()

    if (selectedCourierId.length === 0)
      return
    let count = 0
    let mapItemsList = []
    for (var i = 0; i < map.mapItems.length; ++i) {
      if (map.mapItems[i].courierId === selectedCourierId) {
        mapItemsList.push(map.mapItems[i])
      }
    }
    if (mapItemsList.length > 1) {
      map.fitViewportToMapItems(mapItemsList)
    } else if (mapItemsList.length === 1) {
      map.center = mapItemsList[0].coordinate
    }
  }

  Plugin {
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
  //TODO: fit to field of view when needed
  Map {
    id: map
    anchors.fill: parent
    opacity: 0
    copyrightsVisible: false
    plugin: osmPlugin
    maximumZoomLevel: 17
    center: QtPositioning.coordinate(35.186365358672745,
                                     -0.6319964944472076) // Oslo
    zoomLevel: 15.5
    Component.onCompleted: {
      activeMapType = supportedMapTypes[supportedMapTypes.length - 1]
    }
    Behavior on center {
      CoordinateAnimation {
        duration: 700
        easing.type: Easing.OutCubic
      }
    }
    Behavior on zoomLevel {
      NumberAnimation {
        duration: 700
        easing.type: Easing.OutCubic
      }
    }

    MapItemView {
      //Routes
      model: routeModel
      delegate: MapRoute {
        route: routeData
        line.color: Qt.darker(ui.colorFromHash(selectedCourierId), 1.5)
        line.width: Theme.dp(20)
        smooth: true
        opacity: 0.8
      }
    }

    MapItemView {
      //Gotos view accuracy circle
      model: mapViewedGotosProxyModel
      delegate: MapCircle {
        id: accuracyCircle
        center: QtPositioning.coordinate(parseFloat(model.latitude),
                                         parseFloat(model.longitude))
        radius: model.accuracy
        border.width: 0
        color: ui.colorFromHash(model.courier_id, 0.3)
      }
    }

    MapItemView {
      //Couriers Accuracy view
      model: onlineCouriersModel
      delegate: MapCircle {
        center: QtPositioning.coordinate(latitude, longitude)
        color: ui.colorFromHash(model._id, 0.3)
        radius: accuracy * 1.5
        border.width: 0
      }
    }

    MapItemView {
      //Couriers Markers view
      model: mapViewedCouriersProxyModel
      delegate: CourierMapMarker {
        property string courierId: model._id
        iconColor: ui.colorFromHash(model._id)
        //        iconSource: "qrc:/icons/ic_navigation_48px.svg"
        iconSource: "qrc:/icons/info-black-48dp.svg"
        coordinate: QtPositioning.coordinate(latitude, longitude)
        //        enableMovementRotation: true
        iconScale: root.selectedCourierId === model._id ? 1.3 : 1
        font.pointSize: root.selectedCourierId === model._id ? 9 : 8
      }
    }

    MapItemView {
      //Gotos Markers view
      model: mapViewedGotosProxyModel
      delegate: MapQuickItem {
        anchorPoint.x: markerItem.width / 2
        anchorPoint.y: markerItem.height

        property string courierId: model.courier_id
        coordinate: QtPositioning.coordinate(parseFloat(model.latitude),
                                             parseFloat(model.longitude))

        sourceItem: Column {
          id: markerItem
          Icon {
            property real sizeScale: root.selectedCourierId === model.courier_id ? 1.3 : 1.0
            anchors.horizontalCenter: parent.horizontalCenter
            width: Theme.dp(100) * sizeScale
            height: Theme.dp(100) * sizeScale
            source: "qrc:/icons/ic_place_48px.svg"
            color: ui.colorFromHash(model.courier_id)
          }
        }
      }
    }

    //    Connections {
    //      target: couriersMonitor

    //      function onCourierAdded(courier) {
    //        var marker = Qt.createQmlObject("import QtLocation 5.13;
    //import QtQuick 2.13;
    //MapQuickItem {
    //property string courierId;
    //}", map)

    //        var icon = Qt.createQmlObject(
    //              'import "../tools/"; import "../"; Icon {}', marker)

    //        //                icon.source = "qrc:/icons/delivery_bike.svg"
    //        //                icon.width = Theme.dp(80)
    //        //                icon.height = icon.width * 1.8
    //        icon.source = "qrc:/icons/ic_place_48px.svg"
    //        icon.width = Theme.dp(80)
    //        icon.height = Theme.dp(80)
    //        icon.color = "blue"

    //        marker.anchorPoint = Qt.point(icon.width / 2.0, icon.height / 2.0)
    //        if (courier.location) {
    //          marker.coordinate = QtPositioning.coordinate(
    //                courier.location.latitude, courier.location.longitude)
    //        }
    //        marker.courierId = courier.id
    //        marker.sourceItem = icon
    //        map.addMapItem(marker)
    //      }

    //      function onCourierPositionChanged(courier) {
    //        for (var i = 0; i < map.mapItems.length; ++i) {
    //          if (map.mapItems[i].courierId === courier.id) {
    //            if (root.markerRotationEnabled) {

    //              const oldPos = Qt.vector2d(map.mapItems[i].coordinate.longitude,
    //                                         map.mapItems[i].coordinate.latitude)
    //              const newPos = Qt.vector2d(courier.location.longitude,
    //                                         courier.location.latitude)
    //              const newOrientationVec = newPos.minus(oldPos).normalized()

    //              const oldOrientationVec = Qt.vector2d(0, 1)
    //              const radAngle = Math.atan2(oldOrientationVec.y,
    //                                          oldOrientationVec.x) - Math.atan2(
    //                               newOrientationVec.y, newOrientationVec.x)

    //              let degAngle = radAngle * (180 / Math.PI)
    //              map.mapItems[i].rotation = degAngle
    //            }
    //            map.mapItems[i].coordinate = QtPositioning.coordinate(
    //                  courier.location.latitude, courier.location.longitude)
    //          }
    //        }
    //      }

    //      function onCourierRemoved(courier) {
    //        for (var i = 0; i < map.mapItems.length; ++i) {
    //          if (map.mapItems[i].courierId === courier.id) {
    //            map.removeMapItem(map.mapItems[i])
    //          }
    //        }
    //      }
    //    }
  }
  Rectangle {
    id: mask
    width: parent.width
    height: parent.height
    anchors.centerIn: parent
    radius: Theme.dp(50)
    visible: false
  }
  DropShadow {

    source: mask
    anchors.fill: mask
    verticalOffset: 2
  }
  OpacityMask {
    anchors.fill: map
    source: map
    maskSource: mask
  }

  ListModel {
    id: onlineCouriersLModel
    ListElement {
      username: "Bill Smith"
      courier_id: "5553264"
      status: "online"
      latitude: "35.188281056868490"
      longitude: "-0.633652915929360"
      accuracy: "10"
    }
    ListElement {
      username: "John Brown"
      courier_id: "5558426"
      status: "online"
      latitude: "35.193262603743293"
      longitude: "-0.634428648285905"
      accuracy: "15.0"
    }
    ListElement {
      username: "Bill Smith"
      courier_id: "5553287"
      status: "online"
      latitude: "35.189341961181199"
      longitude: "-0.629423302910482"
      accuracy: "20.0"
    }
  }
}
