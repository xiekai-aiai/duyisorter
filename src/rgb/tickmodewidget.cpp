#include "tickmodewidget.h"

tickModeWidget::tickModeWidget(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT);

    creatPage();

    layoutPage();

    updateTabBar(tickModeTabBar, false, false);
}

/*! 初始化局部参数*/
void tickModeWidget::initLocalParams()
{
    currentChan = 0;

    m_nTickMode = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nTickMode;

    m_Params[EJECT_LENTH_MAX] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectLenthMax;
    m_Params[EJECT_LENTH_DIF] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectLenthDif;
    m_Params[EJECT_LENTH_P1] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectProportion1;
    m_Params[EJECT_LENTH_P2] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectProportion2;
    m_Params[EJECT_LENTH_THRESHOLD] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectThresHold;
    m_Params[REVERSE_ROUNDS] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nRounds;
}

/*! 保存并下发剔除模式参数*/
void tickModeWidget::saveAndSendTickParams()
{
    int nUnitAddr = 0;
    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nTickMode = m_nTickMode;

    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectLenthMax = m_Params[EJECT_LENTH_MAX];
    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectLenthDif = m_Params[EJECT_LENTH_DIF];
    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectProportion1 = m_Params[EJECT_LENTH_P1];
    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectProportion2 = m_Params[EJECT_LENTH_P2];
    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectThresHold = m_Params[EJECT_LENTH_THRESHOLD];
    struCnfp.struGroupTick[struGsh.nLevel][currentChan].nRounds = m_Params[REVERSE_ROUNDS];

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                nUnitAddr = myFlow.getTickGroupAddr(i, j, k);
                MySerial.com1Write(CMD_INT_TICK_MODE,INT, i, nUnitAddr,
                                   0, 0, 0, 0, (struCnfp.struGroupTick[i][j].nTickMode == 1)?1:0, 3);
                MySerial.com1Write(CMD_INT_MASTER_SLAVE_RELATION,INT,i,nUnitAddr,
                                   0, 0, 0, 0, (struCnfp.struGroupTick[i][j].nTickMode == 2)?1:0, 3);

                MySerial.com1Write(CMD_UNIT_TICK_MODE, UNIT, i, nUnitAddr,
                                    (struCnfp.struGroupTick[i][j].nTickMode == 1)?1:0,
                                    struCnfp.struGroupTick[i][j].nEjectLenthDif | (struCnfp.struGroupTick[i][j].nEjectLenthMax << 4),
                                    struCnfp.struGroupTick[i][j].nEjectProportion1,
                                    struCnfp.struGroupTick[i][j].nEjectProportion2,
                                    struCnfp.struGroupTick[i][j].nEjectThresHold,
                                    3);

                MySerial.com1Write(CMD_UNIT_REVERSE, UNIT, i, nUnitAddr,
                                    (struCnfp.struGroupTick[i][j].nTickMode == 2)?1:0,
                                   0,
                                   0,
                                   0,
                                   struCnfp.struGroupTick[i][j].nRounds,
                                   3);
            }
        }
    }
}

void tickModeWidget::creatPage()
{
    m_tickModePage = new QWidget;

    tickModeTabBar = new MyTabBar(m_tickModePage);

    /* 实例化互斥框 */
    m_mutex = new myMutex(Qt::Horizontal);
    m_mutex->setNum(3);
    QStringList strList;
    strList << myLan.common << myLan.accurate << myLan.reverse;
    m_mutex->setLabelText(strList);
    m_mutex->setFixedHeight(BTN_HEIGHT*2);

    QStringList str;
    str << myLan.eject_width_max << myLan.eject_width_dif
        << myLan.proportion + "1" << myLan.proportion + "2" << myLan.threshold
        << myLan.rounds;

    for(int i=0; i<EJECT_PARAMS_NUM;i++) {
        m_paramsLel[i] = new myLabel(str.at(i));
        m_paramsLel[i]->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);
    }

    tickModeSigMapper = new QSignalMapper();
    for(int i=0; i<EJECT_PARAMS_NUM;i++) {
        m_paramsLet[i] = new myLineEdit();
        m_paramsLet[i]->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
        tickModeSigMapper->setMapping(m_paramsLet[i],i);
        connect(m_paramsLet[i],SIGNAL(pressed()),tickModeSigMapper,SLOT(map()));
    }

    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back, true, true);
    m_backBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    connect(tickModeSigMapper, SIGNAL(mapped(int)), this, SLOT(OnTickParamsValueChange(int)));
    connect(tickModeTabBar,  SIGNAL(currentChanged(int)), this, SLOT(onTickModeTabChanged(int)));
    connect(m_mutex, SIGNAL(indexChanged(int)), this, SLOT(onMutexIndexChangedSlt(int)));
    connect(m_backBtn,SIGNAL(pressed()),this,SLOT(onBackBtnPressedSlt()));
}

void tickModeWidget::layoutPage()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(m_tickModePage);
    QHBoxLayout *upHLayout = new QHBoxLayout();
    group1 = new QGroupBox();
    group2 = new QGroupBox();
    QHBoxLayout *groupLay1 = new QHBoxLayout(group1);
    QHBoxLayout *groupLay2 = new QHBoxLayout(group2);
    QHBoxLayout *downHLayout = new QHBoxLayout();

    upHLayout->addWidget(m_mutex);
    groupLay1->addWidget(m_paramsLel[EJECT_LENTH_MAX]);
    groupLay1->addWidget(m_paramsLet[EJECT_LENTH_MAX]);
    groupLay1->addWidget(m_paramsLel[EJECT_LENTH_DIF]);
    groupLay1->addWidget(m_paramsLet[EJECT_LENTH_DIF]);
    groupLay1->addWidget(m_paramsLel[REVERSE_ROUNDS]);
    groupLay1->addWidget(m_paramsLet[REVERSE_ROUNDS]);
    groupLay2->addWidget(m_paramsLel[EJECT_LENTH_P1]);
    groupLay2->addWidget(m_paramsLet[EJECT_LENTH_P1]);
    groupLay2->addWidget(m_paramsLel[EJECT_LENTH_P2]);
    groupLay2->addWidget(m_paramsLet[EJECT_LENTH_P2]);
    groupLay2->addWidget(m_paramsLel[EJECT_LENTH_THRESHOLD]);
    groupLay2->addWidget(m_paramsLet[EJECT_LENTH_THRESHOLD]);

    downHLayout->addStretch(0);
    downHLayout->addWidget(m_backBtn);

    mainLayout->addWidget(tickModeTabBar);
    mainLayout->addStretch();
    mainLayout->addLayout(upHLayout);
    mainLayout->setSpacing(100);
    mainLayout->addWidget(group1);
    mainLayout->setSpacing(100);
    mainLayout->addWidget(group2);
    mainLayout->addStretch();
    mainLayout->addLayout(downHLayout);
    setLayout(mainLayout);
}

/*! 刷新界面函数*/
void tickModeWidget::updatePage()
{
    initLocalParams();

    updateTabBar(tickModeTabBar, false, false);

    updateParamsDisplay();

}

void tickModeWidget::updateParamsDisplay()
{
    int index = tickModeTabBar->currentIndex();
    switch(struCnfp.struGroupTick[struGsh.nLevel][index].nTickMode) {
    case 0:
        m_mutex->setCurrentIndex(0);
        group1->hide();
        group2->hide();
        break;
    case 1:
        m_mutex->setCurrentIndex(1);
        group1->show();
        m_paramsLel[EJECT_LENTH_MAX]->show();
        m_paramsLet[EJECT_LENTH_MAX]->show();
        m_paramsLel[EJECT_LENTH_DIF]->show();
        m_paramsLet[EJECT_LENTH_DIF]->show();
        m_paramsLel[REVERSE_ROUNDS]->hide();
        m_paramsLet[REVERSE_ROUNDS]->hide();
        group2->show();
        break;
    case 2:
        m_mutex->setCurrentIndex(2);
        group1->show();
        m_paramsLel[EJECT_LENTH_MAX]->hide();
        m_paramsLet[EJECT_LENTH_MAX]->hide();
        m_paramsLel[EJECT_LENTH_DIF]->hide();
        m_paramsLet[EJECT_LENTH_DIF]->hide();
        m_paramsLel[REVERSE_ROUNDS]->show();
        m_paramsLet[REVERSE_ROUNDS]->show();
        group2->hide();
        break;
    default:
        break;
    }

    QString str1;
    str1.sprintf("%d",m_Params[EJECT_LENTH_MAX]);
    m_paramsLet[EJECT_LENTH_MAX]->setText(str1);

    QString str2;
    str2.sprintf("%d",m_Params[EJECT_LENTH_DIF]);
    m_paramsLet[EJECT_LENTH_DIF]->setText(str2);

    QString str3;
    str3.sprintf("%d",m_Params[EJECT_LENTH_P1]);
    m_paramsLet[EJECT_LENTH_P1]->setText(str3);

    QString str4;
    str4.sprintf("%d",m_Params[EJECT_LENTH_P2]);
    m_paramsLet[EJECT_LENTH_P2]->setText(str4);

    QString str5;
    str5.sprintf("%d",m_Params[EJECT_LENTH_THRESHOLD]);
    m_paramsLet[EJECT_LENTH_THRESHOLD]->setText(str5);

    QString str6;
    str6.sprintf("%d",m_Params[REVERSE_ROUNDS]);
    m_paramsLet[REVERSE_ROUNDS]->setText(str6);
}

/*! 获取各个输入框输入配置*/
void tickModeWidget::getInputType(int index)
{
    switch(index) {
    case EJECT_LENTH_MAX:
        min = 0;
        max = 15;
        type = intType;
        break;
    case EJECT_LENTH_DIF:
        min = 0;
        max = m_Params[EJECT_LENTH_MAX];
        type = intType;
        break;
    case EJECT_LENTH_P1:
        min = 0;
        max = 255;
        type = intType;
        break;
    case EJECT_LENTH_P2:
        min = 0;
        max = 255;
        type = intType;
        break;
    case EJECT_LENTH_THRESHOLD:
        min = 0;
        max = 255;
        type = intType;
        break;
    case REVERSE_ROUNDS:
        min = 0;
        max = 15;
        type = intType;
    default:
        break;
    }
}

/*! tabbar槽函数*/
void tickModeWidget::onTickModeTabChanged(int index)
{
    if (index < 0)
        return;

    struGsh.nLevel = 0;
    currentChan = index;

    m_nTickMode = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nTickMode;

    m_Params[EJECT_LENTH_MAX] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectLenthMax;
    m_Params[EJECT_LENTH_DIF] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectLenthDif;
    m_Params[EJECT_LENTH_P1] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectProportion1;
    m_Params[EJECT_LENTH_P2] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectProportion2;
    m_Params[EJECT_LENTH_THRESHOLD] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectThresHold;
    m_Params[REVERSE_ROUNDS] = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nRounds;

    updateParamsDisplay();
}

/*! 响应模式选择槽函数*/
void tickModeWidget::onMutexIndexChangedSlt(int index)
{
    switch(index) {
    case 0:
        m_nTickMode = 0;
        group1->hide();
        group2->hide();
        break;
    case 1:
        m_nTickMode = 1;
        group1->show();
        m_paramsLel[EJECT_LENTH_MAX]->show();
        m_paramsLet[EJECT_LENTH_MAX]->show();
        m_paramsLel[EJECT_LENTH_DIF]->show();
        m_paramsLet[EJECT_LENTH_DIF]->show();
        m_paramsLel[REVERSE_ROUNDS]->hide();
        m_paramsLet[REVERSE_ROUNDS]->hide();
        group2->show();
        break;
    case 2:
        if(struCnfp.struGroupTick[struGsh.nLevel][currentChan].nReverseSort) {
            infoWidget->setLabelText("请将此剔除组的正反选模式设置为正选!");
            infoWidget->showSecs();
            m_mutex->setCurrentIndex(m_nTickMode);
            return;
        }
        m_nTickMode = 2;
        group1->show();
        m_paramsLel[EJECT_LENTH_MAX]->hide();
        m_paramsLet[EJECT_LENTH_MAX]->hide();
        m_paramsLel[EJECT_LENTH_DIF]->hide();
        m_paramsLet[EJECT_LENTH_DIF]->hide();
        m_paramsLel[REVERSE_ROUNDS]->show();
        m_paramsLet[REVERSE_ROUNDS]->show();
        group2->hide();
        break;
    default:
        break;
    }
    saveAndSendTickParams();
}

/*! 响应输入框槽函数*/
void tickModeWidget::OnTickParamsValueChange(int index)
{
    int ret;
    getInputType(index);
    myInputPanel inputDlg1(type,min,max,m_Params[index]);
    ret = inputDlg1.exec();
    if (ret == QDialog::Accepted)
    {
        m_Params[index] = inputDlg1.getValue();
    }

    updateParamsDisplay();
    saveAndSendTickParams();
}

/*! 返回按钮槽函数*/
void tickModeWidget::onBackBtnPressedSlt()
{
    emit backToHomePageSig();
}
