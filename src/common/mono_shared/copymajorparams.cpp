/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        copymajorparams.cpp
 * \brief       黑白机型复制重要参数界面源文件。
 * \date        2015.01.14
 */
#include "copymajorparams.h"

CopyMajorParams::CopyMajorParams(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT);
    m_config = g_Config::getInstance();
    createPage();
    connectSigAndSlt();
}

/* 创建复制重要参数页面 */
void CopyMajorParams::createPage()
{
    /* 子页面 */
    QWidget *upWidget = new QWidget;
    upWidget->setFixedSize(width(), height()-BTN_HEIGHT-50);

    /* 定义页面布局管理器 */
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QGridLayout *upLay = new QGridLayout(upWidget);
    QHBoxLayout *downLay = new QHBoxLayout;

    /* 定义页面控件 */
    /* 源方案部分控件 */
    m_srcGroup = new QGroupBox(myLan.scheme_cur);
    m_srcGroup->setFont(m_config->getFont());
    m_srcGroup->setFixedSize(500, 150);
    QHBoxLayout *sourceLay = new QHBoxLayout(m_srcGroup);
    m_srcLabel = new myLabel("");
    m_srcLabel->setAlignment(Qt::AlignCenter);
    m_srcLabel->setFixedHeight(BTN_HEIGHT);
    sourceLay->addWidget(m_srcLabel);

    /* 目的方案部分控件 */
    m_desGroup = new QGroupBox(myLan.scheme_des);
    m_desGroup->setFont(m_config->getFont());
    m_desGroup->setFixedSize(500, 300);
    QGridLayout *desLay = new QGridLayout(m_desGroup);
    m_valueBar = new myValueBar(1, struCnfg.nProfileTotal, 1, 1, struCnfg.nProfile+1);
    m_valueBar->setFixedSize(300, 100);
    m_valueBar->hideFastChangeBtns();
    m_desLabel = new myLabel("");
    m_desLabel->setAlignment(Qt::AlignCenter);
    m_desLabel->setFixedHeight(BTN_HEIGHT);
    m_selAllBtn = new myPushButton(myLan.scheme_all, QIcon());
    m_selAllBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    desLay->addWidget(m_valueBar, 0, 0, Qt::AlignCenter);
    desLay->addWidget(m_desLabel, 1, 0, Qt::AlignCenter);
    desLay->addWidget(m_selAllBtn, 2, 0, Qt::AlignCenter);

    /* 下半部分控件 */
    m_applyBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    m_applyBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    upLay->addWidget(m_srcGroup, 0, 0, Qt::AlignCenter);
    upLay->addWidget(m_desGroup, 1, 0, Qt::AlignCenter);
    downLay->addWidget(m_applyBtn);
    downLay->addStretch();
    downLay->addWidget(m_backBtn);
    mainLay->addWidget(upWidget);
    mainLay->addLayout(downLay);
}

/* 连接页面的信号与槽 */
void CopyMajorParams::connectSigAndSlt()
{
    connect(m_applyBtn, SIGNAL(pressed()), this, SLOT(onApplyBtnPressed()));
    connect(m_backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnPressed()));
    connect(m_selAllBtn, SIGNAL(pressed()), this, SLOT(onSelAllBtnPressed()));
    connect(m_valueBar, SIGNAL(valueChanged(int)), this, SLOT(onValueBarChangedSlt(int)));
}

/* 刷新页面的显示 */
void CopyMajorParams::refreshDisplay()
{
    QString str;
    /* 刷新源方案的显示 */
    str = QString("%1.%2---%3").arg(struCnfg.nProfile+1)
        .arg(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName)
        .arg(myMonoFlow.getNameByMode(struCnfg.nProfileMode[struCnfg.nProfile]));
    m_srcLabel->setText(str);

    /* 刷新目的方案的显示 */
    int nProfile = (struCnfg.nProfile == 0)?2:1;
    if(nProfile > struCnfg.nProfileTotal) {
        nProfile = struCnfg.nProfileTotal;
    }
    m_valueBar->setValue(nProfile);
    m_valueBar->setMaxValue(struCnfg.nProfileTotal);
    str = QString("%1.%2---%3").arg(nProfile)
        .arg(struCnfg.struProfileIndex[nProfile-1].sMaterialName)
        .arg(myMonoFlow.getNameByMode(struCnfg.nProfileMode[nProfile-1]));
    m_desLabel->setText(str);

    m_bSelAll = false;
    m_selAllBtn->setRedColor(DEF);
    m_valueBar->setEnabled(true);
}

/* 复制重要参数至目的方案 */
void CopyMajorParams::copyMajorToProfile()
{
    int nTmpProfile;
    int nTmpAnaGain[MAX_UNIT];
    int nEjectTime[MAX_GROUP_TICK];
    int nDelayTime[MAX_GROUP_TICK];
    int nLampLight[MAX_LAMP];

    /* 1.将全局参数拷贝至局部变量 */
    nTmpProfile = struCnfg.nProfile;
    for(int i=0; i<MAX_UNIT; i++) {
        nTmpAnaGain[i] = struCnfp.nAnaGain[i];
    }
    for(int i=0; i<MAX_GROUP_TICK; i++) {
        nEjectTime[i] = struCnfp.struGroupTick[0][i].nEjectDuration;
        nDelayTime[i] = struCnfp.struGroupTick[0][i].nEjectDelay;
    }
    for(int i=0; i<MAX_LAMP; i++) {
        nLampLight[i] = struCnfp.struGroupCtrl[0].nLampLight[i];
    }

    /* 2.保存当前方案 */
    myFlow.saveProfile();

    /* 3.打开需要复制参数的方案并将重要参数复制至目的方案 */
    if(m_bSelAll) {
        /*------- 复制至所有方案 -------*/
        for(int i=0; i<struCnfg.nProfileTotal; i++) {
            if(i == nTmpProfile) {
                continue;
            }

            struCnfg.nProfile = i;
            myFlow.getProfile();
            for(int j=0; j<MAX_UNIT; j++) {
                struCnfp.nAnaGain[j] = nTmpAnaGain[j];
            }
            for(int j=0; j<MAX_GROUP_TICK; j++) {
                struCnfp.struGroupTick[0][j].nEjectDuration = nEjectTime[j];
                struCnfp.struGroupTick[0][j].nEjectDelay = nDelayTime[j];
            }
            for(int j=0; j<MAX_LAMP; j++) {
                if(j == 2 || j == 7) {
                    continue;
                }
                struCnfp.struGroupCtrl[0].nLampLight[j] = nLampLight[j];
            }
            myFlow.saveProfile();
        }
    } else {
        /*------- 复制至已选方案 -------*/
        /* 若选择方案即为当前方案则退出 */
        if(m_valueBar->getValue()-1 == struCnfg.nProfile) {
            return;
        }

        struCnfg.nProfile = m_valueBar->getValue()-1;
        myFlow.getProfile();
        for(int i=0; i<MAX_UNIT; i++) {
            struCnfp.nAnaGain[i] = nTmpAnaGain[i];
        }
        for(int i=0; i<MAX_GROUP_TICK; i++) {
            struCnfp.struGroupTick[0][i].nEjectDuration = nEjectTime[i];
            struCnfp.struGroupTick[0][i].nEjectDelay = nDelayTime[i];
        }
        for(int i=0; i<MAX_LAMP; i++) {
            if(i == 2 || i == 7) {
                continue;
            }
            struCnfp.struGroupCtrl[0].nLampLight[i] = nLampLight[i];
        }
        myFlow.saveProfile();
    }

    /* 4.恢复至原始方案 */
    struCnfg.nProfile = nTmpProfile;
    myFlow.getProfile();
}

/*!
 * \brief CopyMajorParams::copyColorMajorToProfile
 */
void CopyMajorParams::copyColorMajorToProfile()
{
    int nTmpProfile;
    int nTmpDigiGainUpperRed[MAX_UNIT];
    int nTmpDigiGainUpperGreen[MAX_UNIT];
    int nTmpDigiGainUpperBlue[MAX_UNIT];
    int nTmpDigiGainLowRed[MAX_UNIT];
    int nTmpDigiGainLowGreen[MAX_UNIT];
    int nTmpDigiGainLowBlue[MAX_UNIT];
    int nEjectTime[MAX_GROUP_TICK];
    int nDelayTime[MAX_GROUP_TICK];

    /* 1.将全局参数拷贝至局部变量 */
    nTmpProfile = struCnfg.nProfile;
    for (int i = 0; i < MAX_UNIT; i++) {
        nTmpDigiGainUpperRed[i] = struCnfc.struLevelCamera[0].nDigiGainUpperRed[i];
        nTmpDigiGainUpperGreen[i] = struCnfc.struLevelCamera[0].nDigiGainUpperGreen[i];
        nTmpDigiGainUpperBlue[i] = struCnfc.struLevelCamera[0].nDigiGainUpperBlue[i];

        nTmpDigiGainLowRed[i] = struCnfc.struLevelCamera[0].nDigiGainLowRed[i];
        nTmpDigiGainLowGreen[i] = struCnfc.struLevelCamera[0].nDigiGainLowGreen[i];
        nTmpDigiGainLowBlue[i] = struCnfc.struLevelCamera[0].nDigiGainLowBlue[i];
    }
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        nEjectTime[i] = struCnfp.struGroupTick[0][i].nEjectDuration;
        nDelayTime[i] = struCnfp.struGroupTick[0][i].nEjectDelay;
    }

    /* 2.保存当前方案 */
    myFlow.saveProfile();

    /* 3.打开需要复制参数的方案并将重要参数复制至目的方案 */
    if (m_bSelAll) {
        /*------- 复制至所有方案 -------*/
        for(int i=0; i<struCnfg.nProfileTotal; i++) {
            if(i == nTmpProfile) {
                continue;
            }

            struCnfg.nProfile = i;
            myFlow.getProfile();
            for (int j = 0; j < MAX_UNIT; j++) {
                struCnfc.struLevelCamera[0].nDigiGainUpperRed[j] = nTmpDigiGainUpperRed[j];
                struCnfc.struLevelCamera[0].nDigiGainUpperGreen[j] = nTmpDigiGainUpperGreen[j];
                struCnfc.struLevelCamera[0].nDigiGainUpperBlue[j] = nTmpDigiGainUpperBlue[j];

                struCnfc.struLevelCamera[0].nDigiGainLowRed[j] = nTmpDigiGainLowRed[j];
                struCnfc.struLevelCamera[0].nDigiGainLowGreen[j] = nTmpDigiGainLowGreen[j];
                struCnfc.struLevelCamera[0].nDigiGainLowBlue[j] = nTmpDigiGainLowBlue[j];
            }
            for(int j=0; j<MAX_GROUP_TICK; j++) {
                struCnfp.struGroupTick[0][j].nEjectDuration = nEjectTime[j];
                struCnfp.struGroupTick[0][j].nEjectDelay = nDelayTime[j];
            }
            myFlow.saveProfile();
        }
    } else {
        /*------- 复制至已选方案 -------*/
        /* 若选择方案即为当前方案则退出 */
        if(m_valueBar->getValue()-1 == struCnfg.nProfile) {
            return;
        }

        struCnfg.nProfile = m_valueBar->getValue()-1;
        myFlow.getProfile();
        for(int i=0; i<MAX_UNIT; i++) {
            struCnfc.struLevelCamera[0].nDigiGainUpperRed[i] = nTmpDigiGainUpperRed[i];
            struCnfc.struLevelCamera[0].nDigiGainUpperGreen[i] = nTmpDigiGainUpperGreen[i];
            struCnfc.struLevelCamera[0].nDigiGainUpperBlue[i] = nTmpDigiGainUpperBlue[i];

            struCnfc.struLevelCamera[0].nDigiGainLowRed[i] = nTmpDigiGainLowRed[i];
            struCnfc.struLevelCamera[0].nDigiGainLowGreen[i] = nTmpDigiGainLowGreen[i];
            struCnfc.struLevelCamera[0].nDigiGainLowBlue[i] = nTmpDigiGainLowBlue[i];
        }

        for(int i=0; i<MAX_GROUP_TICK; i++) {
            struCnfp.struGroupTick[0][i].nEjectDuration = nEjectTime[i];
            struCnfp.struGroupTick[0][i].nEjectDelay = nDelayTime[i];
        }

        myFlow.saveProfile();
    }

    /* 4.恢复至原始方案 */
    struCnfg.nProfile = nTmpProfile;
    myFlow.getProfile();
}

/* 响应应用按钮按下事件 */
void CopyMajorParams::onApplyBtnPressed()
{
    if(m_bSelAll) {
        myMessageBox *msgBox = new myMessageBox(MSG_QUES, myLan.cfm_copy_to_all);
        int ret = msgBox->exec();
        if(ret != QDialog::Accepted) {
            return;
        }
    }

    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();
    copyMajorToProfile();
    infoWidget->delayHide();
}

/* 响应返回按钮按下事件 */
void CopyMajorParams::onBackBtnPressed()
{
    emit goToManageSchemeSig();
}

/* 响应选择全部按钮按下事件 */
void CopyMajorParams::onSelAllBtnPressed()
{
    m_bSelAll = !m_bSelAll;
    if(m_bSelAll) {
        m_selAllBtn->setRedColor(GREEN);
        m_valueBar->setEnabled(false);
    } else {
        m_selAllBtn->setRedColor(DEF);
        m_valueBar->setEnabled(true);
    }
}

/* 响应目的方案索引号变化事件 */
void CopyMajorParams::onValueBarChangedSlt(int nValue)
{
    QString str;
    str = QString("%1.%2---%3").arg(nValue)
            .arg(struCnfg.struProfileIndex[nValue-1].sMaterialName)
            .arg(myMonoFlow.getNameByMode(struCnfg.nProfileMode[nValue-1]));
    m_desLabel->setText(str);
}
