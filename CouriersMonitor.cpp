#include "CouriersMonitor.h"

#include <QTimer>
#include <vector>
#include <QtPositioning/QtPositioning>
#include <QDebug>
#include "WebSocket/WSClient.h"

#include "Models/CouriersModelManager.h"

#include "Models/GotosModel.h"
#include "Models/CouriersModel.h"
#include "Models/ClientsLocationsModel.h"
#include "Models/MapViewedGotosProxyModel.h"
#include "Models/FiltredOnlineCouriersModel.h"

#include "MongoDb/MongoDBCollection.h"
#include "CourierToGotoMapRoute.h"
#include "UiNotifier.h"


CouriersMonitor::CouriersMonitor(QObject *parent)
    : QObject(parent)
{

    // MongoDb stuff
    m_pool = new MongoDbPool(this);
    m_gotosModel = new QIdentityProxyModel(this);
    m_couriersModelManager = new CouriersModelManager(m_pool, this);

    m_pool->init();

    // WebSocket stuff
    QTimer *reconnectWsTimer = new QTimer(this);
    reconnectWsTimer->setInterval(3000);
    reconnectWsTimer->setSingleShot(true);
    connect(reconnectWsTimer, &QTimer::timeout, [this]{
        qDebug() << "Reconnecting webSocket";
        connectWebSocket();
    });

    m_wsClient = new WSClient(this);
    m_wsClient->setApiKey("72e49df0-fc78-45dc-ace9-45ccd0fd18e2");
    m_wsClient->setEncryptionKey("2b05f63e9264c1d6c5ea61aa087d7302");
    m_wsClient->setUsername("admin");
    m_wsClient->setPassword("admin369");
    m_wsClient->enablePing();

    QObject::connect(m_wsClient, &WSClient::closed,[this, reconnectWsTimer]{
        if(m_wsClient->error() != QAbstractSocket::UnknownSocketError){
//            qDebug() << "Socket closed due to " << m_wsClient->errorString();
//            qDebug() << "Reconnecting after :"  << reconnectWsTimer->interval();
UiNotifier::get().addWarningNotification("Web-Socket closed", "Reconnecting...");
            reconnectWsTimer->start();
        }
    });


    connect(m_wsClient, &WSClient::authenticationSucceded, [this]{
        qDebug() << "Ws authenticated: "  << m_wsClient->id();
        UiNotifier::get().addInfoNotification("Web-Socket authenticated", "Login was successful");
        m_couriersModelManager->onlineCouriersModel()->setCurrentId(m_wsClient->id());
    });

    connect(m_wsClient, &WSClient::authenticationFailed, [](const QString &reason){
        UiNotifier::get().addErrorNotification("Web-Socket authentication Failed", "Reason: " + reason);
        qDebug() << "Ws authentication Failed: " << reason;
    });

    connect(m_wsClient, &WSClient::jsonObjReceived,this, &CouriersMonitor::onJsonObjectReceived);

    connectWebSocket();

    m_mapRoute = new CourierToGotoMapRoute(this);
    m_mapRoute->setModels(allCouriersModel(),
                          m_couriersModelManager->allGotosModel());
}

CouriersModel *CouriersMonitor::allCouriersModel()
{
    return m_couriersModelManager->allCouriersModel();
}

//CouriersModel *CouriersMonitor::onlineCouriersModel()
//{
//    return m_couriersModelManager->onlineCouriersModel();
//}

FiltredOnlineCouriersModel *CouriersMonitor::onlineCouriersModel()
{
    return m_couriersModelManager->onlineCouriersModel();
}

MapViewedCouriersProxyModel *CouriersMonitor::mapViewedCouriersProxyModel()
{
    return m_couriersModelManager->mapViewedCouriersProxyModel();
}

FilteredAndSortedGotosModel *CouriersMonitor::filteredGotosModel()
{
    return m_couriersModelManager->filteredGotosModel();
}

ClientsLocationsModel *CouriersMonitor::clientsLocationsModel()
{
    return m_couriersModelManager->clientsLocationsModel();
}

MapViewedGotosProxyModel *CouriersMonitor::mapViewedGotosProxyModel()
{
    return m_couriersModelManager->mapViewedGotosProxyModel();
}


QVariantMap CouriersMonitor::getCourierData(const QString &courierId){
    return m_couriersModelManager->getCourierData(courierId);
}

QVariantMap CouriersMonitor::getGotoData(const QString &courierId, const QString &gotoId)
{
    return m_couriersModelManager->getGotoData(courierId, gotoId);
}

//void CouriersMonitor::getGotoModelByCourierId(const QString &id)
//{
//    m_gotosModel->setSourceModel(m_couriersModelManager->getGotoModelByCourierId(id));
//}

void CouriersMonitor::filterGotoModelByCourierId(const QString &id)
{
    m_couriersModelManager->filterGotoModelByCourierId(id);
}

void CouriersMonitor::addGoto(const QString &courierId, const QVariantMap &varMap){
    if(courierId.isEmpty()) return;

//    auto gotosModel_ptr = m_couriersModelManager->getGotoModelByCourierId(courierId);
//    if(gotosModel_ptr != nullptr){
//        gotosModel_ptr->insert(varMap);
//    }

    m_couriersModelManager->allGotosModel()->insert(courierId, varMap);
}

void CouriersMonitor::deleteGoto(const QString &courierId, const QString &gotoId){
    if(courierId.isEmpty() || gotoId.isEmpty()) return;
//    auto gotosModel_ptr = m_couriersModelManager->getGotoModelByCourierId(courierId);
//    if(gotosModel_ptr != nullptr){
//        gotosModel_ptr->remove(gotoId);
//    }
    m_couriersModelManager->allGotosModel()->remove(gotoId);
}

void CouriersMonitor::updateGoto(const QString &courierId, const QString &gotoId, const QVariantMap &varMap){
    if(courierId.isEmpty() || gotoId.isEmpty()) return;
//    auto gotosModel_ptr = m_couriersModelManager->getGotoModelByCourierId(courierId);
//    if(gotosModel_ptr != nullptr){
//        gotosModel_ptr->update(gotoId, varMap);
//    }
    m_couriersModelManager->allGotosModel()->update(gotoId, varMap);
}

void CouriersMonitor::updateGotoStatus(const QString &courierId, const QString &gotoId, const QString &status){
    if(courierId.isEmpty() || gotoId.isEmpty()) return;
//    auto gotosModel_ptr = m_couriersModelManager->getGotoModelByCourierId(courierId);
//    if(gotosModel_ptr != nullptr){
//        gotosModel_ptr->updateStatus(gotoId, status);
//    }
    m_couriersModelManager->allGotosModel()->updateStatus(gotoId, status);
}

void CouriersMonitor::connectWebSocket()
{
    m_wsClient->open(QUrl(QStringLiteral("ws://ishbilia-server.herokuapp.com")));
//    m_wsClient->open(QUrl(QStringLiteral("ws://127.0.0.1:3000")));
}

void CouriersMonitor::onJsonObjectReceived(const QJsonObject &jsonObject)
{
    const QString type = jsonObject.value("type").toString();
    if(type == "couriers-list"){
        qDebug() << "couriers-list";
//        auto listJsonArray = jsonObject.value("list").toArray();
//        if(!listJsonArray.isEmpty()){
//            for(const auto e : listJsonArray){
//                const auto courierJsonObject = e.toObject();
//                const QString courier_id = courierJsonObject.value("courier_id").toString();
//                const QString username = courierJsonObject.value("username").toString();
//                const QString status = courierJsonObject.value("status").toString();

//                const QString latitude = courierJsonObject.value("latitude").toString();
//                const QString longitude = courierJsonObject.value("longitude").toString();
//                const QString accuracy = courierJsonObject.value("accuracy").toString();
//                const QString speed = courierJsonObject.value("speed").toString();
//            }
//        }
    }else if(type == "courier-status-update"){
        const QString courier_id = jsonObject.value("courier_id").toString();
        const QString status = jsonObject.value("status").toString();
        qDebug() << "courier-status-update: " << courier_id << status;

    }else if(type == "location-update"){
        qDebug() << "location-update";
        const QString courier_id = jsonObject.value("courier_id").toString();

        const QString latitude = jsonObject.value("latitude").toString();
        const QString longitude = jsonObject.value("longitude").toString();
        const QString accuracy = jsonObject.value("accuracy").toString();
        const QString speed = jsonObject.value("speed").toString();

        m_couriersModelManager->updateLocationUsingId(courier_id, latitude, longitude,
                                                  accuracy, speed);
    }
}




