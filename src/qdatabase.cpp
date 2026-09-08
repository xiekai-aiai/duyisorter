#include "qdatabase.h"
#include <QDebug>
#include <QStringList>
//#include <String>
#include <QCoreApplication>

QDataBase qDataBaseInfo;
QDataBase::QDataBase()
{
//	QCoreApplication::addLibraryPath("./plugin");

        qDebug() << QSqlDatabase::drivers();
//        QSqlDatabase database;
//        if (QSqlDatabase::contains("qt_sql_default_connection")){
//            database = QSqlDatabase::database("qt_sql_default_connection");
//        }else{

        QDir dir(QString("%1%2").arg(APP_PATH).arg("/userdata/db"));
        if(!dir.exists()){
            dir.mkdir(QString("%1%2").arg(APP_PATH).arg("/userdata/db"));
        }
        QSqlDatabase   database = QSqlDatabase::addDatabase("QSQLITE");
            database.setDatabaseName(SYS_DB_INFO);
            database.setUserName("jiami");
            database.setPassword("123456");
        //        }
        if (!database.open())
        {
            qDebug() << "Error: Failed to connect database." << database.lastError();
        }
//        return database;
}

QDataBase::~QDataBase()
{

}

QSqlDatabase QDataBase:: initQDatabase(){
//    qDebug() << QSqlDatabase::drivers();
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection")){
        database = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        database = QSqlDatabase::addDatabase("QSQLITE");
//        database.setDatabaseName("MyDataBase.db");
//        qDebug() <<QCoreApplication::applicationDirPath();
        database.setDatabaseName(SYS_DB_INFO);
        database.setUserName("jiami");
        database.setPassword("123456");
    }
    return database;
}
void QDataBase:: openQDatabase(QSqlDatabase database){
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
//        createTable();
//        return database;
    }
}

void QDataBase:: createTable(QSqlQuery &query,QString create_sql){
//    QSqlQuery sql_query;
//    QString create_sql = "CREATE TABLE students ("
//                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                         "name VARCHAR(40) NOT NULL, "
//                         " score INTEGER NOT NULL, "
//                         "class VARCHAR(40) NOT NULL)"; //创建数据表

    query.prepare(create_sql); //创建表
    if(!query.exec()){ //查看创建表是否成功
        qDebug()<<QObject::tr("Table Create failed");
        qDebug()<<query.lastError();
    }else{
        qDebug()<< "Table Created" ;
    }
}


void QDataBase:: closeQDatabase(QSqlDatabase database){
     database.close();
}

bool QDataBase:: isTableExist(QSqlQuery &query,QString table)
{
   QString sql = QString("select * from sqlite_master where name='%1'").arg(table);
   query.exec(sql);
   return query.next();
}

int QDataBase:: checkIsTableDataExist(QSqlQuery &query,QString table){
    QString sql = QString("select count(*) from '%1'").arg(table);
    query.exec(sql);
    query.next();
    int isExist = 0;
    qDebug()<<"no"<<query.value(0).toInt()<<flush;
    if(query.value(0).toInt()>0){
        isExist = 1;
    }else{
        isExist = 0;
    }
    return isExist;
}

bool QDataBase::addSfio(QSqlQuery &query, const QString &machineNo, int encyStatus, QString encyCode, QString encyDays ,QDateTime encyTime){
 query.addBindValue(machineNo);
 query.addBindValue(encyStatus);
 query.addBindValue(encyCode);
 query.addBindValue(encyDays);
 query.addBindValue(encyTime);
 return query.exec();

}

bool QDataBase::addEncycodeio(QSqlQuery &query, const QString &machineNo, QString encyCode, QDateTime encyTime){
 query.addBindValue(machineNo);
 query.addBindValue(encyCode);
 query.addBindValue(encyTime);
 return query.exec();

}

bool QDataBase::updateSfio(QSqlQuery &query, QString machineNo, int encyStatus, QString encyCode, QString encyDays ,QDateTime encyTime){
 query.bindValue(":machineNo", machineNo);
 query.bindValue(":encyStatus", encyStatus);
 query.bindValue(":encyCode", encyCode);
 query.bindValue(":encyDays", encyDays);
 query.bindValue(":encyTime", encyTime);
 return query.exec();

}

