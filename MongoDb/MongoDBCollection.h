#ifndef MONGODBCOLLECTION_H
#define MONGODBCOLLECTION_H

#include <QObject>
#include <QJsonObject>
#ifndef Q_MOC_RUN
#include <bsoncxx/builder/stream/document.hpp>
#include "MongoDbPool.h"
#endif

class MongoDBCollection : public QObject
{
    Q_OBJECT
public:
    explicit MongoDBCollection(QObject *parent = nullptr);
    explicit MongoDBCollection(MongoDbPool*pool, QObject *parent = nullptr);
    explicit MongoDBCollection(const QString &collectionName, MongoDbPool*pool,
                               QObject *parent = nullptr);
    explicit MongoDBCollection(const QString &collectionName, MongoDbPool *pool,
                      const QJsonObject &findFilter, const QJsonObject &changeStreamFilter,
                      QObject *parent);
    explicit MongoDBCollection(const QString dbName, const QString &collectionName,
                               MongoDbPool*pool, const QJsonObject &findFilter={},
                               QObject *parent = nullptr);
    explicit MongoDBCollection(const QString &collectionName, MongoDbPool *pool,
                      const QJsonObject &findFilter, QObject *parent=nullptr);

    void init(MongoDbPool *pool, const QJsonObject &findFilter={}, const QJsonObject &changeStreamFilter={});
    MongoDbPool *pool(){return m_pool;}
    void setCollectionName(const QString &collectionName);
    QString collectionName()const{return m_collectionName;}
    void setDbName(const QString &dbName);
    QString dbName()const{return m_dbName;}
    QJsonObject filter()const{return m_findFilter;}
    void find(const QJsonObject &filter = QJsonObject(), const QJsonObject &projection={});
    void findOne(const QJsonObject &filter = QJsonObject(), const QJsonObject &projection={});
    void findById(const QString &id, const QJsonObject &projection={});

    void insert(const std::vector<QJsonObject> &jsonObjects);
    void insert(const QJsonObject &jsonObject);

    void updateMany(const QJsonObject &filter, const QJsonObject &op);
    void updateOne(const QJsonObject &filter, const QJsonObject &op);
    void updateById(const QString &id, const QJsonObject &op);

    void deleteMany(const QJsonObject &filter);
    void deleteOne(const QJsonObject &filter);
    void deleteById(const QString &id);

    static bsoncxx::document::value qjsonObjectToBsonDoc(const QJsonObject &jsonObject);
    static QJsonObject bsonDocToQjsonObject(const bsoncxx::document::view &bsonDoc);
    static QJsonObject createOidJsonObject(const QString &oid);
    static QJsonObject makeNewOidJsonObject();
    static QString oidToString(const QJsonObject &oidJsonObject);

private slots:
    void runPendingOperations();
    void watchForChanges();

signals:
    void findFinished(const QVector<QJsonObject> &result);
    void insertFinished(const QVector<QString> &resultIds);
    void updateFinished(int modifiedCount);
    void updateOneFinished(int modifiedCount, const QJsonObject &filter);
    void deleteFinished(int deletedCount);
    void changeDetected(const QString &operation, const QString &docKey, const QJsonObject &doc);
    void changeEventDetected(const QJsonObject &event);
    void error(const QString &operation, const QString &message, const int errCode);

private:
    MongoDbPool *m_pool = nullptr;
    QJsonObject m_findFilter;
    QJsonObject m_changeStreamFilter;
    QString m_collectionName;
    QString m_dbName;
    std::vector<std::function<void()>> m_pendingOperations;
};

#endif // MONGODBCOLLECTION_H
