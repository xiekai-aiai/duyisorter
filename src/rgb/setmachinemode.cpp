/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setmachinemode.cpp
 * \brief       彩色机型设置机型界面源文件
 * \date        2015.01.14
 */
#include "setmachinemode.h"

/*!
 * \brief setMachineMode::setMachineMode
 * 构造函数
 */
setMachineMode::setMachineMode(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0, 0 ,LCD_WIDTH, LCD_HEIGHT-100);
    config = g_Config::getInstance();

    // 加载机型配置参数
    createMachineTypeMap();

    // 创建机型选择列表
    createMachineTypeList();

    // 创建机型规格列表
    createMachineModeList();
    
    // 创建通道配置列表
    createChuteEjectList();

    // 确定取消功能按键
    QSize btnSize = QSize(BTN_WIDTH, BTN_HEIGHT);
    sureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply, this);
    sureBtn->setFixedSize(btnSize);
    backBtn = new myPushButton(myLan.back, myIcon.Action_Back, this);
    backBtn->setFixedSize(btnSize);

    // 整体界面布局
    QGridLayout *upLay = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *layout = new QVBoxLayout(this);

//    upLay->addWidget(machineTypeList, 0, 0, 2, 1);
    upLay->addWidget(machineModeList, 0, 1, 1, 2);
    upLay->addWidget(machineChuteGroup, 0, 3, 1, 3);
    if(LCD_WIDTH == 640) {
    	upLay->setContentsMargins(5, 10, 5, 10);
    } else {
        upLay->setContentsMargins(25, 25, 25, 25);
    }
    downLay->addWidget(sureBtn,Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(backBtn,Qt::AlignRight);
    layout->addLayout(upLay);
    layout->addLayout(downLay);

    // 信号 & 槽
    connect(sureBtn,SIGNAL(pressed()),this,SLOT(onSureBtnClicked()));
    connect(backBtn,SIGNAL(pressed()),this,SLOT(onBackBtnClicked()));
    connect(ejectNumMinusBtn, SIGNAL(pressed()), this, SLOT(onEjectMinusBtnClicked()));
    connect(ejectNumPlusBtn, SIGNAL(pressed()), this, SLOT(onEjectPlusBtnClicked()));
    connect(ejectNum, SIGNAL(pressed()), this, SLOT(onEjectNumBtnClicked()));
    connect(machineTypeList, SIGNAL(pressed(QModelIndex)), this, SLOT(onMachineTypeChangedSlt()));
    connect(machineModeList, SIGNAL(pressed(QModelIndex)), this, SLOT(onMachineModeChangedSlt()));
}

/*!
 * \brief setMachineMode::~setMachineMode
 * 析构函数
 */
setMachineMode::~setMachineMode()
{

}

/*!
 * \brief setMachineMode::setMachineModeParams
 * 初始化机型设置中的参数
 */
void setMachineMode::setMachineModeParams()
{
    int nUnitNumPerTickGroup = 0;                       // 每组内相机板数量
    m_nMachineMode = struCnfe.nMachine;                 // 机型
    m_nDeriveMachineMode = struCnfe.nDerivedDevType;    // 衍生机型
    m_nCGMachineMode = struCnfe.nEnableCGMachine;       // 杂粮CG机型
    m_nEjectNumPerBoard = struCnfg.nEjectorsPerChute;   // 喷嘴数量
    m_nLevelTotal = struCnfg.nLevelTotal;               // 层数
    for (int i = 0; i < MAX_LEVEL; i++) {
        m_nViewTotal[i] = struCnfg.struLevelInfo[i].nViewTotal;
        m_nViewState[i] = 0;
    }
    for (int i = 0; i < struCnfg.nLevelTotal; i++) { // 每层通道数量
        // 仅适用于通道式机型
        m_nChutePerLevel[i] = struCnfg.struLevelInfo[i].nUnitLevelTotal/struCnfg.struLevelInfo[i].nViewTotal;
    }

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for(int j = 0; j < MAX_GROUP_TICK; j++) {
            m_nChutePerTickGroup[i][j] = 0;    // 单组内相机板数量
            nUnitNumPerTickGroup = struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount;
            switch (struCnfg.struLevelInfo[i].nViewTotal) {
            case SINGLE_VIEW: // 单视
                for (int k = 0; k < nUnitNumPerTickGroup; k++) {
                    if (struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k*2] == 0) { // 当前前视相机板是否被分割
                        m_nChutePerTickGroup[i][j] += 1;
                    } else {
                        m_nChutePerTickGroup[i][j] += 0.5;
                    }
                }
                break;
            case DOUBLE_VIEW: // 双视
                for (int k = 0; k < nUnitNumPerTickGroup; k += 2) {
                    if (struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] == 0) {   // 当前前视相机板是否被分割
                        m_nChutePerTickGroup[i][j] += 1;
                    } else {
                        m_nChutePerTickGroup[i][j] += 0.5;
                    }
                }
                break;
            case QUADRUPLE_VIEW: // 四视
                break;
            default:
                break;
            }
        }
    }

    // 初始化机型配置参数
    qMachineModeConfig = getMachineModeConfig();
}

/*!
 * \brief setMachineMode::createMachineTypeList
 * 创建机型选择列表
 */
void setMachineMode::createMachineTypeList()
{
    machineTypeList = new QListWidget;
    machineTypeList->setFixedWidth(130);

    // 获取机型选择列表
    for (int i = 0; i < MAX_GENERAL_MACHINE_NUM; i++){
        typeItem[i] = new QListWidgetItem;
        typeItem[i]->setTextAlignment(Qt::AlignCenter);
        typeItem[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        typeItem[i]->setSizeHint(QSize(50, 50));
        machineTypeList->addItem(typeItem[i]);
    }
}

/*!
 * \brief setMachineMode::createMachineModeList
 * 创建机型规格列表
 */
void setMachineMode::createMachineModeList()
{
    machineModeList = new QListWidget(this);
    for (int i = 0; i < machineModeNum; i++) {
        modeItem[i] = new QListWidgetItem;
        modeItem[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));        
        modeItem[i]->setSizeHint(QSize(50, 50));
        machineModeList->addItem(modeItem[i]);
    }
}

/*!
 * \brief setMachineMode::createChuteEjectList
 * 创建通道配置列表
 */
void setMachineMode::createChuteEjectList()
{
    // 通道配置比例
    machineChuteGroup = new myGroupBox("");
    machineChuteGroup->setFont(config->getFont());
    machineChuteGroup->setMinimumWidth(280);
    QGridLayout *groupLayout = new QGridLayout(machineChuteGroup);

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    chuteMinusBtnMapper = new QSignalMapper();
    chutePlusBtnMapper  = new QSignalMapper();
    for(int i = 0; i < tickGroupPerLevel; i++){
        chuteLabel[i] = new QLabel;
        chuteLabel[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        chuteMinusBtns[i] = new myPushButton(myIcon.Action_Minus, btnSize);
        chuteEjectNum[i] = new myLCDNumber();
        chuteEjectNum[i]->setDigitCount(3);
        chuteEjectNum[i]->setFixedSize(QSize(80, 45));
        chuteEjectNum[i]->display(m_nChutePerTickGroup[0][i]*m_nEjectNumPerBoard);
        chutePlusBtns[i] = new myPushButton(myIcon.Action_Plus, btnSize);

        chuteMinusBtnMapper->setMapping(chuteMinusBtns[i], i);
        connect(chuteMinusBtns[i], SIGNAL(pressed()), chuteMinusBtnMapper, SLOT(map()));
        chutePlusBtnMapper->setMapping(chutePlusBtns[i], i);
        connect(chutePlusBtns[i], SIGNAL(pressed()), chutePlusBtnMapper, SLOT(map()));
        groupLayout->addWidget(chuteLabel[i], i, 0, 1, 2);
        groupLayout->addWidget(chuteMinusBtns[i], i, 2);
        groupLayout->addWidget(chuteEjectNum[i], i, 3);
        groupLayout->addWidget(chutePlusBtns[i], i, 4);
    }
    connect(chuteMinusBtnMapper, SIGNAL(mapped(int)), this, SLOT(onChuteMinusBtnClicked(int)));
    connect(chutePlusBtnMapper, SIGNAL(mapped(int)), this, SLOT(onChutePlusBtnClicked(int)));

    // 通道喷阀数量
    ejectNumLabel = new QLabel(myLan.eject_num);
    ejectNumLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    ejectNumMinusBtn = new myPushButton(myIcon.Action_Minus, btnSize);
    ejectNum = new myLCDNumber(3,m_nEjectNumPerBoard,this);
    ejectNum->setFixedSize(QSize(80, 45));
    ejectNum->display(m_nEjectNumPerBoard);
    ejectNumPlusBtn = new myPushButton(myIcon.Action_Plus, btnSize);

    groupLayout->addWidget(ejectNumLabel, tickGroupPerLevel, 0, 1, 2);
    groupLayout->addWidget(ejectNumMinusBtn, tickGroupPerLevel, 2);
    groupLayout->addWidget(ejectNum, tickGroupPerLevel, 3);
    groupLayout->addWidget(ejectNumPlusBtn, tickGroupPerLevel, 4);

    // 前/后视、红外/可见配置
    LineStatus = new QFrame();
    LineStatus->setFrameShape(QFrame::HLine);
    LineStatus->setFrameShadow(QFrame::Sunken);
    QSignalMapper *viewBoxMapper = new QSignalMapper();
    viewBox.clear();
    viewBox.append(new MyCheckBox(myLan.front,struCnfe.nDerivedDevType & 0x0001));
    viewBox.append(new MyCheckBox(myLan.rear,struCnfe.nDerivedDevType & 0x0002));
    viewBox.append(new MyCheckBox(myLan.infra+"-"+myLan.front,struCnfe.nDerivedDevType & 0x0100));
    viewBox.append(new MyCheckBox(myLan.infra+"-"+myLan.rear,struCnfe.nDerivedDevType & 0x0200));

    machineViewBoxGroup = new myGroupBox("");
    QGridLayout *boxGroupLayout = new QGridLayout(machineViewBoxGroup);
    for (int i = 0; i < viewBox.size(); i++) {
        viewBox[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        boxGroupLayout->addWidget(viewBox.at(i), i/2, i%2, Qt::AlignLeft);

        viewBoxMapper->setMapping(viewBox[i],i);
        connect(viewBox[i],SIGNAL(toggled(bool)),viewBoxMapper,SLOT(map()));
    }
    connect(viewBoxMapper,SIGNAL(mapped(int)),this,SLOT(onViewBoxClicked(int)));

    if (LCD_WIDTH == 1024) {
        machineViewBoxGroup->setFixedSize(BTN_WIDTH*2+30,BTN_HEIGHT*3);
    } else {
        machineViewBoxGroup->setFixedSize(DEFAULT_BTN_WIDTH*2+50,BTN_HEIGHT*3-10);
    }

    // LED mode
    ledModeCb = new MyCheckBox("  4 LEDs");
    ledModeCb->setFixedSize(BTN_WIDTH, BTN_HEIGHT+20);
    ledModeCb->hide();
    groupLayout->addWidget(LineStatus, tickGroupPerLevel+1, 0, 1, 5);
    groupLayout->addWidget(machineViewBoxGroup,tickGroupPerLevel+2,0,2,4,Qt::AlignLeft);
    groupLayout->addWidget(ledModeCb, tickGroupPerLevel+3, 0, 1, 4);


    // Run mode
    runModeLabel = new QLabel(myLan.run_mode);
    runModeLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    runModeBox = new MyComboBox();
    runModeBox->setFixedHeight(BTN_HEIGHT);
    QList<int> list = myFlow.getRunModeList();
    for (int i = 0; i < list.count(); i++) {
        runModeBox->addItem(myFlow.getRunModeName(list.at(i)), list.at(i));
    }
    groupLayout->addWidget(runModeLabel, tickGroupPerLevel+4, 0, 1, 2);
    groupLayout->addWidget(runModeBox, tickGroupPerLevel+4, 2, 1, 3);
 }

/*!
 * \brief setMachineMode::onSureBtnClicked
 * 响应界面中的确认按钮
 */
void setMachineMode::onSureBtnClicked()
{
    char cmd[100];
    int nTotalPre = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;

    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    /* 根据当前进行设置全局参数 */
    if (m_nOrignMachineSeq != machineTypeList->currentRow()
            || (isViewStateChanged())) { //主机型变化或者上中下层单双视角切换
        snprintf(cmd, sizeof(cmd),"%s", "mv -f userdata/cnf/cnf.vpn-auth userdata/cnf/cnf.network userdata");
        system(cmd);

        snprintf(cmd, sizeof(cmd), "%s", "rm -rf userdata/cnf/*");
        system(cmd);

        snprintf(cmd, sizeof(cmd), "%s", "mv -f userdata/cnf.vpn-auth userdata/cnf.network userdata/cnf");
        system(cmd);

        struCnfe.nMachine = m_nMachineMode;
        struCnfe.nDerivedDevType = m_nDeriveMachineMode;
        struCnfe.nEnableCGMachine = m_nCGMachineMode;
        myFlow.saveEngineer();

        /* 获取所有配置参数 */
        myFlow.getGlobal();         // 获取全局需要保存的参数
        myFlow.getProfile();        // 获取方案参数
        myFlow.getCamera();         // 获取相机参数
    } else {  //主机型只有通道配置变化
        struCnfe.nDerivedDevType = m_nDeriveMachineMode;
        struCnfe.nEnableCGMachine = m_nCGMachineMode;
        myFlow.saveEngineer();
    }

    /* 根据当前设置配置全局参数 */
    setLevelParams();           // 设置层信息
    setGroupParams();           // 设置层内剔除组与识别组信息
    resetLedMode();
    resetRunMode();
    resetLampParams(nTotalPre);

    /* 将设置的全局参数保存到配置文件中 */
    myFlow.saveGlobal();
    myFlow.saveCamera();
    myFlow.saveProfile();

    /* 根据当前进行设置全局参数 */
    if ((m_nOrignMachineSeq != machineTypeList->currentRow())
            || (isViewStateChanged())) {
        infoWidget->delayHide();        
        system("reboot");           //! 重启系统提示
    } else {
        /* 获取所有配置参数 */
        myFlow.getGlobal();         //! 获取全局需要保存的参数
        myFlow.getProfile();        //! 获取方案参数
        myFlow.getCamera();         //! 获取相机参数
        myFlow.getDefaultPara();

        myFlow.updateLampTotal();       //! 更新灯控数量
        myFlow.updateLightSrcLampNum(); //! 更新单个恒流源板的灯控数量
        myFlow.resetGroupParams();  //! 重置组、通道等相关参数
        myFlow.initCamera();
        myFlow.resetSortParams();

        infoWidget->delayHide();
        emit backToFactory();
    }
}

/*!
 * \brief setMachineMode::onBackBtnClicked
 * 响应界面中的返回按钮
 */
void setMachineMode::onBackBtnClicked()
{
    emit backToFactory();
}

/*!
 * \brief setMachineMode::refreshMachineModeDisplaySlt
 * 刷新设置机型显示界面
 */
void setMachineMode::refreshMachineModeDisplaySlt()
{
    // 初始化机型参数
    setMachineModeParams();
    
    // 刷新机型配置页面 - 非默认参数显示
    refreshDisplay(qMachineModeConfig);

    // 保存当前机型选择列表
    m_nOrignMachineSeq = machineTypeList->currentRow();
}

/*!
 * \brief setMachineMode::refreshMachineTypeList
 * 刷新机型选择列表
 * \param config
 */
void setMachineMode::refreshMachineTypeList(const glMachineModeConfig &config)
{
    // 获取机型选择列表
    QList<int> typeList = config.qlTypeList;

    // 刷新机型列表显示
    for (int i = 0; i < typeList.size(); i++) {
        QString text = getMachineTypeName(qMachineTypeMap, typeList.at(i));
        machineTypeList->item(i)->setText(text);
        machineTypeList->item(i)->setHidden(false);
    }
    // 隐藏多余机型列表
    for (int i = typeList.size(); i < MAX_GENERAL_MACHINE_NUM; i++) {
        machineTypeList->item(i)->setHidden(true);
    }

    // 切换机型列表选项
    machineTypeList->setCurrentRow(typeList.indexOf(config.nTypeIndex));
}

/*!
 * \brief setMachineMode::refreshMachineModeList
 * 刷新机型规格列表
 * \param config
 */
void setMachineMode::refreshMachineModeList(const glMachineModeConfig &config)
{
    // 实时更新规格列表
    QStringList modeList = config.qlModeList;
    for (int i = 0; i < modeList.size(); i++) {
        machineModeList->item(i)->setText(modeList.at(i));
        machineModeList->item(i)->setHidden(false);
    }

    // 隐藏多余规格列表
    for(int i = modeList.size(); i < machineModeNum; i++) {
        machineModeList->item(i)->setHidden(true);
    }   
    machineModeList->setCurrentRow(config.nModeIndex);
}

/*!
 * \brief setMachineMode::refreshChuteEjectNum
 * 刷新通道配置显示
 * \param config
 */
void setMachineMode::refreshChuteEjectNum(const glMachineModeConfig &config)
{
    // 隐藏通道配比按钮
    for (int i = 0; i < tickGroupPerLevel; i++) {
        hideChuteRow(i);
        enableChuteRow(i);
    }

    // 显示通道配比信息
    int nTypeIndex = config.nTypeIndex;
    Q_UNUSED(nTypeIndex);
    for (int i = 0; i < config.nTickPerLevel; i++) {
        chuteLabel[i]->setText(QString("%1 %2").arg(i+1).arg(myLan.times));
        chuteEjectNum[i]->display(m_nChutePerTickGroup[ONE_LEVEL][i]*m_nEjectNumPerBoard);
        showChuteRow(i);
        //! 默认最后一组不使能
        if (i == (config.nTickPerLevel-1)) {
            disableChuteRow(i);
        }
    }

    //! 显示单通道喷阀数量
    ejectNum->display(m_nEjectNumPerBoard);
    ejectNumPlusBtn->setEnabled(config.bEnableEject);
    ejectNumMinusBtn->setEnabled(config.bEnableEject);
}

/*!
 * \brief setMachineMode::refreshViewBoxGroup
 * 刷新整机视角配置界面显示
 * \param config
 */
void setMachineMode::refreshViewBoxGroup(const glMachineModeConfig &config)
{
    QString text;
    Q_UNUSED(text);

    // 依据机型配置显示
    if (config.bEnableView) {
        machineViewBoxGroup->show();
    } else {
        machineViewBoxGroup->hide();
    }
    // 默认隐藏所有配置项
    for (int i = 0; i < viewBox.size(); i++) {
        viewBox[i]->hide();
    }
}

/*!
 * \brief setMachineMode::refreshLedModeDisplay
 * 刷新LED指示灯模式的显示
 * \param config
 */
void setMachineMode::refreshLedModeDisplay(const glMachineModeConfig &config)
{
    // 根据机型设定显示或隐藏
    if (config.bEnableLED) {
        ledModeCb->show();
        if (struCnfg.nLedMode == 0) {
            ledModeCb->setChecked(false);
        } else if (struCnfg.nLedMode == 1) {
            ledModeCb->setChecked(true);
        }
    } else {
        ledModeCb->hide();
    }
}

/*!
 * \brief setMachineMode::refreshRunModeDisplay
 * 刷新屏运行模式设定显示
 * \param config
 */
void setMachineMode::refreshRunModeDisplay(const glMachineModeConfig &config)
{
    // 匹配指定当前屏模式选择
    int index = runModeBox->findData(struCnfg.nRunMode, Qt::UserRole);
    if (index != -1) {
        runModeBox->setCurrentIndex(index);
    }

    // 根据机型设定显示或隐藏
    if (config.bEnableRunMode) {
        runModeBox->show();
    } else {
        runModeBox->hide();
    }
}


/*!
 * \brief setMachineMode::onMachineModeChangedSlt
 * 响应机型规格列表索引号变化
 */
void setMachineMode::onMachineModeChangedSlt()
{
    int index = machineModeList->currentRow();

    // 默认通道配比参数
    switch(index) {
    case 0:
        m_nChutePerTickGroup[0][0] = 1;
        m_nChutePerTickGroup[0][1] = 0;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 1;
        break;
    case 1:
        m_nChutePerTickGroup[0][0] = 1;
        m_nChutePerTickGroup[0][1] = 1;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 2;
        break;
    case 2:
        m_nChutePerTickGroup[0][0] = 2;
        m_nChutePerTickGroup[0][1] = 1;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerLevel[0] = 3;
        break;
    case 3:
        m_nChutePerTickGroup[0][0] = 3;
        m_nChutePerTickGroup[0][1] = 1;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 4;
        break;
    case 4:
        m_nChutePerTickGroup[0][0] = 4;
        m_nChutePerTickGroup[0][1] = 1;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 5;
        break;
    case 5:
        m_nChutePerTickGroup[0][0] = 4;
        m_nChutePerTickGroup[0][1] = 2;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 6;
        break;
    case 6:
        m_nChutePerTickGroup[0][0] = 4;
        m_nChutePerTickGroup[0][1] = 2;
        m_nChutePerTickGroup[0][2] = 1;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 7;
        break;
    case 7:
        m_nChutePerTickGroup[0][0] = 6;
        m_nChutePerTickGroup[0][1] = 2;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 8;
        break;
    case 8:
        m_nChutePerTickGroup[0][0] = 6;
        m_nChutePerTickGroup[0][1] = 3;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 9;
        break;
    case 9:
        m_nChutePerTickGroup[0][0] = 7;
        m_nChutePerTickGroup[0][1] = 3;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 10;
        break;
    case 10:
        m_nChutePerTickGroup[0][0] = 8;
        m_nChutePerTickGroup[0][1] = 3;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 11;
        break;
    case 11:
        m_nChutePerTickGroup[0][0] = 9;
        m_nChutePerTickGroup[0][1] = 3;
        m_nChutePerTickGroup[0][2] = 0;
        m_nChutePerTickGroup[0][3] = 0;
        m_nChutePerLevel[0] = 12;
        break;
    default:
        break;
    }

    //! 默认喷嘴数量（单通道）
    m_nEjectNumPerBoard = struCnfg.nEjectorsPerChute;

    //! 刷新设置机型界面
    qMachineModeConfig = getMachineModeConfig();
    refreshChuteEjectNum(qMachineModeConfig);
}

/*!
 * \brief setMachineMode::onMachineTypeChangedSlt
 * 响应机型选择列表索引号变化
 */
void setMachineMode::onMachineTypeChangedSlt()
{
    // 获取对应机型索引
    int index = machineTypeList->currentRow();
    int nTypeIndex = qMachineModeConfig.qlTypeList.at(index);

    // 如果当前选择未变化，直接返回
    if (nTypeIndex == qMachineModeConfig.nTypeIndex) {
        return;
    }

    m_nCGMachineMode = 0;

    // 仅支持 CF 机型
    m_nMachineMode = MACHINE_CF;
    m_nLevelTotal = 1;

    // 机型变更时，仅显示默认机型规格列表及相关参数
    if (qMachineModeConfig.nModeIndex >= getMachineModeList(nTypeIndex).count()) {
       machineModeList->setCurrentRow(0);
    }
    onMachineModeChangedSlt();
    // 更新全局配置参数, 刷新界面
    qMachineModeConfig = getMachineModeConfig();
    refreshDisplay(qMachineModeConfig);
}

/*!
 * \brief setMachineMode::onChuteMinusBtnClicked
 * 响应通道配比减小操作
 * \param index
 */
void setMachineMode::onChuteMinusBtnClicked(int index)
{
    // 默认分配比例0.5
    float step = qMachineModeConfig.nChuteRatio;
    // 减操作处理
    int tmp = chuteEjectNum[index]->value();
    for (int i = 0; i < m_nLevelTotal; i++) {
        if (tmp > m_nEjectNumPerBoard*step) {
            if ((index+1) < qMachineModeConfig.nTickPerLevel) {
                m_nChutePerTickGroup[i][index]   -= step;
                m_nChutePerTickGroup[i][index+1] += step;
                chuteEjectNum[index]->display(m_nEjectNumPerBoard*m_nChutePerTickGroup[i][index]);
                chuteEjectNum[index+1]->display(m_nEjectNumPerBoard*m_nChutePerTickGroup[i][index+1]);
            }
        }
    }
}

/*!
 * \brief setMachineMode::onChutePlusBtnClicked
 * 响应通道配比增加操作
 * \param index
 */
void setMachineMode::onChutePlusBtnClicked(int index)
{
    // 默认分配比例0.5
    float step = qMachineModeConfig.nChuteRatio;

    // 加操作处理
    int tmp = chuteEjectNum[index]->value();
    for (int i = 0; i < m_nLevelTotal; i++) {
        int max = m_nChutePerLevel[i]*m_nEjectNumPerBoard;
        for (int j = 0; j < index; j++) {
            max -= chuteEjectNum[j]->value();
        }
        max = max < 0 ? 0 : max;
        if (tmp < max) {
            chuteEjectNum[index]->display(tmp+m_nEjectNumPerBoard*step);
            m_nChutePerTickGroup[i][index] += step;
            for (int j = qMachineModeConfig.nTickPerLevel -1; j > index; j--) {
                if (m_nChutePerTickGroup[i][j] > 0) {
                    m_nChutePerTickGroup[i][j] -= step;
                    chuteEjectNum[j]->display(m_nEjectNumPerBoard*m_nChutePerTickGroup[i][j]);
                    break;
                }
            }
        }
    }
}

/*!
 * \brief setMachineMode::onEjectMinusBtnClicked
 * 响应喷嘴数目减小按钮
 */
void setMachineMode::onEjectMinusBtnClicked()
{
    int tmp = ejectNum->value();
    if(tmp >= 41) {
        ejectNum->display(tmp-qMachineModeConfig.nEjectStep);
    }

    m_nEjectNumPerBoard = ejectNum->value();
    refreshChuteEjectNum(qMachineModeConfig);
}

/*!
 * \brief setMachineMode::onEjectPlusBtnClicked
 * 响应喷嘴数目增加按钮
 */
void setMachineMode::onEjectPlusBtnClicked()
{
    int tmp = ejectNum->value();
    if(tmp <= 79) {
        ejectNum->display(tmp+qMachineModeConfig.nEjectStep);
    }

    m_nEjectNumPerBoard = ejectNum->value();
    refreshChuteEjectNum(qMachineModeConfig);
}

void setMachineMode::onEjectNumBtnClicked(){
    int tmp = ejectNum->value();
    myInputPanel inputDlg(intType,0,80,tmp);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        tmp= inputDlg.getValue();
        if(tmp!=40 && tmp!=64 && tmp!=80){
            infoWidget->setLabelText(myLan.ai_save_error);
            infoWidget->delayShow();
            myFlow.sleep(1);
            infoWidget->hide();
            return;
        }
        ejectNum->display(tmp);
    }
    m_nEjectNumPerBoard = ejectNum->value();
    refreshChuteEjectNum(qMachineModeConfig);
}



/*!
 * \brief setMachineMode::setLevelParams
 * 设置层信息参数
 */
void setMachineMode::setLevelParams()
{
    struCnfg.nLevelTotal = m_nLevelTotal;
    struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;

    struCnfg.nInterfaceBoardTotal = 1;
    struCnfg.nCtrlBoardTotal = 1;
    {
        struCnfg.nLampTotal = 12;       
        struCnfg.nEnableLampAI = 1;
        struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal = m_nChutePerLevel[ONE_LEVEL]*2;
        if (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal > MAX_UNIT_PER_INT) {
            struCnfg.nInterfaceBoardTotal = 2;
        } else {
            struCnfg.nInterfaceBoardTotal = 1;
        }
    }

    struCnfg.struLevelInfo[0].nViewTotal = 2;       // 双视
    if (m_nChutePerTickGroup[0][1] > -0.000001 && m_nChutePerTickGroup[0][1] < 0.000001){
        struCnfg.struLevelInfo[0].nTickGroupTotal = 1;
        struCnfg.struLevelInfo[0].nIdentifyGroupTotal = 2;
    } else if (m_nChutePerTickGroup[0][2] > -0.000001 && m_nChutePerTickGroup[0][2] < 0.000001){
        struCnfg.struLevelInfo[0].nTickGroupTotal = 2;
        struCnfg.struLevelInfo[0].nIdentifyGroupTotal = 4;
    } else if (m_nChutePerTickGroup[0][3] > -0.000001 && m_nChutePerTickGroup[0][3] < 0.000001){
        struCnfg.struLevelInfo[0].nTickGroupTotal = 3;
        struCnfg.struLevelInfo[0].nIdentifyGroupTotal = 6;
    } else {
        struCnfg.struLevelInfo[0].nTickGroupTotal = 4;
        struCnfg.struLevelInfo[0].nIdentifyGroupTotal = 8;
    }
}

/*!
 * \brief 设置全局参数中的组参数
 */
void setMachineMode::setGroupParams()
{
    setCFGroupParams();
}

/*!
 * \brief 根据索引号隐藏通道配比行
 */
void setMachineMode::hideChuteRow(int index)
{
    chuteLabel[index]->hide();
    chutePlusBtns[index]->hide();
    chuteMinusBtns[index]->hide();
    chuteEjectNum[index]->hide();
}

/*!
 * \brief 根据索引号显示通道配比行
 */
void setMachineMode::showChuteRow(int index)
{
    chuteLabel[index]->show();
    chutePlusBtns[index]->show();
    chuteMinusBtns[index]->show();
    chuteEjectNum[index]->show();
}

/*!
 * \brief 根据索引号使能通道配比行
 */
void setMachineMode::enableChuteRow(int index)
{
    chuteMinusBtns[index]->setEnabled(true);
    chutePlusBtns[index]->setEnabled(true);
}

/*!
 * \brief 根据索引号不使能通道配比行
 */
void setMachineMode::disableChuteRow(int index)
{
    chuteMinusBtns[index]->setEnabled(false);
    chutePlusBtns[index]->setEnabled(false);
}

/*!
 * \brief 根据机型配置刷新显示界面 
 */
void setMachineMode::refreshDisplay(const glMachineModeConfig &config)
{
    //! 刷新机型选择列表

    refreshMachineTypeList(config);
    //! 刷新机型规格列表
    refreshMachineModeList(config);

    //! 刷新机型配置列表
    refreshChuteEjectNum(config);

    //! 刷新机型视角配置
    refreshViewBoxGroup(config);

    //! 刷新LED指示灯模式显示
    refreshLedModeDisplay(config);

    //! 刷新屏运行模式设定显示
    refreshRunModeDisplay(config);
}

/*!
 * \brief 设置CF机型的剔除参数 
 */
void setMachineMode::setCFGroupParams()
{
    quint8 chute2StartIndex;        // 记录二次色选的起始板号
    quint8 chute3StartIndex;        // 记录三次色选的起始板号
    quint8 chute4StartIndex;        // 记录四次色选的起始板号
    bool chute1IsSplice = false;    // 记录一次色选是否有分隔的通道
    bool chute2IsSplice = false;    // 记录二次色选是否有分隔的通道
    bool chute3IsSplice = false;    // 记录三次色选是否有分隔的通道

    m_nChutePerLevel[0] = 0;

    // 单通道喷嘴数量设置
    if (struCnfg.nEjectorsPerChute != m_nEjectNumPerBoard) {
        struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;
        emit resetCamera();
    }

    for(int i=0; i<tickGroupPerLevel; i++){
        m_nChutePerLevel[0] += m_nChutePerTickGroup[0][i];
    }
    struCnfg.struLevelInfo[0].nUnitLevelTotal = m_nChutePerLevel[0]*2;
    for(int i=0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal; i++){
        struCnfg.struLevelInfo[0].nUnitId[i] = i;
    }

    /* 设置剔除组参数 */
    for(int i=0; i<struCnfg.struLevelInfo[0].nTickGroupTotal; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[i].nLevelIndex = 0;
        struCnfg.struLevelInfo[0].struTickGroupInfo[i].nNameIndex = i;
    }

    /* 设置通道1的相关参数 */
    if(((int)(m_nChutePerTickGroup[0][0]*2)%2) == 1){
        chute1IsSplice = true;
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount = m_nChutePerTickGroup[0][0]*2+1;
        for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount-2; i++){
            struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[i] = 0;
        }
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount-2] = 1;
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount-1] = 1;
        chute2StartIndex = struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount-2;
    } else{
        chute1IsSplice = false;
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount = m_nChutePerTickGroup[0][0]*2;
        for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount; i++){
            struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[i] = 0;
        }
        chute2StartIndex = struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount;
    }
    for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitId[i] = i;
    }

    /* 设置通道2的相关参数 */
    if(chute1IsSplice){
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[0] = 2;
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[1] = 2;
        if(((int)(m_nChutePerTickGroup[0][1]*2)%2) == 0){   // 通道1分隔且通道2分隔
            chute2IsSplice = true;
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][1]*2+2;
            for(int i=2; i<struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[i] = 0;
            }
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2] = 1;
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-1] = 1;
            chute3StartIndex = chute2StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2;
        } else{                                             // 通道1分隔且通道2不分隔
            chute2IsSplice = false;
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][1]*2+1;
            for(int i=2; i<struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[i] = 0;
            }
            chute3StartIndex = chute2StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount;
        }
    }
    else{
        if(((int)(m_nChutePerTickGroup[0][1]*2)%2) == 1){   // 通道1不分隔且通道2分隔
            chute2IsSplice = true;
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][1]*2+1;
            for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[i] = 0;
            }
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2] = 1;
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-1] = 1;
            chute3StartIndex = chute2StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2;
        } else{                                             // 通道1不分隔且通道2不分隔
            chute2IsSplice = false;
            struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][1]*2;
            for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[i] = 0;
            }
            chute3StartIndex = chute2StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount;
        }
    }
    for(int i=chute2StartIndex; i<chute2StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitId[i-chute2StartIndex] = i;
    }

    /* 设置通道3参数 */
    if(chute2IsSplice) {
        struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[0] = 2;
        struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[1] = 2;
        if(((int)(m_nChutePerTickGroup[0][2]*2)%2) == 0){   // 通道2分隔且通道3分隔
            chute3IsSplice = true;
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount = m_nChutePerTickGroup[0][2]*2+2;
            for(int i=2; i<struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[i] = 0;
            }
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount-2] = 1;
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount-1] = 1;
            chute4StartIndex = chute3StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount-2;
        } else{                                             // 通道2分隔且通道3不分隔
            chute2IsSplice = false;
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount = m_nChutePerTickGroup[0][2]*2+1;
            for(int i=2; i<struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[i] = 0;
            }
            chute4StartIndex = chute3StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount;
        }
    } else{
	if(((int)(m_nChutePerTickGroup[0][2]*2)%2) == 1){   // 通道2不分隔且通道3分隔
            chute3IsSplice = true;
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount = m_nChutePerTickGroup[0][2]*2+1;
            for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount-2; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[i] = 0;
            }
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount-2] = 1;
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount-1] = 1;
            chute4StartIndex = chute3StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount-2;
        } else{                                             // 通道2不分隔且通道3不分隔
            chute3IsSplice = false;
            struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount = m_nChutePerTickGroup[0][2]*2;
            for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount; i++){
                struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitSplice[i] = 0;
            }
            chute4StartIndex = chute3StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount;
        }
    }
    for(int i=chute3StartIndex; i<chute3StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitCount; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[2].nUnitId[i-chute3StartIndex] = i;
    }

    /* 设置通道4参数 */
    if(chute3IsSplice) {
        struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitSplice[0] = 2;
        struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitSplice[1] = 2;
        struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitCount = m_nChutePerTickGroup[0][3]*2+1;
        for(int i=2; i<struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitCount; i++){
            struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitSplice[i] = 0;
        }
    } else{
        struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitCount = m_nChutePerTickGroup[0][3]*2;
        for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitCount; i++){
            struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitSplice[i] = 0;
        }
    }
    for(int i=chute4StartIndex; i<chute4StartIndex+struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitCount; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[3].nUnitId[i-chute4StartIndex] = i;
    }

    /* 设置识别组参数 */
    for(int i=0; i<TYPE_MAX_IDETIFY_GROUP; i++){
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nLevelIndex = 0;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nNameIndex = i;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitCount = struCnfg.struLevelInfo[0].struTickGroupInfo[i/2].nUnitCount/2;

        for(int j=0; j<struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitCount; j++){
            if(i%2 == 0){
                struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitId[j] = struCnfg.struLevelInfo[0].struTickGroupInfo[i/2].nUnitId[j*2];
                struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitSplice[j] = struCnfg.struLevelInfo[0].struTickGroupInfo[i/2].nUnitSplice[j*2];
            } else{
                struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitId[j] = struCnfg.struLevelInfo[0].struTickGroupInfo[i/2].nUnitId[j*2+1];
                struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitSplice[j] = struCnfg.struLevelInfo[0].struTickGroupInfo[i/2].nUnitSplice[j*2+1];
            }
        }
    }
    printMachineSet();
}

/*!
 * \brief 设置LD2机型组参数 
 */
void setMachineMode::setLD2GroupParams()
{
    setCFGroupParams();

    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nNameIndex = FIRST_UPPER;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nNameIndex = FIRST_DOWN;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nNameIndex = SECOND_UPPER;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nNameIndex = SECOND_DOWN;
}

/*!
 * \brief 设置LD4机型组参数
 */
void setMachineMode::setLD4GroupParams()
{
    struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;
    m_nChutePerLevel[0] = 0;
    for(int i = 0; i < 2; i++){ //最多2个剔除组
        m_nChutePerLevel[0] += m_nChutePerTickGroup[0][i];
    }
    struCnfg.struLevelInfo[0].nUnitLevelTotal = m_nChutePerLevel[0] * 4 + 2;

    /*相机编号*/
    for(int i = 0; i < MAX_UNIT; i++){
        struCnfg.struLevelInfo[0].nUnitId[i] = 0;
    }
    for(int i = 0; i < (struCnfg.struLevelInfo[0].nUnitLevelTotal + 2) / 2; i++){
        struCnfg.struLevelInfo[0].nUnitId[i] = i;
    }

    /*识别组参数*/
    if ((int)m_nChutePerTickGroup[0][1] > 0) {  //两次
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitCount = m_nChutePerTickGroup[0][0] + 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][0] + 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[4].nUnitCount = m_nChutePerTickGroup[0][1] + 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[5].nUnitCount = m_nChutePerTickGroup[0][1] + 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nUnitCount = m_nChutePerTickGroup[0][0];
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nUnitCount = m_nChutePerTickGroup[0][0];
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[6].nUnitCount = m_nChutePerTickGroup[0][1];
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[7].nUnitCount = m_nChutePerTickGroup[0][1];
    } else {    //只有一次
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitCount = m_nChutePerTickGroup[0][0] + 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][0] + 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nUnitCount = m_nChutePerTickGroup[0][0];
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nUnitCount = m_nChutePerTickGroup[0][0];
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[4].nUnitCount = 0;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[5].nUnitCount = 0;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[6].nUnitCount = 0;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[7].nUnitCount = 0;
    }


    /*识别组内相机编号*/
    /***识别组信息示意：
     *   4  6  ->4个
     *   0  2  ->5个
     *   5  7  ->4个
     *   1  3  ->5个
    */
    //group 0
    for (int i = 0 ; i < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitCount;i++) {
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitId[i] = i * 2;
    }
    //group 1
    for (int i = 0 ; i < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitCount;i++) {
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitId[i] = i * 2 + 1;
    }
    //group 4
    for (int i = 0 ; i < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[4].nUnitCount;i++) {
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[4].nUnitId[i] = i * 2
                + struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitId[struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitCount - 1];
    }
    //group 5
    for (int i = 0 ; i < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[5].nUnitCount;i++) {
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[5].nUnitId[i] = i * 2
                + struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitId[struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitCount - 1];
    }
    //group 6
    for (int i = 0 ; i < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[6].nUnitCount;i++) {
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[6].nUnitId[i] = i * 2 + 2
                + struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nUnitId[struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nUnitCount - 1];
    }
    //group 7
    for (int i = 0 ; i < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[7].nUnitCount;i++) {
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[7].nUnitId[i] = i * 2 + 2
                + struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nUnitId[struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nUnitCount - 1];
    }

    /*识别组内分割相机编号分配*/
    for (int i =0; i < struCnfg.struLevelInfo[0].nIdentifyGroupTotal; i++) {
        for (int j =0; j < struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitCount; j++) {
            struCnfg.struLevelInfo[0].struIdentifyGroupInfo[i].nUnitSplice[j] = 0;
        }
    }
    int identitySpliceTmp = m_nChutePerTickGroup[0][0];
    if ((int)m_nChutePerTickGroup[0][0] < (int)m_nChutePerLevel[0]) {/*考虑只有一次情况:无分割相机*/
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nUnitSplice[identitySpliceTmp] = 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nUnitSplice[identitySpliceTmp] = 1;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nUnitSplice[0] = 2;
        struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nUnitSplice[0] = 2;
    }


    // 设置剔除组参数
    for(int i = 0; i < struCnfg.struLevelInfo[0].nTickGroupTotal; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[i].nLevelIndex = 0;
        struCnfg.struLevelInfo[0].struTickGroupInfo[i].nNameIndex = i;
    }

    // 设置剔除组的相关参数
    struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount = m_nChutePerTickGroup[0][0] * 4 + 2;
    struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[0][1] * 4 + 2;

    /*剔除组0内相机编号分配*/
    for(int i = 0; i < struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount / 2 + 1; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitId[i] = i;
    }

    /*剔除组1内相机编号分配*/
    for(int i = 0; i < struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount / 2 + 1; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitId[i] = i + struCnfg.struLevelInfo[0].struIdentifyGroupInfo[4].nUnitId[0];
    }
    for(int i = 0; i < struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount / 2 - 1; i++) {
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitId[i + struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount / 2 + 1]
                = i + struCnfg.struLevelInfo[0].struIdentifyGroupInfo[6].nUnitId[0];
    }

    /*剔除组0内分割相机编号分配*/
    for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[i] = 0;
    }
    if ((int)m_nChutePerTickGroup[0][0] < (int)m_nChutePerLevel[0]) {/*考虑只有一次情况*/
        int tickSpliceTmp = struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitCount / 2;
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[tickSpliceTmp - 1] = 1;
        struCnfg.struLevelInfo[0].struTickGroupInfo[0].nUnitSplice[tickSpliceTmp] = 1;
    }

    /*剔除组1内分割相机编号分配*/
    for(int i=0; i<struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitCount; i++){
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[i] = 0;
    }
    if (m_nChutePerTickGroup[0][0] < m_nChutePerLevel[0]) {/*考虑只有一次情况*/
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[0] = 2;
        struCnfg.struLevelInfo[0].struTickGroupInfo[1].nUnitSplice[1] = 2;
    }

    printMachineSet();
}

/*!
 * \brief 设置LDS机型的组参数 
 */
void setMachineMode::setLDSGroupParams()
{
    for(int i=0; i<struCnfg.nLevelTotal; i++){
        struCnfg.struLevelInfo[i].nUnitLevelTotal = 4;
        for(int j=0; j<struCnfg.struLevelInfo[i].nUnitLevelTotal; j++){
            struCnfg.struLevelInfo[i].nUnitId[j] = 2*j;
        }
    }

    // 设置剔除组参数
    for(int i=0; i<struCnfg.nLevelTotal; i++){
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nLevelIndex = i;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = UNIT_NULL;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 4;
        for(int j=0; j<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount; j++){
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[j] = 2*j;
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[j] = 0;
        }
    }

    // 设置识别组参数
    for(int i=0; i<struCnfg.nLevelTotal; i++){
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nLevelIndex = i;
	struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = UNIT_NULL;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 4;
        for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount; j++){
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[j] = 2*j;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitSplice[j] = 0;
        }
    }
}

/*!
 * \brief 设置LDS2机型组参数 
 */
void setMachineMode::setLDS2GroupParams()
{
    int i, j;
    quint8 chute2StartIndex;        // 记录二次色选的起始板号
    bool chute1IsSplice = false;    // 记录一次色选是否有分隔的通道

    struCnfg.nLevelTotal = m_nLevelTotal;    // 双层
    struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[0].nNameIndex = FIRST_UP_FRONT_ML;//一次上前
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[1].nNameIndex = FIRST_UP_REAR_ML;//一次上后
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[2].nNameIndex = SECOND_UP_FRONT_ML;//二次上前
    struCnfg.struLevelInfo[0].struIdentifyGroupInfo[3].nNameIndex = SECOND_UP_REAR_ML;//二次上后
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[0].nNameIndex = FIRST_DOWN_FRONT_ML;//一次下前
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[1].nNameIndex = FIRST_DOWN_REAR_ML;//一次下后
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[2].nNameIndex = SECOND_DOWN_FRONT_ML;//二次下前
    struCnfg.struLevelInfo[1].struIdentifyGroupInfo[3].nNameIndex = SECOND_DOWN_REAR_ML;//二次下后

    struCnfg.struLevelInfo[0].struTickGroupInfo[0].nNameIndex = FIRST_UP_UNIT;
    struCnfg.struLevelInfo[0].struTickGroupInfo[1].nNameIndex = SECOND_UP_UNIT;
    struCnfg.struLevelInfo[1].struTickGroupInfo[0].nNameIndex = FIRST_DOWN_UNIT;
    struCnfg.struLevelInfo[1].struTickGroupInfo[1].nNameIndex = SECOND_DOWN_UNIT;

    for(i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfg.struLevelInfo[i].nUnitLevelTotal = m_nChutePerLevel[i]*2;
        for (j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            struCnfg.struLevelInfo[i].nUnitId[j] = j;
        }
        struCnfg.struLevelInfo[i].nViewTotal = 2; // 双视

        /* 设置剔除组0的相关参数 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nLevelIndex = i;
        if(((int)(m_nChutePerTickGroup[i][0]*2)%2) == 1) {
            chute1IsSplice = true;
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = m_nChutePerTickGroup[i][0]*2+1;
            for(int k=0; k<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount-2; k++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[k] = 0;
            }
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount-2] = 1;
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount-1] = 1;
            chute2StartIndex = struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount-2;
        } else{
            chute1IsSplice = false;
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = m_nChutePerTickGroup[i][0]*2;
            for(int k=0; k<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount; k++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[k] = 0;
            }
            chute2StartIndex = struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount;
        }
        for(int k=0; k<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount; k++){
            struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[k] = k;
        }

        /* 设置剔除组1的相关参数 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nLevelIndex = i;
        if(chute1IsSplice){
            struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitSplice[0] = 2;
            struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitSplice[1] = 2;
            struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[i][1]*2+1;
            for(int k=2; k<struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount; k++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitSplice[k] = 0;
            }
        } else {
            struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = m_nChutePerTickGroup[i][1]*2;
            for(int k=0; k<struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount; k++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitSplice[k] = 0;
            }
        }
        for(int k=chute2StartIndex; k<chute2StartIndex+struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount; k++){
            struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[k-chute2StartIndex] = k;
        }

        /* 设置识别组参数 */
        for(int k=0; k<4; k++){
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitCount = struCnfg.struLevelInfo[i].struTickGroupInfo[k/2].nUnitCount/2;

            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitCount; j++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitSplice[j] = 0;
                if(k%2 == 0){
                    struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitId[j] = struCnfg.struLevelInfo[i].struTickGroupInfo[k/2].nUnitId[j*2];
                    struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitSplice[j] = struCnfg.struLevelInfo[i].struTickGroupInfo[k/2].nUnitSplice[j*2];
                } else {
                    struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitId[j] = struCnfg.struLevelInfo[i].struTickGroupInfo[k/2].nUnitId[j*2+1];
                    struCnfg.struLevelInfo[i].struIdentifyGroupInfo[k].nUnitSplice[j] = struCnfg.struLevelInfo[i].struTickGroupInfo[k/2].nUnitSplice[j*2+1];
                }
            }
        }
    }
    printMachineSet();
}

/*!
 * \brief 设置TD机型的组参数 
 */
void setMachineMode::setTDGroupParams()
{
    struCnfg.nLevelTotal = m_nLevelTotal;
    struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;
    for(int i=0; i<struCnfg.nLevelTotal; i++){
        if (struCnfg.struLevelInfo[i].nViewTotal == 2) {
            struCnfg.struLevelInfo[i].nUnitLevelTotal = m_nChutePerLevel[i]*2;
            for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
                struCnfg.struLevelInfo[i].nUnitId[j] = j;
            }
        }
        else {
            struCnfg.struLevelInfo[i].nUnitLevelTotal = m_nChutePerLevel[i];
            for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
                struCnfg.struLevelInfo[i].nUnitId[j] = j*2;
            }
        }

    }

    // 设置剔除组参数
    for(int i=0; i<struCnfg.nLevelTotal; i++){
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nLevelIndex = i;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = UNIT_NULL;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal;
        if(struCnfg.struLevelInfo[i].nViewTotal == 2){
            for(int j=0;j<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount;j++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[j] = j;
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[j] = 0;
            }
        }
        else{
            for(int j=0;j<struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount;j++){
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[j] = j*2;
                struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitSplice[j] = 0;
            }
        }
     }


    // 设置识别组参数
    for(int i=0; i<struCnfg.nLevelTotal; i++){
        if(struCnfg.struLevelInfo[i].nViewTotal == 2)
        {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FRONT_UNIT;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = REAR_UNIT;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal/2;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal/2;
            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount; j++)
            {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[j] = j*2;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitSplice[j] = 0;
            }
            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount; j++)
            {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[j] = j*2+1;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitSplice[j] = 0;
            }
        }
        else {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nLevelIndex = i;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = UNIT_NULL;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = UNIT_NULL;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = struCnfg.struLevelInfo[i].nUnitLevelTotal;
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 0;
            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount; j++){
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[j] = 2*j;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitSplice[j] = 0;
            }
            for(int j=0; j<struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount; j++){
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[j] = 2*j;
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitSplice[j] = 0;
            }
        }
    }
}

/*!
 * \brief 设置T2机型的组参数 
 */
void setMachineMode::setT2GroupParams()
{
    struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;
    struCnfg.nLevelTotal = m_nLevelTotal;
    for(int i = 0;i <struCnfg.nLevelTotal; i++)
    {
        /* 识别组通道信息 */
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = FIRST_FRONT;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nNameIndex = FIRST_REAR;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[1].nUnitId[1] = 3;

        for(int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = FRONT_UNIT;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nNameIndex = REAR_UNIT;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitCount = 2;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 0;
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[1] = 2;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[0] = 1;
        struCnfg.struLevelInfo[i].struTickGroupInfo[1].nUnitId[1] = 3;

        for(int j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/*!
 * \brief 设置T1机型的组参数
 */
void setMachineMode::setT1GroupParams()
{
    struCnfg.nLevelTotal = m_nLevelTotal;
    struCnfg.nEjectorsPerChute = m_nEjectNumPerBoard;
    for(int i = 0; i < struCnfg.nLevelTotal; i++) {
        /* 识别组 */
        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nNameIndex = UNIT_NULL;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struIdentifyGroupInfo[0].nUnitId[0] = 1;

        for(int j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitSplice[k] = 0;
            }
        }

        /* 剔除组通道信息 */
        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nNameIndex = UNIT_NULL;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitCount = 1;

        struCnfg.struLevelInfo[i].struTickGroupInfo[0].nUnitId[0] = 1;

        for(int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfg.struLevelInfo[i].struTickGroupInfo[j].nLevelIndex = i;
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitSplice[k] = 0;
            }
        }
    }
}

/*!
 * \brief 输出配置结果 - 辅助调试
 */
void setMachineMode::printMachineSet()
{
    if (DEBUG_OUTPUT) {
        for (int k =0; k < m_nLevelTotal; k++) {
            printf("curren level %d:\n",k);
            for (int i=0; i < MAX_GROUP_IDTNTIFY; i++) {
                printf("id group %d:\n",i);
                for (int j = 0; j < struCnfg.struLevelInfo[k].struIdentifyGroupInfo[i].nUnitCount; j++) {
                    printf("%d ",struCnfg.struLevelInfo[k].struIdentifyGroupInfo[i].nUnitId[j]);
                }
                printf("\n");
                printf("id group splice %d:\n",i);
                for (int j = 0; j < struCnfg.struLevelInfo[k].struIdentifyGroupInfo[i].nUnitCount; j++) {
                    printf("(%d %d) ",j,struCnfg.struLevelInfo[k].struIdentifyGroupInfo[i].nUnitSplice[j]);
                }
                printf("\n");
            }
            for (int i=0; i < MAX_GROUP_TICK; i++) {
                printf("tick group %d:\n",i);
                for (int j = 0; j < struCnfg.struLevelInfo[k].struTickGroupInfo[i].nUnitCount; j++) {
                    printf("%d ",struCnfg.struLevelInfo[k].struTickGroupInfo[i].nUnitId[j]);
                }
                printf("\n");
                printf("tick group splice%d:\n",i);
                for (int j = 0; j < struCnfg.struLevelInfo[k].struTickGroupInfo[i].nUnitCount; j++) {
                    printf("(%d %d) ",j,struCnfg.struLevelInfo[k].struTickGroupInfo[i].nUnitSplice[j]);
                }
                printf("\n");
            }
            printf("chute num :%f,%f,%f\n",m_nChutePerTickGroup[k][0],m_nChutePerTickGroup[k][1],m_nChutePerTickGroup[k][2]);
        }
        fflush(0);
    }
}


/*!
 * \brief setMachineMode::onViewBoxClicked
 * 响应机型视角、光源类型配置操作
 * \param index
 */
void setMachineMode::onViewBoxClicked(int index)
{
    Q_UNUSED(index);
    // 仅 TD/SD 机型支持视角切换，已废弃
}

/*!
 * \brief setMachineMode::onTDViewBoxClicked
 * 响应TD系机型视角、光源配置操作
 * \param index
 */
void setMachineMode::onTDViewBoxClicked(int index)
{
    // 切换当前视角类型
    if (viewBox[index]->isChecked()) {
        m_nViewTotal[index] = DOUBLE_VIEW;
    } else {
        m_nViewTotal[index] = SINGLE_VIEW;
    }
    m_nViewState[index]++;
}

/*!
 * \brief setMachineMode::isViewStateChanged
 * 视角状态是否发生变化
 */
bool setMachineMode::isViewStateChanged()
{
    for (int i = 0; i < m_nLevelTotal; i++) {
        if ((m_nViewState[i]%2) != 0) {
            return true;
        }
    }
    return false;
}

/*!
 * \brief setMachineMode::resetLampParams
 * 机型下重置灯控参数
 * \param nTotal
 */
void setMachineMode::resetLampParams(int nTotal)
{
    Q_UNUSED(nTotal);
}

/*!
 * \brief  setMachineMode::resetLedMode
 * 重置LED指示灯模式
 */
void setMachineMode::resetLedMode()
{
    if (ledModeCb->isChecked()) {
        struCnfg.nLedMode = 1;
    } else {
        struCnfg.nLedMode = 0;
    }
}

/*!
 * \brief setMachineMode::resetRunMode
 * 重置屏运行模式设定
 */
void setMachineMode::resetRunMode()
{
    int curIndex = runModeBox->currentIndex();
    struCnfg.nRunMode = runModeBox->itemData(curIndex, Qt::UserRole).toInt();
}

/*!
 * \brief setMachineMode::getMachineTypeList
 * 获取机型选择列表
 */
QList<int> setMachineMode::getMachineTypeList()
{
    // 构造机型列表 - 仅保留 CF
    QList<int> typeList;
    typeList << MAC_TYPE_CF;
    return typeList;
}

/*!
 * \brief setMachineMode::getMachineModeList
 * 获取机型规格列表
 * \param nTypeIndex
 */
QStringList setMachineMode::getMachineModeList(int nTypeIndex)
{
    Q_UNUSED(nTypeIndex);

    QString modePrefix = "6SXZ-"; // 内控型号前缀
    QString modeSuffix = "F";     // 内控型号后缀
    int modeNum = machineModeNum; // 规格列表数量

    // 构造机型规格列表 - CF 走默认路径
    QStringList modeList;
    for (int i = 0; i < modeNum; i++) {
        QString text = QString("%1. %2%3%4").arg(i+1).arg(modePrefix).arg((i+1)*m_nEjectNumPerBoard).arg(modeSuffix);
        modeList << text;
    }

    return modeList;
}
/*!
 * \brief setMachineMode::getMachineModeConfig
 * 更新机器规格配置
 */
glMachineModeConfig setMachineMode::getMachineModeConfig()
{
    glMachineModeConfig config;
    int nTypeIndex = MAC_TYPE_CF;
    int nModeIndex = m_nChutePerLevel[ONE_LEVEL]-1;
    int nTickPerLevel = 3;
    int nEjectStep = 1;
    float nChuteRatio = 0.5;
    bool bEnableEject = true;
    bool bEnableView = false;
    bool bEnableLED = false;
    bool bEnableRunMode = true;

    // CG 机型使用 4 通道
    if (m_nCGMachineMode) {
        nTickPerLevel = 4;
    }

    config.qlTypeList = getMachineTypeList();
    config.nTypeIndex = nTypeIndex;
    config.qlModeList = getMachineModeList(nTypeIndex);
    config.nModeIndex = nModeIndex;
    config.nTickPerLevel = nTickPerLevel;
    config.nChuteRatio = nChuteRatio;
    config.nEjectStep = nEjectStep;
    config.bEnableEject = bEnableEject;
    config.bEnableView = bEnableView;
    config.bEnableLED = bEnableLED;
    config.bEnableRunMode = bEnableRunMode;
    
    return config;
}

/*!
 * \brief setMachineMode::createMachineTypeMap
 * 创建机型映射列表
 */
void setMachineMode::createMachineTypeMap()
{
    if (!qMachineTypeMap.isEmpty()) {
        qMachineTypeMap.clear();
    }

    qMachineTypeMap.insert(MAC_TYPE_CF, "CF");
}

/*!
 * \brief setMachineMode::getMachineTypeName
 * 根据机型索引获取机型名称
 * \param map
 * \param index
 * \return
 */
QString setMachineMode::getMachineTypeName(const QMap<int, QString> &map, int index)
{
    return map.value(index);
}
