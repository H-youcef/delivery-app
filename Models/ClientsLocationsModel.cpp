#include "ClientsLocationsModel.h"
#include "../MongoDb/MongoDBCollection.h"
#include "../UiNotifier.h"
#include <QDebug>


//TODO: needs test

ClientsLocationsModel::ClientsLocationsModel(MongoDbPool *pool,
                                             QObject *parent)
    :QAbstractListModel(parent)
{
    m_clMongoDbCollection = new MongoDBCollection("clients_locations", pool, this);
    connect(m_clMongoDbCollection, &MongoDBCollection::findFinished,
            this, &ClientsLocationsModel::onClDbCollectionFindFinished);
    connect(m_clMongoDbCollection, &MongoDBCollection::changeDetected,
            this, &ClientsLocationsModel::onClDbCollectionChanged);
    connect(m_clMongoDbCollection, &MongoDBCollection::error,
            this, &ClientsLocationsModel::onClDbCollectionError);
    //Fetches all docs
    m_clMongoDbCollection->find();
}

QVariant ClientsLocationsModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= 0 && index.row() < rowCount()){

        switch (role) {
        case _ID:
            return m_clientsLocations[index.row()].value("_id");
        case PHONE:
            return m_clientsLocations[index.row()].value("phone");
        case ADDRESS:
            return m_clientsLocations[index.row()].value("address");
        case LATITUDE:
            return m_clientsLocations[index.row()].value("latitude");
        case LONGITUDE:
            return m_clientsLocations[index.row()].value("longitude");
        case ACCURACY:
            return m_clientsLocations[index.row()].value("accuracy");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QHash<int, QByteArray> ClientsLocationsModel::roleNames() const
{
    static QHash<int, QByteArray> roleNames = {
        {_ID   , "_id"},
        {PHONE , "phone"},
        {ADDRESS   , "address"  },
        {LATITUDE   , "latitude"  },
        {LONGITUDE  , "longitude" },
        {ACCURACY  , "accuracy" },
    };
    return roleNames;

}

int ClientsLocationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_clientsLocations.size();
}

QVariantMap ClientsLocationsModel::getClientLocationByPhone(const QString &phone)
{
    const auto it = findCLByPhone(phone);
    if(it != m_clientsLocations.end())
        return *it;
    return {};
}

void ClientsLocationsModel::addClientLocation(const QString &phone, const QString &address,
                                              const QString &latitude, const QString &longitude,
                                              const double &accuracy)
{
    if(findCLByPhone(phone) != m_clientsLocations.end()){
        updateClientLocationByPhone(phone, address, latitude, longitude, accuracy);
        return;
    }
    auto oid = MongoDBCollection::makeNewOidJsonObject();

    QVariantMap cl;
    cl.insert("phone", phone);
    cl.insert("address", address);
    cl.insert("latitude", latitude);
    cl.insert("longitude", longitude);
    cl.insert("accuracy", accuracy);

    QJsonObject jo = QJsonObject::fromVariantMap(cl);
    jo.insert("_id", oid);
    m_clMongoDbCollection->insert(jo);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    cl.insert("_id", MongoDBCollection::oidToString(oid));
    m_clientsLocations.append(cl);
    endInsertRows();
}

void ClientsLocationsModel::updateClientLocationByPhone(const QString &phone, const QString &address,
                                                        const QString &latitude, const QString &longitude,
                                                        const double &accuracy)
{
    auto it = findCLByPhone(phone);
    if( it == m_clientsLocations.end())
        return;
    if(it->value("address").toString() == address
       && it->value("latitude").toString() == latitude
        && it->value("longitude").toString() == longitude
        && int(it->value("accuracy").toDouble()) == int(accuracy))
        return;
    it->insert("address", address);
    it->insert("latitude", latitude);
    it->insert("longitude", longitude);
    it->insert("accuracy", accuracy);

    int row = static_cast<int>(std::distance(m_clientsLocations.begin(), it));
    emit dataChanged(index(row), index(row), {ADDRESS, LATITUDE, LONGITUDE, ACCURACY});

    QJsonObject jo = QJsonObject::fromVariantMap(*it);
    jo.remove("_id");
    QJsonObject updateJo({{"$set", jo}});
    m_clMongoDbCollection->updateById(it->value("_id").toString(), updateJo);
}

void ClientsLocationsModel::onClDbCollectionFindFinished(const QVector<QJsonObject> &result)
{
    if(result.isEmpty()) return;
    QList<QVariantMap> vmList;
    for(const auto &jo : result){
        const QString _id = MongoDBCollection::oidToString(jo.value("_id").toObject());
        auto vm = jo.toVariantMap();
        vm.insert("_id", _id);
        vmList.append(vm);
    }
    beginInsertRows(QModelIndex(), m_clientsLocations.length(),
                    m_clientsLocations.length() + vmList.length() - 1);
    m_clientsLocations.append(vmList);
    endInsertRows();
}

void ClientsLocationsModel::onClDbCollectionChanged(const QString &operation,
                                                    const QString &docKey, const QJsonObject &doc)
{
    if(operation != "delete"){
        auto cl = doc.toVariantMap();
        auto it = findCLById(docKey);

        if(it == m_clientsLocations.end()){
            //cl does not exist so we insert a new one
            cl.insert("_id", docKey);
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_clientsLocations.append(cl);
            endInsertRows();
        }else{
            //cl does exist so we update
            it->insert("phone", cl.value("phone"));
            it->insert("address", cl.value("address"));
            it->insert("latitude", cl.value("latitude"));
            it->insert("longitude", cl.value("longitude"));
            it->insert("accuracy", cl.value("accuracy"));

            int row = static_cast<int>(std::distance(m_clientsLocations.begin(), it));
            QVector<int> roles;
            auto rolesVals = roleNames();
            for(auto rit = rolesVals.begin(); rit != rolesVals.end(); ++rit){
                roles.push_back(rit.key());
            }
            emit dataChanged(index(row), index(row), roles);
        }
    }
}

void ClientsLocationsModel::onClDbCollectionError(const QString &operation,
                                                  const QString &message,
                                                  const int errCode)
{
    qDebug() << Q_FUNC_INFO << "Error: " << operation << message;
    UiNotifier::get().addErrorNotification("Error in ClientsLocations Db-collection",
                                           QString("Operation: %1\n Message: %2\n ErrorCode: %3")
                                           .arg(operation)
                                           .arg(message)
                                           .arg(errCode));
}

QList<QVariantMap>::iterator
ClientsLocationsModel::findCLByPhone(const QString &phone)
{
    return std::find_if(m_clientsLocations.begin(), m_clientsLocations.end(),
                        [&phone](const QVariantMap &cl){
        return cl.value("phone") == phone;
    });
}

QList<QVariantMap>::iterator ClientsLocationsModel::findCLById(const QString &_id)
{
    return std::find_if(m_clientsLocations.begin(), m_clientsLocations.end(),
                        [&_id](const QVariantMap &cl){
                            return cl.value("_id") == _id;
                        });
}
