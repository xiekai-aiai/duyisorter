/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        lampctrl.cpp
 * \brief       黑白机型灯控界面源文件。
 * \date        2015.01.14
 */
#include "lampctrl.h"

LampCtrl::LampCtrl(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0, 0, LCD_WIDTH-20, LCD_HEIGHT-60);
    setFixedSize(LCD_WIDTH-20, LCD_HEIGHT-60);
    m_config = g_Config::getInstance();
    createPage();
    connectSigAndSlt();
}

/* 创建灯控页面 */
void LampCtrl::createPage()
{
    QWidget *upWidget = new QWidget;
    upWidget->setFixedSize(width(), height()-BTN_HEIGHT-30);
//    upWidget->setFixedSize(LCD_WIDTH-10, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);


    /* 定义页面使用的布局管理器 */
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *leftLay = new QHBoxLayout;
    QHBoxLayout *rightLay = new QHBoxLayout;
    QHBoxLayout *upLay = new QHBoxLayout(upWidget);
    QHBoxLayout *downLay = new QHBoxLayout;

    /* 实例化页面控件 */
    for (int i = 0; i < MAX_LAMP; i++) {
        m_btn[i] = new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, ICON_HEI));
        m_label[i] = new myLabel;
        m_label[i]->setFixedHeight(ICON_HEI-10);
        m_lcdNum[i] = new myLCDNumber;
        m_lcdNum[i]->setDigitCount(4);
        m_lcdNum[i]->setFixedSize(100, ICON_HEI-10);
    }

    /* 根据机型确定分组的名称 */
    QString strLeft, strRight;
    strLeft = QString("%1 1").arg(myLan.light_src_board);
    strRight = QString("%1 2").arg(myLan.light_src_board);

    m_leftGroup = new QGroupBox(strLeft);
    m_leftGroup->setFont(m_config->getFont());
    QGridLayout *leftGroupLay = new QGridLayout(m_leftGroup);
    for (int i = 0; i < MAX_LAMP/2; i++) {
        leftGroupLay->addWidget(m_label[i], i, 0);
        leftGroupLay->addWidget(m_btn[i], i, 1);
        leftGroupLay->addWidget(m_lcdNum[i], i, 2);
    }

    m_rightGroup = new QGroupBox(strRight);
    m_rightGroup->setFont(m_config->getFont());
    QGridLayout *rightGroupLay = new QGridLayout(m_rightGroup);
    for (int i = MAX_LAMP/2; i < MAX_LAMP; i++) {
        rightGroupLay->addWidget(m_label[i], i-MAX_LAMP/2, 0);
        rightGroupLay->addWidget(m_btn[i], i-MAX_LAMP/2, 1);
        rightGroupLay->addWidget(m_lcdNum[i], i-MAX_LAMP/2, 2);
    }

    /* 实例化下部页面控件 */
    m_applyBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    m_applyBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    leftLay->addWidget(m_leftGroup);
    rightLay->addWidget(m_rightGroup);

    leftLay->setContentsMargins(10, 0, 20, 0);
    rightLay->setContentsMargins(10, 0, 20, 0);
    upLay->setContentsMargins(10, 0, 20, 0);
    upLay->addLayout(leftLay);
    upLay->addLayout(rightLay);
    downLay->addWidget(m_applyBtn);
    downLay->addStretch();
    downLay->addWidget(m_backBtn);
    mainLay->addWidget(upWidget);
    mainLay->addStretch(0);
    mainLay->addLayout(downLay);
}

/* 连接灯控页面的信号与槽 */
void LampCtrl::connectSigAndSlt()
{
    m_sigMapper = new QSignalMapper(this);
    for (int i = 0; i < MAX_LAMP; i++) {
        m_sigMapper->setMapping(m_btn[i], i);
        connect(m_btn[i], SIGNAL(pressed()), m_sigMapper, SLOT(map()));
    }
    connect(m_sigMapper, SIGNAL(mapped(int)), this, SLOT(onBtnsPressed(int)));

    m_lcdSigMap = new QSignalMapper(this);
    for (int i = 0; i < MAX_LAMP; i++) {
        m_lcdSigMap->setMapping(m_lcdNum[i], i);
        connect(m_lcdNum[i], SIGNAL(pressed()), m_lcdSigMap, SLOT(map()));
    }
    connect(m_lcdSigMap, SIGNAL(mapped(int)), this, SLOT(onLcdNumPressed(int)));

    connect(m_applyBtn, SIGNAL(pressed()), this, SLOT(onApplyBtnPressed()));
    connect(m_backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnPressed()));
}

/* 根据全局参数初始化类中的局部参数 */
void LampCtrl::refreshLocalParams()
{
    int nBase = MAX_LAMP/2;
    int nLampPerBoard = struCnfg.nLampTotal/2;

    for (int i = 0; i < nLampPerBoard; i++) {
        m_nLamp[i] = struCnfp.struGroupCtrl[0].nLamp[i];
    }

    for (int i = nBase; i < nBase+nLampPerBoard; i++) {
        m_nLamp[i] = struCnfp.struGroupCtrl[0].nLamp[i-nBase+nLampPerBoard];
    }
}

/* 刷新页面的显示 */
void LampCtrl::refreshDisplay()
{
	setLampName();
	
    refreshLocalParams();

    refreshLampSetDisplay();
}

/* 刷新灯控设置页面显示 */
void LampCtrl::refreshLampSetDisplay()
{
    for (int i = 0; i < struCnfg.nLampTotal/2; i++) {
        /* 刷新使能按钮的显示 */
        if(m_nLamp[i]) {
            m_btn[i]->setIcon(myIcon.Action_Select);
        } else {
            m_btn[i]->setIcon(QIcon());
        }

        /* 刷新灯光强度的显示 */
        m_lcdNum[i]->display(struCnfp.struGroupCtrl[0].nLampLight[i]);
    }
    for (int i = MAX_LAMP/2; i < MAX_LAMP/2 + struCnfg.nLampTotal/2; i++) {
        /* 刷新使能按钮的显示 */
        if(m_nLamp[i]) {
            m_btn[i]->setIcon(myIcon.Action_Select);
        } else {
            m_btn[i]->setIcon(QIcon());
        }

        /* 刷新灯光强度的显示 */
        m_lcdNum[i]->display(struCnfp.struGroupCtrl[0].nLampLight[i-MAX_LAMP/2+struCnfg.nLampTotal/2]);
    }

    /** 隐藏多余灯控 */
    for (int i = struCnfg.nLampTotal/2; i < MAX_LAMP/2; i++) {
        m_label[i]->hide();
        m_btn[i]->hide();
        m_lcdNum[i]->hide();
    }
    for (int i = MAX_LAMP/2+struCnfg.nLampTotal/2; i < MAX_LAMP; i++) {
        m_label[i]->hide();
        m_btn[i]->hide();
        m_lcdNum[i]->hide();
    }

    /** 若为R2及以下机型则隐藏恒流源板2 */
    if (myMonoFlow.checkIsR2()) {
        m_rightGroup->hide();
    } else {
        m_rightGroup->show();
    }
}

/* 保存并下发参数 */
void LampCtrl::saveAndSendParams()
{
    /* 保存参数 */
    for(int i = 0; i < struCnfg.nLampTotal/2; i++) {
        struCnfp.struGroupCtrl[0].nLamp[i] = m_nLamp[i];
        struCnfp.struGroupCtrl[0].nLampLight[i] = m_lcdNum[i]->value();
    }
    for(int i = MAX_LAMP/2; i<MAX_LAMP/2 + struCnfg.nLampTotal/2; i++) {
        struCnfp.struGroupCtrl[0].nLamp[i-MAX_LAMP/2+struCnfg.nLampTotal/2] = m_nLamp[i];
        struCnfp.struGroupCtrl[0].nLampLight[i-MAX_LAMP/2+struCnfg.nLampTotal/2] = m_lcdNum[i]->value();
    }
    myMonoFlow.sendLampParamsMono();
}

/*!
 * \brief 设置灯的名称
 */
void LampCtrl::setLampName()
{
	for (int i = 0; i < MAX_LAMP; i++) {
        if (i < MAX_LAMP/2) {
            m_label[i]->setText(QString("%1").arg(i+1));
        } else {
            m_label[i]->setText(QString("%1").arg(i-MAX_LAMP/2+1));
        }
    }
}

/* 响应灯控页面按钮按下事件 */
void LampCtrl::onBtnsPressed(int index)
{
    if(m_nLamp[index]) {
        m_nLamp[index] = 0;
    } else {
        m_nLamp[index] = 1;
    }
    if(m_nLamp[index]) {
        m_btn[index]->setIcon(myIcon.Action_Select);
    } else {
        m_btn[index]->setIcon(QIcon());
    }
}

/* 响应应用按钮按下事件 */
void LampCtrl::onApplyBtnPressed()
{
    saveAndSendParams();
}

/* 响应返回按钮按下事件 */
void LampCtrl::onBackBtnPressed()
{
    emit backToHomePageSig();
}

/* 响应显示数字按下事件 */
void LampCtrl::onLcdNumPressed(int index)
{
    int nValue = m_lcdNum[index]->value();
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 2000, nValue);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        nValue = inputPanel->getValue();
        m_lcdNum[index]->display(nValue);
    }
}
