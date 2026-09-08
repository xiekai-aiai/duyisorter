/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setcolorcamera.cpp
 * \brief       彩色机型设置相机界面源文件
 * \date        2015.01.14
 */
#include "setcolorcamera.h"

setColorCamera::setColorCamera(QWidget *parent)
    : QWidget(parent)
{
    m_bIsSelected = false;
    config = g_Config::getInstance();
    setGeometry(0, 0, LCD_WIDTH-20,  LCD_HEIGHT-60);
    setFixedSize(LCD_WIDTH-20,  LCD_HEIGHT-60);

    m_ncameraBodyID = 0;
    m_nChannelPageID = 0;
    m_nChannelPageID2 = 0;
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(0,0,width(),height());

    createSetColorCameraPage();     //! 创建彩色相机设置界面
    createCalibrationSignalPage();  //! 创建校准彩色信号界面
    createCalibrationWavePage();    //! 创建校正波形显示界面
    createSetBiasingPage();         //! 创建设置偏置界面
    createGainAdjustPage();         //! 创建增益调节界面
    createDivChannelPage();         //! 创建划分象元界面
    createChannelListPage();        //! 象元列表页面
    createChannelRangePage();       //! 象元范围页面
    createSensorParametersPage();   //! 创建传感器参数界面
    createLensParametersPage();     //! 创建镜头参数界面
    createContrapositionPage();     //! 创建对位测试界面

    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackWidgetIndexChangedSlt(int)));
}

setColorCamera::~setColorCamera()
{
}

/* 生成彩色相机设置界面 */
void setColorCamera::createSetColorCameraPage()
{
    mainWidget = new QWidget;
    mainWidget->setGeometry(0,0,width(),height());

    QGridLayout *upLay = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(mainWidget);
    calibrationColorSignalBtn = new myPushButton(myLan.color_calibrate, QIcon(), true, true);
//    calibrationColorSignalBtn = new myPushButton("白平衡", QIcon(), true, true);
    gainAdjustBtn = new myPushButton(myLan.color_gain_adjust, QIcon(), true, true);
    divChannelBtn = new myPushButton(myLan.channel_divide, QIcon(), true, true);
    setBiasingBtn = new myPushButton(myLan.color_biasing, QIcon(), true, true);
    lensParametersBtn = new myPushButton(myLan.color_lens_params, QIcon(), true, true);
    sensorParametersBtn = new myPushButton(myLan.sensor, QIcon(), true, true);
    contrapositionBtn = new myPushButton(myLan.contra_test,QIcon(),true,true);

    mainBackBtn = new myPushButton(myLan.back, QIcon());
    calibrationColorSignalBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    gainAdjustBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    divChannelBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    lensParametersBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setBiasingBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    sensorParametersBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    contrapositionBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    mainBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    gainAdjustBtn->hide();
    upLay->addWidget(calibrationColorSignalBtn, 0, 0);
    upLay->addWidget(gainAdjustBtn, 0, 1);
    upLay->addWidget(divChannelBtn, 0, 2);
    upLay->addWidget(sensorParametersBtn, 0, 3);
    upLay->addWidget(lensParametersBtn, 0, 4);

    upLay->setContentsMargins(10, 80, 10, 10);
    upLay->setHorizontalSpacing(50);
    upLay->setVerticalSpacing(45);
    downLay->addStretch();
    downLay->addWidget(mainBackBtn, Qt::AlignRight);
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    stackedWidget->addWidget(mainWidget);
    connect(mainBackBtn, SIGNAL(pressed()), this, SLOT(onMainBackBtnClicked()));
    connect(calibrationColorSignalBtn, SIGNAL(pressed()), this, SLOT(onCalibrationColorSignalClicked()));
    connect(setBiasingBtn, SIGNAL(pressed()), this, SLOT(onSetBiasingBtnClicked()));
    connect(gainAdjustBtn, SIGNAL(pressed()), this, SLOT(onGainAdjustBtnClicked()));
    connect(divChannelBtn, SIGNAL(pressed()), this, SLOT(onDivChannelBtnClicked()));
    connect(sensorParametersBtn, SIGNAL(pressed()), this, SLOT(onSensorParametersBtnClicked()));
    connect(lensParametersBtn, SIGNAL(pressed()), this, SLOT(onLensParametersBtnClicked()));
    connect(contrapositionBtn, SIGNAL(pressed()), this, SLOT(onContrapositionBtnClicked()));
}

/* 生成校准彩色信号界面 */
void setColorCamera::createCalibrationSignalPage()
{   
    int space;

    calibrationColorSignalWiget = new QWidget;
    calibrationColorSignalWiget->setGeometry(0, 0, width(), height());

    calibrationAxis = new MyCurve;
    calibrationAxis->setCurveType(2);
    calibrationAxis->setMinimumHeight(CURVE_WIDGET_HEIGHT);

    QHBoxLayout *sliderLay = new QHBoxLayout;
    QVBoxLayout *midLeftLay = new QVBoxLayout;
    QVBoxLayout *midRightLay = new QVBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(calibrationColorSignalWiget);

    calibrationTabBar = new QTabBar(calibrationColorSignalWiget);
    calibrationTabBar->setMinimumWidth(300);
    calibrationTabBar->setFont(config->getFont(DEFAULT_FONT_SIZE));
    calibrationTabBar->addTab(myIcon.Mark_Red, myLan.red);
    calibrationTabBar->addTab(myIcon.Mark_Green, myLan.green);
    calibrationTabBar->addTab(myIcon.Mark_Blue, myLan.blue);

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    calibrationLcdNum = new myLCDNumber(3, struCnfc.nRefRed, calibrationColorSignalWiget);
    calibrationLcdNum->setFixedWidth(120);
    calibrationLcdNum->setFixedHeight(btnSize.height());

    calibrationMinusBtn = new myPushButton(myIcon.Action_Minus,btnSize,calibrationColorSignalWiget);
    calibrationPlusBtn = new myPushButton(myIcon.Action_Plus, btnSize, calibrationColorSignalWiget);

    calibrationGetBtn = new myPushButton(myLan.update_capture, QIcon());
//    calibrationGetBtn = new myPushButton("单次校正", QIcon());

    calibrationGetBtn->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);
    calibrationWholeBtn = new myPushButton(myLan.whole_calibrate, QIcon());
    calibrationWholeBtn->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);
    calibrationBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    calibrationBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    calibrationWaveBtn = new myPushButton(myLan.adjust_wave, myIcon.Chart_Line);
    calibrationWaveBtn->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);
    digGainUpperLabel = new QLabel(myLan.gain_rough);
    digGainUpperLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    analogGainLabel2 = new QLabel(myLan.color_analog_gain);
    digitalGainLabel2 = new QLabel(myLan.color_digital_gain);
    //todo
    targetLabel2 = new QLabel(myLan.target_value);
    analogGainLabel2->setFont(config->getFont(DEFAULT_FONT_SIZE));
    digitalGainLabel2->setFont(config->getFont(DEFAULT_FONT_SIZE));
    targetLabel2->setFont(config->getFont(DEFAULT_FONT_SIZE));

    targetLabel2->setFixedWidth(BTN_WIDTH-10);
    analogGainLabel2->setFixedWidth(BTN_WIDTH-10);
    digitalGainLabel2->setFixedWidth(BTN_WIDTH-10);

    //模拟增益
    QHBoxLayout *lay = new QHBoxLayout;
    anaGainPlusBtn2 = new myPushButton(myIcon.Action_Plus, btnSize);
    anaGainMinusBtn2 = new myPushButton(myIcon.Action_Minus, btnSize);

    anaGainLcdNum2 = new QLCDNumber(1);
    anaGainLcdNum2->setFixedSize(120, btnSize.height());
    anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);

    anaGainLcdNumD22 = new myLCDNumber;
    anaGainLcdNumD22->setDigitCount(4);
    anaGainLcdNumD22->setFixedSize(120, btnSize.height());
    anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);

    if (struCnfc.nSensorType == SENSOR_D2) {
        anaGainLcdNum2->hide();
        anaGainLcdNumD22->show();
    } else {
        anaGainLcdNum2->show();
        anaGainLcdNumD22->hide();
    }

    lay->addWidget(analogGainLabel2,0,Qt::AlignLeft);
    lay->addWidget(anaGainMinusBtn2,0,Qt::AlignRight);
    lay->addWidget(anaGainLcdNum2,0,Qt::AlignRight);
    lay->addWidget(anaGainLcdNumD22,0,Qt::AlignRight);
    lay->addWidget(anaGainPlusBtn2,0,Qt::AlignRight);
//    anaGainGroupLay->addLayout(lay);



    sliderLay->addWidget(targetLabel2,0,Qt::AlignLeft);
    sliderLay->addWidget(calibrationMinusBtn,0,Qt::AlignRight);
    sliderLay->addWidget(calibrationLcdNum,0,Qt::AlignRight);
    sliderLay->addWidget(calibrationPlusBtn,0,Qt::AlignRight);
    midLeftLay->addWidget(calibrationTabBar,1,Qt::AlignRight);
//    midLeftLay->addSpacing(20);
    midLeftLay->addLayout(sliderLay);
//    midLeftLay->addSpacing(20);
    midLeftLay->addLayout(lay);

    midRightLay->addStretch(1);
    switch(LCD_WIDTH) {
    case 1024:
        midRightLay->addWidget(calibrationGetBtn,Qt::AlignLeft);
        midRightLay->addWidget(calibrationWholeBtn,Qt::AlignLeft);
        calibrationTabBar->setStyleSheet("QTabBar::tab{""min-height:45;min-width:100;}");
        space = 50;
        break;
    case 640:
        midRightLay->setContentsMargins(5, 30, 5, 5);
        midRightLay->addWidget(calibrationGetBtn);
        midRightLay->addWidget(calibrationWholeBtn);
        midRightLay->addWidget(calibrationBackBtn);
        calibrationTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}");
        space = 5;
        break;
    default:
        break;
    }

    QHBoxLayout *layout2 = new QHBoxLayout;
    QHBoxLayout *layout3 = new QHBoxLayout;
    digGainUpperLabel2 = new QLabel(myLan.gain_rough);
    digGainUpperLabel2->setFont(config->getFont(DEFAULT_FONT_SIZE));
    digGainUpperPlusBtn2 = new myPushButton(myIcon.Action_Plus, btnSize);
    digGainUpperMinusBtn2 = new myPushButton(myIcon.Action_Minus, btnSize);
    digGainUpperLcdNum2 = new QLCDNumber(2);
    digGainUpperLcdNum2->setFixedSize(120, btnSize.height());
    digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
    digGainLowLabel2 = new QLabel(myLan.gain_tiny);
    digGainLowLabel2->setFont(config->getFont(DEFAULT_FONT_SIZE));
    int nVal = struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit];
    digGainValueBar2 = new myValueBar(0, 63, 1, 10, nVal);

//    layout2->setSpacing(10);
//    layout2->addStretch(0);
    layout2->addWidget(digitalGainLabel2,0,Qt::AlignLeft);
    layout2->addWidget(digGainUpperMinusBtn2,0,Qt::AlignRight);
    layout2->addWidget(digGainUpperLcdNum2,0,Qt::AlignRight);
    layout2->addWidget(digGainUpperPlusBtn2,0,Qt::AlignRight);
//    layout2->addStretch(0);

    layout3->setSpacing(10);
    layout3->addStretch(0);
    layout3->addWidget(digGainLowLabel2);
    layout3->addWidget(digGainValueBar2);
    layout3->addStretch(0);
    QHBoxLayout *digGainGroupLay = new QHBoxLayout;
    midLeftLay->addLayout(layout2);
//    digGainGroupLay->addLayout(layout2);
//    digGainGroupLay->addLayout(layout3);
//    midRightLay->addStretch(1);
    midRightLay->addLayout(layout3,Qt::AlignLeft);
	//! 考虑RDC增加自动复制按钮的布局
	downLay->addSpacing(70);
    downLay->addLayout(midLeftLay);
    downLay->addStretch();

//    downLay->addSpacing(space);
    downLay->addLayout(midRightLay);
    mainLay->addWidget(calibrationAxis);
    mainLay->addLayout(downLay);
    mainLay->addWidget(calibrationBackBtn,0,Qt::AlignBottom|Qt::AlignRight);

    stackedWidget->addWidget(calibrationColorSignalWiget);

    connect(calibrationLcdNum, SIGNAL(pressed()), this, SLOT(refreshCalValue()));
    connect(calibrationBackBtn,SIGNAL(pressed()),this,SLOT(onCalibrationBackBtnClicked()));
    connect(calibrationMinusBtn, SIGNAL(pressed()), this, SLOT(onCalibrationMinusBtnClicked()));
    connect(calibrationPlusBtn, SIGNAL(pressed()), this, SLOT(onCalibrationPlusBtnClicked()));
    connect(calibrationTabBar, SIGNAL(currentChanged(int)), this, SLOT(onCalibrationTabBarChangedSlt(int)));
    connect(calibrationGetBtn, SIGNAL(pressed()), this, SLOT(onCalObtainBtnClicked()));
    connect(calibrationWaveBtn,SIGNAL(pressed()),this,SLOT(onCalWaveBtnClicked()));
    connect(calibrationWholeBtn, SIGNAL(pressed()), this, SLOT(onCalWholeBtnClicked()));
    connect(this, SIGNAL(sCalibrationCurveStart()), calibrationAxis, SLOT(curveStartSlt()));
    connect(this, SIGNAL(sCalibrationCurveStop()), calibrationAxis, SLOT(curveStopSlt()));
    connect(calibrationAxis,SIGNAL(boardNumChanged()),this,SLOT(onCalibrationViewChangedSlt()));

    connect(anaGainPlusBtn2, SIGNAL(pressed()), this, SLOT(onAnaGainPlusBtn2Clicked()));
    connect(anaGainMinusBtn2, SIGNAL(pressed()), this, SLOT(onAnaGainMinusBtn2Clicked()));
    connect(anaGainLcdNumD22, SIGNAL(pressed()), this, SLOT(onAnaGainLcdNumD22Pressed()));
    connect(digGainUpperPlusBtn2,SIGNAL(pressed()),this,SLOT(onDigUpperPlusBtn2Clicked()));
    connect(digGainUpperMinusBtn2,SIGNAL(pressed()),this,SLOT(onDigUpperMinusBtn2Clicked()));
    connect(digGainValueBar2, SIGNAL(valueChanged(int)), this, SLOT(onDigGainLowValue2ChangedSlt()));

}


/* 设置模拟增益的值 */
void setColorCamera::setAnaGainParams2()
{
    switch(calibrationTabBar->currentIndex()){
    case 0:
        if (struCnfc.nSensorType == SENSOR_D2) {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] = anaGainLcdNumD22->value();
        } else {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] = anaGainLcdNum2->value();
        }
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] / 256,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] % 256, 3);
        break;
    case 1:
        if (struCnfc.nSensorType == SENSOR_D2) {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] = anaGainLcdNumD2->value();
        } else {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] = anaGainLcdNum2->value();
        }
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] / 256,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] % 256, 3);

       break;
    case 2:
        if (struCnfc.nSensorType == SENSOR_D2) {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] = anaGainLcdNumD22->value();
        } else {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] = anaGainLcdNum2->value();
        }
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] / 256,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] % 256, 3);

        break;
    default:
        break;
    }
}

/* 响应模拟增益中的增加按钮 */
void setColorCamera::onAnaGainPlusBtn2Clicked()
{
    int tmp = 0;
    if (struCnfc.nSensorType == SENSOR_D2) {
        tmp = anaGainLcdNumD22->value();
        if (tmp < 1023) {
           anaGainLcdNumD22->display(tmp+1);
        } else {
           anaGainLcdNumD22->display(1023);
        }
    } else {
        tmp = anaGainLcdNum2->value();
        if (tmp > 7) {
            tmp = 7;
            anaGainLcdNum2->display(7);
        }
        if(tmp == 3) {
            anaGainLcdNum2->display(7);
        } else {
            if (tmp < 3) {
                anaGainLcdNum2->display(tmp+1);
            }
        }
    }
    setAnaGainParams2();
}

/* 响应模拟增益中的减小按钮 */
void setColorCamera::onAnaGainMinusBtn2Clicked()
{
    int tmp = 0;
    if (struCnfc.nSensorType == SENSOR_D2) {
        tmp = anaGainLcdNumD22->value();
        if(tmp > 0)
           anaGainLcdNumD22->display(tmp - 1);
        else
           anaGainLcdNumD22->display(1023);
    } else {
        tmp = anaGainLcdNum2->value();
        if (tmp > 7) {
            tmp = 7;
        }
        if(tmp == 7) {
            anaGainLcdNum2->display(3);
        } else {
            if (tmp > 0) {
                anaGainLcdNum2->display(tmp - 1);
            }
        }
    }
    setAnaGainParams2();
}

void setColorCamera::onAnaGainLcdNumD22Pressed()
{
    int tmp = anaGainLcdNumD22->value();
    myInputPanel inputDlg(intType, 0, 1023, tmp);
    int ret = inputDlg.exec();
    if(ret == QDialog::Accepted){
        anaGainLcdNumD22->display(inputDlg.getValue());
        setAnaGainParams2();
    }
}

/* 设置数字增益的值 */
void setColorCamera::setDigGainUpperParams2()
{
    switch(calibrationTabBar->currentIndex()){
    case 0:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit] = digGainUpperLcdNum2->value();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit], 3);
        break;
    case 1:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit] = digGainUpperLcdNum2->value();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit], 3);
        break;
    case 2:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit] = digGainUpperLcdNum2->value();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit], 3);
        break;
    default:
        break;
    }
}

/* 设置数字增益细调的值 */
void setColorCamera::setDigGainLowParams2()
{
    switch(calibrationTabBar->currentIndex()){
    case 0:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit] = digGainValueBar2->getValue();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                       struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit], 3);
        break;
    case 1:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit] = digGainValueBar2->getValue();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                       struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit], 3);
        break;
    case 2:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit] = digGainValueBar2->getValue();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                       struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit], 3);
        break;
    default:
        break;
    }
}

/* 响应数字增益置中的粗调增加按钮 */
void setColorCamera::onDigUpperPlusBtn2Clicked()
{
    int tmp = digGainUpperLcdNum2->value();
    if(tmp < 15)
        digGainUpperLcdNum2->display(tmp+1);
    setDigGainUpperParams2();
}

/* 响应数字增益置中的粗调减小按钮 */
void setColorCamera::onDigUpperMinusBtn2Clicked()
{
    int tmp = digGainUpperLcdNum2->value();
    if(tmp > 0)
        digGainUpperLcdNum2->display(tmp-1);
    setDigGainUpperParams2();
}

/* 响应数字增益中细调值变化的事件 */
void setColorCamera::onDigGainLowValue2ChangedSlt()
{
    setDigGainLowParams2();
}
/* 生成校正波形界面 */
void setColorCamera::createCalibrationWavePage()
{
    calibrateWaveWidget = new QWidget;
    calibrateWaveWidget->setGeometry(0,0,width(),height());

    QVBoxLayout *downLeftLay = new QVBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(calibrateWaveWidget);

    calibrateWaveAxis = new MyCurve;
    calibrateWaveAxis->setCurveType(2);
    calibrateWaveAxis->setMinimumHeight(CURVE_WIDGET_HEIGHT);

    calibrateCopyBtn = new myPushButton(myLan.auto_copy, QIcon());
    calibrateCopyBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    profileValueBar = new myValueBar(1, struCnfg.nProfileTotal, 1, 1, 1);
    profileValueBar->setFixedSize(BTN_WIDTH*2,BTN_HEIGHT);
    profileValueBar->hideFastChangeBtns();


    QSize btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    calibrateWaveBackBtn = new myPushButton(myLan.back, myIcon.Action_Back, calibrateWaveWidget);
    calibrateWaveBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    downLay->addWidget(calibrateCopyBtn);
    downLay->addWidget(profileValueBar);

    mainLay->addWidget(calibrateWaveAxis);
    mainLay->addSpacing(70);
    mainLay->addLayout(downLay);
    mainLay->addSpacing(50);
    mainLay->addWidget(calibrateWaveBackBtn,0,Qt::AlignRight);
    connect(calibrateWaveBackBtn,SIGNAL(pressed()),this,SLOT(onCalibrateWaveBackBtnClicked()));
    connect(calibrateWaveAxis, SIGNAL(boardNumChanged()), this, SLOT(onBoardNumChangedSlt()));
    connect(calibrateCopyBtn,SIGNAL(pressed()),this,SLOT(onCalibrateCopyBtnPressed()));

    stackedWidget->addWidget(calibrateWaveWidget);
}

/* 生成设置偏置界面 */
void setColorCamera::createSetBiasingPage()
{
    setBiasingWidget = new QWidget;
    setBiasingWidget->setGeometry(0,0,width(),height());

    QVBoxLayout *downLeftLay = new QVBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(setBiasingWidget);

    setBiasingAxis = new MyCurve;
    setBiasingAxis->setCurveType(2);
    setBiasingAxis->setMinimumHeight(CURVE_WIDGET_HEIGHT);

    for(int i=0; i<RGBNum; i++){
        setBiasingLine[i] = new QLabel(setBiasingWidget);
        setBiasingLine[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        downLeftLay->addWidget(setBiasingLine[i]);
        setBiasingLine[i]->hide();
    }
    QString text;
    text = QString("%1 : 0").arg(myLan.red);
    setBiasingLine[0]->setText(text);
    text = QString("%1 : 0").arg(myLan.green);
    setBiasingLine[1]->setText(text);
    text = QString("%1 : 0").arg(myLan.blue);
    setBiasingLine[2]->setText(text);

    QVBoxLayout *downMidLay = new QVBoxLayout;

    biasingTabBar = new QTabBar;
    biasingTabBar->setMinimumWidth(300);
    biasingTabBar->setFont(config->getFont(DEFAULT_FONT_SIZE));

    biasingTabBar->addTab(myIcon.Mark_Red, myLan.red);
    biasingTabBar->addTab(myIcon.Mark_Green, myLan.green);
    biasingTabBar->addTab(myIcon.Mark_Blue, myLan.blue);
    int nVal = struCnfc.struLevelCamera[struGsh.nLevel].nBiasingRed[struGsh.nUnit];
    biasingValueBar = new myValueBar(0, 31, 1, 10, nVal);
    if (struCnfc.nSensorType == SENSOR_D2) {
        biasingTabBar->removeTab(2);//blue不可调
        biasingValueBar->setMaxValue(255);
    } else {
        biasingTabBar->setMinimumWidth(300);
    }

    switch(LCD_WIDTH){
    case 1024:
        biasingTabBar->setStyleSheet("QTabBar::tab{""min-height:45;min-width:100;}");
        break;
    case 640:
        if (struCnfc.nSensorType == SENSOR_D2) {
            biasingTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:150;}");
        } else {
            biasingTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}");
        }
        break;
    }

    downMidLay->addWidget(biasingTabBar);
    downMidLay->addWidget(biasingValueBar);

    QSize btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    setBiasingBackBtn = new myPushButton(myLan.back, myIcon.Action_Back, setBiasingWidget);
    setBiasingBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    downLay->addStretch();
    downLay->addLayout(downLeftLay);
    downLay->addSpacing(50);
    downLay->addLayout(downMidLay);
    downLay->addStretch();
    switch(LCD_WIDTH){
    case 1024:
        downLay->setContentsMargins(10, 10, 10, 20);
        break;
    case 640:
        downLay->setContentsMargins(10, 1, 10, 1);
        break;
    }
    mainLay->addWidget(setBiasingAxis);
    mainLay->addLayout(downLay);
    mainLay->addWidget(setBiasingBackBtn,0,Qt::AlignRight);

    connect(setBiasingBackBtn, SIGNAL(pressed()), this, SLOT(onSetBiasingBackBtnClicked()));
    connect(biasingTabBar, SIGNAL(currentChanged(int)), this, SLOT(onBiasingTabBarChangedSlt(int)));
    connect(biasingValueBar, SIGNAL(valueChanged(int)), this, SLOT(onBiasingValueChangedSlt(int)));
    connect(setBiasingAxis, SIGNAL(boardNumChanged()), this, SLOT(onBiasingBoardNumChangedSlt()));
    connect(this, SIGNAL(sBiasingCurveStart()), setBiasingAxis, SLOT(curveStartSlt()));
    connect(this, SIGNAL(sBiasingCurveStop()), setBiasingAxis, SLOT(curveStopSlt()));
    stackedWidget->addWidget(setBiasingWidget);
}

/* 生成增益调节界面 */
void setColorCamera::createGainAdjustPage()
{
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);

    gainAdjustWidget = new QWidget;
    gainAdjustWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(gainAdjustWidget);
    myGroupBox  *anaGainGroupBox = new myGroupBox(myLan.color_analog_gain);
    myGroupBox  *digGainGroupBox = new myGroupBox(myLan.color_digital_gain);
    QVBoxLayout *anaGainGroupLay = new QVBoxLayout(anaGainGroupBox);
    QVBoxLayout *digGainGroupLay = new QVBoxLayout(digGainGroupBox);
    anaGainGroupBox->setFixedHeight(155);
    digGainGroupBox->setFixedHeight(155);

    gainAdjustAxis = new MyCurve;
    gainAdjustAxis->setCurveType(2);
    gainAdjustAxis->setFixedHeight(CURVE_WIDGET_HEIGHT);

    //模拟增益调节控件
    GainAdjustTabBar = new QTabBar;
    GainAdjustTabBar->setFont(config->getFont(DEFAULT_FONT_SIZE));
    GainAdjustTabBar->adjustSize();
    GainAdjustTabBar->addTab(myIcon.Mark_Red, myLan.red);
    GainAdjustTabBar->addTab(myIcon.Mark_Green, myLan.green);
    GainAdjustTabBar->addTab(myIcon.Mark_Blue, myLan.blue);
    if (struCnfc.nSensorType == SENSOR_D2) {
        GainAdjustTabBar->removeTab(2);//blue不可调
        GainAdjustTabBar->setMinimumWidth(200);
    } else {
        GainAdjustTabBar->setMaximumWidth(400);
    }
    GainAdjustTabBar->setFixedHeight(55);
    GainAdjustTabBar->setUsesScrollButtons(false);

    switch(LCD_WIDTH){
    case 1024:
        GainAdjustTabBar->setStyleSheet("QTabBar::tab{""min-height:45;min-width:90;}");
        break;
    case 640:
        GainAdjustTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}");
        break;
    default:
        GainAdjustTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}");
        break;
    }
    QHBoxLayout *lay = new QHBoxLayout;
    anaGainPlusBtn = new myPushButton(myIcon.Action_Plus, btnSize);
    anaGainMinusBtn = new myPushButton(myIcon.Action_Minus, btnSize);

    anaGainLcdNum = new QLCDNumber(1);
    anaGainLcdNum->setFixedSize(btnSize.width() * 2, btnSize.height());
    anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);

    anaGainLcdNumD2 = new myLCDNumber;
    anaGainLcdNumD2->setDigitCount(4);
    anaGainLcdNumD2->setFixedSize(btnSize.width() * 2, btnSize.height());
    anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);

    if (struCnfc.nSensorType == SENSOR_D2) {
        anaGainLcdNum->hide();
        anaGainLcdNumD2->show();
    } else {
        anaGainLcdNum->show();
        anaGainLcdNumD2->hide();
    }

    lay->addWidget(anaGainMinusBtn);
    lay->addWidget(anaGainLcdNum);
    lay->addWidget(anaGainLcdNumD2);
    lay->addWidget(anaGainPlusBtn);
    anaGainGroupLay->addLayout(lay);

    QHBoxLayout *layout = new QHBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;
    digGainUpperLabel = new QLabel(myLan.gain_rough);
    digGainUpperLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    digGainUpperPlusBtn = new myPushButton(myIcon.Action_Plus, btnSize);
    digGainUpperMinusBtn = new myPushButton(myIcon.Action_Minus, btnSize);
    digGainUpperLcdNum = new QLCDNumber(2);
    digGainUpperLcdNum->setFixedSize(60, btnSize.height());
    digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
    digGainLowLabel = new QLabel(myLan.gain_tiny);
    digGainLowLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    int nVal = struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit];
    digGainValueBar = new myValueBar(0, 63, 1, 10, nVal);

    layout->setSpacing(10);
    layout->addStretch(0);
    layout->addWidget(digGainUpperLabel);
    layout->addWidget(digGainUpperMinusBtn);
    layout->addWidget(digGainUpperLcdNum);
    layout->addWidget(digGainUpperPlusBtn);
    layout->addStretch(0);

    layout2->setSpacing(10);
    layout2->addStretch(0);
    layout2->addWidget(digGainLowLabel);
    layout2->addWidget(digGainValueBar);
    layout2->addStretch(0);

    digGainGroupLay->addLayout(layout);
    digGainGroupLay->addLayout(layout2);

    btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    gainAdjustBackBtn = new myPushButton(myLan.back,myIcon.Action_Back,
                                         gainAdjustWidget);
    gainAdjustBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    downLay->addWidget(anaGainGroupBox);
    downLay->addWidget(digGainGroupBox);

    switch(LCD_WIDTH){
    case 1024:
        downLay->setContentsMargins(10, 0, 10, 0);
        break;
    case 640:
        downLay->setContentsMargins(10, 1, 10, 1);
        break;
    }

    QHBoxLayout *backDownLay = new QHBoxLayout;
    backDownLay->addWidget(GainAdjustTabBar);
    backDownLay->addWidget(gainAdjustBackBtn,0,Qt::AlignRight);
    backDownLay->setContentsMargins(10, 0, 10, 0);

    mainLay->addWidget(gainAdjustAxis);
    mainLay->addSpacing(0);
    mainLay->addLayout(downLay);
    mainLay->addLayout(backDownLay);

    connect(gainAdjustAxis, SIGNAL(boardNumChanged()), this, SLOT(onGainAdjustBoardNumChangedSlt()));
    connect(gainAdjustBackBtn, SIGNAL(pressed()), this, SLOT(onGainAdjustBackBtnClicked()));
    connect(this, SIGNAL(sGainAdjustCurveStart()), gainAdjustAxis, SLOT(curveStartSlt()));
    connect(this, SIGNAL(sGainAdjustCurveStop()), gainAdjustAxis, SLOT(curveStopSlt()));

    connect(GainAdjustTabBar, SIGNAL(currentChanged(int)), this, SLOT(onGainAdjustTabBarChangedSlt(int)));
    connect(anaGainPlusBtn, SIGNAL(pressed()), this, SLOT(onAnaGainPlusBtnClicked()));
    connect(anaGainMinusBtn, SIGNAL(pressed()), this, SLOT(onAnaGainMinusBtnClicked()));
    connect(anaGainLcdNumD2, SIGNAL(pressed()), this, SLOT(onAnaGainLcdNumD2Pressed()));
    connect(digGainUpperPlusBtn,SIGNAL(pressed()),this,SLOT(onDigUpperPlusBtnClicked()));
    connect(digGainUpperMinusBtn,SIGNAL(pressed()),this,SLOT(onDigUpperMinusBtnClicked()));
    connect(digGainValueBar, SIGNAL(valueChanged(int)), this, SLOT(onDigGainLowValueChangedSlt()));

    stackedWidget->addWidget(gainAdjustWidget);
}

void setColorCamera::onAnaGainLcdNumD2Pressed()
{
    int tmp = anaGainLcdNumD2->value();
    myInputPanel inputDlg(intType, 0, 1023, tmp);
    int ret = inputDlg.exec();
    if(ret == QDialog::Accepted){
        anaGainLcdNumD2->display(inputDlg.getValue());
        setAnaGainParams();
    }
}

/*!
 * \brief setColorCamera::createDivChannelPage
 */
void setColorCamera::createDivChannelPage()
{
    divChannelWidget = new QWidget;
    divChannelWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *downLay = new QHBoxLayout;
    QGridLayout *downRightLay = new QGridLayout;
    downRightLay->setSpacing(5);
    QVBoxLayout *mainLay = new QVBoxLayout(divChannelWidget);

    divChannelAxis = new MyCurve;
    divChannelAxis->setCurveType(2);
    divChannelAxis->setFixedHeight(CURVE_WIDGET_HEIGHT);

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    QGroupBox *ejectorGroup = new QGroupBox(myLan.ejector);
    ejectorGroup->setFont(config->getFont());
    ejectorGroup->setMaximumSize(180, 120);

    myLabel *beginChannelLabel = new myLabel(myLan.start);
    beginChannelLcdNum = new myLCDNumber;
    beginChannelLcdNum->setDigitCount(4);
    beginChannelLcdNum->setFixedSize(60, 30);

    myLabel *beginEjectorLabel = new myLabel(myLan.start);
    beginEjectorLcdNum = new myLCDNumber;
    beginEjectorLcdNum->setDigitCount(4);
    beginEjectorLcdNum->setFixedSize(60, 30);

//    QGroupBox *channelGroup = new QGroupBox(myLan.pixel);
    QGroupBox *channelGroup = new QGroupBox("");

    channelGroup->setFont(config->getFont());
    channelGroup->setMaximumSize(180, 120);

    myLabel *endChannelLabel = new myLabel(myLan.end);
    endChannelLcdNum = new myLCDNumber;
    endChannelLcdNum->setDigitCount(4);
    endChannelLcdNum->setFixedSize(60, 30);

    myLabel *endEjectorLabel = new myLabel(myLan.end);
    endEjectorLcdNum = new myLCDNumber;
    endEjectorLcdNum->setDigitCount(4);
    endEjectorLcdNum->setFixedSize(60, 30);

    QGridLayout *EjectorLayout = new QGridLayout(ejectorGroup);
    EjectorLayout->addWidget(beginEjectorLabel, 0, 0);
    EjectorLayout->addWidget(beginEjectorLcdNum, 0, 1);
    EjectorLayout->addWidget(endEjectorLabel, 1, 0);
    EjectorLayout->addWidget(endEjectorLcdNum, 1, 1);
    QGridLayout *channelLayout = new QGridLayout(channelGroup);
    channelLayout->addWidget(beginChannelLabel,   0, 0);
    channelLayout->addWidget(beginChannelLcdNum,  0, 1);
    channelLayout->addWidget(endChannelLabel,   1, 0);
    channelLayout->addWidget(endChannelLcdNum,  1, 1);

    btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    divChannelListBtn = new myPushButton(myLan.list, myIcon.Edit_List);
    cameraBodyBtn = new myPushButton("", QIcon());
    cameraBodyBtn->hide();
    if (m_ncameraBodyID) {
        cameraBodyBtn->setText(myLan.right_side);
    } else {
        cameraBodyBtn->setText(myLan.left_side);
    }

    QGroupBox *listGroup = new QGroupBox("");
    listGroup->setFont(config->getFont(FONT_SIZE_16));
    listGroup->setMinimumWidth(width()-190);
    QGridLayout *listLayout = new QGridLayout(listGroup);

    for (int i = 0; i < 5; i++) {
        for(int j = 0; j < 13; j++) {
            listLabel2[i][j] = new QLabel;
            listLabel2[i][j]->setFont(config->getFont(FONT_SIZE_16));
            listLabel2[i][j]->setAlignment(Qt::AlignHCenter);
            listLayout->addWidget(listLabel2[i][j], i, j);
        }
    }
    for (int i = 1; i < 13; i++) {
        listLabel2[0][i]->setText(QString("%1").arg(i));
    }

    autoDivChannelBtn = new myPushButton(myLan.auto_divide, QIcon());
    divChannelRangeBtn = new myPushButton(myLan.channel_range, QIcon());
    divChannelBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    divChannelListBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    cameraBodyBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    autoDivChannelBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    divChannelRangeBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    divChannelBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);


    divChannelListResetBtn2 = new myPushButton(myLan.channel_restore, QIcon());
    divChannelListResetBtn2->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    downRightLay->addWidget(autoDivChannelBtn, 0, 0,Qt::AlignRight);
    downRightLay->addWidget(channelGroup, 1, 0,Qt::AlignRight);
    downRightLay->addWidget(ejectorGroup, 2, 0,Qt::AlignRight);
    downRightLay->addWidget(divChannelListBtn, 3, 0,Qt::AlignRight);
    downRightLay->addWidget(divChannelListResetBtn2, 4, 0,Qt::AlignRight);
    divChannelListBtn->hide();

    //! 仅在矿石机下可设置起始及终止喷嘴
    ejectorGroup->hide();


//    downLay->addWidget(ejectorGroup);
//    downLay->addWidget(channelGroup);
    downLay->addWidget(listGroup,Qt::AlignLeft);
    downLay->addLayout(downRightLay);
    mainLay->addWidget(divChannelAxis);
    mainLay->addSpacing(0);
    mainLay->addLayout(downLay);
//    mainLay->addStretch(0);
    mainLay->addWidget(divChannelBackBtn,0,Qt::AlignRight);

    connect(divChannelBackBtn, SIGNAL(pressed()), this, SLOT(onDivChannelBackBtnClicked()));
    connect(cameraBodyBtn, SIGNAL(pressed()), this, SLOT(onCameraBodyBtnClicked()));
    connect(divChannelListBtn, SIGNAL(pressed()), this, SLOT(onDivChannelListBtnClicked()));
    connect(autoDivChannelBtn, SIGNAL(pressed()), this, SLOT(onAutoDivChannelBtnClicked()));
    connect(divChannelAxis, SIGNAL(boardNumChanged()), this, SLOT(onDivChannelBoardNumChangedSlt()));
    connect(this, SIGNAL(sDivChannelCurveStart()), divChannelAxis, SLOT(curveStartSlt()));
    connect(this, SIGNAL(sDivChannelCurveStop()), divChannelAxis, SLOT(curveStopSlt()));
    connect(beginChannelLcdNum, SIGNAL(pressed()), this, SLOT(onStartChannelPressed()));
    connect(endChannelLcdNum, SIGNAL(pressed()), this, SLOT(onEndChannelPressed()));
    connect(beginEjectorLcdNum, SIGNAL(pressed()), this, SLOT(onBeginEjectorPressed()));
    connect(endEjectorLcdNum, SIGNAL(pressed()), this, SLOT(onEndEjectorPressed()));
    connect(divChannelRangeBtn, SIGNAL(pressed()), this, SLOT(onDivChannelRangeBtnPressed()));
    connect(divChannelListResetBtn2, SIGNAL(clicked()), this, SLOT(onDivChannelListResetBtnClicked2()));

    stackedWidget->addWidget(divChannelWidget);
}

/*!
 * \brief setColorCamera::createChannelListPage
 */
void setColorCamera::createChannelListPage()
{
    divChannelListWidget = new QWidget;
    divChannelListWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *listDownLay = new QHBoxLayout;
    QHBoxLayout *listUpLay = new QHBoxLayout;
    QVBoxLayout *listMainLay = new QVBoxLayout(divChannelListWidget);

    QSize btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    QGroupBox *listGroup = new QGroupBox(myLan.list);
    listGroup->setFont(config->getFont());
    listGroup->setMinimumHeight(height()-150);
    QGridLayout *listLayout = new QGridLayout(listGroup);
    for (int i = 0; i < ROW_NUM; i++) {
        for(int j = 0; j < COLUMN_NUM; j++) {
            listLabel[i][j] = new QLabel;
            listLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            listLabel[i][j]->setAlignment(Qt::AlignHCenter);
            listLayout->addWidget(listLabel[i][j], i, j);
        }
    }
    for (int i = 1; i < ROW_NUM; i++) {
        listLabel[i][0]->setText(QString("%1").arg(i));
    }
    listLabel[0][0]->setText(myLan.chute);

    /* 茶叶机型划分象元列表显示使用文本编辑框 */
    m_textEdit = new QTextEdit();
    m_textEdit->setFont(config->getFont());
    m_textEdit->setReadOnly(true);
    m_textEdit->setWordWrapMode(QTextOption::NoWrap);


    divChannelListBackBtn = new myPushButton(myLan.back,myIcon.Action_Back);
    divChannelListBackBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    divChannelListPageConvertBtn = new myPushButton("", QIcon());
    divChannelListPageConvertBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    divChannelListPageConvertBtn2 = new myPushButton("", QIcon());
    divChannelListPageConvertBtn2->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    divChannelListPageConvertBtn->hide();

    divChannelListPageConvertBtn2->hide();

    divChannelListResetBtn = new myPushButton(myLan.channel_restore, QIcon());
    divChannelListResetBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    listUpLay->addWidget(listGroup);
//    listUpLay->setContentsMargins(15, 10, 20, 10);
    listDownLay->addWidget(divChannelListResetBtn);
    listDownLay->addStretch();
    listDownLay->addWidget(divChannelListPageConvertBtn2, 0, Qt::AlignRight);
    listDownLay->addWidget(divChannelListPageConvertBtn, 0, Qt::AlignRight);
    divChannelListPageConvertBtn->hide();
    divChannelListPageConvertBtn2->hide();
    listMainLay->addLayout(listUpLay);
    listMainLay->addLayout(listDownLay);
    listMainLay->addStretch(0);
    listMainLay->addWidget(divChannelListBackBtn,0,Qt::AlignRight);

    connect(divChannelListBackBtn, SIGNAL(clicked()), this, SLOT(onDivChannelListBackBtnClicked()));
    connect(divChannelListResetBtn, SIGNAL(clicked()), this, SLOT(onDivChannelListResetBtnClicked()));
    connect(divChannelListPageConvertBtn, SIGNAL(clicked()), this, SLOT(onDivChannelListPageConvertBtnClicked()));
    connect(divChannelListPageConvertBtn2, SIGNAL(clicked()), this, SLOT(onDivChannelListPageConvertBtn2Clicked()));

    stackedWidget->addWidget(divChannelListWidget);
}

/*!
 * \brief setColorCamera::createChannelRangePage
 */
void setColorCamera::createChannelRangePage()
{
    divChannelRangeWidget = new QWidget;
    divChannelRangeWidget->setGeometry(0, 0, width(), height());

    QGridLayout *upLay = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(divChannelRangeWidget);

    m_pixelBeginLabel = new myLabel(myLan.channel_begin);
    m_pixelBeginLbe = new myLineEdit(QString("%1").arg(struGsh.nPixelBegin));
    m_pixelEndLabel = new myLabel(myLan.channel_end);
    m_pixelEndLbe = new myLineEdit(QString("%1").arg(struGsh.nPixelEnd));
    m_channelRangeBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_pixelBeginLabel->setMaximumHeight(BTN_HEIGHT);
    m_pixelBeginLbe->setMaximumHeight(BTN_HEIGHT);
    m_pixelEndLabel->setMaximumHeight(BTN_HEIGHT);
    m_pixelEndLbe->setMaximumHeight(BTN_HEIGHT);
    m_channelRangeBackBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->setContentsMargins(20, 20, 20, 10);
    upLay->setSpacing(30);
    upLay->addWidget(m_pixelBeginLabel,0, 0);
    upLay->addWidget(m_pixelBeginLbe  ,0, 1);
    upLay->addWidget(m_pixelEndLabel  ,1, 0);
    upLay->addWidget(m_pixelEndLbe    ,1, 1);

    downLay->addStretch();
    downLay->addWidget(m_channelRangeBackBtn, Qt::AlignRight);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(m_pixelBeginLbe, SIGNAL(pressed()), this, SLOT(onPixelBeginBtnPressedSlt()));
    connect(m_pixelEndLbe, SIGNAL(pressed()), this, SLOT(onPixelEndBtnPressedSlt()));
    connect(m_channelRangeBackBtn, SIGNAL(pressed()), this, SLOT(onChannelRangeBackBtnPressedSlt()));

    stackedWidget->addWidget(divChannelRangeWidget);
}

/*　创建传感器参数界面　*/
void setColorCamera::createSensorParametersPage()
{
    sensorParametersWidget = new QWidget;
    sensorParametersWidget->setGeometry(0, 0, width(), height());
    setSensorParams();

    QGridLayout *upLay = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay =  new QVBoxLayout(sensorParametersWidget);

    // 传感器类型
    sensorTypeGroup = new myMutex(Qt::Vertical, m_nSensorType-1, sensorParametersWidget);
    sensorTypeGroup->setNum(5);
    sensorTypeGroup->setTitle(myLan.sensor);
    QStringList strList;
    strList << "K" << "T2" << "T3" << "D2" << "P5";
    sensorTypeGroup->setLabelText(strList);

    // 传感器帧频
    setHorizentalFreGroup = new QGroupBox(myLan.row_fre, sensorParametersWidget);
    setHorizentalFreGroup->setFont(config->getFont());
    QHBoxLayout *groupLay = new QHBoxLayout(setHorizentalFreGroup);
    horizontalFreNum = new myLCDNumber(3, m_fRowFre, setHorizentalFreGroup);
    horizontalFreNum->setFixedHeight(45);
    horizontalFreNum->setMinimumWidth(80);
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    sensorMinusBtn = new myPushButton("", myIcon.Action_Minus, setHorizentalFreGroup);
    sensorMinusBtn->setFixedSize(btnSize);
    sensorPlusBtn = new myPushButton("", myIcon.Action_Plus, setHorizentalFreGroup);
    sensorPlusBtn->setFixedSize(btnSize);
    groupLay->addWidget(sensorMinusBtn);
    groupLay->addWidget(horizontalFreNum);
    groupLay->addWidget(sensorPlusBtn);

    // 传感器物料模式
    sensorModeGroup = new myMutex(Qt::Vertical, m_nSensorMode-1, sensorParametersWidget);
    sensorModeGroup->setTitle(myLan.sensor_mode);
    sensorModeGroup->setLabelText(myLan.sensor_light, myLan.sensor_dark, myLan.sensor_black_white);

    // 传感器分辨率
    sensorPixelGroup = new QGroupBox(myLan.sensor_pixel, sensorParametersWidget);
    sensorPixelGroup->setFont(config->getFont());
    QGridLayout *sensorPixelGridLay = new QGridLayout(sensorPixelGroup);

    sensorPixelLabel[0] = new myLabel(myLan.upper_layer);
    sensorPixelLabel[1] = new myLabel(myLan.mid_layer);
    sensorPixelLabel[2] = new myLabel(myLan.lower_layer);

    for (int i = 0; i < MAX_LEVEL; i++) {
        sensorPixelCbx[i] = new MyCheckBox("2048", false);
    }

    for (int i = 0; i < MAX_LEVEL; i++) {
        sensorPixelGridLay->addWidget(sensorPixelLabel[i], i, 0, 1, 1, Qt::AlignLeft);
    }

    for (int i = 0; i < MAX_LEVEL; i++) {
        sensorPixelGridLay->addWidget(sensorPixelCbx[i], i, 1, 1, 1, Qt::AlignCenter);
    }

    sensorParametersSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply,
                                               sensorParametersWidget);
    sensorParametersSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    sensorParametersBackBtn = new myPushButton(myLan.back, myIcon.Action_Back,
                                               sensorParametersWidget);
    sensorParametersBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    refreshSensorDisplay();

    upLay->addWidget(sensorTypeGroup, 0, 0);
    upLay->addWidget(setHorizentalFreGroup, 0, 1);
    upLay->addWidget(sensorPixelGroup, 1, 0);
    upLay->addWidget(sensorModeGroup, 1, 1);

    downLay->addWidget(sensorParametersSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(sensorParametersBackBtn, Qt::AlignRight);

    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(sensorPlusBtn, SIGNAL(pressed()),this,SLOT(onSensorPlusBtnClicked()));
    connect(sensorMinusBtn, SIGNAL(pressed()),this,SLOT(onSensorMinusBtnClicked()));
    connect(sensorParametersSureBtn, SIGNAL(pressed()),this,SLOT(onSensorParametersSureBtnClicked()));
    connect(sensorParametersBackBtn, SIGNAL(pressed()),this,SLOT(onSensorParametersBackBtnClicked()));
    connect(sensorTypeGroup, SIGNAL(indexChanged(int)), this, SLOT(onSensorTypeChangedSlt(int)));
    connect(sensorModeGroup, SIGNAL(indexChanged(int)), this, SLOT(onSensorModeChangedSlt(int)));
    stackedWidget->addWidget(sensorParametersWidget);

    onSensorTypeChangedSlt(struCnfc.nSensorType - 1);
}

/*　创建镜头参数界面　*/
void setColorCamera::createLensParametersPage()
{
    lensParametersWidget = new QWidget;
    lensParametersWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(lensParametersWidget);

    lensSelectBtn = new myPushButton("", QIcon());
    lensSelectBtn->setFixedSize(QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));

    lensLabel = new QLabel("Phenix");
    lensLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    lensLabel->setFixedHeight(SMALL_BTN_WIDTH);

    lensSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    lensBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    lensSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    lensBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->addStretch();
    upLay->addWidget(lensSelectBtn);
    upLay->addSpacing(15);
    upLay->addWidget(lensLabel);
    upLay->addStretch();

    downLay->addWidget(lensSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(lensBackBtn, Qt::AlignRight);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(lensSelectBtn, SIGNAL(pressed()), this, SLOT(onLensBtnClicked()));
    connect(lensSureBtn, SIGNAL(pressed()), this, SLOT(onLensSureBtnClicked()));
    connect(lensBackBtn, SIGNAL(pressed()), this, SLOT(onLensBackBtnClicked()));

    stackedWidget->addWidget(lensParametersWidget);
}

/*　创建对位测试界面　*/
void setColorCamera::createContrapositionPage()
{
    rowNum_front = 0;
    rowNum_rear = 0;
    bias_front = 0;
    bias_rear = 0;
    bias_correct_front = 0;
    bias_correct_rear = 0;

    contrapositionWidget = new QWidget();
    contrapositionWidget->setGeometry(0, 0, width(), height());

    for(int i = 0 ; i < 5; i++) {
        m_lable[i] = new myLabel;
        m_lable[i]->setFixedSize(BTN_WIDTH-60,BTN_HEIGHT);
    }
    m_lable[0]->setText(myLan.row);
    m_lable[1]->setText(myLan.bias+"(ms)");
    m_lable[2]->setText(myLan.correct+"(ms)");
    m_lable[3]->setText(myLan.front_view);
    m_lable[4]->setText(myLan.rear_view);

    m_sigMapper = new QSignalMapper(contrapositionWidget);
    for(int i = 0 ; i < LTE_NUM; i++) {
        m_LineEdit[i] = new myLineEdit;
        m_LineEdit[i]->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
        m_sigMapper->setMapping(m_LineEdit[i], i);
        connect(m_LineEdit[i], SIGNAL(pressed()), m_sigMapper, SLOT(map()));
    }
    connect(m_sigMapper, SIGNAL(mapped(int)), this, SLOT(setCorrectBias(int)));

    m_chanNum = new QLCDNumber;
    m_chanNum->display(struGsh.nUnit/2+1);
    m_chanNum->setDigitCount(2);
    m_chanNum->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_plusChanNumBtn = new myPushButton("", myIcon.Action_Plus , contrapositionWidget);
    m_minusChanNUmBtn = new myPushButton("", myIcon.Action_Minus, contrapositionWidget);

    m_plusChanNumBtn->setFixedSize(SMALL_BTN_WIDTH,BTN_HEIGHT);
    m_minusChanNUmBtn->setFixedSize(SMALL_BTN_WIDTH,BTN_HEIGHT);

    m_startBtn = new myPushButton(myLan.start,myIcon.Media_Start);
    m_startBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    m_acceptBtn = new myPushButton("获取",myIcon.Edit_Search);
    m_acceptBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    m_biasSendBtn = new myPushButton(myLan.send_bias,myIcon.Sorter_Ejector);
    m_biasSendBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);

    m_contrapositionBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_contrapositionBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QVBoxLayout *mainLay = new QVBoxLayout(contrapositionWidget);
    myGroupBox *upGroup = new myGroupBox;
    myGroupBox *downGroup = new myGroupBox(myLan.chute);
    downGroup->setFixedHeight(100);
    QGridLayout *upGridLay = new QGridLayout(upGroup);
    QHBoxLayout *chuteHBoxLay = new QHBoxLayout(downGroup);
    QHBoxLayout *backBtnLay = new QHBoxLayout;

    upGridLay->setVerticalSpacing(50);
    upGridLay->addWidget(m_lable[3],0,1,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_lable[4],0,2,1,1,Qt::AlignCenter);

    upGridLay->addWidget(m_lable[0],1,0,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_lable[1],2,0,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_lable[2],3,0,1,1,Qt::AlignCenter);

    upGridLay->addWidget(m_LineEdit[0],1,1,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_LineEdit[1],2,1,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_LineEdit[2],3,1,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_LineEdit[3],1,2,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_LineEdit[4],2,2,1,1,Qt::AlignCenter);
    upGridLay->addWidget(m_LineEdit[5],3,2,1,1,Qt::AlignCenter);

    chuteHBoxLay->addWidget(m_minusChanNUmBtn);
    chuteHBoxLay->addWidget(m_chanNum);
    chuteHBoxLay->addWidget(m_plusChanNumBtn);

    chuteHBoxLay->addWidget(m_startBtn);
//    chuteHBoxLay->addWidget(m_acceptBtn);
    chuteHBoxLay->addWidget(m_biasSendBtn);

    backBtnLay->addWidget(m_contrapositionBackBtn,0,Qt::AlignBottom|Qt::AlignRight);

    mainLay->addSpacing(50);
    mainLay->addWidget(upGroup);
    mainLay->addStretch();
    mainLay->addWidget(downGroup);
    mainLay->addStretch();
    mainLay->addLayout(backBtnLay);

    connect(m_plusChanNumBtn , SIGNAL(pressed()), this, SLOT(onPlusChanNumPressedSlt()));
    connect(m_minusChanNUmBtn, SIGNAL(pressed()), this, SLOT(onMinusChanNumPressedSlt()));
    connect(m_startBtn, SIGNAL(pressed()), this, SLOT(onStartBtnPressedSlt()));
    connect(m_acceptBtn, SIGNAL(pressed()), this, SLOT(onAcceptbtnPressedSlt()));
    connect(m_biasSendBtn, SIGNAL(pressed()), this, SLOT(onBiasSendBtnPressedSlt()));
    connect(m_contrapositionBackBtn,SIGNAL(pressed()),this,SLOT(onContrapositionBackBtnClicked()));

    stackedWidget->addWidget(contrapositionWidget);
}

/* 设置模拟增益的值 */
void setColorCamera::setAnaGainParams()
{
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        if (struCnfc.nSensorType == SENSOR_D2) {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] = anaGainLcdNumD2->value();
        } else {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] = anaGainLcdNum->value();
        }
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] / 256,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit] % 256, 3);
        break;
    case 1:
        if (struCnfc.nSensorType == SENSOR_D2) {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] = anaGainLcdNumD2->value();
        } else {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] = anaGainLcdNum->value();
        }
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] / 256,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit] % 256, 3);

       break;
    case 2:
        if (struCnfc.nSensorType == SENSOR_D2) {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] = anaGainLcdNumD2->value();
        } else {
            struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] = anaGainLcdNum->value();
        }
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] / 256,
                           struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit] % 256, 3);

        break;
    default:
        break;
    }
}

/* 设置数字增益的值 */
void setColorCamera::setDigGainUpperParams()
{
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit] = digGainUpperLcdNum->value();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit], 3);
        break;
    case 1:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit] = digGainUpperLcdNum->value();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit], 3);
        break;
    case 2:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit] = digGainUpperLcdNum->value();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit], 3);
        break;
    default:
        break;
    }
}

/* 设置数字增益细调的值 */
void setColorCamera::setDigGainLowParams()
{
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit] = digGainValueBar->getValue();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                       struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit], 3);
        break;
    case 1:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit] = digGainValueBar->getValue();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                       struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit], 3);
        break;
    case 2:
        struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit] = digGainValueBar->getValue();
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                       struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit], 3);
        break;
    default:
        break;
    }
}

/* 计算象元划分的参数，划整个通道*/
void setColorCamera::calDivChannelParamsForWhole(int beginCh, int endCh)
{
    double ave;

    int nChannelEjectorsRangeLeft = 0;  //待划分象元的通道喷嘴的起始下标，左限（本组包含）
    int nChannelEjectorsRangeRight = 0; //待划分象元的通道喷嘴的结束下标，右限（本组不包含）

    nChannelEjectorsRangeLeft  = 0;
    nChannelEjectorsRangeRight = struCnfg.nEjectorsPerChute;

    struCnfc.struLevelCamera[struGsh.nLevel].nMaizeChannelBegin[struGsh.nUnit][0] = beginCh;
    struCnfc.struLevelCamera[struGsh.nLevel].nMaizeChannelEnd[struGsh.nUnit][1] = endCh;
    ave = (endCh-beginCh) / ((double)struCnfg.nEjectorsPerChute);//获得整个通道的喷嘴象元

    for(int i=nChannelEjectorsRangeLeft; i<nChannelEjectorsRangeRight; i++){
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i] =
                beginCh+(i-nChannelEjectorsRangeLeft)*ave+struGsh.fPixelCompensate[i]+0.5;
        if(i > nChannelEjectorsRangeLeft){
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i-1] =
                    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]-1;
        }
    }
    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][nChannelEjectorsRangeRight-1] = endCh;
    for(int i=nChannelEjectorsRangeLeft; i<nChannelEjectorsRangeRight; i++){
        MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, struGsh.nLevel, struGsh.nUnit, i,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]/256,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]%256,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i]/256,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i]%256, 3);
    }
    struCnfc.struLevelCamera[struGsh.nLevel].nMaizeChannelBegin[struGsh.nUnit][1] =
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][struCnfg.nEjectorsPerChute/2];
    struCnfc.struLevelCamera[struGsh.nLevel].nMaizeChannelEnd[struGsh.nUnit][0] =
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][struCnfg.nEjectorsPerChute/2-1];
}

/*判断当前划分象元是相机左侧或右侧*/
int setColorCamera::getCameraBodyID(int x, int y)
{
    int tmp = struCnfc.nSensorPixelLevel[struGsh.nLevel] / 2;
    int ncameraBodyID = 0;
	if ((x + y ) / 2 > tmp) {
		ncameraBodyID = 1;
	} else {
		ncameraBodyID = 0;
	}
    return ncameraBodyID;
}

/* 计算象元划分的参数，划半个通道*/
void setColorCamera::calDivChannelParamsForHalf(int beginCh, int endCh)
{
    double ave;

    int nChannelEjectorsRangeLeft = 0;  //待划分象元的通道喷嘴的起始下标，左限（本组包含）
    int nChannelEjectorsRangeRight = 0; //待划分象元的通道喷嘴的结束下标，右限（本组不包含）
    int ncameraBodyID = 0;
    ncameraBodyID = getCameraBodyID(beginCh, endCh);//自动判断左右侧

    struCnfc.struLevelCamera[struGsh.nLevel].nMaizeChannelBegin[struGsh.nUnit][ncameraBodyID] = beginCh;
    struCnfc.struLevelCamera[struGsh.nLevel].nMaizeChannelEnd[struGsh.nUnit][ncameraBodyID] = endCh;

    ave = (endCh-beginCh) / ((double)struCnfg.nEjectorsPerChute/2);//获得半个通道的喷嘴象元
    if (ncameraBodyID == 0) {                 //左侧
        nChannelEjectorsRangeLeft  = 0;
        nChannelEjectorsRangeRight = struCnfg.nEjectorsPerChute/2;
    } else {                                    //右侧
        nChannelEjectorsRangeLeft  = struCnfg.nEjectorsPerChute/2;
        nChannelEjectorsRangeRight = struCnfg.nEjectorsPerChute;
    }

    for(int i=nChannelEjectorsRangeLeft; i<nChannelEjectorsRangeRight; i++){
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i] =
                beginCh+(i-nChannelEjectorsRangeLeft)*ave+struGsh.fPixelCompensate[i]+0.5;
        if(i > nChannelEjectorsRangeLeft){
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i-1] =
                    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]-1;
        }
    }
    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][nChannelEjectorsRangeRight-1] = endCh;
    for(int i=nChannelEjectorsRangeLeft; i<nChannelEjectorsRangeRight; i++){
        MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, struGsh.nLevel, struGsh.nUnit, i,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]/256,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]%256,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i]/256,
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i]%256, 3);
    }

}

/* 计算象元划分的参数 */
void setColorCamera::calDivChannelParamsMultiView(int beginCh, int endCh)
{

}

/* 计算象元划分的参数 */
void setColorCamera::calDivChannelParams(int beginCh, int endCh)
{
    double ave;
    int min, max, range;


    struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit] = beginCh;
    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit] = endCh;

    min = struCnfc.struLevelCamera[struGsh.nLevel].nEjectorBegin[struGsh.nUnit];
    max =   struCnfc.struLevelCamera[struGsh.nLevel].nEjectorEnd[struGsh.nUnit];
    range = qAbs(max-min)+1;
    ave = (endCh-beginCh)/(double)range;


    // 重置喷嘴像元参数
    for (int i = 0; i < struCnfg.nEjectorsPerChute; i++) {
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i] = 0;
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i] = 0;
    }
    // 动态调整喷嘴像元
    for (int i = 0; i < range; i++) {
        int id = i+min;
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][id] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]+i*ave+struGsh.fPixelCompensate[id]+0.5;
        if (i > 0) {
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][id-1] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][id]-1;
        }
    }
    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][max] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit];

    /* 通道划分 */
    MySerial.com1Write(CMD_UNIT_CHANNEL, UNIT, struGsh.nLevel, struGsh.nUnit, 0,
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]/256,
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]%256,
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]/256,
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]%256, 3);
    /* 喷嘴象元 */
	for (int i = 0; i < struCnfg.nEjectorsPerChute; i++) {
		MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, struGsh.nLevel, struGsh.nUnit, i,
			struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]/256,
			struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i]%256,
			struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i]/256,
			struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i]%256, 3);
	}

    if(struCnfg.aiEnable == 1){
        QByteArray args;
        AI_Data_Protocol_D data;
        int ret;
        args[0] = AIUNIT;
        //ai相机编号，取余
        args[1] = struGsh.nUnit%2;
        args[2] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]/256;
        args[3] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]%256;
        args[4] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]/256;
        args[5] = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]%256;
        MyUpd.writeDatagram(CMD_AI_PIXEL_SEND, struGsh.nUnit/2, 6, args, struGsh.addressList.at(struGsh.nUnit/2), AI_UDP_SEND_PORT);
        data.nCommandAddress = CMD_AI_PIXEL_SEND;
        ret = MyUpd.readUdpDatagrams(&data,13);
        if(ret !=0){
            qDebug("aiDevice: %d, ret: %d",  struGsh.nUnit/2, ret);
        }
    }

    refreshDivChannelList2();
}

void setColorCamera::calDivChannelParams_AFATOXIN(int target)
{
    int flag1 = 0;
    int row[1024];
    int num = 0;
    int channel[63];
    int channel_tmp[63];

    // 0 ~ 60
    for (int i = 0; i < 1023; i++) {
        row[i] = struGsh.sRowGreen[i];
    }

    for (int i = 0; i < 63;i++) {
        channel[i] = 0;
        channel_tmp[i] = 0;
    }

    num = 0;
    for (int i = 0; i < 1023; i++) {
        if (flag1 == 0) {
            if (row[i] > target) {
                flag1 = 1;
                channel[num] = i;
                num++;
            }
        }
        else {
            if (row[i] < target) {
                flag1 = 0;
                channel[num] = i;
                num++;
            }
        }
        if (num >= struCnfg.nEjectorsPerChute/2+3)
            break;
    }
    for (int i = 0; i <= struCnfg.nEjectorsPerChute/2; i++) {
        channel_tmp[i]     = (channel[i+1]+channel[i+2])/2;
    }
    for (int i = 0; i <= struCnfg.nEjectorsPerChute/2; i++) {
        if ( i == struCnfg.nEjectorsPerChute/2){
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i-1] = channel_tmp[i];
            continue;
        }
        if (i == 0) {
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i] = channel_tmp[i];
            continue;
        }
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][i] = channel_tmp[i];
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][i-1] = channel_tmp[i];
    }

    // 	60 ~120
    for (int i = 0; i <= struCnfg.nEjectorsPerChute/2+2;i++) {
        channel[i] = 0;
        channel_tmp[i] = 0;
    }

    num = 0;
    flag1 = 0;
    for (int i = 1023; i > 0 ; i--) {
        if (flag1 == 0) {
            if (row[i] > target) {
                flag1 = 1;
                channel[num] = i;
                num++;
            }
        }
        else {
            if (row[i] <  target) {
                flag1 = 0;
                channel[num] = i;
                num++;
            }
        }
        if (num >= struCnfg.nEjectorsPerChute/2+3)
            break;
    }
    for (int i = 0; i <= struCnfg.nEjectorsPerChute/2; i++) {
        channel_tmp[i] = (channel[i+1]+channel[i+2])/2;
    }
    for (int i = 0; i <= struCnfg.nEjectorsPerChute/2; i++) {
        if ( i == struCnfg.nEjectorsPerChute/2){
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][struCnfg.nEjectorsPerChute-i] =   channel_tmp[struCnfg.nEjectorsPerChute-i];
            continue;
        }
        if (i == 0) {
            struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][struCnfg.nEjectorsPerChute-1-i] = channel_tmp[i];
            continue;
        }
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][struCnfg.nEjectorsPerChute-1-i] = channel_tmp[i];
        struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][struCnfg.nEjectorsPerChute-i] =   channel_tmp[i];
    }

    int nLast = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][struCnfg.nEjectorsPerChute/2-1];
    int nNext = struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][struCnfg.nEjectorsPerChute/2];
    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorEnd[struGsh.nUnit][struCnfg.nEjectorsPerChute/2-1] = (nLast + nNext)/2;
    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEjectorBegin[struGsh.nUnit][struCnfg.nEjectorsPerChute/2] = nLast;
}

/* 设置传感器参数界面的值 */
void setColorCamera::setSensorParams()
{
    m_nSensorType = struCnfc.nSensorType;
    m_nSensorMode = struCnfp.nSensorMode;
    m_fRowFre = struCnfc.nSensorRowFre/10.0;
}

/* 刷新信号校正界面的显示内容 */
void setColorCamera::refreshCalDisplay()
{
    calibrationAxis->refreshCurveDisplay();
    switch(calibrationTabBar->currentIndex()){
    case 0:
        calibrationLcdNum->display(struCnfc.nRefRed);
        break;
    case 1:
        calibrationLcdNum->display(struCnfc.nRefGreen);
        break;
    case 2:
        calibrationLcdNum->display(struCnfc.nRefBlue);
        break;
    default:
        break;
    }


    switch (struCnfe.nMachine) {
    default:
        calibrationWholeBtn->setText(myLan.update_auto);
        break;
    }
    //! 模拟增益
    switch(calibrationTabBar->currentIndex()){
    case 0:
        anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        break;
    case 1:
        anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        break;
    case 2:
        anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        break;
    }
    //! 数字增益
    switch(calibrationTabBar->currentIndex()){
    case 0:
        digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
        digGainValueBar2->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit]);
        break;
    case 1:
        digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit]);
        digGainValueBar2->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit]);
        break;
    case 2:
        digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit]);
        digGainValueBar2->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 刷新设置偏置界面的显示内容 */
void setColorCamera::refreshBiasingDisplay()
{
    setBiasingAxis->refreshCurveDisplay();
    switch(biasingTabBar->currentIndex()){
    case 0:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingRed[struGsh.nUnit]);
        break;
    case 1:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingGreen[struGsh.nUnit]);
        break;
    case 2:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 刷新增益界面的显示内容 */
void setColorCamera::refreshGainDisplay()
{
    gainAdjustAxis->refreshCurveDisplay();
    //! 模拟增益
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        break;
    case 1:
        anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        break;
    case 2:
        anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        break;
    }
    //! 数字增益
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit]);
        break;
    case 1:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit]);
        break;
    case 2:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

void setColorCamera::refreshDivChannelList2(){
    listLabel2[1][0]->setText(myLan.front_begin);
    listLabel2[2][0]->setText(myLan.front_end);
    listLabel2[3][0]->setText(myLan.rear_begin);
    listLabel2[4][0]->setText(myLan.rear_end);
    qDebug()<<"struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal:"<<struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;

    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {
        if (i%2 == 0) {
            listLabel2[1][i/2+1]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelBegin[i]));
            listLabel2[2][i/2+1]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelEnd[i]));
        } else {
            listLabel2[3][i/2+1]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelBegin[i]));
            listLabel2[4][i/2+1]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelEnd[i]));
        }
    }

    for(int i=struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2+1; i<13; i++){
        for(int j=0; j<5; j++){
            listLabel2[j][i]->hide();
        }
    }
}

/* 刷新划分象元界面的显示内容 */
void setColorCamera::refreshDivDisplay()
{
    divChannelAxis->refreshCurveDisplay();
    beginChannelLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]);
    endChannelLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]);
    beginEjectorLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nEjectorBegin[struGsh.nUnit]);
    endEjectorLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nEjectorEnd[struGsh.nUnit]);

}

/* 刷新传感器参数配置界面的显示内容 */
void setColorCamera::refreshSensorDisplay()
{
    for (int i = 0; i < MAX_LEVEL; i++) {
        sensorPixelLabel[i]->hide();
        sensorPixelCbx[i]->hide();
    }

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        if (struCnfc.nSensorPixelLevel[i] == SENSOR_PIXELS_2048
                || struCnfc.nSensorPixelLevel[i] == SENSOR_PIXELS_2592) {
            sensorPixelCbx[i]->setChecked(true);
        } else {
            sensorPixelCbx[i]->setChecked(false);
        }
        sensorPixelLabel[i]->show();
        sensorPixelCbx[i]->show();
    }
    switch (struCnfe.nMachine) {
    default:
        sensorPixelLabel[0]->setText(myLan.upper_layer);
        break;
    }
}

/*!
 * \brief setColorCamera::signalIsOk
 * \return
 */
int setColorCamera::signalIsOk(bool bIsFront, int colorChute)
{
    int nSum = 0;
    int nAvr = 0;
    uchar *p = (uchar *)(struGsh.sRgbRow+10);
    int nBegin = 400;
    int nEnd = struCnfc.nSensorPixel-400;
    int nTotal = nEnd-nBegin;
    int nStandardVal;
    uchar data;

    for (int i = nBegin; i < nEnd; i++) {
        switch(colorChute)
        {
        case 1: //! R
            data = *(p+i*3);
            nSum += data;
            if (struGsh.nUnit%2) {
                nStandardVal = struCnfc.nRearRefRed;
            } else {
                nStandardVal = struCnfc.nRefRed;
            }
            break;
        case 2: //! G
            data = *(p+i*3+1);
            nSum += data;
            if (struGsh.nUnit%2) {
                nStandardVal = struCnfc.nRearRefGreen;
            } else {
                nStandardVal = struCnfc.nRefGreen;
            }
            break;
        case 4: //! B
            data = *(p+i*3+2);
            nSum += data;
            if (struGsh.nUnit%2) {
                nStandardVal = struCnfc.nRearRefBlue;
            } else {
                nStandardVal = struCnfc.nRefBlue;
            }
            break;
        default:
            break;
        }
    }
    nAvr = nSum/nTotal;
    
    qDebug("avr %d",nAvr);
    qDebug("s %d",nStandardVal);
    if (nStandardVal < 2)
        return 0;

    return nStandardVal-nAvr;
}

/*!
 * \brief setColorCamera::adjustDigiGain
 */
void setColorCamera::adjustDigiGain(int colorChute)
{
    bool bIsFront;
    int ret;
    int nDigiGain;

    //! 前后视标志
    if (struGsh.nUnit%2) {
        bIsFront = true;
    } else {
        bIsFront = false;
    }

    //! 数字增益调整
    for (int n = 0; n < 15; n++) {
        ret = signalIsOk(bIsFront,colorChute);
        if (abs(ret) <= 2) {
            break;
        }
        switch(colorChute)
        {
        case 1://! R
            nDigiGain =  struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]*64+
                         struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit];
            break;
        case 2://! G
            nDigiGain =  struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit]*64+
                         struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit];
            break;
        case 4://! B
            nDigiGain =  struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit]*64+
                         struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit];
            break;
        default:
            break;
        }

        qDebug("digi %d",nDigiGain);
        qDebug("ret %d",ret);

        nDigiGain += ret;
        qDebug("digi %d",nDigiGain);

		//! 检查数字增益为非负数
		if (nDigiGain < 0) {
			nDigiGain = 0;
		}

        switch(colorChute)
        {
        case 1:
            struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit] = nDigiGain/64 ;
            struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit] = nDigiGain%64;

            MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                               struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit], 3);
            MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit], 3);
            break;
        case 2:
            struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit] = nDigiGain/64;
            struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit] = nDigiGain%64;

            MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                               struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit], 3);
            MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit], 3);
            break;
        case 4:
            struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit] = nDigiGain/64;
            struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit] = nDigiGain%64;

            MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                               struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit], 3);
            MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit], 3);
            break;
        default:
            break;
        }

        myFlow.sleep(1);
   }
}

/*!
 * \brief RSC机型下重置校正系数显示
 */
void setColorCamera::resetCalDisplayRSC()
{
    if (struGsh.nUnit%2) {
        switch(calibrationTabBar->currentIndex()) {
        case 0:
            calibrationLcdNum->display(struCnfc.nRearRefRed);
            break;
        case 1:
            calibrationLcdNum->display(struCnfc.nRearRefGreen);
            break;
        case 2:
            calibrationLcdNum->display(struCnfc.nRearRefBlue);
            break;
        default:
            break;
        }
    }
}

/*!
 * \brief RSC机型下重置校正系数参数
 */
void setColorCamera::resetCalValueRSC(int nVal)
{
    switch(calibrationTabBar->currentIndex()){
    case 0:
        if (struGsh.nUnit%2) {
            struCnfc.nRearRefRed = nVal;
        } else {
            struCnfc.nRefRed = nVal;
        }
        break;
    case 1:
        if (struGsh.nUnit%2) {
            struCnfc.nRearRefGreen = nVal;
        } else {
            struCnfc.nRefGreen = nVal;
        }
        break;
    case 2:
        if (struGsh.nUnit%2) {
            struCnfc.nRearRefBlue = nVal;
        } else {
            struCnfc.nRefBlue = nVal;
        }
        break;
    default:
        break;
    }
}

/* 新R自动光源标定前配置好灯光*/
void setColorCamera::adjustLampLight()
{
    /** 新R机型灯光配置
     *  物料灯序号: 1 2 4 5
     *  背景灯序号: 3
     *  前视: 全白灯,物料灯光强为120*（通道数+1）,背景灯光强为400
     *  后视: 物料灯1和2为蓝灯,校正时关闭 物料灯4和5为白灯,光强为120*（通道数+1）  背景灯蓝灯,光强为400 */
    int nChuteNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
    int nLight = 120*(nChuteNum+1);
    //前视物料灯
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_1] = 1;
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_2] = 1;
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_4] = 1;
    struCnfp.struGroupCtrl[0].nLamp[FRONT_LAMP_5] = 1;
    struCnfp.struGroupCtrl[0].nLampLight[FRONT_LAMP_1] = nLight;
    struCnfp.struGroupCtrl[0].nLampLight[FRONT_LAMP_2] = nLight;
    struCnfp.struGroupCtrl[0].nLampLight[FRONT_LAMP_4] = nLight;
    struCnfp.struGroupCtrl[0].nLampLight[FRONT_LAMP_5] = nLight;
    //后视物料灯
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_1] = 1;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_2] = 1;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_4] = 1;
    struCnfp.struGroupCtrl[0].nLamp[REAR_LAMP_5] = 1;
    struCnfp.struGroupCtrl[0].nLampLight[REAR_LAMP_1] = 20*(nChuteNum+1);
    struCnfp.struGroupCtrl[0].nLampLight[REAR_LAMP_2] = 20*(nChuteNum+1);
    struCnfp.struGroupCtrl[0].nLampLight[REAR_LAMP_4] = nLight;
    struCnfp.struGroupCtrl[0].nLampLight[REAR_LAMP_5] = nLight;

        /** 背景灯 */
    //struCnfp.struGroupCtrl[0].nLampLight[FRONT_LAMP_3_BACKGROUD] = 400;
    //struCnfp.struGroupCtrl[0].nLampLight[REAR_LAMP_3_BACKGROUD] = 400;

    myMonoFlow.sendLampParamsMono();
}

/* 新R自动光源标定前固定好增益*/
void setColorCamera::adjustGain(int nLevel, int nUnitAddr)
{
    /* 模拟增益*/
    struCnfc.struLevelCamera[nLevel].nAnaGainRed[nUnitAddr] = 1;
    struCnfc.struLevelCamera[nLevel].nAnaGainGreen[nUnitAddr] = 1;
    struCnfc.struLevelCamera[nLevel].nAnaGainBlue[nUnitAddr] = 1;

    MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, nLevel, nUnitAddr, 1, 0, 0,
                       struCnfc.struLevelCamera[nLevel].nAnaGainRed[nUnitAddr]/256,
                       struCnfc.struLevelCamera[nLevel].nAnaGainRed[nUnitAddr]%256, 3);
    MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, nLevel, nUnitAddr, 2, 0, 0,
                       struCnfc.struLevelCamera[nLevel].nAnaGainGreen[nUnitAddr]/256,
                       struCnfc.struLevelCamera[nLevel].nAnaGainGreen[nUnitAddr]%256, 3);
    if (struCnfc.nSensorType != SENSOR_D2) {
        MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, nLevel, nUnitAddr, 4, 0, 0,
                           struCnfc.struLevelCamera[nLevel].nAnaGainBlue[nUnitAddr]/256,
                           struCnfc.struLevelCamera[nLevel].nAnaGainBlue[nUnitAddr]%256, 3);
    }
    /* 数字增益*/
    struCnfc.struLevelCamera[nLevel].nDigiGainUpperRed[nUnitAddr] = 4;
    struCnfc.struLevelCamera[nLevel].nDigiGainLowRed[nUnitAddr] = 0;
    struCnfc.struLevelCamera[nLevel].nDigiGainUpperGreen[nUnitAddr] = 3;
    struCnfc.struLevelCamera[nLevel].nDigiGainLowGreen[nUnitAddr] = 0;
    struCnfc.struLevelCamera[nLevel].nDigiGainUpperBlue[nUnitAddr] = 4;
    struCnfc.struLevelCamera[nLevel].nDigiGainLowBlue[nUnitAddr] = 0;
    if (struCnfc.nSensorType != SENSOR_D2) {//D2无数字增益
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, nLevel, nUnitAddr, 1, 0, 0, 0, struCnfc.struLevelCamera[nLevel].nDigiGainUpperRed[nUnitAddr], 3);
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH,  UNIT, nLevel, nUnitAddr, 1, 0, 0, 0, struCnfc.struLevelCamera[nLevel].nDigiGainLowRed[nUnitAddr], 3);
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, nLevel, nUnitAddr, 2, 0, 0, 0, struCnfc.struLevelCamera[nLevel].nDigiGainUpperGreen[nUnitAddr], 3);
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH,  UNIT, nLevel, nUnitAddr, 2, 0, 0, 0, struCnfc.struLevelCamera[nLevel].nDigiGainLowGreen[nUnitAddr], 3);
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_QUICK, UNIT, nLevel, nUnitAddr, 4, 0, 0, 0, struCnfc.struLevelCamera[nLevel].nDigiGainUpperBlue[nUnitAddr], 3);
        MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH,  UNIT, nLevel, nUnitAddr, 4, 0, 0, 0, struCnfc.struLevelCamera[nLevel].nDigiGainLowBlue[nUnitAddr], 3);
    }
}

// 新R获取校正波形信号
void setColorCamera::getAdjustSignal()
{
    int nRet;
    int nPacketSize = struCnfc.nSensorPixel*3+16;

    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, 0, struGsh.nUnit, 0, 0x02, 0, 0, 0, 3);
    myFlow.msleep(600);
    nRet = MySerial.com1Read(struGsh.sRgbRow, nPacketSize);
    if(nRet != nPacketSize
            || struGsh.sRgbRow[0] != 0xAA
            || struGsh.sRgbRow[1] != 0xAA
            || struGsh.sRgbRow[2] != 0xAA
            || struGsh.sRgbRow[3] != 0xAA) {
        for (int i = 0; i < nPacketSize; i++) {
            struGsh.sRgbRow[i] = 0;
        }
    }

    calibrateWaveAxis->update();
}

/* 响应彩色相机设置界面中的返回按钮 */
void setColorCamera::onMainBackBtnClicked()
{
    myFlow.updateOnoff(1);  //退出相机设置界面后发送校准开始命令
    emit backToMainWindow();
}

/* 响应当前页索引发生变化的情况 */
void setColorCamera::onStackWidgetIndexChangedSlt(int index)
{
    QString title;
    switch(index){
    case SET_COLOR_CAMERA_MAIN_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.color_camera);
        break;
    case CAL_SIGNAL_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.color_calibrate);
        break;
    case CAL_WAVE_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.adjust_wave);
        break;
    case SET_BIASING_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.color_biasing);
        break;
    case GAIN_ADJUST_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.color_gain_adjust);
        break;
    case DIV_CHANNEL_PAGE:
    case CHANNEL_LIST_PAGE:
    case CHANNEL_RANGE_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.channel_divide);
        break;
    case SENSOR_PARAMS_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.sensor);
        break;
    case LENS_PARAMS_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.color_lens_params);
        break;
    case CONTRA_POSITION_PAGE:
        title = QString("%1 > %2 > %3").arg(myLan.factory_set).arg(myLan.color_camera).arg(myLan.contra_test);
        break;
    default:
        break;
    }
    titleLabel->setText(title);
}

/* 响应彩色相机设置界面中的校准彩色信号按钮 */
void setColorCamera::onCalibrationColorSignalClicked()
{
    stackedWidget->setCurrentIndex(CAL_SIGNAL_PAGE);
    refreshCalDisplay();
    emit sCalibrationCurveStart();
}

/* 响应校准彩色信号界面中的返回按钮 */
void setColorCamera::onCalibrationBackBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
    emit sCalibrationCurveStop();
}

/* 响应校准彩色信号界面中的减小按钮 */
void setColorCamera::onCalibrationMinusBtnClicked()
{
    int tmp = calibrationLcdNum->value();
    if(tmp > 0){
        calibrationLcdNum->display(tmp-1);
		setCalValue(tmp-1);
    }
}

/* 响应校准彩色信号界面中的增加按钮 */
void setColorCamera::onCalibrationPlusBtnClicked()
{
    int tmp = calibrationLcdNum->value();
    if (tmp < 255) {
        calibrationLcdNum->display(tmp+1);
		setCalValue(tmp+1);
    }
}

/* 响应校准彩色信号界面中的校准获取按钮 */
void setColorCamera::onCalObtainBtnClicked()
{
	if (myFlow.getProductLineNo() == 0) {
    	myMessageBox msgBox(MSG_QUES, myLan.update_capture + "?");
		if (msgBox.exec() != QDialog::Accepted) {
			return;
		}
	}

    // 向下位机发送校正系数获取命令
    int nRed = struCnfc.nRefRed;
    int nGreen = struCnfc.nRefGreen;
    int nBlue = struCnfc.nRefBlue;
    //校正获取
    MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, struGsh.nUnit,
                       struCnfg.nProfile+1, 1, nRed, nGreen, nBlue, 3);

    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->showSecs();

    myFlow.sleep(1);
    //校正保存
    MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, struGsh.nUnit,
                       struCnfg.nProfile+1, 3, nRed, nGreen, nBlue, 3);
}

/* 响应校准彩色信号界面中的整机校准按钮 */
void setColorCamera::onCalWholeBtnClicked()
{
    if (myFlow.getProductLineNo() == 0) {
    	myMessageBox msgBox(MSG_QUES, myLan.update_auto + "?");
		if (msgBox.exec() != QDialog::Accepted) {
			return;
		}
	}


    //! 校正系数获取
	int nRed, nGreen, nBlue;
    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {
        int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
    	nRed = struCnfc.nRefRed;
    	nGreen = struCnfc.nRefGreen;
    	nBlue = struCnfc.nRefBlue;
        MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr,
                           struCnfg.nProfile+1, 1, nRed, nGreen, nBlue, 3);
    }

    myFlow.sleep(1);

    //! 校正系数保存
    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {
        int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
        MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr,
                           struCnfg.nProfile+1, 3, nRed, nGreen, nBlue, 3);
    }
}

//RSC整机自动校正过程
void setColorCamera::calWholeAutoProcess()
{
    int nAverageR = 0, nAverageG = 0, nAverageB = 0;
    int nSumR = 0, nSumG = 0, nSumB = 0;
    int nActiveBoardCount=0;
    int nActiveBoardCountRear=0;
    int curUnit = struGsh.nUnit;

    infoWidget->setLabelText(myLan.auto_cal_lightset);
    infoWidget->delayShow();
    adjustLampLight();

    infoWidget->setLabelText(myLan.auto_cal_gainset);
    for(int i = 0; i < struCnfg.nInterfaceBoardTotal; i++){
        for(int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++){
           int nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
           adjustGain(i,nUnitAddr);
        }
    }
    myFlow.sleep(4);
    emit sCalibrationCurveStop();//停止获取当前通道信号
    myFlow.sleep(1);
    infoWidget->setLabelText(myLan.auto_cal_get_target);

    //统计前视信号平均值
    for(int i = 0; i < struCnfg.nInterfaceBoardTotal; i++){
        for(int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j+=2){
           int nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
           nAverageR = 0;
           nAverageG = 0;
           nAverageB = 0;
           if(getAdjustAvrForEachUnit(i,nUnitAddr,nAverageR,nAverageG,nAverageB)){
               nActiveBoardCount++;
               nSumR += nAverageR;
               nSumG += nAverageG;
               nSumB += nAverageB;
           }

        }
    }
    if(nActiveBoardCount==0){
        infoWidget->setLabelText(myLan.front_no_signal);
        infoWidget->showSecs();
    }else{
        struCnfc.nRefRed = nSumR/nActiveBoardCount;
        struCnfc.nRefGreen = nSumG/nActiveBoardCount;
        struCnfc.nRefBlue = nSumB/nActiveBoardCount;
    }

    //统计后视信号平均值
    nSumR = 0;
    nSumG = 0;
    nSumB = 0;
    for(int i = 0; i < struCnfg.nInterfaceBoardTotal; i++){
        for(int j = 1; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j+=2){
           int nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
           nAverageR = 0;
           nAverageG = 0;
           nAverageB = 0;
           if(getAdjustAvrForEachUnit(i,nUnitAddr,nAverageR,nAverageG,nAverageB)){
               nActiveBoardCountRear++;
               nSumR += nAverageR;
               nSumG += nAverageG;
               nSumB += nAverageB;
           }

        }
    }
    if(nActiveBoardCountRear==0){
        infoWidget->setLabelText(myLan.rear_no_signal);
        infoWidget->showSecs();
    }else{
        struCnfc.nRearRefRed = nSumR/nActiveBoardCountRear;
        struCnfc.nRearRefGreen = nSumG/nActiveBoardCountRear;
        struCnfc.nRearRefBlue = nSumB/nActiveBoardCountRear;
    }

    refreshCalDisplay();
    struGsh.nUnit = curUnit;
    calibrationAxis->refreshCurveDisplay();
    emit sCalibrationCurveStart();

    int nRed, nGreen, nBlue;
    //前视
    if(nActiveBoardCount!=0){
        //! 校正系数获取
        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i+=2) {
            int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
            nRed = struCnfc.nRefRed;
            nGreen = struCnfc.nRefGreen;
            nBlue = struCnfc.nRefBlue;
            MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr,
                               struCnfg.nProfile+1, 1, nRed, nGreen, nBlue, 3);
        }

        myFlow.sleep(1);

        //! 校正系数保存
        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i+=2) {
            int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
            MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr,
                               struCnfg.nProfile+1, 3, nRed, nGreen, nBlue, 3);
        }
    }

    //后视
    if(nActiveBoardCountRear!=0){
        for (int i = 1; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i+=2) {
            int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
            nRed = struCnfc.nRearRefRed;
            nGreen = struCnfc.nRearRefGreen;
            nBlue = struCnfc.nRearRefBlue;
            MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr,
                               struCnfg.nProfile+1, 1, nRed, nGreen, nBlue, 3);
        }

        myFlow.sleep(1);

        //! 校正系数保存
        for (int i = 1; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i+=2) {
            int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
            MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr,
                               struCnfg.nProfile+1, 3, nRed, nGreen, nBlue, 3);
        }
    }
    //struGsh.nUnit = curUnit;
    //calibrationAxis->refreshCurveDisplay();
    //emit sCalibrationCurveStart();
    myFlow.sleep(1);
    infoWidget->hide();
}



/************************************************
 *descriptipon:针对RSC,取单板信号平均值
 *called by: 1.setColorCamera::calWholeAutoProcess()
 *input:板所在层及层内编号
 *output:单板信号RGB平均值
 *return: true-当前板有信号    false-当前板无信号
 *Notes:计算平均时不考虑传感器首尾各400个像素
 ************************************************/
bool setColorCamera::getAdjustAvrForEachUnit(int level, int nUnitAddr, int& averageR, int& averageG, int& averageB)
{
    char *p = struGsh.sRgbRow;
    int nSumR = 0, nSumG = 0, nSumB = 0;
    int nBeginIndex = 400;
    int nEndIndex = struCnfc.nSensorPixel-400;
    int dataLength = struCnfc.nSensorPixel*3+16;
    int retLen = 0;
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, level, nUnitAddr,0,0,0,0,0,1);
    usleep(800000);
    memset(p, 0, sizeof(struGsh.sRgbRow));
    retLen = MySerial.com1Read(p,dataLength);
    if((retLen != dataLength)||(p[0]!=0xAA)||(p[1]!=0xAA)||(p[2]!=0xAA)||(p[3]!=0xAA)){
        return false;
    }
    for(int i=nBeginIndex; i<nEndIndex; i++){
        nSumR += p[10+i*3];
        nSumG += p[10+i*3+1];
        nSumB += p[10+i*3+2];
    }
    averageR = nSumR / (nEndIndex-nBeginIndex);
    averageG = nSumG / (nEndIndex-nBeginIndex);
    averageB = nSumB / (nEndIndex-nBeginIndex);
    if((averageR==0)&&(averageG==0)&&(averageB==0)){
        return false;
    }
    return true;
}

/*!
 * \brief 校正波形按钮状态
 */
void setColorCamera::onCalWaveBtnClicked()
{
    /* 1.停止采集波形线程 */
    calibrationAxis->curveStopSlt();
    myFlow.sleep(1);

    /* 2.发白板校正系数请求命令 */
    getAdjustSignal();

    calibrateWaveAxis->refreshCurveDisplay();
    calibrateWaveAxis->hidePauseBtn();
    profileValueBar->setMaxValue(99);
    stackedWidget->setCurrentIndex(CAL_WAVE_PAGE);
}

/* 响应校准彩色信号界面中的QTabBar索引发生变化的事件 */
void setColorCamera::onCalibrationTabBarChangedSlt(int index)
{
    switch(index){
    case 0:
        calibrationLcdNum->display(struCnfc.nRefRed);
        break;
    case 1:
        calibrationLcdNum->display(struCnfc.nRefGreen);
        break;
    case 2:
        calibrationLcdNum->display(struCnfc.nRefBlue);
        break;
    default:
        break;
    }


    //模拟增益界面刷新
    switch(index){
    case 0:
        if (struCnfc.nSensorType == SENSOR_D2) {
            anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        } else {
            anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        }
        break;
    case 1:
        if (struCnfc.nSensorType == SENSOR_D2) {
            anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        } else {
            anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        }
        break;
    case 2:
        if (struCnfc.nSensorType == SENSOR_D2) {
            anaGainLcdNumD22->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        } else {
            anaGainLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        }
        break;
    }
    //数字增益界面刷新
    switch(index){
    case 0:
        digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
        digGainValueBar2->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit]);
        break;
    case 1:
        digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit]);
        digGainValueBar2->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit]);
        break;
    case 2:
        digGainUpperLcdNum2->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit]);
        digGainValueBar2->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 实时刷新彩色相机校准中RGB的值 */
void setColorCamera::refreshCalValue()
{   
    int tmp = calibrationLcdNum->value();
    myInputPanel inputDlg(intType, 0, 255, tmp);
    int ret = inputDlg.exec();
    if(ret == QDialog::Accepted){
        tmp = inputDlg.getValue();
        calibrationLcdNum->display(tmp);
    }
	
	setCalValue(tmp);
}

/* 设置校正系数 */
void setColorCamera::setCalValue(int nValue)
{

	switch(calibrationTabBar->currentIndex()){
    case 0:
        struCnfc.nRefRed = nValue;
        break;
    case 1:
        struCnfc.nRefGreen = nValue;
        break;
    case 2:
        struCnfc.nRefBlue = nValue;
        break;
    default:
        break;
    }
}

/*!
 * \brief setColorCamera::onCalibrationViewChangedSlt
 */
void setColorCamera::onCalibrationViewChangedSlt()
{
    switch(calibrationTabBar->currentIndex()){
    case 0:
        calibrationLcdNum->display(struCnfc.nRefRed);
        break;
    case 1:
        calibrationLcdNum->display(struCnfc.nRefGreen);
        break;
    case 2:
        calibrationLcdNum->display(struCnfc.nRefBlue);
        break;
    default:
        break;
    }

}

/* 响应校正波形界面返回按钮 */
void setColorCamera::onCalibrateWaveBackBtnClicked()
{
    emit sCalibrationCurveStart();
    stackedWidget->setCurrentIndex(CAL_SIGNAL_PAGE);
}

/* 响应校正波形界面板号变化情况*/
void setColorCamera::onBoardNumChangedSlt()
{
    getAdjustSignal();
}

/* 响应校正波形界面自动复制按钮动作*/
void setColorCamera::onCalibrateCopyBtnPressed()
{
    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    //! 暂定校正系数复制方案数目为50
    int nProfileTotal = 50;
    nProfileTotal = (nProfileTotal > struCnfg.nProfileTotal) ? nProfileTotal : struCnfg.nProfileTotal;

    for (int i = 0; i < nProfileTotal; i++) {
        profileValueBar->setValue(i+1);
        for (int j = 0; j < struCnfg.nLevelTotal; j++) {
            for (int k = 0; k < struCnfg.struLevelInfo[j].nUnitLevelTotal; k++) {
                int nUnitId = struCnfg.struLevelInfo[j].nUnitId[k];
                MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, j, nUnitId, profileValueBar->getValue(), 3, 0, 0, 0, 3);
            }
        }

        myFlow.msleep(500);
    }

    infoWidget->delayHide();
}

/* 响应彩色相机设置界面中的设置偏置按钮 */
void setColorCamera::onSetBiasingBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_BIASING_PAGE);
    refreshBiasingDisplay();
    emit sBiasingCurveStart();
}

/* 响应设置偏置界面返回按钮 */
void setColorCamera::onSetBiasingBackBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
    emit sBiasingCurveStop();
}

/* 响应设置偏置中QTabBar索引号发生改变的事件 */
void setColorCamera::onBiasingTabBarChangedSlt(int index)
{
    switch(index){
    case 0:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingRed[struGsh.nUnit]);
        break;
    case 1:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingGreen[struGsh.nUnit]);
        break;
    case 2:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 响应设置偏置中Lcd显示的值发生改变的事件 */
void setColorCamera::onBiasingValueChangedSlt(int value)
{
    switch(biasingTabBar->currentIndex()){
    case 0:
        struCnfc.struLevelCamera[struGsh.nLevel].nBiasingRed[struGsh.nUnit] = value;
        MySerial.com1Write(CMD_UNIT_BIASING, UNIT, struGsh.nLevel, struGsh.nUnit, 1, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nBiasingRed[struGsh.nUnit], 3);
        break;
    case 1:
        struCnfc.struLevelCamera[struGsh.nLevel].nBiasingGreen[struGsh.nUnit] = value;
        MySerial.com1Write(CMD_UNIT_BIASING, UNIT, struGsh.nLevel, struGsh.nUnit, 2, 0, 0, 0,
                          struCnfc.struLevelCamera[struGsh.nLevel].nBiasingGreen[struGsh.nUnit], 3);
        break;
    case 2:
        struCnfc.struLevelCamera[struGsh.nLevel].nBiasingBlue[struGsh.nUnit] = value;
        MySerial.com1Write(CMD_UNIT_BIASING, UNIT, struGsh.nLevel, struGsh.nUnit, 4, 0, 0, 0,
                           struCnfc.struLevelCamera[struGsh.nLevel].nBiasingBlue[struGsh.nUnit], 3);
        break;
    default:
        break;
    }
}

/* 响应设置偏置界面板号变化的事件 */
void setColorCamera::onBiasingBoardNumChangedSlt()
{
    switch(biasingTabBar->currentIndex()){
    case 0:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingRed[struGsh.nUnit]);
        break;
    case 1:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingGreen[struGsh.nUnit]);
        break;
    case 2:
        biasingValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nBiasingBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 响应彩色相机设置界面中的模拟增益按钮 */
void setColorCamera::onGainAdjustBtnClicked()
{
    stackedWidget->setCurrentIndex(GAIN_ADJUST_PAGE);
    refreshGainDisplay();
    emit sGainAdjustCurveStart();
}

/* 响应模拟增益中的返回按钮 */
void setColorCamera::onGainAdjustBackBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
    emit sGainAdjustCurveStop();
}

/* 响应模拟增益中的增加按钮 */
void setColorCamera::onAnaGainPlusBtnClicked()
{
    int tmp = 0;
    if (struCnfc.nSensorType == SENSOR_D2) {
        tmp = anaGainLcdNumD2->value();
        if (tmp < 1023) {
           anaGainLcdNumD2->display(tmp+1);
        } else {
           anaGainLcdNumD2->display(1023);
        }
    } else {
        tmp = anaGainLcdNum->value();
		if (tmp > 7) {
			tmp = 7;
            anaGainLcdNum->display(7);
		}
        if(tmp == 3) {
            anaGainLcdNum->display(7);
        } else {
            if (tmp < 3) {
                anaGainLcdNum->display(tmp+1);
            }
        }
    }
    setAnaGainParams();
}

/* 响应模拟增益中的减小按钮 */
void setColorCamera::onAnaGainMinusBtnClicked()
{
    int tmp = 0;
    if (struCnfc.nSensorType == SENSOR_D2) {
        tmp = anaGainLcdNumD2->value();
        if(tmp > 0)
           anaGainLcdNumD2->display(tmp - 1);
        else
           anaGainLcdNumD2->display(1023);
    } else {
        tmp = anaGainLcdNum->value();
		if (tmp > 7) {
			tmp = 7;
		}
        if(tmp == 7) {
            anaGainLcdNum->display(3);
        } else {
            if (tmp > 0) {
                anaGainLcdNum->display(tmp - 1);
            }
        }
    }
    setAnaGainParams();
}

/* 响应模拟增益中的QTabBar的索引变化的事件 */
void setColorCamera::onGainAdjustTabBarChangedSlt(int index)
{  
    //模拟增益界面刷新
    switch(index){
    case 0:
        if (struCnfc.nSensorType == SENSOR_D2) {
            anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        } else {
            anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        }
        break;
    case 1:
        if (struCnfc.nSensorType == SENSOR_D2) {
            anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        } else {
            anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        }
        break;
    case 2:
        if (struCnfc.nSensorType == SENSOR_D2) {
            anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        } else {
            anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        }
        break;
    }
    //数字增益界面刷新
    switch(index){
    case 0:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit]);
        break;
    case 1:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit]);
        break;
    case 2:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 响应模拟增益界面中的板号变化的事件 */
void setColorCamera::onGainAdjustBoardNumChangedSlt()
{
    //模拟增益参数响应
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainRed[struGsh.nUnit]);
        break;
    case 1:
        anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainGreen[struGsh.nUnit]);
        break;
    case 2:
        anaGainLcdNumD2->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        anaGainLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nAnaGainBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
    //数字增益参数响应
    switch(GainAdjustTabBar->currentIndex()){
    case 0:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperRed[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowRed[struGsh.nUnit]);
        break;
    case 1:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperGreen[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowGreen[struGsh.nUnit]);
        break;
    case 2:
        digGainUpperLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainUpperBlue[struGsh.nUnit]);
        digGainValueBar->setValue(struCnfc.struLevelCamera[struGsh.nLevel].nDigiGainLowBlue[struGsh.nUnit]);
        break;
    default:
        break;
    }
}

/* 响应数字增益置中的粗调增加按钮 */
void setColorCamera::onDigUpperPlusBtnClicked()
{
    int tmp = digGainUpperLcdNum->value();
    if(tmp < 15)
        digGainUpperLcdNum->display(tmp+1);
    setDigGainUpperParams();
}

/* 响应数字增益置中的粗调减小按钮 */
void setColorCamera::onDigUpperMinusBtnClicked()
{
    int tmp = digGainUpperLcdNum->value();
    if(tmp > 0)
        digGainUpperLcdNum->display(tmp-1);
    setDigGainUpperParams();
}

/* 响应数字增益中细调值变化的事件 */
void setColorCamera::onDigGainLowValueChangedSlt()
{
    setDigGainLowParams();
}

/* 响应彩色相机设置界面中的划分象元按钮 */
void setColorCamera::onDivChannelBtnClicked()
{
    myFlow.updateOnoff(1);  //进入象元划分界面时发送校准开始命令
    stackedWidget->setCurrentIndex(DIV_CHANNEL_PAGE);
    refreshDivDisplay();
    refreshDivChannelList2();
    emit sDivChannelCurveStart();
}

/* 响应划分象元界面的返回按钮 */
void setColorCamera::onDivChannelBackBtnClicked()
{
    myFlow.updateOnoff(0);// 退出象元划分界面时发送校准停止命令
    emit sDivChannelCurveStop();
    
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
}

#if 0
/* 响应划分象元界面的增加起始象元按钮 */
void setColorCamera::onStartChannelPlusBtnClicked()
{
    int tmp = beginChannelLcdNum->value();
    if (tmp < struCnfc.nSensorPixelLevel[struGsh.nLevel]-1) {
        beginChannelLcdNum->display(tmp+1);
    }
    calDivChannelParams(beginChannelLcdNum->value(), endChannelLcdNum->value());
}

/* 响应划分象元界面的减小起始象元按钮 */
void setColorCamera::onStartChannelMinusBtnClicked()
{
    int tmp = beginChannelLcdNum->value();
    if (tmp > 0) {
        beginChannelLcdNum->display(tmp-1);
    }
    calDivChannelParams(beginChannelLcdNum->value(), endChannelLcdNum->value());
}

/* 响应划分象元界面的增加终止象元按钮 */
void setColorCamera::onEndChannelPlusBtnClicked()
{
    int tmp = endChannelLcdNum->value();
    if (tmp < struCnfc.nSensorPixelLevel[struGsh.nLevel]-1) {
        endChannelLcdNum->display(tmp+1);
    }
    calDivChannelParams(beginChannelLcdNum->value(), endChannelLcdNum->value());
}

/* 响应划分象元界面的减小终止象元按钮 */
void setColorCamera::onEndChannelMinusBtnClicked()
{
    int tmp = endChannelLcdNum->value();
    if (tmp > 0) {
        endChannelLcdNum->display(tmp-1);
    }
    calDivChannelParams(beginChannelLcdNum->value(), endChannelLcdNum->value());
}
#endif

/* 响应划分象元界面的起始象元被按下的事件 */
void setColorCamera::onStartChannelPressed()
{
    int tmp = beginChannelLcdNum->value();
    myInputPanel inputDlg(intType, 0, struCnfc.nSensorPixelLevel[struGsh.nLevel]-1, tmp);
    int ret = inputDlg.exec();
	int begin = 0;
	int end = 0;
    if (ret == QDialog::Accepted) {
        beginChannelLcdNum->display(inputDlg.getValue());
		begin = beginChannelLcdNum->value();
		end   = endChannelLcdNum->value(); 
        calDivChannelParams(begin, end);
    }
}

/* 响应划分象元界面的终止象元被按下的事件 */
void setColorCamera::onEndChannelPressed()
{
    int tmp = endChannelLcdNum->value();
    myInputPanel inputDlg(intType, 0, struCnfc.nSensorPixelLevel[struGsh.nLevel]-1, tmp);
    int ret = inputDlg.exec();
	int begin = 0;
	int end = 0;
    if (ret == QDialog::Accepted) {
        endChannelLcdNum->display(inputDlg.getValue());
		begin = beginChannelLcdNum->value();
		end   = endChannelLcdNum->value(); 
        calDivChannelParams(begin, end);
    }
}

/* 响应划分像元界面的起始喷嘴被按下的事件 */
void setColorCamera::onBeginEjectorPressed()
{
    int begin = beginEjectorLcdNum->value();
    int end = endEjectorLcdNum->value();
    myInputPanel inputDlg(intType, 0, struCnfg.nEjectorsPerChute-1, begin);

    if (inputDlg.exec() == QDialog::Accepted) {
        int tmp = inputDlg.getValue();
        if (tmp <= end) {
            beginEjectorLcdNum->display(tmp);
            struCnfc.struLevelCamera[struGsh.nLevel].nEjectorBegin[struGsh.nUnit] = tmp;

            // 重新划分
            calDivChannelParams(beginChannelLcdNum->value(), endChannelLcdNum->value());
        }
    }
}

/* 响应划分像元界面的终止喷嘴被按下的事件 */
void setColorCamera::onEndEjectorPressed()
{
    int begin = beginEjectorLcdNum->value();
    int end = endEjectorLcdNum->value();
    myInputPanel inputDlg(intType, 0, struCnfg.nEjectorsPerChute-1, end);

    if (inputDlg.exec() == QDialog::Accepted) {
        int tmp = inputDlg.getValue();
        if (tmp >= begin) {
            endEjectorLcdNum->display(tmp);
            struCnfc.struLevelCamera[struGsh.nLevel].nEjectorEnd[struGsh.nUnit] = tmp;

            // 重新划分
            calDivChannelParams(beginChannelLcdNum->value(), endChannelLcdNum->value());
        }
    }
}

/* 玉米定制响应划分象元界面的相机左右侧按钮 */
void setColorCamera::onCameraBodyBtnClicked()
{
    m_ncameraBodyID = (m_ncameraBodyID == 0) ? 1 : 0;
    refreshDivDisplay();
}

/*玉米定制象元界面的象元列表*/
void setColorCamera::listLeftViewPage()
{
    for (int i = 0; i < (struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal + 2) / 4 + 1; i++) {
        for (int j = 9; j < COLUMN_NUM; j++){
            listLabel[i][j]->hide();
        }
        if (m_nChannelPageID2 == 0) {   //隐藏后半部分
            for (int j = 1; j <= 4; j++){
                listLabel[i][j]->show();
            }
            for (int j = 5; j <= 8; j++){
                listLabel[i][j]->hide();
            }
        } else {
            for (int j = 1; j <= 4; j++){
                listLabel[i][j]->hide();
            }
            for (int j = 5; j <= 8; j++){
                listLabel[i][j]->show();
            }
        }
    }
    for (int i = (struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal + 2) / 4; i < ROW_NUM; i++) {
        for(int j = 0; j < COLUMN_NUM; j++){
            listLabel[i][j]->hide();
        }
    }
}

void setColorCamera::listRightViewPage()
{
    int lineTmp = 0;
    int unitTmp = 0;

    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2+1; i++) {
        unitTmp = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
        lineTmp = i / 2 + 1;
        if(i%2 == 0){
            listLabel[lineTmp][1]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelBegin[unitTmp][0]));
            listLabel[lineTmp][2]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelEnd[unitTmp][0]));
            listLabel[lineTmp][3]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelBegin[unitTmp][1]));
            listLabel[lineTmp][4]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelEnd[unitTmp][1]));
        }
        else{
            listLabel[lineTmp][5]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelBegin[unitTmp][0]));
            listLabel[lineTmp][6]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelEnd[unitTmp][0]));
            listLabel[lineTmp][7]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelBegin[unitTmp][1]));
            listLabel[lineTmp][8]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nMaizeChannelEnd[unitTmp][1]));
        }
    }
    for(int j = 0; j < COLUMN_NUM; j++){
        listLabel[(struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal + 2) / 4][j]->show();
    }
    for (int i = 0; i < (struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal + 2) / 4 + 1; i++) {
        for (int j = 9; j < COLUMN_NUM; j++){
            listLabel[i][j]->hide();
        }
        if (m_nChannelPageID2 == 0) {   //隐藏后半部分
            for (int j = 1; j <= 4; j++){
                listLabel[i][j]->show();
            }
            for (int j = 5; j <= 8; j++){
                listLabel[i][j]->hide();
            }
        } else {
            for (int j = 1; j <= 4; j++){
                listLabel[i][j]->hide();
            }
            for (int j = 5; j <= 8; j++){
                listLabel[i][j]->show();
            }
        }
    }
    for (int i = (struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal + 2) / 4 + 1; i < ROW_NUM; i++) {
        for(int j = 0; j < COLUMN_NUM; j++){
            listLabel[i][j]->hide();
        }
    }
}
/*!
 * \brief setColorCamera::onDivChannelRangeBtnPressed
 */
void setColorCamera::onDivChannelRangeBtnPressed()
{
    stackedWidget->setCurrentIndex(CHANNEL_RANGE_PAGE);
}

/* 响应划分象元界面的象元列表按钮 */
void setColorCamera::onDivChannelListBtnClicked()
{
    divChannelListPageConvertBtn->hide();
    divChannelListPageConvertBtn2->hide();
    listLabel[0][1]->setText(myLan.front_begin);
    listLabel[0][2]->setText(myLan.front_end);
    listLabel[0][3]->setText(myLan.rear_begin);
    listLabel[0][4]->setText(myLan.rear_end);


//        qDebug()<<"struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal:"<<struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {
            if (i%2 == 0) {
                listLabel[i/2+1][1]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelBegin[i]));
                listLabel[i/2+1][2]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelEnd[i]));
            } else {
                listLabel[i/2+1][3]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelBegin[i]));
                listLabel[i/2+1][4]->setText(QString("%1").arg(struCnfc.struLevelCamera[0].nChannelEnd[i]));
            }
        }

        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2+1; i++) {
            for (int j = 5; j < COLUMN_NUM; j++) {
                listLabel[i][j]->hide();
            }
        }
        for(int i=struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2+1; i<ROW_NUM; i++){
            for(int j=0; j<COLUMN_NUM; j++){
                listLabel[i][j]->hide();
            }
        }
    stackedWidget->setCurrentIndex(CHANNEL_LIST_PAGE);
    emit sDivChannelCurveStop();
}

/* 玉米定制响应划分象元界面的自动划分按钮 */
void setColorCamera::onAutoDivChannelBtnClickedMultiView_1()
{
    int row[struCnfc.nSensorPixelLevel[struGsh.nLevel]];
    bool bStartFound = false;
    bool bEndFound = false;
    int begin = 0;
    int end = 0;

	int begin_s = 0;
	int end_s   = 0;
	bool bFirstStartFound = false;
	bool bFirstEndFound = false;

    int backgroudID = 0;
    int nSensorPixelRight = 0;//相机像素划象元范围的右限
    int nSensorPixelLeft = 0; //相机像素划象元范围的左限

    int nChannelEjectorsRangeLeft = 0;  //待划分象元的通道喷嘴的起始下标，左限（本组包含）
    int nChannelEjectorsRangeRight = 0; //待划分象元的通道喷嘴的结束下标，右限（本组不包含）

    nSensorPixelLeft  = 0;
    nSensorPixelRight = struCnfc.nSensorPixelLevel[struGsh.nLevel];

    backgroudID = struGsh.nUnit % 2;
    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 1:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowRed[i];
        }
        break;
    case 2:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowGreen[i];
        }
        break;
    case 4:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowBlue[i];
        }
        break;
    case 0:
    case 7:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowRed[i]+struGsh.sRowGreen[i]+struGsh.sRowBlue[i];
        }
        break;
    default:
        break;
    }
    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 1:
    case 2:
    case 4:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]){
				if (!bFirstStartFound) {
					bFirstStartFound = !bFirstStartFound;
                	begin = i;
				}
                bStartFound = true;
                break;
            } else {
				if (bFirstStartFound) {
        	        bStartFound = true;
					begin_s = i - 1;
            	    break;
				}
			}
        }
        for(int i=nSensorPixelRight-1; i>=nSensorPixelLeft; i--){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]){
				if (!bFirstEndFound) {
					bFirstEndFound = !bFirstEndFound;
                	end = i;
				}
                bEndFound = true;
                break;
            } else {
				if (bFirstEndFound) {
             	    bEndFound = true;
					end_s = i + 1;
               		break;
				}
			}
        }
        break;
    case 0:
    case 7:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
				if (!bFirstStartFound) {
					bFirstStartFound = !bFirstStartFound;
                	begin = i;
				}
                bStartFound = true;
                break;
            } else {
				if (bFirstStartFound) {
        	        bStartFound = true;
					begin_s = i - 1;
            	    break;
				}
			}
        }
        for(int i=nSensorPixelRight-1; i>=nSensorPixelLeft; i--){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
				if (!bFirstEndFound) {
					bFirstEndFound = !bFirstEndFound;
                	end = i;
				}
                bEndFound = true;
                break;
            } else {
				if (bFirstEndFound) {
             	    bEndFound = true;
					end_s = i + 1;
               		break;
				}
			}
        }
        break;
    }

    m_ncameraBodyID = getCameraBodyID(begin, end);
    nChannelEjectorsRangeLeft  = 0;
    nChannelEjectorsRangeRight = struCnfg.nEjectorsPerChute;

    if(bStartFound && bEndFound){
        beginChannelLcdNum->display(begin);
        endChannelLcdNum->display(end);
        calDivChannelParams(begin, end);
    }
}

void setColorCamera::onAutoDivChannelBtnClickedMultiView_2()
{
    int row[struCnfc.nSensorPixelLevel[struGsh.nLevel]];
    bool bStartFound = false;
    bool bEndFound = false;
    int begin = 0;
    int end = 0;

    int begin_s = 0;
    int end_s   = 0;
    bool bFirstStartFound = false;
    bool bFirstEndFound = false;

    int backgroudID = 0;
    int nSensorPixelRight = 0;//相机像素划象元范围的右限
    int nSensorPixelLeft = 0; //相机像素划象元范围的左限

    int nChannelEjectorsRangeLeft = 0;  //待划分象元的通道喷嘴的起始下标，左限（本组包含）
    int nChannelEjectorsRangeRight = 0; //待划分象元的通道喷嘴的结束下标，右限（本组不包含）

    nSensorPixelLeft  = 0;
    nSensorPixelRight = struCnfc.nSensorPixelLevel[struGsh.nLevel];

    backgroudID = struGsh.nUnit % 2;
    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 1:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowRed[i];
        }
        break;
    case 2:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowGreen[i];
        }
        break;
    case 4:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowBlue[i];
        }
        break;
    case 0:
    case 7:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            row[i] = struGsh.sRowRed[i]+struGsh.sRowGreen[i]+struGsh.sRowBlue[i];
        }
        break;
    default:
        break;
    }
    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 1:
    case 2:
    case 4:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]){
                if (!bFirstStartFound) {
                    bFirstStartFound = !bFirstStartFound;
                    begin = i;
                }
                continue;
            } else {
                if (bFirstStartFound) {
                    bStartFound = true;
                    begin_s = i - 1;
                    break;
                }
            }
        }
        for(int i=nSensorPixelRight-1; i>=nSensorPixelLeft; i--){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]){
                if (!bFirstEndFound) {
                    bFirstEndFound = !bFirstEndFound;
                    end = i;
                }
                continue;
            } else {
                if (bFirstEndFound) {
                    bEndFound = true;
                    end_s = i + 1;
                    break;
                }
            }
        }
        break;
    case 0:
    case 7:
        for(int i=nSensorPixelLeft; i<nSensorPixelRight; i++){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
                if (!bFirstStartFound) {
                    bFirstStartFound = !bFirstStartFound;
                    begin = i;
                }
                continue;
            } else {
                if (bFirstStartFound) {
                    bStartFound = true;
                    begin_s = i - 1;
                    break;
                }
            }
        }
        for(int i=nSensorPixelRight-1; i>=nSensorPixelLeft; i--){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
                if (!bFirstEndFound) {
                    bFirstEndFound = !bFirstEndFound;
                    end = i;
                }
                continue;
            } else {
                if (bFirstEndFound) {
                    bEndFound = true;
                    end_s = i + 1;
                    break;
                }
            }
        }
        break;
    }

    m_ncameraBodyID = getCameraBodyID(begin, end);
    refreshDivDisplay();
    if (m_ncameraBodyID == 0) { //视点在左侧
        nChannelEjectorsRangeLeft  = 0;
        nChannelEjectorsRangeRight = struCnfg.nEjectorsPerChute/2;
        //begin = (begin_s + begin) / 2 + 1;//LD4没有特殊划象元的工装，所以此处该功能不需要，2015.8.25

    } else { //视点在右侧
        nChannelEjectorsRangeLeft  = struCnfg.nEjectorsPerChute/2;
        nChannelEjectorsRangeRight = struCnfg.nEjectorsPerChute;
        //end = (end_s + end) / 2;
    }

    if(bStartFound && bEndFound){
        beginChannelLcdNum->display(begin);
        endChannelLcdNum->display(end);
        calDivChannelParams(begin, end);
    }
}

/* 响应划分象元界面的自动划分按钮 */
void setColorCamera::onAutoDivChannelBtnClicked()
{
    int row[struCnfc.nSensorPixelLevel[struGsh.nLevel]];
    bool bStartFound = false;
    bool bEndFound = false;
    int begin;
    int end;
    int backgroudID = 0;

    backgroudID = struGsh.nUnit % 2;
    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 1:
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            row[i] = struGsh.sRowRed[i];
        }
        break;
    case 2:
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            row[i] = struGsh.sRowGreen[i];
        }
        break;
    case 4:
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            row[i] = struGsh.sRowBlue[i];
        }
        break;
    case 0:
    case 7:
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            row[i] = struGsh.sRowRed[i]+struGsh.sRowGreen[i]+struGsh.sRowBlue[i];
        }
        break;
    default:
        break;
    }

    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
	case 1:
	case 2:
	case 4:
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]){
                begin = i;
                bStartFound = true;
                break;
            }
        }
        for(int i=struCnfc.nSensorPixelLevel[struGsh.nLevel]-1; i>=0; i--){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]){
                end = i;
                bEndFound = true;
                break;
            }
        }
	    break;
	case 0:	//! 黑背景
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            if(row[i] > struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
                begin = i;
                bStartFound = true;
                break;
            }
        }
        for(int i=struCnfc.nSensorPixelLevel[struGsh.nLevel]-1; i>=0; i--){
            if(row[i] > struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
                end = i;
                bEndFound = true;
                break;
            }
        }
	    break;

	case 7:	//! 白背景
        for(int i=0; i<struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
                begin = i;
                bStartFound = true;
                break;
            }
        }
        for(int i=struCnfc.nSensorPixelLevel[struGsh.nLevel]-1; i>=0; i--){
            if(row[i] < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3){
                end = i;
                bEndFound = true;
                break;
            }
        }
	    break;
		
	default:
		break;
	}

    if (bStartFound && bEndFound) {
        calDivChannelParams(begin, end);
        beginChannelLcdNum->display(begin);
        endChannelLcdNum->display(end);
	}
}

/* 响应象元列表界面的翻页按钮 */
void setColorCamera::onDivChannelListPageConvertBtnClicked()
{
    m_nChannelPageID = (m_nChannelPageID == 0) ? 1 : 0;

}

void setColorCamera::onDivChannelListPageConvertBtn2Clicked()
{
    m_nChannelPageID2 = (m_nChannelPageID2 == 0) ? 1 : 0;
}

/* 响应象元列表界面的返回按钮 */
void setColorCamera::onDivChannelListBackBtnClicked()
{
    stackedWidget->setCurrentIndex(DIV_CHANNEL_PAGE);
    emit sDivChannelCurveStart();
}
/* 响应象元列表界面的象元重置按钮 */
void setColorCamera::onDivChannelListResetBtnClicked(int type)
{
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;

    infoWidget->setLabelText(myLan.msg_divide_channel);
    infoWidget->delayShow();
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            int nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];

            MySerial.com1Write(CMD_UNIT_CHANNEL, UNIT, i, nUnitAddr, 0,
                struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]/256,
                struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]%256,
                struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]/256,
                struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]%256, 3);

            for (int k = 0; k < struCnfg.nEjectorsPerChute; k++) {
                MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, i, nUnitAddr, k,
                                struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k]/256,
                                struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k]%256,
                                struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k]/256,
                                struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k]%256, 3);
            }
            if(struCnfg.aiEnable == 1){
                args.clear();
                args[0] = AIUNIT;
                //ai相机编号，取余
                args[1] = nUnitAddr%2;
                args[2] = struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]/256;
                args[3] = struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]%256;
                args[4] = struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]/256;
                args[5] = struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]%256;
                MyUpd.writeDatagram(CMD_AI_PIXEL_SEND, nUnitAddr/2, 6, args, struGsh.addressList.at(nUnitAddr/2), AI_UDP_SEND_PORT);
                data.nCommandAddress = CMD_AI_PIXEL_SEND;
                ret = MyUpd.readUdpDatagrams(&data,13);
                if(ret !=0){
                    qDebug("aiDevice: %d, ret: %d",  i, ret);
                }
            }
        }
    }

    infoWidget->hide();
}

/* 响应象元列表界面的象元重置按钮 */
void setColorCamera::onDivChannelListResetBtnClicked2(int type)
{
    QByteArray args;
    AI_Data_Protocol_D data;
    int ret;
    infoWidget->setLabelText(myLan.msg_divide_channel);
    infoWidget->delayShow();
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            int nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];

            MySerial.com1Write(CMD_UNIT_CHANNEL, UNIT, i, nUnitAddr, 0,
                struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]/256,
                struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]%256,
                struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]/256,
                struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]%256, 3);

            for (int k = 0; k < struCnfg.nEjectorsPerChute; k++) {
                MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, i, nUnitAddr, k,
                                struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k]/256,
                                struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k]%256,
                                struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k]/256,
                                struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k]%256, 3);
            }
            if(struCnfg.aiEnable == 1){
                args.clear();
                args[0] = AIUNIT;
                //ai相机编号，取余
                args[1] = nUnitAddr%2;
                args[2] = struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]/256;
                args[3] = struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr]%256;
                args[4] = struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]/256;
                args[5] = struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr]%256;
                MyUpd.writeDatagram(CMD_AI_PIXEL_SEND, nUnitAddr/2, 6, args, struGsh.addressList.at(nUnitAddr/2), AI_UDP_SEND_PORT);
                data.nCommandAddress = CMD_AI_PIXEL_SEND;
                ret = MyUpd.readUdpDatagrams(&data,13);
                if(ret !=0){
                    qDebug("aiDevice: %d, ret: %d",  i, ret);
                }
            }
        }
    }

    infoWidget->hide();
}

/* 响应象元划分界面中板号变化的事件 */
void setColorCamera::onDivChannelBoardNumChangedSlt()
{
    beginChannelLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]);
    endChannelLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]);
    beginEjectorLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nEjectorBegin[struGsh.nUnit]);
    endEjectorLcdNum->display(struCnfc.struLevelCamera[struGsh.nLevel].nEjectorEnd[struGsh.nUnit]);
}

/* 响应彩色相机设置界面中的传感器参数按钮 */
void setColorCamera::onSensorParametersBtnClicked()
{
    refreshSensorDisplay();
    stackedWidget->setCurrentIndex(SENSOR_PARAMS_PAGE);
}

/* 响应传感器参数界面中的行频增加按钮 */
void setColorCamera::onSensorPlusBtnClicked()
{
    int nVal = horizontalFreNum->value()*10 + 0.5;
    if (nVal < 55) {
        nVal++;
        m_fRowFre = (float)nVal/10.0;
        horizontalFreNum->display(m_fRowFre);
    }
}

/* 响应传感器参数界面中的行频减少按钮 */
void setColorCamera::onSensorMinusBtnClicked()
{

    int nVal = horizontalFreNum->value()*10 + 0.5;
    if (nVal > 1) {
        nVal--;
        m_fRowFre = (float)nVal/10.0;
        horizontalFreNum->display(m_fRowFre);
    }
}

/* 响应传感器参数界面中的确定按钮 */
void setColorCamera::onSensorParametersSureBtnClicked()
{
    struCnfc.nSensorRowFre = m_fRowFre*10 + 0.5;
    struCnfc.nSensorType = m_nSensorType;
    struCnfp.nSensorMode = m_nSensorMode;

    // D2系相机的增益、模拟增益参数中蓝背景不可调，故隐藏
    if (struCnfc.nSensorType == SENSOR_D2) {
        if (GainAdjustTabBar->count() >= 3) {
            GainAdjustTabBar->removeTab(2);
            GainAdjustTabBar->setFixedWidth(200);
            anaGainLcdNum->hide();
            anaGainLcdNumD2->show();
        }
        if (biasingTabBar->count() >= 3) {
            biasingTabBar->removeTab(2);
            biasingTabBar->setMinimumWidth(300);
            biasingTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:150;}");
            biasingValueBar->setMaxValue(255);
        }
    } else {
        if (GainAdjustTabBar->count() < 3) {
            GainAdjustTabBar->insertTab(2, myIcon.Mark_Blue, myLan.blue);
            GainAdjustTabBar->setMinimumWidth(300);
            anaGainLcdNum->show();
            anaGainLcdNumD2->hide();
        }
        if (biasingTabBar->count() < 3) {
            biasingTabBar->insertTab(2, myIcon.Mark_Blue, myLan.blue);
            biasingTabBar->setMinimumWidth(300);
            biasingTabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}");
            biasingValueBar->setMaxValue(31);
        }
    }

    // 传感器帧频
    int rowFreValue = 0;
    //! T3帧频f与界面设置的速度v计算公式为：f=((28*A)/v-100)*10，其中A=12
    //! 老版本T3驱动帧频与速度v计算公式为：f=((28*A)/v-100)*120，其中A=10
    int tmpRowFreT3 = (336.0/(0.1*struCnfc.nSensorRowFre)-100)*10 + 0.5;
    switch(struCnfc.nSensorType) { // K系
    case SENSOR_K:
        rowFreValue = 768000/struCnfc.nSensorRowFre;
        break;
    case SENSOR_T_2566:  // T2系,1024 pixels
        rowFreValue = ((double)struGsh.nSensCoefficient/m_fRowFre-163.88)*140+0.5;
        break;
    case SENSOR_T_2564:  // T3系-10K,2048 pixels
        rowFreValue = (tmpRowFreT3 < 0) ? 0 : tmpRowFreT3;
        break;
    case SENSOR_D2:
        break;
    default:
        break;
    }

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            int nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            MySerial.com1Write(CMD_UNIT_ROW_FREQUENCY, UNIT, i, nUnitAddr, 0, 0, 0, rowFreValue/256, rowFreValue%256, 3);
            if(struCnfc.nSensorType == SENSOR_T_2566)
                MySerial.com1Write(CMD_UNIT_SENS_T_MODE, UNIT, i, nUnitAddr, 0, 0, 0, 0, struCnfp.nSensorMode, 3);
        }
    }

    // 传感器分辨率
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        if (sensorPixelCbx[i]->isChecked()) {
            struCnfc.nSensorPixelLevel[i] = SENSOR_PIXELS_2048;
        } else {
            struCnfc.nSensorPixelLevel[i] = SENSOR_PIXELS_1024;
        }
    }
	if (struCnfc.nSensorType == SENSOR_D2) {
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            struCnfc.nSensorPixelLevel[i] = SENSOR_PIXELS_2048;
        }
    } else if (struCnfc.nSensorType == SENSOR_P5) {
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            struCnfc.nSensorPixelLevel[i] = SENSOR_PIXELS_2592;
        }
    }

    myFlow.resetTickAdvParams();

    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
}

/* 响应传感器参数界面中的取消按钮 */
void setColorCamera::onSensorParametersBackBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
    setSensorParams();
    sensorTypeGroup->setCurrentIndex(m_nSensorType-1);
    sensorModeGroup->setCurrentIndex(m_nSensorMode-1);
    horizontalFreNum->display(m_fRowFre);
}

/* 响应传感器参数界面传感器类型变化的事件 */
void setColorCamera::onSensorTypeChangedSlt(int index)
{
    m_nSensorType = index + 1;
    switch (m_nSensorType) {
    case SENSOR_K:
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setChecked(0);
            sensorPixelCbx[i]->setEnabled(0);
        }
        sensorModeGroup->setCurrentIndex(0);
        sensorModeGroup->setEnabled(0);
        break;
    case SENSOR_T_2566://T2
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setChecked(0);
            sensorPixelCbx[i]->setEnabled(0);
        }
        sensorModeGroup->setEnabled(1);
        break;
    case SENSOR_T_2564://T3
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setEnabled(1);
        }
        sensorModeGroup->setCurrentIndex(0);
        sensorModeGroup->setEnabled(0);
        break;
    case SENSOR_D2:
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setChecked(1);
            sensorPixelCbx[i]->setEnabled(0);
        }
        sensorModeGroup->setCurrentIndex(0);
        sensorModeGroup->setEnabled(0);
        break;
    case SENSOR_P5:
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setChecked(true);
            sensorPixelCbx[i]->setEnabled(false);
        }
        sensorModeGroup->setEnabled(0);
        break;
    default:
        break;
    }

    //! P5传感器对应分辨率为2592
    if (m_nSensorType == SENSOR_P5) {
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setText("2592");
        }
    } else {
        for (int i = 0; i < MAX_LEVEL; i++) {
            sensorPixelCbx[i]->setText("2048");
        }
    }
}

/* 响应传感器参数界面传感器模式变化的事件 */
void setColorCamera::onSensorModeChangedSlt(int index)
{
    m_nSensorMode = index+1;
}

/* 响应设置彩色相机界面中的镜头参数按钮 */
void setColorCamera::onLensParametersBtnClicked()
{
    stackedWidget->setCurrentIndex(LENS_PARAMS_PAGE);
}

/* 响应镜头参数界面的选择按钮 */
void setColorCamera::onLensBtnClicked()
{
    m_bIsSelected = !m_bIsSelected;
    if(m_bIsSelected) {
        lensSelectBtn->setIcon(myIcon.Action_Select);
    }  else {
        lensSelectBtn->setIcon(QIcon());
    }
}

/* 响应镜头参数界面的确认按钮 */
void setColorCamera::onLensSureBtnClicked()
{
    if (m_bIsSelected) {
        struCnfc.nLensType = 1;
    } else {
        struCnfc.nLensType = 0;
    }

    myFlow.initChannelPixelCompensate();
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
}

/* 响应镜头参数界面的返回按钮 */
void setColorCamera::onLensBackBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
    if (struCnfc.nLensType == 1) {
        lensSelectBtn->setIcon(myIcon.Action_Select);
    } else {
        lensSelectBtn->setIcon(QIcon());
    }
}

/* 响应对位测试界面按钮 */
void setColorCamera::onContrapositionBtnClicked()
{
    myFlow.updateOnoff(1); // 进入对位测试界面开校正使能
    refreshContrapositionPage();
    stackedWidget->setCurrentIndex(CONTRA_POSITION_PAGE);
}

/* 通道加号响应槽函数*/
void setColorCamera::onPlusChanNumPressedSlt()
{
    int IDTotal;
    if(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal==1) {
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
    } else {
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
    }
    if(struGsh.nUnit < IDTotal-2) {
        struGsh.nUnit += 2;
    }
    refreshContrapositionPage();
}

/* 通道减号响应槽函数*/
void setColorCamera::onMinusChanNumPressedSlt()
{
    if(struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
    }
    refreshContrapositionPage();
}

/* 开始响应槽函数*/
void setColorCamera::onStartBtnPressedSlt()
{
    int nUnitAddr;
    if(struGsh.nUnit%2 == 0) {
        nUnitAddr = struGsh.nUnit;
    } else {
        nUnitAddr = struGsh.nUnit - 1;
    }
    myMessageBox msgBox(MSG_QUES,myLan.start+"?");
    int ret = msgBox.exec();
    if (ret == QDialog::Accepted) {
        MySerial.com1Write(CMD_UNIT_POSITION_START,UNIT,struGsh.nLevel,nUnitAddr,0,0,0,0,1,1);
    } else {
        return;
    }
    myFlow.sleep(2);
    myMessageBox msgBox1(MSG_TIPS,myLan.msg_accept_data);
    ret = msgBox1.exec();
    if (ret == QDialog::Accepted) {
        onAcceptbtnPressedSlt();
    }
}

/* 获取响应槽函数*/
void setColorCamera::onAcceptbtnPressedSlt()
{
    int nUnitAddr;
    rowNum_front = 0;
    rowNum_rear = 0;
    if(struGsh.nUnit%2 == 0) {
        nUnitAddr = struGsh.nUnit;
    } else {
        nUnitAddr = struGsh.nUnit - 1;
    }

    QByteArray rowNumArry_front;
    QByteArray rowNumArry_rear;
    rowNumArry_front.resize(24);
    rowNumArry_front.fill(0);
    rowNumArry_rear.resize(24);
    rowNumArry_rear.fill(0);

    infoWidget->setLabelText(myLan.msg_receiving+QString("1/2"));
    infoWidget->delayShow();
    MySerial.com1Write(CMD_UNIT_POSITION_ACCEPT,UNIT,struGsh.nLevel,nUnitAddr,0,0,0,0,1,1);
    usleep(50000);
    MySerial.com1Read(rowNumArry_front.data(), rowNumArry_front.size(),2*1000);
    rowNumArry_front.remove(0,10);
    rowNumArry_front.truncate(rowNumArry_front.size()-6);

    for(int j=0; j<8; j++){
        printf("%02x ", rowNumArry_front.at(j) & 0xFF);
        rowNum_front += (((quint64)rowNumArry_front.at(j)& 0xFF) << ((7-j)*8));
    }

    infoWidget->setLabelText(myLan.msg_receiving+QString("2/2"));
    infoWidget->delayShow();
    MySerial.com1Write(CMD_UNIT_POSITION_ACCEPT,UNIT,struGsh.nLevel,nUnitAddr+1,0,0,0,0,1,1);
    usleep(50000);
    MySerial.com1Read(rowNumArry_rear.data(), rowNumArry_rear.size(),2*1000);
    rowNumArry_rear.remove(0,10);
    rowNumArry_rear.truncate(rowNumArry_rear.size()-6);

    for(int j=0; j<8; j++){
        printf("%02x ", rowNumArry_rear.at(j) & 0xFF);
        rowNum_rear += (((quint64)rowNumArry_rear.at(j)& 0xFF) << ((7-j)*8));
    }

    if(rowNum_front > rowNum_rear) {
        bias_front = 0;                         //前视偏置
        bias_rear = rowNum_front - rowNum_rear; //后视偏置
    } else {
        bias_rear = 0;                          //后视偏置
        bias_front = rowNum_rear - rowNum_front;//前视偏置
    }

    struCnfc.struLevelCamera[struGsh.nLevel].nContraPosBias[nUnitAddr] = bias_front;
    struCnfc.struLevelCamera[struGsh.nLevel].nContraPosBias[nUnitAddr+1] = bias_rear;

    refreshContrapositionPage();
    infoWidget->hide();
}

/* 对位偏置下发槽函数*/
void setColorCamera::onBiasSendBtnPressedSlt()
{
    int nUnitAddr;
    int nDelay;

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            nDelay = struCnfc.struLevelCamera[i].nContraPosBias[nUnitAddr] + struCnfc.struLevelCamera[i].nContraPosCorrect[nUnitAddr];
            MySerial.com1Write(CMD_INT_POSITION_BIAS,INT,struGsh.nLevel,nUnitAddr,0,0,0,(nDelay>=0)?0:1,abs(nDelay),1);
        }
    }
}

/* 刷新对位测试界面*/
void setColorCamera::refreshContrapositionPage()
{
    int nUnitAddr;
    if(struGsh.nUnit%2 == 0) {
        nUnitAddr = struGsh.nUnit;
    } else {
        nUnitAddr = struGsh.nUnit - 1;
    }
    bias_front = struCnfc.struLevelCamera[struGsh.nLevel].nContraPosBias[nUnitAddr];
    bias_rear = struCnfc.struLevelCamera[struGsh.nLevel].nContraPosBias[nUnitAddr+1];
    bias_correct_front = struCnfc.struLevelCamera[struGsh.nLevel].nContraPosCorrect[nUnitAddr];
    bias_correct_rear = struCnfc.struLevelCamera[struGsh.nLevel].nContraPosCorrect[nUnitAddr+1];
    m_LineEdit[ROW_FRONT]->setText(QString("%1").arg(rowNum_front));
    m_LineEdit[ROW_REAR]->setText(QString("%1").arg(rowNum_rear));

    QString strBiasFront;
    strBiasFront.sprintf("%.1f",(float)(bias_front*0.1));
    m_LineEdit[BIAS_FRONT]->setText(strBiasFront);

    QString strBiasRear;
    strBiasRear.sprintf("%.1f",(float)(bias_rear*0.1));
    m_LineEdit[BIAS_REAR]->setText(strBiasRear);

    QString strCorrectFront;
    strCorrectFront.sprintf("%.1f",(float)(bias_correct_front*0.1));
    m_LineEdit[CORRECT_FRONT]->setText(strCorrectFront);

    QString strCorrectRear;
    strCorrectRear.sprintf("%.1f",(float)(bias_correct_rear*0.1));
    m_LineEdit[CORRECT_REAR]->setText(strCorrectRear);

    m_chanNum->display(struGsh.nUnit/2+1);
}

/* 响应延迟偏置修正 */
void setColorCamera::setCorrectBias(int index)
{
    int ret;
    int nUnitAddr;
    QString str;

    if(struGsh.nUnit%2 == 0) {
        nUnitAddr = struGsh.nUnit;
    } else {
        nUnitAddr = struGsh.nUnit - 1;
    }

    myInputPanel inputDlg1(floatType,-128,128,0);
    inputDlg1.setInputNegEn(true);
    ret = inputDlg1.exec();
    switch(index) {
    case CORRECT_FRONT:
        if (ret == QDialog::Accepted) {
            bias_correct_front = inputDlg1.getValue()*10;
            str.sprintf("%.1f",bias_correct_front*0.1);
            m_LineEdit[index]->setText(str);
            struCnfc.struLevelCamera[struGsh.nLevel].nContraPosCorrect[nUnitAddr] = bias_correct_front;
        }
        break;
    case CORRECT_REAR:
        if (ret == QDialog::Accepted) {
            bias_correct_rear = inputDlg1.getValue()*10;
            str.sprintf("%.1f",bias_correct_rear*0.1);
            m_LineEdit[index]->setText(str);
            struCnfc.struLevelCamera[struGsh.nLevel].nContraPosCorrect[nUnitAddr+1] = bias_correct_rear;
        }
        break;
    }
}

/* 响应对位测试界面返回按钮 */
void setColorCamera::onContrapositionBackBtnClicked()
{
    myFlow.updateOnoff(0); //退出对位测试界面关校正使能
    stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_MAIN_PAGE);
}

void setColorCamera::onPixelBeginBtnPressedSlt()
{
    myInputPanel inputDlg(intType, 0, 2048, (double)struGsh.nPixelBegin);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        struGsh.nPixelBegin = inputDlg.getValue();
        QString str = QString("%1").arg(struGsh.nPixelBegin);
        m_pixelBeginLbe->setText(str);
    }
}

void setColorCamera::onPixelEndBtnPressedSlt()
{
    myInputPanel inputDlg(intType, 0, 2048, (double)struGsh.nPixelEnd);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        struGsh.nPixelEnd = inputDlg.getValue();
        QString str = QString("%1").arg(struGsh.nPixelEnd);
        m_pixelEndLbe->setText(str);
    }
}

void setColorCamera::onChannelRangeBackBtnPressedSlt()
{
    stackedWidget->setCurrentIndex(DIV_CHANNEL_PAGE);
}

/* 返回第一页的槽函数 */
void setColorCamera::gotoFirstPageSlt()
{
    switch(stackedWidget->currentIndex()) {
    case CAL_SIGNAL_PAGE:
        emit sCalibrationCurveStop();
        break;
    case SET_BIASING_PAGE:
        emit sBiasingCurveStop();
        break;
    case GAIN_ADJUST_PAGE:
        emit sGainAdjustCurveStop();
        break;
    case DIV_CHANNEL_PAGE:
        emit sDivChannelCurveStop();
        break;
	default:
		break;
    }

    stackedWidget->setCurrentIndex(0);
}


WholeCalConfirm::WholeCalConfirm(QString text,QWidget *parent) :
    QDialog(parent)
{
    int margin = 25;
    int nLabelHei;

    switch(LCD_WIDTH) {
    case 1024:
        margin = 10;
        nLabelHei = 120;
        break;
    case 640:
        margin = 5;
        nLabelHei = 80;
        break;
    }

    this->setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(LCD_WIDTH/4,LCD_HEIGHT/4,LCD_WIDTH/2,LCD_HEIGHT/2);
    setStyleSheet(g_style2);

    m_textLabel = new myLabel(myLan.default_params, this);
    m_checkBtn = new myPushButton("",QIcon());
    m_checkBtn->setFixedSize(SMALL_BTN_WIDTH,BTN_HEIGHT);
    myLabel *noteLabel = new myLabel(myLan.auto_cal_note);
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::blue);
    noteLabel->setPalette(pe);
    QFont font;
    font.setPixelSize(16);
    noteLabel->setFont(font);

    m_iconLabel   = new myLabel(text, this);
    m_iconLabel->setGeometry(QRect(margin,margin,100,50));
    m_iconLabel->setScaledContents(true);
    m_okBtn      =  new myPushButton(myLan.ok, myIcon.Action_Apply, this);
    m_okBtn->setGeometry(QRect(margin,this->height()-60,BTN_WIDTH,BTN_HEIGHT));
    m_cancelBtn  =  new myPushButton(myLan.cancel, myIcon.Action_Cancel, this);
    m_cancelBtn->setGeometry(QRect(width()-BTN_WIDTH-margin,height()-60,BTN_WIDTH,BTN_HEIGHT));

    QGridLayout *checklayout = new QGridLayout();
    checklayout->addWidget(m_checkBtn,0,0,1,1,Qt::AlignRight);
    checklayout->addWidget(m_textLabel,0,1,1,1,Qt::AlignLeft);
    checklayout->addWidget(noteLabel,1,0,1,2,Qt::AlignCenter);
    QVBoxLayout *mainlayout = new QVBoxLayout();
    mainlayout->addSpacing(100);
    mainlayout->addLayout(checklayout);
    mainlayout->addSpacing(100);
    setLayout(mainlayout);

    setchecked(true);
    connect(m_checkBtn, SIGNAL(pressed()), this, SLOT(onAutoCalEnableBtnClicked()));
    connect(m_okBtn    ,   SIGNAL(pressed()), this,   SLOT(onOkBtnClicked()));
    connect(m_cancelBtn,  SIGNAL(pressed()),  this,   SLOT(onCancelBtnClicked()));
}

//  确定按钮
void WholeCalConfirm::onOkBtnClicked()
{
    accept();
}

//  取消按钮
void WholeCalConfirm::onCancelBtnClicked()
{
    reject();
}

//响应checkbox点击事件
void WholeCalConfirm::onAutoCalEnableBtnClicked()
{
    m_bIsChecked = !m_bIsChecked;
    if(m_bIsChecked) {
        m_checkBtn->setIcon(myIcon.Action_Select);
    }  else {
        m_checkBtn->setIcon(QIcon());
    }
}

void WholeCalConfirm::setchecked(bool ischecked)
{
    m_bIsChecked = ischecked;
    if(m_bIsChecked) {
        m_checkBtn->setIcon(myIcon.Action_Select);
    }  else {
        m_checkBtn->setIcon(QIcon());
    }
}

bool WholeCalConfirm::isChecked()
{
    return m_bIsChecked;
}
