#ifndef FILTREDONLINECOURIERSMODEL_H
#define FILTREDONLINECOURIERSMODEL_H

#include <QSortFilterProxyModel>

class FiltredOnlineCouriersModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FiltredOnlineCouriersModel(QObject *parent=nullptr);
    void setCurrentId(const QString &currentId);
protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
private:
    QString m_currentId;
};

#endif // FILTREDONLINECOURIERSMODEL_H
