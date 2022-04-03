#include "sqliteadapter.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>

SQLiteAdapter::SQLiteAdapter()
{
    QSqlDatabase database;
        if (QSqlDatabase::contains("qt_sql_default_connection"))
        {
            database = QSqlDatabase::database("qt_sql_default_connection");
        }
        else
        {
            db = QSqlDatabase::addDatabase("QSQLITE");
            QString dbname = QDir::currentPath() + QString("/")+ QString("statanalyse.db");
            db.setDatabaseName(dbname);
        }

}

SQLiteAdapter::~SQLiteAdapter()
{
    db.close();
}

bool SQLiteAdapter::openDb()
{
    if(db.open() == false){
        return false;
    }
    return true;
}

void SQLiteAdapter::createTable()
{
    QSqlQuery query;
    QString str = QString("CREATE TABLE 'StatAnalyse' (\
                          'id' INTEGER PRIMARY KEY AUTOINCREMENT,\
                          'address' VARCHAR(255),\
                          'data_size' INTEGER,\
                          'datatime_query' DATETIME)");
    query.exec(str);
}

QJsonObject SQLiteAdapter::queryTable()
{
    QJsonArray m_jsonarray;
    QSqlQuery query;
    QString str = QString("SELECT address,data_size,datatime_query FROM 'StatAnalyse'");
    query.exec(str);
    qDebug() << query.lastError().text() ;
    int count = 1;
    while(query.next()) {
        QJsonObject m_jsonobj;
        m_jsonobj.insert("Key",QString("%1").arg(count));
        m_jsonobj.insert("Data",QString("%1 %2 bytes from %3").arg(query.value(2).toDateTime().toString("dd/MM/yyyy hh:mm:ss"))
                                                        .arg(query.value(1).toInt())
                                                         .arg(query.value(0).toString()));
        m_jsonarray.append(m_jsonobj);
        count++;
    }
    QJsonObject m_repobj;
    m_repobj.insert("ReportName","Use stat");
    m_repobj.insert("Data",m_jsonarray);
    return m_repobj;
}

void SQLiteAdapter::singleInsertData(statToDb &singledb)
{
    QSqlQuery query;
    query.prepare("INSERT INTO 'StatAnalyse'( 'address','data_size','datatime_query' ) VALUES (:address,:data_size,:datatime_query)");
    query.bindValue(":address", singledb.address);
    query.bindValue(":data_size", singledb.data_size);
    query.bindValue(":datatime_query",singledb.datatime_query);
    query.exec();
}

void SQLiteAdapter::closeDb(void)
{
    db.close();
}
