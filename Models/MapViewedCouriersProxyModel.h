#ifndef MAPVIEWEDCOURIERSPROXYMODEL_H
#define MAPVIEWEDCOURIERSPROXYMODEL_H

#include <QSortFilterProxyModel>

class MapViewedCouriersProxyModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    MapViewedCouriersProxyModel(QObject *parent=nullptr);
protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // MAPVIEWEDCOURIERSPROXYMODEL_H
