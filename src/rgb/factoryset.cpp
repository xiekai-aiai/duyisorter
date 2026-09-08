/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        factoryset.cpp
 * \brief       彩色机型厂家设置界面源文件
 * \date        2015.01.14
 */
#include "factoryset.h"
#include <QProcess>

factorySet::factorySet(QWidget *parent)
    : QMainWindow(parent)
{
    setGeometry(0, 0, LCD_WIDTH-20, LCD_HEIGHT-60);
    setFixedSize(LCD_WIDTH-20, LCD_HEIGHT-60);

    config = g_Config::getInstance();
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(0, 0, width(), height());

    // 工厂设置界面
    createFactorySetPage();

    // 彩色相机设置界面
    setColorCamera *setColorCameraWidget = new setColorCamera;
    stackedWidget->addWidget(setColorCameraWidget);

    // 设置语言界面
    createSetLanguagePage();

    // 设置通用机型界面
    setMachineMode *setMachineModeWidget = NULL;
    setMachineModeWidget = new setMachineMode;
    stackedWidget->addWidget(setMachineModeWidget);
    connect(setMachineModeWidget,SIGNAL(resetCamera(int)),setColorCameraWidget,SLOT(onDivChannelListResetBtnClicked(int)));

    // 设置老化测试界面
    createAgeingTestPage();

    // 设置产量测试界面
    createOutputTestPage();

    // FPGA升级
    upgradeFpgaWidget = new UpgradeProgramWidget;
    stackedWidget->addWidget(upgradeFpgaWidget);

    // 设置下位机启动模式界面
    createSetFPGAModePage();

    // 创建背光设置界面
    createBacklightPasswordPage();

    // 创建振动器高低电压设置界面
    createSetFeederVoltage();

    // 创建灯控设置页面
    pageLampCtrl = new LampCtrl;
    stackedWidget->addWidget(pageLampCtrl);
    // 消磁时间、截断时间界面
    m_degaussTimeWidget = new DegaussTime;
    stackedWidget->addWidget(m_degaussTimeWidget);

    //! 振动器外接控制
    m_feederCtrlWidget = new FeederRemoteCtrl;
    stackedWidget->addWidget(m_feederCtrlWidget);

    //! 信号校正参数列表
    m_autoSigListWidget = new autoSigListWidget;
    stackedWidget->addWidget(m_autoSigListWidget);

    //! 通过量测试页面
    m_througPutWidget = new Throughput;
    stackedWidget->addWidget(m_througPutWidget);

    //! 精准剔除页面
    m_tickModeWidget = new tickModeWidget;
    stackedWidget->addWidget(m_tickModeWidget);

    setAiParams();

    connect(setColorCameraWidget, SIGNAL(backToMainWindow()), this, SLOT(backToFactorySet()));
    connect(m_degaussTimeWidget, SIGNAL(backToVenderset()), this, SLOT(onDegaussTimeBackBtnClicked()));

    connect(setMachineModeWidget, SIGNAL(backToFactory()), this, SLOT(backToFactorySet()));
    connect(setMachineModeWidget, SIGNAL(backToFactory()), this, SLOT(factoryUpdateSlt()));
    connect(this, SIGNAL(refreshMachineModeDisplaySig()), setMachineModeWidget, SLOT(refreshMachineModeDisplaySlt()));

    connect(upgradeFpgaWidget, SIGNAL(sBackToFactorySetPage()), this, SLOT(backToFactorySet()));
    connect(this, SIGNAL(sonIntGoToFirstSig()), setColorCameraWidget, SLOT(gotoFirstPageSlt()));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetIndexChangedSlt(int)));
    connect(pageLampCtrl,SIGNAL(backToHomePageSig()),this,SLOT(backToFactorySet()));
    connect(m_feederCtrlWidget, SIGNAL(backToHomePageSig()), this, SLOT(backToFactorySet()));
    connect(m_autoSigListWidget, SIGNAL(sBackToFactorySetPage()), this, SLOT(backToFactorySet()));
    connect(m_througPutWidget,SIGNAL(backToHomePageSig()),this,SLOT(backToFactorySet()));
    connect(m_tickModeWidget,SIGNAL(backToHomePageSig()),this,SLOT(backToFactorySet()));
}

factorySet::~factorySet()
{
}

void factorySet::setAiParams(){
    setAiWidget = new QWidget;
    setAiWidget->setGeometry(0, 0, stackedWidget->width(), stackedWidget->height());
    stackedWidget->addWidget(setAiWidget);

    imgFetchHeight = struCnfg.imgFetchHeight;
    imgInferHeight = struCnfg.imgInferHeight;
    imgPicHeight = struCnfg.imgPicHeight;
    imgVideoHeight = struCnfg.imgVideoHeight;
    aiEnbaleChx = new myCustomCheckBox(myLan.enable+"AI", false);
    setAiSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    setAiSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setAiBackbtn = new myPushButton(myLan.back, myIcon.Action_Back);
    setAiBackbtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    imgFetchHeightLbl = new myLabel(myLan.acquisition_height, setAiWidget);
    imgFetchHeightEdit = new myLineEdit(QString("%1").arg(imgFetchHeight), setAiWidget);

    imgFetchHeightEdit->setReadOnly(true);
    imgFetchHeightEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    imgInferHeightLbl = new myLabel(myLan.reasoning_height, setAiWidget);
    imgInferHeightEdit = new myLineEdit(QString("%1").arg(imgInferHeight), setAiWidget);

    imgInferHeightEdit->setReadOnly(true);
    imgInferHeightEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    imgPicHeightLbl = new myLabel("图片高度", setAiWidget);
    imgPicHeightEdit = new myLineEdit(QString("%1").arg(imgPicHeight), setAiWidget);

    imgPicHeightEdit->setReadOnly(true);
    imgPicHeightEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    imgVideoHeightLbl = new myLabel("视频高度", setAiWidget);
    imgVideoHeightEdit = new myLineEdit(QString("%1").arg(imgVideoHeight), setAiWidget);

    imgVideoHeightEdit->setReadOnly(true);
    imgVideoHeightEdit->setFixedSize(BTN_WIDTH+20,BTN_HEIGHT);

    QVBoxLayout *upLay = new QVBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(setAiWidget);

    QHBoxLayout * imgFetchHeightHLayout = new QHBoxLayout;
    imgFetchHeightHLayout->addWidget(imgFetchHeightLbl);
    imgFetchHeightHLayout->addWidget(imgFetchHeightEdit);
    imgFetchHeightHLayout->addSpacing(300);

    QHBoxLayout * imgInferHeightHLayout = new QHBoxLayout;
    imgInferHeightHLayout->addWidget(imgInferHeightLbl);
    imgInferHeightHLayout->addWidget(imgInferHeightEdit);
    imgInferHeightHLayout->addSpacing(300);

    QHBoxLayout * imgPicHeightHLayout = new QHBoxLayout;
    imgPicHeightHLayout->addWidget(imgPicHeightLbl);
    imgPicHeightHLayout->addWidget(imgPicHeightEdit);
    imgPicHeightHLayout->addSpacing(300);

    QHBoxLayout * imgVideoHeightHLayout = new QHBoxLayout;
    imgVideoHeightHLayout->addWidget(imgVideoHeightLbl);
    imgVideoHeightHLayout->addWidget(imgVideoHeightEdit);
    imgVideoHeightHLayout->addSpacing(300);
    upLay->setContentsMargins(80,10,20,10);
    upLay->addWidget(aiEnbaleChx,Qt::AlignCenter);
    upLay->addLayout(imgFetchHeightHLayout);
    upLay->addLayout(imgInferHeightHLayout);
    upLay->addLayout(imgPicHeightHLayout);
    upLay->addLayout(imgVideoHeightHLayout);


    downLay->addWidget(setAiSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(setAiBackbtn, Qt::AlignRight);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(aiEnbaleChx, SIGNAL(pressed()), this, SLOT(onAiEnbaleChxBtnClicked()));
    connect(imgFetchHeightEdit, SIGNAL(pressed()), this, SLOT(onImgFetchHeightEditPressedSlt()));
    connect(imgInferHeightEdit, SIGNAL(pressed()), this, SLOT(onImgInferHeightEditPressedSlt()));
    connect(imgPicHeightEdit, SIGNAL(pressed()), this, SLOT(onImgPicHeightEditPressedSlt()));
    connect(imgVideoHeightEdit, SIGNAL(pressed()), this, SLOT(onImgVideoHeightEditPressedSlt()));
    connect(setAiSureBtn, SIGNAL(pressed()), this, SLOT(onSetAiSureBtnClicked()));
    connect(setAiBackbtn, SIGNAL(pressed()), this, SLOT(onSetAiBackBtnClicked()));
}

void factorySet::onSetAiSureBtnClicked()
{
    struCnfg.imgFetchHeight = imgFetchHeight;
    struCnfg.imgInferHeight = imgInferHeight;
    struCnfg.imgPicHeight = imgPicHeight;
    struCnfg.imgVideoHeight = imgVideoHeight;
    struCnfg.aiEnable = aiEnable;
    myFlow.initUdpImagPara();
}

void factorySet::onSetAiBackBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

void factorySet::onAiEnbaleChxBtnClicked(){
    aiEnable = !aiEnable;
    aiEnbaleChx->setChecked(aiEnable);
    struCnfg.aiEnable = aiEnable;
}

void factorySet::onImgFetchHeightEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, imgFetchHeight);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        imgFetchHeight = inputDlg.getValue();
        QString str = QString("%1").arg(imgFetchHeight);
        imgFetchHeightEdit->setText(str);
    }
}

void factorySet::onImgInferHeightEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, imgInferHeight);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        imgInferHeight = inputDlg.getValue();
        QString str = QString("%1").arg(imgInferHeight);
        imgInferHeightEdit->setText(str);
    }
}

void factorySet::onImgPicHeightEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, imgPicHeight);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        imgPicHeight = inputDlg.getValue();
        QString str = QString("%1").arg(imgPicHeight);
        imgPicHeightEdit->setText(str);
    }
}

void factorySet::onImgVideoHeightEditPressedSlt(){
    myInputPanel inputDlg(intType, 0, 65535, imgVideoHeight);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        imgVideoHeight = inputDlg.getValue();
        QString str = QString("%1").arg(imgVideoHeight);
        imgVideoHeightEdit->setText(str);
    }
}

/* 生成工厂设置界面 */
void factorySet::createFactorySetPage()
{
    factorySetWidget = new QWidget(stackedWidget);
    factorySetWidget->setGeometry(0,0,stackedWidget->width(),stackedWidget->height());
    factorySetWidget->setFixedSize(LCD_WIDTH-20,  LCD_HEIGHT-80);


    upLay = new QGridLayout;
    downLay = new QHBoxLayout;
    mainLay = new QVBoxLayout(factorySetWidget);

    setColorCameraBtn = new myPushButton(QString(myLan.camera_new), QIcon(), true, true);
//    setColorCameraBtn = new myPushButton("相机参数", QIcon(), true, true);

    setLanguageBtn = new myPushButton(myLan.language, QIcon(), true, true);
//    setLanguageBtn = new myPushButton("语言设定", QIcon(), true, true);

    setAiBtn = new myPushButton("Ai "+myLan.set, QIcon(), true, true);

    clearRunningTimeBtn = new myPushButton(myLan.clear_running_time, QIcon(), true, true);
    ageingTestBtn = new myPushButton(QString(myLan.old_test), QIcon(), true, true);
    updateARMBtn = new myPushButton(myLan.upgrade_screen, QIcon(), true, true);
    setFPGAStartModeBtn = new myPushButton(myLan.init_mode, QIcon(), true, true);

    setMachineModeBtn = new myPushButton(QString(myLan.set_machine), QIcon(), true, true);

//    setMachineModeBtn = new myPushButton("机型设定", QIcon(), true, true);

    calibrationTSBtn = new myPushButton(myLan.ts_calibrate, QIcon(), true, true);
//    calibrationTSBtn = new myPushButton("触摸屏校正", QIcon(), true, true);


    outPutTestBtn = new myPushButton(myLan.throughput_test, QIcon(), true, true);
    updateFPGABtn = new myPushButton(myLan.upgrade_fpga, QIcon(), true, true);
    setBacklightPasswordBtn = new myPushButton(myLan.set_screensaver, QIcon(), true, true);

    setFeederVoltageBtn = new myPushButton(QString(myLan.set_vib_voltage), QIcon(), true, true);

//    setFeederVoltageBtn = new myPushButton("振动器电压", QIcon(), true, true);

    factorySetBackBtn = new myPushButton(myLan.back, QIcon(), true, true);
    lampCtrlBtn = new myPushButton(QString(myLan.setLight),QIcon(), true, true);
    degaussTimeBtn = new myPushButton(QString(myLan.tdemag), QIcon(), true, true);
    m_feederCtrlBtn = new myPushButton(myLan.feeder_remote_control, QIcon(), true, true);
    m_througPutBtn = new myPushButton(QString(myLan.test_output), QIcon(), true, true);
    m_tickModeBtn = new myPushButton(myLan.tick_mode,QIcon(),true,true);

    m_setNetWorkBtn = new myPushButton(myLan.network,QIcon(),true,true);
    m_sysHelpBtn = new myPushButton(myLan.help,QIcon(),true,true);
    m_sysInfoBtn = new myPushButton(myLan.device,QIcon(),true,true);

    setAiBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    //设备加密
    m_encryptBtn = new myPushButton(myLan.encrypt,QIcon(), true,true);
    setColorCameraBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setLanguageBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    clearRunningTimeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    ageingTestBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    updateARMBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setFPGAStartModeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setMachineModeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    calibrationTSBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    outPutTestBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    updateFPGABtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setBacklightPasswordBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setFeederVoltageBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    factorySetBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    lampCtrlBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    degaussTimeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_feederCtrlBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_througPutBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_tickModeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    m_encryptBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_setNetWorkBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_sysHelpBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_sysInfoBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->setContentsMargins(10, 80, 10, 50);
    upLay->setHorizontalSpacing(50);

    upLay->setVerticalSpacing(50);

    factoryUpdateSlt();

    downLay->addStretch();
    downLay->addWidget(factorySetBackBtn, Qt::AlignRight);
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);
    stackedWidget->addWidget(factorySetWidget);

    /* 设置不同按键的响应函数 */
    connect(factorySetBackBtn, SIGNAL(pressed()), this, SLOT(onFactoryBackBtnClicked()));
    connect(setColorCameraBtn, SIGNAL(pressed()), this, SLOT(onSetColorCameraBtnClicked()));
    connect(setLanguageBtn, SIGNAL(pressed()), this, SLOT(onSetLanguageBtnClicked()));
    connect(setMachineModeBtn, SIGNAL(pressed()), this, SLOT(onSetMachineModeBtnClicked()));
    connect(clearRunningTimeBtn, SIGNAL(pressed()), this, SLOT(onClearRunningTimeBtnClicked()));
    connect(calibrationTSBtn, SIGNAL(pressed()), this, SLOT(onCalibrationBtnClicked()));
    connect(ageingTestBtn, SIGNAL(pressed()), this, SLOT(onAgeingTestBtnClicked()));
    connect(outPutTestBtn, SIGNAL(pressed()), this, SLOT(onOutputTestBtnClicked()));
    connect(updateARMBtn, SIGNAL(pressed()), this, SLOT(onUpdateARMBtnClicked()));
    connect(updateFPGABtn, SIGNAL(pressed()), this, SLOT(onUpdateFPGABtnClicked()));
    connect(setFPGAStartModeBtn, SIGNAL(pressed()), this, SLOT(onSetFPGAStartModeBtnClicked()));
    connect(setFeederVoltageBtn, SIGNAL(pressed()), this, SLOT(onSetFeederVolatageBtnClicked()));
    connect(setBacklightPasswordBtn, SIGNAL(pressed()), this, SLOT(onSetBacklightPasswordBtnClicked()));
    connect(lampCtrlBtn, SIGNAL(pressed()), this, SLOT(onLampCtrlBtnClicked()));
    connect(degaussTimeBtn, SIGNAL(pressed()), this, SLOT(onDegaussTimeBtnClicked()));
    connect(m_feederCtrlBtn, SIGNAL(pressed()), this, SLOT(onFeederCtrlBtnPressed()));
    connect(m_througPutBtn,SIGNAL(pressed()),this,SLOT(onThroughPutBtnPressedSlt()));      //吞吐量按钮
    connect(m_tickModeBtn,SIGNAL(pressed()),this,SLOT(onTickModeBtnPressedSlt()));
    connect(setAiBtn,SIGNAL(pressed()),this,SLOT(onAiSetBtnPressedSlt()));

    connect(m_encryptBtn,SIGNAL(pressed()),this,SLOT(onEncryptBtnBtnClickedSlt()));

}

void factorySet::factoryUpdateSlt()
{
    switch(struCnfe.nMachine){
    case MACHINE_CF:
        {
//                qDebug()<<"AuthenticationLevel"<<struGsh.nAuthenticationLevel<<endl;
                upLay->addWidget(setColorCameraBtn, 0, 0);
                upLay->addWidget(setMachineModeBtn, 0, 1);
                upLay->addWidget(setLanguageBtn, 0, 2);
                upLay->addWidget(calibrationTSBtn, 0, 3);
                upLay->addWidget(degaussTimeBtn, 1, 0);
                upLay->addWidget(setFPGAStartModeBtn, 1, 1);
                upLay->addWidget(m_througPutBtn, 1, 2);
                upLay->addWidget(ageingTestBtn, 1, 3);
                upLay->addWidget(setBacklightPasswordBtn, 2, 1);
                upLay->addWidget(setFeederVoltageBtn, 2, 2);
//                  upLay->addWidget(lampCtrlBtn, 2, 3);
                upLay->addWidget(setAiBtn, 2, 3);

                upLay->addWidget(updateARMBtn, 3, 0);
                upLay->addWidget(updateFPGABtn, 3, 1);
                upLay->addWidget(m_encryptBtn, 3, 2);
                upLay->addWidget(m_sysHelpBtn, 3, 3);

                upLay->addWidget(m_setNetWorkBtn, 4, 0);
                upLay->addWidget(m_sysInfoBtn, 4, 1);

                if(struGsh.nAuthenticationLevel == 1){
                   setColorCameraBtn->show();
                   setMachineModeBtn->show();
                   setLanguageBtn->show();
                   m_througPutBtn->show();
                   ageingTestBtn->show();
                   calibrationTSBtn->show();
                   setBacklightPasswordBtn->show();
                   setFeederVoltageBtn->show();
                   degaussTimeBtn->show();
                   lampCtrlBtn->show();
                   setFPGAStartModeBtn->show();
                   setAiBtn->show();

                   updateARMBtn->hide();
                   updateFPGABtn->hide();
                   m_setNetWorkBtn->hide();
                   m_sysHelpBtn->hide();
                   m_sysInfoBtn->hide();
                   m_encryptBtn->hide();
                }
                if(struGsh.nAuthenticationLevel == 2){

                    setColorCameraBtn->hide();
                    setMachineModeBtn->hide();
                    setLanguageBtn->hide();
                    m_througPutBtn->hide();
                    ageingTestBtn->hide();
                    calibrationTSBtn->hide();
                    setBacklightPasswordBtn->hide();
                    setFeederVoltageBtn->hide();
                    degaussTimeBtn->hide();
                    lampCtrlBtn->hide();
                    setFPGAStartModeBtn->hide();
                    setAiBtn->hide();

                    updateARMBtn->show();
                    updateFPGABtn->show();
                    m_setNetWorkBtn->show();
                    m_sysHelpBtn->show();
                    m_sysInfoBtn->show();
                    m_encryptBtn->show();
                }

                lampCtrlBtn->hide();
                m_setNetWorkBtn->hide();
                m_sysInfoBtn->hide();
//                m_encryptBtn->hide();

//                upLay->addWidget(setMachineModeBtn, 0, 0);
//                upLay->addWidget(setFPGAStartModeBtn, 0, 1);
//                upLay->addWidget(setColorCameraBtn, 1, 0);
//                upLay->addWidget(setLanguageBtn, 1, 1);
//                upLay->addWidget(setFeederVoltageBtn, 2, 0);
//                upLay->addWidget(degaussTimeBtn, 2, 1);
//                upLay->addWidget(m_througPutBtn, 3, 0);
//                upLay->addWidget(ageingTestBtn, 3, 1);
//                upLay->addWidget(updateARMBtn, 4, 0);
//                upLay->addWidget(updateFPGABtn, 4, 1);
//                upLay->addWidget(calibrationTSBtn, 5, 0);
//                upLay->addWidget(setBacklightPasswordBtn, 5, 1);
//                upLay->addWidget(m_encryptBtn, 6, 0);

        }
        break;
    default:
        break;
    }
}

/* 生成语言设置界面 */
void factorySet::createSetLanguagePage()
{
    langWidget = new QWidget;
    langWidget->setGeometry(0,0,stackedWidget->width(),stackedWidget->height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(langWidget);

    langList = new QListWidget(langWidget);
    langList->setIconSize(QSize(ICON_WID, ICON_HEI));
    langList->setMinimumWidth(500);

    for (int i = 0; i < MAX_LANG; i++){
        items[i] = new QListWidgetItem;
        items[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        items[i]->setSizeHint(QSize(50, 60));
        langList->addItem(items[i]);
    }

    langSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    langCancelBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    langSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    langCancelBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->setContentsMargins(0, 25, 0, 10);
    upLay->addStretch();
    upLay->addWidget(langList);
    upLay->addStretch();
    downLay->addWidget(langSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(langCancelBtn, Qt::AlignRight);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);

    stackedWidget->addWidget(langWidget);

    connect(langCancelBtn, SIGNAL(pressed()), this, SLOT(onLangCancelBtnClicked()));
    connect(langSureBtn, SIGNAL(pressed()), this, SLOT(onLangSureBtnClicked()));
}

/* 老化测试界面 */
void factorySet::createAgeingTestPage()
{
    ageingTestWidget = new QWidget;
    ageingTestWidget->setGeometry(0, 0, width(), height());
    m_nMachineTestFlag = (struGsh.bFlagMachinetest == 0) ? 0 : 1;
    m_nDetectFre = struGsh.nSelfExamineFre;

    int groupWidth = 320;
    int groupHeight = 140;

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *midLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(ageingTestWidget);

    runningModeGroup = new myMutex(Qt::Vertical, m_nMachineTestFlag, ageingTestWidget);
    runningModeGroup->setNum(2);
    runningModeGroup->setTitle(myLan.run_mode);
    runningModeGroup->setLabelText(myLan.normal, myLan.run_test);
    runningModeGroup->setMinimumSize(QSize(groupWidth, groupHeight));

    QGroupBox *ageingTestFre = new QGroupBox(myLan.run_fre, ageingTestWidget);
    ageingTestFre->setFont(config->getFont());
    ageingTestFre->setMinimumSize(QSize(groupWidth, groupHeight));
    QHBoxLayout *groupLay  = new QHBoxLayout(ageingTestFre);
    ageingTestValueBar = new myValueBar(0, 255, 1, 10, m_nDetectFre, ageingTestFre);
    groupLay->addWidget(ageingTestValueBar);

    ageingTestSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    ageingTestSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    ageingTestBackBtn = new myPushButton(myLan.back,myIcon.Action_Back);
    ageingTestBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->addStretch();
    upLay->addWidget(runningModeGroup);
    upLay->addStretch();
    midLay->addStretch();
    midLay->addWidget(ageingTestFre);
    midLay->addStretch();
    downLay->addWidget(ageingTestSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(ageingTestBackBtn, Qt::AlignRight);
    mainLay->addSpacing(20);
    mainLay->addLayout(upLay);
    if(LCD_WIDTH == 640)
        mainLay->addSpacing(20);
    else
        mainLay->addSpacing(50);

    mainLay->addLayout(midLay);
    mainLay->addSpacing(20);
    mainLay->addLayout(downLay);

    connect(ageingTestSureBtn, SIGNAL(pressed()), this, SLOT(onAgeingTestSureBtnClicked()));
    connect(ageingTestBackBtn, SIGNAL(pressed()), this, SLOT(onAgeingTestBackBtnClicked()));
    connect(ageingTestValueBar, SIGNAL(valueChanged(int)), this, SLOT(onAgeingTestValueChangedSlt(int)));
    connect(runningModeGroup, SIGNAL(indexChanged(int)), this, SLOT(onAgeingTestFlagChangedSlt(int)));
    stackedWidget->addWidget(ageingTestWidget);
}

/* 生成产量测试界面 */
void factorySet::createOutputTestPage()
{
    outputTestWidget = new QWidget;
    outputTestWidget->setGeometry(0, 0, stackedWidget->width(), stackedWidget->height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *midLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(outputTestWidget);

    int groupWidth = 400;
    int groupHeight = 100;

    QGroupBox *testTimeGroup = new QGroupBox(myLan.test_time, outputTestWidget);
    testTimeGroup->setFont(config->getFont());
    testTimeGroup->setMinimumSize(QSize(groupWidth, groupHeight));
    QGridLayout *testTimeLayout = new QGridLayout;

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    outputTestMinusBtn = new myPushButton(myIcon.Action_Minus, btnSize);
    outputTestPlusBtn = new myPushButton(myIcon.Action_Plus, btnSize);

    outputTestSlider = new QSlider(Qt::Horizontal);
    outputTestSlider->setFixedSize(QSize(300, 40));
    outputTestSlider->setSliderPosition(5);
    outputTestSlider->setRange(0, 99);
    outputTestSlider->setValue(10);
    outputTestSlider->setStyleSheet("QSlider::groove:horizontal{border: 1px solid #999999;height: 45px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);     margin: 2px 0; }  QSlider::handle:horizontal {background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);     border: 1px solid #5c5c5c;     width: 45px;     margin: -2px 0;border-radius: 3px; }");
    outputTestTimeNum = new QLCDNumber(2);
    outputTestTimeNum->setFixedSize(QSize(60, 45));
    outputTestTimeNum->display(10);
    testTimeLayout->addWidget(outputTestMinusBtn, 0, 0, 1, 1);
    testTimeLayout->addWidget(outputTestSlider, 0, 1, 1, 5);
    testTimeLayout->addWidget(outputTestPlusBtn, 0, 6, 1, 1);
    testTimeLayout->addWidget(outputTestTimeNum, 0, 7, 1, 1);
    testTimeGroup->setLayout(testTimeLayout);

    outputTestStartBtn = new myPushButton(myLan.start,myIcon.Media_Start);
    outputTestStartBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    outputTestBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    outputTestBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->addStretch();
    upLay->addWidget(testTimeGroup);
    upLay->addStretch();
    midLay->addStretch();
    midLay->addWidget(outputTestStartBtn);
    midLay->addStretch();
    downLay->addStretch();
    downLay->addWidget(outputTestBackBtn);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addLayout(midLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    stackedWidget->addWidget(outputTestWidget);

    connect(outputTestSlider, SIGNAL(valueChanged(int)), outputTestTimeNum, SLOT(display(int)));
    connect(outputTestStartBtn, SIGNAL(pressed()), this, SLOT(onOutputTestStartBtnClicked()));
    connect(outputTestBackBtn, SIGNAL(pressed()), this, SLOT(onOutputTestBackBtnClicked()));
    connect(outputTestMinusBtn, SIGNAL(pressed()), this, SLOT(onOutputTestMinusBtnClicked()));
    connect(outputTestPlusBtn, SIGNAL(pressed()), this, SLOT(onOutputTestPlusBtnClicked()));
}

/* 生成下位机启动模式设置界面 */
void factorySet::createSetFPGAModePage()
{
    setFPGAUserModeWidget = new QWidget;
    setFPGAUserModeWidget->setGeometry(0, 0, stackedWidget->width(), stackedWidget->height());
    setFPGAModeParams();

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(setFPGAUserModeWidget);
    int groupHei = 300;

    intModeGroup = new myMutex(Qt::Vertical, m_nIntMode, setFPGAUserModeWidget);
    intModeGroup->setContentsMargins(1, 1, 1, 1);
    intModeGroup->setLabelText(myLan.mode_1, myLan.mode_2, myLan.mode_3);
    intModeGroup->setTitle(myLan.interface_board);
    intModeGroup->setMinimumHeight(groupHei);

    colorModeGroup = new myMutex(Qt::Vertical, m_nColorMode, setFPGAUserModeWidget);
    colorModeGroup->setContentsMargins(1, 1, 1, 1);
    colorModeGroup->setLabelText(myLan.mode_1, myLan.mode_2, myLan.mode_3);
    colorModeGroup->setTitle(myLan.color_board);
    colorModeGroup->setMinimumHeight(groupHei);

    setModeSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    setModeSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setModeBackbtn = new myPushButton(myLan.back, myIcon.Action_Back);
    setModeBackbtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->addWidget(intModeGroup);
    upLay->addSpacing(10);
    upLay->addWidget(colorModeGroup);
    upLay->addSpacing(10);
    downLay->addWidget(setModeSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(setModeBackbtn, Qt::AlignRight);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(setModeSureBtn, SIGNAL(pressed()), this, SLOT(onSetModeSureBtnClicked()));
    connect(setModeBackbtn, SIGNAL(pressed()), this, SLOT(onSetModeBackBtnClicked()));
    connect(intModeGroup, SIGNAL(indexChanged(int)), this, SLOT(onIntModeChangedSlt(int)));
    connect(colorModeGroup, SIGNAL(indexChanged(int)), this, SLOT(onColorModeChangedSlt(int)));

    stackedWidget->addWidget(setFPGAUserModeWidget);
}

/* FPGA模式参数的设置 */
void factorySet::setFPGAModeParams()
{
    //除以2是因为模式的索引号0、1、2对应的参数为1、2、4
    m_nIntMode = struCnfp.nInterfaceBoardMode/2;
    m_nColorMode = struCnfp.nColorBoardMode/2;
}

/* 响应子界面返回主界面的操作 */
void factorySet::backToFactorySet()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
    factoryUpdateSlt();
}

/* 响应厂家设置界面的返回按钮　*/
void factorySet::onFactoryBackBtnClicked()
{
    myFlow.updateOnoff(1);
    emit goToHomePage();
}

/* 响应厂家设置界面的设置彩色相机按钮　*/
void factorySet::onSetColorCameraBtnClicked()
{
    myMessageBox msgBox(MSG_QUES, myLan.confirm_set_camera);
	if (msgBox.exec() == QDialog::Accepted) {
        struGsh.bIsAssist = false;  //当前界面显示的是主配信号
        myFlow.updateOnoff(0);      // 进入相机设置界面后发送校准停止命令
        stackedWidget->setCurrentIndex(SET_COLOR_CAMERA_PAGE);
	}
}

/* 响应厂家设置界面的设置语言按钮　*/
void factorySet::onSetLanguageBtnClicked()
{
    int index = 0;
    
    struCnfe.nLangList[0] = 20;
    for (int i = 0; i < 20; i++){
        struCnfe.nLangList[i+1] = i+1;
    }

    for (int i = 0; i < struCnfe.nLangList[0]; i++) {
        items[i]->setHidden(false);
        nLangIndex[i] = struCnfe.nLangList[i+1];
        items[index]->setText(myFlow.getLanguageListName(nLangIndex[i], false));
        items[index]->setIcon(myFlow.getLanguageListIcon(nLangIndex[i]));
        index++;
    }

    //for (int i = struCnfe.nLangList[0]; i < MAX_LANG; i++){
    //    items[i]->setHidden(true);
    //}

    for(int i = 0; i < struCnfe.nLangList[0]; i++){
        if (nLangIndex[i] == struCnfg.nLang) {
            langList->setCurrentRow(i);
        }
    }

    stackedWidget->setCurrentIndex(SET_LANG_PAGE);
}

/* 响应语言设置界面的确认按钮 */
void factorySet::onLangSureBtnClicked()
{
    int oldLang = struCnfg.nLang;
    int newLang = nLangIndex[langList->currentRow()];

    if(newLang != oldLang){
        myMessageBox mesBox(MSG_QUES, myLan.cfm_change_language);
        int ret = mesBox.exec();
        if(ret == QDialog::Accepted){
            struCnfg.nLang = newLang;
            myFlow.setLang();
            myFlow.saveCamera();
            myFlow.saveGlobal();
            myFlow.saveProfile();
            system("./myrestart.sh");
        }

        return;
    }
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应语言设置界面的取消按钮　*/
void factorySet::onLangCancelBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应厂家设置界面的设置机型按钮　*/
void factorySet::onSetMachineModeBtnClicked()
{
    stackedWidget->setCurrentIndex(SET_PRODUCT_PAGE);
    emit refreshMachineModeDisplaySig();
}

/* 响应厂家设置界面的清零累计运行时间按钮　*/
void factorySet::onClearRunningTimeBtnClicked()
{
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_clear_time);
    int ret = mesBox->exec();
    if(ret == QDialog::Accepted){
        struCnfg.nCounter = 0;
    }
}
/* 响应厂家设置界面的清零累计运行时间按钮　*/
void factorySet::onSwitchToCFBtnClicked()
{
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, "确定启动CF程序");
    int ret = mesBox->exec();
    if(ret == QDialog::Accepted)
         system("/app/startcf.sh -d");
}

/* 响应厂家设置界面的校准触摸屏按钮　*/
void factorySet::onCalibrationBtnClicked()
{
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_ts_calibrate);
    int ret = mesBox->exec();
    if(ret == QDialog::Accepted){
        char cmd[64];
        sprintf(cmd, "rm /etc/pointercal");
        system(cmd);
        sprintf(cmd, "rm /userdata/images-335x/pointercal");
        system(cmd);
        myFlow.msleep(10);
        sprintf(cmd, "reboot");
        system(cmd);
    }
}

/* 响应厂家设置界面的老化测试按钮　*/
void factorySet::onAgeingTestBtnClicked()
{
    m_nDetectFre = struGsh.nSelfExamineFre;
    m_nMachineTestFlag = (struGsh.bFlagMachinetest == 0) ? 0 : 1;
    runningModeGroup->setCurrentIndex(m_nMachineTestFlag);
    ageingTestValueBar->setValue(m_nDetectFre);
    stackedWidget->setCurrentIndex(AGEING_TEST_PAGE);
}

/* 响应老化测试界面的确定按钮　*/
void factorySet::onAgeingTestSureBtnClicked()
{
    struGsh.nSelfExamineFre = m_nDetectFre;
    struGsh.bFlagMachinetest = (m_nMachineTestFlag == 0) ? 0 : 2;
    
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应老化测试界面的返回按钮　*/
void factorySet::onAgeingTestBackBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应老化测试界面测试频率变化的情况 */
void factorySet::onAgeingTestValueChangedSlt(int value)
{
    m_nDetectFre = value;
}

/* 响应老化测试界面整机状态变化的情况 */
void factorySet::onAgeingTestFlagChangedSlt(int index)
{
    m_nMachineTestFlag = index;
}

/* 响应厂家设置界面的产量测试按钮　*/
void factorySet::onOutputTestBtnClicked()
{
    outputTestTimeNum->display(10);
    outputTestSlider->setValue(10);
    stackedWidget->setCurrentIndex(OUTPUT_TEST_PAGE);
}

/* 响应产量测试界面的增加按钮　*/
void factorySet::onOutputTestPlusBtnClicked()
{
    int tmp = outputTestTimeNum->value();
    if(tmp < 99){
        outputTestTimeNum->display(tmp+1);
        outputTestSlider->setValue(tmp+1);
    }
}

/* 响应产量测试界面的减少按钮　*/
void factorySet::onOutputTestMinusBtnClicked()
{
    int tmp = outputTestTimeNum->value();
    if(tmp > 1){
        outputTestTimeNum->display(tmp-1);
        outputTestSlider->setValue(tmp-1);
    }
}

/* 响应产量测试界面的开始按钮　*/
void factorySet::onOutputTestStartBtnClicked()
{   
    infoWidget->setLabelText(myLan.msg_testing);
    infoWidget->delayShow();
    myFlow.onOff();
    outputTestTimer = new QTimer;
    connect(outputTestTimer,SIGNAL(timeout()),this,SLOT(outputTimeoutSlt()));
    outputTestTimer->start(1000*outputTestTimeNum->value());
}

/* 响应产量测试界面的返回按钮 */
void factorySet::onOutputTestBackBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应厂家设置界面的升级上位机程序按钮　*/
void factorySet::onUpdateARMBtnClicked()
{   
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_upgrade_screen);
    int ret = mesBox->exec();
    if(ret == QDialog::Accepted){
        QProcess *process = new QProcess;
        QStringList strList;

        //! MC机型升级仅显示英文
        if (struGsh.bIsMC) {
            strList << "-eng";
        }

        strList << "-qws" << "-display" << "VNC:LinuxFB";

#ifdef Q_OS_UNIX
//        process->startDetached("./MyUpdate", strList);
        system("killall DuySorter");
#endif
    }
}

/* 响应厂家设置界面的升级下位机程序按钮　*/
void factorySet::onUpdateFPGABtnClicked()
{  
    myMessageBox *mesBox = new myMessageBox(MSG_QUES,  myLan.cfm_upgrade_fpga);
    int ret = mesBox->exec();
    if(ret == QDialog::Accepted){
#ifdef Q_OS_UNIX
        emit hideTitleBtns();
#endif
        upgradeFpgaWidget->refreshDisplay();
        stackedWidget->setCurrentIndex(UPDATE_FPGA_PAGE);
    }
}

/* 响应厂家设置界面的设置下位机启动模式按钮　*/
void factorySet::onSetFPGAStartModeBtnClicked()
{
    setFPGAModeParams();
    intModeGroup->setCurrentIndex(m_nIntMode);
    colorModeGroup->setCurrentIndex(m_nColorMode);

    stackedWidget->setCurrentIndex(SET_START_MODE_PAGE);
}

/* 响应设置用户模式界面下确认按钮　*/
void factorySet::onSetModeSureBtnClicked()
{
    myMessageBox msgBox(MSG_QUES, myLan.msg_change_init_mode);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted) {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        
		if (m_nIntMode == 0) {
        	struCnfp.nInterfaceBoardMode = 1;
        } else {
            struCnfp.nInterfaceBoardMode = m_nIntMode*2;
        }

        if (m_nColorMode == 0) {
            struCnfp.nColorBoardMode = 1;
        } else {
            struCnfp.nColorBoardMode = m_nColorMode*2;
        }

        myFlow.saveProfile();
        myFlow.saveCamera();
        myFlow.saveGlobal();

        myFlow.resetFPGAMode();
        myFlow.sleep(10);
        
		myFlow.initSerial();            // 初始化串口设备
        myFlow.initMachineType();		// 初始化机器类型
		myFlow.initCommunication();
        myFlow.initSendAllParams();
        infoWidget->hide();
        stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
    }
}

/* 响应设置用户模式界面下取消按钮　*/
void factorySet::onSetModeBackBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应设置用户模式界面下接口板模式变化的情况 */
void factorySet::onIntModeChangedSlt(int index)
{
    m_nIntMode = index;
}

/* 响应设置用户模式界面下彩色相机板模式变化的情况 */
void factorySet::onColorModeChangedSlt(int index)
{
    m_nColorMode = index;
}

/* 减少剩余的测试时间 */
void factorySet::outputTimeoutSlt()
{
    outputTestTimer->stop();
    myFlow.onOff();
    infoWidget->hide();
}

void factorySet::createBacklightPasswordPage()
{
    setBacklightPasswordWidget = new QWidget;
    setBacklightPasswordWidget->setGeometry(0, 0, stackedWidget->width(), stackedWidget->height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QGridLayout *midLay = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(setBacklightPasswordWidget);

    enablePasswordChx = new myCustomCheckBox(myLan.enable_screensaver, false);

    newPasswordLabel = new myLabel(myLan.input_screensaver_password, setBacklightPasswordWidget);
    newPasswordConfrimLabel = new myLabel(myLan.confirm_screensaver_password, setBacklightPasswordWidget);

    newPasswordLineEdit = new myLineEdit("", setBacklightPasswordWidget);
    newPasswordConfirmLineEdit = new myLineEdit("", setBacklightPasswordWidget);

    newPasswordLineEdit->setReadOnly(true);
    newPasswordConfirmLineEdit->setReadOnly(true);

    newPasswordLineEdit->setMinimumHeight(BTN_HEIGHT);
    newPasswordConfirmLineEdit->setMinimumHeight(BTN_HEIGHT);

    setBacklightPasswordSureBtn = new myPushButton(myLan.ok, myIcon.Action_Apply);
    setBacklightPasswordSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setBacklightPasswordCancelBtn = new myPushButton(myLan.cancel, myIcon.Action_Cancel);
    setBacklightPasswordCancelBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->addStretch();
    upLay->addWidget(enablePasswordChx);
    upLay->addStretch();

    midLay->addWidget(newPasswordLabel, 0, 0, 1, 1, Qt::AlignRight);
    midLay->addWidget(newPasswordLineEdit, 0, 1, 1, 1, Qt::AlignLeft);

    midLay->addWidget(newPasswordConfrimLabel, 1, 0, 1, 1, Qt::AlignRight);
    midLay->addWidget(newPasswordConfirmLineEdit, 1, 1, 1, 1, Qt::AlignLeft);

    downLay->addWidget(setBacklightPasswordSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(setBacklightPasswordCancelBtn, Qt::AlignRight);

    mainLay->addStretch();
    mainLay->addLayout(upLay);

    mainLay->addStretch();
    mainLay->addLayout(midLay);

    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(setBacklightPasswordSureBtn, SIGNAL(pressed()), this, SLOT(onSetBacklightPasswordSureBtnClicked()));
    connect(setBacklightPasswordCancelBtn, SIGNAL(pressed()), this, SLOT(onSetBacklightPasswordCancelBtnClicked()));
    connect(enablePasswordChx, SIGNAL(pressed()), this, SLOT(onSetBacklightPasswordEnableBtnClicked()));

    connect(newPasswordLineEdit, SIGNAL(pressed()), this, SLOT(onSetNewPasswordClicked()));
    connect(newPasswordConfirmLineEdit, SIGNAL(pressed()), this, SLOT(onSetNewPasswordConfirmClicked()));

    stackedWidget->addWidget(setBacklightPasswordWidget);
}

void factorySet::onSetNewPasswordClicked()
{
    tmpPassword.clear();
    QString strPassword = "";
    myInputPanel inputDlg(passwdType, 0, 0, 0);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        tmpPassword = inputDlg.getText();
        for (int i = 0; i < tmpPassword.size(); i++) {
            strPassword += "*";
        }
    }
    newPasswordLineEdit->setText(strPassword);
}

void factorySet::onSetNewPasswordConfirmClicked()
{
    tmpPasswordConfirm.clear();
    QString strPassword = "";
    myInputPanel inputDlg(passwdType, 0, 0, 0);
    int ret  = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        tmpPasswordConfirm = inputDlg.getText();
        for (int i = 0; i < tmpPasswordConfirm.size(); i++) {
            strPassword += "*";
        }
    }
    newPasswordConfirmLineEdit->setText(strPassword);
}

void factorySet::onSetBacklightPasswordBtnClicked()
{
    tmpEnablePassword = struCnfg.nEnableBacklightPassword;
    enablePasswordChx->setChecked(tmpEnablePassword);
    if(tmpEnablePassword) {
        newPasswordLineEdit->setEnabled(true);
        newPasswordConfirmLineEdit->setEnabled(true);
    }
    else {
        newPasswordLineEdit->setEnabled(false);
        newPasswordConfirmLineEdit->setEnabled(false);
    }

    newPasswordLineEdit->setText("");
    newPasswordConfirmLineEdit->setText("");

    stackedWidget->setCurrentIndex(SET_BACKLIGHT_PASSWORD_PAGE);
}

void factorySet::onSetBacklightPasswordEnableBtnClicked()
{
    tmpEnablePassword = !tmpEnablePassword;

    if(tmpEnablePassword) {
        newPasswordLineEdit->setEnabled(true);
        newPasswordConfirmLineEdit->setEnabled(true);
    }
    else {
        newPasswordLineEdit->setEnabled(false);
        newPasswordConfirmLineEdit->setEnabled(false);
        QString strPassword = "";

        tmpPassword.clear();
        tmpPasswordConfirm.clear();
        for (int i = 0; i < tmpPasswordConfirm.size(); i++) {
            strPassword += "*";
        }
        newPasswordConfirmLineEdit->setText(strPassword);

        strPassword.clear();
        for (int i = 0; i < tmpPassword.size(); i++) {
            strPassword += "*";
        }
        newPasswordLineEdit->setText(strPassword);
    }
}

void factorySet::onSetBacklightPasswordSureBtnClicked()
{
    QString strPassword = "";

    if(tmpPassword != tmpPasswordConfirm) {
        infoWidget->setLabelText(myLan.msg_password_error);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();

        tmpPassword.clear();
        tmpPasswordConfirm.clear();
        for (int i = 0; i < tmpPasswordConfirm.size(); i++) {
            strPassword += "*";
        }
        newPasswordConfirmLineEdit->setText(strPassword);

        strPassword.clear();
        for (int i = 0; i < tmpPassword.size(); i++) {
            strPassword += "*";
        }
        newPasswordLineEdit->setText(strPassword);
        return;
    }
    else {
	if(tmpPassword.size() > 0) {
            sprintf(struCnfg.sBacklightPassword, "%s", tmpPassword.toLatin1().data());
	}
        tmpPassword.clear();
        tmpPasswordConfirm.clear();

        struCnfg.nEnableBacklightPassword = tmpEnablePassword;
        stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
    }
}

void factorySet::onSetBacklightPasswordCancelBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 生成设置振动器高低电压界面 */
void factorySet::createSetFeederVoltage()
{
    setFeederVoltageWidget = new QWidget;
    setFeederVoltageWidget->setGeometry(0, 0, stackedWidget->width(), stackedWidget->height());

    QVBoxLayout *upLay = new QVBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(setFeederVoltageWidget);
    int groupWid = 400;
    int groupHei = 200;

    feederVoltageGroup = new myMutex(Qt::Vertical, struCnfg.nFeederVoltage, setFeederVoltageWidget);
    feederVoltageGroup->setNum(2);
    feederVoltageGroup->setContentsMargins(1, 1, 1, 1);
    feederVoltageGroup->setLabelText(myLan.feeder_voltage_low, myLan.feeder_voltage_high);
    feederVoltageGroup->setTitle(myLan.feeder_voltage);
    feederVoltageGroup->setFixedSize(groupWid, groupHei);

    autoFeedEnable = new myPushButton("", QIcon(), setFeederVoltageWidget);
    autoFeedEnable->setFixedSize(config->getBtnSize(SMALL_BTN_SIZE));
    myLabel *autoFeedLabel = new myLabel(myLan.feeder_AI, setFeederVoltageWidget);
    autoFeedLabel->setFixedSize(config->getBtnSize(DEFAULT_BTN_SIZE));
    if (struCnfp.autoFeedEnablePageFlag == 1) {
        autoFeedEnable->setIcon(myIcon.Action_Apply);
    } else {
        autoFeedEnable->setIcon(QIcon());
    }

    statisticEnable = new myPushButton("", QIcon(), setFeederVoltageWidget);
    statisticEnable->setFixedSize(config->getBtnSize(SMALL_BTN_SIZE));
    myLabel *statisticLabel = new myLabel(myLan.bigData, setFeederVoltageWidget);
    statisticLabel->setFixedSize(config->getBtnSize(DEFAULT_BTN_SIZE));
    if (struCnfg.nStatisticEnable == 1) {
        statisticEnable->setIcon(myIcon.Action_Apply);
    }
    statisticLabel->hide();
    statisticEnable->hide();

    QGroupBox *otherGroup = new QGroupBox(myLan.other);
    otherGroup->setFont(config->getFont());
    otherGroup->setMinimumWidth(groupWid);
    QGridLayout *otherLayout = new QGridLayout(otherGroup);

    if ((struCnfe.nMachine == MACHINE_CF) || (struCnfe.nEnableBeanMachine == 1)) {
        otherLayout->addWidget(autoFeedEnable,0,0,Qt::AlignHCenter);
        otherLayout->addWidget(autoFeedLabel,0,1,Qt::AlignHCenter);
        autoFeedEnable->show();
        otherGroup->show();
    } else {
        autoFeedEnable->hide();
        otherGroup->hide();
    }

    otherLayout->addWidget(statisticEnable,1,0,Qt::AlignHCenter);
    otherLayout->addWidget(statisticLabel,1,1,Qt::AlignHCenter);

    setFeederVoltageSureBtn = new myPushButton(myLan.apply, myIcon.Action_Apply);
    setFeederVoltageSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setFeederVoltageBackbtn = new myPushButton(myLan.back, myIcon.Action_Back);
    setFeederVoltageBackbtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    /* 功能特点：点击即生效 */
    setFeederVoltageSureBtn->hide();
    upLay->addWidget(feederVoltageGroup,0,Qt::AlignHCenter);
    upLay->addWidget(otherGroup,0,Qt::AlignHCenter);

    downLay->addWidget(setFeederVoltageSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(setFeederVoltageBackbtn, Qt::AlignRight);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(setFeederVoltageSureBtn, SIGNAL(pressed()), this, SLOT(onSetFeederVoltageSureBtnClicked()));
    connect(setFeederVoltageBackbtn, SIGNAL(pressed()), this, SLOT(onSetFeederVoltageBackBtnClicked()));
    connect(feederVoltageGroup, SIGNAL(indexChanged(int)), this, SLOT(onSetFeederVoltageChangedSlt(int)));
    connect(autoFeedEnable, SIGNAL(pressed()), this, SLOT(onAutoFeedEnablePressedSlt()));
    connect(statisticEnable, SIGNAL(pressed()), this, SLOT(onStatisticEnablePressedSlt()));

    stackedWidget->addWidget(setFeederVoltageWidget);
}

/* 响应厂家设置界面的设置振动器高低电压按钮　*/
void factorySet::onSetFeederVolatageBtnClicked()
{
    feederVoltageGroup->setCurrentIndex(struCnfg.nFeederVoltage);
    if (struCnfp.autoFeedEnablePageFlag == 1) {
        autoFeedEnable->setIcon(myIcon.Action_Apply);
    } else {
        autoFeedEnable->setIcon(QIcon());
    }
    stackedWidget->setCurrentIndex(SET_FEEDER_VOLTAGE);
}

/* 响应设置振动器高低电压界面下确认按钮　*/
void factorySet::onSetFeederVoltageSureBtnClicked()
{
    MySerial.com2Write(CMD_CTRL_FEED_VOLTAGE, CTRL_GENERAL, 0, 0, 0, 0, 0, struCnfg.nFeederVoltage, 3);
    MySerial.com2Write(CMD_CTRL_FEED_VOLTAGE, CTRL_GENERAL, 1, 0, 0, 0, 0, struCnfg.nFeederVoltage, 3);
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应设置振动器高低电压界面下取消按钮　*/
void factorySet::onSetFeederVoltageBackBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/* 响应设置振动器高低电压界面下电压变化的情况 */
void factorySet::onSetFeederVoltageChangedSlt(int index)
{
    struCnfg.nFeederVoltage = index;
    MySerial.com2Write(CMD_CTRL_FEED_VOLTAGE, CTRL_GENERAL, 0, 0, 0, 0, 0, struCnfg.nFeederVoltage, 3);
    MySerial.com2Write(CMD_CTRL_FEED_VOLTAGE, CTRL_GENERAL, 1, 0, 0, 0, 0, struCnfg.nFeederVoltage, 3);
}

/* 智能供料使能按钮 */
void factorySet::onAutoFeedEnablePressedSlt()
{
    if (struCnfp.autoFeedEnablePageFlag == 0) {
        struCnfp.autoFeedEnablePageFlag = 1;
        autoFeedEnable->setIcon(myIcon.Action_Apply);
    } else {
        struCnfp.autoFeedEnablePageFlag = 0;
        autoFeedEnable->setIcon(QIcon());
    }
}

void factorySet::onStatisticEnablePressedSlt()
{
    if (struCnfg.nStatisticEnable == 0) {
        struCnfg.nStatisticEnable = 1;
        statisticEnable->setIcon(myIcon.Action_Apply);
    } else {
        struCnfg.nStatisticEnable = 0;
        statisticEnable->setIcon(QIcon());
    }
}

/* 进入灯控设置页面*/
void factorySet::onLampCtrlBtnClicked()
{
    pageLampCtrl->refreshDisplay();
    stackedWidget->setCurrentIndex(LAMP_CTRCL_PAGE);
}

/* 响应消磁时间页面按钮 */
void factorySet::onDegaussTimeBtnClicked()
{
    m_degaussTimeWidget->updateDegaussTimePage();
    stackedWidget->setCurrentIndex(DEGAUSS_TIME_PAGE);
}

/* 响应消磁时间界面设置返回按钮 */
void factorySet::onDegaussTimeBackBtnClicked()
{
    stackedWidget->setCurrentIndex(FACTORY_SET_PAGE);
}

/*!
 * \brief 响应料位外接控制按钮
 */
void factorySet::onFeederCtrlBtnPressed()
{
    m_feederCtrlWidget->refreshDisplay();
    stackedWidget->setCurrentIndex(FEEDER_REMOTE_CONTROL_PAGE);
}

/*!
 * \brief 吞吐量页面Slt
 */
void factorySet::onThroughPutBtnPressedSlt()
{
    stackedWidget->setCurrentIndex(THROUGH_PUT_PAGE);
    m_througPutWidget->updatePage();
}

/*!
 * \brief 剔除模式页面Slt
 */
void factorySet::onTickModeBtnPressedSlt()
{
    stackedWidget->setCurrentIndex(TICK_MODE_PAGE);
    m_tickModeWidget->updatePage();
}

void factorySet::onAiSetBtnPressedSlt(){
    stackedWidget->setCurrentIndex(AI_SET_PAGE);
    aiEnable = struCnfg.aiEnable;
    aiEnbaleChx->setChecked(aiEnable);
    imgFetchHeight = struCnfg.imgFetchHeight;
    imgInferHeight = struCnfg.imgInferHeight;
    imgPicHeight = struCnfg.imgPicHeight;
    imgVideoHeight = struCnfg.imgVideoHeight;
    imgFetchHeightEdit->setText(QString("%1").arg(imgFetchHeight));
    imgInferHeightEdit->setText(QString("%1").arg(imgInferHeight));
    imgPicHeightEdit->setText(QString("%1").arg(imgPicHeight));
    imgVideoHeightEdit->setText(QString("%1").arg(imgVideoHeight));

}

/* 页面切至第一页 */
void factorySet::gotoFirstPageSlt()
{
    stackedWidget->setCurrentIndex(0);
    emit sonIntGoToFirstSig();
}

/* 响应当前页面索引发生变化的情况 */
void factorySet::onStackedWidgetIndexChangedSlt(int index)
{
    QString title;
    switch(index){
    case FACTORY_SET_PAGE:
        title = QString("%1").arg(myLan.factory_set);
        break;
    case SET_COLOR_CAMERA_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.color_camera);
        break;
    case SET_LANG_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.language);
        break;
    case SET_PRODUCT_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.product);
        break;
    case AGEING_TEST_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.run_test);
        break;
    case OUTPUT_TEST_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.throughput_test);
        break;
    case UPDATE_FPGA_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.upgrade_fpga);
        break;
    case SET_START_MODE_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.init_mode);
        break;
    case SET_BACKLIGHT_PASSWORD_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.set_screensaver);
        break;
    case SET_FEEDER_VOLTAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.feeder_voltage);
        break;
    case LAMP_CTRCL_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.lamp_control);
        break;
    case DEGAUSS_TIME_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.degauss_time);
        break;
    case AUTO_SIG_LIST_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.auto_set+myLan.init_autoSig);
        break;
    case THROUGH_PUT_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg(myLan.throughput_test);
        break;
    case TICK_MODE_PAGE:
        title = QString("%1 > %2").arg(myLan.factory_set).arg("剔除模式");
        break;
    default:
        break;
    }
    titleLabel->setText(title);
}

/*!
  * \brief 设备加密模块slt
  */
void factorySet::onEncryptBtnBtnClickedSlt(){
//    qDebug()<<"yes"<<flush;
     setEncryt dlg;
     int ret = dlg.exec();
//     if (ret == QDialog::Accepted) {

//     }
}
