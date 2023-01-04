#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QDebug>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickStyle>
#include "UiHelper.h"

#include "FileWatcher.h"
#include "CouriersMonitor.h"
#include "Models/CouriersModel.h"
#include "Models/OnlineCouriersModel.h"

#include "Models/GotosModel.h"
#include "Models/ClientsLocationsModel.h"
#include "Models/MapViewedGotosProxyModel.h"
#include "Models/FilteredAndSortedGotosModel.h"
#include "Models/MapViewedCouriersProxyModel.h"
#include "Models/FiltredOnlineCouriersModel.h"
#include "CourierToGotoMapRoute.h"

#include "UiNotifier.h"

#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    qRegisterMetaType<QVector<QJsonObject>>("QVector<QJsonObject>");
    qRegisterMetaType<QVector<QJsonObject>>("std::error_code");
    QQuickStyle::setStyle("Material");


//    const QUrl url(QStringLiteral("qrc:/main.qml"));
    const QUrl url("/home/youcef/workspace/c++/delivery_app/delivery_app/main.qml");
    UiHelper uiHelper;
    uiHelper.loadColorsFile("/home/youcef/workspace/c++/Projecto/projecto/material-colors.json");



    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("ui", &uiHelper);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    FileWatcher fileWatcher;
    fileWatcher.setEngine(engine);
    fileWatcher.setPath("/home/youcef/workspace/c++/delivery_app/delivery_app/");
    engine.rootContext()->setContextProperty("fileWatcher", &fileWatcher);

    CouriersMonitor couriersMonitor;

    engine.rootContext()->setContextProperty("couriersMonitor", &couriersMonitor);
    engine.rootContext()->setContextProperty("allCouriersModel", couriersMonitor.allCouriersModel());
    engine.rootContext()->setContextProperty("onlineCouriersModel", couriersMonitor.onlineCouriersModel());
    engine.rootContext()->setContextProperty("mapViewedCouriersProxyModel", couriersMonitor.mapViewedCouriersProxyModel());
    engine.rootContext()->setContextProperty("gotoModel", couriersMonitor.gotosModel());
    engine.rootContext()->setContextProperty("filteredGotosModel", couriersMonitor.filteredGotosModel());
    engine.rootContext()->setContextProperty("mapViewedGotosProxyModel",
                                             couriersMonitor.mapViewedGotosProxyModel());
    engine.rootContext()->setContextProperty("clientsLocationsModel",
                                             couriersMonitor.clientsLocationsModel());
    engine.rootContext()->setContextProperty("mapRoute", couriersMonitor.mapRoute());
    engine.rootContext()->setContextProperty("uiNotifier", &UiNotifier::get());

    engine.load(url);

    return app.exec();
}












