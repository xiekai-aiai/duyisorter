/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mysettime.h
 * \brief       设置时间控件头文件
 * \date        2015.01.14
 */
#ifndef MYSETTIME_H
#define MYSETTIME_H

#include <QDialog>

#include <QApplication>
#include <sys/time.h>
#include <time.h>

#include "mylabel.h"
#include "mylineedit.h"
#include "myinputpanel.h"
#include "mypushbutton.h"
#include "globalflow.h"
#include "mylanguage.h"

/*!
 * \brief 设置时间控件类
 */
class mySetTime : public QDialog
{
    Q_OBJECT
public:
    explicit mySetTime(QWidget *parent = 0);

    QSignalMapper *timeSignalMapper;
    myLabel       *warning;
    myLabel       *dateLabel[6];
    myLineEdit    *dateLbe[6];
    QDateTimeEdit *dateTimeEdit;
    QSpacerItem   *horizontalSpacer;
    myPushButton  *setDateBtn;
    QGridLayout   *gridLayout;
    QHBoxLayout   *dateHBLayout;
    QSpacerItem   *horizontalSpacer_2;
    int            data[6];
    myPushButton  *nextBtn;
signals:
    
public slots:
    void getTimeIndex(int index);
    QString getTime();
    QString getTime1();
    QString getDateTime();
};

#endif // MYSETTIME_H
