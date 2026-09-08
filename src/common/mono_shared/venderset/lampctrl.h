/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        lampctrl.h
 * \brief       黑白机型灯控界面头文件。
 * \date        2015.01.14
 */
#ifndef LAMPCTRL_H
#define LAMPCTRL_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include "common/globalconfig.h"
#include "globalflow.h"
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylcdnumber.h"
#include "common/myinputpanel.h"

/*!
 * \brief 黑白机型灯控界面类
 */
class LampCtrl : public QWidget
{
    Q_OBJECT
public:
    explicit LampCtrl(QWidget *parent = 0);
    
    void createPage(void);          // 创建灯控页面
    void connectSigAndSlt(void);    // 连接灯控页面的信号与槽
    void refreshLocalParams(void);  // 根据全局参数初始化类中的局部参数
    void refreshDisplay(void);      // 刷新页面的显示
    void refreshLampSetDisplay(void);       // 刷新灯控设置页面显示
    void saveAndSendParams(void);           // 保存并下发参数
    void setLampName(void);         // 设置灯的名称

signals:
    void backToHomePageSig();               // 返回主界面信号

public slots:
    /* 灯控页面控件对应槽函数 */
    void onBtnsPressed(int index);          // 响应灯控页面按钮按下事件
    void onApplyBtnPressed(void);           // 响应应用按钮按下事件
    void onBackBtnPressed(void);            // 响应返回按钮按下事件
    void onLcdNumPressed(int index);        // 响应显示数字按下事件

private:
    /* 定义灯控页面对应数据 */
    g_Config *m_config;
    int m_nLamp[MAX_LAMP];
    QSignalMapper *m_sigMapper;
    QSignalMapper *m_lcdSigMap;

    /* 定义灯控页面对应控件 */
    QGroupBox *m_leftGroup;
    QGroupBox *m_rightGroup;
    myPushButton *m_btn[MAX_LAMP];
    myLabel *m_label[MAX_LAMP];
    myLCDNumber *m_lcdNum[MAX_LAMP];
    myPushButton *m_applyBtn;
    myPushButton *m_backBtn;
};

#endif // LAMPCTRL_H
