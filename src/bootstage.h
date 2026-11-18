/*!
 * \file        bootstage.h
 * \brief       开机检测分组与状态定义（启动画面 5 段进度条共用）
 * \date        2026.11.12
 */
#ifndef BOOTSTAGE_H
#define BOOTSTAGE_H

/*!
 * \brief 开机检测分组，与启动画面 5 段进度条一一对应
 */
enum BootCheckStage
{
    BOOT_STAGE_CONFIG = 0,      //! 系统配置/参数文件/方案/相机参数
    BOOT_STAGE_SERIAL = 1,      //! 串口设备/机型下发
    BOOT_STAGE_AI     = 2,      //! AI图像参数/AI像素参数
    BOOT_STAGE_UI     = 3,      //! 界面与数据表/加密授权
    BOOT_STAGE_HW     = 4,      //! 设备自检/AI板自检
    BOOT_STAGE_COUNT  = 5
};

/*!
 * \brief 单个检测分组的状态
 */
enum BootCheckStatus
{
    BOOT_STATUS_IDLE    = 0,    //! 未开始
    BOOT_STATUS_ACTIVE  = 1,    //! 检测中
    BOOT_STATUS_OK      = 2,    //! 正常
    BOOT_STATUS_ERROR   = 3,    //! 异常
    BOOT_STATUS_SKIPPED = 4     //! 按配置跳过
};

#endif // BOOTSTAGE_H
