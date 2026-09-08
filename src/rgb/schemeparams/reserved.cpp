#include "reserved.h"

/* 构造保留参数页面 */
PageReserved::PageReserved(int nArithIndex, QWidget *parent)
    : QWidget(parent)
{

    setArithIndex(nArithIndex);

/* page init */
    createPage();

/* page update */
    updatePage();

/* page layout */
    layoutPage();
}

/* 应用当前保留参数页面设置 */
void PageReserved::setReservedParams()
{
/* write params to memory */
    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                struCnfp.struGroupIdentify[i][j].struGreyColor[m_nArithIndex-ARITH_GREY_A].struReserved = m_struReserved[i*step+j];
            }
        }
        break;
    case ARITH_PEANUT_D:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].struReserved = m_struReserved[i*step+j];
            }
        }
        break;
    case ARITH_INTEL_A:
    case ARITH_INTEL_B:
    case ARITH_INTEL_C:
    case ARITH_INTEL_D:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                struCnfp.struGroupIdentify[i][j].struIntel[m_nArithIndex-ARITH_INTEL_A].struReserved = m_struReserved[i*step+j];
            }
        }
        break;
    case ARITH_SHAPE_POLE_A:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                struCnfp.struGroupIdentify[i][j].struPole.struReserved = m_struReserved[i*step+j];
            }
        }
        break;
    }

/* write params to file */
    switch(struCnfe.nMachine) {
    case MACHINE_CF:
        myFlow.materialCopyAssemble(0, tabBar->currentIndex(), 0, m_nArithIndex, 1);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, tabBar->currentIndex(), 0, m_nArithIndex, 1);
        break;
    default:
        break;
    }

/* write params to fpga */
}

/* 重置当前保留参数页面设置 */
void PageReserved::resetReservedParams()
{
/* reset params */
    switch (m_nArithIndex) {
    case ARITH_GREY_A:
    case ARITH_GREY_B:
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                m_struReserved[i*step+j] = struCnfp.struGroupIdentify[i][j].struGreyColor[m_nArithIndex-ARITH_GREY_A].struReserved;
            }
        }
        break;
    case ARITH_PEANUT_D:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                m_struReserved[i*step+j] = struCnfp.struGroupIdentify[i][j].struPeanutAbcd[m_nArithIndex-ARITH_PEANUT_A].struReserved;
            }
        }
        break;
    case ARITH_INTEL_A:
    case ARITH_INTEL_B:
    case ARITH_INTEL_C:
    case ARITH_INTEL_D:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                m_struReserved[i*step+j] = struCnfp.struGroupIdentify[i][j].struIntel[m_nArithIndex-ARITH_INTEL_A].struReserved;
            }
        }
        break;
    case ARITH_SHAPE_POLE_A:
        for (int i = 0; i < struCnfg.nLevelTotal; i++) {
            int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
            for (int j = 0; j < step; j++) {
                m_struReserved[i*step+j] = struCnfp.struGroupIdentify[i][j].struPole.struReserved;
            }
        }
        break;
    }

/* reset page */
    emit pageUpdated();
}

/* 设置当前保留参数页面对应算法索引号 */
void PageReserved::setArithIndex(int index)
{
    m_nArithIndex = index;
}

/* 获取当前保留页面对应算法索引号 */
int PageReserved::getArithIndex()
{
    return m_nArithIndex;
}

void PageReserved::setComponentsHidden(QList<int> &component)
{
    foreach(int tmp, component) {
        if (!hideComponent.contains(tmp)) {
            hideComponent << tmp;
        }
    }
}

void PageReserved::setTabBarIndex(int index)
{
    tabBar->setCurrentIndex(index);
}

/* 响应保留页面杂质名称设置 */
void PageReserved::setImpurityNameSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_rename_impurity);

    if (msgBox.exec() == QDialog::Accepted) {
        myInputMethod inputDlg(myLan.msg_input_name, nameValue->text());
        if (inputDlg.exec() == QDialog::Accepted) {
            QString text = inputDlg.getText();
            /* update button text */
            nameValue->setText(text);
        }
    }
}

/* 响应颜色切换设置 */
void PageReserved::setColorModeValueSlt(int btnId){
    int tabIndex, sens, mode, color;
    QString text;

    tabIndex = tabBar->currentIndex();
    color = colorModeRadio[btnId]->currentIndex()/2;
    mode = colorModeRadio[btnId]->currentIndex()%2;

    switch (btnId) {
    case 0:
        m_struReserved[tabIndex].nDiscolor1 = color;
        m_struReserved[tabIndex].nMode1 = mode;
        sens = mode == 0 ? m_struReserved[tabIndex].nSensMin1: m_struReserved[tabIndex].nSensMax1;
        text.sprintf("%.1f%%", sens*0.1);
        sensValue[btnId]->setText(text);
        break;
    case 1:
        m_struReserved[tabIndex].nColor2 = 1 << color;
        m_struReserved[tabIndex].nMode2 = mode;
        sens = mode == 0 ? m_struReserved[tabIndex].nSensMin2: m_struReserved[tabIndex].nSensMax2;
        text.sprintf("%.1f%%", sens*0.1);
        sensValue[btnId]->setText(text);
        break;
    }

//    printf("btnId:%d\n",btnId);
//    printf("color:%d\n",color);
//    printf("mode:%d\n",mode);


}

/* 响应颜色切换设置 */
void PageReserved::setColorValueSlt(int btnId)
{
    int color;
    int tabIndex = tabBar->currentIndex();


//    printf("%d\n",tabIndex);
    color = colorValue[btnId]->currentIndex();
    switch (btnId) {
    case 0:
        m_struReserved[tabIndex].nDiscolor1 = color;
        break;
    case 1:
        m_struReserved[tabIndex].nColor2 = 1 << color;
        break;
    }
}

/* 响应模式切换设置 */
void PageReserved::setModeValueSlt(int btnId)
{
    int tabIndex, sens, mode;
    QString text;

    tabIndex = tabBar->currentIndex();

    mode = modeValue[btnId]->currentIndex();
    switch (btnId) {
    case 0:
        m_struReserved[tabIndex].nMode1 = mode;
        sens = mode == 0 ? m_struReserved[tabIndex].nSensMin1: m_struReserved[tabIndex].nSensMax1;
        text.sprintf("%.1f%%", sens*0.1);
        sensValue[btnId]->setText(text);
        break;
    case 1:
        m_struReserved[tabIndex].nMode2 = mode;
        sens = mode == 0 ? m_struReserved[tabIndex].nSensMin2: m_struReserved[tabIndex].nSensMax2;
        text.sprintf("%.1f%%", sens*0.1);
        sensValue[btnId]->setText(text);
        break;
    }
}

/* 响应灵敏度参数设置 */
void PageReserved::setSensValueSlt(int btnId)
{
    inputType type;
    double tmp, min, max;
    int tabIndex, mode;
    int *pSens;
    QString text;

    tabIndex = tabBar->currentIndex();

    switch (btnId) {
    case 0:
        mode  = m_struReserved[tabIndex].nMode1;
        tmp   = mode == 0 ? m_struReserved[tabIndex].nSensMin1*0.1 : m_struReserved[tabIndex].nSensMax1*0.1;
        pSens = mode == 0 ? &m_struReserved[tabIndex].nSensMin1 : &m_struReserved[tabIndex].nSensMax1;
        min   = 0;
        max   = 100;
        type  = floatType;
        break;
    case 1:
        mode = m_struReserved[tabIndex].nMode2;
        tmp   = mode == 0 ? m_struReserved[tabIndex].nSensMin2*0.1 : m_struReserved[tabIndex].nSensMax2*0.1;
        pSens = mode == 0 ? &m_struReserved[tabIndex].nSensMin2 : &m_struReserved[tabIndex].nSensMax2;
        min   = 0;
        max   = 100;
        type  = floatType;
        break;
    }

    myInputPanel inputDlg(type, min, max, tmp);

    if (inputDlg.exec() == QDialog::Accepted) {
        tmp = inputDlg.getValue();

        /* update text */
        *pSens = tmp*10;
        text.sprintf("%.1f%%", tmp);
        sensValue[btnId]->setText(text);

        /* write params */
        setReservedParams();
    }
}

/* 响应病斑数设置 */
void PageReserved::setPercentValueSlt(int btnId)
{
    int type, tmp, min, max, *pSens;
    int tabIndex = tabBar->currentIndex();


    switch (m_nArithIndex) {
    case ARITH_SHAPE_POLE_A:
        type = intType;
        min = 0;
        max = 255;
        tmp = btnId == 1 ? m_struReserved[tabIndex].nSensMin1 : m_struReserved[tabIndex].nSensMax1;
        pSens = btnId == 1 ? &m_struReserved[tabIndex].nSensMin1 : &m_struReserved[tabIndex].nSensMax1;
        break;
    case ARITH_GREY_A:
    case ARITH_GREY_B:
    case ARITH_DISCOLOR_A:
    case ARITH_DISCOLOR_B:
    case ARITH_INTEL_A:
    case ARITH_INTEL_B:
    case ARITH_INTEL_C:
    case ARITH_INTEL_D:
    case ARITH_PEANUT_D:
        type = intType;
        min = 1;
        max = 512;
        tmp = 0;
        pSens = btnId == 1 ? &m_struReserved[tabIndex].nSensMin1 : &m_struReserved[tabIndex].nSensMax1;
        break;
    }
    myInputPanel inputDlg(intType, min, max, tmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        tmp = inputDlg.getValue();
        *pSens = tmp;
        percentValue[btnId]->setText(QString("%1").arg(tmp));

        /* write params */
        setReservedParams();
    }
}

/* 响应tab页面切换 */
void PageReserved::changeCurrentSlt(int index1)
{
    QString text;
    int index = index1;
    if (index < 0) {
        return;
    }

//    int index = index1;
    /* name */
    nameValue->setText(QString("%1").arg(getReservedText(m_nArithIndex)));

    /* reserved */
    /* color */
    colorValue[0]->setCurrentIndex(m_struReserved[index].nDiscolor1);
    colorValue[1]->setCurrentIndex(m_struReserved[index].nColor2/2);

    /* mode */
    modeValue[0]->setCurrentIndex(m_struReserved[index].nMode1);
    modeValue[1]->setCurrentIndex(m_struReserved[index].nMode2);

    colorModeRadio[0]->setCurrentIndex(m_struReserved[index].nDiscolor1*2+m_struReserved[index].nMode1);
    colorModeRadio[1]->setCurrentIndex(m_struReserved[index].nColor2/2*2+m_struReserved[index].nMode2);

    /* sens */
    switch (m_struReserved[index].nMode1) {
    case 0:
        text.sprintf("%.1f%%", m_struReserved[index].nSensMin1*0.1);
        sensValue[0]->setText(text);
        break;
    case 1:
        text.sprintf("%.1f%%", m_struReserved[index].nSensMax1*0.1);
        sensValue[0]->setText(text);
        break;
    }

    switch (m_struReserved[index].nMode2) {
    case 0:
        text.sprintf("%.1f%%", m_struReserved[index].nSensMin2*0.1);
        sensValue[1]->setText(text);
        break;
    case 1:
        text.sprintf("%.1f%%", m_struReserved[index].nSensMax2*0.1);
        sensValue[1]->setText(text);
        break;
    }

    /* percent */
    text = QString("%1").arg(m_struReserved[index].nSensMax1); // 临时借用参数
    percentValue[0]->setText(text);
    text = QString("%1").arg(m_struReserved[index].nSensMin1); // 临时借用参数
    percentValue[1]->setText(text);
}

/* 初始化保留参数页面 */
void PageReserved::createPage()
{
    /* tab index */
    tabBar = new MyTabBar();

/* params area */
    /* name group */
    nameGroup  = new myGroupBox(myLan.defect_name);
    nameValue  = new myLineEdit(myLan.reserved);
    nameValue->setAlignment(Qt::AlignCenter);
    nameValue->setMinimumHeight(ICON_HEI);
    nameValue->setMaximumHeight(BTN_HEIGHT);

    /* color & mode group */
    QString     reservedList[2] = {myLan.reserved_1,
                                   myLan.reserved_2};
    QStringList colorList[2] = {QStringList() << myLan.red_green
                                              << myLan.red_blue
                                              << myLan.green_blue,
                                QStringList() << myLan.red
                                              << myLan.green
                                              << myLan.blue};
    QStringList modeList = QStringList()  << myLan.dark
                                          << myLan.light;

//    QStringList colorModeList[2] = {QStringList()   << "红<绿"
//                                                    << "红>绿"
//                                                    << "红<蓝"
//                                                    << "红>蓝"
//                                                    << "绿<蓝"
//                                                    << "绿>蓝",
//                                      QStringList() << "红低"
//                                                    << "红高"
//                                                    << "绿低"
//                                                    << "绿高"
//                                                    << "蓝低"
//                                                    << "蓝高"};
    QStringList colorModeList[2] = {QStringList()   << myLan.red+"<"+myLan.green
                                                    << myLan.red+">"+myLan.green
                                                    << myLan.red+"<"+myLan.blue
                                                    << myLan.red+">"+myLan.blue
                                                    << myLan.green+"<"+myLan.blue
                                                    << myLan.green+">"+myLan.blue,
                                      QStringList() << myLan.red+myLan.lower
                                                    << myLan.red+myLan.upper
                                                    << myLan.green+myLan.lower
                                                    << myLan.green+myLan.upper
                                                    << myLan.blue+myLan.lower
                                                    << myLan.blue+myLan.upper};

    /* sens */
    for (int i = 0; i < 2; i++) {
        reservedGroup.append(new myGroupBox(""));
        reservedGroup[i]->setTitle(reservedList[i]);

        colorValue.append(new MyButtonGroup(colorList[i]));
        modeValue.append(new MyButtonGroup(modeList));
        colorModeRadio.append(new MyButtonGroup(colorModeList[i]));
        colorModeRadio[i]->setMinimumWidth(BTN_WIDTH*4);
        colorValue[i]->setMinimumHeight(ICON_HEI);
        modeValue[i]->setMinimumHeight(ICON_HEI);

        sensLabel.append(new myLabel(myLan.sensitivity));
        sensValue.append(new myLineEdit(tr("")));
        sensValue[i]->setMinimumHeight(ICON_HEI);
        sensValue[i]->setMaximumHeight(BTN_HEIGHT);
        sensValue[i]->setMaximumWidth(BTN_WIDTH);

        percentLabel.append(new myLabel(myLan.purity));
        percentValue.append(new myLineEdit(tr("")));
        percentValue[i]->setMinimumHeight(ICON_HEI);
        percentValue[i]->setMaximumHeight(BTN_HEIGHT);
        percentValue[i]->setMaximumWidth(BTN_WIDTH);
    }

    /* signals */
    colorMapper = new QSignalMapper;
    for (int i = 0; i < 2; i++) {
        colorMapper->setMapping(colorValue[i], i);
        connect(colorValue[i], SIGNAL(currentChanged(int)), colorMapper, SLOT(map()));
    }
    connect(colorMapper, SIGNAL(mapped(int)), this, SLOT(setColorValueSlt(int)));

    colorModeMapper = new QSignalMapper;
    for (int i = 0; i < 2; i++) {
        colorModeMapper->setMapping(colorModeRadio[i], i);
        connect(colorModeRadio[i], SIGNAL(currentChanged(int)), colorModeMapper, SLOT(map()));
    }
    connect(colorModeMapper, SIGNAL(mapped(int)), this, SLOT(setColorModeValueSlt(int)));

    modeMapper = new QSignalMapper;
    for (int i = 0; i < 2; i++) {
        modeMapper->setMapping(modeValue[i], i);
        connect(modeValue[i], SIGNAL(currentChanged(int)), modeMapper, SLOT(map()));
    }
    connect(modeMapper, SIGNAL(mapped(int)), this, SLOT(setModeValueSlt(int)));

    sensMapper = new QSignalMapper;
    for (int i = 0; i < 2; i++) {
        sensMapper->setMapping(sensValue[i], i);
        connect(sensValue[i], SIGNAL(pressed()), sensMapper, SLOT(map()));
    }
    connect(sensMapper, SIGNAL(mapped(int)), this, SLOT(setSensValueSlt(int)));

    percentMapper = new QSignalMapper;
    for (int i = 0; i < 2; i++) {
        percentMapper->setMapping(percentValue[i], i);
        connect(percentValue[i], SIGNAL(pressed()), percentMapper, SLOT(map()));
    }
    connect(percentMapper, SIGNAL(mapped(int)), this, SLOT(setPercentValueSlt(int)));

    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(this, SIGNAL(pageUpdated()), this, SLOT(updatePage()));
}

/* 更新保留参数页面 */
void PageReserved::updatePage()
{
    /* tab bar */
    updateTabBar(tabBar, true, false);

    /* reserved */
    switch (m_nArithIndex) {
    case ARITH_SHAPE_POLE_A:
        percentLabel[0]->hide();
        percentValue[0]->hide();
        percentLabel[1]->show();
        percentValue[1]->show();
        reservedGroup[0]->hide();
        break;
    default:
        percentLabel[0]->hide();
        percentValue[0]->hide();
        percentLabel[1]->hide();
        percentValue[1]->hide();
        reservedGroup[0]->show();
        break;
    }

    colorValue[0]->hide();
    modeValue[0]->hide();
    colorValue[1]->hide();
    modeValue[1]->hide();

    foreach(int tmp, hideComponent) {
        switch(tmp) {
        case COMPONENT_TABBAR:
            tabBar->hide();
            break;
        default:
            break;
        }
    }
}

/* 布局保留参数页面 */
void PageReserved::layoutPage()
{
/* page layout */
    /* page layout */
    /* name group layout */
    QHBoxLayout *nameLayout = new QHBoxLayout(nameGroup);
    nameLayout->addWidget(nameValue);

    /* color & mode group layout */
    QVector<QGridLayout *> modeLayout;

    QVector<QHBoxLayout *> sensLayout;
    QVector<QVBoxLayout *> reservedLayout;
    for (int i = 0; i < 2; i++) {
        modeLayout.append(new QGridLayout);
        modeLayout[i]->addWidget(colorValue[i], 0,0, Qt::AlignLeft);
        modeLayout[i]->addWidget(modeValue[i], 0,1, Qt::AlignRight);
        modeLayout[i]->addWidget(colorModeRadio[i], 1,0, Qt::AlignCenter);
        colorValue[i]->hide();
        modeValue[i]->hide();

        sensLayout.append(new QHBoxLayout);
        sensLayout[i]->addWidget(sensLabel[i]);
        sensLayout[i]->addSpacing(10);
        sensLayout[i]->addWidget(sensValue[i]);
        sensLayout[i]->addStretch(1);
        sensLayout[i]->addWidget(percentLabel[i]);
        sensLayout[i]->addSpacing(10);
        sensLayout[i]->addWidget(percentValue[i]);

        reservedLayout.append(new QVBoxLayout(reservedGroup[i]));
        reservedLayout[i]->addLayout(modeLayout[i]);
        reservedLayout[i]->addLayout(sensLayout[i]);
        reservedLayout[i]->setContentsMargins(10, 0, 10, 10);
    }

    /* sens group layout */

    /* reserved group layout */

    /* main layout */
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(tabBar,        0, 0);
    mainLayout->addWidget(nameGroup,        1, 0);
    mainLayout->addWidget(reservedGroup[0], 2, 0);
    mainLayout->addWidget(reservedGroup[1], 3, 0);

    setLayout(mainLayout);
}
