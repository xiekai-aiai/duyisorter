/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        copymajorparams.h
 * \brief       黑白机型复制重要参数界面头文件。
 * \date        2015.01.14
 */
#ifndef COPYMAJORPARAMS_H
#define COPYMAJORPARAMS_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include "common/mypushbutton.h"
#include "common/myvaluebar.h"
#include "common/mymessagebox.h"
#include "common/globalconfig.h"
#include "globalparams.h"
#include "globalflow.h"

/*!
 * \brief 黑白机型复制重要参数界面类
 */
class CopyMajorParams : public QWidget
{
    Q_OBJECT
public:
    explicit CopyMajorParams(QWidget *parent = 0);

    void createPage(void);          // 创建复制重要参数页面
    void connectSigAndSlt(void);    // 连接页面的信号与槽
    void refreshDisplay(void);      // 刷新页面的显示
    void copyMajorToProfile(void);  // 复制重要参数至目的方案
    void copyColorMajorToProfile(); // 彩色复制重要参数至目的方案
    
signals:
    void goToManageSchemeSig(void); // 返回方案管理页面信号
    
public slots:
    /* 响应页面控件的槽函数 */
    void onApplyBtnPressed(void);   // 响应应用按钮按下事件
    void onBackBtnPressed(void);    // 响应返回按钮按下事件
    void onSelAllBtnPressed(void);  // 响应选择全部按钮按下事件
    void onValueBarChangedSlt(int nValue);      // 响应目的方案索引号变化事件

private:
    /* 复制重要参数页面数据 */
    g_Config *m_config;
    bool m_bSelAll;

    /* 复制重要参数页面控件 */
    myPushButton *m_applyBtn;
    myPushButton *m_backBtn;
    QGroupBox *m_srcGroup;
    QGroupBox *m_desGroup;
    myValueBar *m_valueBar;
    myLabel *m_srcLabel;
    myLabel *m_desLabel;
    myPushButton *m_selAllBtn;
};

#endif // COPYMAJORPARAMS_H
