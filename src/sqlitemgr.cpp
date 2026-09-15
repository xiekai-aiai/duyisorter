#include "sqlitemgr.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QApplication>
#include "unilog.h"

SQLiteMgr::SQLiteMgr() {}

SQLiteMgr::~SQLiteMgr() {}

bool SQLiteMgr::Init()
{
    db_ = QSqlDatabase::addDatabase("QSQLITE");                // 添加SQLITE数据库驱动
    db_.setDatabaseName(AI_DB_NAME);                           // 设置数据库文件
    if (!db_.open())
    {
        LOG_ERROR_STM("open sqlite db[" << AI_DB_NAME << "] failed! " << db_.lastError().text().toStdString());
        return false;
    }

    LOG_INFO_STM("open sqlite db[" << AI_DB_NAME << "] successful!");
    return true;
}

bool SQLiteMgr::LoadAllCfg(QMap<QString, ConfigItem>& cfg_map)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << AI_DB_NAME << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    QString cmd("select id, group_name, param_name, param_value from config");

    // 1. 准备sql
    if (!query.prepare(cmd))
    {
        LOG_ERROR_STM("prepare: " << cmd.toStdString() << " failed! error:" << query.lastError().text().toStdString());
        return false;
    }

    // 2. 执行sql
    if (!query.exec())
    {
        LOG_ERROR_STM("exec: " << cmd.toStdString() << " failed! error:" << query.lastError().text().toStdString());
        return false;
    }

    // 3. 遍历查询结果
    while (query.next())
    {
        ConfigItem item;
        item.id = query.value("id").toInt();
        item.group_name_ = query.value("group_name").toString();
        item.param_name_ = query.value("param_name").toString();
        item.param_value_ = query.value("param_value").toString();

        const QString key = item.group_name_ + "." + item.param_name_;
        cfg_map.insert(key, item);
    }

    return true;
}

bool SQLiteMgr::UpdateConfig(const QVector<ConfigItem>& cfg_items)
{
    if (cfg_items.isEmpty())
    {
        return true;
    }

    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << AI_DB_NAME << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    if (!db_.transaction())
    {
        LOG_ERROR_STM("Begin transaction failed:" << db_.lastError().text().toStdString());
        return false;
    }

    query.prepare(
        "INSERT INTO config (group_name, param_name, param_value) "
        "VALUES (:group_name, :param_name, :param_value) "
        "ON CONFLICT (group_name, param_name) "
        "DO UPDATE SET param_value = excluded.param_value"
    );

    for (const ConfigItem& item : cfg_items)
    {
        query.bindValue(":group_name", item.group_name_);
        query.bindValue(":param_name", item.param_name_);
        query.bindValue(":param_value", item.param_value_);

        if (!query.exec())
        {
            LOG_ERROR_STM("Set config failed:" << item.group_name_.toStdString() << "." << item.param_name_.toStdString()
                << query.lastError().text().toStdString());

            db_.rollback();
            return false;
        }
    }

    if (!db_.commit())
    {
        LOG_ERROR_STM("Commit transaction failed:"
            << db_.lastError().text().toStdString());
        return false;
    }

    return true;
}
