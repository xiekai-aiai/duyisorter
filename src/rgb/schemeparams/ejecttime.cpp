#include "ejecttime.h"

/* 构造喷阀时间设置页面 */
PageEjectTime::PageEjectTime(QWidget *parent)
    : QWidget(parent)
{
    initLocalParams();

/* page init */
    createPage();

/* page layout */
    layoutPage();

/* page update */
    updatePage();
}

/* 析构喷阀时间设置页面 */
PageEjectTime::~PageEjectTime()
{

}

/*!
 * \brief 初始化局部参数
 */
void PageEjectTime::initLocalParams()
{
    for (int i = 0; i < MAX_LEVEL*MAX_GROUP_TICK; i++) {
        for (int j = 0; j < 6; j++) {
            m_nEjectTime[i][j] = 0;
            m_nEjectTimeCent[i][j] = 0;
        }
        for (int j = 0; j < 3; j++) {
            m_nAiEjectTime[i][j] = 0;
        }
    }
}

/* 应用当前喷阀时间页面设置 */
void PageEjectTime::setEjectTimeParams()
{
    /* write params to memory */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int step = struCnfg.struLevelInfo[i].nTickGroupTotal;
        for (int j = 0; j < step; j++) {
            struCnfp.struGroupTick[i][j].nEjectDuration = m_nEjectTime[i*step+j][0];

            struCnfp.struGroupTick[i][j].nEjectDurationCent = m_nEjectTimeCent[i*step+j][0];

            struCnfp.struGroupTick[i][j].nEjectDelay = m_nEjectTime[i*step+j][1];
            struCnfp.struGroupTick[i][j].nEjectDelayCent = m_nEjectTimeCent[i*step+j][1];

            struCnfp.struGroupTick[i][j].nAssistEjectTime = m_nEjectTime[i*step+j][3];
            struCnfp.struGroupTick[i][j].nAssistDelayTime = m_nEjectTime[i*step+j][4];

            struCnfp.struGroupTick[i][j].aiEjectorDelay = m_nAiEjectTime[i*step+j][0];
            struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay = m_nAiEjectTime[i*step+j][1];
            struCnfp.struGroupTick[i][j].aiEjectorBlowTime = m_nAiEjectTime[i*step+j][2];
        }
    }

    /* 重置延迟时间 */
    int nDelay = myFlow.resetEjectTime();

    if (nDelay <= 0) {
        if (nDelay == 0) {
            infoWidget->setLabelText(myLan.msg_delay_get_zero);
        } else {
            infoWidget->setLabelText(myLan.infra + " " +  myLan.msg_delay_get_zero);
        }
        infoWidget->delayShow();
        myFlow.sleep(2);
        infoWidget->hide();
    }
}

/* 重置喷阀时间页面设置 */
void PageEjectTime::resetEjectTimeParams()
{
    /* reset eject time params */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int step = struCnfg.struLevelInfo[i].nTickGroupTotal;
        for (int j = 0; j < step; j++) {
            m_nEjectTime[i*step+j][0]  = struCnfp.struGroupTick[i][j].nEjectDuration;
            m_nEjectTimeCent[i*step+j][0] = struCnfp.struGroupTick[i][j].nEjectDurationCent;

            m_nEjectTime[i*step+j][1] = struCnfp.struGroupTick[i][j].nEjectDelay;
            m_nEjectTimeCent[i*step+j][1] = struCnfp.struGroupTick[i][j].nEjectDelayCent;
            m_nEjectTime[i*step+j][2] = struCnfp.struGroupTick[i][j].nEjectDelayBias;
            m_nEjectTimeCent[i*step+j][2] = struCnfp.struGroupTick[i][j].nEjectDelayBiasCent;

            m_nEjectTime[i*step+j][3] = struCnfp.struGroupTick[i][j].nAssistEjectTime;
            m_nEjectTime[i*step+j][4] = struCnfp.struGroupTick[i][j].nAssistDelayTime;

            m_nAiEjectTime[i*step+j][0] = struCnfp.struGroupTick[i][j].aiEjectorDelay ;
            m_nAiEjectTime[i*step+j][1] = struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay;
            m_nAiEjectTime[i*step+j][2] = struCnfp.struGroupTick[i][j].aiEjectorBlowTime;
        }
    }

    /* trigger page update */
    emit pageUpdated();
}

/* 响应喷阀时间设置 */
void PageEjectTime::setEjectTimeSlt(int index)
{
    inputType type;
    double nTmp, nMin, nMax;
    int tabIndex = tabBar->currentIndex();

    nTmp = m_nEjectTime[tabIndex][index]/10.0+m_nEjectTimeCent[tabIndex][index]/100.0;
    switch (index) {
    case Index_Eject_Duration:
        nMin = 0;
        nMax = 25;
        type = floatType;
        break;
    case Index_Eject_Delay:
        nMin = 0;
        nMax = 120;
        type = floatType;
        break;
    case Index_Eject_Bias:
        nMin = 0;
        nMax = 50;
        type = floatType;
        break;
    case Index_Assist_Duration:
        nMin = 0;
        nMax = 10;
        type = floatType;
        break;
    case Index_Assist_Delay:
        nMin = 1;
        nMax = 13;
        type = floatType;
        break;
    case Index_Infra_Delay:
        nMin = 0;
        nMax = 120;
        type = floatType;
        break;
    }

    /* trigger input dialog */
    myInputPanel inputDlg(type, nMin, nMax, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nTmp = inputDlg.getValue();

        /* params update */
        m_nEjectTime[tabIndex][index] = nTmp*10+0.05;
        m_nEjectTimeCent[tabIndex][index] = ((int)(nTmp*100+0.5))%10;

        /* page update */
        nTmp = m_nEjectTime[tabIndex][index]/10.0+m_nEjectTimeCent[tabIndex][index]/100.0;

        QString text = QString("%1").arg(nTmp);
        ejectTimeValue[index]->setText(text);

        /* write parameters */
        setEjectTimeParams();
    }
}

/* 响应喷阀开关设置 */
void PageEjectTime::setEjectOnOffSlt(bool enable)
{
    struGsh.bStatEjectFpga = (enable == true) ? 1 : 0;
    struGsh.bAlarmCloseEject = false;
}

/* 响应tab页面切换 */
void PageEjectTime::changeCurrentSlt(int index)
{
    if(index < 0){
        index = 0;
    }
    refreshEjectSetDisplay();
}

/* 初始化喷阀时间设置页面 */
void PageEjectTime::createPage()
{
    /* tab widget to hold index */
    tabBar = new MyTabBar();

    /* eject time group */
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    for (int i = 0; i < 6; i++) {//第六个是I机型红外延迟时间设置
        ejectTimeLabel.append(new myLabel(""));
        ejectTimeValue.append(new myLineEdit(""));
        ejectTimeValue[i]->setSizePolicy(sizePolicy);
        ejectTimeValue[i]->setMinimumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    }

    ejectTimeLabel[2]->hide();
    ejectTimeValue[2]->hide();
    //红外延迟默认隐藏
    ejectTimeLabel[5]->hide();
    ejectTimeValue[5]->hide();

    aiEjectorDelayLbl = new myLabel("Ai "+myLan.fixed_delay + "(ms)");
    aiEjectorDelayEdit = new myLineEdit("");
    aiEjectorDelayEdit->setSizePolicy(sizePolicy);
    aiEjectorDelayEdit->setMinimumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    aiEjectorDynamicDelayLbl = new myLabel("Ai "+myLan.dynamic_delay+ "(ms)");
    aiEjectorDynamicDelayEdit = new myLineEdit("");
    aiEjectorDynamicDelayEdit->setSizePolicy(sizePolicy);
    aiEjectorDynamicDelayEdit->setMinimumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    aiEjectorBlowTimeLbl = new myLabel("Ai "+myLan.eject_time+ "(ms)");
    aiEjectorBlowTimeEdit = new myLineEdit("");
    aiEjectorBlowTimeEdit->setSizePolicy(sizePolicy);
    aiEjectorBlowTimeEdit->setMinimumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    ejectorOnCheckBox = new MyCheckBox(myLan.eject_on, this);

    /* eject time text */
    displayPage();

    /* signals to slots */
    sigMapper = new QSignalMapper;
    for (int i = 0; i < ejectTimeValue.size(); i++) {
        sigMapper->setMapping(ejectTimeValue[i], i);
        connect(ejectTimeValue[i], SIGNAL(pressed()), sigMapper, SLOT(map()));
    }
    connect(sigMapper,          SIGNAL(mapped(int)),         this, SLOT(setEjectTimeSlt(int)));
    connect(tabBar,             SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(ejectorOnCheckBox,  SIGNAL(toggled(bool)),       this, SLOT(setEjectOnOffSlt(bool)));
    connect(this,               SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(this,               SIGNAL(pageUpdated()),       this, SLOT(updatePage()));
    connect(aiEjectorDelayEdit, SIGNAL(pressed()),           this, SLOT(setEjectorDelayEditSlt()));
    connect(aiEjectorDynamicDelayEdit, SIGNAL(pressed()),    this, SLOT(setEjectorDynamicDelayEditSlt()));
    connect(aiEjectorBlowTimeEdit, SIGNAL(pressed()),        this, SLOT(setEjectorBlowTimeEditSlt()));
}

void PageEjectTime::setEjectorDelayEditSlt(){
    inputType type = intType;
    int nTmp, nMin, nMax;
    int tabIndex = tabBar->currentIndex();
    nTmp = aiEjectorDelayEdit->text().toInt();
    nMin = 0;
    nMax = 1023;
    myInputPanel inputDlg(type, nMin, nMax, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nTmp = inputDlg.getValue();
        m_nAiEjectTime[tabIndex][0] = nTmp;
        aiEjectorDelayEdit->setText(QString("%1").arg(nTmp));
        /* write parameters */
        setEjectTimeParams();
    }

}
void PageEjectTime::setEjectorDynamicDelayEditSlt(){
    inputType type = intType;
    int nTmp, nMin, nMax;
    int tabIndex = tabBar->currentIndex();
    nTmp = aiEjectorDynamicDelayEdit->text().toInt();
    nMin = 0;
    nMax = 1023;
    myInputPanel inputDlg(type, nMin, nMax, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nTmp = inputDlg.getValue();
        m_nAiEjectTime[tabIndex][1] = nTmp;
        aiEjectorDynamicDelayEdit->setText(QString("%1").arg(nTmp));
        /* write parameters */
        setEjectTimeParams();
    }

}
void PageEjectTime::setEjectorBlowTimeEditSlt(){
    inputType type = intType;
    int nTmp, nMin, nMax;
    int tabIndex = tabBar->currentIndex();
    nTmp = aiEjectorBlowTimeEdit->text().toInt();
    nMin = 0;
    nMax = 255;
    myInputPanel inputDlg(type, nMin, nMax, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nTmp = inputDlg.getValue();
        m_nAiEjectTime[tabIndex][2] = nTmp;
        aiEjectorBlowTimeEdit->setText(QString("%1").arg(nTmp));
        /* write parameters */
        setEjectTimeParams();
    }

}

/* 显示当前喷阀时间设置项名称 */
void PageEjectTime::displayPage()
{
    /* text */
    QStringList list;
    list = QStringList() << myLan.eject_duration
                                     << myLan.eject_delay
                                     << myLan.eject_bias
                                     << myLan.config_assist+myLan.eject_duration
                                     << myLan.config_assist+myLan.eject_delay
                                     << myLan.infra+myLan.eject_delay;;

    for (int i = 0; i < ejectTimeLabel.size(); i++) {
        ejectTimeLabel[i]->setText(list[i]);
    }
}

/* 刷新吹气、延迟时间 */
void PageEjectTime::refreshEjectSetDisplay()
{
    int index = tabBar->currentIndex();

    for (int i = 0; i < 3; i++) {
        QString text = QString("%1").arg(m_nEjectTime[index][i]/10.0+m_nEjectTimeCent[index][i]/100.0);
        ejectTimeValue[i]->setText(text);
    }

    if (checkContainAssist(index)) {
        showAssistEjectSet();
        for (int i = 3; i < 5; i++) {
            QString text = QString("%1").arg(m_nEjectTime[index][i]/10.0+m_nEjectTimeCent[index][i]/100.0);
            ejectTimeValue[i]->setText(text);
        }
    } else {
        hideAssistEjectSet();
    }

    QString text = QString("%1").arg(m_nEjectTime[index][5]/10.0+m_nEjectTimeCent[index][5]/100.0);
    ejectTimeValue[5]->setText(text);

    /* set ejector check box state */
    if(struGsh.bStatEjectFpga) {
        ejectorOnCheckBox->setChecked(true);
    } else {
        ejectorOnCheckBox->setChecked(false);
    }

    text = QString("%1").arg(m_nAiEjectTime[index][0]);
    aiEjectorDelayEdit->setText(text);
    text = QString("%1").arg(m_nAiEjectTime[index][1]);
    aiEjectorDynamicDelayEdit->setText(text);
    text = QString("%1").arg(m_nAiEjectTime[index][2]);
    aiEjectorBlowTimeEdit->setText(text);
}

/*!
 * \brief 检查是否包含辅配相机
 * \param nTick 剔除组
 * \return true:包含辅配 false:不包含辅配
 */
bool PageEjectTime::checkContainAssist(int nTick)
{
    return true;
}

/*!
 * \brief 显示辅配吹气设置
 */
void PageEjectTime::showAssistEjectSet()
{
    ejectTimeLabel[3]->show();
    ejectTimeValue[3]->show();
    ejectTimeLabel[4]->show();
    ejectTimeValue[4]->show();
}

/*!
 * \brief 隐藏辅配吹气设置
 */
void PageEjectTime::hideAssistEjectSet()
{
    ejectTimeLabel[3]->hide();
    ejectTimeValue[3]->hide();
    ejectTimeLabel[4]->hide();
    ejectTimeValue[4]->hide();
}


/* 更新喷阀时间设置页面 */
void PageEjectTime::updatePage()
{
    /* update page */
    updateTabBar(this->tabBar, false, false);
    refreshEjectSetDisplay();

    if(struCnfg.aiEnable == 1){
            aiEjectorDelayLbl->show();
            aiEjectorDelayEdit->show();
            aiEjectorDynamicDelayLbl->show();
            aiEjectorDynamicDelayEdit->show();
            aiEjectorBlowTimeLbl->show();
            aiEjectorBlowTimeEdit->show();
            ejectTimeLabel[0]->hide();
            ejectTimeValue[0]->hide();
            ejectTimeLabel[1]->hide();
            ejectTimeValue[1]->hide();
        }else{
            aiEjectorDelayLbl->hide();
            aiEjectorDelayEdit->hide();
            aiEjectorDynamicDelayLbl->hide();
            aiEjectorDynamicDelayEdit->hide();
            aiEjectorBlowTimeLbl->hide();
            aiEjectorBlowTimeEdit->hide();
            ejectTimeLabel[0]->show();
            ejectTimeValue[0]->show();
            ejectTimeLabel[1]->show();
            ejectTimeValue[1]->show();
        }
}

/* 布局喷阀时间设置页面 */
void PageEjectTime::layoutPage()
{
    /* eject time layout */
    ejectTimeGroup = new myGroupBox(myLan.eject_time);
    gridLayout = new QGridLayout(ejectTimeGroup);
    for (int i = 0; i < ejectTimeLabel.size(); i++) {
        gridLayout->addWidget(ejectTimeLabel[i], i, 0, Qt::AlignLeft);
        gridLayout->addWidget(ejectTimeValue[i], i, 1, Qt::AlignLeft);
    }
    gridLayout->addWidget(aiEjectorDelayLbl, 6, 0, Qt::AlignLeft);
    gridLayout->addWidget(aiEjectorDelayEdit, 6, 1, Qt::AlignLeft);
    gridLayout->addWidget(aiEjectorDynamicDelayLbl, 7, 0, Qt::AlignLeft);
    gridLayout->addWidget(aiEjectorDynamicDelayEdit, 7, 1, Qt::AlignLeft);
    gridLayout->addWidget(aiEjectorBlowTimeLbl, 8, 0, Qt::AlignLeft);
    gridLayout->addWidget(aiEjectorBlowTimeEdit, 8, 1, Qt::AlignLeft);
    gridLayout->addWidget(ejectorOnCheckBox, 9, 0, Qt::AlignLeft);

    /* main layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabBar);
    mainLayout->addWidget(ejectTimeGroup);
    setLayout(mainLayout);
}
