#include "GotosModel.h"

#include "../MongoDb/MongoDBCollection.h"
#include "ClientsLocationsModel.h"
#include <QDebug>

//TODO: to be handled (created) by couriersModel

GotosModel::GotosModel(MongoDBCollection *mongoDbCollection, QObject *parent)
    : QAbstractListModel(parent),
    m_mongoDbCollection(mongoDbCollection)
{
    init();
}

GotosModel::GotosModel(const QString &courierId, MongoDBCollection *mongoDbCollection, QObject *parent)
    : QAbstractListModel(parent),
      m_courierId(courierId),
      m_mongoDbCollection(mongoDbCollection)
{
    init();
}

void GotosModel::init()
{
    m_clientsLocationsModel = new ClientsLocationsModel(m_mongoDbCollection->pool(), this);
}

QVariant GotosModel::data(const QModelIndex &index, int role) const
{

    if(index.row() >= 0 && index.row() < rowCount()){

        switch (role) {
        case _ID:
            return m_gotos[index.row()].value("_id");
        case COURIER_ID:
            return m_gotos[index.row()].value("courier_id");
        case STATUS:
            return m_gotos[index.row()].value("status");
        case LATITUDE:
            return m_gotos[index.row()].value("latitude");
        case LONGITUDE:
            return m_gotos[index.row()].value("longitude");
        case ACCURACY:
            return m_gotos[index.row()].value("accuracy");
        case PHONE:
            return m_gotos[index.row()].value("phone");
        case START_TIME:
            return m_gotos[index.row()].value("start_time");
        case FROM:
            return m_gotos[index.row()].value("from");
        case ADDRESS:
            return m_gotos[index.row()].value("address");
        case DESCRIPTION:
            return m_gotos[index.row()].value("description");
        case SEEN_BY_COURIER:
            return m_gotos[index.row()].value("seen_by_courier");
        case SYNC:
            return m_gotos[index.row()].value("sync", false);
        default:
            return QVariant();
        }

    }
    return QVariant();
}

QHash<int, QByteArray> GotosModel::roleNames() const
{
    static QHash<int, QByteArray> roleNames = {
        {COURIER_ID , "courier_id" },
        {_ID        , "_id"        },
        {STATUS     , "status"     },
        {LATITUDE   , "latitude"   },
        {LONGITUDE  , "longitude"  },
        {ACCURACY   , "accuracy"   },
        {PHONE      , "phone"      },
        {START_TIME , "start_time" },
        {FROM       , "from"       },
        {ADDRESS    , "address"    },
        {DESCRIPTION, "description"},
        {SYNC       , "sync"       },
        {SEEN_BY_COURIER, "seen_by_courier"},
    };
    return roleNames;
}

int GotosModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_gotos.size();
}

QVariantMap GotosModel::getGotoById(const QString &goto_id)
{
    auto it = findGotoById(goto_id);
    if(it != m_gotos.end()){
        return *it;
    }
    return {};
}

void GotosModel::addGoto(const QString &goto_id, const QString &courier_id,
                         const QString &status, const QString &latitude,
                         const QString &longitude, const QString &phone,
                         const double start_time, const QString &from,
                         const QString &address, const QString &description,
                         bool seenByCourier)
{
    auto it = findGotoById(goto_id);
    if(it != m_gotos.end()){
        return;
    }
    beginInsertRows(QModelIndex(), 0, 0);
    QVariantMap gotoMap;
    gotoMap.insert("_id", goto_id);
    gotoMap.insert("courier_id", courier_id);
    gotoMap.insert("status"    , status);
    gotoMap.insert("latitude"  , latitude);
    gotoMap.insert("longitude" , longitude);
    gotoMap.insert("phone", phone);
    gotoMap.insert("start_time", start_time);
    gotoMap.insert("from", from);
    gotoMap.insert("address", address);
    gotoMap.insert("description", description);
    gotoMap.insert("sync", false);
    gotoMap.insert("seen_by_courier", seenByCourier);
    m_gotos.prepend(gotoMap);
    endInsertRows();
}

void GotosModel::addGoto(const QVariantMap &gotoHash){
    auto it = findGotoById(gotoHash.value("_id").toString());
    if(it != m_gotos.end()){
        return;
    }
    beginInsertRows(QModelIndex(), 0, 0);
    m_gotos.prepend(gotoHash);
    endInsertRows();
}

void GotosModel::updateGoto(const QString gotoId, const QVariantMap &gotoHash)
{
    QVariantMap newGoto = gotoHash;
    newGoto.remove("_id"); //because it is oid not a string
    newGoto.insert("_id", gotoId);
    auto it = findGotoById(gotoId);

    if(it == m_gotos.end()){
        //not found
        beginInsertRows(QModelIndex(), 0, 0);
        m_gotos.prepend(newGoto);
        endInsertRows();
    }else if(newGoto != *it){
        *it = newGoto;
        int row = static_cast<int>(std::distance(m_gotos.begin(), it));
        QVector<int> roles;
        auto rolesVals = roleNames();
        for(auto rit = rolesVals.begin(); rit != rolesVals.end(); ++rit){
            roles.push_back(rit.key());
        }
        emit dataChanged(index(row), index(row), roles);
    }

}

void GotosModel::updateSync(const QString &goto_id, bool isSyncing){
    auto it = findGotoById(goto_id);
    if(it == m_gotos.end())
        return;

    if(it->value("sync").toBool() == isSyncing) return;
    it->insert("sync", isSyncing);

    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    emit dataChanged(index(row), index(row), {SYNC});
}

void GotosModel::updateIsSeenByCourier(const QString &goto_id, bool seenByCourier){
    auto it = findGotoById(goto_id);
    if(it == m_gotos.end())
        return;

    if(it->value("seen_by_courier").toBool() == seenByCourier) return;
    it->insert("seen_by_courier", seenByCourier);

    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    emit dataChanged(index(row), index(row), {SEEN_BY_COURIER});
}

void GotosModel::updateStatusById(const QString &goto_id, const QString &status)
{
    auto it = findGotoById(goto_id);
    if(it == m_gotos.end())
        return;
    it->insert("status", status);
    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    emit dataChanged(index(row), index(row), {STATUS});
}

void GotosModel::updateLocationById(const QString &goto_id, const QString &latitude,
                                    const QString &longitude)
{
    auto it = findGotoById(goto_id);
    if(it == m_gotos.end())
        return;
    it->insert("latitude", latitude);
    it->insert("longitude", longitude);
    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    emit dataChanged(index(row), index(row), {LATITUDE, LONGITUDE});
}

void GotosModel::removeGotoById(const QString &goto_id)
{
    auto it = findGotoById(goto_id);
    if(it == m_gotos.end()) return;
    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    beginRemoveRows(QModelIndex(), row, row);
    m_gotos.erase(it);
    endRemoveRows();
}

void GotosModel::clear()
{
    if(m_gotos.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    m_gotos.clear();
    endRemoveRows();
}

void GotosModel::refetch()
{
    if(m_mongoDbCollection == nullptr) return;
    clear();
    auto befor = QDateTime::currentMSecsSinceEpoch() - (2 * 3600 * 1000);
    QJsonObject filter;
    filter.insert("start_time",QJsonObject({{"$gt", befor}}));
    filter.insert("courier_id", m_courierId);
    m_mongoDbCollection->find(filter);
}

void GotosModel::insert(const QVariantMap &gotoVM)
{
    insert(m_courierId, gotoVM);
}

void GotosModel::insert(const QString &courierId, const QVariantMap &gotoVM)
{

    const auto oid = MongoDBCollection::makeNewOidJsonObject();
    const QString status = "pending";
    const auto start_time = QDateTime::currentMSecsSinceEpoch();

    QVariantMap gotoMap = gotoVM;

    gotoMap.insert("_id", MongoDBCollection::oidToString(oid));
    gotoMap.insert("courier_id", courierId);
    gotoMap.insert("status"    , status);
    gotoMap.insert("start_time", start_time);
    gotoMap.insert("seen_by_courier", false);
    gotoMap.insert("sync", true);
    addGoto(gotoMap);

    if(m_mongoDbCollection){
        auto jsonObject = QJsonObject::fromVariantMap(gotoMap);
        jsonObject.insert("_id", oid);
        jsonObject.remove("sync");
        qDebug() << "Inserting in db: " << jsonObject.value("seen_by_courier");
        m_mongoDbCollection->insert(jsonObject);
    }

    const QString lat = gotoMap.value("latitude").toString();
    const QString longt = gotoMap.value("longitude").toString();

    if(!lat.isEmpty() && !longt.isEmpty()){
        const QString phone = gotoMap.value("phone").toString();
        const QString addr = gotoMap.value("address").toString();
        const double accu = gotoMap.value("accuracy").toDouble();
        m_clientsLocationsModel->addClientLocation(phone, addr, lat, longt, accu);
    }

}

void GotosModel::remove(const QString &goto_id){
    removeGotoById(goto_id);

    if(m_mongoDbCollection){
        m_mongoDbCollection->deleteById(goto_id);
    }
}

void GotosModel::update(const QString &gotoId, const QVariantMap &varMap){

    auto it = findGotoById(gotoId);
    if(it == m_gotos.end())
        return;
//    bool isSame = true;
//    for(auto fit = varMap.begin(); fit != varMap.end(); ++fit){
//        if(it->value(fit.key()) != fit.value()){
//            isSame = false;
//            break;
//        }
//    }
//    if(isSame) return;


    for(auto fit = varMap.begin(); fit != varMap.end(); ++fit){
        it->insert(fit.key(), fit.value());
    }
    it->insert("sync", true);
    it->insert("seen_by_courier", false);
    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    QVector<int> roles;
    auto rolesVals = roleNames();
    for(auto rit = rolesVals.begin(); rit != rolesVals.end(); ++rit){
        roles.push_back(rit.key());
    }
    emit dataChanged(index(row), index(row), roles);

    //Db update
    if(m_mongoDbCollection){
        auto jsonObject = QJsonObject::fromVariantMap(*it);
        jsonObject.remove("_id");
        jsonObject.remove("sync");
        QJsonObject updateJsonObject;
        updateJsonObject.insert("$set", jsonObject);
//        qDebug()  << "Updating using: " << jsonObject;
        m_mongoDbCollection->updateById(it->value("_id").toString(), updateJsonObject);
    }
}

void GotosModel::updateStatus(const QString &gotoId, const QString &status)
{
    auto it = findGotoById(gotoId);
    if(it == m_gotos.end())
        return;
    if(it->value("status").toString() == status) return;
    it->insert("status", status);
    it->insert("sync", true);
    int row = static_cast<int>(std::distance(m_gotos.begin(), it));
    emit dataChanged(index(row), index(row), {STATUS});

    //Db update
    if(m_mongoDbCollection){
        QJsonObject updateJsonObject;
        updateJsonObject.insert("$set", QJsonObject({{"status", status}}));
        m_mongoDbCollection->updateById(it->value("_id").toString(), updateJsonObject);
    }
}

bool GotosModel::contains(const QString &id){
    return findGotoById(id) != m_gotos.end();
}

QList<QVariantMap>::iterator
GotosModel::findGotoById(const QString &_id)
{
    return std::find_if(m_gotos.begin(), m_gotos.end(),
                        [&_id](const QVariantMap &gt){
        return gt.value("_id") == _id;
    });
}


