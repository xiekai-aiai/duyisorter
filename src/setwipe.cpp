/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setwipe.cpp
 * \brief       设置清灰界面源文件
 *              子页面包括设置清灰、清灰气帘及清灰喷水。
 * \date        2015.01.14
 */
#include "setwipe.h"

/****************************************************************************
**
** definition of class wipe brush
**
****************************************************************************/
WipeBrushWidget::WipeBrushWidget(QWidget *parent) :
    QWidget(parent)
{
    QString str;
    bFlagWipeEnable = struGsh.bFlagAutowipe == 1 ? true : false;

    wipeBrushDurationLabel = new myLabel(myLan.wipe_duration+"(s)", this);
    wipeBrushIntervalLabel = new myLabel(myLan.wipe_interval+"(min)", this);
    liftTimeBeforeWipeLabel = new myLabel("前预备时间(s)", this);
    liftTimeAfterWipeLabel = new myLabel("后预备时间(s)", this);

    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeInterval);
    wipeBrushIntervalLbe =  new myLineEdit(str, this);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeDuration);
    wipeBrushDurationLbe =  new myLineEdit(str, this);

    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nLiftTimeBeforeWipe);
    liftTimeBeforeWipeLbe =  new myLineEdit(str, this);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nLiftTimeAfterWipe);
    liftTimeAfterWipeLbe =  new myLineEdit(str, this);

    wipeBrushDuration2Label = new myLabel(myLan.wipe_duration+"2(s)");
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeDuration2);
    wipeBrushDuration2Lbe = new myLineEdit(str);

    wipeBrushDelayLabel = new myLabel(myLan.wipe_delay+"(s)");
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeDelay);
    wipeBrushDelayLbe = new myLineEdit(str);

    wipeBrushDurationLbe->setReadOnly(true);
    wipeBrushIntervalLbe->setReadOnly(true);
    wipeBrushDuration2Lbe->setReadOnly(true);
    wipeBrushDelayLbe->setReadOnly(true);

    wipeBrushDurationLbe->setMaximumSize(BTN_WIDTH, BTN_HEIGHT);
    wipeBrushIntervalLbe->setMaximumSize(BTN_WIDTH, BTN_HEIGHT);
    wipeBrushDuration2Lbe->setMaximumSize(BTN_WIDTH, BTN_HEIGHT);
    wipeBrushDelayLbe->setMaximumSize(BTN_WIDTH, BTN_HEIGHT);

    liftTimeBeforeWipeLbe->setMaximumHeight(BTN_WIDTH);
    liftTimeAfterWipeLbe->setMaximumHeight(BTN_WIDTH);
    liftTimeBeforeWipeLbe->setMaximumWidth(BTN_WIDTH);
    liftTimeAfterWipeLbe->setMaximumWidth(BTN_WIDTH);
    liftTimeBeforeWipeLabel->hide();
    liftTimeAfterWipeLabel->hide();
    liftTimeBeforeWipeLbe->hide();
    liftTimeAfterWipeLbe->hide();
    wipeBrushDuration2Label->hide();
    wipeBrushDuration2Lbe->hide();

    wipeBrushDelayLabel->hide();
    wipeBrushDelayLbe->hide();
    enableWipeBrushBtn = new myCustomCheckBox(myLan.wipe_enable, bFlagWipeEnable);
    enableWipeBrushBtn->hide();
    manualWipeBrushBtn = new myPushButton(myLan.wipe_manual,QIcon(":/res/png/Sorter_WipeManual_New.png"));
    manualWipeBrushBtn->setFixedSize(BTN_WIDTH+40, BTN_HEIGHT);

    // 页面布局
    wipeBrushPageGridLayout = new QGridLayout(this);
    wipeBrushPageGridLayout->setContentsMargins(30,20,10,10);
    wipeBrushPageGridLayout->setHorizontalSpacing(50);
    wipeBrushPageGridLayout->addWidget(wipeBrushDurationLabel, 0, 0, Qt::AlignRight);
    wipeBrushPageGridLayout->addWidget(wipeBrushDurationLbe  , 0, 1, Qt::AlignLeft);
//    wipeBrushPageGridLayout->addWidget(wipeBrushDuration2Label, 2, 0, Qt::AlignRight);
//    wipeBrushPageGridLayout->addWidget(wipeBrushDuration2Lbe  , 2, 1, Qt::AlignLeft);
    wipeBrushPageGridLayout->addWidget(wipeBrushIntervalLabel, 1, 0, Qt::AlignRight);
    wipeBrushPageGridLayout->addWidget(wipeBrushIntervalLbe  , 1, 1, Qt::AlignLeft);
//    wipeBrushPageGridLayout->addWidget(wipeBrushDelayLabel, 3, 0, Qt::AlignRight);
//    wipeBrushPageGridLayout->addWidget(wipeBrushDelayLbe, 3, 1, Qt::AlignLeft);

//    wipeBrushPageGridLayout->addWidget(liftTimeBeforeWipeLabel, 3, 0, Qt::AlignRight);
//    wipeBrushPageGridLayout->addWidget(liftTimeBeforeWipeLbe  , 3, 1, Qt::AlignLeft);
//    wipeBrushPageGridLayout->addWidget(liftTimeAfterWipeLabel, 4, 0, Qt::AlignRight);
//    wipeBrushPageGridLayout->addWidget(liftTimeAfterWipeLbe  , 4, 1, Qt::AlignLeft);

    wipeBrushPageGridLayout->addWidget(manualWipeBrushBtn    , 2, 0,1,2, Qt::AlignCenter);

    // 槽函数
    connect(manualWipeBrushBtn, SIGNAL(pressed()), this, SLOT(onManualWipeBrushBtnClickedSlt()));
    connect(enableWipeBrushBtn, SIGNAL(pressed()), this, SLOT(onEnableWipeBrushBtnClickedSlt()));
    connect(wipeBrushDurationLbe, SIGNAL(pressed()), this, SLOT(onWipeBrushDurationLbeClickedSlt()));
    connect(wipeBrushIntervalLbe, SIGNAL(pressed()), this, SLOT(onWipeBrushIntervalLbeClikedSlt()));
    connect(liftTimeBeforeWipeLbe, SIGNAL(pressed()), this, SLOT(onLiftTimeBeforeWipeLbeClickedSlt()));
    connect(liftTimeAfterWipeLbe, SIGNAL(pressed()), this, SLOT(onLiftTimeAfterWipeLbeClikedSlt()));
    connect(wipeBrushDuration2Lbe, SIGNAL(pressed()), this, SLOT(onWipeBrushDuration2LbeClickedSlt()));
    connect(wipeBrushDelayLbe, SIGNAL(pressed()), this, SLOT(onWipeBrushDelayLbeClickedSlt()));
}

void WipeBrushWidget::updateWipeBrushPage()
{
    QString str;
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeDuration);
    wipeBrushDurationLbe->setText(str);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeInterval);
    wipeBrushIntervalLbe->setText(str);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeDuration2);
    wipeBrushDuration2Lbe->setText(str);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeDelay);
    wipeBrushDelayLbe->setText(str);

    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nLiftTimeBeforeWipe);
    liftTimeBeforeWipeLbe->setText(str);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nLiftTimeAfterWipe);
    liftTimeAfterWipeLbe->setText(str);

    //! 根据清灰模式更新显示
    if (struCnfg.nWipeMode == 0) {
        wipeBrushDuration2Label->hide();
        wipeBrushDuration2Lbe->hide();
    } else {
        wipeBrushDuration2Label->show();
        wipeBrushDuration2Lbe->show();
    }
    //! 主屏模式下显示清灰同步延迟时间
    if (struCnfg.nRunMode == MODE_RUN_SLAVE) {
        wipeBrushDelayLabel->show();
        wipeBrushDelayLbe->show();
    } else {
        wipeBrushDelayLabel->hide();
        wipeBrushDelayLbe->hide();
    }
}

/**
 * @brief WipeBrushWidget::onWipeDurationLbeClickedSlt
 * 清灰持续时间修改
 */
void WipeBrushWidget::onWipeBrushDurationLbeClickedSlt()
{
    double nWipeBrushDuration;
    nWipeBrushDuration = struCnfp.struGroupCtrl[0].nWipeDuration;
    myInputPanel inputDlg(intType,1,60,nWipeBrushDuration);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        nWipeBrushDuration = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nWipeDuration = nWipeBrushDuration;
        QString str = QString("%1").arg(nWipeBrushDuration);
        wipeBrushDurationLbe->setText(str);
        MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeDuration, 0, 3);
        MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 1, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeDuration, 0, 3);
    }
}

/*!
 * \brief 清灰时间2修改
 */
void WipeBrushWidget::onWipeBrushDuration2LbeClickedSlt()
{
    double nWipeBrushDuration = struCnfp.struGroupCtrl[0].nWipeDuration2;
    myInputPanel inputDlg(intType, 1, 60, nWipeBrushDuration);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        nWipeBrushDuration = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nWipeDuration2 = nWipeBrushDuration;
        QString str = QString("%1").arg(nWipeBrushDuration);
        wipeBrushDuration2Lbe->setText(str);
    }
}

/*!
 * \brief WipeBrushWidget::onWipeIntervalLbeClikedSlt
 * 清灰间隔修改
 */
void WipeBrushWidget::onWipeBrushIntervalLbeClikedSlt()
{
    double nWipeBrushInterval;
    nWipeBrushInterval = struCnfp.struGroupCtrl[0].nWipeInterval;
    myInputPanel inputDlg(intType,1,480,nWipeBrushInterval);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted){
        nWipeBrushInterval = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nWipeInterval = nWipeBrushInterval;
        QString str = QString("%1").arg(nWipeBrushInterval);
        wipeBrushIntervalLbe->setText(str);
    }
}

/*!
 * \brief WipeBrushWidget::onWipeBrushDelayLbeClickedSlt
 * 清灰同步延迟
 */
void WipeBrushWidget::onWipeBrushDelayLbeClickedSlt()
{
    quint8 nWipeDelay = struCnfp.struGroupCtrl[ONE_LEVEL].nWipeDelay;
    myInputPanel inputDlg(intType, 0, 60, nWipeDelay);
    if (inputDlg.exec() == QDialog::Accepted) {
        nWipeDelay = inputDlg.getValue();
        struCnfp.struGroupCtrl[ONE_LEVEL].nWipeDelay = nWipeDelay;

        wipeBrushDelayLbe->setText(QString("%1").arg(nWipeDelay));
    }
}

void WipeBrushWidget::onLiftTimeBeforeWipeLbeClickedSlt()
{
    double tmp;
    tmp = struCnfp.struGroupCtrl[0].nLiftTimeBeforeWipe;
    myInputPanel inputDlg(intType,1,60,tmp);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        tmp = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nLiftTimeBeforeWipe = tmp;
        QString str = QString("%1").arg(tmp);
        liftTimeBeforeWipeLbe->setText(str);
    }
}

void WipeBrushWidget::onLiftTimeAfterWipeLbeClikedSlt()
{
    double tmp;
    tmp = struCnfp.struGroupCtrl[0].nLiftTimeAfterWipe;
    myInputPanel inputDlg(intType,1,60,tmp);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        tmp = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nLiftTimeAfterWipe = tmp;
        QString str = QString("%1").arg(tmp);
        liftTimeAfterWipeLbe->setText(str);
    }
}

/**
 * 手动功能
 */
void WipeBrushWidget::onManualWipeBrushBtnClickedSlt()
{
    //! 从屏模式下提示操作无效
    if (struCnfg.nRunMode == MODE_RUN_SLAVE) {
        myMessageBox msg(MSG_WARN, myLan.slave);
        if (msg.exec() == QDialog::Rejected) {
            return;
        }
    }
    //! 使能手动清灰标志
    struGsh.bFlagManuwipe = 1;
}

/**
 * 清灰使能功能
 */
void WipeBrushWidget::onEnableWipeBrushBtnClickedSlt()
{
    bFlagWipeEnable = !bFlagWipeEnable;
    struGsh.bFlagAutowipe = bFlagWipeEnable;
}

/****************************************************************************
**
** definition of class wipe wind
**
****************************************************************************/
WipeWindWidget::WipeWindWidget(QWidget *parent) :
        QWidget(parent)
{
    QString str;

    wipeWindDurationLabel = new myLabel(myLan.wipe_wind_duration+"(s)");
    wipeWindIntervalLabel = new myLabel(myLan.wipe_wind_interval+"(s)");

    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWindDuration*0.1);
    wipeWindDurationLbe = new myLineEdit(str, this);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWindInterval);
    wipeWindIntervalLbe = new myLineEdit(str, this);
    wipeWindDurationLbe->setReadOnly(true);
    wipeWindIntervalLbe->setReadOnly(true);
    wipeWindDurationLbe->setMaximumHeight(BTN_HEIGHT);
    wipeWindIntervalLbe->setMaximumHeight(BTN_HEIGHT);
    wipeWindDurationLbe->setMaximumWidth(BTN_WIDTH);
    wipeWindIntervalLbe->setMaximumWidth(BTN_WIDTH);

    /* 手动吹风按钮 */
    m_manualBtn = new myPushButton(myLan.wipe_wind_manual, myIcon.Sorter_WipeManual);
    m_manualBtn->setFixedSize(BTN_WIDTH+50, BTN_HEIGHT);
    m_manualBtn->hide();

    /* 页面布局 */
    wipeWindPageGridLayout = new QGridLayout(this);
    wipeWindPageGridLayout->setContentsMargins(30,20,10,10);
    wipeWindPageGridLayout->setHorizontalSpacing(50);
    wipeWindPageGridLayout->setVerticalSpacing(50);

    wipeWindPageGridLayout->addWidget(wipeWindDurationLabel, 0, 0, Qt::AlignRight);
    wipeWindPageGridLayout->addWidget(wipeWindDurationLbe  , 0, 1, Qt::AlignLeft);
    wipeWindPageGridLayout->addWidget(wipeWindIntervalLabel, 1, 0, Qt::AlignRight);
    wipeWindPageGridLayout->addWidget(wipeWindIntervalLbe  , 1, 1, Qt::AlignLeft);
    wipeWindPageGridLayout->addWidget(m_manualBtn, 2, 0, 1, 2, Qt::AlignCenter);

    // 槽函数
    connect(wipeWindDurationLbe     ,SIGNAL(pressed()),this,SLOT(onWipeWindDurationLbeClickedSlt()));
    connect(wipeWindIntervalLbe     ,SIGNAL(pressed()),this,SLOT(onWipeWindIntervalLbeClikedSlt()));
    connect(m_manualBtn, SIGNAL(pressed()), this, SLOT(onManualBtnPressedSlt()));
}

void WipeWindWidget::updateWipeWindPage()
{
    QString str;
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWindDuration*0.1);
    wipeWindDurationLbe->setText(str);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWindInterval);
    wipeWindIntervalLbe->setText(str);
}
/**
 * @brief WipeWindWidget::onWipeWindDurationLbeClickedSlt
 * 清灰吹风持续时间修改
 */
void WipeWindWidget::onWipeWindDurationLbeClickedSlt()
{
    double nWipeWindDuration;
    int nMax = 25;

    nWipeWindDuration = struCnfp.struGroupCtrl[0].nWipeWindDuration*0.1;
    myInputPanel inputDlg(floatType, 0, nMax, nWipeWindDuration);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nWipeWindDuration = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nWipeWindDuration = nWipeWindDuration*10+0.5;
        QString str = QString("%1").arg(nWipeWindDuration);
        wipeWindDurationLbe->setText(str);
        MySerial.com2Write(CMD_CTRL_WIPE_WIND, CTRL_GENERAL, 0, 0, 0,
                        struCnfp.struGroupCtrl[0].nWipeWindInterval,
                        struCnfp.struGroupCtrl[0].nWipeWindDuration, 0, 3);
    }
}
/**
 * @brief WipeWindWidget::onWipeWindIntervalLbeClikedSlt
 * 清灰吹风间隔修改
 */
void WipeWindWidget::onWipeWindIntervalLbeClikedSlt()
{
    double nWipeWindInterval;
    int nMax = 255;

    nWipeWindInterval = struCnfp.struGroupCtrl[0].nWipeWindInterval;
    myInputPanel inputDlg(intType, 1, nMax, nWipeWindInterval);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nWipeWindInterval = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nWipeWindInterval = nWipeWindInterval;
        QString str = QString("%1").arg(nWipeWindInterval);
        wipeWindIntervalLbe->setText(str);
        MySerial.com2Write(CMD_CTRL_WIPE_WIND, CTRL_GENERAL, 0, 0, 0,
                        struCnfp.struGroupCtrl[0].nWipeWindInterval,
                        struCnfp.struGroupCtrl[0].nWipeWindDuration, 0, 3);
    }
}

/* 响应自动吹风按钮按下事件 */
void WipeWindWidget::onManualBtnPressedSlt()
{
    MySerial.com2Write(CMD_CTRL_WIPE_WIND_RS, CTRL_GENERAL, 0, 0, 0, 0,
                            struCnfp.struGroupCtrl[0].nWipeWindDuration, 0xAA, 3);
}

/****************************************************************************
**
** definition of class wipe water
**
****************************************************************************/

WipeWaterWidget::WipeWaterWidget(QWidget *parent) :
            QWidget(parent)
{
    QString str;

    wipeWaterDurationLabel    = new myLabel(myLan.wipe_water_duration+"(s)", this);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWaterDuration*0.1);
    wipeWaterDurationLbe      =  new myLineEdit(str, this);
    wipeWaterDurationLbe->setMaximumHeight(BTN_HEIGHT);
    wipeWaterDurationLbe->setMaximumWidth(BTN_WIDTH);

    wipeWaterDelayLabel    = new myLabel(myLan.wipe_water_delay+"(s)", this);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWaterDelay*0.1);
    wipeWaterDelayLbe      =  new myLineEdit(str, this);
    wipeWaterDelayLbe->setMaximumHeight(BTN_HEIGHT);
    wipeWaterDelayLbe->setMaximumWidth(BTN_WIDTH);

    //页面布局
    wipeWaterPageGridLayout = new QGridLayout(this);
    wipeWaterPageGridLayout->setContentsMargins(30,20,10,10);
    wipeWaterPageGridLayout->setHorizontalSpacing(50);
    wipeWaterPageGridLayout->setVerticalSpacing(50);

    wipeWaterPageGridLayout->addWidget(wipeWaterDurationLabel, 0, 0, Qt::AlignRight);
    wipeWaterPageGridLayout->addWidget(wipeWaterDurationLbe  , 0, 1, Qt::AlignLeft);

    wipeWaterPageGridLayout->addWidget(wipeWaterDelayLabel, 1, 0, Qt::AlignRight);
    wipeWaterPageGridLayout->addWidget(wipeWaterDelayLbe  , 1, 1, Qt::AlignLeft);

    // 槽函数
    connect(wipeWaterDurationLbe     ,SIGNAL(pressed()),this,SLOT(onWipeWaterDurationLbeClickedSlt()));
    connect(wipeWaterDelayLbe     ,SIGNAL(pressed()),this,SLOT(onWipeWaterDelayLbeClickedSlt()));
}

void WipeWaterWidget::updateWaterWindPage()
{
    QString str;
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWaterDuration*0.1);
    wipeWaterDurationLbe->setText(str);
    str = QString("%1").arg(struCnfp.struGroupCtrl[0].nWipeWaterDelay*0.1);
    wipeWaterDelayLbe->setText(str);
}
/**
 * @brief WipeWaterWidget::onWipeWaterDurationLbeClickedSlt
 * 清灰喷水持续时间修改
 */
void WipeWaterWidget::onWipeWaterDurationLbeClickedSlt()
{
    double nWipeWaterDuration;

    nWipeWaterDuration = struCnfp.struGroupCtrl[0].nWipeWaterDuration/10.0;
    myInputPanel inputDlg(floatType,0,10,nWipeWaterDuration);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nWipeWaterDuration = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nWipeWaterDuration = (nWipeWaterDuration + 0.005) * 10;
        QString str = QString("%1").arg(nWipeWaterDuration);
        wipeWaterDurationLbe->setText(str);
        qDebug()<<struCnfp.struGroupCtrl[0].nWipeWindDuration;
        MySerial.com2Write(CMD_CTRL_WIPE_WATER, CTRL_GENERAL, 0, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeWaterDuration, 0, 3);
        MySerial.com2Write(CMD_CTRL_WIPE_WATER, CTRL_GENERAL, 1, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeWaterDuration, 0, 3);
    }
}

void WipeWaterWidget::onWipeWaterDelayLbeClickedSlt()
{
    double nWipeWaterDelay;

    nWipeWaterDelay = struCnfp.struGroupCtrl[0].nWipeWaterDelay/10.0;
    myInputPanel inputDlg(floatType,0,5,nWipeWaterDelay);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nWipeWaterDelay = inputDlg.getValue() ;
        struCnfp.struGroupCtrl[0].nWipeWaterDelay = nWipeWaterDelay * 10 + 0.5;
        QString str = QString("%1").arg(nWipeWaterDelay);
        wipeWaterDelayLbe->setText(str);
        qDebug()<<struCnfp.struGroupCtrl[0].nWipeWaterDelay;
        MySerial.com2Write(CMD_CTRL_WIPE_WATER_DELAY, CTRL_GENERAL, 0, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeWaterDelay, 0, 3);
        MySerial.com2Write(CMD_CTRL_WIPE_WATER_DELAY, CTRL_GENERAL, 1, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeWaterDelay, 0, 3);
    }
}

/****************************************************************************
**
** definition of class wipe mode set
**
****************************************************************************/
WipeModeWidget::WipeModeWidget(QWidget *parent):
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *upLay = new QHBoxLayout;

    m_wipeModeMutex = new myMutex(Qt::Vertical, struCnfg.nWipeMode);
    m_wipeModeMutex->setTitle(myLan.wipe+myLan.mode);
    m_wipeModeMutex->setNum(2);
    m_wipeModeMutex->setLabelText(myLan.single, myLan.complex);
    m_wipeModeMutex->setFixedSize(400, 300);

    connect(m_wipeModeMutex, SIGNAL(indexChanged(int)), this, SLOT(onWipeModeMutexChangedSlt(int)));

    upLay->addWidget(m_wipeModeMutex, 0, Qt::AlignCenter);
    mainLay->addLayout(upLay);
}


void WipeModeWidget::onWipeModeMutexChangedSlt(int nIndex)
{
    struCnfg.nWipeMode = nIndex;
}
