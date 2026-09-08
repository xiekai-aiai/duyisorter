/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setwipe.h
 * \brief       设置清灰界面头文件
 *              子页面包括设置清灰、清灰气帘及清灰喷水。
 * \date        2015.01.14
 */
#ifndef SETWIPE_H
#define SETWIPE_H

#include <QWidget>
#include <QtCore>
#include <QRegExp>
#include "mypushbutton.h"
#include "mylabel.h"
#include "mylineedit.h"
#include "mylanguage.h"
#include "myinputmethod.h"
#include "myinputpanel.h"
#include "mymessagebox.h"
#include "mycustomcheckbox.h"
#include "globalflow.h"
#include "globalparams.h"
#include "common/mygroupbox.h"

/*!
 * \brief 设置清灰界面类
 */
class WipeBrushWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WipeBrushWidget(QWidget *parent = 0);
    
public slots:
    // wipe brush widget
    void onManualWipeBrushBtnClickedSlt();
    void onEnableWipeBrushBtnClickedSlt();
    void onWipeBrushDurationLbeClickedSlt();
    void onWipeBrushDuration2LbeClickedSlt();
    void onWipeBrushIntervalLbeClikedSlt();
    void onWipeBrushDelayLbeClickedSlt();
    void onLiftTimeBeforeWipeLbeClickedSlt();
    void onLiftTimeAfterWipeLbeClikedSlt();

    void updateWipeBrushPage();

private:
    /* 清灰刷页面相关控件 */
    myLabel       *wipeBrushDurationLabel;
    myLabel       *wipeBrushIntervalLabel;
    myLabel       *wipeBrushDelayLabel;
    myLabel       *liftTimeBeforeWipeLabel;
    myLabel       *liftTimeAfterWipeLabel;
    myLabel       *wipeBrushDuration2Label;

    myLineEdit    *wipeBrushDurationLbe;
    myLineEdit    *wipeBrushIntervalLbe;
    myLineEdit    *wipeBrushDelayLbe;
    myLineEdit    *liftTimeBeforeWipeLbe;
    myLineEdit    *liftTimeAfterWipeLbe;
    myLineEdit    *wipeBrushDuration2Lbe;

    myPushButton   *manualWipeBrushBtn;    //  手动清灰
    myCustomCheckBox *enableWipeBrushBtn;  //  清灰使能
    myPushButton   *cancelSetWipeBrushBtn;

    bool           bFlagWipeEnable;

    QGridLayout    *wipeBrushPageGridLayout;
    QHBoxLayout    *wipeBrushPageHBLayout;
    QSpacerItem    *horizontalSpacerBrush;
};

/*!
 * \brief 清灰气帘界面类
 */
class WipeWindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WipeWindWidget(QWidget *parent = 0);

public slots:
    // wipe wind widget
    void onWipeWindDurationLbeClickedSlt();
    void onWipeWindIntervalLbeClikedSlt();
    void onManualBtnPressedSlt(void);               // 响应自动吹风按钮按下事件
    void updateWipeWindPage();

private:
    /* 清灰气帘页面相关控件 */
    myLabel       *wipeWindDurationLabel;
    myLabel       *wipeWindIntervalLabel;

    myLineEdit    *wipeWindDurationLbe;
    myLineEdit    *wipeWindIntervalLbe;

    QGridLayout    *wipeWindPageGridLayout;
    QHBoxLayout    *wipeWindPageHBLayout;
    QSpacerItem    *horizontalSpacerWind;
    myPushButton *m_manualBtn;
};

/*!
 * \brief 清灰喷水界面类
 */
class WipeWaterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WipeWaterWidget(QWidget *parent = 0);
    void createWipeWaterPage(void);             // 创建清灰喷水页面

public slots:
    void onWipeWaterDurationLbeClickedSlt();
    void onWipeWaterDelayLbeClickedSlt();
    void updateWaterWindPage();

private:
    /* 清灰气帘页面相关控件 */
    myLabel       *wipeWaterDurationLabel;
    myLineEdit    *wipeWaterDurationLbe;

    myLabel       *wipeWaterDelayLabel;
    myLineEdit    *wipeWaterDelayLbe;

    QGridLayout    *wipeWaterPageGridLayout;
    QHBoxLayout    *wipeWaterPageHBLayout;
    QSpacerItem    *horizontalSpacerWater;

};

/*!
 * \brief 清灰模式设置界面类
 */
class WipeModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WipeModeWidget(QWidget *parent = 0);
    void createWipeModePage(void);             // 创建清灰喷水页面

public slots:
//    void onWipeWaterDurationLbeClickedSlt();
//    void onWipeWaterDelayLbeClickedSlt();
//    void updateWaterWindPage();
      void onWipeModeMutexChangedSlt(int nIndex);

private:
    /* 清灰气帘页面相关控件 */
//    myLabel       *wipeWaterDurationLabel;
//    myLineEdit    *wipeWaterDurationLbe;

//    myLabel       *wipeWaterDelayLabel;
//    myLineEdit    *wipeWaterDelayLbe;

//    QGridLayout    *wipeWaterPageGridLayout;
//    QHBoxLayout    *wipeWaterPageHBLayout;
//    QSpacerItem    *horizontalSpacerWater;
      myMutex *m_wipeModeMutex;

};


#endif // SETWIPE_H
