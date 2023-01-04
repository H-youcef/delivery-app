#ifndef UINOTIFIER_H
#define UINOTIFIER_H

#include <QAbstractListModel>
#include <QHash>

#include <QTimer>

class UiNotifier: public QAbstractListModel
{
    Q_OBJECT

    static constexpr int defaultDuration = 5000; //in msec

    enum Roles{
        TIME,
        SEVERITY,
        TITLE,
        MESSAGE,
        DURATION,
    };

    struct Notification{
        QString time;
        QString severity;
        QString title;
        QString message;
        int duration = defaultDuration;
        bool operator==(const Notification &other)const{
            return (severity == other.severity
                    && title == other.title
                    && message == other.message);
        }
        bool operator!=(const Notification &other)const{return !(*this == other);}
    };

    UiNotifier();
public:
    UiNotifier(const UiNotifier &) = delete;
    UiNotifier(UiNotifier &&) = delete;
    UiNotifier& operator=(const UiNotifier &) = delete;
    UiNotifier& operator=(UiNotifier &&) = delete;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


    void addNotification(const QString &severity, const QString &title, const QString &message, int duration=defaultDuration);
    void addInfoNotification(const QString &title, const QString &message, int duration=defaultDuration);
    void addWarningNotification(const QString &title, const QString &message, int duration=defaultDuration);
    void addErrorNotification(const QString &title, const QString &message, int duration=defaultDuration);

    static UiNotifier& get();

signals:
    void notificationInserted(const QString &severity, const QString &title,
                              const QString &message, int duration);

    //For internal use only
    void newNotification(const QString &severity, const QString &title,
                         const QString &message, int duration);

private slots:
    void doAddNotification(const QString &severity, const QString &title, const QString &message, int duration=defaultDuration);
    void clamp();
private:
    QList<Notification> m_notifications;

};

#endif // UINOTIFIER_H
