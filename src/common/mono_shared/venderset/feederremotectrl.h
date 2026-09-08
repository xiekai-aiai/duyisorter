/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        feederctrlenable.h
 * \brief       黑白机型振动器外接控制界面头文件。
 * \date        2015.01.14
 */
#ifndef FEEDERREMOTECTRL_H
#define FEEDERREMOTECTRL_H

#include <QWidget>
#include "common/mypushbutton.h"
#include "common/mymutex.h"
#include "globalparams.h"
#include "globalconfig.h"

/*!
 * \brief 黑白机型振动器外接控制界面类
 */
class FeederRemoteCtrl : public QWidget
{
    Q_OBJECT
public:
    explicit FeederRemoteCtrl(QWidget *parent = 0);

    void createPage(void);              // 创建页面
    void connectSigAndSlt(void);        // 连接页面中的信号与槽
    void refreshDisplay(void);          // 刷新页面的显示
    
signals:
    void backToHomePageSig();           // 返回上一级页面信号
    
public slots:
    void onOkBtnPressed(void);          // 确定
    void onCancelBtnPressed(void);      // 取消
    void onMutexIndexChangedSlt(int nIndex);        // 更改单选框
    
private:
    int m_nMode;
    g_Config *m_config;

    myPushButton *m_okBtn;
    myPushButton *m_cancelBtn;
    myMutex *m_mutex;
};

#endif // FEEDERREMOTECTRL_H
