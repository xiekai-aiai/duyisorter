/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        systeminitialization.h
 * \brief       系统初始化界面头文件
 *              子页面包括通信自检、自检列表、系统初始化及系统预热。
 * \date        2015.01.14
 */
#ifndef SYSTEMINITIALIZATION_H
#define SYSTEMINITIALIZATION_H

#include <QProgressBar>
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
    explicit communication(QWidget *parent = 0);

signals:
    void updateCommunicationSig();
    void goToCommListPageSig();
    void goToInitPageSig();

public slots:
    void startComSlt(void);

private:
    myLabel         *waitCommLabel;
    QVBoxLayout     *commVBLayout;
};

/*!
 * \brief 通信自检界面类
 */
class communicationList : public QWidget
{
    Q_OBJECT
public:
    explicit communicationList(QWidget *parent = 0);

    void commuUpdateInterface();
    void commuUpdateCtrl();
    void commuUpdateCamera();
    void commuUpdateCameraMultiView();
    void commuUpdateCameraRS();
    void commuUpdateLightSrc(void);     // 更新恒流源板版本信息
    void hideColumn(int index);         // 隐藏某一列
    void showColumn(int index);         // 显示某一列

signals:
    void goToInitPageSig();
    void goToCommPageSig();

public slots:
    void updateCommunicationResultSlt();
    void onRecheckBtnClickedSlt();
    void onNextBtnClickedSlt();

private:
    g_Config *config;
    // 通信列表
    myLabel         *waitCommLabel;
    myPushButton    *rechekBtn;
    myPushButton    *nextBtn;

    myGroupBox *interfaceBoardListGroup;
    QGridLayout *interfaceListLayout;

    myGroupBox *newInterfaceBoardListGroup;
    QGridLayout *newInterfaceListLayout;
    QLabel *cameraStateLbl;
    QLabel *testStateLbl;

    /** 接口板版本中加入USB固件版本 */
    QLabel *interfaceListLabel[MAX_INT+1][2];

    myGroupBox *ctrlBoardListGroup;
    QGridLayout *controlListLayout;
    QLabel *controlListLabel[MAX_CTRL][2];

    QLabel *aiLabelName;
    QLabel *aiLabelStatus;

    QGroupBox *cameraBoardListGroup;
    QGridLayout *cameraListLayout;
    QLabel *cameraListLabel[MAX_UNIT/2 + 1][MAX_BACKGROUND_GROUP + 3];
    /* 横流源板版本部分控件 */
    QGroupBox *m_lightSrcListGroup;
    QLabel *m_lightSrcLabel[MAX_LIGHT_SOURCE][2];
    QGridLayout *m_lightSrcLay;

    QVBoxLayout *backHBLayout;
    QHBoxLayout *communicationLayout;

    QGridLayout     *commListGridLayout;
    QHBoxLayout     *commListHBLayout;
    QSpacerItem     *horizontalSpacer;
};

/*!
 * \brief 系统初始化界面类
 */
class initialization : public QWidget
{
    Q_OBJECT
public:
    explicit initialization(QWidget *parent = 0);

signals:
    void goToPreheatPageSig();

public slots:
    void startInitSlt();

private:
    // 通信列表
    myLabel         *waitCommLabel;

    QVBoxLayout     *initVBLayout;
};

/*!
 * \brief 系统预热界面类
 */
class preheat : public QWidget
{
    Q_OBJECT
public:
    explicit preheat(QWidget *parent = 0);

signals:
    void goToHomePageSig();

public slots:
    void startPreheatSlt();
    void onSkipBtnClickSlt();

private:
    // 通信列表
    myLabel         *waitCommLabel;
    QProgressBar    *progressBar;
    myPushButton    *skipBtn;
    bool            bFlagSkip;

    QGridLayout     *preheatGridLayout;
    QHBoxLayout     *preheatHBLayout;
    QSpacerItem     *horizontalSpacer;
};

#endif // SYSTEMINITIALIZATION_H
