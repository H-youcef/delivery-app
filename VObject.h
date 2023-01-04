#ifndef VOBJECT_H
#define VOBJECT_H

#include <QObject>
#include <QVariant>

class VObject : public QObject
{
    Q_OBJECT
public:
    VObject(QObject *parent=nullptr) :QObject(parent){}
    VObject(const QVariant &value, QObject *parent=nullptr)
        :QObject(parent), m_value(value){}
    VObject& operator=(const VObject &obj){
        setValue(obj.m_value);
        return *this;
    }
    VObject& operator=(const QVariant &value){
        setValue(value);
        return *this;
    }
    void setValue(const QVariant &v){
        if(v == m_value)
            return;
        m_value = v;
        emit valueChanged();
    }
    QVariant value()const{
        return m_value;
    }
signals:
    void valueChanged();

private:
    QVariant m_value;
};

#endif // VOBJECT_H
