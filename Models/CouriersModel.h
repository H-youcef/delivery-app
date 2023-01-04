#ifndef COURIERSMODEL_H
#define COURIERSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QJsonObject>

class MongoDBCollection;
class MongoDbPool;
class GotosModel;
class CouriersModel: public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles{
        _ID = Qt::UserRole+1,
        USERNAME,
        STATUS,
        LATITUDE,
        LONGITUDE,
        ACCURACY,
        SPEED,
    };

public:
    CouriersModel(MongoDBCollection *gotosDbCollection, QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void addCourier(const QString &courier_id, const QString &username,
                    const QString &status, const QString &latitude,
                    const QString &longitude, const QString &accuracy,
                    const QString &speed);

    void removeCourierById(const QString &courier_id);

    QVariantMap updateLocationUsingId(const QString &courier_id, const QString &latitude,
                               const QString &longitude, const QString &accuracy,
                               const QString &speed);
    QVariantMap updateStatusUsingId(const QString &courier_id, const QString &status);
    void addCourier(const QVariantMap &courier);

    /**
      * Initializes the connection to the database
      * @param[in] filter The filter used to fetch data from MongoDb (in find operation).
      */
    void initMongoDbCollection(MongoDbPool *pool);
    void clear();
    void addMultipleCouriers(const QList<QVariantMap> &couriers);
    QVariantMap getCourierById(const QString &courier_id);
    QVariantMap updateUsernameUsingId(const QString &courier_id, const QString &username);


private: //Methods
    QList<QVariantMap>::iterator findCourierById(const QString &courier_id);

private:
    QList<QVariantMap> m_couriers;
//    QHash<QString, GotosModel*> m_gotosModels;
//    MongoDBCollection *m_gotosDbCollection = nullptr;
};

#endif // COURIERSMODEL_H
