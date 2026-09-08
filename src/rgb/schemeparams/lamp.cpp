#include "lamp.h"

/* 构造灯控设置页面 */
PageLamp::PageLamp(QWidget *parent)
    : QWidget(parent)
{
/* private data */
    flagExtra = struCnfg.nEnableLampAI;

/* page init */
    createPage();

/* page layout */
    layoutPage();
}

/* 获取当前灯控页面设置 */
void PageLamp::getLampParams()
{
    for (int i = 0; i < lampSwitchCbx.size(); i++) {
        if (lampSwitchCbx[i]->isChecked()) {
            m_nLamp[i] = 1;
        } else {
            m_nLamp[i] = 0;
        }
    }
}

/* 应用当前灯控页面设置 */
void PageLamp::setLampParams()
{
    /* write params to memory */
    getLampParams();

	struCnfg.nEnableLampAI = 1;
	updateMachineParams();

    switch(struCnfe.nMachine) {
    case MACHINE_CF:
        memcpy(struCnfp.struGroupCtrl[0].nLamp, m_nLamp, sizeof(m_nLamp));
        memcpy(struCnfp.struGroupCtrl[0].nLampLight, m_nLampLight, sizeof(m_nLampLight));
        break;
    default:
        break;
    }

    /* write params to fpga */
    myFlow.resetLamp();
}

/* 重置当前灯控页面设置 */
void PageLamp::resetLampParams()
{
	struCnfg.nEnableLampAI = 1;
    //! 更新机型相关变更参数
    updateMachineParams(); 

    switch(struCnfe.nMachine) {
    case MACHINE_CF:
        memcpy(m_nLamp, struCnfp.struGroupCtrl[0].nLamp, sizeof(struCnfp.struGroupCtrl[0].nLamp));
        memcpy(m_nLampLight, struCnfp.struGroupCtrl[0].nLampLight, sizeof(struCnfp.struGroupCtrl[0].nLampLight));
        break;
    default:
        break;
    }

    updateTabBar();

    /* trigger page update */
    updatePage();
}

/* 响应灯控开关勾选框设置 */
void PageLamp::setLampSwitch(int btnId)
{
    if (lampSwitchCbx[btnId]->isChecked()) {
        // do something
        m_nLamp[btnId] = 1;

    } else {
        // do something
        m_nLamp[btnId] = 0;
    }
}

/* 响应灯控亮度调节按钮 */
void PageLamp::setLampLight(int btnId)
{
    inputType type;
    int min, max, value;

    /* trigger input dialog */
    type = intType;
    min = 0;
    max = 4095;
    value = m_nLampLight[btnId];
    myInputPanel inputDlg(type, min, max, value);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get value */
        value = inputDlg.getValue();

        /* update params */
        m_nLampLight[btnId] = value;

        /* update page */
        lampLightValue[btnId]->setText(QString("%1").arg(value));

        /* write params */
    }
}

/* 响应灯控开关及灯控亮度页面切换按钮状态 */
void PageLamp::setLampExtra()
{
    flagExtra = !flagExtra;
    if (!flagExtra) {
        struCnfg.nEnableLampAI = 0;
    } else {
        struCnfg.nEnableLampAI = 1;
    }
    updatePage();
}

/* 初始化灯控设置页面 */
void PageLamp::createPage()
{
    tabBar = new MyTabBar();

    /* lamp switch group */
    createLampSwitch();

    /* lamp light group */
    createLampLight();

    /* lamp extra button */
    createLampExtra();
}

/* 构建灯控开关使能页面控件 */
void PageLamp::createLampSwitch()
{
    lampSwitchCbx.clear();
    for (int i = 0; i < MAX_LAMP; i++) {
        lampSwitchCbx.append(new MyCheckBox(tr(""), this));
    }

    /* signals to slots */
    lampSwitchMap = new QSignalMapper(this);

    for (int i = 0; i < lampSwitchCbx.size(); i++) {
        lampSwitchMap->setMapping(lampSwitchCbx[i], i);
        connect(lampSwitchCbx[i], SIGNAL(toggled(bool)), lampSwitchMap, SLOT(map()));
    }
    connect(lampSwitchMap, SIGNAL(mapped(int)), this, SLOT(setLampSwitch(int)));
}

/* 构建灯控亮度调节页面控件 */
void PageLamp::createLampLight()
{
    lampLightLabel.clear();
    lampLightValue.clear();

    for (int i = 0; i < MAX_LAMP; i++) {
        lampLightLabel.append(new myLabel(tr("")));
        lampLightValue.append(new myLineEdit(tr("")));
        lampLightValue[i]->setMaximumSize(QSize(BTN_WIDTH-10, BTN_HEIGHT-10));
        lampLightValue[i]->setMinimumSize(QSize(BTN_WIDTH-10, BTN_HEIGHT-10));
    }

    /* signals & slots */
    lampLightMap = new QSignalMapper(this);

    for (int i = 0; i < lampLightValue.size(); i++) {
        lampLightMap->setMapping(lampLightValue[i], i);
        connect(lampLightValue[i], SIGNAL(pressed()), lampLightMap, SLOT(map()));
    }
    connect(lampLightMap, SIGNAL(mapped(int)), this, SLOT(setLampLight(int)));
}

/* 创建灯控开关及灯控亮度切换控件 */
void PageLamp::createLampExtra()
{
    /* seperate frame line */
    topLine = new QFrame;
    topLine->setFrameShape(QFrame::VLine);
    topLine->setFrameShadow(QFrame::Sunken);
    bottomLine = new QFrame;
    bottomLine->setFrameShape(QFrame::VLine);
    bottomLine->setFrameShadow(QFrame::Sunken);

    /* switch button */
    lampExtraBtn = new myPushButton("", myIcon.Arrow_Right);
    lampExtraBtn->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);
    lampExtraBtn->setIconSize(QSize(ICON_WID, ICON_HEI));

    connect(lampExtraBtn, SIGNAL(pressed()), this, SLOT(setLampExtra()));
}

/* 更新机型相关变更参数 */
void PageLamp::updateMachineParams()
{
    /* lamp extra page */
    switch (struCnfe.nMachine) {
    case MACHINE_CF:
        if(struCnfe.nEnableHGQMachine==1)
        {
            topLine->hide();
            lampExtraBtn->hide();
            bottomLine->hide();
        } else {
            topLine->show();
            lampExtraBtn->show();
            bottomLine->show();
        }
        break;
    default:
        topLine->hide();
        lampExtraBtn->hide();
        bottomLine->hide();
        break;
    }
    myFlow.updateLampTotal();
}

/* 更新灯控开关页面列表命名 */
void PageLamp::updateLampText()
{
    /* label of lamp checkboxes */
    myFlow.getLampName();

    for (int i = 0; i < lampSwitchCbx.size(); i++) {
//            qDebug()<<"i:"<<i<<"-"<<myString.sLamp[i]<<endl;
        lampSwitchCbx[i]->setText(myString.sLamp[i]);
        lampLightLabel[i]->setText(myString.sLamp[i]);
    }
}

/* 更新灯控开关页面列表状态 */
void PageLamp::updateLampState()
{
    m_nLampTotal = struCnfg.nLampTotal;

    for (int i = 0; i < lampSwitchCbx.size(); i++) {
        if (i < m_nLampTotal) {
            lampSwitchCbx[i]->show();
        } else {
            lampSwitchCbx[i]->hide();
        }
        if (m_nLamp[i] == 1) {
            lampSwitchCbx[i]->setChecked(true);
        } else {
            lampSwitchCbx[i]->setChecked(false);
        }
    }
}

/* 更新灯控页面列表 */
void PageLamp::updateLampSwitch()
{
    /* lamp text */
    updateLampText();

    /* lamp state */
    updateLampState();
}

/* 更新灯控亮度调节页面 */
void PageLamp::updateLampLight()
{
    flagExtra = struCnfg.nEnableLampAI;
    for (int i = 0; i < MAX_SRC_BOARD; i++) {
        lampLightGroup[i]->hide();
    }
    if (!flagExtra) {   // 控制板界面
        lampExtraBtn->setIcon(myIcon.Arrow_Right);
        lampSwitchGroup->show();
    } else {            // 智能恒流源界面
        lampExtraBtn->setIcon(myIcon.Arrow_Left);
        lampSwitchGroup->hide();

        for (int i = 0; i < lampLightValue.size(); i++) {
            lampLightValue[i]->setText(QString("%1").arg(m_nLampLight[i]));
            lampLightLabel[i]->hide();
            lampLightValue[i]->hide();
        }

        if (((struCnfe.nMachine == MACHINE_CF) && (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal > 16))
                || ((struCnfe.nMachine == MACHINE_CF) && (struCnfe.nEnableCGMachine == 1))) {
            for (int i = 0; i < struCnfg.nLampAINum; i++) {

//                    lampLightGroup[2]->hide();
//                    lampLightGroup[5]->hide();

//                    lampLightGroup[6]->hide();
//                    lampLightGroup[7]->hide();
                if (struCnfg.nCurSrcBoradLampNum[i] == 0) {
                    lampLightGroup[i]->hide();
                } else {
                    lampLightGroup[i]->show();
                }

                    lampLightGroup[2]->hide();
                    lampLightGroup[5]->hide();

                    lampLightGroup[6]->hide();
                    lampLightGroup[7]->hide();
                for(int j = 0; j < struCnfg.nCurSrcBoradLampNum[i]; j++) {
                    lampLightLabel[i*6+j]->show();
                    lampLightValue[i*6+j]->show();
                }
            }

//            if (tabBar->currentIndex() == 0) { //! 显示第一层
//                for (int i = 0; i < struCnfg.nLampAINum/2; i++) {
//                    if (struCnfg.nCurSrcBoradLampNum[i] == 0) {
//                        lampLightGroup[i]->hide();
//                    } else {
//                        lampLightGroup[i]->show();
//                    }
//                    for(int j = 0; j < struCnfg.nCurSrcBoradLampNum[i]; j++) {
//                        lampLightLabel[i*6+j]->show();
//                        lampLightValue[i*6+j]->show();
//                    }
//                }
//            } else { //! 显示第二层
//                for (int i = struCnfg.nLampAINum/2; i < struCnfg.nLampAINum; i++) {
//                    if(struCnfe.nEnableCGMachine) { //! 按照原有布局第二层恒流源板会向后错一位，看起来不美观，因此加入此句改变第二层原有布局
//                        lampLightGroupLayout->addWidget(lampLightGroup[i], (i+1)/2, (i+1)%2);
//                    }
//                    if (struCnfg.nCurSrcBoradLampNum[i] == 0) {
//                        lampLightGroup[i]->hide();
//                    } else {
//                        lampLightGroup[i]->show();
//                    }
//                    for(int j = 0; j < struCnfg.nCurSrcBoradLampNum[i]; j++) {
//                        lampLightLabel[i*6+j]->show();
//                        lampLightValue[i*6+j]->show();
//                    }
//                }
//            }
        } else {
            for (int i = 0; i < struCnfg.nLampAINum; i++) {
//                lampLightGroup[i]->show();
//                if (struCnfg.nCurSrcBoradLampNum[i] == 0) {
//                    lampLightGroup[i]->hide();
//                } else {
//                    lampLightGroup[i]->show();
//                }
                for (int j = 0; j < struCnfg.nCurSrcBoradLampNum[i]; j++) {
                    lampLightLabel[i*6+j]->show();
                    lampLightValue[i*6+j]->show();
                }
            }
        }
    }

    for (int i = 0; i < lampLightLabel.size(); i++) {
        lampLightLabel[i]->setMinimumWidth(80);
    }


}

void PageLamp::updateTabBar()
{
    while (tabBar->count() > 0) {
        tabBar->removeTab(0);
    }
    if (((struCnfe.nMachine == MACHINE_CF) && (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal > 16))
            ||((struCnfe.nMachine == MACHINE_CF) && (struCnfe.nEnableCGMachine == 1))){
        tabBar->insertTab(0, myLan.lamp_front);
        tabBar->insertTab(1, myLan.lamp_rear);
    }
}

/* 更新灯控设置页面 */
void PageLamp::updatePage()
{
    updateMachineParams();

    /* lamp switch page */
    updateLampSwitch();

    /* lamp light page */
    updateLampLight();

    if (struCnfg.nEnableLampAI) {
        if (((struCnfe.nMachine == MACHINE_CF) && (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal > 16))
                || ((struCnfe.nMachine == MACHINE_CF) && (struCnfe.nEnableCGMachine == 1))) {
                tabBar->hide();
        }
    } else {
        tabBar->hide();
    }
}

void PageLamp::changeCurrentSlt(int index)
{
    updatePage();
}

/* 布局灯控设置页面 */
void PageLamp::layoutPage()
{
    /* lamp switch group */
    lampSwitchGroup = new myGroupBox(myLan.control_board);
    /* lamp switch gird layout */
    lampLayout = new QGridLayout(lampSwitchGroup);

    for (int i = 0; i < lampSwitchCbx.count(); i++) {
        lampLayout->addWidget(lampSwitchCbx[i], i/3, i%3, Qt::AlignLeft);
    }

    /* lamp light group layout */
    lampLightGroupLayout = new QGridLayout();
    /* lamp light group */
    for (int i = 0; i < MAX_SRC_BOARD; i++) {
        if(i==0 || i==4){
            lampLightGroup[i] = new myGroupBox(myLan.front_material);
        }
        if(i==1 || i==5){
            lampLightGroup[i] = new myGroupBox(myLan.back_background);
        }
        if(i==2 || i==6){
            lampLightGroup[i] = new myGroupBox(myLan.front_background);
        }
        if(i==3 || i==7){
            lampLightGroup[i] = new myGroupBox(myLan.back_material);
        }

//        lampLightGroup[i] = new myGroupBox(myLan.light_src_board + QString("%1").arg(i+1));
        lampLightGroup[i]->setMaximumWidth(BTN_WIDTH * 4);
//        lampLightGroupLayout->addWidget(lampLightGroup[i], i/2, i%2);
    }
    lampLightGroup[2]->hide();
    lampLightGroup[5]->hide();

    lampLightGroup[6]->hide();
    lampLightGroup[7]->hide();

    lampLightGroupLayout->addWidget(lampLightGroup[0], 0, 0);
    lampLightGroupLayout->addWidget(lampLightGroup[1], 0, 1);
    lampLightGroupLayout->addWidget(lampLightGroup[3], 1, 0);
    lampLightGroupLayout->addWidget(lampLightGroup[4], 1, 1);

    /* lamp extra button layout */
    lampExtraLayout = new QVBoxLayout;
    lampExtraLayout->addWidget(topLine, Qt::AlignHCenter);
    lampExtraLayout->addWidget(lampExtraBtn);
    lampExtraLayout->addWidget(bottomLine, Qt::AlignHCenter);

    /* lamp light grid layout */
    QVector<QHBoxLayout *> lampHBoxLayout;
    for(int i = 0; i < MAX_SRC_BOARD; i++) {
        lampLightLayout[i] = new QVBoxLayout(lampLightGroup[i]);
        lampLightLayout[i]->setSpacing(1);
    }
    for (int i = 0; i < lampLightValue.count(); i++) {
        lampHBoxLayout.append(new QHBoxLayout);
        lampHBoxLayout[i]->addWidget(lampLightLabel[i]);
        lampHBoxLayout[i]->addWidget(lampLightValue[i]);
        lampLightLayout[i/6]->addLayout(lampHBoxLayout[i]);
    }

    QHBoxLayout *LowerHLayout = new QHBoxLayout;
    LowerHLayout->addWidget(lampSwitchGroup);
    LowerHLayout->addLayout(lampExtraLayout);
    LowerHLayout->addLayout(lampLightGroupLayout);

    /* main layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabBar);
    mainLayout->addLayout(LowerHLayout);

    setLayout(mainLayout);

    connect(tabBar,SIGNAL(currentChanged(int)),this, SLOT(changeCurrentSlt(int)));
}
