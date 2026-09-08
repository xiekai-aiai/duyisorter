#include "upgradefpgawidget.h"

UpgradeFPGAWidget::UpgradeFPGAWidget(QWidget *parent) :
    QWidget(parent)
{
    initLocalParams();

    createPage();

    connectSigAndSlt();

    refreshDisplay();
}

/*!
 * \brief 初始化本地参数
 */
void UpgradeFPGAWidget::initLocalParams()
{
    initUpdateParams();

    initTextParams();
}

/*!
 * \brief 初始化升级参数
 */
void UpgradeFPGAWidget::initUpdateParams()
{
    for (int i = 0; i < BOARD_TYPE; i++) {
        m_struInfo[i].bEn = false;
        m_struInfo[i].nBoard = 0;
        m_struInfo[i].nBoardTotal = 1;
        m_struInfo[i].nMode = 1;
        m_struInfo[i].sFileName = myLan.select_file+"...";
        m_struInfo[i].nWriteLen = 0;
        for (int j = 0; j < MAX_UNIT_PER_INT; j++) {
            m_struInfo[i].nStatus[j] = UPGRADE_UNSTARTED;
            m_struInfo[i].sUpgradeInfo[j]="";
        }
        m_struInfo[i].qvIntUnit << 0x01;
        m_struInfo[i].qvMajorUnit << 0x00;
        m_struInfo[i].qvAssistUnit << 0x00;
    }
}

/*!
 * \brief 初始化板类型文本
 */
void UpgradeFPGAWidget::initTextParams()
{
    BoardTypeNameList << myLan.interface_board << myLan.config_major+myLan.front_view
                      << myLan.config_major+myLan.rear_view << myLan.config_assist+myLan.front_view
                      << myLan.config_assist+myLan.rear_view;

    IntStepInfoList << myLan.msg_int_to_factory << myLan.msg_int_download_file
                    << myLan.msg_int_burn_file << myLan.msg_int_to_user << " ";

    MajorStepInfoList << myLan.msg_int_to_factory << myLan.msg_major_file_to_int
                      << myLan.msg_major_to_factory << myLan.msg_major_download_file
                      << myLan.msg_major_burn_file << myLan.msg_major_to_user
                      << myLan.msg_int_to_user << " ";

    SlaveStepInfoList << myLan.msg_int_to_factory << myLan.msg_assist_file_to_int
                      << myLan.msg_major_to_factory << myLan.msg_assist_to_factory
                      << myLan.msg_assist_file_download_to_major << myLan.msg_assist_download_file
                      << myLan.msg_assist_burn_file << myLan.msg_assist_to_user
                      << myLan.msg_color_to_user << myLan.msg_int_to_user << " ";
}

/*!
 * \brief 创建页面
 */
void UpgradeFPGAWidget::createPage()
{
    QFont font;
    font.setPixelSize(20);
    //! 布局管理器
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *upLay = new QHBoxLayout;
    QGridLayout *upLeftLay = new QGridLayout;
    QHBoxLayout *upRightLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    upLeftLay->setSpacing(15);

    //! 左上侧板使能、需升级板及升级文件选择控件
    for (int i = 0; i < BOARD_TYPE; i++) {
        m_selBtns[i] = new myPushButton(QIcon(),QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));
        m_typeLbls[i] = new myLabel(BoardTypeNameList[i]);
        m_typeLbls[i]->setFixedSize(DEFAULT_BTN_WIDTH,BTN_HEIGHT);
        m_boardComboBox[i] = new MyComboBox;
        m_boardComboBox[i]->setIconSize(QSize(1, 45));
        m_modeComboBox[i] = new MyComboBox;
        m_modeComboBox[i]->setIconSize(QSize(1, 45));
        m_fileLineEdits[i] = new myLineEdit(myLan.select_file+"...");
        m_fileLineEdits[i]->setFixedHeight(BTN_HEIGHT);
        m_fileLineEdits[i]->setFont(font);
        upLeftLay->addWidget(m_selBtns[i], i, 0);
        upLeftLay->addWidget(m_typeLbls[i], i, 1);
        upLeftLay->addWidget(m_boardComboBox[i], i, 2);
        upLeftLay->addWidget(m_modeComboBox[i], i, 3);
        upLeftLay->addWidget(m_fileLineEdits[i], i, 4);
    }

    //! 右上侧升级按钮
    m_upgradeBtn = new myPushButton(myLan.upgrade,myIcon.Update_Chip);
    m_upgradeBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    upRightLay->addWidget(m_upgradeBtn, 0, Qt::AlignCenter);

    //! 下侧结果显示控件
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setStyleSheet("color: #036eb8; width: 30");
    m_resultLbl = new myLabel;
    m_resultLbl->setFixedWidth(LCD_WIDTH-100);
    scrollArea->setWidget(m_resultLbl);

    //! 返回按钮
    m_backBtn = new myPushButton(myLan.back,myIcon.Action_Back);
    m_backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    downLay->addStretch(0);
    downLay->addWidget(m_backBtn);

    upLay->addLayout(upLeftLay);
    upLay->addLayout(upRightLay);
    mainLay->addLayout(upLay);
    mainLay->addWidget(scrollArea);
    mainLay->addLayout(downLay);
}

/*!
 * \brief 连接信号与槽
 */
void UpgradeFPGAWidget::connectSigAndSlt()
{
    m_selBtnSigMap = new QSignalMapper(this);
    m_boardComboBoxSigMap = new QSignalMapper(this);
    m_modeComboBoxSigMap = new QSignalMapper(this);
    m_fileSigMap = new QSignalMapper(this);

    for (int i = 0; i < BOARD_TYPE; i++) {
        m_selBtnSigMap->setMapping(m_selBtns[i], i);
        connect(m_selBtns[i], SIGNAL(pressed()), m_selBtnSigMap, SLOT(map()));

        m_boardComboBoxSigMap->setMapping(m_boardComboBox[i], i);
        connect(m_boardComboBox[i], SIGNAL(currentIndexChanged(int)), m_boardComboBoxSigMap, SLOT(map()));

        m_modeComboBoxSigMap->setMapping(m_modeComboBox[i],i);
        connect(m_modeComboBox[i], SIGNAL(currentIndexChanged(int)), m_modeComboBoxSigMap, SLOT(map()));

        m_fileSigMap->setMapping(m_fileLineEdits[i], i);
        connect(m_fileLineEdits[i], SIGNAL(pressed()), m_fileSigMap, SLOT(map()));
    }

    connect(m_selBtnSigMap, SIGNAL(mapped(int)), this, SLOT(onSelBtnsPressed(int)));
    connect(m_boardComboBoxSigMap, SIGNAL(mapped(int)), this, SLOT(onBoardComboBoxesPressed(int)));
    connect(m_modeComboBoxSigMap, SIGNAL(mapped(int)), this, SLOT(onModeComboBoxesPressed(int)));
    connect(m_fileSigMap, SIGNAL(mapped(int)), this, SLOT(onFileLineEditsPressed(int)));
    connect(m_upgradeBtn, SIGNAL(pressed()), this, SLOT(onUpgradeBtnPressed()));
    connect(m_backBtn,SIGNAL(pressed()),this,SLOT(onBackBtnPressed()));
}

/*!
 * \brief 刷新界面显示
 */
void UpgradeFPGAWidget::refreshDisplay()
{
    initLocalParams();
    refreshBoardTotal();
    refreshSelBtnsDisplay();
    refreshBoardComboBoxDisplay();
    refreshModeComboBoxDisplay();
    refreshFileNameDisplay();
    refreshResultDisplay();
}

void UpgradeFPGAWidget::refreshBoardTotal()
{
    for(int n=BOARD_MAJOR_FRONT; n<=BOARD_ASSIST_REAR;n++)
    {
        m_struInfo[n].nBoardTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
    }

    for(int n=BOARD_MAJOR_FRONT; n<=BOARD_ASSIST_REAR;n++)
    {
        qDebug("m_struInfo[%d].nBoardTotal = %d",n,m_struInfo[n].nBoardTotal);
    }


}

/*!
 * brief 刷新板级使能的显示
 */
void UpgradeFPGAWidget::refreshSelBtnsDisplay()
{
    for(int i = 0; i < BOARD_TYPE; i++) {
        if (m_struInfo[i].bEn) {
            m_selBtns[i]->setIcon(myIcon.Action_Apply);
        } else {
            m_selBtns[i]->setIcon(QIcon());
        }
    }
}

/*!
 * \brief 刷新升级板号组合框的显示
 */
void UpgradeFPGAWidget::refreshBoardComboBoxDisplay()
{
    QPixmap pixmap(1, 45);
    pixmap.fill(Qt::transparent);
    QIcon icon(pixmap);

    //! 增加"全部"选项
    for (int i = 1; i < BOARD_TYPE; i++) {
        m_boardComboBox[i]->clear();
        m_boardComboBox[i]->addItem(icon, myLan.all);
    }
    //! 接口板(接口板不支持整机升级功能)
    m_boardComboBox[BOARD_INT]->clear();

    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++) {
        m_boardComboBox[BOARD_INT]->addItem(icon, QString("%1").arg(i+1));
    }
    for(int n=BOARD_MAJOR_FRONT; n<=BOARD_ASSIST_REAR;n++)
    {
        qDebug("m_struInfo[%d].nBoardTotal = %d",n,m_struInfo[n].nBoardTotal);
    }
    //! 主配前视相机板
    //for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2; i++) {
    qDebug("BOARD_MAJOR_FRONT nBoardTotal = %d",m_struInfo[BOARD_MAJOR_FRONT].nBoardTotal);
    for (int i = 0; i < m_struInfo[BOARD_MAJOR_FRONT].nBoardTotal; i++) {
        m_boardComboBox[BOARD_MAJOR_FRONT]->addItem(icon, QString("%1").arg(i+1));
    }
    //! 主配后视相机板
    //for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2; i++) {
    for (int i = 0; i < m_struInfo[BOARD_MAJOR_REAR].nBoardTotal; i++) {
        m_boardComboBox[BOARD_MAJOR_REAR]->addItem(icon, QString("%1").arg(i+1));
    }
    //! 辅配前视相机板
    //for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2; i++) {
    for (int i = 0; i < m_struInfo[BOARD_ASSIST_FRONT].nBoardTotal; i++) {
        m_boardComboBox[BOARD_ASSIST_FRONT]->addItem(icon, QString("%1").arg(i+1));
    }
    //! 辅配后视相机板
    //for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2; i++) {
    for (int i = 0; i < m_struInfo[BOARD_ASSIST_REAR].nBoardTotal; i++) {
        m_boardComboBox[BOARD_ASSIST_REAR]->addItem(icon, QString("%1").arg(i+1));
    }
}

/*!
 * \brief 刷新升级模式组合框的显示
 */
void UpgradeFPGAWidget::refreshModeComboBoxDisplay()
{
    QPixmap pixmap(1, 45);
    pixmap.fill(Qt::transparent);
    QIcon icon(pixmap);

    //! 增加"模式"选项
    for (int i = 0; i < BOARD_TYPE; i++) {
        m_modeComboBox[i]->clear();
        m_modeComboBox[i]->addItem(icon, myLan.mode);
    }
    //! 接口板
    for (int i = 0; i < 3; i++) {
        m_modeComboBox[BOARD_INT]->addItem(icon, QString("%1%2").arg(myLan.mode).arg(i+1));
    }

    //! 主配前视相机板
    for (int i = 0; i < 3; i++) {
        m_modeComboBox[BOARD_MAJOR_FRONT]->addItem(icon, QString("%1%2").arg(myLan.mode).arg(i+1));
    }
    //! 主配后视相机板
    for (int i = 0; i < 3; i++) {
        m_modeComboBox[BOARD_MAJOR_REAR]->addItem(icon, QString("%1%2").arg(myLan.mode).arg(i+1));
    }
    //! 辅配前视相机板
    for (int i = 0; i < 3; i++) {
        m_modeComboBox[BOARD_ASSIST_FRONT]->addItem(icon, QString("%1%2").arg(myLan.mode).arg(i+1));
    }
    //! 辅配后视相机板
    for (int i = 0; i < 3; i++) {
        m_modeComboBox[BOARD_ASSIST_REAR]->addItem(icon, QString("%1%2").arg(myLan.mode).arg(i+1));
    }
}

/*!
 * brief 刷新文件名的显示
 */
void UpgradeFPGAWidget::refreshFileNameDisplay()
{
    for(int i = 0; i < BOARD_TYPE; i++) {
        m_fileLineEdits[i]->setText(m_struInfo[i].sFileName);
    }
}

/*!
 * \brief 刷新升级板的初始化状态
 */
void UpgradeFPGAWidget::refreshInitDisplay()
{
    int nBoardTotal = m_struInfo[curBoardMode].nBoardTotal;
    int nIndex = m_struInfo[curBoardMode].nBoard;
    if (0 == nIndex) {
        for (int j = 0; j < nBoardTotal; j++) {
            m_struInfo[curBoardMode].nStatus[j] = UPGRADE_DOING;
            m_struInfo[curBoardMode].sUpgradeInfo[j] = myLan.msg_int_to_factory;
        }
    } else {
        m_struInfo[curBoardMode].nStatus[nIndex-1] = UPGRADE_DOING;
        m_struInfo[curBoardMode].sUpgradeInfo[nIndex-1] = myLan.msg_int_to_factory;
    }
    refreshCurrentDisplay(curBoardMode);
}

/*!
 * \brief 刷新显示当前升级板的升级信息
 */
void UpgradeFPGAWidget::refreshCurrentDisplay(int nType)
{
    QString str;
    int nUnitCount = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
    nUnitCount = m_struInfo[nType].nBoardTotal;
    switch(nType) {
    case BOARD_INT:
        for (int i = 0; i< struCnfg.nInterfaceBoardTotal;i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.interface_board)
                    .arg(i+1)
                    .arg(getTextByStatus(nType,m_struInfo[nType].nStatus[i],i))
                    .arg(m_struInfo[nType].sUpgradeInfo[i]);
        }
        break;
    case BOARD_MAJOR_FRONT:

        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_major+myLan.front_view)
                    .arg(i+1)
                    .arg(getTextByStatus(nType,m_struInfo[nType].nStatus[i],i))
                    .arg(m_struInfo[nType].sUpgradeInfo[i]);
        }
        break;
    case BOARD_MAJOR_REAR:
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_major+myLan.rear_view)
                    .arg(i+1)
                    .arg(getTextByStatus(nType,m_struInfo[nType].nStatus[i],i))
                    .arg(m_struInfo[nType].sUpgradeInfo[i]);
        }
        break;
    case BOARD_ASSIST_FRONT:
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_assist+myLan.front_view)
                    .arg(i+1)
                    .arg(getTextByStatus(nType,m_struInfo[nType].nStatus[i],i))
                    .arg(m_struInfo[nType].sUpgradeInfo[i]);
        }
        break;
    case BOARD_ASSIST_REAR:
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_assist+myLan.rear_view)
                    .arg(i+1)
                    .arg(getTextByStatus(nType,m_struInfo[nType].nStatus[i],i))
                    .arg(m_struInfo[nType].sUpgradeInfo[i]);
        }
        break;
    default:
        break;
    }
    m_resultLbl->setText(str);
    //! 动态设定显示控件的高度
    int nRow = 1;
    for (int i = 0; i < str.size(); i++) {
        if (str.at(i) == '\n') {
            nRow++;
        }
    }
    m_resultLbl->setFixedHeight(30*nRow);
}

/*!
 * \brief 刷新升级完成后总的结果显示
 */
void UpgradeFPGAWidget::refreshResultDisplay()
{
    QString str;
    int nUnitCount = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;

    if (m_struInfo[BOARD_INT].bEn) {
        for(int i = 0; i< struCnfg.nInterfaceBoardTotal;i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.interface_board)
                    .arg(i+1)
                    .arg(getTextByStatus(BOARD_INT,m_struInfo[BOARD_INT].nStatus[i],i))
                    .arg(m_struInfo[BOARD_INT].sUpgradeInfo[i]);
        }
    }

    if (m_struInfo[BOARD_MAJOR_FRONT].bEn) {
        nUnitCount = m_struInfo[BOARD_MAJOR_FRONT].nBoardTotal;
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_major+myLan.front_view)
                    .arg(i+1)
                    .arg(getTextByStatus(BOARD_MAJOR_FRONT,m_struInfo[BOARD_MAJOR_FRONT].nStatus[i],i))
                    .arg(m_struInfo[BOARD_MAJOR_FRONT].sUpgradeInfo[i]);
        }
    }

    if (m_struInfo[BOARD_MAJOR_REAR].bEn) {
        nUnitCount = m_struInfo[BOARD_MAJOR_REAR].nBoardTotal;
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_major+myLan.rear_view)
                    .arg(i+1)
                    .arg(getTextByStatus(BOARD_MAJOR_REAR,m_struInfo[BOARD_MAJOR_REAR].nStatus[i],i))
                    .arg(m_struInfo[BOARD_MAJOR_REAR].sUpgradeInfo[i]);
        }
    }
    if (m_struInfo[BOARD_ASSIST_FRONT].bEn) {
        nUnitCount = m_struInfo[BOARD_ASSIST_FRONT].nBoardTotal;
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_assist+myLan.front_view)
                    .arg(i+1)
                    .arg(getTextByStatus(BOARD_ASSIST_FRONT,m_struInfo[BOARD_ASSIST_FRONT].nStatus[i],i))
                    .arg(m_struInfo[BOARD_ASSIST_FRONT].sUpgradeInfo[i]);
        }
    }
    if (m_struInfo[BOARD_ASSIST_REAR].bEn) {
        nUnitCount = m_struInfo[BOARD_ASSIST_REAR].nBoardTotal;
        for (int i = 0; i < nUnitCount; i++) {
            str += QString("%1%2\t\t%3\t\t%4\n")
                    .arg(myLan.config_assist+myLan.rear_view)
                    .arg(i+1)
                    .arg(getTextByStatus(BOARD_ASSIST_REAR,m_struInfo[BOARD_ASSIST_REAR].nStatus[i],i))
                    .arg(m_struInfo[BOARD_ASSIST_REAR].sUpgradeInfo[i]);
        }
    }

    m_resultLbl->setText(str);

    //! 动态设定显示控件的高度
    int nRow = 1;
    for (int i = 0; i < str.size(); i++) {
        if (str.at(i) == '\n') {
            nRow++;
        }
    }
    m_resultLbl->setFixedHeight(30*nRow);
}

/*!
 * \brief 检查板级是否使能
 */
bool UpgradeFPGAWidget::checkUpgradeBoardEn()
{
    bool stat = false;
    for (int i = 0; i < BOARD_TYPE; i++) {
        if(m_struInfo[i].bEn) {
            stat = true;
        }
    }
    if (!stat) {
        infoWidget->setLabelText(myLan.msg_enable_upgrade_board);
        infoWidget->showSecs();
        return false;
    }

    for (int i = 0; i < BOARD_TYPE; i++) {
        if (m_struInfo[i].bEn && m_struInfo[i].nMode == 0) {
            infoWidget->setLabelText(myLan.msg_select_user_mode);
            infoWidget->showSecs(2);
            return false;
        }
    }

    for (int i = 0; i < BOARD_TYPE; i++) {
        if (m_struInfo[i].bEn && m_struInfo[i].nWriteLen == 0) {
            infoWidget->setLabelText(myLan.msg_select_right_file);
            infoWidget->showSecs(2);
            return false;
        }
    }

    myMessageBox msgBox(MSG_QUES, myLan.msg_insert_data_wire);
    if (msgBox.exec() == QDialog::Accepted) {
#ifdef Q_OS_UNIX
        if(myUsbDev != NULL){
            usb.myusbFree();
        }
        usb.myusbInit();
        if(myUsbDev == NULL){
            qDebug("usb init error");
            infoWidget->setLabelText(myLan.cfm_insert_datawire);
            infoWidget->showSecs();
            return false;
        }
#endif
    } else {
        return false;
    }
    return true;
}

/*!
 * \brief 清空对应下位机的升级状态
 * \param nType 类型
 */
void UpgradeFPGAWidget::clearUpgradeStatus(int nType)
{
    for (int i = 0; i < MAX_UNIT_PER_INT; i++) {
        m_struInfo[nType].nStatus[i] = UPGRADE_UNSTARTED;
    }
}

/*!
 * \brief 根据状态获取文本
 * \param nStat 状态
 * \return 文本
 */
QString UpgradeFPGAWidget::getTextByStatus(int nType,int nStat,int nBoard)
{
    int step_sum;
    switch(nType) {
    case BOARD_INT:
        step_sum = 4;
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        step_sum = 7;
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        step_sum = 10;
        break;
    default:
        break;
    }
    QString str;
    switch (nStat) {
    case UPGRADE_UNSTARTED:
        str = myLan.upgrade_untreated;
        break;
    case UPGRADE_DOING:
        str = myLan.upgrade_doing+QString("(%1/%2)").arg(step+1).arg(step_sum);
        break;
    case UPGRADE_ERR_FACTORY:
        str = myLan.upgrade_fail+QString("%1").arg(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_SEND:
        str = myLan.upgrade_fail+QString("%1").arg(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_BURN:
        str = myLan.upgrade_fail+QString("%1").arg(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_USER:
        str = myLan.upgrade_fail+QString("%1").arg(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_OK:
        str = myLan.upgrade_ok;
        break;
    default:
        break;
    }

    return str;
}

/*!
 * \brief 根据板类型及当前升级到哪一步step获取该步具体升级内容
 * \param nType 板类型     step当前步骤
 * \return 文本
 */
QString UpgradeFPGAWidget::getTextUpgradeInfo(int nType, int step)
{
    QString str;
    switch(nType) {
    case BOARD_INT:
        str = IntStepInfoList.at(step);
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        str = MajorStepInfoList.at(step);
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        str = SlaveStepInfoList.at(step);
        break;
    }
    return str;
}

/* 切换下位机至工厂模式 */
bool UpgradeFPGAWidget::stepSwitchToFactory(int nType)
{
    switch(nType) {
    case BOARD_INT:
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_FACTORY, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0, 0, 0, 0, 3);
        myFlow.setCom1Baud();
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvMajorUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                qDebug("UpgradeFPGAWidget nBoardTotal = %d",m_struInfo[1].nBoardTotal);
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_FACTORY, 2, m_struInfo[curBoardMode].qvMajorUnit.at(i), 0, 0, 0, 1, 0, 3);
        }
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvAssistUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) { //检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_FACTORY, 2, m_struInfo[curBoardMode].qvAssistUnit.at(i), 0, 0, 0, 2, 0, 3);
        }
        break;
    default:
        break;
    }

    myFlow.sleep(4);

    if(!checkStatus(nType,UPGRADE_FACTORY)) {
        return false;
    }
    return true;
}

/*!
 * \brief 升级步骤2: 发送升级文件
 * \return 发送是否成功
 */
bool UpgradeFPGAWidget::stepSendFileByUSB(int nType)
{
    int ret;
    char buf[128] = {0};
    int writeLenTotal = m_struInfo[curBoardMode].nWriteLen - 1;

    switch(nType) {
    case BOARD_INT:
    #ifdef Q_OS_UNIX
        if(myUsbDev != NULL){
            usb.myusbFree();
        }
        usb.myusbInit();
        if(myUsbDev == NULL){
            printf("usb init error");
            fflush(0);
            return false;
        }

        /* 考虑到USB初始化向下位机发送升级文件数据时，会漏发前64个字节的数据，
         * 因此USB初始化结束后向下位机写128个字节的废数据 */
        ret = usb_bulk_write(myUsbDev, USB_EP_OUT_AI, buf, 128, 1000);
        printf("MSG : write_ret1=%d", ret);
        fflush(0);

        MySerial.fpgaUpdateWrite(COM_INT_FILE_LEN, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0, 0, writeLenTotal/256, writeLenTotal%256, 3);
        myFlow.sleep(1);

        /* 切换接口板至USB模式 */
        MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0, 0, 0, 0, 1, 3);
        USBSendFile(COM_SEND_FILE, m_struInfo[curBoardMode].qvIntUnit.at(0), m_struInfo[curBoardMode].nWriteLen,curBoardMode);
        myFlow.sleep(1);
        break;
#endif
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvMajorUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_FILE_LEN, 2, m_struInfo[curBoardMode].qvMajorUnit.at(i) , 0, 0, 0, writeLenTotal/256, writeLenTotal%256, 3);
        }
        MySerial.fpgaUpdateWrite(COM_TRANSFER, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0xff, 0xff, 0xff ,0xff, 3);//转发命令
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvAssistUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_FILE_LEN, 2, m_struInfo[curBoardMode].qvAssistUnit.at(i) , 0, 0, 0, writeLenTotal/256, writeLenTotal%256, 3);
            MySerial.fpgaUpdateWrite(COM_TRANSFER, 2,m_struInfo[curBoardMode].qvMajorUnit.at(i), 0, 0xff, 0xff, 0xff ,0xff, 3);//转发命令
        }
        break;
    default:
        break;
    }

    myFlow.sleep(5);

    if(!checkStatus(nType,UPGRADE_SEND)) {
        return false;
    }
    return true;
}

/*!
 * \brief 升级步骤3: 烧写文件
 * \return 烧写是否成功
 */
bool UpgradeFPGAWidget::stepBurnFile(int nType)
{
    switch(nType) {
    case BOARD_INT:
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_BURN, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0, 0, 0, m_struInfo[nType].nMode, 3);
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvMajorUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, m_struInfo[curBoardMode].qvMajorUnit.at(i), 0, 0, 0, 1, m_struInfo[nType].nMode , 3);
        }
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvAssistUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_BURN, 2, m_struInfo[curBoardMode].qvAssistUnit.at(i), 0, 0, 0, 1, m_struInfo[nType].nMode , 3);
        }
        break;
    default:
        break;
    }

    myFlow.sleep(20);

    if(!checkStatus(nType,UPGRADE_BURN)) {
        return false;
    }
    return true;
}

/*!
 * \brief 升级步骤4: 切换至用户模式
 */
bool UpgradeFPGAWidget::stepSwtichToUser(int nType)
{
    switch(nType) {
    case BOARD_INT:
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_USER, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0, 0, 0,
                                 (m_struInfo[nType].nMode == 0)?struCnfp.nInterfaceBoardMode:m_struInfo[nType].nMode,3);
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvMajorUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, m_struInfo[curBoardMode].qvMajorUnit.at(i), 0, 0, 0, 1,
                                     (m_struInfo[nType].nMode == 0)?struCnfp.nColorBoardMode:m_struInfo[nType].nMode, 3);
        }
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvAssistUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {//检测当前板级升级状态，若失败直接跳过。
                    continue;
                }
            }
            MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, m_struInfo[curBoardMode].qvAssistUnit.at(i), 0, 0, 0, 2,
                                     (m_struInfo[nType].nMode == 0)?struCnfp.nInterfaceBoardMode:m_struInfo[nType].nMode, 3);
        }
        break;
    default:
        break;
    }

    myFlow.sleep(6);

    if(!checkStatus(nType,UPGRADE_USER)) {
        return false;
    }
    return true;
}

/* USB发送文件 */
void UpgradeFPGAWidget::USBSendFile(int cmd, unsigned char module, int num, int nType)
{
#ifdef Q_OS_UNIX
    char packet[PACKET_COUNT_BIG*FRAME_LEN+16];
    char sBuf[FRAME_LEN_MAIZE];
    unsigned int crcitt;
    int i, j;
    int ret = 0;

    /* pack & add checksum */
    bzero(packet, PACKET_COUNT_BIG*FRAME_LEN+16);

    packet[0]  = 0xF0;
    packet[1]  = 0xA5;
    packet[2]  = module;
    packet[3]  = 0;
    packet[4]   = cmd/256;
    packet[5]   = cmd%256;
    packet[6]   = struGsh.nSerialCount1++;
    packet[7]   = 0;
    packet[8]   = 0;
    packet[9]   = 0;
    packet[10]  = (num-1)/256;
    packet[11]  = (num-1)%256;

    for(int i=0;i<num*FRAME_LEN;i++) {
        packet[12+i] = m_struInfo[curBoardMode].fileBuf[i];
    }

    crcitt = MySerial.crcFpga(packet + 2, num*FRAME_LEN + 10);
    packet[num*FRAME_LEN+12] = crcitt/256;
    packet[num*FRAME_LEN+13] = crcitt%256;
    packet[num*FRAME_LEN+14] = 0xff;
    packet[num*FRAME_LEN+15] = 0xff;

#if 0
    printf("\n");
    printf("usb >> ");
    for(i=0; i<500; i++){
        if (i%20 == 0) {
            printf("\n");
        }
        printf("%02x ", packet[i]);
    }
    printf("\n");
#endif

    if (struGsh.nSerialCount1 > 250) {
        struGsh.nSerialCount1 = 0;
    }

    ret = usb_bulk_read(myUsbDev, USB_EP_IN_AI, sBuf, FRAME_LEN_MAIZE, 1000);
    qDebug("MSG : read_ret=%d", ret);
    ret = usb_bulk_write(myUsbDev, USB_EP_OUT_AI, packet, num*FRAME_LEN+16, 1000);
    qDebug("MSG : write_ret = %d", ret);
#endif
}

/*!
 * \brief 检查对应下位机的升级状态
 * \param nType 板类型
 * \param nUnit 板地址
 */
bool UpgradeFPGAWidget::checkBoardStatus(int nType, int nUnit)
{
    int nStat = m_struInfo[nType].nStatus[nUnit];
    if (nStat == UPGRADE_ERR_FACTORY
            || nStat == UPGRADE_ERR_SEND
            || nStat == UPGRADE_ERR_BURN
            || nStat == UPGRADE_ERR_USER) {
        return false;
    }
    return true;
}

/*!
 * \brief 检查下位机状态
 * \param nBoardType 板类型
 * \param nStatType 状态类型
 * \return 对应板类型的状态
 */
bool UpgradeFPGAWidget::checkUpdateStatus(int nBoardType, int nUnit, int nStatType)
{
    char *p = NULL;
    int nTimes = 0;

RECHECK:
    switch (nBoardType) {
    case BOARD_INT:
        MySerial.fpgaUpdateWrite(COM_INT_UPDATE_STATUS, nUnit, 0, 0, 0, 0, 0, 0, 1);
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, nUnit, 0, 0, 0, 1, 0, 1);
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_STATUS, 2, nUnit, 0, 0, 0, 2, 0, 1);
        break;
    default:
        break;
    }

    myFlow.msleep(3);
    MySerial.fpgaUpdateRead(packetBuf, PACKET_SIZE);

    switch (nStatType) {
    case UPGRADE_FACTORY:
        if (packetBuf[15] & packetBuf[15]&MASK_LOAD_FACTORY_MODE) {
            return true;
        }
        break;
    case UPGRADE_SEND:
        if (packetBuf[15] & MASK_BURN_SDRAM_ALL_FILE) {
            return true;
        }
        break;
    case UPGRADE_BURN:
        if (packetBuf[15] & MASK_BURN_FLASH_FILE) {
            return true;
        }
        break;
    case UPGRADE_USER:
        if (packetBuf[15] & MASK_LOAD_CUSTOM_MODE) {
            return true;
        }
        break;
    default:
        break;
    }

    if (nTimes++ < 10) {
        myFlow.sleep(1);
        goto RECHECK;
    }

    return false;
}

bool UpgradeFPGAWidget::checkStatus(int nBoardType, int nStatType)
{
    bool bStat = false;
    switch(nBoardType) {
    case BOARD_INT:
        if (!checkUpdateStatus(nBoardType, m_struInfo[curBoardMode].qvIntUnit.at(0), nStatType)) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                for(int i = 0; i < m_struInfo[curBoardMode].nBoardTotal; i++) {
                        m_struInfo[curBoardMode].nStatus[i] = nStatType+1;
                        m_struInfo[curBoardMode].nstep[i] = step;
//                        m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step-1);
                }
            } else { //单板升级
                m_struInfo[curBoardMode].nStatus[m_struInfo[curBoardMode].nBoard-1] = nStatType+1;
                m_struInfo[curBoardMode].nstep[m_struInfo[curBoardMode].nBoard-1] = step;
//                m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step-1);
            }
            /* 发送文件失败后复位接口板固件 */
            if(nStatType == UPGRADE_SEND) {
                MySerial.com1Write(CMD_INT_IMAGE_ERROR_REVISE, m_struInfo[curBoardMode].qvIntUnit.at(0), 0, 0, 0, 0, 0, 0, 1, 3);
                myFlow.sleep(1);
            }
        } else {
            bStat = true;
            if(nStatType == UPGRADE_USER) {
                for(int i = 0; i < m_struInfo[curBoardMode].nBoardTotal; i++) {
                    if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                        if(!checkBoardStatus(curBoardMode,i)) {
                            continue;
                        }
                        m_struInfo[curBoardMode].nStatus[i] = UPGRADE_OK;
                        m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step);
                    } else {
                        m_struInfo[curBoardMode].nStatus[m_struInfo[curBoardMode].nBoard-1] = UPGRADE_OK;
                        m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step);
                    }
                }
            } else {
                for(int i = 0; i < m_struInfo[curBoardMode].nBoardTotal; i++) {
                    if(0 == m_struInfo[curBoardMode].nBoard) {//全部升级
                        if(!checkBoardStatus(curBoardMode,i)) {
                            continue;
                        } else {
                            m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step);
                        }
                    } else {
                        m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step);
                    }
                }
            }
        }
        break;
    case BOARD_MAJOR_FRONT:
    case BOARD_MAJOR_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvMajorUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {
                    continue;
                }
                if (!checkUpdateStatus(nBoardType, m_struInfo[curBoardMode].qvMajorUnit.at(i), nStatType)) {
                    m_struInfo[curBoardMode].nStatus[i] = nStatType+1;
                    m_struInfo[curBoardMode].nstep[i] = step;
//                    m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step-1);
                } else {
                    bStat = true;
                    m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step);
                }
            } else {//单板升级
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {
                    continue;
                }
                if (!checkUpdateStatus(nBoardType, m_struInfo[curBoardMode].qvMajorUnit.at(i), nStatType)) {
                    m_struInfo[curBoardMode].nStatus[m_struInfo[curBoardMode].nBoard-1] = nStatType+1;
                    m_struInfo[curBoardMode].nstep[m_struInfo[curBoardMode].nBoard-1] = step;
//                    m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step-1);
                } else {
                    bStat = true;
                    m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step);
                }
            }
        }
        break;
    case BOARD_ASSIST_FRONT:
    case BOARD_ASSIST_REAR:
        for(int i = 0; i < m_struInfo[curBoardMode].qvAssistUnit.size();i++) {
            if(0 == m_struInfo[curBoardMode].nBoard) { //全部升级
                if(!checkBoardStatus(curBoardMode,i)) {
                    continue;
                }
                if (!checkUpdateStatus(nBoardType, m_struInfo[curBoardMode].qvAssistUnit.at(i), nStatType)) {
                    m_struInfo[curBoardMode].nStatus[i] = nStatType+1;
                    m_struInfo[curBoardMode].nstep[i] = step;
//                    m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step-1);
                } else {
                    m_struInfo[curBoardMode].sUpgradeInfo[i] = getTextUpgradeInfo(curBoardMode,step);
                    bStat = true;
                }
            } else {
                if(!checkBoardStatus(curBoardMode,m_struInfo[curBoardMode].nBoard-1)) {
                    continue;
                }
                if (!checkUpdateStatus(nBoardType, m_struInfo[curBoardMode].qvAssistUnit.at(i), nStatType)) {
                    m_struInfo[curBoardMode].nStatus[m_struInfo[curBoardMode].nBoard-1] = nStatType+1;
                    m_struInfo[curBoardMode].nstep[m_struInfo[curBoardMode].nBoard-1] = step;
//                    m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step-1);
                } else {
                    m_struInfo[curBoardMode].sUpgradeInfo[m_struInfo[curBoardMode].nBoard-1] = getTextUpgradeInfo(curBoardMode,step);
                    bStat = true;
                }
            }
        }
        break;
    default:
        break;
    }
    refreshCurrentDisplay(curBoardMode);
    return bStat;
}

/*!
 * \brief 接口板升级
 */
bool UpgradeFPGAWidget::upgradeIntBoard()
{
    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1.切换接口板至工厂模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_INT)) {
        return false;
    }

    myFlow.sleep(4); //切工厂模式后接口板USB固件复位等待时间

    //! 2.下发升级文件
    step = 2;
    if(!stepSendFileByUSB(BOARD_INT)) {
        return false;
    }

    //! 3.烧写升级文件
    step = 3;
    if(!stepBurnFile(BOARD_INT)) {
        return false;
    }

    //! 4.切换接口板至用户模式
    step = 4;
    if(!stepSwtichToUser(BOARD_INT)) {
        return false;
    }
    return true;
}

/*!
 * \brief 彩色相机板升级
 */
bool UpgradeFPGAWidget::upgradeMajorBoard(int nType)
{
    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1.切换接口板至厂家模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_INT)) {
        return false;
    }

    //! 2.下发升级文件到接口板
    step = 2;
    myFlow.sleep(4);//切工厂模式后接口板USB固件复位等待时间
    if(!stepSendFileByUSB(BOARD_INT)) {
        return false;
    }

    //! 3.切换相机板至厂家模式
    step = 3;
    if(!stepSwitchToFactory(nType)) {
        return false;
    }

    //! 4.下载文件到主相机板
    step = 4;
    if(!stepSendFileByUSB(nType)){
        return false;
    }

    //! 5.烧写文件到flash
    step = 5;
    if(!stepBurnFile(nType)) {
        return false;
    }

    //! 6.相机板切回用户模式
    step = 6;
    if(!stepSwtichToUser(nType)) {
        return false;
    }

    //! 7.接口板切回用户模式
    step = 7;
    if(!stepSwtichToUser(BOARD_INT)) {
        return false;
    }
    return true;
}

/*!
 * \brief 红外相机板升级
 */
void UpgradeFPGAWidget::upgradeAssistBoard(int nType)
{
    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1.切换接口板至厂家模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_INT)) {
        return;
    }

    //! 2.下发升级文件到接口板
    step = 2;
    myFlow.sleep(4);//切工厂模式后接口板USB固件复位等待时间
    if(!stepSendFileByUSB(BOARD_INT)) {
        return;
    }

    //! 3.切换主相机板至厂家模式
    step = 3;
    if(!stepSwitchToFactory(nType-2)) {
        return;
    }

    //! 4.切换从相机板至厂家模式
    step = 4;
    if(!stepSwitchToFactory(nType)) {
        return;
    }

    //! 5.下载文件到主相机板
    step = 5;
    if(!stepSendFileByUSB(nType-2)) {
        return;
    }

    //! 6.下载文件到从相机板
    step = 6;
    if(!stepSendFileByUSB(nType)) {
        return;
    }

    //! 7.烧写文件到从板flash
    step  = 7;
    if(!stepBurnFile(nType)) {
        return;
    }

    //! 8.从相机板切回用户模式
    step = 8;
    if(!stepSwtichToUser(nType)) {
        return;
    }

    //! 9.主相机板切回用户模式
    step = 9;
    if(!stepSwtichToUser(nType-2)) {
        return;
    }

    //! 10.接口板切回用户模式
    step = 10;
    if(!stepSwtichToUser(BOARD_INT)) {
        return;
    }
}

/*!
 * \brief 槽函数: 使能按钮
 * \param nIndex 按钮索引号
 */
void UpgradeFPGAWidget::onSelBtnsPressed(int nIndex)
{
    m_struInfo[nIndex].bEn = !m_struInfo[nIndex].bEn;

    if (m_struInfo[nIndex].bEn) {
        m_selBtns[nIndex]->setIcon(myIcon.Action_Apply);
    } else {
        m_selBtns[nIndex]->setIcon(QIcon());
    }
//    refreshResultDisplay();
//    refreshCurrentDisplay(nIndex);
}

/*!
 * \brief 槽函数: 升级板选择组合框
 * \param nIndex 组合框索引号
 */
void UpgradeFPGAWidget::onBoardComboBoxesPressed(int nIndex)
{
    m_struInfo[nIndex].nBoard = m_boardComboBox[nIndex]->currentIndex();
    //m_struInfo[nIndex].nBoardTotal = 1;
    m_struInfo[nIndex].qvIntUnit.clear();
    m_struInfo[nIndex].qvMajorUnit.clear();
    m_struInfo[nIndex].qvAssistUnit.clear();
    int nBoardCount = m_struInfo[nIndex].nBoardTotal;
    switch(nIndex) {
    case BOARD_INT:
        if(m_struInfo[nIndex].nBoard == 0) {
            m_struInfo[nIndex].nBoardTotal = struCnfg.nInterfaceBoardTotal;
            for(int i = 0; i < struCnfg.nInterfaceBoardTotal; i++) {
                m_struInfo[nIndex].qvIntUnit << 1 + (i<<4);
            }
        } else {
            m_struInfo[nIndex].qvIntUnit << m_struInfo[nIndex].nBoard;
        }
        break;
    case BOARD_MAJOR_FRONT:
        m_struInfo[nIndex].qvIntUnit << 0x01;
        if(m_struInfo[nIndex].nBoard == 0) {
            //m_struInfo[nIndex].nBoardTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
            //for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;i++) {
            for(int i = 0; i < nBoardCount;i++) {
                m_struInfo[nIndex].qvMajorUnit << 2*i;
            }
        } else {
            m_struInfo[nIndex].qvMajorUnit << 2*(m_struInfo[nIndex].nBoard-1);
        }
        break;
    case BOARD_MAJOR_REAR:
        m_struInfo[nIndex].qvIntUnit << 0x01;
        if(m_struInfo[nIndex].nBoard == 0) {
            //m_struInfo[nIndex].nBoardTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
            //for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;i++) {
            for(int i = 0; i < nBoardCount;i++) {
                m_struInfo[nIndex].qvMajorUnit << 2*i+1;
            }
        } else {
            m_struInfo[nIndex].qvMajorUnit << 2*m_struInfo[nIndex].nBoard-1;
        }
        break;
    case BOARD_ASSIST_FRONT:
        m_struInfo[nIndex].qvIntUnit << 0x01;
        if(m_struInfo[nIndex].nBoard == 0) {
            //m_struInfo[nIndex].nBoardTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
            //for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;i++) {
            for(int i = 0; i < nBoardCount;i++) {
                m_struInfo[nIndex].qvMajorUnit << 2*i;
            }
        } else {
            m_struInfo[nIndex].qvMajorUnit << 2*(m_struInfo[nIndex].nBoard-1);
        }
        break;
    case BOARD_ASSIST_REAR:
        m_struInfo[nIndex].qvIntUnit << 0x01;
        if(m_struInfo[nIndex].nBoard == 0) {
            //m_struInfo[nIndex].nBoardTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
            //for(int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;i++) {
            for(int i = 0; i < nBoardCount;i++) {
                m_struInfo[nIndex].qvMajorUnit << 2*i+1;
            }
        } else {
            m_struInfo[nIndex].qvMajorUnit << 2*m_struInfo[nIndex].nBoard-1;
        }
        break;
    default:
        break;
    }
}

/*!
 * \brief 槽函数: 升级模式组合框
 * \param nIndex 组合框索引号
 */
void UpgradeFPGAWidget::onModeComboBoxesPressed(int nIndex)
{
    switch(m_modeComboBox[nIndex]->currentIndex()) {
    case 0:
        m_struInfo[nIndex].nMode = 0;
        break;
    case 1:
        m_struInfo[nIndex].nMode = 1;
        break;
    case 2:
        m_struInfo[nIndex].nMode = 2;
        break;
    case 3:
        m_struInfo[nIndex].nMode = 4;
        break;
    default:
        break;
    }
}

/*!
 * \brief 槽函数: 文件选择
 * \param nIndex 编辑框索引号
 */
void UpgradeFPGAWidget::onFileLineEditsPressed(int nIndex)
{
    //! 检查U盘
#ifdef Q_OS_UNIX
    if (!myFlow.mountUdisk()) {
        infoWidget->setLabelText(myLan.msg_insert_udisk);
        infoWidget->showSecs();
        return;
    }
#endif

    QStringList strList;
    strList << "*.rpd" << "*.bin";
    SelectFileDialog *dialog = new SelectFileDialog(strList);
    if (dialog->exec() == QDialog::Accepted) {
        QString sFileName = dialog->getFileName();
        m_fileLineEdits[nIndex]->setText(sFileName);

        //! 若升级文件更换则清空升级状态
        if (m_struInfo[nIndex].sFileName != sFileName) {
            clearUpgradeStatus(nIndex);
            m_struInfo[nIndex].sFileName = sFileName;
        }

        //! 根据升级文件大小确定包长
        QFile file(g_sUpgradeDir + m_struInfo[nIndex].sFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            infoWidget->setLabelText(myLan.msg_insert_udisk);
            infoWidget->showSecs();
            return;
        }

        //! 读取升级文件
        m_struInfo[nIndex].fileBuf = file.readAll();
        printf("%d\n",m_struInfo[nIndex].fileBuf.size());
        fflush(0);

        if (file.size() >= FILE_SIZE_4M) {
             m_struInfo[nIndex].nWriteLen = PACKET_COUNT_BIG;
        } else if (file.size() == FILE_SIZE_2M) {
            m_struInfo[nIndex].nWriteLen = PACKET_COUNT;
        } else if (file.size() == FILE_SIZE_512K) {
            m_struInfo[nIndex].nWriteLen = PACKET_COUNT_SMALL;
        } else {
            m_struInfo[nIndex].nWriteLen = 0;
        }

        file.close();
    }
}

/*!
 * \brief 槽函数: 升级
 */
void UpgradeFPGAWidget::onUpgradeBtnPressed()
{
    if(!checkUpgradeBoardEn()) {
        return;
    }
    qDebug("UpgradeFPGAWidget ld4 nUnitLevelTotal = %d",struCnfg.struLevelInfo[0].nUnitLevelTotal);

    qDebug("UpgradeFPGAWidget nBoardTotal = %d",m_struInfo[1].nBoardTotal);

    m_upgradeBtn->setEnabled(false);
    m_backBtn->setEnabled(false);

    infoWidget->hideCentralWidget();
    infoWidget->delayShow();

    //! 根据使能升级对应下位机
    if (m_struInfo[BOARD_INT].bEn) {
        curBoardMode = BOARD_INT;
        if(!upgradeIntBoard())
            goto EXIT;
    }

    myFlow.sleep(1);
    if (m_struInfo[BOARD_MAJOR_FRONT].bEn) {
        curBoardMode = BOARD_MAJOR_FRONT;
        upgradeMajorBoard(BOARD_MAJOR_FRONT);
    }

    myFlow.sleep(1);
    if (m_struInfo[BOARD_MAJOR_REAR].bEn) {
        curBoardMode = BOARD_MAJOR_REAR;
        upgradeMajorBoard(BOARD_MAJOR_REAR);
    }

    myFlow.sleep(1);
    if (m_struInfo[BOARD_ASSIST_FRONT].bEn) {
        curBoardMode = BOARD_ASSIST_FRONT;
        upgradeAssistBoard(BOARD_ASSIST_FRONT);
    }

    myFlow.sleep(1);
    if (m_struInfo[BOARD_ASSIST_REAR].bEn) {
        curBoardMode = BOARD_ASSIST_REAR;
        upgradeAssistBoard(BOARD_ASSIST_REAR);
    }

EXIT:
    refreshResultDisplay();
    infoWidget->delayHide();
    m_upgradeBtn->setEnabled(true);
    m_backBtn->setEnabled(true);
}

/*!
 * \brief 槽函数: 返回
 */
void UpgradeFPGAWidget::onBackBtnPressed()
{
     emit sBackToFactorySetPage();
}
