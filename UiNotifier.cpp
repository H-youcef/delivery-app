#include "UiNotifier.h"
#include <QTimer>
#include <QTime>
#include <QDebug>
UiNotifier::UiNotifier()
{
    connect(this, &UiNotifier::newNotification, this, &UiNotifier::doAddNotification);

//    QTimer *t1 = new QTimer(this);
//    t1->setSingleShot(true);
//    t1->setInterval(1000);
//    connect(t1, &QTimer::timeout, [this]{
//        emit notificationInserted("warning", "tttiiitle1111", "meesssagge111", 5000);
//    });

//    QTimer *t = new QTimer(this);
//    t->setSingleShot(true);
//    t->setInterval(4000);
//    connect(t, &QTimer::timeout, [t1, this]{
//        emit notificationInserted("info", "tttiiitle", "meesssagge", 5000);
//        t1->start();
//    });
//    t->start();

//    for(int i = 0; i < 20; ++i){
//        Notification notification;
//        notification.time = QTime::currentTime().toString("HH:mm:ss");
//        notification.severity = "info";
//        notification.title = "title";
//        notification.message = "message";
//        notification.duration = defaultDuration;
//        m_notifications.append(notification);
//    }

}

QHash<int, QByteArray> UiNotifier::roleNames() const
{
    static QHash<int, QByteArray> roleNames{
        {TIME, "time"},
        {SEVERITY, "severity"},
        {TITLE   , "title"   },
        {MESSAGE , "message" },
        {DURATION, "duration"},

    };
    return roleNames;
}

int UiNotifier::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_notifications.length();
}

QVariant UiNotifier::data(const QModelIndex &index, int role) const
{
    if(index.row() >= 0 && index.row() < rowCount()){
        switch (role) {
        case TIME: return m_notifications.at(index.row()).time;
        case SEVERITY: return m_notifications.at(index.row()).severity;
        case TITLE: return m_notifications.at(index.row()).title;
        case MESSAGE: return m_notifications.at(index.row()).message;
        case DURATION: return m_notifications.at(index.row()).duration;
        default: return QVariant();
        }
    }
    return QVariant();
}

void UiNotifier::addNotification(const QString &severity, const QString &title, const QString &message, int duration)
{
    QMetaObject::invokeMethod(this, [severity, title, message, duration, this]{
        doAddNotification(severity, title, message, duration);
    });

}

void UiNotifier::addInfoNotification(const QString &title, const QString &message, int duration)
{
    addNotification("info", title, message, duration);
}

void UiNotifier::addWarningNotification(const QString &title, const QString &message, int duration)
{
    addNotification("warning", title, message, duration);
}

void UiNotifier::addErrorNotification(const QString &title, const QString &message, int duration)
{
    addNotification("error", title, message, duration);
}

UiNotifier &UiNotifier::get()
{
    static UiNotifier uiNotifer;
    return uiNotifer;
}

void UiNotifier::doAddNotification(const QString &severity, const QString &title, const QString &message, int duration)
{
    Notification notification;
    notification.time = QTime::currentTime().toString("HH:mm:ss");
    notification.severity = severity;
    notification.title = title;
    notification.message = message;
    notification.duration = duration;

    if(m_notifications.isEmpty()){
        beginInsertRows(QModelIndex(), 0, 0);
        m_notifications.prepend(notification);
        endInsertRows();
        emit notificationInserted(severity, title, message, duration);
    }else{
        const auto lastNotification = m_notifications.first(); // first because we are prepending
        if(lastNotification != notification){
            beginInsertRows(QModelIndex(), 0, 0);
            m_notifications.prepend(notification);
            endInsertRows();
            emit notificationInserted(severity, title, message, duration);
        }
    }
    clamp();
}

void UiNotifier::clamp()
{
    if(m_notifications.size() > 100){
        beginRemoveRows(QModelIndex(), rowCount() - 1 , rowCount() - 1);
        m_notifications.removeLast();
        endRemoveRows();
    }
}
