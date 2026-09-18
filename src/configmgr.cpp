#include "configmgr.h"
#include "sqlitemgr.h"

ConfigMgr::ConfigMgr() {}

ConfigMgr::~ConfigMgr() {}

bool ConfigMgr::Init()
{
    QMap<QString, ConfigItem> cfg_map;
    if (!SQLiteMgr::Instance().LoadAllCfg(cfg_map))
    {
        return false;
    }

    InitAiCfgInfo(cfg_map);
    InitCollPicNum(cfg_map);
    InitTimeCfgInfo(cfg_map);

    return true;
}

void ConfigMgr::InitAiCfgInfo(const QMap<QString, ConfigItem>& cfg_map)
{
    AiCfgInfo info;

    // 使能AI
    QString key = QString(GROUP_NAME_AI) + ".enable_ai";
    if (cfg_map.contains(key))
    {
        info.enable_ai_ = (cfg_map[key].param_value_ == "true") ? true : false;
    }

    // 采集高度
    key = QString(GROUP_NAME_AI) + ".collect_height";
    if (cfg_map.contains(key))
    {
        info.collect_height_ = cfg_map[key].param_value_.toUShort();
    }

    // 推理高度
    key = QString(GROUP_NAME_AI) + ".infer_height";
    if (cfg_map.contains(key))
    {
        info.infer_height_ = cfg_map[key].param_value_.toUShort();
    }

    // 图片预览高度
    key = QString(GROUP_NAME_AI) + ".img_view_height";
    if (cfg_map.contains(key))
    {
        info.img_view_height_ = cfg_map[key].param_value_.toUShort();
    }

    // 视频预览高度
    key = QString(GROUP_NAME_AI) + ".video_view_height";
    if (cfg_map.contains(key))
    {
        info.video_view_height_ = cfg_map[key].param_value_.toUShort();
    }

    // 复选高度
    key = QString(GROUP_NAME_AI) + ".sliding_step";
    if (cfg_map.contains(key))
    {
        info.sliding_step_ = cfg_map[key].param_value_.toUShort();
    }

    ai_cfg_info_ = info;
}

AiCfgInfo ConfigMgr::GetAiCfgInfo()
{
    return ai_cfg_info_;
}

bool ConfigMgr::SetAiCfgInfo(const AiCfgInfo& info)
{
    QVector<ConfigItem> cfg_items;
    // 使能AI
    ConfigItem ci;
    ci.group_name_ = GROUP_NAME_AI;
    ci.param_name_ = "enable_ai";
    ci.param_value_ = info.enable_ai_ ? "true" : "false";
    cfg_items.append(ci);
    // 采集高度
    ci.param_name_ = "collect_height";
    ci.param_value_ = QString::number(info.collect_height_);
    cfg_items.append(ci);
    // 推理高度
    ci.param_name_ = "infer_height";
    ci.param_value_ = QString::number(info.infer_height_);
    cfg_items.append(ci);
    // 图片预览高度
    ci.param_name_ = "img_view_height";
    ci.param_value_ = QString::number(info.img_view_height_);
    cfg_items.append(ci);
    // 视频预览高度
    ci.param_name_ = "video_view_height";
    ci.param_value_ = QString::number(info.video_view_height_);
    cfg_items.append(ci);
    // 复选高度
    ci.param_name_ = "sliding_step";
    ci.param_value_ = QString::number(info.sliding_step_);
    cfg_items.append(ci);

    // 更新到数据库中
    if (!SQLiteMgr::Instance().UpdateConfig(cfg_items))
    {
        return false;
    }
    ai_cfg_info_ = info;
    return true;
}

quint16 ConfigMgr::GetCollPicNum()
{
    return coll_pic_num_;
}

bool ConfigMgr::SetCollPicNum(quint16 num)
{
    QVector<ConfigItem> cfg_items;
    ConfigItem ci;
    // 采集张数
    ci.group_name_ = GROUP_NAME_AI;
    ci.param_name_ = "coll_pic_num";
    ci.param_value_ = QString::number(num);
    cfg_items.append(ci);

    // 更新到数据库中
    if (!SQLiteMgr::Instance().UpdateConfig(cfg_items))
    {
        return false;
    }
    coll_pic_num_ = num;
    return true;

}

void ConfigMgr::InitCollPicNum(const QMap<QString, ConfigItem>& cfg_map)
{
    // 使能AI
    coll_pic_num_ = 0;
    QString key = QString(GROUP_NAME_AI) + ".coll_pic_num";
    if (cfg_map.contains(key))
    {
        coll_pic_num_ = cfg_map[key].param_value_.toUShort();
    }
}

TimeCfgInfo ConfigMgr::GetTimeCfgInfo()
{
    return time_cfg_info_;
}

bool ConfigMgr::SetTimeCfgInfo(const TimeCfgInfo& info)
{
    QVector<ConfigItem> cfg_items;
    // 使能喷阀
    ConfigItem ci;
    ci.group_name_ = GROUP_NAME_AI;
    ci.param_name_ = "enable_spray_valve";
    ci.param_value_ = info.enable_spray_valve_ ? "true" : "false";
    cfg_items.append(ci);
    // 吹气时间
    ci.param_name_ = "blow_time";
    ci.param_value_ = QString::number(info.blow_time_);
    cfg_items.append(ci);
    // 动态延迟
    ci.param_name_ = "dynamics_delay";
    ci.param_value_ = QString::number(info.dynamics_delay_);
    cfg_items.append(ci);
    // 固定延迟
    ci.param_name_ = "fixed_delay";
    ci.param_value_ = QString::number(info.fixed_delay_);
    cfg_items.append(ci);
    // 喷嘴个数
    ci.param_name_ = "jets_num";
    ci.param_value_ = QString::number(info.jets_num_);
    cfg_items.append(ci);
    // 传统吹气时间
    ci.param_name_ = "trad_blow_time";
    ci.param_value_ = QString::number(info.trad_blow_time_);
    cfg_items.append(ci);
    // 传统延迟
    ci.param_name_ = "trad_delay_time";
    ci.param_value_ = QString::number(info.trad_delay_time_);
    cfg_items.append(ci);

    // 更新到数据库中
    if (!SQLiteMgr::Instance().UpdateConfig(cfg_items))
    {
        return false;
    }
    time_cfg_info_ = info;
    return true;
}

void ConfigMgr::InitTimeCfgInfo(const QMap<QString, ConfigItem>& cfg_map)
{
    TimeCfgInfo info;

    // 使能喷阀
    QString key = QString(GROUP_NAME_AI) + ".enable_spray_valve";
    if (cfg_map.contains(key))
    {
        info.enable_spray_valve_ = (cfg_map[key].param_value_ == "true") ? true : false;
    }

    // 吹气时间
    key = QString(GROUP_NAME_AI) + ".blow_time";
    if (cfg_map.contains(key))
    {
        info.blow_time_ = static_cast<quint8>(cfg_map[key].param_value_.toUShort());
    }

    // 动态延迟
    key = QString(GROUP_NAME_AI) + ".dynamics_delay";
    if (cfg_map.contains(key))
    {
        info.dynamics_delay_ = cfg_map[key].param_value_.toUShort();
    }

    // 固定延迟
    key = QString(GROUP_NAME_AI) + ".fixed_delay";
    if (cfg_map.contains(key))
    {
        info.fixed_delay_ = cfg_map[key].param_value_.toUShort();
    }

    // 喷嘴个数
    key = QString(GROUP_NAME_AI) + ".jets_num";
    if (cfg_map.contains(key))
    {
        info.jets_num_ = static_cast<quint8>(cfg_map[key].param_value_.toUShort());
    }
    // 传统吹气时间
    key = QString(GROUP_NAME_AI) + ".trad_blow_time";
    if (cfg_map.contains(key))
    {
        info.trad_blow_time_ = static_cast<quint8>(cfg_map[key].param_value_.toUShort());
    }
    // 传统延迟
    key = QString(GROUP_NAME_AI) + ".trad_delay_time";
    if (cfg_map.contains(key))
    {
        info.trad_delay_time_ = cfg_map[key].param_value_.toUShort();
    }

    time_cfg_info_ = info;
}

bool ConfigMgr::GetEnableAcquisition()
{
    return enable_acquisition_;
}


void ConfigMgr::SetEnableAcquisition(bool enable)
{
    // note: 图像采集使能配置不持久化到数据库
    enable_acquisition_ = enable;
}
