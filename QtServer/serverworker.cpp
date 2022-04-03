#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QHostAddress>
#include <QDateTime>

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveJson);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
}

QHostAddress ServerWorker::getHAdress()
{
    return m_serverSocket->peerAddress();
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::sendJson(const QJsonDocument &json)
{
    const QByteArray jsonData = json.toJson(QJsonDocument::Compact);
    emit logMessage(QLatin1String("Send data to user ip: %1").arg(getHAdress().toString()));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->disconnectFromHost();
}

void ServerWorker::receiveJson()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) {

                    const QJsonValue typeVal = jsonDoc.object().value(QLatin1String("type"));
                    if (typeVal.isNull() || !typeVal.isString())
                        return;
                    if (typeVal.toString().compare(QLatin1String("file"), Qt::CaseInsensitive) != 0) {
                        if (typeVal.toString().compare(QLatin1String("command"), Qt::CaseInsensitive) == 0) {
                            const QJsonValue dataVal = jsonDoc.object().value(QLatin1String("data"));
                            if (dataVal.toString().compare(QLatin1String("getstat"), Qt::CaseInsensitive) == 0)
                                emit getUseStat();
                                return;
                        }
                        return;
                    }
                    const QJsonValue dataVal = jsonDoc.object().value(QLatin1String("data"));
                    if (dataVal.isNull())
                        return;
                    QByteArray data = dataVal.toVariant().toByteArray();
                    if (data.isEmpty())
                        return;
                    emit logMessage(QLatin1String("Incoming data %1 bytes from ip ").arg(QString::number(data.length()).toStdString().data()).arg(getHAdress().toString()));

                    analyse = std::make_shared<LTelecomAnalyse>(data);
                    thread1 = std::make_shared<QThread>() ;
                    connect(thread1.get(),&QThread::started,analyse.get(),&LTelecomAnalyse::run);
                    connect(analyse.get(),&LTelecomAnalyse::finished,thread1.get(),&QThread::terminate);
                    connect(analyse.get(),&LTelecomAnalyse::sigJsonResultDone,this,&ServerWorker::sendJson);
                    analyse->moveToThread(thread1.get());
                    thread1.get()->start();
                    emit sendStatToDb(statToDb(m_serverSocket->peerAddress().toString(), data.size(), QDateTime::currentDateTime()));
                }
                else
                    emit logMessage(QLatin1String("Wrong data"));
            } else {
                emit logMessage(QLatin1String("Wrong data"));
            }
        } else {
            break;
        }
    }
}


