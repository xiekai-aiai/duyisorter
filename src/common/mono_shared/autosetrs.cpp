/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        autosetrs.cpp
 * \brief       黑白机型一键自动界面源文件。
 * \date        2015.01.14
 */
#include "autosetrs.h"

autoSetPage::autoSetPage(QWidget *parent) :
    QWidget(parent)
{
    m_config = g_Config::getInstance();
    m_bIsFirst = true;
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedSize(LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);

    autoSetMainPage = new QWidget();
    stackedWidget->addWidget(autoSetMainPage);
    CreatAutoSetMainWidget();

    cameraSingalPage = new QWidget();
    stackedWidget->addWidget(cameraSingalPage);
    CreateCameraSingalWidget();
}

void autoSetPage::CreatAutoSetMainWidget(void)
{
     adjustStartBtn = new myPushButton(myLan.auto_set,myIcon.Media_Start);
     adjustStartBtn->setFixedSize(BTN_WIDTH+50,BTN_HEIGHT);
     cameraSingalBtn = new myPushButton(myLan.camera_signal,myIcon.Chart_Line);
     cameraSingalBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
     autoSetBackBtn = new myPushButton(myLan.back,myIcon.Action_Back);
     autoSetBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

     /* 槽函数 */
     connect(autoSetBackBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnClickedSlt()));
     connect(adjustStartBtn, SIGNAL(pressed()), this, SLOT(onAdjustStartBtnClickedSlt()));
     connect(cameraSingalBtn, SIGNAL(pressed()), this, SLOT(onCameraSingalBtnClickedSlt()));

     /* 页面布局 */
     QHBoxLayout *upLay = new QHBoxLayout;
     QHBoxLayout *downLay = new QHBoxLayout;
     QVBoxLayout *mainLay = new QVBoxLayout(autoSetMainPage);

     mainLay->setContentsMargins(20, 0, 20, 0);
     upLay->setContentsMargins(0, 250, 0, 260);
     upLay->addWidget(adjustStartBtn);
     downLay->addWidget(cameraSingalBtn);
     downLay->addSpacing(655);
     downLay->addWidget(autoSetBackBtn);
     mainLay->addLayout(upLay);
     mainLay->addLayout(downLay);
}

void autoSetPage::CreateCameraSingalWidget(void)
{
    autoSetCurve = new MyCurve();

    QGroupBox *group = new QGroupBox(myLan.background_set);
    group->setFixedSize(400, 120);
    group->setFont(m_config->getFont());
    QHBoxLayout *groupLay = new QHBoxLayout(group);
    backgroundValueBar = new myValueBar(0, 2000, 1, 10, 0);
    backgroundValueBar->setFixedSize(350, BTN_HEIGHT);
    groupLay->addWidget(backgroundValueBar);

    returnMainBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    returnMainBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    QHBoxLayout *uplay = new QHBoxLayout;
    QHBoxLayout *midLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(cameraSingalPage);

    uplay->setContentsMargins(0, 20, 0, 20);
    uplay->addWidget(autoSetCurve);
    midLay->addWidget(group);
    downLay->addStretch();
    downLay->addWidget(returnMainBtn);
    mainLay->addLayout(uplay);
    mainLay->addLayout(midLay);
    mainLay->addLayout(downLay);

    /* 槽函数 */
    connect(returnMainBtn, SIGNAL(pressed()), this, SLOT(onReturnMainBtnClickedSlt()));
    connect(autoSetCurve, SIGNAL(boardNumChanged()), this, SLOT(onBoardNumChangedSlt()));
    connect(backgroundValueBar, SIGNAL(valueChanged(int)), this, SLOT(onValueBarChangedSlt(int)));
}

void autoSetPage::onCancelBtnClickedSlt(void)
{
    emit goToHomePageSig();
}

/* 刷新相机信号页面的显示 */
void autoSetPage::refreshCamSigDisplay()
{
    autoSetCurve->refreshCurveDisplay();

    /* 刷新背景值的显示 */
    refreshValueBarDisplay();
}

/* 刷新背景值的显示 */
void autoSetPage::refreshValueBarDisplay()
{
    if(struGsh.nUnit%2 == 0) {
        //! 后视第3盏灯为背景灯
        if(struCnfp.struGroupCtrl[0].nLamp[m_nFrontBkgLamp] == 0) {
        	backgroundValueBar->setValue(0);
			backgroundValueBar->setEnabled(false);
		} else {
            backgroundValueBar->setValue(struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp]);
			backgroundValueBar->setEnabled(true);
		}
    } else {
        if(struCnfp.struGroupCtrl[0].nLamp[m_nRearBkgLamp] == 0) {
        	backgroundValueBar->setValue(0);
			backgroundValueBar->setEnabled(false);
		} else {
            backgroundValueBar->setValue(struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp]);
			backgroundValueBar->setEnabled(true);
		}
    }
}

/*!
 * \brief 重置背景灯索引
 */
void autoSetPage::resetBkgLampIndex()
{
    /** R2机型：前视对应的背景灯为恒流源板1的第4盏灯
               后视对应的背景灯为恒流源板1的第3盏灯
        其它米机：前视对应的背景灯为恒流源板2的第3盏灯
                后视对应的背景灯为恒流源板1的第3盏灯
    */
    if (myMonoFlow.checkIsR2()) {
        m_nFrontBkgLamp = 5;
        m_nRearBkgLamp = 2;
    } else {
        m_nFrontBkgLamp = 8;
        m_nRearBkgLamp = 2;
    }
}

/* 响应一键自动按钮 */
void autoSetPage::onAdjustStartBtnClickedSlt(void)
{
    /* 提示用户是否进行此操作 */
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_auto_set);
    int nRet = mesBox->exec();
    if(nRet == QDialog::Rejected) {
        return;
    }

    int nFeederEn[10];
    /* 深色专用，直接清灰 */
    if (struCnfg.nAutoFlagDark == 1) {
        infoWidget->setLabelText(myLan.msg_wiping);
        infoWidget->delayShow();
        MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeDuration, 0xAA, 3);
    	myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration+3);
        infoWidget->hide();
    } else {
        /* 设置供料量 */
        if (struCnfg.nAutoFeederSet == 1) {
            for (int i = 0; i < 10; i++) {
                feederTemp[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
                nFeederEn[i] = struCnfp.struGroupCtrl[0].nFeederEnable[i];
                struCnfp.struGroupCtrl[0].nFeeder[i] = 400;
                struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
            }
            myFlow.resetFeeder(0);
        }

        /* 设置灵敏度至固定值 */
        adjustSens();

        /* 根据色选模式判断前后视是否需要校准 */
        int nFrontMode = -1;
        int nRearMode = -1;
        setFlagByMode(nFrontMode, nRearMode);

        /* 关背景灯 */
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nFrontBkgLamp/MAX_LAMP_PER_BOARD,
                           0,
                           0,
                           m_nFrontBkgLamp%MAX_LAMP_PER_BOARD,
                           0,
                           0,
                           3);
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nRearBkgLamp/MAX_LAMP_PER_BOARD,
                           0,
                           0,
                           m_nRearBkgLamp%MAX_LAMP_PER_BOARD,
                           0,
                           0,
                           3);

        /* 清灰 */
        infoWidget->setLabelText(myLan.msg_wiping);
        infoWidget->delayShow();
        MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeDuration, 0xAA, 3);
    	myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration+3);
        infoWidget->hide();

        infoWidget->setLabelText(myLan.msg_auto_calibrating);
        infoWidget->delayShow();

        /* 增益校准 */
        calibrateGain(nFrontMode, nRearMode);

        /* 清灰 */
        infoWidget->setLabelText(myLan.msg_wiping);
        MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0, 0, 0, 0,
                           struCnfp.struGroupCtrl[0].nWipeDuration, 0xAA, 3);
    	myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration+3);
        infoWidget->setLabelText(myLan.msg_auto_calibrating);

        if (m_bIsFirst) {
            /* 增益校准 */
            calibrateGain(nFrontMode, nRearMode);
            m_bIsFirst = false;
        }

        /* 亮背景灯 */
        if (struCnfp.struGroupCtrl[0].nLamp[m_nRearBkgLamp] == 1) {
            MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nRearBkgLamp/MAX_LAMP_PER_BOARD,
                               0,
                               0,
                               m_nRearBkgLamp%MAX_LAMP_PER_BOARD,
                               struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp]/256,
                               struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp]%256, 3);
        }
        if (struCnfp.struGroupCtrl[0].nLamp[m_nFrontBkgLamp] == 1) {
            MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nFrontBkgLamp/MAX_LAMP_PER_BOARD,
                               0,
                               0,
                               m_nFrontBkgLamp%MAX_LAMP_PER_BOARD,
                               struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp]/256,
                               struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp]%256, 3);
        }
        myFlow.sleep(2);

        /* 前视校准 */
        if (nFrontMode == 1) {
            adjustBackground(0);
        }

        /* 后视校准 */
        if (nRearMode == 1) {
            adjustBackground(1);
        }

        /* 恢复灵敏度 */
        resumeSens();

        /* 恢复供料量 */
        if(struCnfg.nAutoFeederSet == 1) {
            for(int i=0; i<10; i++) {
                struCnfp.struGroupCtrl[0].nFeeder[i] = feederTemp[i];
                struCnfp.struGroupCtrl[0].nFeederEnable[i] = nFeederEn[i];
            }
            myFlow.resetFeeder(0);
        }
    }
    infoWidget->hide();
    onCameraSingalBtnClickedSlt();
}

/* 响应相机信号按钮 */
void autoSetPage::onCameraSingalBtnClickedSlt(void)
{
    refreshCamSigDisplay();
    stackedWidget->setCurrentIndex(CAMERA_SINGAL_PAGE);
    autoSetCurve->curveStartSlt();
}

/* 响应返回相机信号按钮 */
void autoSetPage::onReturnMainBtnClickedSlt(void)
{
    autoSetCurve->curveStopSlt();
    stackedWidget->setCurrentIndex(AUTO_SET_MAIN_PAGE);
}

/* 响应背景值变化事件 */
void autoSetPage::onValueBarChangedSlt(int nValue)
{
    //! 前视对应的后背景灯，后视对应的前背景灯
    if(struGsh.nUnit%2 == 0) {
        struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp] = nValue;
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nFrontBkgLamp/MAX_LAMP_PER_BOARD,
                           0,
                           0,
                           m_nFrontBkgLamp%MAX_LAMP_PER_BOARD,
                           nValue/256,
                           nValue%256,
                           3);
    } else {
        struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp] = nValue;
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nRearBkgLamp/MAX_LAMP_PER_BOARD,
                           0,
                           0,
                           m_nRearBkgLamp%MAX_LAMP_PER_BOARD,
                           nValue/256,
                           nValue%256,
                           3);
    }
}

/* 响应相机板号变化事件 */
void autoSetPage::onBoardNumChangedSlt()
{
    refreshValueBarDisplay();
}

/* 调整灵敏度 */
void autoSetPage::adjustSens(void)
{
    int nIdeTotal = struCnfg.struLevelInfo[struGsh.nLevel].nIdentifyGroupTotal;
    if(struCnfg.nParamSetAlone == 0) {  //! 分次设置
        //! 保存全局灵敏度至局部变量中,并将全局灵敏度设置为固定值150
        for(int i = 0; i < nIdeTotal; i++) {
            sensTemp[i] = struCnfp.struGroupIdentify[0][i].struRice.nSens;
            struCnfp.struGroupIdentify[0][i].struRice.nSens = 150;
        }
    } else {    //! 独立设置
        //! 保存全局灵敏度至局部变量中,并将全局灵敏度设置为固定值150
        for (int i = 0; i < MAX_UNIT; i++) {
            for (int j = 0; j < 2; j++) {
                sensSepatateTemp[i][j] = struCnfp.nSens[i][j];
                struCnfp.nSens[i][j] = 150;
            }
        }
    }

    myMonoFlow.sendAllSensParams();
}

/* 恢复灵敏度 */
void autoSetPage::resumeSens(void)
{
    int nIdeTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
    if(struCnfg.nParamSetAlone == 0) {  //! 分次设置
        for(int i = 0; i<nIdeTotal; i++) {
            struCnfp.struGroupIdentify[0][i].struRice.nSens = sensTemp[i];
        }
    } else {    //! 独立设置
        for (int i = 0; i < MAX_UNIT; i++) {
            for (int j = 0; j < 2; j++) {
                struCnfp.nSens[i][j] = sensSepatateTemp[i][j];
            }
        }
    }

    myMonoFlow.sendAllSensParams();
}

/* 校准模拟增益 */
void autoSetPage::calibrateGain(int nFrontMode, int nRearMode)
{
    if (nFrontMode == -1 && nRearMode == -1) {
        return;
    }

    for (int k = 0; k < 2; k++) {
        /* 1.发送正常色选命令 */
        myMonoFlow.sendOnOffParams(1);

        /* 2.开启振动器 */
        struGsh.bStatFeed = 1;
        myFlow.resetFeeder(0);
        myFlow.resetFeeder(1);
        myFlow.sleep(8);

        /* 3.关闭振动器 */
        struGsh.bStatFeed = 0;
        myFlow.resetFeeder(1);
        myFlow.sleep(3);

        /* 4.发送停止色选命令 */
        myMonoFlow.sendOnOffParams(0);
        myFlow.sleep(1);

        /* 5.主配通过修改模拟增益将物料亮度拉至正选200/反选150 */
        for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal-1; i+=2) {
            if (nFrontMode == 1) {
                lightAuto(i, 200);
            }
            if (nFrontMode == 0) {
                lightAuto(i, 150);
            }
        }
        for (int i = 1; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i+=2) {
            if (nRearMode == 1) {
                lightAuto(i, 200);
            }
            if (nRearMode == 0) {
                lightAuto(i, 150);
            }
        }

    }
}

/* 通过修改模拟增益设置物料亮度值 */
void autoSetPage::lightAuto(int unit,int target)
{
    int avr = 0;
    char buf[18];
    memset(buf, 0, sizeof(buf));

    /* 获取物料亮度 */
    MySerial.com1Write(CMD_UNIT_GET_LIGHT, UNIT, 0, unit, 0, 0, 0, 0, 0, 3);
    myFlow.msleep(10);
    MySerial.com1Read(buf, 18);

    if(buf[0] == 0xAA && buf[1] == 0xAA
            && buf[2] == 0xAA && buf[3] == 0xAA) {
        avr = buf[10];
    } else {
        avr = 0;
        return;
    }

    /* 若获取的物料亮度与背景亮度值相等则退出 */
    if (buf[10] == buf[11]) {
        return;
    }
    
    if (abs(avr-target) <= 5) {
        return;
    } else {
        if (avr < 255 && avr > 0) {
            if(target > avr) {
                struCnfp.nAnaGain[unit] += target-avr;
            } else {
                struCnfp.nAnaGain[unit] -= avr-target;
            }
        }
        if(avr >= 240) {
            struCnfp.nAnaGain[unit] = struCnfp.nAnaGain[unit]/2;
        }
        if(struCnfp.nAnaGain[unit] <= 0) {
            struCnfp.nAnaGain[unit] = 0;
        }
        if(struCnfp.nAnaGain[unit] >= 1023) {
            struCnfp.nAnaGain[unit] = 1023;
        }

        myMonoFlow.sendAnaGainParams(unit);
    }
}

/*!
 * \brief 辅配自动校正
 * \param nUnit 辅配相机地址
 * \param nTarget 调整目标值
 */
void autoSetPage::assistLightAuto(int nUnit, int nTarget)
{
}

/* 通过调节背景光强调整背景 */
void autoSetPage::adjustBackground(int index)
{
    int nPacketSize = struCnfc.nSensorPixel+16;
    char buf[nPacketSize];
    int i, j;
    int avr, sum;
    int sign;
    memset(buf, 0, sizeof(buf));
    int reference = struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp];
    int nTarget;
	int nFrontAnaGainAvr, nRearAnaGainAvr;
	int nCamNum = struCnfg.struLevelInfo[0].nUnitLevelTotal;
	int nBegin = 300;
	int nEnd = struCnfc.nSensorPixel-300;
	int nNum = nEnd-nBegin;

	/* 计算前后视增益平均值 */
	sum = 0;
    for (i = 0; i < nCamNum; i+=2) {
		sum += struCnfp.nAnaGain[i];
	}
	nFrontAnaGainAvr = sum/(nCamNum/2);
	sum = 0;
    for (i = 1; i < nCamNum; i+=2) {
		sum += struCnfp.nAnaGain[i];
	}
	nRearAnaGainAvr = sum/(nCamNum/2);

    /* 前视背景调节 */
    if (index == 0) {
        nTarget = struCnfp.nGainTarget[2*(struCnfg.nAutoReferChute-1)];
        sign = 1;
        MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, 2*(struCnfg.nAutoReferChute-1),
                           0, 0, 0, 0, 0, 1);
        myFlow.msleep(300);
        MySerial.com1Read(buf, nPacketSize);
        sum = 0;
        for (i = nBegin; i < nEnd; i++) {
            sum += buf[10+i];
        }
        avr = sum/nNum;
        if(abs(avr-nTarget) <= 5) {
            sign = 0;
        }

        /* 需要调背景 */
        if (sign) {
            for (i = 0; i < 8; i++) {
                /* 参考滑槽前视光源亮度值校准 */
                MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nFrontBkgLamp/MAX_LAMP_PER_BOARD,
                                   0,
                                   0,
                                   m_nFrontBkgLamp%MAX_LAMP_PER_BOARD,
                                   reference/256,
                                   reference%256,
                                   3);
                myFlow.sleep(1);
                MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, 2*(struCnfg.nAutoReferChute-1),
                                   0, 0, 0, 0, 0, 1);
                myFlow.msleep(300);
                MySerial.com1Read(buf, nPacketSize);
                sum = 0;
                for (j = nBegin; j < nEnd; j++) {
                    sum += buf[10+j];
                }
                avr = sum/nNum;

                /* 背景目标值为增益目标 */
                if(abs(avr-nTarget) <= 5) {
                    break;
                } else {
                    if (avr > nTarget) {
                        reference -= (avr-nTarget)/2;
                    } else {
                        reference += (nTarget-avr)/2;
                    }

                    if (reference < 0) {
                        reference = 0;
                    }
                }
            }
        }
        struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp] = reference;

        for (i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal-1; i+=2) {
            avr = 0;
            sum = 0;
            sign = 0;
            MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, i, 0, 0, 0, 0, 0, 1);
            myFlow.msleep(300);
            MySerial.com1Read(buf, nPacketSize);
            for (j = nBegin; j < nEnd; j++) {
                sum += buf[10+j];
            }
            avr = sum/nNum;

            if(abs(avr-nTarget) >= 10) {
                sign = 1;
            }

            int nDif = abs(struCnfp.nAnaGain[i]-nFrontAnaGainAvr);
            if (sign && nDif > 200) {
                struCnfp.nAnaGain[i] = nFrontAnaGainAvr;
                myMonoFlow.sendAnaGainParams(i);
            }
        }
    } else {        // 后视背景调节
        nTarget = struCnfp.nGainTarget[2*(struCnfg.nAutoReferChute-1)+1];
        sign = 1;
        reference = struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp];
        MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, 2*(struCnfg.nAutoReferChute-1)+1,
                           0, 0, 0, 0, 0, 1);
        myFlow.msleep(300);
        MySerial.com1Read(buf, nPacketSize);
        sum = 0;
        for (i = nBegin; i < nEnd; i++) {
            sum += buf[10+i];
        }
        avr = sum/nNum;
        if(abs(avr-nTarget) <= 5) {
            sign = 0;
        }

        /* 需要调背景 */
        if (sign) {
            for(i = 0; i < 8; i++) {
                /* 参考滑槽后视光源亮度值校准 */
                MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, m_nRearBkgLamp/MAX_LAMP_PER_BOARD,
                                   0,
                                   0,
                                   m_nRearBkgLamp%MAX_LAMP_PER_BOARD,
                                   reference/256,
                                   reference%256,
                                   3);
                myFlow.sleep(1);
                MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, 2*(struCnfg.nAutoReferChute-1)+1,
                                   0, 0, 0, 0, 0, 1);
                myFlow.msleep(300);
                MySerial.com1Read(buf, nPacketSize);
                sum = 0;
                for (j = nBegin; j < nEnd; j++) {
                    sum += buf[10+j];
                }
                avr = sum/nNum;
                
                if (abs(avr-nTarget) <= 5) {
                    break;
                } else {
                    if (avr > nTarget) {
                        reference -= (avr-nTarget)/2;
                    } else {
                        reference += (nTarget-avr)/2;
                    }

                    if (reference < 0) {
                        reference = 0;
                    }
                }
            }
        }
        struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp] = reference;

        for (i = 1; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i+=2) {
            avr = 0;
            sum = 0;
            sign = 0;
            MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, i, 0, 0, 0, 0, 0, 1);
            myFlow.msleep(300);
            MySerial.com1Read(buf, nPacketSize);
            for (j = nBegin; j < nEnd; j++) {
                sum += buf[10+j];
            }
            avr = sum/nNum;

            if (abs(avr-nTarget) >= 10) {
                sign = 1;
            }
	    
            int nDif = abs(struCnfp.nAnaGain[i]-nRearAnaGainAvr);
            if (sign && nDif > 200) {
                struCnfp.nAnaGain[i] = nRearAnaGainAvr;
                myMonoFlow.sendAnaGainParams(i);
            }
        }
    }
}

/* 根据色选模式设置前后视背景是否校准标志位 */
void autoSetPage::setFlagByMode(int &nFront, int &nRear)
{
    /** 1:正选 物料亮度调节至200
        0:反选 物料亮度调节至150
        -1:不选 不调节物料亮度 */
    switch (struCnfg.nProfileMode[struCnfg.nProfile]) {
    case RS_DARK_SORT:
        nFront = -1;
        nRear = 1;
        break;
    case RS_DIFF_POSITIVE_SORT:
    case RS_YELLOW_WHITE_SORT:
        nFront = 1;
        nRear = 1;
        break;
    case RS_DIFF_NEGATIVE_SORT:
        nFront = 0;
        nRear = 0;
        break;
    case RS_WHITE_POSITIVE_SORT:
        nFront = 1;
        nRear = -1;
        break;
    case RS_WHITE_NEGATIVE_SORT:
        nFront = 0;
        nRear = -1;
        break;
    case RS_YELLOW_TRANS_SORT:
        nFront = 0;
        nRear = 1;
        break;
    case RS_YELLOW_YELLOW_NEGATIVE_SORT:
        nFront = 1;
        nRear = 0;
        break;
    case RS_WHITE_YELLOW_NEGATIVE_SORT:
        nFront = 1;
        nRear = 0;
        break;
    case RS_USER_DEFINED_SORT:
        /* 前视背景灯亮 */
        if (struCnfp.struGroupCtrl[0].nLamp[m_nFrontBkgLamp] == 1) {
            nFront = 1;
        } else {
            nFront = 0;
        }
        if (struCnfp.struGroupCtrl[0].nLamp[m_nRearBkgLamp] == 1) {
            nRear = 1;
        } else {
            nRear = 0;
        }
        break;
    default:
        break;
    }
}
