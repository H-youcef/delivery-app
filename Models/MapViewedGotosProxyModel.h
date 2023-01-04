#ifndef MAPVIEWEDGOTOSPROXYMODEL_H
#define MAPVIEWEDGOTOSPROXYMODEL_H

#include <QSortFilterProxyModel>

class MapViewedGotosProxyModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    MapViewedGotosProxyModel(QObject *parent=nullptr);
protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // MAPVIEWEDGOTOSPROXYMODEL_H
