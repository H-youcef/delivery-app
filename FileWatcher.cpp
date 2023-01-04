#include "FileWatcher.h"
#include <QDir>
#include <QDebug>
#include <QTimer>
FileWatcher::FileWatcher(QObject *parent) : QObject(parent)
{
    watcher = new QFileSystemWatcher(this);
    m_nameFilters << "*.qml";
    connect(watcher, &QFileSystemWatcher::fileChanged,
            this, &FileWatcher::onFileChanged);
    timer = new QTimer(this);
//    timer->connect(timer, &QTimer::timeout,
//                   this, &FileWatcher::fileChanged);
    timer->connect(timer, &QTimer::timeout,[this]{
        engine->clearComponentCache();
        emit fileChanged();
    });
    timer->setSingleShot(true);
    timer->setInterval(100);
    setPath("/home/youcef/workspace/c++/Projecto/projecto");
}

void FileWatcher::setPath(const QString &path)
{
    QFileInfoList paths;
    paths << QFileInfo(path);

    {
        int index = 0;
        while(true){
            if(index == paths.size()){
                break;
            }

            auto folders = QDir(paths[index++].filePath())
                    .entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            if(folders.isEmpty()) continue;

            paths.append(folders);
        }
    }

    for(const auto &p: paths){
        auto fileInfos = QDir(p.filePath()).entryInfoList(m_nameFilters, QDir::Files);
        for(const auto &fi: fileInfos){
            filesToWatch.push_back(fi.filePath());
        }
    }

    watcher->addPaths(filesToWatch);
}

void FileWatcher::clearCache()
{
    engine->clearComponentCache();
    engine->trimComponentCache();
}

void FileWatcher::onFileChanged(const QString &filePath)
{
//    qDebug() << timer->isActive();
    Q_UNUSED(filePath)
    watcher->addPaths(filesToWatch);
    if(!timer->isActive()){
        timer->start();
    }
}


















