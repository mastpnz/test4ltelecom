#include "chatclient.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QHostAddress>
#include <QDebug>
#include <QJsonArray>
#include <QVariant>
#include <QVariantMap>

ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
{
    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &ChatClient::disconnected);
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatClient::error);
}

void ChatClient::sendCommand()
{
    QDataStream clientStream(m_clientSocket);
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("command");
    message[QStringLiteral("data")] = QStringLiteral("getstat");
    clientStream << QJsonDocument(message).toJson();
}

void ChatClient::sendFile(const QByteArray &data)
{
    if (data.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("file");
    message[QStringLiteral("data")] = QJsonValue::fromVariant(QVariant(data));
    clientStream << QJsonDocument(message).toJson();
}

void ChatClient::openAndSendFile(const QString &path)
{
    if(!path.isEmpty())
    {
        QString m_path = path;
#ifdef Q_OS_WINDOWS
        m_path = m_path.remove(0, 1); //remove start \Simbol
#endif
        QFile file(m_path);
        QByteArray data;
        file.open(QIODevice::ReadOnly);
        data = file.readAll();
        sendFile(data);
    }
}

void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ChatClient::jsonReceived(const QJsonObject &docObj)
{
    QJsonArray jsonReportsArray = docObj["reports"].toArray();

    foreach (const QJsonValue & value, jsonReportsArray) {
        QVariantList varlist;
        QJsonObject obj = value.toObject();
        QString reportName = obj["ReportName"].toString();
        foreach(const QJsonValue & repval, obj["Data"].toArray()) {
                QJsonObject repobj = repval.toObject();
                QVariantMap map;
                map.insert("Key",repobj["Key"].toString());
                map.insert("Data",repobj["Data"].toString());
                varlist.append(map);
        }
        emit analyseComplete(reportName,QVariant::fromValue(varlist));
    }
}

void ChatClient::connectToServer(const QString &address)
{
    QHostAddress addr;
    addr.setAddress(address);
    m_clientSocket->connectToHost(addr, 3333);
}

void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    jsonReceived(jsonDoc.object());
            }
        } else {
            break;
        }
    }
}
