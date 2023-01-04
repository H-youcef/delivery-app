#ifndef CLIENTSLOCATIONSMODEL_H
#define CLIENTSLOCATIONSMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>

//TODO: finish this
class MongoDbPool;
class MongoDBCollection;

class ClientsLocationsModel: public QAbstractListModel
{
    Q_OBJECT
    enum Roles{
        _ID = Qt::UserRole + 1,
        PHONE,
        ADDRESS,
        LATITUDE,
        LONGITUDE,
        ACCURACY,
    };
public:
    ClientsLocationsModel(MongoDbPool *pool, QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE QVariantMap getClientLocationByPhone(const QString &phone);
    void addClientLocation(const QString &phone, const QString &address,
                           const QString &latitude, const QString &longitude, const double &accuracy);
    void updateClientLocationByPhone(const QString &phone, const QString &address,
                           const QString &latitude, const QString &longitude, const double &accuracy);
private slots:
    void onClDbCollectionFindFinished(const QVector<QJsonObject> &result);
    void onClDbCollectionChanged(const QString &operation, const QString &docKey, const QJsonObject &doc);
    void onClDbCollectionError(const QString &operation, const QString &message, const int errCode);
private://Methods
    QList<QVariantMap>::iterator findCLByPhone(const QString &phone);
    QList<QVariantMap>::iterator findCLById(const QString &_id);
private:
    QList<QVariantMap> m_clientsLocations;
    MongoDBCollection *m_clMongoDbCollection = nullptr;
};

#endif // CLIENTSLOCATIONSMODEL_H
