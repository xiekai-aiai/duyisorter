/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        systeminitialization.h
 * \brief       系统初始化界面头文件
 *              子页面包括通信自检、自检列表、系统初始化及系统预热。
 * \date        2026.11.21
 */
#ifndef SYSTEMINITIALIZATION_H
#define SYSTEMINITIALIZATION_H

#include <QProgressBar>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputpanel.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputmethod.h"
#include "globalflow.h"
#include "globalparams.h"
#include "mydelaycode.h"
#include "rgb/setencryt.h"

 /*!
  * \brief 通信自检界面类
  */
class communication : public QWidget
{
    Q_OBJECT
public:
    explicit communication(QWidget* parent = 0);

signals:
    void updateCommunicationSig();
    void goToCommListPageSig();
    void goToInitPageSig();

public slots:
    void startComSlt(void);

private:
    myLabel* waitCommLabel;
    QVBoxLayout* commVBLayout;
};

class StateMatrixWidget;

/*!
 * \brief 通信自检界面类（参考 state_page 重构：汇总卡 + 相机矩阵 + 板卡卡片）
 */
class communicationList : public QWidget
{
    Q_OBJECT
public:
    explicit communicationList(QWidget* parent = 0);

    void commuUpdateCtrl();
    void commuUpdateCamera();

signals:
    void goToInitPageSig();
    void goToCommPageSig();

public slots:
    void updateCommunicationResultSlt();
    void onRecheckBtnClickedSlt();
    void onNextBtnClickedSlt();

protected:
    void paintEvent(QPaintEvent* event);

private:
    QWidget* createSummaryCard(int index, const QString& title, const QColor& color);

    g_Config* config;
    // 通信列表
    myPushButton* rechekBtn;
    myPushButton* nextBtn;

    //! 顶部汇总卡片: 0通道 1相机 2AI 3告警
    QLabel* m_sumValue[4];
    QLabel* m_sumTotal[4];

    //! 相机 / AI 板矩阵（自绘）
    StateMatrixWidget* m_matrix;

    //! 控制板矩阵（自绘，与相机板同一控件）
    StateMatrixWidget* m_ctrlMatrix;
};

/*!
 * \brief 系统初始化界面类
 */
class initialization : public QWidget
{
    Q_OBJECT
public:
    explicit initialization(QWidget* parent = 0);

signals:
    void goToPreheatPageSig();

public slots:
    void startInitSlt();
    void runNextStepSlt();

protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void onAnimTickSlt();

private:
    enum { INIT_STEP_COUNT = 5 };

    void drawTitle(QPainter& p);
    void drawRing(QPainter& p);
    void drawSteps(QPainter& p);
    void drawFooter(QPainter& p);
    void drawStateIcon(QPainter& p, int cx, int cy, int state);   //! 0待执行 1执行中 2已完成
    QString stepText(int index);

    int     m_currentStep;      //! 等待执行的步骤序号
    int     m_doneCount;        //! 已完成步骤数
    qreal   m_percent;          //! 平滑显示的百分比
    qreal   m_targetPercent;    //! 目标百分比
    int     m_pulseTick;        //! 脉动计时（33ms/格）
    int     m_finishDelay;      //! 完成后的停留计时
    bool    m_finished;         //! 是否已跳转预热页
    QTimer* m_animTimer;
};

/*!
 * \brief 系统预热界面类
 */
class preheat : public QWidget
{
    Q_OBJECT
public:
    explicit preheat(QWidget* parent = 0);

signals:
    void goToHomePageSig();

public slots:
    void startPreheatSlt();
    void onSkipBtnClickSlt();

private:
    // 通信列表
    myLabel* waitCommLabel;
    QProgressBar* progressBar;
    myPushButton* skipBtn;
    bool            bFlagSkip;

    QGridLayout* preheatGridLayout;
    QHBoxLayout* preheatHBLayout;
    QSpacerItem* horizontalSpacer;
};

#endif // SYSTEMINITIALIZATION_H
