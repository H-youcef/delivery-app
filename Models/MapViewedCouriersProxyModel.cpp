#include "MapViewedCouriersProxyModel.h"
#include "CouriersModel.h"
MapViewedCouriersProxyModel::MapViewedCouriersProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

bool MapViewedCouriersProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);


    const QString latStr = sourceModel()->data(index, CouriersModel::LATITUDE).toString();
    if(latStr.isEmpty()) return false;

    bool convertedToDouble = false;
    latStr.toDouble(&convertedToDouble);
    if(!convertedToDouble) return false;

    const QString longtStr = sourceModel()->data(index, CouriersModel::LONGITUDE).toString();
    if(longtStr.isEmpty()) return false;

    convertedToDouble = false;
    longtStr.toDouble(&convertedToDouble);
    if(!convertedToDouble) return false;

    if(sourceModel()->data(index, CouriersModel::STATUS).toString() != "online"){
        return false;
    }
    return true;
}
