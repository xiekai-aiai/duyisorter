/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        sensbias.h
 * \brief       彩色机型灵敏度偏置界面头文件
 * \date        2015.01.14
 */
#ifndef SENSBIAS_H
#define SENSBIAS_H

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include "globalflow.h"
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/mylineedit.h"
#include "common/mylabel.h"
#include "common/myinputpanel.h"

/*!
 * \brief 彩色机型灵敏度偏置界面类
 */
class SensBias : public QWidget
{
    Q_OBJECT
public:
    explicit SensBias(QWidget *parent = 0);

    void createPage(void);          // 创建灵敏度偏置页面
    void updatePage(void);          // 刷新灵敏度偏置页面
    void connectSigAndSlt(void);    // 连接页面中的信号与槽
    bool isSensBiasEnable(int nArithNo);     // 判断算法是否需要灵敏度偏置
    void updateList(void);          // 刷新算法列表
    void updateSensBias(void);      // 刷新灵敏度偏置
    QString getArithNameFromId(int nArithIndex);        // 根据算法索引号获取算法名称
    int getIdentifyGroupId(int nUnit);                  // 根据相机号获取相机号所在识别组
    void createPageForGeneral(void);            // 创建灵敏度偏置灵敏度偏置布局页面
    void createPageForMazie(void);              // 创建灵敏度偏置灵敏度偏置布局页面
    void createPageForLD4(void);                // 创建灵敏度偏置灵敏度偏置布局页面
    
signals:
    void goToSetSensPageSig(void);          // 跳转至灵敏度设置页面信号
    
public slots:
    /* 灵敏度偏置页面对应槽函数 */
    void onLineEditsPressed(int nIndex);    // 响应编辑框按下事件
    void onListIndexChangedSlt(void);       // 响应算法列表索引号变化事件
    void onBackBtnPressed(void);            // 响应返回按钮按下事件
    void onSensGroupIndexChange(int index); // 玉米添加
    void onLevelBtnPressed(void);           // LDS2多层添加
    
private:
    /* 灵敏度偏置页面控件 */
    QListWidget *m_list;
    QGridLayout *upRightLay;    //各机型灵敏度偏置设置布局不同
    QListWidgetItem *m_items[ARITHMETIC_TOTAL];
    myLineEdit *m_lineEdits[MAX_UNIT];
    myLabel *m_labels[MAX_UNIT];
    myPushButton *m_backBtn;
    myPushButton *m_levelBtn;
    myMutex *m_sensGroup;

    /* 灵敏度页面数据 */
    g_Config *m_config;
    QSignalMapper *m_sigMap;
    int m_nArithIndex[ARITHMETIC_TOTAL];
    int m_nMaizeSensIndex;
    int m_nCurrentLevel;
};

#endif // SENSBIAS_H
