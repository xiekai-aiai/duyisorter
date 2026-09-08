#ifndef QDATABASE_H
#define QDATABASE_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>
#include <QtGui>
#include <QProcess>
#include <globalparams.h>

class QDataBase
{
public:
    QDataBase();
    ~QDataBase();
    QSqlDatabase  initQDatabase();
    void openQDatabase(QSqlDatabase database);
    void closeQDatabase(QSqlDatabase database);
    void createTable(QSqlQuery &query,QString create_sql);
    bool isTableExist(QSqlQuery &query,QString table);
    int checkIsTableDataExist(QSqlQuery &query,QString table);
    bool addSfio(QSqlQuery &query, const QString &machineNo, int encyStatus, QString encyCode,QString encyDays, QDateTime time);
    bool addEncycodeio(QSqlQuery &query, const QString &machineNo, QString encyCode, QDateTime encyTime);
    bool updateSfio(QSqlQuery &query, QString machineNo, int encyStatus, QString encyCode, QString encyDays ,QDateTime encyTime);
};
extern QDataBase qDataBaseInfo;


#endif // QDATABASE_H
