#include "FiltredOnlineCouriersModel.h"
#include "CouriersModel.h"

FiltredOnlineCouriersModel::FiltredOnlineCouriersModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

void FiltredOnlineCouriersModel::setCurrentId(const QString &currentId){
    beginResetModel();
    m_currentId = currentId;
    endResetModel();
}

bool FiltredOnlineCouriersModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    const QString status = sourceModel()->data(index, CouriersModel::STATUS).toString();
    const QString id = sourceModel()->data(index, CouriersModel::_ID).toString();

    return (m_currentId != id && status == "online");

}
