#include "CouriersModelManager.h"

#include "CouriersModel.h"
#include "GotosModel.h"
#include "../UiNotifier.h"


#include "../MongoDb/MongoDbPool.h"
#include "../MongoDb/MongoDBCollection.h"
#include "MapViewedGotosProxyModel.h"
#include "FilteredAndSortedGotosModel.h"
#include "MapViewedCouriersProxyModel.h"
#include "FiltredOnlineCouriersModel.h"
#include <QJsonArray>

CouriersModelManager::CouriersModelManager(MongoDbPool *pool, QObject *parent)
    :QObject(parent)
{
    m_gotosDbCollection = new MongoDBCollection("gotos", pool, this);
    connect(m_gotosDbCollection, &MongoDBCollection::insertFinished,
            this, &CouriersModelManager::onGotosInsertFinished);
    connect(m_gotosDbCollection, &MongoDBCollection::updateOneFinished,
            this, &CouriersModelManager::onGotosUpdateOneFinished);
    connect(m_gotosDbCollection, &MongoDBCollection::findFinished,
            this, &CouriersModelManager::onGotosFindFinished);
    connect(m_gotosDbCollection, &MongoDBCollection::error,
            this, &CouriersModelManager::onGotosDbError);
    connect(m_gotosDbCollection, &MongoDBCollection::changeDetected,
            this, &CouriersModelManager::onGotosDbChangeDetected);

    m_allCouriersModel = new CouriersModel(m_gotosDbCollection, this);
//    m_onlineCouriersModel = new CouriersModel(m_gotosDbCollection, this);

    m_filtredOnlineCouriersModel = new FiltredOnlineCouriersModel(this);
    m_filtredOnlineCouriersModel->setSourceModel(m_allCouriersModel);


    m_mapViewedCouriersProxyModel = new MapViewedCouriersProxyModel(this);
    m_mapViewedCouriersProxyModel->setSourceModel(m_filtredOnlineCouriersModel);

    m_allGotosModel = new GotosModel(m_gotosDbCollection, this);
    m_filteredGotosModel = new FilteredAndSortedGotosModel(this);
    m_filteredGotosModel->setSourceModel(m_allGotosModel);
    m_filteredGotosModel->setFilterRole(GotosModel::COURIER_ID);


    m_mapViewedGotosProxyModel = new MapViewedGotosProxyModel(this);
    m_mapViewedGotosProxyModel->setSourceModel(m_allGotosModel);

    m_couriersDbCollection = new MongoDBCollection("logins", pool, this);
    connect(m_couriersDbCollection, &MongoDBCollection::findFinished,
            this, &CouriersModelManager::onFindFinished);
    connect(m_couriersDbCollection, &MongoDBCollection::error,
            this, &CouriersModelManager::onDbError);
    connect(m_couriersDbCollection, &MongoDBCollection::changeDetected,
            this, &CouriersModelManager::onDbChangeDetected);

    QJsonObject projection;
    projection.insert("username", 1);
    projection.insert("connection_state", 1);
    projection.insert("roles", 1);
    m_couriersDbCollection->find({}, projection);

}

ClientsLocationsModel *CouriersModelManager::clientsLocationsModel()
{
    return m_allGotosModel->clientsLocationsModel();
}

QVariantMap CouriersModelManager::getCourierData(const QString &courierId) const
{
    return m_allCouriersModel->getCourierById(courierId);
}

QVariantMap CouriersModelManager::getGotoData(const QString &courierId,
                                              const QString &gotoId) const
{
    return m_allGotosModel->getGotoById(gotoId);
}

void CouriersModelManager::updateLocationUsingId(const QString &courier_id,
                                                 const QString &latitude,
                                                 const QString &longitude,
                                                 const QString &accuracy,
                                                 const QString &speed)
{
    m_allCouriersModel->updateLocationUsingId(courier_id, latitude, longitude, accuracy, speed);
    //    m_onlineCouriersModel->updateLocationUsingId(courier_id, latitude, longitude, accuracy, speed);
}

void CouriersModelManager::filterGotoModelByCourierId(const QString &id)
{
    m_filteredGotosModel->setFilterFixedString(id);
}


void CouriersModelManager::onFindFinished(const QVector<QJsonObject> &joVec)
{

    m_allCouriersModel->clear();
    QList<QVariantMap> allCouriersList;

    for(const auto &jo : joVec){
        const auto rolesJsonArray = jo.value("roles").toArray();
        if(!rolesJsonArray.contains("courier")){
            continue;
        }
        QVariantMap courier;
        const QString status = jo.value("connection_state").toString();
        const QString courierId = MongoDBCollection::oidToString(jo.value("_id").toObject());
        courier.insert("_id", courierId);
        courier.insert("username"  , jo.value("username").toString());
        courier.insert("status"    , jo.value("connection_state").toString());

        //Fake locations
        if(jo.value("username").toString() == "courier1"){
            courier.insert("latitude", "35.188362665386400");
            courier.insert("longitude", "-0.633752771888425");
        }else if(jo.value("username").toString() == "courier2"){
            courier.insert("latitude", "35.189429397732951");
            courier.insert("longitude", "-0.629387640075976");
        }else if(jo.value("username").toString() == "courier3"){
            courier.insert("latitude", "35.193236099196355");
            courier.insert("longitude", "-0.634342160661788");
        }


        allCouriersList.append(courier);

    }

    m_allCouriersModel->addMultipleCouriers(allCouriersList);

    queryGotosDbCollection();
}

void CouriersModelManager::onDbChangeDetected(const QString &op, const QString &key, const QJsonObject &doc)
{
    const auto rolesJsonArray = doc.value("roles").toArray();
    if(!rolesJsonArray.contains("courier"))
        return;
    if(op == "insert"){
        const QString courier_id = key;
        const QString username = doc.value("username").toString();
        const QString status = doc.value("connection_state").toString();

            if(m_allCouriersModel->getCourierById(courier_id).isEmpty()){
                m_allCouriersModel->addCourier(courier_id, username, status, "", "", "", "");
//                m_gotosModels.insert(courier_id, new GotosModel(courier_id, m_gotosDbCollection, this));
            }else{
                m_allCouriersModel->updateStatusUsingId(courier_id, status);
                m_allCouriersModel->updateUsernameUsingId(courier_id, username);
            }

//            if(status == "online"){
//                m_onlineCouriersModel->addCourier(courier_id, username, status, "", "", "", "");
//            }else{
//                m_onlineCouriersModel->removeCourierById(courier_id);
//            }

    }else if(op == "update" || op == "replace"){
        const QString courier_id = key;
        const QString username = doc.value("username").toString();
        const QString status = doc.value("connection_state").toString();

        m_allCouriersModel->updateStatusUsingId(courier_id, status);
        m_allCouriersModel->updateUsernameUsingId(courier_id, username);

//        if(status == "online"){
//            m_onlineCouriersModel->addCourier(courier_id, username, status, "", "", "", "");
//        }else{
//            m_onlineCouriersModel->removeCourierById(courier_id);
//        }
    }else if(op == "delete"){
        const QString courier_id = key;
//        m_onlineCouriersModel->removeCourierById(courier_id);
        m_allCouriersModel->removeCourierById(courier_id);
//        m_gotosModels.remove(key);
    }

}

void CouriersModelManager::onDbError(const QString &op, const QString &message, const int errCode)
{
    qDebug() << "CouriersModelManager: Db error: "
             << " operation: " << op
             << " ec: " << errCode << " " << message;
    UiNotifier::get().addErrorNotification("Error in Couriers Db collection",
                                           QString("Operation: %1\n Message: %2\n ErrorCode: %3")
                                               .arg(op)
                                               .arg(message)
                                               .arg(errCode));
}

void CouriersModelManager::onGotosInsertFinished(const QVector<QString> &resultIds)
{
    //Failed to update
    if(resultIds.length() == 0) return;

    for(const auto &id : resultIds){
        m_allGotosModel->updateSync(id, false);
//        for(auto &gotosModel : m_gotosModels){
//            if(gotosModel->contains(id)){
//                gotosModel->updateSync(id, false);
//                break;
//            }
//        }
    }
}

void CouriersModelManager::onGotosUpdateOneFinished(int count, const QJsonObject &filter)
{
    //Failed to update
    if(count == 0) return;

    const QString updatedId = MongoDBCollection::oidToString(filter.value("_id").toObject());

    m_allGotosModel->updateSync(updatedId, false);
//    for(auto &gotosModel : m_gotosModels){
//        if(gotosModel->contains(updatedId)){
//            gotosModel->updateSync(updatedId, false);
//            break;
//        }
//    }
}


//GotosModel *CouriersModelManager::getGotoModelByCourierId(const QString &id)
//{

//    if(id.isEmpty() || !m_gotosModels.contains(id))
//        return nullptr;
//    return m_gotosModels.value(id);
//}


void CouriersModelManager::queryGotosDbCollection()
{
    m_allGotosModel->clear();

//    for(auto &gotosModel : m_gotosModels){
//        gotosModel->clear();
//    }
    auto befor = QDateTime::currentMSecsSinceEpoch() - (2 * 3600 * 1000);
    QJsonObject filter;
    filter.insert("start_time",QJsonObject({{"$gt", befor}}));
    m_gotosDbCollection->find(filter);
}

void CouriersModelManager::onGotosFindFinished(const QVector<QJsonObject> &joVec)
{
    for(const auto &jo : joVec){
        const QString courierId = jo.value("courier_id").toString();
        if(courierId.isEmpty()) continue;
        const QString gotoId = MongoDBCollection::oidToString(jo.value("_id").toObject());
        QVariantMap gotoMap = jo.toVariantMap();
        gotoMap.insert("_id", gotoId);
        m_allGotosModel->addGoto(gotoMap);

//        if(m_gotosModels.contains(courierId)){
//            auto gotosModel = m_gotosModels.value(courierId);
//            gotosModel->addGoto(gotoMap);
//        }
    }
}

void CouriersModelManager::onGotosDbChangeDetected(const QString &op, const QString &key, const QJsonObject &doc)
{
    qDebug() << "onGotosDbChangeDetected: " << op << " " << key;
    m_allGotosModel->updateGoto(key, doc.toVariantMap());
}

void CouriersModelManager::onGotosDbError(const QString &op, const QString &message, const int errCode)
{
    qDebug() << "CouriersModelManager: onGotosDbError: "
             << " operation: " << op
             << " ec: " << errCode << " " << message;
    UiNotifier::get().addErrorNotification("Error in Gotos/Tasks Db collection",
                                           QString("Operation: %1\n Message: %2\n ErrorCode: %3")
                                               .arg(op)
                                               .arg(message)
                                               .arg(errCode));
}
