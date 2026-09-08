/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        feedercontrolthread.h
 * \brief       黑白机型料位监控线程头文件。
 * \date        2015.01.14
 */
#ifndef FEEDERCONTROLTHREAD_H
#define FEEDERCONTROLTHREAD_H

#include <QThread>
#include "globalflow.h"
#include "globalparams.h"

/* 料位监控模式索引号 */
enum {
    /* 模式1为单料位传感器
     * 有料时开启振动器
     * 无料时关闭振动器 */
    FEEDER_CONTROL_MODE_1 = 0,

    /* 模式2共三个料位传感器(位置为上,中,下)
     * 1.都有料时增加振动量
     * 2.上无料,中有料,下有料时正常下料
     * 3.仅有下有料时减小振动量
     * 4.都无料时关闭振动器 */
    FEEDER_CONTROL_MODE_2,

    /* 模式3为单料位传感器
     * 连续有料时增大产量
     * 无料时正常下料 */
    FEEDER_CONTROL_MODE_3
};

/* 料位传感器索引号 */
enum {
    SENSOR_FIRST_UPPER = 0,
    SENSOR_FIRST_LEFT,
    SENSOR_FIRST_RIGHT,
    SENSOR_SECOND_UPPER,
    SENSOR_SECOND_LEFT,
    SENSOR_SECOND_RIGHT,
    SENSOR_THIRD_UPPER,
    SENSOR_THIRD_LEFT,
    SENSOR_THIRD_RIGHT
};

#define LEVEL_PACKET_SIZE           25  //! 定义料位信息回包的长度
#define LEVEL_SENSOR_NUM            MAX_GROUP_TICK * 3  //! 定义料位传感器理论数量
#define INSTALL_LEVEL_SENSOR_NUM    9   //! 定义实际可安装料位传感器数量

/*!
 * \brief 黑白机型料位监控线程类
 */
class FeederControlThread : public QThread
{
    Q_OBJECT
public:
    explicit FeederControlThread(void);
    ~FeederControlThread();

    void stop(void);                    // 停止读取料位信息
    bool threadIsRunning(void);         // 当前线程是否正在运行
    void checkFeedLevel(void);          // 检查料位是否正常
    void checkByMode1(void);            // 根据模式1监控供料
    void checkByMode2(void);            // 根据模式2监控供料
    void checkByMode3(void);            // 根据模式3监控供料
    void openGroupFeed(int nGroupId);   // 打开某组振动器
    void closeGroupFeed(int nGroupId);  // 关闭某组振动器
    void addGroupFeed(int nGroupId, int nFeeder = 50);  // 增加某组供料量
    void reduceGroupFeed(int nGroupId); // 减小某组供料量
    void setCtrlMode(int nMode);        // 设置料位监控模式
    bool feederIsOn(int nGroupId);      // 当前组振动器是否开启
    char *getFeederInfo(void);          // 获取料位信息
    void setNeedEmitFlag(bool bFlag);   // 设置发送信号标志
    bool checkDetectWait(void);         // 判断检测等待是否符合要求
    bool checkDataIsValid(void);        // 检查数据合法性
    void clearFeederBias(void);         // 清空供料偏置

signals:
    void refreshFeederInfo(void);       // 刷新料位信息信号

public slots:
    void run(void);                     // 开始读取料位信息

private:
    /* 料位监控线程数据 */
    bool m_bIsRunning;
    int m_nMode;
    char m_nFeederInfo[LEVEL_SENSOR_NUM];
    bool m_bNeedEmit;
    char m_nPreInfo[LEVEL_SENSOR_NUM];
    int m_nTimes;
    int m_nFeederBias[MAX_GROUP_TICK];
};

#endif // FEEDERCONTROLTHREAD_H
