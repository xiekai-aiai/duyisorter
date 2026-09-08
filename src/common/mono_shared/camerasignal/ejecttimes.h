/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        ejecttimes.h
 * \brief       黑白机型吹气频率界面头文件。
 * \date        2015.01.14
 */
#ifndef EJECTTIMES_H
#define EJECTTIMES_H

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QStackedWidget>
#include "common/mypushbutton.h"
#include "common/myvaluebar.h"
#include "common/mylabel.h"
#include "common/mymessagebox.h"
#include "mycurve.h"
#include "common/globalconfig.h"
#include "globalflow.h"
#include "globalparams.h"
#include "time.h"
#include "ejecttimesthread.h"

/* 定义吹气次数页面索引号 */
enum {
    SM_EJECT_MAIN_PAGE = 0,
    SM_EJECT_SET_PAGE
};

/* 定义前后视索引号 */
enum {
    SM_EJECT_MAJOR_FRONT = 0,   // 主配前视
    SM_EJECT_MAJOR_REAR,        // 主配后视
    SM_EJECT_ASSIST_FRONT,      // 辅配前视
    SM_EJECT_ASSIST_REAR        // 辅配后视
};

/*!
 * \brief 吹气频率界面类
 */
class ejectTimes : public QWidget
{
    Q_OBJECT
public:
    explicit ejectTimes(QWidget *parent = 0);
    explicit ejectTimes(QSize size, QWidget *parent = 0);

    /* 创建吹气次数界面 */
    void initLocalParams(void);         // 初始化页面数据
    void createMainPage(void);          // 创建吹气次数主页面
    void createMainPageGeneral(void);   // 创建通用机型吹气次数主页面
    void createSetPage(void);           // 创建吹气设置页面
    void connectSigAndSlt(void);        // 连接界面控件的信号与槽
    void refreshDisplay(void);          // 刷新当前页面的显示
    void refreshChuteDisplay(void);     // 刷新滑槽部分页面显示
    void refreshAxisDisplay(void);      // 刷新坐标轴显示
    void stopRefresh(void);             // 停止页面刷新
    void clearShareEjectTimes(void);    // 清空全局吹气次数参数
    void onChuteOrTimeChanged(void);    // 响应滑槽或采样时间变化事件
    void clearEjectTimes(void);         // 清空前后视吹气次数显示
    void refreshEjectTimes(void);       // 刷新前后视吹气次数
    int  getCurPageIndex(void);         // 获取当前切换页的索引号
    void setCurPageIndex(int index);    // 设置当前切换页的索引号
    void feederAutoAdjust(void);        // 供料量设置更新

signals:
    
public slots:
    /* 吹气次数主页面控件对应槽函数 */
    void onEjectSetBtnClicked(void);    // 响应吹气设置按钮按下事件
    void onPlusBtnPressed(void);        // 响应增加按钮按下事件
    void onMinusBtnPressed(void);       // 响应减少按钮按下事件
    void onRcvDataFinishedSlt(void);    // 响应读吹气次数完成事件
    void onValueBarChangedSlt(void);    // 响应采样时间变化事件
    void onModeBtnPressed(void);        // 响应相机模式按钮按下事件
    void onStartWipeSlt(void);          // 响应开始清灰事件

    /* 吹气设置页面槽函数 */
    void onLcdNumsPressed(int index);   // 响应喷阀次数限制变化事件

    /* CF机型新增吹气控件的槽函数2014-11-4 */
    void onEjectFreStatBtnPressed(void);
    void onFeedPercentRangeLeftPressed(void);
    void onFeedPercentRangeRightPressed(void);
    void onEjectFreRangeLeftPressed(void);
    void onEjectFreRangeRightPressed(void);
    void hideEjectSet();

private:
    /* 定义吹气次数页面数据 */
    g_Config *m_config;
    EjectTimesThread *m_thread;
    int m_nOkTimes;
    int m_nCurrentEjectTimes;

    /* 定义吹气次数主页面控件 */
    QStackedWidget *m_stackedWidget;
    QWidget *m_mainPage;
    MyAxis *m_axis;
    myPushButton *m_modeBtn;
    myPushButton *m_plusBtn;
    QLCDNumber *m_lcdNum;
    myPushButton *m_minusBtn;
    myLabel *m_frontLabel;
    myLabel *m_rearLabel;
    QLCDNumber *m_frontTimes;
    QLCDNumber *m_rearTimes;
    myPushButton *m_setBtn;

    /* 定义新增吹气次数主页面控件2014-11-4 */
    myLabel *ejectFreStatLabel;           //吹气次数调节供料量使能按钮标签
    myPushButton *ejectFreStatBtn;       //吹气次数调节供料量使能按钮

    myLabel *m_currentEjectFreqLabel;     //当前页面显示通道之当前吹气频率
    QLCDNumber *m_currentEjectFreq;

    myLabel *feedPercentRangeLabel;   //供料量范围标签
    myLCDNumber *feedPercentRangeLeft;   //供料量范围下限
    myLCDNumber *feedPercentRangeRight;  //供料量范围上限

    myLabel *ejectFreRangeLabel;      //吹气频率范围标签
    myLCDNumber *ejectFreRangeLeft;      //吹气频率范围下限
    myLCDNumber *ejectFreRangeRight;     //吹气频率范围上限

    /* 定义吹气设置页面控件 */
    QWidget *m_setPage;
    myLabel *m_limitLabel[MAX_GROUP_TICK];
    myLCDNumber *m_limitNum[MAX_GROUP_TICK];
    QSignalMapper *m_sigMapper;

    int typeMachine;    //0：米，1：杂粮，2：茶叶
};

#endif // EJECTTIMES_H
