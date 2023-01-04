#include "MongoDBCollection.h"
#include <QtConcurrent/QtConcurrent>
#include <QDebug>
#include <system_error>
#include "../UiNotifier.h"

#ifndef Q_MOC_RUN
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#endif //Q_MOC_RUN

MongoDBCollection::MongoDBCollection(QObject *parent) : QObject(parent)
{

}

MongoDBCollection::MongoDBCollection(MongoDbPool *pool, QObject *parent)
    :QObject(parent)
{
    init(pool);
    setDbName(m_pool->dbName());
}

MongoDBCollection::MongoDBCollection(const QString &collectionName, MongoDbPool *pool, QObject *parent)
    :QObject(parent)
{
    init(pool);
    setCollectionName(collectionName);
    setDbName(m_pool->dbName());
}

MongoDBCollection::MongoDBCollection(const QString &collectionName, MongoDbPool *pool,
                                     const QJsonObject &findFilter,
                                     const QJsonObject &changeStreamFilter,
                                     QObject *parent)
    :QObject(parent)
{
    init(pool, findFilter, changeStreamFilter);
    setCollectionName(collectionName);
    setDbName(m_pool->dbName());
}

MongoDBCollection::MongoDBCollection(const QString &collectionName,
                                     MongoDbPool *pool, const QJsonObject &findFilter,
                                     QObject *parent)
    :QObject(parent)
{
    init(pool, findFilter);
    setCollectionName(collectionName);
    setDbName(m_pool->dbName());
}

MongoDBCollection::MongoDBCollection(const QString dbName, const QString &collectionName,
                                     MongoDbPool *pool, const QJsonObject &findFilter,
                                     QObject *parent)
    :QObject(parent)
{
    init(pool, findFilter);
    setCollectionName(collectionName);
    setDbName(dbName);
}

void MongoDBCollection::init(MongoDbPool *pool, const QJsonObject &findFilter,
                             const QJsonObject &changeStreamFilter)
{
    m_pool = pool;
    m_findFilter = findFilter;
    m_changeStreamFilter = changeStreamFilter;
    connect(m_pool, &MongoDbPool::ready, this, &MongoDBCollection::runPendingOperations);
    connect(m_pool, &MongoDbPool::ready, this, &MongoDBCollection::watchForChanges);
}

void MongoDBCollection::setCollectionName(const QString &collectionName)
{
    m_collectionName = collectionName;
}

void MongoDBCollection::setDbName(const QString &dbName)
{
    m_dbName = dbName;
}

void MongoDBCollection::runPendingOperations()
{
    for(auto&& operation : m_pendingOperations){
        QtConcurrent::run(std::move(operation));
    }
}

void MongoDBCollection::watchForChanges()
{
    QtConcurrent::run([this]{
        //TODO: if there is a resume token resume from there
        auto client = m_pool->acquireClient();
        auto collection = client->database(m_dbName.toStdString())
                .collection(m_collectionName.toStdString());
        auto options = mongocxx::options::change_stream();
        options.max_await_time(std::chrono::milliseconds(4000))
                .full_document("updateLookup");

        mongocxx::pipeline pipeline;

        QJsonObject pipeFilter;
        if(!m_changeStreamFilter.isEmpty()){
            for(auto cit = m_changeStreamFilter.constBegin(); cit != m_changeStreamFilter.constEnd(); ++cit){
                const QString newKey = "fullDocument." + cit.key();
                pipeFilter.insert(newKey, cit.value());
            }
        }
        pipeline.match(qjsonObjectToBsonDoc(pipeFilter));

        auto watchStream = collection.watch(pipeline, options);

        while(true){
            try {
                for(auto it = watchStream.begin(); it != watchStream.end(); ++it){
                    auto eventJsonObject = bsonDocToQjsonObject(*it);

                    const QString operation = eventJsonObject.value("operationType").toString();
                    const QString key = oidToString( eventJsonObject.value("documentKey")
                            .toObject().value("_id").toObject());
                    const QJsonObject doc = eventJsonObject.value("fullDocument").toObject();
                    emit changeDetected(operation, key, doc);
                    emit changeEventDetected(eventJsonObject);
                }
            } catch (const std::system_error &e) {
                qDebug() << "watchForChanges exception: "
                         << e.code().value() << QString(e.code().message().c_str());
                UiNotifier::get().addErrorNotification("MongoDbCollection: watchForChanges exception",
                                                       QString(e.code().message().c_str()));
            }catch ( const std::exception& e )
            {
                qDebug() << "watchForChanges exception: " << QString(e.what());
                UiNotifier::get().addErrorNotification("MongoDbCollection: watchForChanges exception",
                                                       QString(e.what()));
            }
        }
    });
}

void MongoDBCollection::find(const QJsonObject &filter, const QJsonObject &projection)
{
    QJsonObject queryFilter = filter.isEmpty() ? m_findFilter : filter;
    auto operation = [this, queryFilter, projection](){
        try
        {
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());

            mongocxx::options::find findOptions;
            findOptions.limit(10000);
            if(!projection.isEmpty()){
                findOptions.projection(qjsonObjectToBsonDoc(projection));
            }

            mongocxx::cursor cursor = collection.find(qjsonObjectToBsonDoc(queryFilter), findOptions);
            QVector<QJsonObject> result;
            for(auto doc : cursor){
                result.push_back(bsonDocToQjsonObject(doc));
            }
            emit findFinished(result);

        }catch(const std::system_error &e){
            //TODO: try using the error type supplied by mongo
            emit error("find", QString(e.code().message().c_str()), e.code().value());
        }

    };

    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::findOne(const QJsonObject &filter, const QJsonObject &projection)
{
    QJsonObject queryFilter = filter.isEmpty() ? m_findFilter : filter;
    auto operation = [this, queryFilter, projection](){
        try
        {
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());
            mongocxx::options::find findOptions;
            if(!projection.isEmpty()){
                findOptions.projection(qjsonObjectToBsonDoc(projection));
            }
            bsoncxx::stdx::optional<bsoncxx::document::value> resultDoc =
                    collection.find_one(qjsonObjectToBsonDoc(queryFilter), findOptions);

            if(resultDoc){
                emit findFinished({bsonDocToQjsonObject(*resultDoc)});
            }else{
                emit findFinished({});
            }

        }catch(const std::system_error &e){
            emit error("findOne", QString(e.code().message().c_str()), e.code().value());
        }

    };

    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }

}

void MongoDBCollection::findById(const QString &id, const QJsonObject &projection)
{
    auto oid = createOidJsonObject(id);
    QJsonObject filter;
    filter.insert("_id", oid);
    findOne(filter, projection);
}

void MongoDBCollection::insert(const std::vector<QJsonObject> &jsonObjects)
{
    auto operation = [this, jsonObjects]{
        try{
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());

            std::vector<bsoncxx::document::value> documents;
            for(const auto &jsonObject : jsonObjects){
                documents.push_back(qjsonObjectToBsonDoc(jsonObject));
            }
            auto result = collection.insert_many(documents);
            if(result){
                QVector<QString> ids;
                for(auto &id : result->inserted_ids()){
                    const std::string idStr = id.second.get_oid().value.to_string();
                    ids.push_back(QString(idStr.c_str()));
                }
                emit insertFinished(ids);
            }else{
                emit insertFinished({});
            }
        }catch(const std::system_error &e){
            emit error("insert", QString(e.code().message().c_str()), e.code().value());
        }
    };
    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::insert(const QJsonObject &jsonObject)
{
    auto operation = [this, jsonObject]{
        try{
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());


            auto result = collection.insert_one(qjsonObjectToBsonDoc(jsonObject));
            if(result){
                const std::string idStr = result->inserted_id().get_oid().value.to_string();
                emit insertFinished({QString(idStr.c_str())});
            }else{
                emit insertFinished({});
            }
        }catch(const std::system_error &e){
            emit error("insert", QString(e.code().message().c_str()), e.code().value());
        }
    };
    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::updateMany(const QJsonObject &filter, const QJsonObject &op)
{
    auto operation = [this, filter, op]{
        try{
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());

            auto result = collection.update_many(qjsonObjectToBsonDoc(filter),
                                                qjsonObjectToBsonDoc(op));
            if(result){
                emit updateFinished(result->modified_count());
            }else{
                emit updateFinished(0);
            }
        }catch(const std::system_error &e){
            emit error("updateMany", QString(e.code().message().c_str()), e.code().value());
        }
    };
    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::updateOne(const QJsonObject &filter, const QJsonObject &op)
{
    auto operation = [this, filter, op]{
        try{
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());

            auto result = collection.update_one(qjsonObjectToBsonDoc(filter),
                                                qjsonObjectToBsonDoc(op));
            if(result){
                emit updateOneFinished(result->modified_count(), filter);
            }else{
                emit updateOneFinished(0, {});
            }
        }catch(const std::system_error &e){
            emit updateOneFinished(0, {});
            emit error("updateOne", QString(e.code().message().c_str()), e.code().value());
        }
    };
    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::updateById(const QString &id, const QJsonObject &op)
{
    auto oid = createOidJsonObject(id);
    QJsonObject filter;
    filter.insert("_id", oid);
    updateOne(filter, op);
}

void MongoDBCollection::deleteMany(const QJsonObject &filter)
{
    auto operation = [this, filter]{
        try{
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());

            auto result = collection.delete_many(qjsonObjectToBsonDoc(filter));
            if(result){
                emit deleteFinished(result->deleted_count());
            }else{
                emit deleteFinished(0);
            }
        }catch(const std::system_error &e){
            emit error("deleteMany", QString(e.code().message().c_str()), e.code().value());
        }
    };
    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::deleteOne(const QJsonObject &filter)
{

    auto operation = [this, filter]{
        try{
            auto client = m_pool->acquireClient();
            auto collection = client->database(m_dbName.toStdString())
                    .collection(m_collectionName.toStdString());

            auto result = collection.delete_one(qjsonObjectToBsonDoc(filter));
            if(result){
                emit deleteFinished(result->deleted_count());
            }else{
                emit deleteFinished(0);
            }
        }catch(const std::system_error &e){
            emit error("deleteOne", QString(e.code().message().c_str()), e.code().value());
        }
    };
    if(m_pool->isReady()){
        QtConcurrent::run(std::move(operation));
    }else{
        m_pendingOperations.push_back(std::move(operation));
    }
}

void MongoDBCollection::deleteById(const QString &id)
{
    auto oid = createOidJsonObject(id);
    QJsonObject filter;
    filter.insert("_id", oid);
    deleteOne(filter);
}

bsoncxx::document::value MongoDBCollection::qjsonObjectToBsonDoc(const QJsonObject &jsonObject)
{
    const std::string jsonStr = QString(QJsonDocument(jsonObject).toJson()).toStdString();
    return bsoncxx::from_json(jsonStr);
}

QJsonObject MongoDBCollection::bsonDocToQjsonObject(const bsoncxx::document::view &bsonDoc)
{
    const std::string jsonStr = bsoncxx::to_json(bsonDoc);
    return QJsonDocument::fromJson(jsonStr.c_str()).object();
}

QJsonObject MongoDBCollection::createOidJsonObject(const QString &oidStr)
{
    QJsonObject oid;
    oid.insert("$oid", oidStr);
    return oid;
}

QJsonObject MongoDBCollection::makeNewOidJsonObject()
{
    return createOidJsonObject(bsoncxx::oid().to_string().c_str());
}

QString MongoDBCollection::oidToString(const QJsonObject &oidJsonObject)
{
    return oidJsonObject.value("$oid").toString();
}




