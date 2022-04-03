#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>

class QHostAddress;
class QJsonDocument;

class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
public slots:
    void connectToServer(const QString &address);
    void sendCommand();
    void sendFile(const QByteArray &data);
    void openAndSendFile(const QString &path);
    void disconnectFromHost();
private slots:
    void onReadyRead();
signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void analyseComplete(QString name,QVariant data);
private:
    QTcpSocket *m_clientSocket;
    void jsonReceived(const QJsonObject &doc);
};

#endif // CHATCLIENT_H
