#include "aiinputparams.h"

AIInputParams::AIInputParams(QWidget *parent) :
    QWidget(parent)
{
    m_config = g_Config::getInstance();

    createPage();

    connectSigAndSlt();
}

/*!
 * \brief 创建页面
 */
void AIInputParams::createPage()
{
    //! 布局管理器
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QGridLayout *upLay = new QGridLayout;
    upLay->setVerticalSpacing(50);
    QHBoxLayout *downLay = new QHBoxLayout;

    //! 页面控件
    m_tabBar = new QTabBar;
    QFont font;
    font.setPixelSize(24);
    m_tabBar->setFont(font);
//    m_tabBar->setStyleSheet("QTabBar::tab{""height:40;width:90;}");
    m_tabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}::scroller{""width:30;}");

//    m_tabBar->setFont(m_config->getFont());

    QStringList strList;
    strList << "1" << "2" << "3"
            << "4" << "5" << "6"
            << "7" << "8" << "9"
            << "10";
    for (int i = 0; i < AI_PARAMS_NUM; i++) {
        m_labels[i] = new myLabel(strList.at(i));
        m_labels[i]->setFixedHeight(BTN_HEIGHT);
        m_labels[i]->setAlignment(Qt::AlignCenter);
        m_lineEdits[i] = new myLineEdit;
        m_lineEdits[i]->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
        upLay->addWidget(m_labels[i], i/3, (i%3)*2);
        upLay->addWidget(m_lineEdits[i], i/3, (i%3)*2+1);
    }

    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    downLay->addStretch();
    downLay->addWidget(m_backBtn);
    mainLay->addSpacing(30);
    mainLay->addWidget(m_tabBar, 0, Qt::AlignHCenter);
    mainLay->addSpacing(70);
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);
}

/*!
 * \brief 连接信号与槽
 */
void AIInputParams::connectSigAndSlt()
{
    m_sigMap = new QSignalMapper(this);
    for (int i = 0; i < AI_PARAMS_NUM; i++) {
        m_sigMap->setMapping(m_lineEdits[i], i);
        connect(m_lineEdits[i], SIGNAL(pressed()), m_sigMap, SLOT(map()));
    }
    connect(m_sigMap, SIGNAL(mapped(int)), this, SLOT(onLineEditPressedSlt(int)));

    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabBarIndexChangedSlt()));
    connect(m_backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnPressed()));
}

/*!
 * \brief 刷新显示
 */
void AIInputParams::refreshDisplay()
{
    m_nIndexOfArithIntel = struGsh.curIntelArithNo;
    refreshTabBarDisplay();

    refreshParamsDisplay();
}

/*!
 * \brief 刷新QTabBar的显示
 */
void AIInputParams::refreshTabBarDisplay()
{
    //! 清空QTabBar
    while (m_tabBar->count()) {
        m_tabBar->removeTab(0);
    }

    //! 增加tab至QTabBar
    int nTotal = struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal;
    for (int i = 0; i < nTotal; i++) {
        m_tabBar->addTab(myString.sIdentifyGroupName[ONE_LEVEL][i]);
    }
//    m_tabBar->setFixedWidth(nTotal * QTABBAR_TAB_WIDTH);

    //! 根据当前识别组参数关系设置使能
    switch (struCnfp.nMatAssembleMode) {
    case PARAMS_ALL_SEPARATE:       //! 全部独立
        for (int i = 0; i < nTotal; i++) {
            m_tabBar->setTabEnabled(i, true);
        }
        break;
    case PARAMS_ALL_SAME:           //! 全部相同
        m_tabBar->setTabEnabled(FIRST_FRONT, true);
        for (int i = 1; i < nTotal; i++) {
            m_tabBar->setTabEnabled(i, false);
        }
        break;
    case PARAMS_FRONT_REAR_SAME:    //! 前后视相同
        for (int i = 0; i < nTotal; i++) {
            if (i%2 == 0) {
                m_tabBar->setTabEnabled(i, true);
            } else {
                m_tabBar->setTabEnabled(i, false);
            }
        }
        break;
    case PARAMS_FIRST_SECOND_SAME:  //! 一二三次相同前后视不同
        m_tabBar->setTabEnabled(FIRST_FRONT, true);
        m_tabBar->setTabEnabled(FIRST_REAR, true);
        for (int i = 2; i < nTotal; i++) {
            m_tabBar->setTabEnabled(i, false);
        }
        break;
    default:
        break;
    }

    //! 设置当前识别组为一次前视
    m_tabBar->setCurrentIndex(FIRST_FRONT);
}

/*!
 * \brief 刷新智能参数的显示
 */
void AIInputParams::refreshParamsDisplay()
{
    int nId = m_tabBar->currentIndex();

    //! 记录符号位
    QVector <int> vSign;
    vSign << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoR[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoG[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoB[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoRG[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoRB[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoGB[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoRR[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoGG[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoBB[0]
             << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nConsDfl[0];

    //! 记录数据
    QVector <int> vVal;
    vVal << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoR[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoG[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoB[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoRG[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoRB[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoGB[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoRR[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoGG[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nQuoBB[1]
            << struCnfp.struGroupIdentify[ONE_LEVEL][nId].struIntel[m_nIndexOfArithIntel].nConsDfl[1];

    for (int i = 0; i < AI_PARAMS_NUM; i++) {
        if (vSign.at(i) == 0) { //! 正数
            m_lineEdits[i]->setText(QString("%1").arg(vVal.at(i)));
        } else {
            m_lineEdits[i]->setText(QString("%1").arg(-vVal.at(i)));
        }
    }
}

/*!
 * \brief 设置智能参数
 * \param nIndex 参数索引
 */
void AIInputParams::setAIParams(int nIndex, int nVal)
{
    int nId = m_tabBar->currentIndex();
    int nSign = (nVal > 0) ? 0 : 1;
    int nTmp = abs(nVal);

    //! 根据当前算法组合形式获取需要拷贝参数的识别组
    QVector <int> vGroups;
    vGroups.clear();
    switch (struCnfp.nMatAssembleMode) {
    case PARAMS_ALL_SEPARATE:       //! 全部独立
        vGroups.push_back(nId);
        break;
    case PARAMS_ALL_SAME:           //! 全部相同
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal; i++) {
            vGroups.push_back(i);
        }
        break;
    case PARAMS_FRONT_REAR_SAME:    //! 前后视相同
        vGroups.push_back(nId);
        vGroups.push_back(nId+1);
        break;
    case PARAMS_FIRST_SECOND_SAME:  //! 一二三次相同前后视不同
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal/2; i++) {
            if (nId%2 == 0) {
                vGroups.push_back(i*2);
            } else {
                vGroups.push_back(i*2+1);
            }
        }
        break;
    default:
        break;
    }

    switch (nIndex) {
    case AI_PARAMS_R:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoR[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoR[1] = nTmp;
        }
        break;
    case AI_PARAMS_G:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoG[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoG[1] = nTmp;
        }
        break;
    case AI_PARAMS_B:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoB[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoB[1] = nTmp;
        }
        break;
    case AI_PARAMS_RG:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoRG[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoRG[1] = nTmp;
        }
        break;
    case AI_PARAMS_RB:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoRB[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoRB[1] = nTmp;
        }
        break;
    case AI_PARAMS_GB:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoGB[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoGB[1] = nTmp;
        }
        break;
    case AI_PARAMS_RR:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoRR[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoRR[1] = nTmp;
        }
        break;
    case AI_PARAMS_GG:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoGG[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoGG[1] = nTmp;
        }
        break;
    case AI_PARAMS_BB:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoBB[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nQuoBB[1] = nTmp;
        }
        break;
    case AI_PARAMS_CONS:
        for (int i = 0; i < vGroups.size(); i++) {
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nConsDfl[0] = nSign;
            struCnfp.struGroupIdentify[ONE_LEVEL][vGroups.at(i)].struIntel[m_nIndexOfArithIntel].nConsDfl[1] = nTmp;
        }
        break;
    default:
        break;
    }
}

/*!
 * \brief 槽函数: 识别组号变化
 * \param nIndex 识别组号
 */
void AIInputParams::onTabBarIndexChangedSlt()
{
    refreshParamsDisplay();
}

/*!
 * \brief 槽函数: 编辑框按下
 * \param nIndex 输入框索引号
 */
void AIInputParams::onLineEditPressedSlt(int nIndex)
{
    int nVal = m_lineEdits[nIndex]->text().toInt();
    myInputPanel *inputPanel = new myInputPanel(textType, -2147483647, 2147483647, nVal);
    inputPanel->setInputNegEn(true);

    if (inputPanel->exec() == QDialog::Accepted) {
        nVal = inputPanel->getValue();
        m_lineEdits[nIndex]->setText(QString("%1").arg(nVal));
        setAIParams(nIndex, nVal);
    }
}

/*!
 * \brief 槽函数: 返回
 */
void AIInputParams::onBackBtnPressed()
{
    emit backToHomePageSig();
}
