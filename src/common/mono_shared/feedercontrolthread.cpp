/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        feedercontrolthread.cpp
 * \brief       黑白机型料位监控线程源文件。
 * \date        2015.01.14
 */
#include "feedercontrolthread.h"
#include "unilog.h"

 /************
  *
  * 料位监控线程
  *
  ****************************/
FeederControlThread::FeederControlThread()
{
    m_bIsRunning = false;
    m_bNeedEmit = false;
    m_nTimes = 0;
    for (int i = 0; i < LEVEL_SENSOR_NUM; i++)
    {
        m_nPreInfo[i] = 0;
    }

    m_nMode = FEEDER_CONTROL_MODE_1;

    clearFeederBias();
}

FeederControlThread::~FeederControlThread()
{
}

/* 开始读取料位信息 */
void FeederControlThread::run()
{
    m_nMode = struCnfg.nFeederCtrlMode;
    m_bIsRunning = true;
    while (m_bIsRunning)
    {
        /* 判断振动器是否已开启 */
        if (struGsh.bStatFeed == 1
            && struGsh.bStatFeedCtrlEn == 1)
        {
            checkFeedLevel();
        }
        else
        {
            struGsh.nAlarmLevel = ALARM_LEVEL_NULL;
            for (int i = 0; i < MAX_GROUP_TICK; i++)
            {
                struGsh.nAlarmLevelGroup[i] = ALARM_LEVEL_NULL;
            }
            clearFeederBias();
        }

        sleep(5);
    }
}

/* 停止读取料位信息 */
void FeederControlThread::stop()
{
    m_bIsRunning = false;
}

/* 当前线程是否正在运行 */
bool FeederControlThread::threadIsRunning()
{
    return m_bIsRunning;
}

/* 检查料位是否正常 */
void FeederControlThread::checkFeedLevel()
{
    int ret;
    char p[LEVEL_PACKET_SIZE] = { 0 };
    MySerial.com2Write(CMD_CTRL_FEED_LEVEL, CTRL_GENERAL, 0, 0, 0, 0, 0, 0xAA, 3);
    msleep(10);
    ret = MySerial.com2Read(p, LEVEL_PACKET_SIZE);

    if (ret == LEVEL_PACKET_SIZE
        && p[0] == 0xAA && p[1] == 0xAA
        && p[2] == 0xAA && p[3] == 0xAA)
    {
        /* 1.保存控制板料位信息 */
        for (int i = 0; i < LEVEL_SENSOR_NUM; i++)
        {
            m_nFeederInfo[i] = p[10 + i];
            qDebug("INFO: Feeder[%d] = %d", i, m_nFeederInfo[i]);
        }

        /* 2.若不符合检测等待要求立即退出 */
        if (!checkDetectWait())
        {
            goto UPDATE;
        }

        /* 3.根据供料监控模式设置振动器 */
        switch (m_nMode)
        {
        case FEEDER_CONTROL_MODE_1:
            checkByMode1();
            break;
        case FEEDER_CONTROL_MODE_2:
            checkByMode2();
            break;
        case FEEDER_CONTROL_MODE_3:
            checkByMode3();
            break;
        default:
            break;
        }

        /* 4.根据标志确定是否发送信号 */
    UPDATE:
        if (m_bNeedEmit)
        {
            emit refreshFeederInfo();
        }
    }
}

/* 根据模式1监控供料 */
void FeederControlThread::checkByMode1()
{
    /* 1.保存当前产量 */
    int nFeeder[10] = { 0 };
    for (int i = 0; i < 10; i++)
    {
        nFeeder[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
    }
    /* 2.根据料位情况设置料位 */
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        int nAlarm = ALARM_LEVEL_NULL;
        if (struCnfg.nFeederCtrlTickEn[i] == 1)
        {
            /* 若料位为高则打开振动器，反之则关闭振动器(目前仅使用上部传感器) */
            if (m_nFeederInfo[3 * i] == 0)
            {
                openGroupFeed(i);
            }
            else
            {
                nAlarm = ALARM_LEVEL_EMPTY;
                closeGroupFeed(i);
            }
        }
        struGsh.nAlarmLevelGroup[i] = nAlarm;
    }

    struGsh.nAlarmLevel = 0;
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        if (struGsh.nAlarmLevelGroup[i] == ALARM_LEVEL_EMPTY)
        {
            struGsh.nAlarmLevel = 1;
        }
    }

    myFlow.resetFeeder(0);
    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal / 2; i++)
    {
        LOG_INFO_STM("idx:" << i << " FeederStatus:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
    }

    /* 3.恢复产量 */
    for (int i = 0; i < 10; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = nFeeder[i];
    }
}

/* 根据模式2监控供料 */
void FeederControlThread::checkByMode2()
{
    int nFeeder[10] = { 0 };

    /* 1.保存当前产量 */
    for (int i = 0; i < 10; i++)
    {
        nFeeder[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
    }

    /* 2.根据料位情况设置料位 */
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        int nAlarm = ALARM_LEVEL_NULL;
        if (struCnfg.nFeederCtrlTickEn[i] == 1)
        {
            int up = m_nFeederInfo[3 * i];
            int mid = m_nFeederInfo[3 * i + 1];
            int down = m_nFeederInfo[3 * i + 2];
            if (down == 1)
            { //下无料
                closeGroupFeed(i);
                nAlarm = ALARM_LEVEL_EMPTY;
            }
            else
            { //下有料
                if (up == 1 && mid == 1)
                {
                    /* 仅下有料则减小产量 */
                    openGroupFeed(i);
                    reduceGroupFeed(i);
                }
                else if (up == 1 && mid == 0)
                {
                    /* 若中有料则开启振动器 */
                    openGroupFeed(i);
                }
                else if ((up == 0 && mid == 1) || (up == 0 && mid == 0))
                {
                    openGroupFeed(i);
                    addGroupFeed(i);
                }
            }
        }
        struGsh.nAlarmLevelGroup[i] = nAlarm;
    }

    struGsh.nAlarmLevel = 0;
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        if (struGsh.nAlarmLevelGroup[i] == ALARM_LEVEL_EMPTY)
        {
            struGsh.nAlarmLevel = 1;
        }
    }

    myFlow.resetFeeder(0);
    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal / 2; i++)
    {
        LOG_INFO_STM("idx:" << i << " FeederStatus:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
    }

    /* 3.恢复产量 */
    for (int i = 0; i < 10; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = nFeeder[i];
    }
}

/*!
 * \brief 根据模式3监控供料
 */
void FeederControlThread::checkByMode3()
{
    int nFeeder[10] = { 0 };
    /* 1.保存当前产量 */
    for (int i = 0; i < 10; i++)
    {
        nFeeder[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
    }

    /* 2.根据料位情况设置料位 */
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        int nAlarm = ALARM_LEVEL_NULL;
        openGroupFeed(i);
        if (struCnfg.nFeederCtrlTickEn[i] == 1)
        {
            if (m_nFeederInfo[3 * i] == 0)
            {
                //! 有料则逐渐增大产量
                m_nFeederBias[i] += 10;
                if (m_nFeederBias[i] > 150)
                {
                    m_nFeederBias[i] = 150;
                }
                addGroupFeed(i, m_nFeederBias[i]);
            }
            else
            {
                //! 无料正常下料
                m_nFeederBias[i] = 0;
                nAlarm = ALARM_LEVEL_EMPTY;
            }
        }
        struGsh.nAlarmLevelGroup[i] = nAlarm;
    }

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++)
    {
        if (struGsh.nAlarmLevelGroup[i] == ALARM_LEVEL_EMPTY)
        {
            struGsh.nAlarmLevel = 1;
        }
    }
    myFlow.resetFeeder(0);

    /* 3.恢复产量 */
    for (int i = 0; i < 10; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = nFeeder[i];
    }
}

/* 打开某组振动器 */
void FeederControlThread::openGroupFeed(int nGroupId)
{
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitCount;
    int nFeederStart = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[0] / 2;
    int nFeederEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[nUnitTotal - 1] / 2;

    for (int i = nFeederStart; i <= nFeederEnd; i++)
    {
        struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
    }
}

/* 关闭某组振动器 */
void FeederControlThread::closeGroupFeed(int nGroupId)
{
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitCount;
    int nFeederStart = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[0] / 2;
    int nFeederEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[nUnitTotal - 1] / 2;

    for (int i = nFeederStart; i <= nFeederEnd; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = 0;
        struCnfp.struGroupCtrl[0].nFeederEnable[i] = 0;
    }
}

/* 增加某组供料量 */
void FeederControlThread::addGroupFeed(int nGroupId, int nFeeder)
{
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitCount;
    int nFeederStart = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[0] / 2;
    int nFeederEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[nUnitTotal - 1] / 2;

    for (int i = nFeederStart; i <= nFeederEnd; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = (int)((double)struCnfp.struGroupCtrl[0].nFeeder[i] + 50);
    }
}

/* 减小某组供料量 */
void FeederControlThread::reduceGroupFeed(int nGroupId)
{
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitCount;
    int nFeederStart = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[0] / 2;
    int nFeederEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[nUnitTotal - 1] / 2;

    for (int i = nFeederStart; i <= nFeederEnd; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = (int)((double)struCnfp.struGroupCtrl[0].nFeeder[i] - 50);
    }
}

/* 设置料位监控模式 */
void FeederControlThread::setCtrlMode(int nMode)
{
    m_nMode = nMode;
}

/* 当前组振动器是否开启 */
bool FeederControlThread::feederIsOn(int nGroupId)
{
    int nFeeder = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[nGroupId].nUnitId[0] / 2;
    if (struCnfp.struGroupCtrl[0].nFeederEnable[nFeeder] == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* 获取料位信息 */
char* FeederControlThread::getFeederInfo()
{
    return m_nFeederInfo;
}

/* 设置发送信号标志 */
void FeederControlThread::setNeedEmitFlag(bool bFlag)
{
    m_bNeedEmit = bFlag;
}

/* 判断检测等待是否符合要求 */
bool FeederControlThread::checkDetectWait()
{
    bool bIsOk = true;
    for (int i = 0; i < LEVEL_SENSOR_NUM; i++)
    {
        if (m_nFeederInfo[i] != m_nPreInfo[i])
        {
            m_nTimes = 0;
            bIsOk = false;
            break;
        }
    }

    if (bIsOk)
    {
        m_nTimes++;
    }

    if (m_nTimes >= struCnfg.nFeederCtrlWaitTime / 5)
    {
        bIsOk = true;
        m_nTimes = 0;
    }
    else
    {
        bIsOk = false;
    }

    /* 赋值前一阶段状态 */
    for (int i = 0; i < LEVEL_SENSOR_NUM; i++)
    {
        m_nPreInfo[i] = m_nFeederInfo[i];
    }

    return bIsOk;
}

/*!
 * \brief 清空供料偏置
 */
void FeederControlThread::clearFeederBias()
{
    for (int i = 0; i < MAX_GROUP_TICK; i++)
    {
        m_nFeederBias[i] = 0;
    }
}
