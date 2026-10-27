#include "arithenable.h"
#include "configmgr.h"
#include "unilog.h"

/**
 * @brief PageIdentify::PageIdentify
 * 构造识别参数页面
 * @param parent
 */
PageIdentify::PageIdentify(QWidget* parent)
    : QWidget(parent)
    , m_bInitialized(false)
{
    /* import global params */
    resetIdentifyParams();

    /* page init */
    createPage();

    /* page layout */
    layoutPage();

    /* page update */
    updatePage();

    /* 构造完成，允许 setArithButton 正常写回 */
    m_bInitialized = true;
}

/**
 * @brief PageIdentify::~PageIdentify
 * 析构识别参数页面
 */
PageIdentify::~PageIdentify()
{

}

/**
 * @brief PageIdentify::setIdentifyParams
 * 应用识别参数页面对应设置
 */
void PageIdentify::setIdentifyParams()
{
    /* write params to memory */
        /* material assemble mode */
    setMatAssembleMode();

    /* arithmetic list enable */
    setArithmeticEnable();

    /* 重置延迟时间 */
    myFlow.resetEjectTime();
}

/**
 * @brief PageIdentify::resetIdentifyParams
 * 重置识别参数页面对应参数
 */
void PageIdentify::resetIdentifyParams()
{
    /* reset identify params */
        /* mat material mode */
    m_nMatMode = struCnfp.nMatAssembleMode;

    /* arithmetic list count */

    /* arithmetic list enable */
    memset(m_nArithmeticEnable, 0, sizeof(m_nArithmeticEnable));
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int g = 0; g < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; g++)
        {
            for (int j = 0; j < ARITHMETIC_TOTAL; j++)
            {
                m_nArithmeticEnable[i][g][j] = struCnfp.nArithmeticEnableGroup[i][g][j];
            }
        }
    }

    /* 算法使能永远全局统一：每层一律以 group 0 为准，
       兼容旧方案里各组使能不一致的历史数据，并同步写回 struCnfp */
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        int total = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        if (total <= 0)
            continue;
        /* AI 未使能时，智能算法一律置否 */
        if (!struCnfp.enableAi)
            m_nArithmeticEnable[i][0][ARITH_PISTACHIO] = 0;
        for (int g = 1; g < total; g++)
        {
            memcpy(m_nArithmeticEnable[i][g], m_nArithmeticEnable[i][0], sizeof(int) * ARITHMETIC_TOTAL);
        }
    }
    memcpy(struCnfp.nArithmeticEnableGroup, m_nArithmeticEnable, sizeof(struCnfp.nArithmeticEnableGroup));

    /* trigger page update */
    emit pageUpdated();
}

/**
 * @brief PageIdentify::getMatAssembleMode
 * 获取当前物料组合设置
 * @return
 */
int PageIdentify::getMatAssembleMode()
{
    return matModeCombo->itemData(matModeCombo->currentIndex()).toInt();
}

/**
 * @brief PageIdentify::setMatAssembleMode
 * 应用当前物料组合设置
 */
void PageIdentify::setMatAssembleMode()
{
    m_nMatMode = getMatAssembleMode();
    if (struCnfp.nMatAssembleMode != m_nMatMode)
    {
        struCnfp.nMatAssembleMode = m_nMatMode;
    }
}

/**
 * @brief PageIdentify::getArithmeticEnable
 * 获取识别算法使能个数
 * @return
 */
int PageIdentify::getArithmeticEnable()
{
    int i, nArithCount = 0;

    /* arithmetic list */
    for (i = 0; i < arithmeticBox.size(); i++)
    {
        if (arithmeticBox[i]->isChecked())
        {
            nArithCount++;
        }
    }

    return nArithCount;
}

/**
 * @brief PageIdentify::setArithmeticEnable
 * 保存当前识别算法使能设置
 */
void PageIdentify::setArithmeticEnable()
{
    int level = struGsh.nLevel;
    int group = tabBar->currentIndex();
    LOG_INFO_STM("level:" << level << ", group:" << group << ", arithmetic count:" << arithmeticBox.size());
    if (group < 0 || group >= MAX_GROUP_IDTNTIFY) return;

    // 先把当前 tabBar 页的勾选框状态同步到内存
    for (int i = 0; i < arithmeticBox.size(); i++)
    {
        int arith = arithmeticSeq[i];
        m_nArithmeticEnable[level][group][arith] = arithmeticBox[i]->isChecked() ? 1 : 0;
    }

    // 算法使能永远全局统一，不分 m_nMatMode；从当前 tab 的组同步到所有其他组
    int total = struCnfg.struLevelInfo[level].nIdentifyGroupTotal;
    for (int g = 0; g < total; g++)
    {
        if (g != group)
            memcpy(m_nArithmeticEnable[level][g], m_nArithmeticEnable[level][group], sizeof(int) * ARITHMETIC_TOTAL);
    }

    // 把完整三维写回全局
    memcpy(struCnfp.nArithmeticEnableGroup, m_nArithmeticEnable, sizeof(struCnfp.nArithmeticEnableGroup));
}

/*!
 * \brief PageIdentify::getMatAssembleModeList
 * 获取参数组合类型列表
 * \return
 */
QList<int> PageIdentify::getMatAssembleModeList()
{
    QList<int> list;

    switch (struCnfe.nMachine)
    {
        case MACHINE_CF:
        case MACHINE_CG:
            list << PARAMS_ALL_SEPARATE
                << PARAMS_ALL_SAME
                << PARAMS_FRONT_REAR_SAME
                << PARAMS_FIRST_SECOND_SAME;
            break;
        default:
            list << PARAMS_ALL_SEPARATE
                << PARAMS_ALL_SAME;
            break;
    }

    return list;
}

/*!
 * \brief PageIdentify::getMatAssembleModeName
 * 获取参数组合类型对应显示文本
 * \param mode
 * \return
 */
QString PageIdentify::getMatAssembleModeName(int mode)
{
    QString text = "";
    switch (mode)
    {
        case PARAMS_ALL_SAME:
            text = myLan.all_alike;
            break;
        case PARAMS_ALL_SEPARATE:
            text = myLan.all_seperate;
            break;
        case PARAMS_FRONT_REAR_SAME:
            text = myLan.front_rear;
            break;
        case PARAMS_FIRST_SECOND_SAME:
            text = myLan.first_second;
            break;
        default:
            break;
    }

    return text;
}

/**
 * @brief PageIdentify::setMatModeCombo
 * 响应物料组合框列表项改变
 * @param index
 */
void PageIdentify::setMatModeCombo(int index)
{
    if (index < 0)
        return;

    m_nMatMode = matModeCombo->itemData(index).toInt();
    struCnfp.nMatAssembleMode = m_nMatMode;

    // 算法使能永远全局统一，只在 setArithmeticEnable 里改，这里不动

    int level = struGsh.nLevel;
    int totalGroup = struCnfg.struLevelInfo[level].nIdentifyGroupTotal;

    /* matModeCombo切换时，识别组参数立即按新模式组装：
       逐个源组调用，保证多组机型每一对组（前视/后视、第一/第二次）都能按新模式同步；
       全部单独设置(ALL_SEPARATE)时 materialCopyAssemble 内部直接 break，不复制 */
    for (int g = 0; g < totalGroup; g++)
        myFlow.materialCopyAssemble(level, g, 1, 0, 0);

    /* 复制完立即把本层所有组的新参数下发硬件，切换模式后即时生效 */
    for (int g = 0; g < totalGroup; g++)
        myFlow.materialResetGroupAssemble(level, g, 1, 0, 0);

    /* 刷新 tabBar enabled 状态和页面 */
    updatePage();
}

/**
 * @brief PageIdentify::setArithButton
 * 响应算法列表勾选框
 * @param btnId
 */
void PageIdentify::setArithButton(int btnId)
{
    LOG_INFO_STM("********* btnId:" << btnId << ", is checked:" << arithmeticBox[btnId]->isChecked()
        << ", seq:" << arithmeticSeq[btnId]);

    /* 构造期间 setChecked 等 UI 刷新触发的信号要吞掉，
       因为 resetIdentifyParams 刚从 struCnfp 拷了正确值，
       这里再全量 memcpy 会用错误的 UI 默认值覆盖 struCnfp */
    if (!m_bInitialized) return;

    /* enable arithmetic buttons */
    if (arithmeticBox[btnId]->isChecked())
    {
        /* 西瓜子算法已从 CF/CG 机型算法列表中移除（nArithmeticSeq[ARITH_WATERMELON]=0，
           界面不会创建该选项），其与其它算法的排他使能逻辑一并注释掉，
           勾选任意算法时直接使能
        if (arithmeticSeq[btnId] == ARITH_WATERMELON)
        {
            //! 使能西瓜子算法可同时使能智能A
            for (int i = 0; i < arithmeticSeq.size(); i++)
            {
                if (i == btnId || arithmeticSeq[i] == ARITH_INTEL_A)
                {
                    continue;
                }
                arithmeticBox[i]->setEnabled(false);
                arithmeticBox[i]->setChecked(false);
                arithmeticBtn[i]->setEnabled(false);
                m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[i]] = 0;
            }
            m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[btnId]] = 1;
            arithmeticBtn[btnId]->setEnabled(true);
        }
        else
        {
            //! 智能A使能不受西瓜子算法使能影响
            if (m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][ARITH_WATERMELON] == 0
                || arithmeticSeq[btnId] == ARITH_INTEL_A)
            {
                m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[btnId]] = 1;
                arithmeticBtn[btnId]->setEnabled(true);
            }
            else
            {
                for (int i = 0; i < arithmeticSeq.size(); i++)
                {
                    if (arithmeticSeq[i] == ARITH_WATERMELON)
                    {
                        continue;
                    }
                    arithmeticBox[i]->setEnabled(false);
                    arithmeticBox[i]->setChecked(false);
                    arithmeticBtn[i]->setEnabled(false);
                    m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[i]] = 0;
                }
            }
        }
        */
        m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[btnId]] = 1;
        arithmeticBtn[btnId]->setEnabled(true);
    }
    else
    {
        /* 西瓜子算法已移除，取消勾选时无需恢复其它选项的可用性
        if (arithmeticSeq[btnId] == ARITH_WATERMELON)
        {
            for (int i = 0; i < arithmeticSeq.size(); i++)
            {
                arithmeticBox[i]->setEnabled(true);
            }
        }
        */
        m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[btnId]] = 0;
        arithmeticBtn[btnId]->setEnabled(false);
    }
    /* assign arithmetic enable */
    setArithmeticEnable();

    /* reset arithmetic enable */
    myFlow.resetArithmeticEnable();
}

/**
 * @brief PageIdentify::resetArithmeticEnable
 * 下发算法使能参数
 */
void PageIdentify::resetArithmeticEnable()
{
    /* 先把当前 tabBar 页勾选框同步到三维内存 */
    setArithmeticEnable();

    /* 重置算法使能 */
    myFlow.resetArithmeticEnable();

    /* 重置延迟时间 */
    myFlow.resetEjectTime();
}

/**
 * @brief PageIdentify::changePage
 * 响应算法列表按键，触发对应不同算法索引号的页面ID信号
 * @param btnId
 */
void PageIdentify::changePage(int btnId)
{
    int pageId = getPageIndex(btnId);
    LOG_INFO_STM("btn id:" << btnId << ", pageId:" << pageId);

    /* trigger pageChanged signal to change page by pageId */
    emit pageChanged(pageId);
}


/**
 * @brief PageIdentify::createPage
 * 初始化识别参数页面
 */
void PageIdentify::createPage()
{
    /* tab widget */
    tabBar = new MyTabBar();

    /* arithmetic list group */
    arithmeticGroup = new QWidget;

    /* material assemble mode group */
    matModeLabel = new myLabel(myLan.material_mode);
    matModeCombo = new MyComboBox();
    matModeCombo->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    matModeCombo->setFixedHeight(BTN_HEIGHT);
    QString g_style1 = "background-color: rgb(210, 231, 245);""outline: none;""border-radius: 8;";
    matModeCombo->setStyleSheet(g_style1);

    /* arithmetic list */
    createArithmeticList();

    /* signals to slots */
    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(matModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setMatModeCombo(int)));
    connect(this, SIGNAL(pageUpdated()), this, SLOT(updatePage()));
}

/**
 * @brief PageIdentify::createArithmeticList
 * 构造识别参数页面算法列表
 */
void PageIdentify::createArithmeticList()
{
    bool bIsHide = false;

    /* clear list vector */
    arithmeticBox.clear();
    arithmeticBtn.clear();
    arithmeticSeq.clear();

    /* construct arith list */
    for (int i = 0; i < ARITHMETIC_TOTAL; i++)
    {
        /* skip invalid arith */
        if (struCnfe.nArithmeticSeq[i] == 0)
            continue;

        /* skip specific arith */
        bIsHide = (i == ARITH_INTEL_A || i == ARITH_INTEL_B ||
            i == ARITH_INTEL_C || i == ARITH_INTEL_D);
        if (bIsHide)
            continue;

        /* append arith list to vector */
        arithmeticBox.append(new MyCheckBox("", this));
        arithmeticBtn.append(new myPushButton("", QIcon(), true, this));
        arithmeticSeq.append(i);
    }
    if (myFlow.getProductLineNo() == 0)
    {
        for (int i = 0; i < 2; i++)
        {
            /* append hardcode arith list */
            arithmeticBox.append(new MyCheckBox("", this));
            arithmeticBtn.append(new myPushButton("", QIcon(), true, this));
            arithmeticSeq.append(ARITH_INTEL_A + i);
        }
    }

    /* signal mapper */
    sigBtnMapper = new QSignalMapper(this);
    sigBoxMapper = new QSignalMapper(this);

    for (int i = 0; i < arithmeticBox.size(); i++)
    {
        /* size policy */
        arithmeticBox[i]->setMinimumSize(BTN_HEIGHT, BTN_HEIGHT);
        arithmeticBtn[i]->setFocusPolicy(Qt::NoFocus);
        arithmeticBtn[i]->setMinimumSize(BTN_WIDTH, BTN_HEIGHT);

        /* signal mapped */
        sigBoxMapper->setMapping(arithmeticBox[i], i);
        connect(arithmeticBox[i], SIGNAL(pressed()), sigBoxMapper, SLOT(map()));

        sigBtnMapper->setMapping(arithmeticBtn[i], arithmeticSeq[i]);
        connect(arithmeticBtn[i], SIGNAL(pressed()), sigBtnMapper, SLOT(map()));
    }

    /* signal to slots */
    connect(sigBoxMapper, SIGNAL(mapped(int)), this, SLOT(setArithButton(int)));
    connect(sigBtnMapper, SIGNAL(mapped(int)), this, SLOT(changePage(int)));
}

/**
 * @brief PageIdentify::updateArithmeticName
 * 更新算法列表命名
 */
void PageIdentify::updateArithmeticName()
{
    /* get arithmetic list name from hard code */
    for (int i = 0; i < arithmeticSeq.size(); i++)
    {
        arithmeticBtn[i]->setText(myString.sArithmeticName[arithmeticSeq[i]]);
    }
}

/**
 * @brief PageIdentify::updateArithmeticState
 * 更新算法列表状态
 */
void PageIdentify::updateArithmeticState()
{
    for (int i = 0; i < arithmeticBtn.size(); i++)
    {
        if (struCnfe.nArithmeticSeq[arithmeticSeq[i]] == 1)
        {
            arithmeticBox[i]->show();
            arithmeticBtn[i]->show();

            if (m_nArithmeticEnable[struGsh.nLevel][tabBar->currentIndex()][arithmeticSeq[i]] == 1)
            {
                arithmeticBox[i]->setChecked(true);
                arithmeticBtn[i]->setEnabled(true);
                //只展示，没法使能，同时置算法使能为否
                if (!struCnfp.enableAi && arithmeticSeq[i] == ARITH_PISTACHIO)
                {
                    arithmeticBox[i]->setChecked(false);
                    arithmeticBox[i]->setEnabled(false);
                    arithmeticBtn[i]->setEnabled(false);
                    /* 算法使能全局统一：智能算法置否要同步到本层所有组 */
                    for (int g = 0; g < struCnfg.struLevelInfo[struGsh.nLevel].nIdentifyGroupTotal; g++)
                        m_nArithmeticEnable[struGsh.nLevel][g][ARITH_PISTACHIO] = 0;
                }
                else
                {
                    arithmeticBox[i]->setEnabled(true);
                }
            }
            else
            {
                arithmeticBox[i]->setChecked(false);
                arithmeticBtn[i]->setEnabled(false);
                //只展示，没法使能，同时置算法使能为否
                if (!struCnfp.enableAi && arithmeticSeq[i] == ARITH_PISTACHIO)
                {
                    arithmeticBox[i]->setChecked(false);
                    arithmeticBox[i]->setEnabled(false);
                    arithmeticBtn[i]->setEnabled(false);
                    /* 算法使能全局统一：智能算法置否要同步到本层所有组 */
                    for (int g = 0; g < struCnfg.struLevelInfo[struGsh.nLevel].nIdentifyGroupTotal; g++)
                        m_nArithmeticEnable[struGsh.nLevel][g][ARITH_PISTACHIO] = 0;
                }
                else
                {
                    arithmeticBox[i]->setEnabled(true);
                }
            }
        }
        else
        {
            arithmeticBox[i]->setChecked(false);
            arithmeticBtn[i]->setEnabled(false);
            arithmeticBox[i]->hide();
            arithmeticBtn[i]->hide();
        }
    }
}

/**
 * @brief PageIdentify::updateArithmeticList
 * 更新算法列表
 */
void PageIdentify::updateArithmeticList()
{
    /* display Arithmetic Name */
    updateArithmeticName();

    /* display Arithmetic State */
    updateArithmeticState();
}

/**
 * @brief PageIdentify::updateMatModeCombo
 * 更新物料参数组合显示
 * @param mode
 */
void PageIdentify::updateMatModeCombo(int mode)
{
    // 清空物料组合控件列表
    disconnect(matModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setMatModeCombo(int)));
    matModeCombo->clear();

    // 动态获取物料参数列表
    QList<int> list = getMatAssembleModeList();
    for (int i = 0; i < list.count(); i++)
    {
        matModeCombo->addItem(getMatAssembleModeName(list.at(i)), list.at(i));
    }

    // 匹配当前物料参数组合
    int index = matModeCombo->findData(mode);
    if (index < 0)
    {
        index = matModeCombo->count() - 1;
    }
    connect(matModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setMatModeCombo(int)));
    QSignalBlocker blocker(matModeCombo);
    matModeCombo->setCurrentIndex(index);
}

/**
 * @brief PageIdentify::changeCurrentSlt
 * @param index
 */
void PageIdentify::changeCurrentSlt(int index)
{
    /* prevent if there no tab page */
    if (index < 0)
        return;

    switch (struCnfe.nMachine)
    {
    default:
        struGsh.nLevel = ONE_LEVEL;
        break;
    }

    /* update arithmetic list */
    updateArithmeticList();
}

/**
 * @brief PageIdentify::updatePage
 * 更新识别参数页面
 */
void PageIdentify::updatePage()
{
    /* update tab widget */
    updateTabBar(tabBar, false, true);
    {
        QSignalBlocker blocker(tabBar);
        tabBar->setCurrentIndex(0);
    }
    /* material assemble mode */
    updateMatModeCombo(m_nMatMode);

    /* 手动刷新算法列表（上面 QSignalBlocker 阻止了 tabBar currentChanged 信号，
     * 导致 changeCurrentSlt() 不触发 → updateArithmeticList() 被跳过）*/
    updateArithmeticList();
}

/**
 * @brief PageIdentify::updateArithmeticLayout
 * 更新算法列表布局
 */
void PageIdentify::updateArithmeticLayout()
{
    /* clear arithmetic item layout */
    arithmeticLayout.clear();

    for (int i = 0; i < arithmeticBox.size(); i++)
    {
        arithmeticLayout.append(new QFormLayout);
        arithmeticLayout[i]->addRow(arithmeticBox[i], arithmeticBtn[i]);
    }

    /* clear arithmetic list layout */
    for (int i = gridLayout->count() - 1; i >= 0; i--)
    {
        gridLayout->removeItem(gridLayout->itemAt(i));
    }

    /* add arithmetic item to grid layout */

    gridLayout->addWidget(matModeCombo, 0, 0, 1, 2);
    for (int i = 0; i < arithmeticBox.size(); i++)
    {
        gridLayout->addLayout(arithmeticLayout[i], i / 2 + 1, i % 2, Qt::AlignRight);
    }

#if 0
    gridLayout->setColumnStretch(0, 4);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 4);

    gridLayout->addWidget(matModeCombo, 0, 0, 1, 3);
    gridLayout->setSpacing(25);
    for (int i = 0; i < arithmeticBox.size(); i++)
    {
        if (i % 2 == 0)
            gridLayout->addLayout(arithmeticLayout[i], i / 2 + 1, 0, Qt::AlignRight);
        else
            gridLayout->addLayout(arithmeticLayout[i], i / 2 + 1, 2, Qt::AlignRight);

    }
#endif
}

/**
 * @brief PageIdentify::layoutPage
 * 布局识别参数页面
 */
void PageIdentify::layoutPage()
{
    /* arithmetic list group */
    gridLayout = new QGridLayout(arithmeticGroup);
    updateArithmeticLayout();

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(arithmeticGroup);
    scrollArea->setStyleSheet("width: 30");
    scrollArea->setWidgetResizable(true);

    /* main layout */
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabBar);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 10, 5, 5);
    setLayout(mainLayout);
}
