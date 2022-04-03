#include "chatserver.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>
#include <iostream>
#include <QTime>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent)
{
    m_sqlad.openDb();
    m_sqlad.createTable();
}

ChatServer::~ChatServer()
{
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }
    connect(worker, &ServerWorker::getUseStat, this, std::bind(&ChatServer::sendUseStat, this, worker));
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&ChatServer::userDisconnected, this, worker));
    connect(worker, &ServerWorker::error, this, std::bind(&ChatServer::userError, this, worker));
    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage);
    connect(worker, &ServerWorker::sendStatToDb, this, &ChatServer::writeStat);

    emit logMessage(QStringLiteral("New client connected from %1").arg(worker->getHAdress().toString()));
    m_clients.append(worker);
}

void ChatServer::writeStat(statToDb stat) {
    m_sqlad.singleInsertData(stat);
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    emit logMessage(QLatin1String("Client disconnected from %1").arg(sender->getHAdress().toString()));
    sender->deleteLater();
}

void ChatServer::sendUseStat(ServerWorker *sender)
{
    QJsonDocument m_resdoc;
    QJsonArray m_resultsarray;
    m_resultsarray.append(m_sqlad.queryTable());
    QJsonObject m_resobj;
    m_resobj.insert("reports",m_resultsarray);
    m_resdoc.setObject(m_resobj);
    sender->sendJson(m_resdoc);
}

void ChatServer::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage(QLatin1String("Client error from %1").arg(sender->getHAdress().toString()));
}

void ChatServer::stopServer()
{
    for (ServerWorker *worker : m_clients) {
        worker->disconnectFromClient();
    }
    close();
}

void ChatServer::logMessage(const QString &msg)
{
    QTime nowtime = QTime::currentTime();
    std::cout << "ServerLogs(" << nowtime.toString("dd/MM/yy hh:mm:ss").toStdString() << "): " << msg.toStdString() << std::endl;
}
