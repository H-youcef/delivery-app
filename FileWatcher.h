#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QQmlApplicationEngine>

class FileWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FileWatcher(QObject *parent = nullptr);
    void setPath(const QString &path);
    void setNameFilters(const QStringList &filters){m_nameFilters = filters;}
    void setEngine(QQmlApplicationEngine &e){
        engine = &e;
    }

    Q_INVOKABLE void clearCache();
signals:
    void fileChanged();

private slots:
    void onFileChanged(const QString &filePath);


private:
    QFileSystemWatcher *watcher{nullptr};
    QStringList m_nameFilters;
    QStringList filesToWatch;
    bool fileIsChaned{false};
    QTimer *timer{nullptr};
    QQmlApplicationEngine *engine;
};

#endif // FILEWATCHER_H
