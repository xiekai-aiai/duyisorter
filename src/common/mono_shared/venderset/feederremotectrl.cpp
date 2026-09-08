/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        feederremotectrl.cpp
 * \brief       黑白机型振动器外接控制界面源文件。
 * \date        2015.01.14
 */
#include "feederremotectrl.h"

FeederRemoteCtrl::FeederRemoteCtrl(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT);
    m_config = g_Config::getInstance();

    createPage();
    connectSigAndSlt();
}

/* 创建页面 */
void FeederRemoteCtrl::createPage()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *downLay = new QHBoxLayout;

    QWidget *upWidget = new QWidget;
    upWidget->setFixedSize(width(), height()-BTN_HEIGHT-50);
    QHBoxLayout *upLay = new QHBoxLayout(upWidget);

    m_mutex = new myMutex(Qt::Vertical);
    m_mutex->setNum(2);
    m_mutex->setLabelText(myLan.opposite_direction, myLan.forward_direction);
    m_mutex->setFixedSize(400, 300);

    m_okBtn = new myPushButton(myLan.ok, myIcon.Action_Apply);
    m_okBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_cancelBtn = new myPushButton(myLan.cancel, myIcon.Action_Cancel);
    m_cancelBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    downLay->addWidget(m_okBtn);
    downLay->addStretch();
    downLay->addWidget(m_cancelBtn);
    upLay->addWidget(m_mutex);
    mainLay->addWidget(upWidget);
    mainLay->addLayout(downLay);
}

/* 连接页面中的信号与槽 */
void FeederRemoteCtrl::connectSigAndSlt()
{
    connect(m_okBtn, SIGNAL(pressed()), this, SLOT(onOkBtnPressed()));
    connect(m_cancelBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnPressed()));
    connect(m_mutex, SIGNAL(indexChanged(int)), this, SLOT(onMutexIndexChangedSlt(int)));
}

/* 刷新页面的显示 */
void FeederRemoteCtrl::refreshDisplay()
{
    m_nMode = struCnfg.nFeederRemoteCtrlMode;
    m_mutex->setCurrentIndex(m_nMode);
}

/* 确定 */
void FeederRemoteCtrl::onOkBtnPressed()
{
    struCnfg.nFeederRemoteCtrlMode = m_nMode;
    myMonoFlow.sendFeederRemoteCtrlParams();

    emit backToHomePageSig();
}

/* 取消 */
void FeederRemoteCtrl::onCancelBtnPressed()
{
    emit backToHomePageSig();
}

/* 更改单选框 */
void FeederRemoteCtrl::onMutexIndexChangedSlt(int nIndex)
{
    m_nMode = nIndex;
}
