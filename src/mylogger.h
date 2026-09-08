/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylogger.h
 * \brief       全局日志头文件
 * \date        2015.04.16
 */

#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <QThread>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QSettings>
#include <QString>
#include <QObject>
#include <QTime>
#include <QCoreApplication>

#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"
#include "log4qt/propertyconfigurator.h"
#include "log4qt/rollingfileappender.h"
#include "log4qt/helpers/factory.h"
#include "log4qt/appender.h"
#include "log4qt/layout.h"
#include "log4qt/level.h"
#include "log4qt/log4qt.h"
#include "log4qt/logmanager.h"
#include "log4qt/patternlayout.h"
#include "log4qt/logmanager.h"
#include "log4qt/appenderskeleton.h"
#include "log4qt/consoleappender.h"
#include "qfile.h"
#include "qiodevice.h"
#include "globalparams.h"

/*!
 * \brief 全局日志类
 */
class MyLogger : public QObject           // 必须从QObject继承，否则不能用tr("")方式显示文字
{
    Q_OBJECT

public:
    explicit MyLogger(QObject *parent = 0);
    ~MyLogger();

    void configMyLogger();            // 设置应用程序通用日志服务，此函数只对应通用的日志记录mylog
    void configQtLogger();            // 设置Qt的日志服务，此函数只对应Qt的日志记录qtlog

    void PrintLog(Log4Qt::Logger *logger, int prio, const char *fmt, ...);
    void PrintLog(Log4Qt::Logger *logger, int prio, QString qStr);

    /* 上位机程序通用的日志记录方法由以下函数提供 */
    void info(int type, const char *fmt, ...);    //信息级别
    void info(int type, QString qStr);

    void debug(const char *fmt, ...);   //调试级别
    void debug(QString qStr);

    void warn(const char *fmt, ...);    //告警级别
    void warn(QString qStr);

    void error(const char *fmt, ...);   //错误级别
    void error(QString qStr);

    void fatal(const char *fmt, ...);   //崩溃错误级别
    void fatal(QString qStr);

    void trace(const char *fmt, ...);   //跟踪信息级别
    void trace(QString qStr);

private:
    Log4Qt::Logger *myLog;              // 自定义的通用日志服务
    Log4Qt::Logger *qtLog;
};
extern MyLogger *myLog;
#endif // MYLOGGER_H
