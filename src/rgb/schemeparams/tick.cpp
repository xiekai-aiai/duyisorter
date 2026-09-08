#include "tick.h"

/****************************************************************************
**
** definition of class PageTick
**
****************************************************************************/
/* 构造剔除参数设置页面 */
PageTick::PageTick(QWidget *parent)
    : QWidget(parent)
{
    /* page init */
    createPage();

    /* page layout */
    layoutPage();

    /* page update */
    updatePage();

    refreshDisplayByProduct();
}

/* 析构剔除参数页面 */
PageTick::~PageTick()
{

}

/* 应用剔除参数页面设置 */
void PageTick::setTickParams()
{
    /* write params to memory */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            struCnfp.struGroupTick[i][j] = m_struGroupTick[i*struCnfg.struLevelInfo[i].nTickGroupTotal+j];
        }
    }
    struCnfp.struGroupTick[0][1].nEnableBalance = struCnfp.struGroupTick[0][0].nEnableBalance;

    //! 确认反选开传染
    if (!checkReverseSort()) {
        infoWidget->setLabelText(myLan.msg_set_impure);
        infoWidget->showSecs();
    }

    /* write params to fpga */
    myFlow.resetTick();

    /* 重置延迟时间 */
    myFlow.resetEjectTime();
}

/* 重置剔除参数页面设置 */
void PageTick::resetTickParams()
{
    /* reste tick params */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            m_struGroupTick[i*struCnfg.struLevelInfo[i].nTickGroupTotal+j] = struCnfp.struGroupTick[i][j];
        }
    }

    //! 更新页面显示
    updatePage();
}

/*!
 * \brief PageTick::checkReverseSort
 */
bool PageTick::checkReverseSort()
{
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int nTotal = struCnfg.struLevelInfo[i].nTickGroupTotal;
        for (int j = 0; j < nTotal; j++) {
            stu_group_tick tick = struCnfp.struGroupTick[i][j];
            if ((tick.nReverseSort == 1 && tick.nImpured == 0)
                    || (tick.nShapeSortMode == 1 && tick.nShapeImpured == 0)) {
                return false;
            }
        }
    }

    return true;
}

/*!
 * \brief PageTick::refreshByProduct
 */
void PageTick::refreshDisplayByProduct()
{
    //! 玉米机型的传染行数不可调，固定为最大为16
    if (struCnfe.nEnableMaizeDoubleView == 1) {
        m_colorImpuredLbl->hide();
        m_colorImpuredLe->hide();
    }

    //! 暂不考虑形选膨胀，杂粮暂不考虑形选腐蚀
    m_shapeExpLbl->hide();
    m_shapeExpLe->hide();
    if (myFlow.getProductLineNo() == 0) {
        m_shapeCorrLbl->hide();
        m_shapeCorrLe->hide();
    }
}

/* 响应tab页切换 */
void PageTick::changeCurrentSlt(int index)
{
    /* prevent if there is no tab */
    if (index < 0) {
        return;
    }

    /* eject width */
    switch (m_struGroupTick[index].nEjwidth) {
    case 1:
        huffModeValue->setCurrentIndex(0);
        break;
    case 4:
        huffModeValue->setCurrentIndex(1);
        break;
    default:
        break;
    }

    /* front & rear logical relation */
    switch (m_struGroupTick[index].nFrontRearMode) {
    case 0:
        logicalModeValue->setCurrentIndex(0);
        break;
    case 1:
        logicalModeValue->setCurrentIndex(1);
        break;
    }

    //! 形选逻辑关系
    switch (m_struGroupTick[index].nShapeLogicMode) {
    case 0:
        m_shapeLogicModeRadio->setCurrentIndex(0);
        break;
    case 1:
        m_shapeLogicModeRadio->setCurrentIndex(1);
        break;
    default:
        break;
    }

    /* reverse sort mode */
    switch (m_struGroupTick[index].nReverseSort) {
    case 0:
        sortModeValue->setCurrentIndex(0);
        break;
    case 1:
        sortModeValue->setCurrentIndex(1);
        break;
    }

    //! 形选正反选
    switch (m_struGroupTick[index].nShapeSortMode) {
    case 0:
        m_shapeSortModeRadio->setCurrentIndex(0);
        break;
    case 1:
        m_shapeSortModeRadio->setCurrentIndex(1);
        break;
    default:
        break;
    }

    //! 传染
    infectChkBox->setChecked(m_struGroupTick[index].nImpuredFlag);
    m_colorImpuredLe->setText(QString("%1").arg(m_struGroupTick[index].nImpured));
    m_shapeImpuredLe->setText(QString("%1").arg(m_struGroupTick[index].nShapeImpured));
    if (infectChkBox->isChecked()) {
        m_colorImpuredLe->setPressEnabled(false);
        m_shapeImpuredLe->setPressEnabled(false);
    } else {
        m_colorImpuredLe->setPressEnabled(true);
        m_shapeImpuredLe->setPressEnabled(true);
    }

    //! 膨胀/腐蚀
    corrValueEdit->setText(QString("%1").arg(m_struGroupTick[index].nCorrupted));
    expandValueEdit->setText(QString("%1").arg(m_struGroupTick[index].nExpanded));
    m_shapeCorrLe->setText(QString("%1").arg(m_struGroupTick[index].nShapeCorrupted));
    m_shapeExpLe->setText(QString("%1").arg(m_struGroupTick[index].nShapeExpanded));

    /* edge cut */
    QString text = QString("%1").arg(m_struGroupTick[index].nEdgeCut);
    edgeCutValue->setText(text);

    /* edge revise */
    if (m_struGroupTick[index].nEdgeRevise) {
        edgeReviseCbx->setChecked(true);
    } else {
        edgeReviseCbx->setChecked(false);
    }

    /* bad num */
    text.setNum(m_struGroupTick[index].nBadNum);
    badNumValue->setText(text);

    /* huff width*/
    text.setNum(m_struGroupTick[index].nhuffWidth);
    huffWidthValue->setText(text);

    /* enable balance */
    if (m_struGroupTick[index].nEnableBalance) {
        enableBalanceCbx->setChecked(true);
    } else {
        enableBalanceCbx->setChecked(false);
    }
}

/* 响应吹气方式设置 */
void PageTick::setHuffModeSlt(int index)
{
    switch (index) {
    case 0:
        m_struGroupTick[tabBar->currentIndex()].nEjwidth = 1;
        break;
    case 1:
        m_struGroupTick[tabBar->currentIndex()].nEjwidth = 4;
        break;
    default:
        break;
    }
}

/* 响应前后视关系组合设置 */
void PageTick::setLogicalModeSlt(int index)
{
    m_struGroupTick[tabBar->currentIndex()].nFrontRearMode = index;
}

/*!
 * \brief PageTick::setShapeLogicModeSlt
 * \param index
 */
void PageTick::setShapeLogicModeSlt(int index)
{
    m_struGroupTick[tabBar->currentIndex()].nShapeLogicMode = index;
}

/* 响应色选模式设置 */
void PageTick::setSortModeSlt(int index)
{
    m_struGroupTick[tabBar->currentIndex()].nReverseSort = index;
}

/*!
 * \brief PageTick::setShapeSortModeSlt
 * \param index
 */
void PageTick::setShapeSortModeSlt(int index)
{
    m_struGroupTick[tabBar->currentIndex()].nShapeSortMode = index;
}

/* 响应边缘切除设置 */
void PageTick::setEdgeCutValueSlt()
{
    int nEdgeCut, nMin, nMax;

    /* get global params */
    nEdgeCut = edgeCutValue->text().toInt();
    nMin = 0;
    nMax = 3;
    myInputPanel inputDlg(intType, nMin, nMax, nEdgeCut);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nEdgeCut = inputDlg.getValue();

        /* upate params */
        m_struGroupTick[tabBar->currentIndex()].nEdgeCut = nEdgeCut;

        /* update page */
        QString text = QString("%1").arg(nEdgeCut);
        edgeCutValue->setText(text);
    }
}

/* 响应坏点个数设置 */
void PageTick::setBadNumValueSlt()
{
    int nBadNum;

    /* get global params */
    nBadNum = badNumValue->text().toInt();

    myInputPanel inputDlg(intType, 0, 255, nBadNum);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nBadNum = inputDlg.getValue();

        /* update params */
        m_struGroupTick[tabBar->currentIndex()].nBadNum = nBadNum;

        /* update page */
        QString text = QString("%1").arg(nBadNum);
        badNumValue->setText(text);
    }
}

/* 响应吹气宽度设置 */
void PageTick::setHuffWidthValueSlt()
{
    int nhuffWidth;

    /* get global params */
    nhuffWidth = huffWidthValue->text().toInt();

    myInputPanel inputDlg(intType, 0, 31, nhuffWidth);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nhuffWidth = inputDlg.getValue();

        /* update params */
        m_struGroupTick[tabBar->currentIndex()].nhuffWidth = nhuffWidth;

        /* update page */
        QString text = QString("%1").arg(nhuffWidth);
        huffWidthValue->setText(text);
    }
}

/* 响应传染使能设置 */
void PageTick::setInfectModeSlt()
{
    m_struGroupTick[tabBar->currentIndex()].nImpuredFlag = infectChkBox->isChecked();
    if (m_struGroupTick[tabBar->currentIndex()].nImpuredFlag == 1) {
        m_colorImpuredLe->setPressEnabled(false);
        m_shapeImpuredLe->setPressEnabled(false);

        //! RS-C默认传染色选0行，形选7行
        //! 其它默认传染色选、形选最大行数
        m_colorImpuredLe->setText(QString("%1").arg(struGsh.maxRowNumber));
        m_shapeImpuredLe->setText(QString("%1").arg(struGsh.maxRowNumber));
    } else {
        m_colorImpuredLe->setText("0");
        m_colorImpuredLe->setPressEnabled(true);
        m_shapeImpuredLe->setText("0");
        m_shapeImpuredLe->setPressEnabled(true);
    }
    m_struGroupTick[tabBar->currentIndex()].nImpured = m_colorImpuredLe->text().toInt();
    m_struGroupTick[tabBar->currentIndex()].nShapeImpured = m_shapeImpuredLe->text().toInt();
}

/* 色选腐蚀 */
void PageTick::setCorrSlt()
{
    int nCorr = corrValueEdit->text().toInt();

    //! 彩色米机腐蚀最大圈数为3,其它为32
    int nMax = CORR_MAX;

    myInputPanel inputDlg(intType, 0, nMax, nCorr);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nCorr = inputDlg.getValue();

        /* upate params */
        m_struGroupTick[tabBar->currentIndex()].nCorrupted = nCorr;

        /* update page */
        QString text = QString("%1").arg(nCorr);
        corrValueEdit->setText(text);
    }
}

/* 色选膨胀 */
void PageTick::setExpandSlt()
{
    int nExpand = expandValueEdit->text().toInt();

    //! 彩色米机膨胀最大圈数为3,其它为32
    int nMax = EXPAND_MAX;

    myInputPanel inputDlg(intType, 0, nMax, nExpand);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nExpand = inputDlg.getValue();

        /* upate params */
        m_struGroupTick[tabBar->currentIndex()].nExpanded = nExpand;

        /* update page */
        QString text = QString("%1").arg(nExpand);
        expandValueEdit->setText(text);
    }
}

/* 响应边缘补偿使能设置 */
void PageTick::setEdgeReviseSlt(bool enable)
{
    if (enable) {
        m_struGroupTick[tabBar->currentIndex()].nEdgeRevise = 1;
    } else {
        m_struGroupTick[tabBar->currentIndex()].nEdgeRevise = 0;
    }
}

/* 响应平衡病斑使能设置 */
void PageTick::setEnableBalanceSlt(bool enable)
{
    if (enable) {
        m_struGroupTick[tabBar->currentIndex()].nEnableBalance = 1;
    } else {
        m_struGroupTick[tabBar->currentIndex()].nEnableBalance = 0;
    }
}

/*!
 * \brief 槽函数: 色选传染
 */
void PageTick::onColorImpuredLePressed()
{
    int nVal = m_colorImpuredLe->text().toInt();

    //! 彩色米机最大传染圈数为7,其它为62
    int nMax = IMPURED_MAX;

    myInputPanel *inputPanel = new myInputPanel(intType, 0, nMax, nVal);

    if (inputPanel->exec() == QDialog::Accepted) {
        nVal = inputPanel->getValue();
        m_colorImpuredLe->setText(QString("%1").arg(nVal));
        int nIndex = tabBar->currentIndex();
        m_struGroupTick[nIndex].nImpured = nVal;
    }
}

/*!
 * \brief 槽函数: 形选传染
 */
void PageTick::onShapeImpuredLePressed()
{
    int nVal = m_shapeImpuredLe->text().toInt();

    //! 彩色米机最大传染圈数为7,其它为62
    int nMax = IMPURED_MAX;

    myInputPanel *inputPanel = new myInputPanel(intType, 0, nMax, nVal);

    if (inputPanel->exec() == QDialog::Accepted) {
        nVal = inputPanel->getValue();
        m_shapeImpuredLe->setText(QString("%1").arg(nVal));
        int nIndex = tabBar->currentIndex();
        m_struGroupTick[nIndex].nShapeImpured = nVal;
    }
}

/*!
 * \brief 槽函数: 形选腐蚀
 */
void PageTick::onShapeCorrLePressed()
{
    int nVal = m_shapeCorrLe->text().toInt();

    //! 彩色米机腐蚀最大圈数为3,其它为32
    int nMax = CORR_MAX;

    myInputPanel *inputPanel = new myInputPanel(intType, 0, nMax, nVal);

    if (inputPanel->exec() == QDialog::Accepted) {
        nVal = inputPanel->getValue();
        m_shapeCorrLe->setText(QString("%1").arg(nVal));
        int nIndex = tabBar->currentIndex();
        m_struGroupTick[nIndex].nShapeCorrupted = nVal;
    }
}

/*!
 * \brief 槽函数: 形选膨胀
 */
void PageTick::onShapeExpLePressed()
{
    int nVal = m_shapeExpLe->text().toInt();

    //! 彩色米机膨胀最大圈数为3,其它为32
    int nMax = EXPAND_MAX;

    myInputPanel *inputPanel = new myInputPanel(intType, 0, nMax, nVal);

    if (inputPanel->exec() == QDialog::Accepted) {
        nVal = inputPanel->getValue();
        m_shapeExpLe->setText(QString("%1").arg(nVal));
        int nIndex = tabBar->currentIndex();
        m_struGroupTick[nIndex].nShapeExpanded = nVal;
    }
}

/* 初始化剔除参数页面 */
void PageTick::createPage()
{
/* page init */
    /* tab widget */
    tabBar = new MyTabBar();
    if (LCD_WIDTH == 640) {
        tabBar->setMaximumHeight(BTN_HEIGHT-12);
    }
    /* huff mode group */
    QStringList huffModeList = QStringList() << myLan.fixed_mode
                                             << myLan.extended_mode;
    huffModeGroup = new myGroupBox(myLan.eject_mode);
//    huffModeGroup = new myGroupBox("吹气算法");
    huffModeValue = new MyRadioGroup(huffModeList);

    /* infect group */
    infectGroup = new myGroupBox(myLan.infect_arithmetic);
//    infectGroup = new myGroupBox("膨胀圈数");

    infectChkBox = new MyCheckBox(myLan.enable_default, false);
//    infectChkBox = new MyCheckBox("默认", false);

    m_colorImpuredLbl = new myLabel(myLan.color_sort);
    m_colorImpuredLe = new myLineEdit;
    m_shapeImpuredLbl = new myLabel(myLan.shape_sort);
    m_shapeImpuredLe = new myLineEdit;

    /* edge cut group */
    edgeCutGroup = new myGroupBox(myLan.advanced_parameters);
    edgeCutLabel = new myLabel(myLan.edge_cut);
    edgeCutValue = new myLineEdit("");
    edgeReviseCbx = new MyCheckBox(myLan.edge_revise, false, QBoxLayout::RightToLeft);

    /* 高级参数 */
    adParamsGroup = new myGroupBox(myLan.advanced_parameters);
//    adParamsGroup = new myGroupBox("腐蚀圈数");
    corrValueEdit = new myLineEdit("");
    expandValueEdit = new myLineEdit("");
//    corrValueEditLabel = new myLabel(QString("%1%2").arg(myLan.color_sort).arg(1));
//    expandValueEditLabel = new myLabel(QString("%1%2").arg(myLan.color_sort).arg(2));
    corrValueEditLabel = new myLabel(QString("%1%2").arg("").arg(1));
    expandValueEditLabel = new myLabel(QString("%1%2").arg("").arg(2));
    m_shapeCorrLbl = new myLabel(QString("%1").arg(myLan.shape_sort));
    m_shapeExpLbl = new myLabel(QString("%1%2").arg(myLan.shape_sort).arg(2));
    m_shapeCorrLe = new myLineEdit;
    m_shapeExpLe = new myLineEdit;

    /* bad number group */
    badNumGroup = new myGroupBox(myLan.other);
    badNumLabel = new myLabel(myLan.bad_number);
    badNumValue = new myLineEdit("");
    enableBalanceCbx = new MyCheckBox(myLan.enable_balance, false, QBoxLayout::RightToLeft);
    huffWidthLabel = new myLabel(myLan.huff_width);
    huffWidthValue = new myLineEdit("");

    /* logical mode group */
    QStringList logicalModeList = QStringList() << myLan.logic_or << myLan.logic_and;
    logicalModeGroup = new myGroupBox(myLan.logical_relation);
//    logicalModeGroup = new myGroupBox("前后逻辑");

    logicalModeValue = new MyButtonGroup(logicalModeList);
    m_shapeLogicModeRadio = new MyButtonGroup(logicalModeList);

    /* sort mode group */
    QStringList sortModeList = QStringList() << myLan.sort << myLan.reverse;
    sortModeGroup  = new myGroupBox(myLan.sort_mode);
//    sortModeGroup  = new myGroupBox("正反选择");

    sortModeValue = new MyButtonGroup(sortModeList);
    m_shapeSortModeRadio = new MyButtonGroup(sortModeList);

/* page update */
    /* signals to slots */
    connect(tabBar,        SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(huffModeValue,    SIGNAL(currentChanged(int)), this, SLOT(setHuffModeSlt(int)));
    connect(logicalModeValue, SIGNAL(currentChanged(int)), this, SLOT(setLogicalModeSlt(int)));
    connect(sortModeValue,    SIGNAL(currentChanged(int)), this, SLOT(setSortModeSlt(int)));
    connect(edgeCutValue,     SIGNAL(pressed()),           this, SLOT(setEdgeCutValueSlt()));
    connect(edgeReviseCbx,    SIGNAL(toggled(bool)),       this, SLOT(setEdgeReviseSlt(bool)));
    connect(badNumValue,      SIGNAL(pressed()),           this, SLOT(setBadNumValueSlt()));
    connect(huffWidthValue,   SIGNAL(pressed()),           this, SLOT(setHuffWidthValueSlt()));
    connect(enableBalanceCbx, SIGNAL(toggled(bool)),       this, SLOT(setEnableBalanceSlt(bool)));
    connect(corrValueEdit,    SIGNAL(pressed()),           this, SLOT(setCorrSlt()));
    connect(expandValueEdit,  SIGNAL(pressed()),           this, SLOT(setExpandSlt()));
    connect(infectChkBox,     SIGNAL(pressed()),           this, SLOT(setInfectModeSlt()));
    connect(m_colorImpuredLe, SIGNAL(pressed()), this, SLOT(onColorImpuredLePressed()));
    connect(m_shapeImpuredLe, SIGNAL(pressed()), this, SLOT(onShapeImpuredLePressed()));
    connect(m_shapeCorrLe, SIGNAL(pressed()), this, SLOT(onShapeCorrLePressed()));
    connect(m_shapeExpLe, SIGNAL(pressed()), this, SLOT(onShapeExpLePressed()));
    connect(m_shapeLogicModeRadio, SIGNAL(currentChanged(int)), this, SLOT(setShapeLogicModeSlt(int)));
    connect(m_shapeSortModeRadio, SIGNAL(currentChanged(int)), this, SLOT(setShapeSortModeSlt(int)));
}

/* 更新剔除参数设置页面 */
void PageTick::updatePage()
{
    edgeCutGroup->hide();
    adParamsGroup->show();

    /* refresh all the tab items */
    updateTabBar(this->tabBar, false, false);
}

/* 布局剔除参数设置页面 */
void PageTick::layoutPage()
{
/* page layout */
    /* size policy */
    edgeCutValue->setFixedHeight(BTN_HEIGHT-10);
    badNumValue->setFixedHeight(BTN_HEIGHT-10);
    huffWidthValue->setFixedHeight(BTN_HEIGHT-10);
    corrValueEdit->setFixedHeight(BTN_HEIGHT-10);
    expandValueEdit->setFixedHeight(BTN_HEIGHT-10);

    if (LCD_WIDTH == 1024) {
        edgeCutValue->setFixedWidth(BTN_WIDTH-40);
        badNumValue->setFixedWidth(BTN_WIDTH-40);
        huffWidthValue->setFixedWidth(BTN_WIDTH-40);
        corrValueEdit->setFixedWidth(BTN_WIDTH-40);
        expandValueEdit->setFixedWidth(BTN_WIDTH-40);
        m_colorImpuredLe->setFixedWidth(BTN_WIDTH-30);
        m_shapeImpuredLe->setFixedWidth(BTN_WIDTH-30);
        m_shapeCorrLe->setFixedWidth(BTN_WIDTH-40);
        m_shapeExpLe->setFixedWidth(BTN_WIDTH-40);
    } else {
        edgeCutValue->setFixedWidth(60);
        badNumValue->setFixedWidth(60);
        huffWidthValue->setFixedWidth(60);
        corrValueEdit->setFixedWidth(60);
        expandValueEdit->setFixedWidth(60);
        m_colorImpuredLe->setFixedWidth(60);
        m_shapeImpuredLe->setFixedWidth(60);
        m_shapeCorrLe->setFixedWidth(60);
        m_shapeExpLe->setFixedWidth(60);
    }

    /* huff mode group layout */
    QVBoxLayout *huffLayout = new QVBoxLayout(huffModeGroup);
    huffLayout->addWidget(huffModeValue);

    infectChkBox->setFixedHeight(BTN_HEIGHT+30);

    /* 传染控件布局 */
    QVBoxLayout *infectLayout = new QVBoxLayout(infectGroup);
//    infectLayout->addWidget(infectChkBox, 0, 0, 1, 2);
    infectLayout->addWidget(infectChkBox);

    QHBoxLayout *colorLayout = new QHBoxLayout;
    QHBoxLayout *shapeLayout = new QHBoxLayout;

    colorLayout->addWidget(m_colorImpuredLbl);
    colorLayout->addWidget(m_colorImpuredLe);
    shapeLayout->addWidget(m_shapeImpuredLbl);
    shapeLayout->addWidget(m_shapeImpuredLe);
//    infectLayout->addSpacing(15);
    infectLayout->addStretch();
    infectLayout->addLayout(colorLayout);
    infectLayout->addLayout(shapeLayout);

//    infectLayout->addWidget(m_colorImpuredLbl, 1, 0, 1, 1);
//    infectLayout->addWidget(m_colorImpuredLe, 1, 1, 1, 1);
//    infectLayout->addWidget(m_shapeImpuredLbl, 2, 0, 1, 1);
//    infectLayout->addWidget(m_shapeImpuredLe, 2, 1, 1, 1);

    /* edge cut group layout */
    QHBoxLayout *edgeCutLayout = new QHBoxLayout;
    edgeCutLayout->addWidget(edgeCutLabel);
    edgeCutLayout->addWidget(edgeCutValue);

    QVBoxLayout *edgeLayout = new QVBoxLayout(edgeCutGroup);
    edgeLayout->addWidget(enableBalanceCbx);
    edgeLayout->addWidget(edgeReviseCbx);

    /* 腐蚀膨胀控件布局 */
    QGridLayout *adLayout = new QGridLayout(adParamsGroup);
    adLayout->addWidget(corrValueEditLabel, 0, 0);
    adLayout->addWidget(corrValueEdit, 0, 1);
    adLayout->addWidget(expandValueEditLabel, 1, 0);
    adLayout->addWidget(expandValueEdit, 1, 1);
    adLayout->addWidget(m_shapeCorrLbl, 2, 0);
    adLayout->addWidget(m_shapeCorrLe, 2, 1);
    adLayout->addWidget(m_shapeExpLbl, 3, 0);
    adLayout->addWidget(m_shapeExpLe, 3, 1);

    /* bad number group layout */
    QHBoxLayout *badNumLayout = new QHBoxLayout();
    badNumLayout->addWidget(badNumLabel);
    badNumLayout->addWidget(badNumValue);

    /* eject width group layout*/
    QHBoxLayout *huffWidthLayout = new QHBoxLayout();
    huffWidthLabel->setFixedWidth(BTN_WIDTH+10);
    huffWidthLayout->addWidget(huffWidthLabel);
    huffWidthLayout->addWidget(huffWidthValue);

    QHBoxLayout *badLayout = new QHBoxLayout(badNumGroup);
    badLayout->addLayout(badNumLayout);
    badLayout->setSpacing(30);
    badLayout->addLayout(edgeCutLayout);
    if(myFlow.getProductLineNo() == 0) { //杂粮机型适用
        badLayout->addLayout(huffWidthLayout);
    }

    /* logical mode group layout */
    QGridLayout *logicalLayout = new QGridLayout(logicalModeGroup);
    m_colorLogicLbl = new myLabel(myLan.color_sort);
    m_shapeLogicLbl = new myLabel(myLan.shape_sort);
    blankLbl1 = new myLabel("");
//    logicalLayout->addWidget(m_colorLogicLbl, 1, 0);
//    logicalLayout->addWidget(m_shapeLogicLbl, 2, 0);
    logicalLayout->addWidget(blankLbl1, 0, 0, 1, 2);
    logicalLayout->addWidget(logicalModeValue, 1, 1, 1, 2);
    logicalLayout->addWidget(m_shapeLogicModeRadio, 2, 1, 1, 2);

    /* sort mode group layout */
    QGridLayout *sortLayout = new QGridLayout(sortModeGroup);
    m_colorSortLbl = new myLabel(myLan.color_sort);
    m_shapeSortLbl = new myLabel(myLan.shape_sort);
    blankLbl2 = new myLabel("");
//    sortLayout->addWidget(m_colorSortLbl, 0, 0);
//    sortLayout->addWidget(m_shapeSortLbl, 1, 0);
    sortLayout->addWidget(blankLbl2, 0, 0, 1, 2);
    sortLayout->addWidget(sortModeValue, 1, 1, 1, 2);
    sortLayout->addWidget(m_shapeSortModeRadio, 2, 1, 1, 2);

    /* tick grid layout */
    upperHbLayout = new QHBoxLayout();
    upperHbLayout->addWidget(huffModeGroup,1);
    upperHbLayout->addWidget(infectGroup,1);
//    upperHbLayout->addWidget(adParamsGroup);
    upperHbLayout->addWidget(logicalModeGroup,1);
    upperHbLayout->addWidget(sortModeGroup,1);
//    upperHbLayout->addWidget(edgeCutGroup);

    lowerHbLayout = new QHBoxLayout();
    lowerHbLayout->addWidget(adParamsGroup,1);
    lowerHbLayout->addWidget(badNumGroup,3);

    QGridLayout *tickLayout = new QGridLayout;
    tickLayout->setVerticalSpacing(0);
    tickLayout->addLayout(upperHbLayout, 0, 0, 3, 1);
    tickLayout->setSpacing(20);
    tickLayout->addLayout(lowerHbLayout, 3, 0, 2, 1);

    /* main layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabBar);
    mainLayout->addLayout(tickLayout);
    setLayout(mainLayout);
}
