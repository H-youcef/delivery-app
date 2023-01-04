#ifndef COURIERSMODELMANAGER_H
#define COURIERSMODELMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QSortFilterProxyModel>

class MongoDbPool;
class MongoDBCollection;
class CouriersModel;
class OnlineCouriersModel;
class GotosModel;
class MapViewedGotosProxyModel;
class FilteredAndSortedGotosModel;
class MapViewedCouriersProxyModel;
class ClientsLocationsModel;
class FiltredOnlineCouriersModel;

class CouriersModelManager: public QObject
{
    Q_OBJECT

public:
    CouriersModelManager(MongoDbPool *pool, QObject *parent = nullptr);
    CouriersModel *allCouriersModel(){return m_allCouriersModel;}
//    CouriersModel *onlineCouriersModel(){return m_onlineCouriersModel;}
    FiltredOnlineCouriersModel *onlineCouriersModel(){return m_filtredOnlineCouriersModel;}
    MapViewedCouriersProxyModel *mapViewedCouriersProxyModel(){return m_mapViewedCouriersProxyModel;}
    GotosModel *allGotosModel(){return m_allGotosModel;}
    ClientsLocationsModel *clientsLocationsModel();
    FilteredAndSortedGotosModel *filteredGotosModel(){return m_filteredGotosModel;}
    MapViewedGotosProxyModel *mapViewedGotosProxyModel(){return m_mapViewedGotosProxyModel;}

    void updateLocationUsingId(const QString &courier_id, const QString &latitude,
                               const QString &longitude, const QString &accuracy,
                               const QString &speed);
    void filterGotoModelByCourierId(const QString &id);
//    GotosModel *getGotoModelByCourierId(const QString &id);
    QVariantMap getCourierData(const QString &courierId)const;
    QVariantMap getGotoData(const QString &courierId, const QString &gotoId)const;
private slots:
    void onGotosInsertFinished(const QVector<QString> &resultIds);
    void onFindFinished(const QVector<QJsonObject> &joVec);
    void onDbChangeDetected(const QString &op, const QString &key, const QJsonObject &doc);
    void onDbError(const QString &op, const QString &message, const int errCode);


    void onGotosUpdateOneFinished(int count, const QJsonObject &filter);
    void onGotosFindFinished(const QVector<QJsonObject> &joVec);
    void onGotosDbChangeDetected(const QString &op, const QString &key, const QJsonObject &doc);
    void onGotosDbError(const QString &op, const QString &message, const int errCode);

private: //Funtions
    void queryGotosDbCollection();
private:
    CouriersModel *m_allCouriersModel = nullptr;
//    CouriersModel *m_onlineCouriersModel = nullptr;
    FiltredOnlineCouriersModel *m_filtredOnlineCouriersModel = nullptr;
    MapViewedCouriersProxyModel *m_mapViewedCouriersProxyModel = nullptr;

    MongoDBCollection *m_couriersDbCollection = nullptr;

    MongoDBCollection *m_gotosDbCollection = nullptr;

    GotosModel *m_allGotosModel = nullptr;
    FilteredAndSortedGotosModel *m_filteredGotosModel = nullptr;
    MapViewedGotosProxyModel *m_mapViewedGotosProxyModel=nullptr;
};

#endif // COURIERSMODELMANAGER_H
