/*
 * @Description: Ai相关接口辅助头文件
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-18 11:10:07
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-18 11:16:08
 */
#ifndef AIHELPER_H
#define AIHELPER_H

#include <QtGlobal>

namespace ai_helper
{

    /**
     * @brief: 根据索引获取AI设备的IP地址
     * @return {*}
     */
    QString GetAiIpByIndex(int index);

    /**
     * @brief: 获取sqlite数据库路径
     * @return {*}
     */
    QString GetDBPath();

    /**
     * @brief: 获取本地图片采集的根目录
     * @return {*}
     */
    QString GetAcqImgRootPath();

};

#endif // AIHELPER_H
