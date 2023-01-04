#ifndef COURIERSMONITOR_H
#define COURIERSMONITOR_H

#include <QObject>
#include <QVariantMap>
#include <QAbstractItemModel>
#include <QIdentityProxyModel>
#include <QSortFilterProxyModel>


class MongoDbPool;
class MongoDBCollection;

class WSClient;
class CouriersModel;
class OnlineCouriersModel;
class GotosModel;
class ClientsLocationsModel;
class CouriersModelManager;
class MapViewedGotosProxyModel;
class FilteredAndSortedGotosModel;
class CourierToGotoMapRoute;
class MapViewedCouriersProxyModel;
class FiltredOnlineCouriersModel;

class CouriersMonitor : public QObject
{
    Q_OBJECT
public:
    explicit CouriersMonitor(QObject *parent = nullptr);
    void setAllCouriersModel(CouriersModel &allCouriersModel);
    void setOnlineCouriersModel(CouriersModel &onlineCouriersModel);
    void setGotosModel(GotosModel &gotosModel);

//    Q_INVOKABLE void addGoto(const QString &courier_id, const QString &status,
//                             const QString &latitude, const QString &longitude,
//                             const QString &phone, const QString &start_time,
//                             const QString &from, const QString &address,
//                             const QString &description);


    void setClientsLocationsModel(ClientsLocationsModel &clm);

    CouriersModel *allCouriersModel();
//    CouriersModel *onlineCouriersModel();
    FiltredOnlineCouriersModel *onlineCouriersModel();
    MapViewedCouriersProxyModel *mapViewedCouriersProxyModel();
    QIdentityProxyModel *gotosModel(){return m_gotosModel;}
    FilteredAndSortedGotosModel *filteredGotosModel();
    ClientsLocationsModel *clientsLocationsModel();
    MapViewedGotosProxyModel *mapViewedGotosProxyModel();
    CourierToGotoMapRoute *mapRoute(){return m_mapRoute;}
public: //Qml Invokables
//    Q_INVOKABLE void getGotoModelByCourierId(const QString &id);
    Q_INVOKABLE void filterGotoModelByCourierId(const QString &id);
    Q_INVOKABLE void addGoto(const QString &courierId, const QVariantMap &varMap);
    Q_INVOKABLE void deleteGoto(const QString &courierId, const QString &gotoId);
    Q_INVOKABLE void updateGoto(const QString &courierId, const QString &gotoId,
                                const QVariantMap &varMap);
    Q_INVOKABLE void updateGotoStatus(const QString &courierId, const QString &gotoId, const QString &status);
    Q_INVOKABLE QVariantMap getCourierData(const QString &courierId);
    Q_INVOKABLE QVariantMap getGotoData(const QString &courierId, const QString &gotoId);

private: //Member functions
    void connectWebSocket();

signals:
    void courierAdded(const QVariantMap &courier);
    void courierRemoved(const QVariantMap &courier);
    void courierPositionChanged(const QVariantMap &courier);
    void courierStatusChanged(const QVariantMap &courier);

private slots:
    void onJsonObjectReceived(const QJsonObject &jsonObject);

private:
    WSClient *m_wsClient;
    CouriersModelManager * m_couriersModelManager = nullptr;

    QIdentityProxyModel *m_gotosModel = nullptr;

    MongoDbPool *m_pool = nullptr;
    MongoDBCollection *m_gotosCollection = nullptr;
    MongoDBCollection *m_clientsLocationsCollection = nullptr;

    CourierToGotoMapRoute *m_mapRoute = nullptr;

};

#endif // COURIERSMONITOR_H
