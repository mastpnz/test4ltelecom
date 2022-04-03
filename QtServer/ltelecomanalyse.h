#pragma once

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QtConcurrentRun>
#include <QHash>

class LTelecomAnalyse : public QThread
{
    Q_OBJECT
public:
    LTelecomAnalyse(QByteArray data) : m_data(data) {}
protected:
    QJsonObject analyseCharsFreq(const QByteArray &data);
    QJsonObject analyseWordLength(const QByteArray &data);
    QJsonObject packReport(QHash<QString,int> &hashdata, QString namereport);
    QByteArray m_data;
public slots:
    void run();
signals:
    void finished();
    void sigJsonResultDone(const QJsonDocument &m_jsondoc);
};
