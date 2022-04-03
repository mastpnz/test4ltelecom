#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QVector>
#include "sqliteadapter.h"

class QThread;
class ServerWorker;
class ChatServer : public QTcpServer

{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)
public:
    explicit ChatServer(QObject *parent = nullptr);
    ~ChatServer();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
public slots:
    void logMessage(const QString &msg);
    void stopServer();
    void writeStat(statToDb stat);
private slots:
    void sendUseStat(ServerWorker *sender);
    void userDisconnected(ServerWorker *sender);
    void userError(ServerWorker *sender);
private:
    SQLiteAdapter m_sqlad;

    void sendJson(ServerWorker *destination, const QJsonObject &message);
    QVector<ServerWorker *> m_clients;
};

#endif // CHATSERVER_H
