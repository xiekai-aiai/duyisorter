/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        paramsetalone.h
 * \brief       黑白机型独立设置界面头文件。
 * \date        2015.01.14
 */
#ifndef PARAMSETALONE_H
#define PARAMSETALONE_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include "common/mymutex.h"
#include "common/globalconfig.h"
#include "globalparams.h"

/*!
 * \brief 黑白机型独立设置界面类
 */
class paramSetAlone : public QWidget
{
    Q_OBJECT
public:
    explicit paramSetAlone(QWidget *parent = 0);
    explicit paramSetAlone(QSize size, QWidget *parent = 0);

    void createPage(void);              // 创建页面显示
    void connectSigAndSlt(void);        // 连接页面中的信号与槽
    void refreshDisplay(void);          // 刷新页面的显示
    void saveLocalParams(void);         // 将局部参数保存至全局参数
    
signals:
    
public slots:
    /* 独立设置页面槽函数 */
    void onMutexIndexChangedSlt(int index);     // 响应互斥索引变化事件

private:
    /* 独立设置页面数据 */
    g_Config *m_config;
    int m_nAloneSet;
    
    /* 独立设置页面控件 */
    myMutex *m_mutex;
};

#endif // PARAMSETALONE_H
