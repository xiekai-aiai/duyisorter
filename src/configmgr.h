/*
 * @Description: Ai相关UI配置管理类，所有的配置由该类进行统一管理(查询和存储)
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-15 14:45:14
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-15 14:47:19
 */
#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include <QObject>
#include <QtGlobal>
#include "sortertypes.h"

class ConfigMgr final
{
public:
    static ConfigMgr& Instance()
    {
        static ConfigMgr mgr;
        return mgr;
    }

    bool Init();

    /**
     * @brief: 获取AI参数
     * @return {*}
     */
    AiCfgInfo GetAiCfgInfo();

    /**
     * @brief: 设置AI参数
     * @param {AiCfgInfo&} Ai参数信息
     * @return {*}
     */
    bool SetAiCfgInfo(const AiCfgInfo& info);

    /**
     * @brief: 获取图片采集张数
     * @return {*}
     */
    quint16 GetCollPicNum();

    /**
     * @brief: 设置图片采集张数
     * @param {quint16&} 图片采集张数
     * @return {*}
     */
    bool SetCollPicNum(quint16 num);

    /**
     * @brief: 获取时间信息
     * @return {*}
     */
    TimeCfgInfo GetTimeCfgInfo();

    /**
     * @brief: 设置时间信息
     * @param {TimeCfgInfo&} 时间信息
     * @return {*}
     */
    bool SetTimeCfgInfo(const TimeCfgInfo& info);

private:
    ConfigMgr();
    ~ConfigMgr();
    ConfigMgr(ConfigMgr&&) = delete;
    ConfigMgr(const ConfigMgr&) = delete;
    ConfigMgr& operator=(const ConfigMgr&) = delete;
    ConfigMgr& operator=(ConfigMgr&&) = delete;

    /**
     * @brief: 初始化AI参数信息
     * @param {QMap<QString, ConfigItem>&} 配置表记录
     * @return {*}
     */
    void InitAiCfgInfo(const QMap<QString, ConfigItem>& cfg_map);

    /**
     * @brief: 初始化图片采集张数
     * @param {QMap<QString, ConfigItem>&} 配置表记录
     * @return {*}
     */
    void InitCollPicNum(const QMap<QString, ConfigItem>& cfg_map);

    /**
     * @brief: 初始化时间配置
     * @param {QMap<QString, ConfigItem>&} 配置表记录
     * @return {*}
     */
    void InitTimeCfgInfo(const QMap<QString, ConfigItem>& cfg_map);

private:
    AiCfgInfo ai_cfg_info_;           // AI 信息
    quint16 coll_pic_num_;            // 图片采集张数
    TimeCfgInfo time_cfg_info_;       //
};

#endif // CONFIGMGR_H
