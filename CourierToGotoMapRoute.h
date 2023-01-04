#ifndef COURIERTOGOTOMAPROUTE_H
#define COURIERTOGOTOMAPROUTE_H

#include <QAbstractItemModel>
#include <QObject>

class CouriersModel;
class GotosModel;

class CourierToGotoMapRoute : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap routeEnds READ routeEnds NOTIFY routeEndsChanged)
public:
    explicit CourierToGotoMapRoute(QObject *parent = nullptr);
    Q_INVOKABLE void setModels(CouriersModel *courierModel,
                             GotosModel *gotosModel);

    Q_INVOKABLE void setGotoId(const QString &gotoId);
    QVariantMap routeEnds()const{return m_routeEnds;}
signals:
    void routeEndsChanged();
private slots:
    void onCouriersModelDatatChanged(const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight,
                                   const QVector<int> &roles = QVector<int>());
    void onGotosModelDatatChanged(const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight,
                                   const QVector<int> &roles = QVector<int>());
private:
    QVariantMap m_routeEnds;

    QString m_courierId;
    QString m_gotoId;

    CouriersModel *m_couriersModel = nullptr;
    GotosModel *m_gotosModel = nullptr;

};

#endif // MAPROUTE_H
