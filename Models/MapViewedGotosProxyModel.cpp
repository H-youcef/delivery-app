#include "MapViewedGotosProxyModel.h"
#include "GotosModel.h"

MapViewedGotosProxyModel::MapViewedGotosProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

bool MapViewedGotosProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);


    const QString latStr = sourceModel()->data(index, GotosModel::LATITUDE).toString();
    if(latStr.isEmpty()) return false;

    bool convertedToDouble = false;
    latStr.toDouble(&convertedToDouble);
    if(!convertedToDouble) return false;

    const QString longtStr = sourceModel()->data(index, GotosModel::LONGITUDE).toString();
    if(longtStr.isEmpty()) return false;

    convertedToDouble = false;
    longtStr.toDouble(&convertedToDouble);
    if(!convertedToDouble) return false;

    if(sourceModel()->data(index, GotosModel::STATUS).toString() != "pending"){
        return false;
    }
    return true;
}
