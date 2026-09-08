/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        manageengineer.cpp
 * \brief       工程模式管理界面源文件
 * \date        2015.01.14
 */
#include "manageengineer.h"

/**
 * @brief manageEngineer::manageEngineer
 * 工程模式构造函数
 * @param parent
 */
manageEngineer::manageEngineer(QWidget *parent) :
    QWidget(parent)
{
    //! 页面显示尺寸
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-100);
    config = g_Config::getInstance();
    margin = 25;

    //! 构造页面容器
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);

    //! 工程模式管理页面
    createManageEngineerPage();
    //! 机型设置页面
    createSetProductPage();
    //! 语言设置页面
    createSetLanguagePage();
    //! 延迟付款页面
    createDelayPaymentPage();
    //! 豆类定制页面 - 已废弃
    if(struCnfe.nEnableBeanMachine == 1) {
        createSetBeanPage();
    }

    //! 信号 & 槽
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetIndexChangedSlt(int)));
}

/*******************************************************************************
 *
 *                          工程模式管理主页面函数
 *
********************************************************************************/
/**
 * @brief manageEngineer::createManageEngineerPage
 * 创建工程模式管理主页面
 */
void manageEngineer::createManageEngineerPage()
{
    manageEngineerWidget = new QWidget(stackedWidget);
    manageEngineerWidget->setFixedSize(stackedWidget->width(), stackedWidget->height());

    //! 内置公司标识信息（名称、网址）
    QString str = QString("%1\n%2").arg("   HEFEI MEYER OPTOELECTRONIC TECHNOLOGY INC.")
            .arg("   www.chinameyer.com");
    myLabel *label = new myLabel(str);
    label->setFixedHeight(BTN_HEIGHT*2);

    //! 页面功能按钮
    productBtn = new myPushButton("设置机型", QIcon(), true, true);
    languageBtn = new myPushButton("设置语言", QIcon(), true, true);
    limitBtn = new myPushButton("设置期限", QIcon(), true, true);
    setBeanBtn = new myPushButton("豆类机型定制", QIcon(), true, true);
    customLanguageBtn = new myPushButton("定制标题", QIcon(), true, true);
    backBtn = new myPushButton("返回", myIcon.Action_Back, true, true);

    productBtn->setFixedSize(QSize(PAGE_BTN_WIDTH, PAGE_BTN_HEIGHT));
    languageBtn->setFixedSize(QSize(PAGE_BTN_WIDTH, PAGE_BTN_HEIGHT));
    limitBtn->setFixedSize(QSize(PAGE_BTN_WIDTH, PAGE_BTN_HEIGHT));
    setBeanBtn->setFixedSize(QSize(PAGE_BTN_WIDTH, PAGE_BTN_HEIGHT));
    customLanguageBtn->setFixedSize(QSize(PAGE_BTN_WIDTH, PAGE_BTN_HEIGHT));
    backBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    //! 页面布局
    mainVbLayout = new QVBoxLayout(manageEngineerWidget);
    upHbLayout = new QHBoxLayout;
    downHbLayout = new QHBoxLayout;
    upGrid = new QGridLayout;

    upGrid->setSpacing(120);
    upGrid->setContentsMargins(20, 20, 30, 10);

    upGrid->addWidget(productBtn ,0 ,0 ,1 ,1);
    upGrid->addWidget(languageBtn,0 ,1 ,1 ,1);
    upGrid->addWidget(limitBtn   ,1 ,0 ,1 ,1);
    upGrid->addWidget(customLanguageBtn, 1, 1, 1, 1);

    upHbLayout->addLayout(upGrid);
    downHbLayout->setContentsMargins(10, 10, 20, 10);
    downHbLayout->addStretch();
    downHbLayout->addWidget(backBtn);
    mainVbLayout->addWidget(label);
    mainVbLayout->addLayout(upHbLayout);
    mainVbLayout->addLayout(downHbLayout);

    stackedWidget->addWidget(manageEngineerWidget);

    /* 槽函数 */
    connect(productBtn,SIGNAL(pressed()),this,SLOT(onProductBtnClickedSlt()));
    connect(languageBtn,SIGNAL(pressed()),this,SLOT(onLanguageBtnClickedSlt()));
    connect(limitBtn,SIGNAL(pressed()),this,SLOT(onLimitBtnClickedSlt()));
    connect(setBeanBtn,SIGNAL(pressed()),this,SLOT(onSetBeanBtnClickedSlt()));
    connect(customLanguageBtn,SIGNAL(pressed()),this,SLOT(onCustomLanguageBtnClickedSlt()));
    connect(backBtn,SIGNAL(pressed()),this,SLOT(onBackBtnClckedSlt()));
}

/**
 * @brief manageEngineer::onProductBtnClickedSlt
 * 进入机型设置界面
 */
void manageEngineer::onProductBtnClickedSlt()
{
    updateProduct();
    stackedWidget->setCurrentIndex(Page_Product);
}

/**
 * @brief manageEngineer::onLanguageBtnClickedSlt
 * 进入语言设置界面
 */
void manageEngineer::onLanguageBtnClickedSlt()
{
    updateLangList();
    stackedWidget->setCurrentIndex(Page_Language);
}

/**
 * @brief manageEngineer::onLimitBtnClickedSlt
 * 进入延迟付款界面
 */
void manageEngineer::onLimitBtnClickedSlt()
{
    myEncrypt dlg;
    bool bFlagEna = true;

    //! 加密状态判断，用于决定后续是否启用定时参数保存功能
    if (paramDelayCode.ena != 0xAA) {
        bFlagEna = true;
    } else {
        bFlagEna = false;
    }

//    qDebug()<<"bFlagEna:"<<bFlagEna<<flush;

    paramDelayCode.m_nEnableDelayCodeInfo = struCnfg.nEnableDelayCodeInfo;
    int ret = dlg.exec();
    if (ret == QDialog::Accepted) {
        if (bFlagEna) {
           /* 启动定时保存参数功能 */
           QProcess *process = new QProcess;
           QStringList strList;
           strList<<"";
//           process->startDetached("./MyTimer",strList);
        }
    }

    struCnfg.nEnableDelayCodeInfo = paramDelayCode.m_nEnableDelayCodeInfo;

    stackedWidget->setCurrentIndex(Page_ManageEngineer);
}

/**
 * @brief manageEngineer::onBackBtnClckedSlt
 * 退出工程模式页面
 */
void manageEngineer::onBackBtnClckedSlt()
{
    emit sUpdateSchemeList();           // 更新预制方案列表
    emit sUpdateArithmeticList();       // 更新算法列表
    emit goToHomePageSig();
}

/*******************************************************************************
 *
 *                          机型设置页面函数
 *
********************************************************************************/
void manageEngineer::createSetProductPage()
{
    productWidget = new QWidget;
    productWidget->setGeometry(0,0,stackedWidget->width(),stackedWidget->height());

    nProduct = struCnfe.nProduct;
    isBeanEnable = struCnfe.nEnableBeanMachine;

    productList = new QListWidget;
    productList->setIconSize(QSize(ICON_WID, ICON_HEI));
    productList->setMinimumWidth(500);

    for(int i = 0; i < machineTotal; i++){
        machineItem[i] = new QListWidgetItem;
        machineItem[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        machineItem[i]->setSizeHint(QSize(50, 50));
        productList->addItem(machineItem[i]);
    }

    machineItem[0]->setText("通用机型");
    machineItem[1]->setText("花生机型");
    machineItem[2]->setText("西瓜籽机型");
    machineItem[3]->setText("豆类定制机型");
    machineItem[6]->setText("大蒜定制机型");
    machineItem[7]->setText("糖果定制机型");
    machineItem[8]->setText("开心果定制机型");
    machineItem[9]->setText("黑枸杞定制机型");

    productSureBtn = new myPushButton("确定", myIcon.Action_Apply);
    productSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    productCancelBtn = new myPushButton("取消", myIcon.Action_Cancel);
    productCancelBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QHBoxLayout *productSetLayout = new QHBoxLayout;
    productSetLayout->addWidget(productSureBtn);
    productSetLayout->addStretch();
    productSetLayout->addWidget(productCancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(productList);
    mainLayout->addLayout(productSetLayout);

    productWidget->setLayout(mainLayout);
    stackedWidget->addWidget(productWidget);

    connect(productCancelBtn,SIGNAL(pressed()),this,SLOT(onProductCancelBtnClicked()));
    connect(productSureBtn,SIGNAL(pressed()),this,SLOT(onPoductSureBtnClicked()));
    connect(productList,SIGNAL(currentRowChanged(int)),this,SLOT(onProductListChanged(int)));
}

void manageEngineer::onProductCancelBtnClicked()
{
    nProduct = struCnfe.nProduct;
    isBeanEnable = struCnfe.nEnableBeanMachine;
    stackedWidget->setCurrentIndex(Page_ManageEngineer);
}

void manageEngineer::onPoductSureBtnClicked()
{
    if((struCnfe.nProduct == nProduct)
            && struCnfe.nEnableBeanMachine == isBeanEnable
) {
        stackedWidget->setCurrentIndex(Page_ManageEngineer);
		return;
	}


    //! 提示用户需清除参数且重启
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, "切换机型需清除参数且重启，\n确认执行该操作？");
    if (mesBox->exec() == QDialog::Rejected) {
        return;
    }

    int i, j;

    char cmd[100];
    snprintf(cmd, sizeof(cmd),"%s", "mv -f userdata/cnf/cnf.vpn-auth userdata/cnf/cnf.network /userdata");
    system(cmd);

    snprintf(cmd, sizeof(cmd), "%s", "rm -rf userdata/cnf/*");
    system(cmd);

    snprintf(cmd, sizeof(cmd), "%s", "mv -f userdata/cnf.vpn-auth userdata/cnf.network userdata/cnf");
    system(cmd);

    struCnfe.nProduct = nProduct;
    struCnfe.nEnableMaizeDoubleView = 0;
    struCnfe.nEnableBeanMachine = isBeanEnable;
    struCnfe.nEnableHGQMachine=0;
    for (i = 0; i < ARITHMETIC_TOTAL; i++) {
        struCnfe.nArithmeticSeq[i] = 0;
    }

    /* 算法定制 */
    struCnfe.nArithmeticSeq[ARITH_GREY_A]       = 1;
    struCnfe.nArithmeticSeq[ARITH_GREY_B]       = 1;
    struCnfe.nArithmeticSeq[ARITH_DISCOLOR_A]   = 1;
    struCnfe.nArithmeticSeq[ARITH_DISCOLOR_B]   = 1;
    struCnfe.nArithmeticSeq[ARITH_CROSS]        = 1;
    struCnfe.nArithmeticSeq[ARITH_SHAPE]        = 1;
    struCnfe.nArithmeticSeq[ARITH_SCALE]        = 1;
    struCnfe.nArithmeticSeq[ARITH_SHAPE_LENGTH] = 1;
    struCnfe.nArithmeticSeq[ARITH_BUD_1]        = 1;
    struCnfe.nArithmeticSeq[ARITH_BUD_2]        = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_A]      = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_B]      = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_C]      = 1;
    struCnfe.nArithmeticSeq[ARITH_INTEL_D]      = 1;
    struCnfe.nArithmeticSeq[ARITH_SCALE_B]      = 1;
    struCnfe.nArithmeticSeq[ARITH_PISTACHIO]    = 1;

    /* 预制方案定制 */
    for (i = 0; i < MAX_PARENT_PREFIXED_PROFILE_COUNT; i++) {
        struCnfe.struParentPrefixedProfile[i].nIsAble = 0;
    }
    struCnfe.nParentPrefixedProfileTotal = 1;
    struCnfe.struParentPrefixedProfile[0].nSonProfileCount = 1;     // 普通通用类
    struCnfe.struParentPrefixedProfile[1].nSonProfileCount = 8;     // 综合类
    struCnfe.struParentPrefixedProfile[2].nSonProfileCount = 7;     // 豆类
    struCnfe.struParentPrefixedProfile[3].nSonProfileCount = 5;     // 花生类
    struCnfe.struParentPrefixedProfile[4].nSonProfileCount = 7;     // 籽仁类

    for (i = 0; i < struCnfe.nParentPrefixedProfileTotal; i++) {
        struCnfe.struParentPrefixedProfile[i].nIsAble = 1;
        for (j = 0; j < struCnfe.struParentPrefixedProfile[i].nSonProfileCount; j++) {
            struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble = 1;
        }
    }

    myFlow.getPrefixedName();
    myFlow.saveEngineer();
    system("reboot");
}

/* 响应机型设置索引改变：当前只保留通用机型 */
void manageEngineer::onProductListChanged(int index)
{
    Q_UNUSED(index);
    isBeanEnable = 0;
    nProduct = OPT_PRODUCT_ANCOO_GENERAL;
}

/*!
 * \brief 响应TD机型列表索引变化
 */
void manageEngineer::onTeaProductListChanged(void)
{
    int nIndex = productList->currentRow();
    switch (nIndex) {
    case 0: //! 通用机型
        //! 若机型未修改则直接退出
        if (struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_A] == 0) {
            return;
        }
        struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_A] = 0;
        struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_B] = 0;
        struCnfe.nArithmeticSeq[ARITH_SHAPE_SLICE] = 0;
        struCnfe.nArithmeticSeq[ARITH_SHAPE_LEAF] = 0;
        break;
    case 1: //! 大红袍定制机型
        //! 若机型未修改则直接退出
        if (struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_A] == 1) {
            return;
        }
        struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_A] = 1;
        struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_B] = 1;
        struCnfe.nArithmeticSeq[ARITH_SHAPE_SLICE] = 1;
        struCnfe.nArithmeticSeq[ARITH_SHAPE_LEAF] = 1;
        break;
    default:
        break;
    }

    char cmd[100];
    snprintf(cmd, sizeof(cmd),"%s", "mv -f userdata/cnf/cnf.vpn-auth userdata/cnf/cnf.network userdata");
    system(cmd);

    snprintf(cmd, sizeof(cmd), "%s", "rm -rf userdata/cnf/*");
    system(cmd);

    snprintf(cmd, sizeof(cmd), "%s", "mv -f userdata/cnf.vpn-auth userdata/cnf.network userdata/cnf");
    system(cmd);

    myFlow.saveEngineer();

    system("reboot");
}

/* 机型设置页面update  */
void manageEngineer::updateProduct()
{
    if(struCnfe.nEnableBeanMachine == 1) {
        productList->setCurrentRow(3);
    } else {
        productList->setCurrentRow(0);
    }
}

/*!
 * \brief 更新茶叶机型列表
 */
void manageEngineer::updateTeaProduct()
{
    if (struCnfe.nArithmeticSeq[ARITH_SHAPE_POLE_A] == 0) {
        productList->item(0)->setBackground(QBrush(QColor(172,144,75)));
    } else {
        productList->item(1)->setBackground(QBrush(QColor(172,144,75)));
    }
}


/*******************************************************************************
 *
 *                          语言设置页面函数
 *
********************************************************************************/
/**
 * @brief manageEngineer::createSetLanguagePage
 * 创建语言设置界面
 */
void manageEngineer::createSetLanguagePage()
{
    //! 初始化成员参数
    langWidget = new QWidget;
    langWidget->setGeometry(0, 0, stackedWidget->width(), stackedWidget->height());

    //! 支持语言列表
    allLangListWidget = new QListWidget;
    allLangListWidget->setIconSize(QSize(ICON_WID, ICON_HEI));

    allLangList = myFlow.getLanguageList();
    for (int i = 0; i < allLangList.count(); i++) {
        int index = allLangList.at(i);
        allLangListWidget->addItem(myFlow.getLanguageListName(index));
        allLangListWidget->item(i)->setFont(config->getFont(DEFAULT_FONT_SIZE));
        allLangListWidget->item(i)->setSizeHint(QSize(BTN_HEIGHT, BTN_HEIGHT));
    }

    //! 选择语言列表
    selLangListWidget = new QListWidget;
    selLangListWidget->setIconSize(QSize(ICON_WID, ICON_HEI));

    selLangList.clear();
    for (int i = 0; i < struCnfe.nLangList[0]; i++) {
        selLangList.append(struCnfe.nLangList[i+1]);
    }
    //! 内置中文支持
    if (selLangList.count() == 0) {
        selLangList << LANG_CHS;
    }
    for (int i = 0; i < selLangList.count(); i++) {
        int index = selLangList.at(i);
        selLangListWidget->addItem(myFlow.getLanguageListName(index));
        selLangListWidget->item(i)->setFont(config->getFont(DEFAULT_FONT_SIZE));
        selLangListWidget->item(i)->setSizeHint(QSize(BTN_HEIGHT, BTN_HEIGHT));
    }

    //! 添加 & 删除
    langAddBtn = new myPushButton("添加", myIcon.Arrow_Right);
    langAddBtn->setIconSize(QSize(ICON_WID, ICON_HEI));
    langAddBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    langDelBtn = new myPushButton("移除", myIcon.Arrow_Left);
    langDelBtn->setIconSize(QSize(ICON_WID, ICON_HEI));
    langDelBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    //! 确定 & 取消
    langSureBtn = new myPushButton("确定", myIcon.Action_Apply);
    langSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    langCancelBtn = new myPushButton("取消", myIcon.Action_Cancel);
    langCancelBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QVBoxLayout *langSetLayout = new QVBoxLayout;
    langSetLayout->addStretch();
    langSetLayout->addWidget(langAddBtn);
    langSetLayout->addWidget(langDelBtn);
    langSetLayout->addStretch();

    QHBoxLayout *langListLayout = new QHBoxLayout;
    langListLayout->addWidget(allLangListWidget);
    langListLayout->addLayout(langSetLayout);
    langListLayout->addWidget(selLangListWidget);

    QHBoxLayout *langBoxLayout = new QHBoxLayout;
    langBoxLayout->addWidget(langSureBtn);
    langBoxLayout->addStretch();
    langBoxLayout->addWidget(langCancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(langListLayout);
    mainLayout->addLayout(langBoxLayout);

    langWidget->setLayout(mainLayout);
    stackedWidget->addWidget(langWidget);

    connect(langAddBtn,    SIGNAL(clicked()), this, SLOT(onLangAddBtnClicked()));
    connect(langDelBtn,    SIGNAL(clicked()), this, SLOT(onLangDelBtnClicked()));
    connect(langCancelBtn, SIGNAL(clicked()), this, SLOT(onLangCancelBtnClicked()));
    connect(langSureBtn,   SIGNAL(clicked()), this, SLOT(onLangSureBtnClicked()));
}


/**
 * @brief manageEngineer::onLangCancelBtnClicked
 * 响应语言设置界面取消功能按键
 */
void manageEngineer::onLangCancelBtnClicked()
{
    selLangList.clear();
    for (int i = 0; i < struCnfe.nLangList[0]; i++) {
        selLangList.append(struCnfe.nLangList[i+1]);
    }
    stackedWidget->setCurrentIndex(Page_ManageEngineer);
}

/**
 * @brief manageEngineer::onLangSureBtnClicked
 * 响应语言设置界面确定功能按键
 */
void manageEngineer::onLangSureBtnClicked()
{
    struCnfe.nLangList[0] = selLangList.count();
    for (int i = 0; i < selLangList.count(); i++) {
        struCnfe.nLangList[i+1] = selLangList.at(i);
    }
    /*
    for (int i = 0; i < langTotal; i++) {
        struCnfe.nLangList[i] = nLangList[i];
    }
    */
    myFlow.saveEngineer();
    stackedWidget->setCurrentIndex(Page_ManageEngineer);
}

/**
 * @brief manageEngineer::onLangAddBtnClicked
 * 响应语言设置界面增加功能按键
 */
void manageEngineer::onLangAddBtnClicked()
{
    //! 确认语言列表非空
    if (allLangListWidget->count() <= 0)
        return;

    //! 确认是否选择语言
    if (allLangListWidget->currentRow() == -1)
        return;

    if (!allLangListWidget->currentItem()->isSelected())
        return;

    //! 确认语言是否存在
    int langId = allLangList.at(allLangListWidget->currentRow());
    if (selLangList.indexOf(langId) != -1)
        return;

    //! 语言移至选择列表
    selLangList.append(langId);

    //! 刷新语言选择列表
    updateLangList();
}

/**
 * @brief manageEngineer::onLangDelBtnClicked
 * 响应语言设置界面移除功能按键
 */
void manageEngineer::onLangDelBtnClicked()
{
    //! 确认选择列表非空
    if (selLangListWidget->count() <= 1)
        return;

    //! 确认是否选择语言
    if (selLangListWidget->currentRow() == -1)
        return;

    if (!selLangListWidget->currentItem()->isSelected())
        return;

    //! 锁定当前系统语言
    int langId = selLangList.at(selLangListWidget->currentRow());
    if (langId == struCnfg.nLang)
        return;

    //! 锁定简体中文支持
    if (langId == LANG_CHS)
        return;

    //! 语言移出选择列表
    selLangList.removeAll(langId);

    //! 刷新语言选择列表
    updateLangList();
}

/* 刷新语言列表 */
void manageEngineer::updateLangList()
{
    //! 刷新支持语言列表
    /*
    allLangListWidget->clear();
    for (int i = 0; i < allLangList.count(); i++) {
        int langId = allLangList.at(i);
        allLangListWidget->addItem(myFlow.getLanguageListName(langId));
        allLangListWidget->item(i)->setFont(config->getFont(DEFAULT_FONT_SIZE));
        allLangListWidget->item(i)->setSizeHint(QSize(BTN_HEIGHT, BTN_HEIGHT));
    }
    */

    //! 刷新选择语言列表
    selLangListWidget->clear();
    for (int i = 0; i < selLangList.count(); i++) {
        int langId = selLangList.at(i);
        selLangListWidget->addItem(myFlow.getLanguageListName(langId));
        selLangListWidget->item(i)->setFont(config->getFont(DEFAULT_FONT_SIZE));
        selLangListWidget->item(i)->setSizeHint(QSize(BTN_HEIGHT, BTN_HEIGHT));
    }

}

/*******************************************************************************
 *
 *                          延迟付款设置页面函数
 *
********************************************************************************/
void manageEngineer::createDelayPaymentPage()
{
    delayPaymentWidget = new QWidget;
    delayPaymentWidget->setGeometry(0,0,stackedWidget->width(),stackedWidget->height());
    char limit_cur[DCCRYPT_MAX_STR] = {}, product_sn_cur[DCCRYPT_MAX_STR] = {};
    char str[128] = {};

    sprintf(str, "%s  %s%s", "ssbk_ui.dc_set", product_sn_cur, limit_cur+2);

    QSize btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    delayPaymentSureBtn = new myPushButton("确定", myIcon.Action_Apply, delayPaymentWidget);
    delayPaymentSureBtn->setGeometry(QRect(QPoint(margin,height()-btnSize.height()-margin), btnSize));
    delayPaymentCancelBtn = new myPushButton("取消", myIcon.Action_Cancel, delayPaymentWidget);
    delayPaymentCancelBtn->setGeometry(QRect(QPoint(width()-btnSize.width()-margin,height()-btnSize.height()-margin),btnSize));


    stackedWidget->addWidget(delayPaymentWidget);
}

/*******************************************************************************
 *
 *                          响应设置豆类定制按钮
 *
********************************************************************************/
void manageEngineer::onSetBeanBtnClickedSlt()
{
    updateSetBean();
    stackedWidget->setCurrentIndex(Page_SetBean);
}

void manageEngineer::createSetBeanPage()
{
    int i;

    for (i = 0; i < struCnfe.struParentPrefixedProfile[0].nSonProfileCount; i++) {
        beanList[i] = struCnfe.struParentPrefixedProfile[0].struSonProfile[i].nIsAble;
    }

    setBeanWidget = new QWidget;
    setBeanWidget->setGeometry(0,0,stackedWidget->width(),stackedWidget->height());

    setBeanGrid = new QGridLayout(setBeanWidget);
    setBeanGrid->setSpacing(80);
    setBeanGrid->setContentsMargins(150,30,150,120);

    setBeanSignalMapper = new QSignalMapper(setBeanWidget);

    setBeanCheckBoxList[0] = new myCustomCheckBox("白芸豆",false, CB_STYLE_CIRCLE);
    setBeanCheckBoxList[1] = new myCustomCheckBox("红小豆",false, CB_STYLE_CIRCLE);
    setBeanCheckBoxList[2] = new myCustomCheckBox("黑芸豆",false, CB_STYLE_CIRCLE);
    setBeanCheckBoxList[3] = new myCustomCheckBox("奶花芸豆",false, CB_STYLE_CIRCLE);
    setBeanCheckBoxList[4] = new myCustomCheckBox("英国红",false, CB_STYLE_CIRCLE);
    setBeanCheckBoxList[5] = new myCustomCheckBox("绿豆",false, CB_STYLE_CIRCLE);

    for (i = 0; i < beanTotal; i++) {
        setBeanCheckBoxList[i]->setFixedSize(150, 45);
        setBeanGrid->addWidget(setBeanCheckBoxList[i], i/2, i%2, 1, 1);

        setBeanSignalMapper->setMapping(setBeanCheckBoxList[i], i);
        connect(setBeanCheckBoxList[i], SIGNAL(pressed()), setBeanSignalMapper, SLOT(map()));
    }

    updateSetBean();

    QSize btnSize = config->getBtnSize(DEFAULT_BTN_SIZE);
    setBeanSureBtn = new myPushButton("确定",myIcon.Action_Apply,setBeanWidget);
    setBeanSureBtn->setGeometry(QRect(QPoint(margin,height()-btnSize.height()-margin),btnSize));
    setBeanCancelBtn = new myPushButton("取消",myIcon.Action_Cancel,setBeanWidget);
    setBeanCancelBtn->setGeometry(QRect(QPoint(width()-btnSize.width()-margin,height()-btnSize.height()-margin),btnSize));

    stackedWidget->addWidget(setBeanWidget);

    connect(setBeanSignalMapper, SIGNAL(mapped(int)), this, SLOT(onsetBeanSelectChange(int)));

    connect(setBeanCancelBtn,SIGNAL(pressed()),this,SLOT(onsetBeanCancelBtnClicked()));
    connect(setBeanSureBtn,SIGNAL(pressed()),this,SLOT(onsetBeanSureBtnClicked()));
}
/* 取消 */
void manageEngineer::onsetBeanCancelBtnClicked()
{
    for (int i = 0; i < struCnfe.struParentPrefixedProfile[0].nSonProfileCount; i++) {
        beanList[i] = struCnfe.struParentPrefixedProfile[0].struSonProfile[i].nIsAble;
    }
     stackedWidget->setCurrentIndex(Page_ManageEngineer);
}
/* 确定 */
void manageEngineer::onsetBeanSureBtnClicked()
{
    for (int i = 0; i < struCnfe.struParentPrefixedProfile[0].nSonProfileCount; i++) {
        struCnfe.struParentPrefixedProfile[0].struSonProfile[i].nIsAble = beanList[i];
    }
    myFlow.saveEngineer();
    myFlow.sleep(1);
    system("reboot");
}

/* 豆类列表使能响应 */
void manageEngineer::onsetBeanSelectChange(int indexBean)
{
    if(setBeanCheckBoxList[indexBean]->getChecked()) {
        beanList[indexBean] = 1;
    } else {
        beanList[indexBean] = 0;
    }
}

/* 更新豆类列表update */
void manageEngineer::updateSetBean()
{
    for (int i = 0; i < beanTotal; i++) {
        if (beanList[i] == 1) {
            setBeanCheckBoxList[i]->setChecked(true);
        }
    }
}


/*******************************************************************************
 *
 *                          响应返回主界面按钮
 *
********************************************************************************/
void manageEngineer::gotoFirstPageSlt()
{
    emit sUpdateSchemeList();
    emit sUpdateArithmeticList();       // 更新算法列表
    stackedWidget->setCurrentIndex(0);
}

/* 响应当前页索引发生变化的情况 */
void manageEngineer::onStackedWidgetIndexChangedSlt(int index)
{
    QString title;

    title = QString("%1").arg(myLan.engineering_mode);
    titleLabel->setText(title);
}

void manageEngineer::onCustomLanguageBtnClickedSlt()
{
    QString str = str.fromLatin1(struCnfe.sTitle);
    myInputMethod inputPanel(myLan.msg_custom_language_input, str);

    int ret = inputPanel.exec();
    if (ret == QDialog::Accepted) {
        const char *strName = inputPanel.getText().toLatin1();
        int length = strlen(strName);
        if (length > 64) {
            myMessageBox msgBox(MSG_WARN, myLan.msg_rename_custom_language);
            ret = msgBox.exec();
        } else {
            strcpy(struCnfe.sTitle, strName);
            myFlow.saveEngineer();
        }
    }
}
