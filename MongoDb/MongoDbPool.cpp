#include "MongoDbPool.h"

#include <QtConcurrent/QtConcurrent>
mongocxx::instance MongoDbPool::inst {};

MongoDbPool::MongoDbPool(QObject *parent) : QObject(parent)
{
    m_scheme = "mongodb+srv";
    m_clusterLocation = "cluster-ishbilia.oskgm.mongodb.net";
    m_username = "admin";
    m_password = "admin147";
    m_dbname = "ishbilia";
    m_connOptions.push_back("retryWrites=true");
    m_connOptions.push_back("w=majority");
}

MongoDbPool::~MongoDbPool()
{
    if(m_pool) delete m_pool;
}

void MongoDbPool::setScheme(const QString &shceme)
{
    m_scheme = shceme;
}

void MongoDbPool::setUsername(const QString &username)
{
    m_username = username;
}

void MongoDbPool::setPassword(const QString &password)
{
    m_password = password;
}

void MongoDbPool::setDbName(const QString &dbname)
{
    m_dbname = dbname;
}

void MongoDbPool::setConnectionOptions(const std::vector<QString> &options)
{
    m_connOptions = options;
}

void MongoDbPool::setClusterLocation(const QString &clusterLocation)
{
    m_clusterLocation = clusterLocation;
}

void MongoDbPool::init(const QString &uri)
{
    QtConcurrent::run([this, uri]{
        m_pool = new mongocxx::pool(mongocxx::uri(uri.toStdString()));
        m_isReady = true;
        emit ready();
    });

}

void MongoDbPool::init()
{
    init(getUri());
}

mongocxx::pool::entry MongoDbPool::acquireClient()
{
    return m_pool->acquire();
}


//mongocxx::collection MongoDbConnection::getCollection(const QString &dbName, const QString &collection)
//{
//    return (*m_client)[dbName.toStdString()][collection.toStdString()];
//}



QString MongoDbPool::getUri() const
{

    QString uri = QString("%1://%2:%3@%4/%5")
            .arg(m_scheme)
            .arg(m_username)
            .arg(m_password)
            .arg(m_clusterLocation)
            .arg(m_dbname);

    if(!m_connOptions.empty()){
        uri.append("?");
        for(size_t i = 0; i < m_connOptions.size(); ++i){
            uri.append(m_connOptions[i]);
            if(i != m_connOptions.size() - 1)
                uri.append("&");
        }
    }

    return uri;
}
