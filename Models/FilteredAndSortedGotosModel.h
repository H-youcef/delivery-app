#ifndef FILTEREDANDSORTEDGOTOSMODEL_H
#define FILTEREDANDSORTEDGOTOSMODEL_H

#include <QSortFilterProxyModel>

class FilteredAndSortedGotosModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilteredAndSortedGotosModel(QObject *parent=nullptr);
public slots:
    void doSort();
private slots:
    void onSourceModelChanged();
protected:
    virtual bool lessThan(const QModelIndex &source_left,
                          const QModelIndex &source_right) const override;

};

#endif // FILTEREDANDSORTEDGOTOSMODEL_H
