#include "CouriersModel.h"
#include "../MongoDb/MongoDBCollection.h"
#include <algorithm>
#include "GotosModel.h"

CouriersModel::CouriersModel(MongoDBCollection *gotosDbCollection, QObject *parent)
    :QAbstractListModel(parent)
{
//    m_gotosDbCollection = gotosDbCollection;
//    connect(m_gotosDbCollection, &MongoDBCollection::findFinished,
//            this, &CouriersModel::onGotosFindFinished);
//    connect(m_gotosDbCollection, &MongoDBCollection::error,
//            this, &CouriersModel::onGotosDbError);
//    connect(m_gotosDbCollection, &MongoDBCollection::changeDetected,
//            this, &CouriersModel::onGotosDbChangeDetected);

}

QVariant CouriersModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= 0 && index.row() < rowCount()){

        switch (role) {
        case _ID:
            return m_couriers[index.row()].value("_id");
        case USERNAME:
            return m_couriers[index.row()].value("username");
        case STATUS:
            return m_couriers[index.row()].value("status");
        case LATITUDE:
            return m_couriers[index.row()].value("latitude");
        case LONGITUDE:
            return m_couriers[index.row()].value("longitude");
        case ACCURACY:
            return m_couriers[index.row()].value("accuracy");
        case SPEED:
            return m_couriers[index.row()].value("speed");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QHash<int, QByteArray> CouriersModel::roleNames() const
{
    static QHash<int, QByteArray> roleNames = {
        {_ID , "_id"},
        {USERNAME   , "username"  },
        {STATUS     , "status"    },
        {LATITUDE   , "latitude"  },
        {LONGITUDE  , "longitude" },
        {ACCURACY   , "accuracy"  },
        {SPEED      , "speed"     },
    };
    return roleNames;
}

int CouriersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_couriers.size();
}

void CouriersModel::addCourier(const QString &_id, const QString &username,
                               const QString &status, const QString &latitude,
                               const QString &longitude, const QString &accuracy,
                               const QString &speed)
{
    auto it = findCourierById(_id);
    //If courier exists, return.
    if(it != m_couriers.end()){
        return;
    }
    beginInsertRows(QModelIndex(), m_couriers.size(), m_couriers.size());
    QVariantMap courier;
    courier.insert("_id", _id);
    courier.insert("username"  , username);
    courier.insert("status"    , status);
    courier.insert("latitude"  , latitude);
    courier.insert("longitude" , longitude);
    courier.insert("accuracy"  , accuracy);
    courier.insert("speed"     , speed);
    m_couriers.append(courier);
//    m_gotosModels.insert(_id, new GotosModel(_id, m_gotosDbCollection, this));
    endInsertRows();
}

void CouriersModel::addCourier(const QVariantMap &courier)
{
    const QString _id = courier.value("_id").toString();
    if(_id.isEmpty()){
        return;
    }
    auto it = findCourierById(_id);
    //If courier exists, return.
    if(it != m_couriers.end()){
        return;
    }
    beginInsertRows(QModelIndex(), m_couriers.size(), m_couriers.size());
    m_couriers.append(courier);
//    m_gotosModels.insert(_id, new GotosModel(_id, m_gotosDbCollection, this));
    endInsertRows();
}

void CouriersModel::addMultipleCouriers(const QList<QVariantMap> &couriers)
{
    QList<QVariantMap> couriersList;
    for(const auto &c : couriers){
        const QString _id = c.value("_id").toString();
        if(_id.isEmpty()){
            continue;
        }
        auto it = findCourierById(_id);
        //If courier exists, return.
        if(it != m_couriers.end()){
            continue;
        }
        couriersList.push_back(c);
//        m_gotosModels.insert(_id, new GotosModel(_id, m_gotosDbCollection, this));
    }
    beginInsertRows(QModelIndex(), m_couriers.size(), m_couriers.size() + couriersList.size() - 1);
    m_couriers.append(couriersList);
    endInsertRows();
}

void CouriersModel::removeCourierById(const QString &_id)
{
    auto it = findCourierById(_id);
    if(it == m_couriers.end()) return;
    int row = static_cast<int>(std::distance(m_couriers.begin(), it));
    beginRemoveRows(QModelIndex(), row, row);
    m_couriers.erase(it);

    //Extra safty
//    auto gotoModel = m_gotosModels.value(_id);
//    if(m_gotosModels.contains(_id) && gotoModel){
//        gotoModel->deleteLater();
//    }
//    m_gotosModels.remove(_id);

    endRemoveRows();
}

QVariantMap CouriersModel::updateLocationUsingId(const QString &_id,
                                          const QString &latitude,
                                          const QString &longitude,
                                          const QString &accuracy,
                                          const QString &speed)
{
    auto it = findCourierById(_id);

    if(it == m_couriers.end()) return {};
    it->insert("latitude"  , latitude);
    it->insert("longitude" , longitude);
    it->insert("accuracy"  , accuracy);
    it->insert("speed"     , speed);
    int row = static_cast<int>(std::distance(m_couriers.begin(), it));
    emit dataChanged(index(row), index(row), {LATITUDE, LONGITUDE, ACCURACY, SPEED});
    return *it;
}

QVariantMap CouriersModel::updateStatusUsingId(const QString &_id, const QString &status)
{
    auto it = findCourierById(_id);
    if(it == m_couriers.end()) return {};
    if(it->value("status") != status){
        it->insert("status", status);
        int row = static_cast<int>(std::distance(m_couriers.begin(), it));
        emit dataChanged(index(row), index(row), {STATUS});
    }
    return *it;
}

QVariantMap CouriersModel::updateUsernameUsingId(const QString &_id,
                                                                const QString &username)
{
    auto it = findCourierById(_id);
    if(it == m_couriers.end()) return {};
    if(it->value("username") != username){
        it->insert("username", username);
        int row = static_cast<int>(std::distance(m_couriers.begin(), it));
        emit dataChanged(index(row), index(row), {USERNAME});
    }
    return *it;
}

QList<QVariantMap>::iterator CouriersModel::findCourierById(const QString &_id)
{
    return std::find_if(m_couriers.begin(), m_couriers.end(),
                        [&_id](const QVariantMap &courier){
        return courier.value("_id") == _id;
    });
}

void CouriersModel::clear(){
    if(!m_couriers.empty()){
        beginRemoveRows(QModelIndex(), 0, m_couriers.size() - 1);
        m_couriers.clear();
        endRemoveRows();
    }
//    for(auto &v : m_gotosModels){
//        v->deleteLater();
//    }
//    m_gotosModels.clear();
}

QVariantMap CouriersModel::getCourierById(const QString &_id)
{
    auto it = findCourierById(_id);
    if(it != m_couriers.end())
        return *it;
    return {};
}


