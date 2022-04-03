#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>

struct statToDb {
    QString address;
    int data_size;
    QDateTime datatime_query;

    statToDb(QString addr, int size, QDateTime dt) {
        address = addr;
        data_size = size;
        datatime_query = dt;
    };
    statToDb();
};

class SQLiteAdapter
{
public:
    SQLiteAdapter();
    ~SQLiteAdapter();
public:
    bool openDb(void);
    void createTable(void);
    QJsonObject queryTable();
    void singleInsertData (statToDb & singledb);
    void closeDb(void);
private:
         QSqlDatabase db;
};

