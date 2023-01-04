#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include "WSClient.h"
//#include "../UiNotifier.h"


WSClient::WSClient(QObject *parent) :
    QObject(parent)
{
    m_webSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_webSocket, &QWebSocket::connected, this, &WSClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WSClient::closed);
    connect(m_webSocket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), [this](){
        qDebug() << "Error connecting: " << this->m_webSocket->errorString();
//        UiNotifier::get().addErrorNotification("Error connecting web-socket",
//                                               this->m_webSocket->errorString().left(200));
    });
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &WSClient::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,
            this, &WSClient::onBinaryMessageReceived);

}

void WSClient::enablePing(size_t interval)
{
    if(m_pingTimer){
        m_pingTimer->stop();
    }

    if(interval == 0) return;

    if(!m_pingTimer){
        m_pingTimer = new QTimer(this);
    }

    m_pingTimer->setInterval(static_cast<int>(interval));
    m_pingTimer->setSingleShot(false);
    connect(m_pingTimer, &QTimer::timeout,[this]{
        m_webSocket->ping();
    });
    m_pingTimer->start();
}

void WSClient::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void WSClient::setEncryptionKey(const QString &key)
{
    m_tea.setKey(key);
}

void WSClient::open(const QUrl &url)
{
    m_url = url;
    m_webSocket->open(QUrl(url));
}

void WSClient::sendJson(const QJsonObject &jsonObject)
{
    const QString message = QJsonDocument(jsonObject).toJson();

    if(m_tea.hasKey()){
        m_webSocket->sendBinaryMessage(m_tea.encrypt(message));
    }else{
        m_webSocket->sendTextMessage(message);
    }
}

void WSClient::setUsername(const QString &username)
{
    m_username = username;
}

void WSClient::setPassword(const QString &password)
{
    m_password = password;
}

//! [constructor]

//! [onConnected]
void WSClient::onConnected()
{

    qDebug() << "WebSocket connected";

    QJsonObject jsonObj;
    jsonObj.insert("type", "login");
    jsonObj.insert("api_key", m_apiKey);
    jsonObj.insert("username", m_username);
    jsonObj.insert("password", m_password);

    sendJson(jsonObj);

}
//! [onConnected]

//! [onTextMessageReceived]
void WSClient::onTextMessageReceived(const QString &message)
{
    handleRecivedTextMessage(message);
}

void WSClient::onBinaryMessageReceived(const QByteArray &byteArray)
{
    try {
        auto message = m_tea.decrypt(byteArray);
        handleRecivedTextMessage(message);
    } catch (std::invalid_argument &e) {
        qDebug() << "Error decrypting message : " << e.what();
//        UiNotifier::get().addErrorNotification("Error decrypting message", e.what());
    }
}

void WSClient::handleRecivedTextMessage(const QString &message)
{
    const auto jsonObj = QJsonDocument::fromJson(message.toUtf8()).object();
    if(jsonObj.isEmpty()){
        qDebug() << Q_FUNC_INFO << ": Non Json or bad data received";
//         UiNotifier::get().addInfoNotification("Unknown data received", "Non Json or bad data received");
        return;
    }
    qDebug() << "received: " << jsonObj;

    if(hasKeyValue(jsonObj, "type", "reply") && hasKeyValue(jsonObj, "cmd", "login")){
        if(hasKeyValue(jsonObj, "value", "success")){
            m_authenticated = true;
            m_id = jsonObj.value("id").toString();
            emit authenticationSucceded();
        }else{
            m_authenticated = false;
            emit authenticationFailed(jsonObj.value("reason").toString("The server did not suply a reason"));
        }
    }else{
        emit jsonObjReceived(jsonObj);
    }

    //    m_webSocket.close();
}


