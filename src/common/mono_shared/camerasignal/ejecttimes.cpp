/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        ejecttimes.cpp
 * \brief       黑白机型吹气频率页面源文件。
 * \date        2015.01.14
 */
#include "ejecttimes.h"

/**********
 *
 * 吹气次数界面类
 *
 *****************************************************/
ejectTimes::ejectTimes(QWidget *parent) :
    QWidget(parent)
{
}

ejectTimes::ejectTimes(QSize size, QWidget *parent):
    QWidget(parent)
{
    setFixedSize(size);
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setFixedSize(size);
    m_nCurrentEjectTimes = 0;

    typeMachine = 1;

    initLocalParams();
    createMainPage();
    createSetPage();
    connectSigAndSlt();

    m_stackedWidget->addWidget(m_mainPage);
    m_stackedWidget->addWidget(m_setPage);
}

/* 初始化页面数据 */
void ejectTimes::initLocalParams()
{
    m_config = g_Config::getInstance();
    m_nOkTimes = 0;
}

/* 创建吹气次数主页面 */
void ejectTimes::createMainPageGeneral()
{
#if LCD_WIDTH == 1024
    QSize size(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_mainPage = new QWidget;
    m_mainPage->setFixedSize(QSize(width(), height()));

    /* 定义布局管理器 */
    QVBoxLayout *mainLay = new QVBoxLayout(m_mainPage);
    QHBoxLayout *upLay = new QHBoxLayout;

    /* 实例化主页面上半部分的控件 */
    m_axis = new MyAxis();
    m_axis->setDrawType(TYPE_EJECT_TIMES);
    m_axis->setFixedSize(width()-200, height()-300);
    QGroupBox *rightGroup = new QGroupBox(myLan.chute);
    rightGroup->setFont(m_config->getFont());
    rightGroup->setFixedSize(100,350);
    rightGroup->setAlignment(Qt::AlignLeft);
    QVBoxLayout *upRightLay = new QVBoxLayout(rightGroup);

    m_modeBtn = new myPushButton("", QIcon());
    m_modeBtn->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_plusBtn = new myPushButton(myIcon.Action_Plus, size);
    m_lcdNum = new QLCDNumber(2);
    m_lcdNum->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_minusBtn = new myPushButton(myIcon.Action_Minus, size);

    upRightLay->addWidget(m_modeBtn, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_plusBtn, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_lcdNum, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_minusBtn, 0, Qt::AlignHCenter);

    upLay->addWidget(m_axis,0,Qt::AlignLeft);
    upLay->addWidget(rightGroup,0,Qt::AlignLeft);

    /* 实例化主页面下半部分的控件 */
    m_frontLabel = new myLabel(myLan.front_view);
    m_frontLabel->setFixedSize(80, BTN_HEIGHT);
    m_frontLabel->setAlignment(Qt::AlignCenter);
    m_rearLabel = new myLabel(myLan.rear_view);
    m_rearLabel->setFixedSize(80, BTN_HEIGHT);
    m_rearLabel->setAlignment(Qt::AlignCenter);
    m_frontTimes = new QLCDNumber(5);
    m_frontTimes->setFixedSize(100, BTN_HEIGHT);
    m_frontTimes->setPalette(QPalette(Qt::red));
    m_rearTimes = new QLCDNumber(5);
    m_rearTimes->setFixedSize(100, BTN_HEIGHT);
    m_rearTimes->setPalette(QPalette(Qt::green));
    m_setBtn = new myPushButton(myLan.eject_set, QIcon());
    m_setBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    /*通用机型页面下半部分的布局*/
    ejectFreStatLabel       =   new myLabel(myLan.eject_stat_enable);
    ejectFreStatBtn         =   new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));
    ejectFreStatLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    if (struCnfp.isFeederAIEnable) {
        ejectFreStatBtn->setIcon(myIcon.Action_Apply);
    }

    m_currentEjectFreqLabel = new myLabel(myLan.current_eject_freq);
    m_currentEjectFreq      = new QLCDNumber(5);
    m_currentEjectFreqLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    m_currentEjectFreq->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));

    feedPercentRangeLabel   = new myLabel(myLan.feeder_range);
    feedPercentRangeLeft    = new myLCDNumber;
    feedPercentRangeRight   = new myLCDNumber;
    feedPercentRangeLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    feedPercentRangeLeft->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    feedPercentRangeRight->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    feedPercentRangeLeft->display(struCnfp.nFeederCapMin);
    feedPercentRangeRight->display(struCnfp.nFeederCapMax);

    ejectFreRangeLabel = new myLabel(myLan.ejector_range);
    ejectFreRangeLeft  = new myLCDNumber;
    ejectFreRangeRight = new myLCDNumber;
    ejectFreRangeLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    ejectFreRangeLeft->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    ejectFreRangeRight->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 50, BTN_HEIGHT));
    ejectFreRangeLeft->display(struCnfp.nEjectTimesMin);
    ejectFreRangeRight->display(struCnfp.nEjectTimesMax);

    myLabel *waveLabel1   = new myLabel("- ->");
    myLabel *waveLabel2   = new myLabel("- ->");
    myLabel *waveLabelEmpty   = new myLabel("     ");
    waveLabel1->setFixedSize(QSize(SMALL_BTN_WIDTH + 50 , BTN_HEIGHT));
    waveLabel2->setFixedSize(QSize(SMALL_BTN_WIDTH + 50 , BTN_HEIGHT));
    waveLabelEmpty->setFixedSize(QSize(SMALL_BTN_WIDTH + 50 , BTN_HEIGHT));

    QGridLayout *downLayGeneral = new QGridLayout;
    downLayGeneral->addWidget(ejectFreStatLabel,0,0,Qt::AlignRight);
    downLayGeneral->addWidget(ejectFreStatBtn,0,1,Qt::AlignRight);
    downLayGeneral->addWidget(m_currentEjectFreqLabel,0,2,Qt::AlignRight);
    downLayGeneral->addWidget(m_currentEjectFreq,0,3,Qt::AlignRight);
    downLayGeneral->addWidget(waveLabelEmpty,0,4,Qt::AlignRight);

    downLayGeneral->addWidget(feedPercentRangeLabel,1,0,Qt::AlignRight);
    downLayGeneral->addWidget(feedPercentRangeLeft,1,1,Qt::AlignRight);
    downLayGeneral->addWidget(waveLabel1,1,2,Qt::AlignCenter);
    downLayGeneral->addWidget(feedPercentRangeRight,1,3,Qt::AlignRight);
    downLayGeneral->addWidget(waveLabelEmpty,1,4,Qt::AlignRight);

    downLayGeneral->addWidget(ejectFreRangeLabel,2,0,Qt::AlignRight);
    downLayGeneral->addWidget(ejectFreRangeLeft,2,1,Qt::AlignRight);
    downLayGeneral->addWidget(waveLabel2,2,2,Qt::AlignCenter);
    downLayGeneral->addWidget(ejectFreRangeRight,2,3,Qt::AlignRight);
    downLayGeneral->addWidget(waveLabelEmpty,2,4,Qt::AlignRight);

    mainLay->addLayout(upLay);
    upLay->setAlignment(Qt::AlignTop);
    downLayGeneral->setAlignment(Qt::AlignTop);
    mainLay->addLayout(downLayGeneral);

#endif

#if LCD_WIDTH == 640
    QSize size(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_mainPage = new QWidget;
    m_mainPage->setFixedSize(QSize(width(), height()));

    /* 定义布局管理器 */
    QVBoxLayout *mainLay = new QVBoxLayout(m_mainPage);
    QHBoxLayout *upLay = new QHBoxLayout;

    /* 实例化主页面上半部分的控件 */
    m_axis = new MyAxis();
    m_axis->setDrawType(TYPE_EJECT_TIMES);
    m_axis->setFixedSize(500, 240);
    QGroupBox *rightGroup = new QGroupBox(myLan.chute);
    rightGroup->setFont(m_config->getFont());
    rightGroup->setFixedSize(90,240);
    rightGroup->setAlignment(Qt::AlignLeft);
    QVBoxLayout *upRightLay = new QVBoxLayout(rightGroup);

    m_modeBtn = new myPushButton("", QIcon());
    m_modeBtn->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_plusBtn = new myPushButton(myIcon.Action_Plus, size);
    m_lcdNum = new QLCDNumber(2);
    m_lcdNum->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_minusBtn = new myPushButton(myIcon.Action_Minus, size);

    upRightLay->addWidget(m_modeBtn, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_plusBtn, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_lcdNum, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_minusBtn, 0, Qt::AlignHCenter);

    upLay->addWidget(m_axis,0,Qt::AlignLeft);
    upLay->addWidget(rightGroup,0,Qt::AlignLeft);

    /* 实例化主页面下半部分的控件 */
    QGroupBox *downGroup = new QGroupBox(myLan.sampling_time+"(s)");
    downGroup->setFont(m_config->getFont());
    downGroup->setFixedSize(350, 120);

    m_frontLabel = new myLabel(myLan.front_view);
    m_frontLabel->setFixedSize(80, BTN_HEIGHT);
    m_frontLabel->setAlignment(Qt::AlignCenter);
    m_rearLabel = new myLabel(myLan.rear_view);
    m_rearLabel->setFixedSize(80, BTN_HEIGHT);
    m_rearLabel->setAlignment(Qt::AlignCenter);
    m_frontTimes = new QLCDNumber(5);
    m_frontTimes->setFixedSize(100, BTN_HEIGHT);
    m_frontTimes->setPalette(QPalette(Qt::red));
    m_rearTimes = new QLCDNumber(5);
    m_rearTimes->setFixedSize(100, BTN_HEIGHT);
    m_rearTimes->setPalette(QPalette(Qt::green));

    m_setBtn = new myPushButton(myLan.eject_set, QIcon());
    m_setBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    /*通用机型页面下半部分的布局*/
    ejectFreStatLabel       =   new myLabel(myLan.eject_stat_enable);
    ejectFreStatBtn         =   new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));
    ejectFreStatLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 20, BTN_HEIGHT));
    if (struCnfp.isFeederAIEnable) {
        ejectFreStatBtn->setIcon(myIcon.Action_Apply);
    }

    m_currentEjectFreqLabel = new myLabel(myLan.current_eject_freq);
    m_currentEjectFreq      = new QLCDNumber(5);
    m_currentEjectFreqLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 20, BTN_HEIGHT));
    m_currentEjectFreq->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 10, BTN_HEIGHT));

    feedPercentRangeLabel   = new myLabel(myLan.feeder_range);
    feedPercentRangeLeft    = new myLCDNumber;
    feedPercentRangeRight   = new myLCDNumber;
    feedPercentRangeLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 20, BTN_HEIGHT-10));
    feedPercentRangeLeft->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 10, BTN_HEIGHT-10));
    feedPercentRangeRight->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 10, BTN_HEIGHT-10));
    feedPercentRangeLeft->display(struCnfp.nFeederCapMin);
    feedPercentRangeRight->display(struCnfp.nFeederCapMax);

    ejectFreRangeLabel = new myLabel(myLan.ejector_range);
    ejectFreRangeLeft  = new myLCDNumber;
    ejectFreRangeRight = new myLCDNumber;
    ejectFreRangeLabel->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 20, BTN_HEIGHT-10));
    ejectFreRangeLeft->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 10, BTN_HEIGHT-10));
    ejectFreRangeRight->setFixedSize(QSize(SMALL_BTN_WIDTH*2 + 10, BTN_HEIGHT-10));
    ejectFreRangeLeft->display(struCnfp.nEjectTimesMin);
    ejectFreRangeRight->display(struCnfp.nEjectTimesMax);

    myLabel *waveLabel1   = new myLabel("- ->");
    myLabel *waveLabel2   = new myLabel("- ->");
    myLabel *waveLabelEmpty   = new myLabel("     ");
    waveLabel1->setFixedSize(QSize(SMALL_BTN_WIDTH , BTN_HEIGHT-10));
    waveLabel2->setFixedSize(QSize(SMALL_BTN_WIDTH , BTN_HEIGHT-10));
    waveLabelEmpty->setFixedSize(QSize(SMALL_BTN_WIDTH , BTN_HEIGHT-10));

    QGridLayout *downLayGeneral = new QGridLayout;
    downLayGeneral->addWidget(ejectFreStatLabel,0,0,Qt::AlignLeft);
    downLayGeneral->addWidget(ejectFreStatBtn,0,1,Qt::AlignLeft);
    downLayGeneral->addWidget(m_currentEjectFreqLabel,0,2,Qt::AlignLeft);
    downLayGeneral->addWidget(m_currentEjectFreq,0,3,Qt::AlignLeft);
    downLayGeneral->addWidget(waveLabelEmpty,0,4,Qt::AlignLeft);

    downLayGeneral->addWidget(feedPercentRangeLabel,1,0,Qt::AlignLeft);
    downLayGeneral->addWidget(feedPercentRangeLeft,1,1,Qt::AlignLeft);
    downLayGeneral->addWidget(waveLabel1,1,2,Qt::AlignCenter);
    downLayGeneral->addWidget(feedPercentRangeRight,1,3,Qt::AlignLeft);
    downLayGeneral->addWidget(waveLabelEmpty,1,4,Qt::AlignLeft);

    downLayGeneral->addWidget(ejectFreRangeLabel,2,0,Qt::AlignLeft);
    downLayGeneral->addWidget(ejectFreRangeLeft,2,1,Qt::AlignLeft);
    downLayGeneral->addWidget(waveLabel2,2,2,Qt::AlignCenter);
    downLayGeneral->addWidget(ejectFreRangeRight,2,3,Qt::AlignLeft);
    downLayGeneral->addWidget(waveLabelEmpty,2,4,Qt::AlignLeft);

    mainLay->addLayout(upLay);
    upLay->setAlignment(Qt::AlignTop);
    downLayGeneral->setAlignment(Qt::AlignTop);
    mainLay->addLayout(downLayGeneral);
#endif
}

/* 创建吹气次数主页面 */
void ejectTimes::createMainPage()
{
    if (typeMachine == 1) {
        createMainPageGeneral();
        return;
    }
    m_mainPage = new QWidget;
    m_mainPage->setFixedSize(width(), height());

    /* 定义布局管理器 */
    QVBoxLayout *mainLay = new QVBoxLayout(m_mainPage);
    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QGridLayout *downMidLay = new QGridLayout;

    /* 实例化主页面上半部分的控件 */
    m_axis = new MyAxis();
    m_axis->setDrawType(TYPE_EJECT_TIMES);
    m_axis->setFixedSize(width()-120, height()-150);
    QGroupBox *rightGroup = new QGroupBox(myLan.chute);
    rightGroup->setFont(m_config->getFont());
    QVBoxLayout *upRightLay = new QVBoxLayout(rightGroup);
    QSize size(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_modeBtn = new myPushButton("", QIcon());
    m_modeBtn->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_plusBtn = new myPushButton(myIcon.Action_Plus, size);
    m_lcdNum = new QLCDNumber(2);
    m_lcdNum->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_minusBtn = new myPushButton(myIcon.Action_Minus, size);

    upRightLay->addWidget(m_modeBtn, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_plusBtn, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_lcdNum, 0, Qt::AlignHCenter);
    upRightLay->addWidget(m_minusBtn, 0, Qt::AlignHCenter);

    /* 实例化主页面下半部分的控件 */
    m_frontLabel = new myLabel(myLan.front_view);
    m_frontLabel->setFixedSize(80, BTN_HEIGHT);
    m_frontLabel->setAlignment(Qt::AlignCenter);
    m_rearLabel = new myLabel(myLan.rear_view);
    m_rearLabel->setFixedSize(80, BTN_HEIGHT);
    m_rearLabel->setAlignment(Qt::AlignCenter);
    m_frontTimes = new QLCDNumber(5);
    m_frontTimes->setFixedSize(100, BTN_HEIGHT);
    m_frontTimes->setPalette(QPalette(Qt::red));
    m_rearTimes = new QLCDNumber(5);
    m_rearTimes->setFixedSize(100, BTN_HEIGHT);
    m_rearTimes->setPalette(QPalette(Qt::green));
    downMidLay->addWidget(m_frontLabel, 0, 0);
    downMidLay->addWidget(m_rearLabel, 1, 0);
    downMidLay->addWidget(m_frontTimes, 0, 1);
    downMidLay->addWidget(m_rearTimes, 1, 1);

    m_setBtn = new myPushButton(myLan.eject_set, QIcon());
    m_setBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    upLay->addWidget(m_axis);
    upLay->addWidget(rightGroup);
    downLay->addLayout(downMidLay);
    downLay->addWidget(m_setBtn);
    downLay->setContentsMargins(0, 10, 0, 10);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);
}

/* 创建吹气设置页面 */
void ejectTimes::createSetPage()
{
    m_setPage = new QWidget;
    m_setPage->setFixedSize(width(), height());

    /* 定义布局管理器 */
    QVBoxLayout *mainLay = new QVBoxLayout(m_setPage);
    QGroupBox *group = new QGroupBox;
    group->setFixedSize(width()-20, height()-50);
    QHBoxLayout *groupLay = new QHBoxLayout(group);
    QVBoxLayout *leftLay = new QVBoxLayout;
    QVBoxLayout *rightLay = new QVBoxLayout;

    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        m_limitLabel[i] = new myLabel;
        m_limitLabel[i]->setFixedSize(400, BTN_HEIGHT);
        m_limitLabel[i]->setAlignment(Qt::AlignCenter);
        leftLay->addWidget(m_limitLabel[i]);
        m_limitNum[i] = new myLCDNumber;
        m_limitNum[i]->setDigitCount(3);
        m_limitNum[i]->setFixedSize(100, BTN_HEIGHT);
        m_limitNum[i]->display(struCnfp.struGroupTick[struGsh.nLevel][i].nEjectTimesLimit);
        rightLay->addWidget(m_limitNum[i]);
    }
    groupLay->addLayout(leftLay);
    groupLay->addLayout(rightLay);

    mainLay->addWidget(group);
}

/* 连接界面控件的信号与槽 */
void ejectTimes::connectSigAndSlt()
{
    connect(m_setBtn, SIGNAL(pressed()), this, SLOT(onEjectSetBtnClicked()));
    connect(m_minusBtn, SIGNAL(pressed()), this, SLOT(onMinusBtnPressed()));
    connect(m_plusBtn, SIGNAL(pressed()), this, SLOT(onPlusBtnPressed()));
    connect(g_ejectTimesThread, SIGNAL(readDataFinished()), this, SLOT(onRcvDataFinishedSlt()));
    connect(m_modeBtn, SIGNAL(pressed()), this, SLOT(onModeBtnPressed()));
    connect(g_ejectTimesThread, SIGNAL(startWipeSig()), this, SLOT(onStartWipeSlt()));

    m_sigMapper = new QSignalMapper(this);
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        m_sigMapper->setMapping(m_limitNum[i], i);
        connect(m_limitNum[i], SIGNAL(pressed()), m_sigMapper, SLOT(map()));
    }
    connect(m_sigMapper, SIGNAL(mapped(int)), this, SLOT(onLcdNumsPressed(int)));

    /* 新增通用机型槽函数 */
    if (typeMachine == 1) {
        connect(ejectFreStatBtn, SIGNAL(pressed()), this, SLOT(onEjectFreStatBtnPressed()));
        connect(feedPercentRangeLeft, SIGNAL(pressed()), this, SLOT(onFeedPercentRangeLeftPressed()));
        connect(feedPercentRangeRight, SIGNAL(pressed()), this, SLOT(onFeedPercentRangeRightPressed()));
        connect(ejectFreRangeLeft, SIGNAL(pressed()), this, SLOT(onEjectFreRangeLeftPressed()));
        connect(ejectFreRangeRight, SIGNAL(pressed()), this, SLOT(onEjectFreRangeRightPressed()));
    }
}

/*智能供料使能按钮槽*/
void ejectTimes::onEjectFreStatBtnPressed(void)
{
    if (!struCnfp.isFeederAIEnable)  {
        ejectFreStatBtn->setIcon(myIcon.Action_Apply);
        struCnfp.isFeederAIEnable = 1;
        g_ejectTimesThread->start();
    } else {
        struCnfp.isFeederAIEnable = 0;
        ejectFreStatBtn->setIcon(QIcon());
        g_ejectTimesThread->stopStatRun();
        m_nCurrentEjectTimes = 0;
        g_ejectTimesThread->wait();
    }
    refreshDisplay();
}

/*智能供料供料量调节槽*/
void ejectTimes::onFeedPercentRangeLeftPressed(void)
{
    int tmp  = 0;
    int ret  = 0;

    myInputPanel inputDlg(intType,0,100,struCnfp.nFeederCapMin);
    ret = inputDlg.exec();
    if (ret == QDialog::Accepted){
        if ((tmp = inputDlg.getValue()) <= struCnfp.nFeederCapMax) {
            struCnfp.nFeederCapMin = tmp;
            feedPercentRangeLeft->display(tmp);
        }
    }
}

/*智能供料供料量调节槽*/
void ejectTimes::onFeedPercentRangeRightPressed(void)
{
    int tmp  = 0;
    int ret  = 0;

    myInputPanel inputDlg(intType,0,100,struCnfp.nFeederCapMax);
    ret = inputDlg.exec();
    if (ret == QDialog::Accepted){
        if ((tmp = inputDlg.getValue()) >= struCnfp.nFeederCapMin) {
            struCnfp.nFeederCapMax = tmp;
            feedPercentRangeRight->display(tmp);
        }
    }
}

/*智能供料吹气次数量调节槽*/
void ejectTimes::onEjectFreRangeLeftPressed(void)
{
    int tmp  = 0;
    int ret  = 0;

    myInputPanel inputDlg(intType,0,100,struCnfp.nEjectTimesMin);
    ret = inputDlg.exec();
    if (ret == QDialog::Accepted){
        if ((tmp = inputDlg.getValue()) <= struCnfp.nEjectTimesMax) {
            struCnfp.nEjectTimesMin = tmp;
            ejectFreRangeLeft->display(tmp);
        }
    }
}

/*智能供料吹气次数量调节槽*/
void ejectTimes::onEjectFreRangeRightPressed(void)
{
    int tmp  = 0;
    int ret  = 0;

    myInputPanel inputDlg(intType,0,100,struCnfp.nEjectTimesMax);
    ret = inputDlg.exec();
    if (ret == QDialog::Accepted){
        if ((tmp = inputDlg.getValue()) >= struCnfp.nEjectTimesMin) {
            struCnfp.nEjectTimesMax = tmp;
            ejectFreRangeRight->display(tmp);
        }
    }
}

/* 刷新当前页面的显示 */
void ejectTimes::refreshDisplay()
{
    m_nOkTimes = 0;
    m_stackedWidget->setCurrentIndex(SM_EJECT_MAIN_PAGE);
    clearShareEjectTimes();
    refreshChuteDisplay();
    refreshAxisDisplay();
    clearEjectTimes();
    g_ejectTimesThread->startEmitSignal();
}

/* 刷新滑槽部分页面显示 */
void ejectTimes::refreshChuteDisplay()
{
    bool bIsShow = false;
    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {
        if(struCnfg.nAssistCamEn[i] == 1) {
            bIsShow = true;
            break;
        }
    }
    if(bIsShow) {
        m_modeBtn->show();
        if (struGsh.bIsAssist) {
            m_modeBtn->setText(myLan.config_assist);
        } else {
            m_modeBtn->setText(myLan.config_major);
        }
    } else {
        m_modeBtn->hide();
    }

    m_lcdNum->display(struGsh.nUnit/2+1);
}

/* 刷新坐标轴显示 */
void ejectTimes::refreshAxisDisplay()
{
    int nChute = struGsh.nUnit/2+1;
    m_axis->setMinValueX(struCnfg.nEjectorsPerChute*(nChute-1));
    m_axis->setMaxValueX(struCnfg.nEjectorsPerChute*nChute);
    m_axis->setTickAttr();
    m_axis->update();
}

/* 停止页面刷新 */
void ejectTimes::stopRefresh()
{
    g_ejectTimesThread->stopEmitSignal();
}

/* 清空全局吹气次数参数 */
void ejectTimes::clearShareEjectTimes()
{
    memset(struGsh.pEjectTimes, 0, sizeof(struGsh.pEjectTimes));
    m_nCurrentEjectTimes = 0;
}

/* 响应滑槽或采样时间变化事件 */
void ejectTimes::onChuteOrTimeChanged()
{
    m_nOkTimes = 0;
    clearShareEjectTimes();
    refreshAxisDisplay();
    clearEjectTimes();
}

/* 清空前后视吹气次数显示 */
void ejectTimes::clearEjectTimes()
{
    m_frontTimes->display(0);
    m_rearTimes->display(0);

    if (typeMachine == 1) {
        m_currentEjectFreq->display(m_nCurrentEjectTimes);
    }
}

/* 供料器自动调节振动量 */
void ejectTimes::feederAutoAdjust()
{
    int tmp = struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2];

    if (struCnfp.isFeederAIEnable != 1) {
        return;
    }

    /*第一步： 根据吹气次数，调节供料器的振动量*/
    if (m_nCurrentEjectTimes < struCnfp.nEjectTimesMin) {
        struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2] += 2;
    } else if (m_nCurrentEjectTimes > struCnfp.nEjectTimesMax) {
        struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2] -= 2;
    }

    /*第二步：检查供料器的振动量是否在设定范围内*/
    if (struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2] < struCnfp.nFeederCapMin){
        struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2] = struCnfp.nFeederCapMin;
    } else if (struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2] > struCnfp.nFeederCapMax) {
        struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2] = struCnfp.nFeederCapMax;
    }

    /*重置供料器设置*/
    if (tmp != struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2]) {
        myFlow.resetFeeder(0);
    }
    myLog->info(LOG_FEED,"Feeder%d:%d",struGsh.nUnitFeederAI/2,struCnfp.struGroupCtrl[0].nFeeder[struGsh.nUnitFeederAI/2]);
}

/* 刷新前后视吹气次数 */
void ejectTimes::refreshEjectTimes()
{
    int frontTotal = 0;
    int rearTotal = 0;

    for (int i = 0; i < MAX_EJECTOR_SIXTY; i++) {
		if (struGsh.bIsAssist) {
        	frontTotal += struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY*2];
        	rearTotal += struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY*3];
		} else {
        	frontTotal += struGsh.pEjectTimes[i];
        	rearTotal += struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY];
		}
    }

    m_frontTimes->display(frontTotal);
    m_rearTimes->display(rearTotal);

    /* 将获取的每秒吹气次数增加到全局参数中 */
    if (typeMachine == 1) {
        if (struGsh.nUnit == struGsh.nUnitFeederAI || struGsh.nUnit == struGsh.nUnitFeederAI + 1) {
            m_currentEjectFreq->display(m_nCurrentEjectTimes);
        }
        feederAutoAdjust();
    }
}

/* 获取当前切换页的索引号 */
int ejectTimes::getCurPageIndex()
{
    return m_stackedWidget->currentIndex();
}

/* 设置当前切换页的索引号 */
void ejectTimes::setCurPageIndex(int index)
{
    m_stackedWidget->setCurrentIndex(index);
}

/* 响应吹气设置按钮按下事件 */
void ejectTimes::onEjectSetBtnClicked()
{
    m_limitLabel[0]->setText(struGsh.first+myLan.eject_times_limit+"(Hz)");
    m_limitLabel[1]->setText(struGsh.second+myLan.eject_times_limit+"(Hz)");
    m_limitLabel[2]->setText(struGsh.third+myLan.eject_times_limit+"(Hz)");
    m_stackedWidget->setCurrentIndex(SM_EJECT_SET_PAGE);
    stopRefresh();
}

/* 响应增加按钮按下事件 */
void ejectTimes::onPlusBtnPressed()
{
    if(struGsh.nUnit < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal-2) {
        struGsh.nUnit += 2;
        onChuteOrTimeChanged();
    }
    m_lcdNum->display(struGsh.nUnit/2+1);
}

/* 响应减少按钮按下事件 */
void ejectTimes::onMinusBtnPressed()
{
    if(struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
        onChuteOrTimeChanged();
    }
    m_lcdNum->display(struGsh.nUnit/2+1);
}

/* 响应读吹气次数完成事件 */
void ejectTimes::onRcvDataFinishedSlt()
{
    m_nOkTimes++;

    /* 将获取的每秒吹气次数增加到全局参数中 */
    if (typeMachine == 1) {
        if (struCnfp.isFeederAIEnable == 1 && struCnfp.autoFeedEnablePageFlag == 1) {
            for(int i=0; i<struCnfg.nEjectorsPerChute; i++) {
                struGsh.pEjectTimes[i] += pEjectTimesPerSec[i];
                struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY] += pEjectTimesPerSec[i+MAX_EJECTOR_SIXTY];
                m_nCurrentEjectTimes = pEjectTimesPerSec[MAX_EJECTOR_SIXTY*4];
            }
        } else {
            clearShareEjectTimes();
        }
    } else if (typeMachine == 0){
        /* RS机型 */
        if (!struGsh.bIsAssist) {    // 主配
            for (int i = 0; i < struCnfg.nEjectorsPerChute; i++) {
                struGsh.pEjectTimes[i] += pEjectTimesPerSec[i*4];
                struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY] += pEjectTimesPerSec[i*4+1];
            }
        } else {                    // 辅配
            for (int i = 0; i < struCnfg.nEjectorsPerChute; i++) {
                struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY*2] += pEjectTimesPerSec[i*4+2];
                struGsh.pEjectTimes[i+MAX_EJECTOR_SIXTY*3] += pEjectTimesPerSec[i*4+3];
            }
        }
    }

    /* 若达到采集次数则更新显示 */
    if (m_nOkTimes >= 1) {
        m_nOkTimes = 0;
        refreshEjectTimes();
        if (typeMachine == 1) {
            if (struGsh.nUnit == struGsh.nUnitFeederAI || struGsh.nUnit == struGsh.nUnitFeederAI + 1) {
                m_axis->update();
            }
        } else {
            m_axis->update();
        }
    }
}

/* 响应采样时间变化事件 */
void ejectTimes::onValueBarChangedSlt()
{
    onChuteOrTimeChanged();
}

/* 响应相机模式按钮按下事件 */
void ejectTimes::onModeBtnPressed()
{
    struGsh.bIsAssist = !struGsh.bIsAssist;
    if(!struGsh.bIsAssist) {
        m_modeBtn->setText(myLan.config_major);
    } else {
        m_modeBtn->setText(myLan.config_assist);
    }
}

/* 响应开始清灰事件 */
void ejectTimes::onStartWipeSlt()
{
    infoWidget->setLabelText(myLan.msg_wiping);
    infoWidget->delayShow();
    myFlow.startWipe();
    infoWidget->hide();
}

/* 响应喷阀次数限制变化事件 */
void ejectTimes::onLcdNumsPressed(int index)
{
    int value = m_limitNum[index]->value();
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 790, value);
    int ret = inputPanel->exec();
    if (ret == QDialog::Accepted) {
        value = inputPanel->getValue();
        m_limitNum[index]->display(value);
        struCnfp.struGroupTick[struGsh.nLevel][index].nEjectTimesLimit = value;
    }
}

/* 隐藏吹气设置按钮*/
void ejectTimes::hideEjectSet()
{
    m_setBtn->hide();
}
