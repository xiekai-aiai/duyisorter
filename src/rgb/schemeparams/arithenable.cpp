#include "arithenable.h"

/**
 * @brief PageIdentify::PageIdentify
 * 构造识别参数页面
 * @param parent
 */
PageIdentify::PageIdentify(QWidget *parent)
    : QWidget(parent)
{
/* import global params */
    resetIdentifyParams();

/* page init */
    createPage();

/* page layout */
    layoutPage();

/* page update */
    updatePage();
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
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfe.nArithmeticTotal; j++) {
            m_nArithmeticEnable[i][j] = struCnfp.nArithmeticEnable[j];
        }
    }

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
    if (struCnfp.nMatAssembleMode != m_nMatMode) {
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
    for (i = 0; i < arithmeticBox.size(); i++) {
        if (arithmeticBox[i]->isChecked()) {
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
    memcpy(struCnfp.nArithmeticEnable, m_nArithmeticEnable, sizeof(struCnfp.nArithmeticEnable));
}

/*!
 * \brief PageIdentify::getMatAssembleModeList
 * 获取参数组合类型列表
 * \return
 */
QList<int> PageIdentify::getMatAssembleModeList()
{
    QList<int> list;

    switch (struCnfe.nMachine) {
    case MACHINE_CF:
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
    switch (mode) {
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
    struCnfp.nMatAssembleMode =  m_nMatMode;

    /*matModeCombo切换时，识别组参数立即修改*/
    myFlow.materialCopyAssemble(0, 0, 1, 0, 0);

}

/**
 * @brief PageIdentify::setArithButton
 * 响应算法列表勾选框
 * @param btnId
 */
void PageIdentify::setArithButton(int btnId)
{
    /* enable arithmetic buttons */
    if (arithmeticBox[btnId]->isChecked()) {
        if (arithmeticSeq[btnId] == ARITH_WATERMELON) {
            //! 使能西瓜子算法可同时使能智能A
            for (int i = 0; i < arithmeticSeq.size(); i++) {
                if (i == btnId || arithmeticSeq[i] == ARITH_INTEL_A) {
                    continue;
                }
                arithmeticBox[i]->setEnabled(false);
                arithmeticBox[i]->setChecked(false);
                arithmeticBtn[i]->setEnabled(false);
                m_nArithmeticEnable[struGsh.nLevel][arithmeticSeq[i]] = 0;
            }
            m_nArithmeticEnable[struGsh.nLevel][arithmeticSeq[btnId]] = 1;
            arithmeticBtn[btnId]->setEnabled(true);
        } else {
            //! 智能A使能不受西瓜子算法使能影响
            if (m_nArithmeticEnable[struGsh.nLevel][ARITH_WATERMELON] == 0
                    || arithmeticSeq[btnId] == ARITH_INTEL_A) {
                m_nArithmeticEnable[struGsh.nLevel][arithmeticSeq[btnId]] = 1;
                arithmeticBtn[btnId]->setEnabled(true);
            } else {
                for (int i = 0; i < arithmeticSeq.size(); i++) {
                    if (arithmeticSeq[i] == ARITH_WATERMELON) {
                        continue;
                    }
                    arithmeticBox[i]->setEnabled(false);
                    arithmeticBox[i]->setChecked(false);
                    arithmeticBtn[i]->setEnabled(false);
                    m_nArithmeticEnable[struGsh.nLevel][arithmeticSeq[i]] = 0;
                }
            }
        }
    } else {
        if (arithmeticSeq[btnId] == ARITH_WATERMELON) {
            for (int i = 0; i < arithmeticSeq.size(); i++) {
                arithmeticBox[i]->setEnabled(true);
            }
        }
        m_nArithmeticEnable[struGsh.nLevel][arithmeticSeq[btnId]] = 0;
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
    connect(tabBar,         SIGNAL(currentChanged(int)),        this, SLOT(changeCurrentSlt(int)));
    connect(matModeCombo,   SIGNAL(currentIndexChanged(int)),   this, SLOT(setMatModeCombo(int)));
    connect(this,           SIGNAL(pageUpdated()),              this, SLOT(updatePage()));
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
    for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
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
    if (myFlow.getProductLineNo() == 0) {
        for(int i = 0; i < 2; i++) {
            /* append hardcode arith list */
            arithmeticBox.append(new MyCheckBox("", this));
            arithmeticBtn.append(new myPushButton("", QIcon(), true, this));
            arithmeticSeq.append(ARITH_INTEL_A+i);
        }
    }

    /* signal mapper */
    sigBtnMapper = new QSignalMapper(this);
    sigBoxMapper = new QSignalMapper(this);

    for (int i = 0; i < arithmeticBox.size(); i++) {
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
    for (int i = 0; i < arithmeticSeq.size(); i++) {
        arithmeticBtn[i]->setText(myString.sArithmeticName[arithmeticSeq[i]]);
    }
}

/**
 * @brief PageIdentify::updateArithmeticState
 * 更新算法列表状态
 */
void PageIdentify::updateArithmeticState()
{
    for (int i = 0; i < arithmeticBtn.size(); i++) {
        if (struCnfe.nArithmeticSeq[arithmeticSeq[i]] == 1 ) {
            arithmeticBox[i]->show();
            arithmeticBtn[i]->show();

            if (m_nArithmeticEnable[tabBar->currentIndex()][arithmeticSeq[i]] == 1) {
                arithmeticBox[i]->setChecked(true);
                arithmeticBtn[i]->setEnabled(true);
                //只展示，没法使能，同时置算法使能为否
                if(struCnfg.aiEnable != 1){
                    if(arithmeticSeq[i] ==  ARITH_PISTACHIO){
                        arithmeticBox[i]->setChecked(false);
                        arithmeticBox[i]->setEnabled(false);
                        arithmeticBtn[i]->setEnabled(false);
                        struCnfp.nArithmeticEnable[ARITH_PISTACHIO] == 0;
                    }
                }else{
                    arithmeticBox[i]->setEnabled(true);
                }
            } else {
                arithmeticBox[i]->setChecked(false);
                arithmeticBtn[i]->setEnabled(false);
                //只展示，没法使能，同时置算法使能为否
                if(struCnfg.aiEnable != 1){
                  if(arithmeticSeq[i] ==  ARITH_PISTACHIO){
                      arithmeticBox[i]->setChecked(false);
                      arithmeticBox[i]->setEnabled(false);
                      arithmeticBtn[i]->setEnabled(false);
                      struCnfp.nArithmeticEnable[ARITH_PISTACHIO] == 0;
                  }
                }else{
                  arithmeticBox[i]->setEnabled(true);
                }
            }
        } else {
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
    for (int i = 0; i < list.count(); i++) {
        matModeCombo->addItem(getMatAssembleModeName(list.at(i)), list.at(i));
    }

    // 匹配当前物料参数组合
    int index = matModeCombo->findData(mode);
    if (index < 0) {
        index = matModeCombo->count()-1;
    }
    connect(matModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setMatModeCombo(int)));
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

    switch (struCnfe.nMachine) {
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
    tabBar->setCurrentIndex(0);
    /* material assemble mode */
    updateMatModeCombo(m_nMatMode);
}

/**
 * @brief PageIdentify::updateArithmeticLayout
 * 更新算法列表布局
 */
void PageIdentify::updateArithmeticLayout()
{
    /* clear arithmetic item layout */
    arithmeticLayout.clear();

    for (int i = 0; i < arithmeticBox.size(); i++) {
        arithmeticLayout.append(new QFormLayout);
        arithmeticLayout[i]->addRow(arithmeticBox[i], arithmeticBtn[i]);
    }

    /* clear arithmetic list layout */
    for (int i = gridLayout->count()-1; i >= 0; i--) {
        gridLayout->removeItem(gridLayout->itemAt(i));
    }

    /* add arithmetic item to grid layout */

    gridLayout->addWidget(matModeCombo, 0, 0, 1, 2);
    for (int i = 0; i < arithmeticBox.size(); i++) {
        gridLayout->addLayout(arithmeticLayout[i], i/2+1, i%2, Qt::AlignRight);
    }

#if 0
    gridLayout->setColumnStretch(0,4);
    gridLayout->setColumnStretch(1,1);
    gridLayout->setColumnStretch(2,4);

    gridLayout->addWidget(matModeCombo, 0, 0, 1, 3);
    gridLayout->setSpacing(25);
    for (int i = 0; i < arithmeticBox.size(); i++) {
        if(i%2==0)
            gridLayout->addLayout(arithmeticLayout[i], i/2+1, 0, Qt::AlignRight);
        else
            gridLayout->addLayout(arithmeticLayout[i], i/2+1, 2, Qt::AlignRight);

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

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(arithmeticGroup);
    scrollArea->setStyleSheet("width: 30");
    scrollArea->setWidgetResizable(true);

    /* main layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabBar);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0,10,5,5);
    setLayout(mainLayout);
}
