#ifndef ONLINECOURIERSMODEL_H
#define ONLINECOURIERSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QJsonObject>

class MongoDBCollection;
class MongoDbPool;

class OnlineCouriersModel: public QAbstractListModel
{
    Q_OBJECT
    enum Roles{
        COURIER_ID = Qt::UserRole+1,
        USERNAME,
        STATUS,
        LATITUDE,
        LONGITUDE,
        ACCURACY,
        SPEED,
    };

public:
    OnlineCouriersModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void addCourier(const QString &courier_id, const QString &username,
                    const QString &status, const QString &latitude,
                    const QString &longitude, const QString &accuracy,
                    const QString &speed);

    void removeCourierById(const QString &courier_id);

    QHash<QString, QString> updateLocationUsingId(const QString &courier_id, const QString &latitude,
                               const QString &longitude, const QString &accuracy,
                               const QString &speed);
    QHash<QString, QString> updateStatusUsingId(const QString &courier_id, const QString &status);
    void addCourier(const QHash<QString, QString> &courier);
    QHash<QString, QString> getCourierById(const QString &courier_id);

    /**
      * Initializes the connection to the database
      * @param[in] filter The filter used to fetch data from MongoDb (in find operation).
      */
    void initMongoDbCollection(MongoDbPool *pool);

private: //Methods
    QList<QHash<QString, QString>>::iterator findCourierById(const QString &courier_id);

private slots:
    void onFindFinished(const QVector<QJsonObject> &joVec);
    void onDbChangeDetected(const QString &op, const QString &key, const QJsonObject &doc);
    void onDbError(const QString &op, const std::error_code ec);

private:
    QList<QHash<QString, QString>> m_couriers;
    MongoDBCollection *m_mongoDbCollection = nullptr;
};

#endif // ONLINECOURIERSMODEL_H
