/*
 * @Description: AI相关功能数据库管理类头文件
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-15 14:45:14
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-22 10:58:52
 */
#ifndef SQLITEMGR_H
#define SQLITEMGR_H

#include <QSqlDatabase>
#include <QMap>
#include <QVector>
#include "sortertypes.h"


class SQLiteMgr final
{
public:
    static SQLiteMgr& Instance()
    {
        static SQLiteMgr sql_mgr;
        return sql_mgr;
    }

    /*
     * 初始化数据库，并加载所有配置参数
    */
    bool Init();

    /**
     * @brief: 读取配置表中所有参数
     * @param {QMap<QString, QString>&} 配置参数信息
     * @return {*}
     */
    bool LoadAllCfg(QMap<QString, ConfigItem>& cfg_map);

    /**
     * @brief: 更新配置表中参数
     * @param {QVector<ConfigItem>&} 配置参数信息
     * @return {*}
     */
    bool UpdateConfig(const QVector<ConfigItem>& cfg_items);

    /**
     * @brief: 插入记录到模型信息表中
     * @param {ModelInfo& info} 模型信息
     * @return {*}
     */
    bool InsertModelInfo(const ModelInfo& info);

    /**
     * @brief: 更新模型上传标识
     * @param {QString& model_id} 模型id
     * @param {bool is_upload} 是否上传
     * @return {*}
     */
    bool UpdateModelInfoUploadFlag(const QString& model_id, bool is_upload);

    /**
     * @brief: 更新模型应用标识
     * @note: 应用该模型时，会自动将其他模型的应用标识设置为0
     * @param {QString& model_id} 模型id
     * @param {bool is_apply} 是否应用
     * @return {*}
     */
    bool UpdateModelInfoApplyFlag(const QString& model_id, bool is_apply);

    /**
     * @brief: 从模型信息表中删除记录
     * @param {QString& model_id} 模型id
     * @return {*}
     */
    bool DelModelInfo(const QString& model_id);

    /**
     * @brief: 从模型信息表中查询所有模型
     * @param {QVector<ModelInfo> model_infos} 模型信息
     * @return {*}
     */
    bool LoadAllModeInfo(QVector<ModelInfo>& model_infos);

    /**
     * @brief: 插入记录到模型参数表中
     * @param {ModelClsParam& info} 模型参数
     * @return {*}
     */
    bool InsertModelClsParam(const ModelClsParam& info);

    /**
     * @brief: 批量插入记录到模型参数表中
     * @param {QVector<ModelClsParam>& infos} 模型参数
     * @return {*}
     */
    bool InsertModelClsParams(const QVector<ModelClsParam>& infos);

    /**
     * @brief: 查询模型参数信息
     * @param {QString& model_id} 模型id
     * @param {QVector<ModelClsParam>& infos} 模型参数
     * @return {*}
     */
    bool LoadModelClsParam(const QString& model_id, QVector<ModelClsParam>& infos);

private:
    SQLiteMgr();
    ~SQLiteMgr();
    SQLiteMgr(SQLiteMgr&&) = delete;
    SQLiteMgr(const SQLiteMgr&) = delete;
    SQLiteMgr& operator=(const SQLiteMgr&) = delete;
    SQLiteMgr& operator=(SQLiteMgr&&) = delete;


private:
    QSqlDatabase db_;                  // 数据库实例
    QString db_path_;                  // 数据库文件路径
};

#endif // SQLITEMGR_H
