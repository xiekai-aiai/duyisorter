/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mythread.h
 * \brief       全局线程头文件
 *              包含更新系统状态及读取系统报警线程。
 * \date        2015.01.14
 */
#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "globalparams.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include "myuploader.h"

/*!
 * \brief 系统状态更新线程，即在系统运行记时，更新系统时间，自动清灰处理
 */
class updateStatusThread : public QThread
{
    Q_OBJECT

public:
   updateStatusThread();
   ~updateStatusThread();

   void checkWipe(void);        //! 检查清灰气刷
   void checkWipeWind(void);    //! 检查清灰气帘
   void getThroughtPutParam();

signals:
   void sWipeStart(int trigger);
   void sUpdateSystemTime();

public slots:
   void systemClockRun();

protected:
    QTimer *timer;
    void run(void);
    bool threadIsRunning;
};

/*!
 * \brief 读取报警信息线程
 */
class getStatusThread : public QThread
{
    Q_OBJECT

public:
   getStatusThread();
   ~getStatusThread(){}
   void updateSystemStatus();
   QString getLocalIP();

protected:
    void run(void);

signals:
    void closeFeederSig(int trigger);      // 关闭振动器信号
    void openFeederSig(int trigger);       // 开启振动器信号
    void showPressureAlarmMsgSig(bool isShow);
    void alarmReady(void);          // 报警信号

public:
   void getTemperatureStatus(void); // 检查温度状态
   void getAlarmStatus(void);       // 检查报警状态
   bool isAlarmStatusChanged(void);   // 报警状态是否变化
   void handleAbnormalStatus(void);
   void writeAlamLog();
private:
   int m_nPreAlarmPressure;
};


#endif // MYTHREAD_H
