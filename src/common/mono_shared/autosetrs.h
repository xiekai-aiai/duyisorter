/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        autosetrs.h
 * \brief       黑白机型一键自动界面头文件。
 * \date        2015.01.14
 */
#ifndef AUTOSETRS_H
#define AUTOSETRS_H

#include <QApplication>
#include <QWidget>
#include <QSignalMapper>
#include <QStackedWidget>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/myvaluebar.h"
#include "common/mymessagebox.h"
#include "mycurve.h"
#include "globalparams.h"
#include "globalflow.h"

/*!
 * \brief 黑白机型一键自动界面类
 */
class autoSetPage : public QWidget
{
    Q_OBJECT
public:
    explicit autoSetPage(QWidget *parent = 0);
    void CreatAutoSetMainWidget(void);          // 创建一键智能主页面
    void CreateCameraSingalWidget(void);        // 创建相机信号页面
    void refreshCamSigDisplay(void);            // 刷新相机信号页面的显示
    void refreshValueBarDisplay(void);          // 刷新背景值的显示
    void resetBkgLampIndex(void);               // 重置背景灯索引

    void adjustSens(void);
    void resumeSens(void);
    void calibrateGain(int nFrontMode, int nRearMode);
    void lightAuto(int unit,int target);
    void assistLightAuto(int nUnit, int nTarget);   //! 辅配自动校正
    void adjustBackground(int index);
    void setFlagByMode(int &nFront, int &nRear);    // 根据色选模式设置前后视背景是否校准标志位
    
signals:
    void goToHomePageSig();                     // 返回主界面
    
public slots:
    void onCancelBtnClickedSlt(void);
    void onAdjustStartBtnClickedSlt(void);
    void onCameraSingalBtnClickedSlt(void);
    void onReturnMainBtnClickedSlt(void);
    void onValueBarChangedSlt(int nValue);      // 响应背景值变化事件
    void onBoardNumChangedSlt(void);            // 响应相机板号变化事件

private:
    enum{
        AUTO_SET_MAIN_PAGE,         // 一键智能主页面
        CAMERA_SINGAL_PAGE          // 相机信号页面
    };

    QStackedWidget *stackedWidget;
    QWidget        *autoSetMainPage;
    QWidget        *cameraSingalPage;

    // 设置主页面控件
    myPushButton   *adjustStartBtn;
    myPushButton   *cameraSingalBtn;
    myPushButton   *autoSetBackBtn;

    // 设置相机信号页面控件
    myPushButton   *returnMainBtn;
    MyCurve        *autoSetCurve;
    myValueBar     *backgroundValueBar;

    int  buttonEnableControl;       // 主页面使能状态控制变量
    int feederTemp[10];
    int sensTemp[MAX_GROUP_IDTNTIFY];
    int sensSepatateTemp[MAX_UNIT][2];
    g_Config *m_config;
    bool m_bIsFirst;
    int m_nFrontBkgLamp;
    int m_nRearBkgLamp;
};

#endif // AUTOSETRS_H
