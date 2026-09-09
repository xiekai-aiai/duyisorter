/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mythread.cpp
 * \brief       全局线程源文件
 *              包含更新系统状态及读取系统报警线程。
 * \date        2015.01.14
 */
#include <QDebug>
#include "mythread.h"
#include <QTimer>
#include <QtGlobal>
#include "unilog.h"

 /* ****************************************************************************
  *
  *                        系统时钟运行和清灰处理线程
  *
  *******************************************************************************/
updateStatusThread::updateStatusThread()
{

}

updateStatusThread::~updateStatusThread()
{

}

/*!
 * \brief updateStatusThread::checkWipe
 * 检查清灰
 */
void updateStatusThread::checkWipe()
{
    bool trigger = false;
    int mode = TRIGGER_MANUAL;

    //! 清灰时间合法性校验
    int nInterval = 60 * struCnfp.struGroupCtrl[0].nWipeInterval + struCnfp.struGroupCtrl[0].nWipeDuration;
    if (nInterval <= 0)
    {
        return;
    }

    //! 自动清灰判断
    if (struGsh.nCounter % nInterval == 0 && struGsh.bFlagAutowipe == 1)
    {
        trigger = true;
        mode = TRIGGER_TIMER;
    }

    //! 手动清灰判断
    if (struGsh.bFlagManuwipe == 1)
    {
        trigger = true;
        struGsh.bFlagManuwipe = 0;
    }

    //! 触发清灰操作
    if (trigger)
    {
        emit sWipeStart(mode);
    }
}

/*!
 * \brief updateStatusThread::checkWipeWind
 * 检查清灰气帘
 */
void updateStatusThread::checkWipeWind()
{
    //! 米机气帘吹风(MC机型外)
}

/*!
 * \brief updateStatusThread::run
 * 系统时钟运行处理线程入口
 */
void updateStatusThread::run(void)
{
    QTimer* timer = new QTimer();

    /* 关联定时器计满信号和相应的槽函数 */
    connect(timer, SIGNAL(timeout()), this, SLOT(systemClockRun()), Qt::DirectConnection);
    timer->start(1000);
    exec();
}

/*!
 * \brief updateStatusThread::systemClockRun
 * 定时中断响应处理
 */
void updateStatusThread::systemClockRun()
{
    //! 系统时钟计时操作
    struGsh.nCounter++;
    struCnfg.nCounter++;
    struGsh.nBacklightCounter++;
    if (struGsh.bStatFeed == 1)
    {
        struGsh.nFeedCounter++;
        struCnfg.nFeedCounter++;
    }

    //! 背光时间超时处理
    if (struGsh.nBacklightCounter >= BACKLIGHT_TIMEOUT
        && struGsh.nBacklightStat == 1)
    {
        myFlow.setTsBackLight(0);
    }

    //! 触发更新系统时间操作
    emit sUpdateSystemTime();

    //! 清灰气刷触发流程处理
    checkWipe();

    //! 清灰气帘触发流程处理
    checkWipeWind();

    //每隔三分钟统计一次
    if (struGsh.nCounter % 180 == 0)
    {
        getThroughtPutParam();
    }
}

void updateStatusThread::getThroughtPutParam()
{

    quint64 m_throughLocalput = 0;
    quint64 m_throughAllput = 0;
    quint64  pixelNum = 0;
    quint64  badPointNum = 0;
    float    realDirtPer = 0;
    QByteArray materialStatistic;
    materialStatistic.resize(32);
    materialStatistic.fill(0);
    int nUnitAddr;
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            // 目前只采集前视物料点信息
            if (nUnitAddr % 2 == 0)
            {
                pixelNum = 0;
                badPointNum = 0;
                realDirtPer = 0;
                materialStatistic.resize(32);
                materialStatistic.fill(0);
                m_throughAllput = struCnfg.nThroughtPut[nUnitAddr / 2];
                m_throughLocalput = 0;

                MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_UPLOAD_ALL, UNIT, 0, nUnitAddr, 0, 0, 0, 0, 1, 1);
                materialStatistic.remove(0, 10);
                materialStatistic.truncate(materialStatistic.size() - 6);
                for (int j = 0; j < 8; j++)
                {
                    printf("%02x ", materialStatistic.at(j));
                    pixelNum += ((quint64)materialStatistic.at(j)) << ((7 - j) * 8);
                }
                for (int j = 8; j < 16; j++)
                {
                    printf("%02x ", materialStatistic.at(j));
                    badPointNum += ((quint64)materialStatistic.at(j)) << ((15 - j) * 8);
                }
                if (pixelNum == 0)
                {
                    realDirtPer = 0;
                }
                else
                {
                    realDirtPer = (float)badPointNum / (float)pixelNum;
                }
                if (pixelNum == 0)
                {
                    m_throughLocalput = 0;
                }
                else
                {
                    m_throughLocalput = struCnfg.aFactor * pixelNum + struCnfg.bFactor;
                }
                struGsh.nThroughtPut[nUnitAddr / 2] = m_throughLocalput;

                struCnfg.nThroughtPut[nUnitAddr / 2] = m_throughAllput + m_throughLocalput;

                printf("%lld...%lld...%f...%lld...%lld\n", pixelNum, badPointNum, realDirtPer, m_throughLocalput, m_throughAllput);
                fflush(0);
            }
            else
            {
                continue;
            }
        }
    }


}
/* ****************************************************************************
 *
 *                        获取状态信息线程
 *
 *******************************************************************************/
getStatusThread::getStatusThread()
{

}

/*!
 * \brief getStatusThread::run
 *        获取状态信息线程入口
 */
void getStatusThread::run(void)
{
    while (this->isRunning())
    {
        if (struGsh.nCounter % 7 == 0)
        {
            //! 报警状态查询
            getAlarmStatus();

            //! 温度状态检测
            getTemperatureStatus();

            //! 从设备通信状态检测
            myFlow.checkSlaveCommStat();

            //! 异常状态处理
            handleAbnormalStatus();
        }
        sleep(1);
    }
    exec();
}

/*!
 * \brief getStatusThread::checkTemperatureStatus
 * 检测温度传感器反馈值
 */
void getStatusThread::getTemperatureStatus()
{
    char buf[64] = { 0 };
    float temp = 0;

    // 控制板地址
    int sModuleAddr = 0;

    // 温度状态查询
    MySerial.com2Write(CMD_CTRL_TEMPERATURE, CTRL_GENERAL, sModuleAddr, 0, 0, 0, 0, 0, 3);
    msleep(500);
    if (MySerial.com2Read(buf, 28) == 28
        && buf[0] == 0xAA
        && buf[1] == 0xAA
        && buf[2] == 0xAA
        && buf[3] == 0xAA)
    {

        // 计算所有温度传感器反馈最大值
        int max = 0;
        int len = buf[8] * 256 + buf[9];
        for (int i = 0; i < len; i++)
        {
            struGsh.nSensorTemperature[i] = buf[10 + i];
            max = qMax(max, (int)buf[10 + i]);
        }
        // 最大反馈值转化 (仅检测高温)
        temp = 0.448 * max - 0.824;
        if (temp < 0)
        {
            temp = 0;
        }
    }

    struGsh.nTemperatureMax = temp;
}

/*!
 * \brief getStatusThread::getAlarmStatus
 * 检测报警状态反馈值
 */
void getStatusThread::getAlarmStatus()
{
    char buf[64] = { 0 };
    // 控制板模块
    int sModuleAddr = 0;
    // 报警状态查询
    MySerial.com2Write(CMD_CTRL_ALARM, CTRL_GENERAL, sModuleAddr, 0, 0, 0, 0, 0xAA, 3);
    msleep(10);
    if (MySerial.com2Read(buf, 18) == 18
        && buf[0] == 0xAA
        && buf[1] == 0xAA
        && buf[2] == 0xAA
        && buf[3] == 0xAA)
    {
        //! 皮带
        struGsh.bStatBelt[0] = buf[10] & 1;
        struGsh.bStatBelt[1] = (buf[10] & 2) / 2;
        //! 气压
        struGsh.bAlarmPressure = buf[11] & 3; //buf[11]的bit0=1（气压不足）  bit1=1(气压报警)
        //! 风机
        struGsh.bAlarmBlower = (buf[11] & 8) / 8;
        //! 料位
        struGsh.nFeederControlLevel = ((buf[10] & 0xfc) << 1) | (buf[11] >> 5);
    }
    else
    {//串口数据异常
        struGsh.bStatBelt[0] = 0;
        struGsh.bStatBelt[1] = 0;
        struGsh.bAlarmPressure = 0;
        struGsh.bAlarmBlower = 0;
        struGsh.nFeederControlLevel = 0;
    }
}

/*!
 * \brief getStatusThread::getLocalIP
 * 获取IP地址
 */
QString getStatusThread::getLocalIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, list)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}

/*!
 * \brief getStatusThread::updateSystemStatus
 * 系统状态记录
 */
void getStatusThread::updateSystemStatus()
{
    QString ip = getLocalIP();
    QString fileName;

    if (ip.length() == 0)
    {
        fileName = QString("/tmp/%1.stat").arg("192.168.0.114");
    }
    else
    {
        fileName = QString("/tmp/%1.stat").arg(ip);
    }
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << "feed\t" << struGsh.bStatFeed << "\n";
    out << "eject\t" << struGsh.bStatEjectFpga << "\n";
    out << "Pressure\t" << struGsh.bAlarmPressure << "\n";
    out << "Blower\t" << struGsh.bAlarmBlower << "\n";
    out << "Ejector\t" << struGsh.bAlarmEjector << "\n";
    out << "Belt0\t" << struGsh.bStatBelt[0] << "\n";
    out << "Belt1\t" << struGsh.bStatBelt[1] << "\n";

    file.close();
}

/*!
 * \brief getStatusThread::isAlarmStatusChanged
 * 检测报警状态是否变化
 */
bool getStatusThread::isAlarmStatusChanged(void)
{
    int nAlarmStatus = 0;
    // 报警状态遍历
    nAlarmStatus |= (struGsh.bAlarmPressure << ALARM_ID_PRESSURE);
    nAlarmStatus |= (struGsh.bAlarmBlower << ALARM_ID_BLOWER);
    nAlarmStatus |= ((struGsh.bStatBelt[0] || struGsh.bStatBelt[1]) << ALARM_ID_BELT);
    nAlarmStatus |= ((struGsh.nAlarmLevel != ALARM_LEVEL_NULL) << ALARM_ID_LEVEL);
    nAlarmStatus |= (struGsh.nCommStat << ALARM_ID_COMM);

    // 判断是否发生变化
    if (nAlarmStatus != struGsh.nAlarmStatus)
    {
        writeAlamLog();
        struGsh.nAlarmStatus = nAlarmStatus;
        struGsh.bAlarmChanged = true;
    }
    else
    {
        struGsh.bAlarmChanged = false;
    }

    return struGsh.bAlarmChanged;
}

/*!
 * \brief getStatusThread::handleAbnormalStatus
 * 处理系统异常状态
 */
void getStatusThread::handleAbnormalStatus()
{
    //! 如系统报警状态变化, 则发送报警状态统计信号
    if (isAlarmStatusChanged())
    {
        if (struCnfg.nStatisticEnable == 1)
        {
            emit alarmReady();
        }
    }

    //! 温度监控处理
    if (struCnfg.nAlarmEnable[ALARM_ID_TEMPERATURE] == 1
        && struGsh.nTemperatureMax >= struCnfg.nTemperatureThreshold)
    {
        //! 关闭供料
        if (struGsh.bStatFeed == 1)
        {
            emit closeFeederSig(TRIGGER_ALARM);
            myFlow.sleep(5);
        }
    }


    //! 常规报警处理 - 气压、风机
    if ((struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] == 1 && ((struGsh.bAlarmPressure & 0x02) != 0))
        || (struCnfg.nAlarmEnable[ALARM_ID_BLOWER] == 1 && struGsh.bAlarmBlower == 1))
    {
        if (struGsh.bStatFeed == 1)
        {
            // 报警解除恢复供料标识
            struGsh.bAlarmCloseFeed = true;
            emit closeFeederSig(TRIGGER_ALARM);
            if (struGsh.bStatEjectFpga == 1)
            {
                struGsh.bAlarmCloseEject = true;
                struGsh.bStatEjectFpga = 0;
                //                myFlow.ejectOff();
            }
        }
    }
    else if (struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] == 0 && (struGsh.bAlarmPressure & 0x02) != 0)
    {
        if (struGsh.bStatFeed == 0)
        {
            if (struGsh.bAlarmCloseFeed)
            {
                struGsh.bAlarmCloseFeed = false;
                emit openFeederSig(TRIGGER_ALARM);
            }
        }
        if (struGsh.bStatFeed == 1 && struGsh.bStatEjectFpga == 1)
        {
            struGsh.bAlarmCloseEject = true;
            struGsh.bStatEjectFpga = 0;
            myFlow.ejectOff();
        }
    }
    else
    {
        if (struGsh.bAlarmCloseEject)
        {
            struGsh.bAlarmCloseEject = false;
            struGsh.bStatEjectFpga = 1;
            myFlow.ejectorOn();
        }
        if (struGsh.bStatFeed == 0)
        {
            if (struGsh.bAlarmCloseFeed)
            {
                struGsh.bAlarmCloseFeed = false;
                emit openFeederSig(TRIGGER_ALARM);
            }
        }
    }

    //气压报警弹框提醒
    if (((struGsh.bAlarmPressure & 0x02) != 0) && (struGsh.bStatFeed == 1
        || (struGsh.bStatFeed == 0 && struGsh.bAlarmCloseFeed == true)))
    {//开下料报警、应报警关闭下料两种情况要弹框提醒
        if (m_nPreAlarmPressure != struGsh.bAlarmPressure)
        {  // 新的报警
            struCnfg.nPressureAlarmCount++;//连续气压报警次数统计
            struGsh.nTotalAlarmPreTimes++;
        }
        emit showPressureAlarmMsgSig(true);
    }
    else
    {
        emit showPressureAlarmMsgSig(false);
    }
    if (struGsh.bStatFeed == 1)
    { //在开下料状态下才更新上次气压状态
        m_nPreAlarmPressure = struGsh.bAlarmPressure;
    }
}

//记录报警日志
void getStatusThread::writeAlamLog()
{
    //气压报警状态改变
    QString str;
    if (struGsh.bAlarmPressure != (struGsh.nAlarmStatus & 0x11))
    {
        if ((struGsh.bAlarmPressure & 0x02) != 0)
        {
            str = myLan.abnormal;
        }
        else if ((struGsh.bAlarmPressure & 0x01) != 0)
        {
            str = myLan.pressure_not_enough;
        }
        else
        {
            str = myLan.normal;
        }
        LOG_INFO_STM("body:" << myLan.pressure_alarm.toStdString() << " status:" << str.toStdString());
    }

    //风机报警状态改变
    if (struGsh.bAlarmBlower != ((struGsh.nAlarmStatus >> ALARM_ID_BLOWER) & 0x01))
    {
        if (struGsh.bAlarmBlower != 0)
        {
            str = myLan.abnormal;
        }
        else
        {
            str = myLan.normal;
        }
        LOG_INFO_STM("body:" << myLan.blower_alarm.toStdString() << " status:" << str.toStdString());
    }

    //输送带状态改变
    if ((struGsh.bStatBelt[0] || struGsh.bStatBelt[1]) != ((struGsh.nAlarmStatus >> ALARM_ID_BELT) & 0x01))
    {
        if (struGsh.bStatBelt[0] == 0)
        {
            str = myLan.off;
        }
        else
        {
            str = myLan.on;
        }
        LOG_INFO_STM("body:" << myLan.belt_1_status.toStdString() << " status:" << str.toStdString());

        if (struGsh.bStatBelt[1] == 0)
        {
            str = myLan.off;
        }
        else
        {
            str = myLan.on;
        }
        LOG_INFO_STM("body:" << myLan.belt_2_status.toStdString() << " status:" << str.toStdString());
    }

    //料位报警标志改变
    if ((struGsh.nAlarmLevel != 0) != ((struGsh.nAlarmStatus >> ALARM_ID_LEVEL) & 0x01))
    {
        QString strStatus;
        if (struGsh.nAlarmLevel != 0)
        {
            strStatus = myLan.abnormal;
        }
        else
        {
            strStatus = myLan.normal;
        }
        switch (struGsh.nAlarmLevel)
        {
        case ALARM_LEVEL_EMPTY:
            str = QString("%1: %2").arg(myLan.level_empty_alarm).arg(strStatus);
            break;
        default:
            str = QString("%1: %2").arg(myLan.feeder_level).arg(strStatus);
            break;
        }
        LOG_INFO_STM("body:" << str.toStdString());
    }

    //通信异常报警
    if (struGsh.nCommStat != ((struGsh.nAlarmStatus >> ALARM_ID_COMM) & 0x01))
    {
        if (struGsh.nCommStat == 0)
        {
            str = myLan.normal;
        }
        else
        {
            str = myLan.abnormal;
        }
        LOG_INFO_STM("body:" << myLan.communication_alarm.toStdString() << " status:" << str.toStdString());
    }
}
