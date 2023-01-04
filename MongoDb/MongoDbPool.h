#ifndef MONGODBMANGER_H
#define MONGODBMANGER_H

#include <QObject>
#include <vector>

#ifndef Q_MOC_RUN
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#endif //Q_MOC_RUN

class MongoDbPool : public QObject
{
    Q_OBJECT
public:
    explicit MongoDbPool(QObject *parent = nullptr);
    ~MongoDbPool();

    void setScheme(const QString &shceme);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setClusterLocation(const QString &clusterLocation);
    void setDbName(const QString &dbname);
    void setConnectionOptions(const std::vector<QString> &options);

    QString dbName()const{return m_dbname;}


    void init(const QString &uri);
    void init();

    mongocxx::pool::entry acquireClient();

    QString getUri()const;
    bool isReady()const{return m_isReady;}
signals:
    void ready();

private:
    static mongocxx::instance inst;
    mongocxx::pool *m_pool = nullptr;

    QString m_scheme;
    QString m_username;
    QString m_password;
    QString m_clusterLocation;
    QString m_dbname;
    std::vector<QString> m_connOptions;

    std::atomic_bool m_isReady = false;
};

#endif // MONGODBMANGER_H
