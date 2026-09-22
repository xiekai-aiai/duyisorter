#include "sqlitemgr.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QApplication>
#include "unilog.h"
#include "aihelper.h"

SQLiteMgr::SQLiteMgr() {}

SQLiteMgr::~SQLiteMgr() {}

bool SQLiteMgr::Init()
{
    db_path_ = ai_helper::GetDBPath();
    db_ = QSqlDatabase::addDatabase("QSQLITE");                // 添加SQLITE数据库驱动
    db_.setDatabaseName(db_path_);                           // 设置数据库文件
    if (!db_.open())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! " << db_.lastError().text().toStdString());
        return false;
    }

    LOG_INFO_STM("open sqlite db[" << db_path_.toStdString() << "] successful!");
    return true;
}

bool SQLiteMgr::LoadAllCfg(QMap<QString, ConfigItem>& cfg_map)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
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
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    if (!db_.transaction())
    {
        LOG_ERROR_STM("Begin transaction failed:" << db_.lastError().text().toStdString());
        return false;
    }
#if 0
    // note: 在老版本上这种写法更新数据库是报错
    query.prepare(
        "INSERT INTO config (group_name, param_name, param_value) "
        "VALUES (:group_name, :param_name, :param_value) "
        "ON CONFLICT (group_name, param_name) "
        "DO UPDATE SET param_value = excluded.param_value"
    );
#endif 
    query.prepare(
        "INSERT OR REPLACE INTO config "
        "(group_name, param_name, param_value) "
        "VALUES (:group_name, :param_name, :param_value)"
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

bool SQLiteMgr::InsertModelInfo(const ModelInfo& info)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    query.prepare(R"(
        INSERT INTO model_info (modelId, modelName, isApply, isUpload)
        VALUES (?, ?, ?, ?)
    )");

    query.addBindValue(info.model_id_);
    query.addBindValue(info.model_name_);
    query.addBindValue(info.is_apply_);
    query.addBindValue(info.is_upload_);

    if (!query.exec())
    {
        LOG_ERROR_STM("insert record into model info failed:"
            << db_.lastError().text().toStdString());
        return false;
    }
    return true;
}


bool SQLiteMgr::UpdateModelInfoUploadFlag(const QString& model_id, bool is_upload)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("update model_info set isUpload = ?  WHERE modelId = ?");
    query.addBindValue(is_upload);
    query.addBindValue(model_id);

    if (!query.exec())
    {
        LOG_ERROR_STM("update record into model info failed:"
            << db_.lastError().text().toStdString());
        return false;
    }
    return true;
}

bool SQLiteMgr::UpdateModelInfoApplyFlag(const QString& model_id, bool is_apply)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    if (!db_.transaction())
    {
        LOG_ERROR_STM("begin transaction model info failed:"
            << db_.lastError().text().toStdString());
        return false;
    }

    if (is_apply)
    {
        // 同时只能应用一个模型，如果是应用，将其他模型应用标识设置为0
        query.prepare("UPDATE model_info SET isApply = 0 WHERE modelId != ? AND isApply = 1");
        query.addBindValue(model_id);
        if (!query.exec())
        {
            LOG_ERROR_STM("[ApplyModel] Step1 (disable others) failed:" << query.lastError().text().toStdString());
            db_.rollback();
            return false;
        }
    }

    query.prepare("UPDATE model_info SET isApply = ? WHERE modelId = ?");
    query.addBindValue(is_apply);
    query.addBindValue(model_id);
    if (!query.exec())
    {
        LOG_ERROR_STM("[ApplyModel] Step1 (disable others) failed:" << query.lastError().text().toStdString());
        db_.rollback();
        return false;
    }

    // 4. 提交事务
    if (!db_.commit())
    {
        LOG_ERROR_STM("[ApplyModel] Commit failed::" << db_.lastError().text().toStdString());
        db_.rollback();
        return false;
    }

    LOG_INFO_STM("model apply info updated successfully, model_id:" << model_id.toStdString() << ", is_apply:" << is_apply);
    return true;
}


bool SQLiteMgr::DelModelInfo(const QString& model_id)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("DELETE FROM model_info WHERE modelId = ?");
    query.addBindValue(model_id);

    if (!query.exec())
    {
        LOG_ERROR_STM("[Delete Failed]" << query.lastError().text().toStdString());
        return false;
    }

    LOG_INFO_STM("delete model info successfully, model_id:" << model_id.toStdString());
    return true;
}

bool SQLiteMgr::LoadAllModeInfo(QVector<ModelInfo>& model_infos)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    QString cmd("select id, modelId, modelName, isApply, isUpload from model_info");

    // 1. 准备sql
    if (!query.prepare(cmd))
    {
        LOG_ERROR_STM("[prepare Failed]" << query.lastError().text().toStdString());
        return false;
    }

    // 2. 执行sql
    if (!query.exec())
    {
        LOG_ERROR_STM("[exec Failed]" << query.lastError().text().toStdString());
        return false;
    }

    // 3. 遍历查询结果
    while (query.next())
    {
        ModelInfo item;
        item.id_ = query.value("id").toInt();
        item.model_id_ = query.value("modelId").toString();
        item.model_name_ = query.value("modelName").toString();
        item.is_apply_ = query.value("isApply").toBool();
        item.is_upload_ = query.value("isUpload").toBool();

        model_infos.append(item);
    }

    return true;
}

bool SQLiteMgr::LoadModelInfoById(const QString& model_id, ModelInfo& info)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    QSqlQuery query(db_);
    QString cmd("select id, modelId, modelName, isApply, isUpload from model_info where modelId = ? LIMIT 1");

    // 1. 准备sql
    if (!query.prepare(cmd))
    {
        LOG_ERROR_STM("[prepare Failed]" << query.lastError().text().toStdString());
        return false;
    }

    query.addBindValue(model_id);

    // 2. 执行sql
    if (!query.exec() || !query.next())
    {
        LOG_ERROR_STM("[exec Failed]" << query.lastError().text().toStdString());
        return false;
    }

    info.id_ = query.value(0).toInt();
    info.model_id_ = query.value(1).toString();
    info.model_name_ = query.value(2).toString();
    info.is_apply_ = query.value(3).toInt();
    info.is_upload_ = query.value(4).toInt();
    return true;
}

bool SQLiteMgr::InsertModelClsParam(const ModelClsParam& info)
{
    return InsertModelClsParams(QVector<ModelClsParam>{info});
}


bool SQLiteMgr::InsertModelClsParams(const QVector<ModelClsParam>& infos)
{
    if (infos.isEmpty())
    {
        return true;
    }

    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }

    if (!db_.transaction())
    {
        LOG_ERROR_STM("begin transaction model info failed:"
            << db_.lastError().text().toStdString());
        return false;
    }

    QSqlQuery query(db_);
    query.prepare(R"(
        INSERT INTO model_params
        (modelId, clsId, clsName, level, identifyGroup, threshold, isApply, areaModel, areaThreshold)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");

    bool success = true;
    for (int i = 0; i < infos.size(); ++i)
    {
        const ModelClsParam& p = infos[i];
        query.bindValue(0, p.model_id_);
        query.bindValue(1, p.cls_id_);
        query.bindValue(2, p.cls_name_);
        query.bindValue(3, p.level_);
        query.bindValue(4, p.identify_grp_);
        query.bindValue(5, p.threshold_);
        query.bindValue(6, p.is_apply_);
        query.bindValue(7, p.area_model_);
        query.bindValue(8, p.area_threshold_);

        if (!query.exec())
        {
            LOG_ERROR_STM("[BatchInsert] Failed at row" << i << ":" << query.lastError().text().toStdString());
            success = false;
            break;
        }
    }

    if (success)
    {
        return db_.commit();
    }

    db_.rollback();
    LOG_ERROR_STM("[BatchInsert] Rolled back due to error");
    return false;
}


bool SQLiteMgr::LoadModelClsParam(const QString& model_id, QVector<ModelClsParam>& infos)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed! ");
        return false;
    }


    QSqlQuery query(db_);
    QString cmd("select modelId, clsId, clsName, level, identifyGroup, threshold, isApply, areaModel, areaThreshold from model_params where modelId = ?");

    // 1. 准备sql
    if (!query.prepare(cmd))
    {
        LOG_ERROR_STM("prepare: " << cmd.toStdString() << " failed! error:" << query.lastError().text().toStdString());
        return false;
    }

    query.addBindValue(model_id);

    // 2. 执行sql
    if (!query.exec())
    {
        LOG_ERROR_STM("exec: " << cmd.toStdString() << " failed! error:" << query.lastError().text().toStdString());
        return false;
    }

    // 3. 遍历查询结果
    while (query.next())
    {
        ModelClsParam item;
        item.model_id_ = query.value("modelId").toString();
        item.cls_id_ = query.value("clsId").toUInt();
        item.cls_name_ = query.value("clsName").toString();
        item.level_ = query.value("level").toUInt();
        item.identify_grp_ = query.value("identifyGroup").toUInt();
        item.threshold_ = query.value("threshold").toUInt();
        item.is_apply_ = query.value("isApply").toBool();
        item.area_model_ = query.value("areaModel").toUInt();
        item.area_threshold_ = query.value("areaThreshold").toUInt();

        infos.append(item);
    }

    return true;
}

bool SQLiteMgr::UpdateModelClsParam(const QVector<ModelClsParam>& infos)
{
    if (!db_.isOpen())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] failed!");
        return false;
    }

    if (!db_.transaction())
    {
        LOG_ERROR_STM("open sqlite db[" << db_path_.toStdString() << "] Transaction start failed!");
        return false;
    }

    QSqlQuery query(db_);
    // 预编译 UPDATE 语句（复用提升性能）
    query.prepare(R"(
            UPDATE model_params
            SET threshold = ?, isApply = ?, areaModel = ?, areaThreshold = ?
            WHERE modelId = ? AND clsId = ? AND level = ? AND identifyGroup = ?
    )");

    bool success = true;
    int updatedCount = 0;

    for (int i = 0; i < infos.size(); ++i)
    {
        const auto& p = infos.at(i);

        // 按 '?' 占位符顺序绑定值
        query.bindValue(0, p.threshold_);
        query.bindValue(1, p.is_apply_);
        query.bindValue(2, p.area_model_);
        query.bindValue(3, p.area_threshold_);
        query.bindValue(4, p.model_id_);
        query.bindValue(5, p.cls_id_);
        query.bindValue(6, p.level_);
        query.bindValue(7, p.identify_grp_);

        if (!query.exec())
        {
            LOG_ERROR_STM("BatchUpdate failed, modelId:" << p.model_id_.toStdString() << ", cls id:" << p.cls_id_
                << ", level:" << p.level_ << ", identify grp:" << p.identify_grp_ << ",error:"
                << query.lastError().text().toStdString());
            success = false;
            break;  // 遇到错误立即中断，保证原子性
        }
    }

    if (!success)
    {
        db_.rollback();
        LOG_ERROR_STM("[BatchUpdate] Rolled back due to execution error!");
        return false;
    }

    if (!db_.commit())
    {
        LOG_ERROR_STM("[BatchUpdate] Commit failed:" << db_.lastError().text().toStdString());
        db_.rollback();
        return false;
    }

    return true;
}

