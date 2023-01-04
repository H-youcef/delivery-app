#ifndef WSCLIENT_H
#define WSCLIENT_H



#include <QtCore/QObject>
#include <QJsonObject>
#include <QtWebSockets/QWebSocket>
#include "Tea.h"

class QTimer;

inline bool hasKeyValue(const QJsonObject &jsonObj, const QString &key, const QString &value)
{
    return (jsonObj.value(key).toString().compare(value, Qt::CaseInsensitive) == 0);
}

class WSClient : public QObject
{
    Q_OBJECT
public:
    explicit WSClient(QObject *parent = nullptr);
    void enablePing(size_t interval=20000);

    void setApiKey(const QString &apiKey);
    void setEncryptionKey(const QString &key);
    void open(const QUrl &url);
    bool isAuthenticated()const{return m_authenticated;}
    void sendJson(const QJsonObject &jsonObject);
    QWebSocketProtocol::CloseCode closeCode() const{return m_webSocket->closeCode();}
    QString closeReason() const{return m_webSocket->closeReason();}
    QUrl url()const{return m_url;}

    QAbstractSocket::SocketError error() const{return m_webSocket->error();}
    QString errorString() const{return m_webSocket->errorString();}

    bool flush(){return m_webSocket->flush();}

    void setUsername(const QString &username);
    void setPassword(const QString &password);

    QString id()const{return m_id;}

public slots:
    void close(){m_webSocket->close();}

signals:
    void closed();
    void authenticationSucceded();
    void authenticationFailed(const QString &reason);
    void jsonObjReceived(const QJsonObject &jsonObject);

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &byteArray);
    void handleRecivedTextMessage(const QString &message);
private:
    QWebSocket *m_webSocket;
    QUrl m_url;
    QString m_apiKey;
    Tea m_tea;
    bool m_authenticated = false;
    QString m_id;
    QString m_username;
    QString m_password;

    QTimer *m_pingTimer = nullptr;
};


#endif // WSCLIENT_H
