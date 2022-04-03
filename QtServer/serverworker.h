#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <memory>
#include "ltelecomanalyse.h"
#include "sqliteadapter.h"

class QJsonObject;
class ServerWorker : public QObject

{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QHostAddress getHAdress();
signals:
    void jsonReceived(const QJsonObject &jsonDoc);
    void disconnectedFromClient();
    void error();
    void logMessage(const QString &msg);
    void sendStatToDb(statToDb stat);
    void getUseStat();
public slots:
    void disconnectFromClient();
    void sendJson(const QJsonDocument &jsonData);
private slots:
    void receiveJson();
private:
    QTcpSocket *m_serverSocket;
    std::shared_ptr<QThread> thread1;
    std::shared_ptr<LTelecomAnalyse> analyse;
};

#endif // SERVERWORKER_H
