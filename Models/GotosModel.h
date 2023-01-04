#ifndef GOTOSMODEL_H
#define GOTOSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QJsonObject>

class MongoDBCollection;
class MongoDbPool;
class ClientsLocationsModel;

class GotosModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles{
        _ID = Qt::UserRole+1,
        COURIER_ID,
        STATUS,
        LATITUDE,
        LONGITUDE,
        ACCURACY,
        PHONE,
        START_TIME,
        FROM,
        ADDRESS,
        DESCRIPTION,
        SEEN_BY_COURIER,
        SYNC,
    };

public:
    explicit GotosModel(MongoDBCollection *mongoDbCollection, QObject *parent = nullptr);
    explicit GotosModel(const QString &courierId, MongoDBCollection *mongoDbCollection, QObject *parent = nullptr);
    ClientsLocationsModel *clientsLocationsModel(){return m_clientsLocationsModel;}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariantMap getGotoById(const QString &goto_id);
    void addGoto(const QString &goto_id, const QString &courier_id,
                 const QString &status, const QString &latitude,
                 const QString &longitude, const QString &phone,
                 const double start_time, const QString &from,
                 const QString &address, const QString &description, bool seenByCourier);

    void addGoto(const QVariantMap &gotoHash);
    void updateGoto(const QString gotoId, const QVariantMap &gotoHash);
    void updateSync(const QString &goto_id, bool isSyncing);
    void updateIsSeenByCourier(const QString &goto_id, bool seenByCourier);
    void updateStatusById(const QString &goto_id, const QString &status);
    void updateLocationById(const QString &goto_id, const QString &latitude,
                            const QString &longitude);
    void removeGotoById(const QString &goto_id);
    void clear();
    bool contains(const QString &id);

    /**
     * @brief refetch the data from dataBase
     */
    Q_INVOKABLE void refetch();

    /**
     * @brief inserts the new goto data into the database and in the model
     * @param gotoVM the new goto data to insert.
     */
    Q_INVOKABLE void insert(const QVariantMap &gotoVM);

    void insert(const QString &courierId, const QVariantMap &gotoVM);

    /**
     * @brief deletes goto data from the database and in the model
     * @param gotoVM the new goto data to delete.
     */
    Q_INVOKABLE void remove(const QString &goto_id);

    /**
     * @brief updates goto data in the the database and in the model
     *
     */
    Q_INVOKABLE void update(const QString &gotoId, const QVariantMap &varMap);

    /**
     * @brief updates goto status in the the database and in the model
     *
     */
    Q_INVOKABLE void updateStatus(const QString &gotoId, const QString &status);


private: //Methods
    QList<QVariantMap>::iterator findGotoById(const QString &_id);
    void init();
private:
    const QString m_courierId;
    QList<QVariantMap> m_gotos;
    MongoDBCollection *m_mongoDbCollection = nullptr;

    ClientsLocationsModel *m_clientsLocationsModel = nullptr;
};

#endif // GOTOSMODEL_H
