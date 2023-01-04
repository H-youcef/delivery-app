#include "OnlineCouriersModel.h"

#include "../MongoDb/MongoDBCollection.h"
#include <algorithm>

OnlineCouriersModel::OnlineCouriersModel(QObject *parent)
    :QAbstractListModel(parent)
{

}

QVariant OnlineCouriersModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= 0 && index.row() < rowCount()){

        switch (role) {
        case COURIER_ID:
            return m_couriers[index.row()].value("courier_id");
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

QHash<int, QByteArray> OnlineCouriersModel::roleNames() const
{
    static QHash<int, QByteArray> roleNames = {
        {COURIER_ID , "courier_id"},
        {USERNAME   , "username"  },
        {STATUS     , "status"    },
        {LATITUDE   , "latitude"  },
        {LONGITUDE  , "longitude" },
        {ACCURACY   , "accuracy"  },
        {SPEED      , "speed"     },
    };
    return roleNames;
}

int OnlineCouriersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_couriers.size();
}

void OnlineCouriersModel::addCourier(const QString &courier_id, const QString &username,
                               const QString &status, const QString &latitude,
                               const QString &longitude, const QString &accuracy,
                               const QString &speed)
{
    auto it = findCourierById(courier_id);
    //If courier exists, return.
    if(it != m_couriers.end()){
        return;
    }
    beginInsertRows(QModelIndex(), m_couriers.size(), m_couriers.size());
    QHash<QString, QString> courier;
    courier.insert("courier_id", courier_id);
    courier.insert("username"  , username);
    courier.insert("status"    , status);
    courier.insert("latitude"  , latitude);
    courier.insert("longitude" , longitude);
    courier.insert("accuracy"  , accuracy);
    courier.insert("speed"     , speed);
    m_couriers.append(courier);
    endInsertRows();
}

void OnlineCouriersModel::addCourier(const QHash<QString, QString> &courier)
{
    const QString courier_id = courier.value("courier_id");
    if(courier_id.isEmpty()){
        return;
    }
    auto it = findCourierById(courier_id);
    //If courier exists, return.
    if(it != m_couriers.end()){
        return;
    }
    beginInsertRows(QModelIndex(), m_couriers.size(), m_couriers.size());
    m_couriers.append(courier);
    endInsertRows();
}

QHash<QString, QString> OnlineCouriersModel::getCourierById(const QString &courier_id)
{
    auto it = findCourierById(courier_id);
    if(it != m_couriers.end())
        return *it;
    return {};
}

void OnlineCouriersModel::removeCourierById(const QString &courier_id)
{
    auto it = findCourierById(courier_id);
    if(it == m_couriers.end()) return;
    int row = static_cast<int>(std::distance(m_couriers.begin(), it));
    beginRemoveRows(QModelIndex(), row, row);
    m_couriers.erase(it);
    endRemoveRows();
}

QHash<QString, QString>  OnlineCouriersModel::updateLocationUsingId(const QString &courier_id,
                                          const QString &latitude,
                                          const QString &longitude,
                                          const QString &accuracy,
                                          const QString &speed)
{
    auto it = findCourierById(courier_id);

    if(it == m_couriers.end()) return QHash<QString, QString>();
    it->insert("latitude"  , latitude);
    it->insert("longitude" , longitude);
    it->insert("accuracy"  , accuracy);
    it->insert("speed"     , speed);
    int row = static_cast<int>(std::distance(m_couriers.begin(), it));
    emit dataChanged(index(row), index(row), {LATITUDE, LONGITUDE, ACCURACY, SPEED});
    return *it;
}

QHash<QString, QString> OnlineCouriersModel::updateStatusUsingId(const QString &courier_id, const QString &status)
{
    auto it = findCourierById(courier_id);
    if(it == m_couriers.end()) return QHash<QString, QString>();
    if(it->value("status") != status){
        it->insert("status", status);
        int row = static_cast<int>(std::distance(m_couriers.begin(), it));
        emit dataChanged(index(row), index(row), {STATUS});
    }
    return *it;
}



QList<QHash<QString, QString>>::iterator OnlineCouriersModel::findCourierById(const QString &courier_id)
{
    return std::find_if(m_couriers.begin(), m_couriers.end(),
                        [&courier_id](const QHash<QString, QString> &courier){
        return courier.value("courier_id") == courier_id;
    });
}

/*
 * ----------------DB code----------------
 */

#warning The code bellow needs MAHHOSIVE tests

void OnlineCouriersModel::initMongoDbCollection(MongoDbPool *pool)
{
    m_mongoDbCollection = new MongoDBCollection("logins", pool, this);
    connect(m_mongoDbCollection, &MongoDBCollection::findFinished,
            this, &OnlineCouriersModel::onFindFinished);
    connect(m_mongoDbCollection, &MongoDBCollection::error,
            this, &OnlineCouriersModel::onDbError);
    connect(m_mongoDbCollection, &MongoDBCollection::changeDetected,
            this, &OnlineCouriersModel::onDbChangeDetected);

    QJsonObject filter;
    filter.insert("connection_state", "online");

    QJsonObject projection;
    projection.insert("username", 1);
    projection.insert("connection_state", 1);
    m_mongoDbCollection->find(filter, projection);
}

void OnlineCouriersModel::onFindFinished(const QVector<QJsonObject> &joVec)
{
    qDebug() << "Got results : " << joVec.size();
    if(!m_couriers.empty()){
        beginRemoveRows(QModelIndex(), 0, m_couriers.size() - 1);
        m_couriers.clear();
        endMoveRows();
    }

    beginInsertRows(QModelIndex(), 0, int(joVec.size()) - 1);
    for(const auto &jo : joVec){
        QHash<QString, QString> courier;
        courier.insert("courier_id", MongoDBCollection::oidToString(jo.value("_id").toObject()));
        courier.insert("username"  , jo.value("username").toString());
        courier.insert("status"    , jo.value("connection_state").toString());
        m_couriers.append(courier);
    }
    endInsertRows();
}

void OnlineCouriersModel::onDbChangeDetected(const QString &op, const QString &key, const QJsonObject &doc)
{
    if(op == "delete"){
        removeCourierById(key);
    }else if(op == "insert"){
        const QString courier_id = MongoDBCollection::oidToString(doc.value("_id").toObject());
        const QString username = doc.value("username").toString();
        const QString status = doc.value("connection_state").toString();
        if(status == "online"){
            addCourier(courier_id, username, status, "", "", "", "");
        }

    }else if(op == "update" || op == "replace"){
        const QString courier_id = MongoDBCollection::oidToString(doc.value("_id").toObject());
        const QString username = doc.value("username").toString();
        const QString status = doc.value("connection_state").toString();

        auto it = findCourierById(key);
        if(it == m_couriers.end()) {
            if(status == "online"){
                addCourier(courier_id, username, status, "", "", "", "");
            }
        }else{
            if(status == "offline"){
                removeCourierById(courier_id);

            }else if(it->value("username") != username){
                it->insert("username", username);
                int row = static_cast<int>(std::distance(m_couriers.begin(), it));
                emit dataChanged(index(row), index(row), {USERNAME});
            }
        }
    }
}

void OnlineCouriersModel::onDbError(const QString &op, const std::error_code ec)
{
    //TODO: error
}

