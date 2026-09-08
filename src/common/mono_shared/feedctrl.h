/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        feedctrl.h
 * \brief       黑白机型料位监控界面头文件。
 * \date        2015.01.14
 */
#ifndef FEEDCTRL_H
#define FEEDCTRL_H

#include <QWidget>
#include "globalparams.h"
#include "globalflow.h"
#include "common/mylineedit.h"
#include "common/mylabel.h"
#include "common/mymutex.h"
#include "feedercontrolthread.h"
#include "common/mywid.h"

const int FEED_CTRL_ROW_NUM = 3;
const int FEED_CTRL_COL_NUM = MAX_GROUP_TICK;

/*!
 * \brief 黑白机型料位监控界面类
 */
class FeedCtrl : public QWidget
{
    Q_OBJECT
public:
    explicit FeedCtrl(QWidget *parent = 0);
    void createPage(void);          //! 创建料位监控页面
    void connectSigAndSlt(void);    //! 连接信号与槽
    void refreshDisplay(void);      //! 刷新页面的显示
    void refreshFeedDiplay(void);   //! 刷新料位信息的显示
    void refreshFeedStatus(void);   //! 刷新料位状态
    void refreshSelBtnStatus(void); //! 刷新分次使能按钮状态
    void stopRefresh(void);         //! 停止料位信息的刷新
    
signals:
    
public slots:
    void onMutexIndexChangedSlt(int nIndex);    //! 单选框索引变化
    void onOneKeyFeedBtnPressedSlt(void);   //! 一键放料
    void refreshFeederInfo(void);       //! 刷新料位信息的显示
    void onSelBtnPressed(int nIndex);   //! 分次使能状态变化
    void hideOneKeyFeedSlt();           //! 不使能一键放料按钮
    void showOneKeyFeedSlt();           //! 使能一键放料按钮
private:
    /* 料位监控页面控件 */
    QSignalMapper *m_sigMap;
    myMutex *m_mutex;
    myPushButton *m_oneKeyFeedBtn;
    myLineEdit *m_lineEdit[FEED_CTRL_COL_NUM][FEED_CTRL_ROW_NUM];
    myLabel *m_rowLabel[FEED_CTRL_ROW_NUM];
    myCustomCheckBox *m_selCbx[FEED_CTRL_COL_NUM];
    FeederControlThread *m_thread;

    bool m_bIsFeed;
    myLineEdit  *m_colorLet[3];
    myLabel     *m_colorLbl[3];
};

#endif // FEEDCTRL_H
