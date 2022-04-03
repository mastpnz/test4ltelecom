#include "ltelecomanalyse.h"
#include <QHash>
#include <iostream>

void LTelecomAnalyse::run()
{
    QJsonDocument m_resdoc;
    QJsonArray m_resultsarray;
    m_resultsarray.append(analyseCharsFreq(m_data));
    m_resultsarray.append(analyseWordLength(m_data));
    QJsonObject m_resobj;
    m_resobj.insert("reports",m_resultsarray);
    m_resdoc.setObject(m_resobj);
    emit sigJsonResultDone(m_resdoc);
    emit finished();
}

QJsonObject LTelecomAnalyse::analyseCharsFreq(const QByteArray &data)
{
    QHash<QString,int> chars_freq;
    foreach (char ch, data) {
        chars_freq.insert(QString(ch),chars_freq.value(QString(ch), 0) +1 );
    }
    return packReport(chars_freq,"Chars freqence");
};

QJsonObject LTelecomAnalyse::analyseWordLength(const QByteArray &data)
{
    QHash<QString,int> words_length;
    foreach (auto &str, QString(data).split("\n")) {
        foreach (auto &word, str.split(" ")) {
            if(word.length() > 0)
                words_length.insert(QString("%1").arg(word.length()),words_length.value(QString("%1").arg(word.length()),0)+1);
        }
    }
    return packReport(words_length,"Word length");
};

QJsonObject LTelecomAnalyse::packReport(QHash<QString, int> &hashdata, QString namereport)
{
    QHashIterator<QString,int> it(hashdata);
    QJsonArray m_jsonarray;
    while (it.hasNext()) {
        it.next();
        QJsonObject m_jsonobj;
        m_jsonobj.insert("Key",QString("%1").arg(it.key()));
        m_jsonobj.insert("Data",QString("%1").arg(it.value()));
        m_jsonarray.append(m_jsonobj);
    }
    QJsonObject m_repobj;
    m_repobj.insert("ReportName",namereport);
    m_repobj.insert("Data",m_jsonarray);
    return m_repobj;
}
