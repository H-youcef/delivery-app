#include "FilteredAndSortedGotosModel.h"
#include "GotosModel.h"

FilteredAndSortedGotosModel::FilteredAndSortedGotosModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    connect(this, &FilteredAndSortedGotosModel::sourceModelChanged,
            this, &FilteredAndSortedGotosModel::onSourceModelChanged);
}

void FilteredAndSortedGotosModel::doSort()
{
    sort(0);
}

void FilteredAndSortedGotosModel::onSourceModelChanged()
{
    connect(sourceModel(), &QAbstractItemModel::dataChanged,
            this, &FilteredAndSortedGotosModel::doSort);
    connect(sourceModel(), &QAbstractItemModel::modelReset,
            this, &FilteredAndSortedGotosModel::doSort);
    connect(sourceModel(), &QAbstractItemModel::rowsMoved,
            this, &FilteredAndSortedGotosModel::doSort);
    connect(sourceModel(), &QAbstractItemModel::rowsInserted,
            this, &FilteredAndSortedGotosModel::doSort);
    connect(sourceModel(), &QAbstractItemModel::rowsRemoved,
            this, &FilteredAndSortedGotosModel::doSort);

    doSort();
}

bool FilteredAndSortedGotosModel::lessThan(const QModelIndex &left,
                                           const QModelIndex &right) const
{
    QString leftData = sourceModel()->data(left, GotosModel::STATUS).toString();
    QString rightData = sourceModel()->data(right, GotosModel::STATUS).toString();

    if(leftData == "pending" && (rightData == "canceled" || rightData == "done"))
        return true;
    if((leftData == "done" && rightData == "canceled"))
        return true;
    if(leftData == "canceled" && (rightData == "pending" || rightData == "done"))
        return false;
    if(leftData == "done" && rightData == "pending")
        return false;

    leftData = sourceModel()->data(left, GotosModel::START_TIME).toString();
    rightData = sourceModel()->data(right, GotosModel::START_TIME).toString();

    return leftData.toDouble() > rightData.toDouble();
}
