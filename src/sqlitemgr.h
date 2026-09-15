/*
 * @Description: AI相关功能数据库管理类头文件
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-15 14:45:14
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-15 14:51:48
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

private:
    SQLiteMgr();
    ~SQLiteMgr();
    SQLiteMgr(SQLiteMgr&&) = delete;
    SQLiteMgr(const SQLiteMgr&) = delete;
    SQLiteMgr& operator=(const SQLiteMgr&) = delete;
    SQLiteMgr& operator=(SQLiteMgr&&) = delete;


private:
    QSqlDatabase db_;                  // 数据库实例
};

#endif // SQLITEMGR_H
