#include "CourierToGotoMapRoute.h"
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "Models/CouriersModel.h"
#include "Models/GotosModel.h"

CourierToGotoMapRoute::CourierToGotoMapRoute(QObject *parent) : QObject(parent)
{
//    auto timer = new QTimer(this);
//    timer->setSingleShot(false);
//    connect(timer, &QTimer::timeout, this, &MapRoute::startChanged);
//    timer->start(2000);
}

void CourierToGotoMapRoute::setModels(CouriersModel *couriersModel,
                                      GotosModel *gotosModel)
{
    m_couriersModel = couriersModel;
    m_gotosModel = gotosModel;

    connect(couriersModel, &QAbstractItemModel::dataChanged,
            this, &CourierToGotoMapRoute::onCouriersModelDatatChanged);
    connect(gotosModel, &QAbstractItemModel::dataChanged,
            this, &CourierToGotoMapRoute::onGotosModelDatatChanged);

//    connect(&startModel, &QAbstractItemModel::rowsRemoved,
//            this, &MapRoute::onStartModelRowRemoved);
//    connect(&finishModel, &QAbstractItemModel::rowsRemoved,
//            this, &MapRoute::onFinishModelRowsRemoved);
}

void CourierToGotoMapRoute::setGotoId(const QString &gotoId)
{
    if(gotoId.isEmpty()){
        m_gotoId.clear();
        m_courierId.clear();
        m_routeEnds.clear();
        emit routeEndsChanged();
        return;
    }

    m_gotoId = gotoId;
    m_courierId = m_gotosModel->getGotoById(m_gotoId).value("courier_id").toString();
    m_routeEnds.clear();
    emit routeEndsChanged();
    if(!m_couriersModel || !m_gotosModel) return;

    QtConcurrent::run([this]{
        const auto courier = m_couriersModel->getCourierById(m_courierId);
        if(courier.isEmpty()) return;

        const auto goto_ = m_gotosModel->getGotoById(m_gotoId);
        if(goto_.isEmpty()) return;

        QVariantMap endsMap;
        endsMap.insert("courier_latitude", courier.value("latitude"));
        endsMap.insert("courier_longitude", courier.value("longitude"));
        endsMap.insert("goto_latitude", goto_.value("latitude"));
        endsMap.insert("goto_longitude", goto_.value("longitude"));
        if(endsMap != m_routeEnds){
            m_routeEnds = endsMap;
            emit routeEndsChanged();
        }
    });
}

void CourierToGotoMapRoute::onCouriersModelDatatChanged(const QModelIndex &topLeft,
                                        const QModelIndex &bottomRight,
                                        const QVector<int> &roles)
{
    if(m_gotoId.isEmpty() || m_courierId.isEmpty()) return;
    if(topLeft != bottomRight) return;

    if(!roles.contains(CouriersModel::LATITUDE)
        || !roles.contains(CouriersModel::LONGITUDE))
        return;

    if(topLeft.data(CouriersModel::_ID).toString() != m_courierId ) return;

    QVariantMap endsMap = m_routeEnds;
    endsMap.insert("courier_latitude", topLeft.data(CouriersModel::LATITUDE));
    endsMap.insert("courier_longitude", topLeft.data(CouriersModel::LONGITUDE));


    if(endsMap != m_routeEnds){
        m_routeEnds = endsMap;
        emit routeEndsChanged();
    }

}

void CourierToGotoMapRoute::onGotosModelDatatChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if(m_gotoId.isEmpty() || m_courierId.isEmpty()) return;
    if(topLeft != bottomRight) return;

    if(!roles.contains(CouriersModel::LATITUDE)
        || !roles.contains(CouriersModel::LONGITUDE))
        return;

    if(topLeft.data(GotosModel::_ID).toString() != m_gotoId ) return;

    QVariantMap endsMap = m_routeEnds;
    endsMap.insert("goto_latitude", topLeft.data(CouriersModel::LATITUDE));
    endsMap.insert("goto_longitude", topLeft.data(CouriersModel::LONGITUDE));

    if(endsMap != m_routeEnds){
        m_routeEnds = endsMap;
        emit routeEndsChanged();
    }
}
