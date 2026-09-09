/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mainwidget.cpp
 * \brief       程序主界面源文件
 * \date        2015.01.14
 */
#include "mainwidget.h"
#include "unilog.h"

myApplication::myApplication(int& argc, char** argv) :
    QApplication(argc, argv)
{
}

bool myApplication::qwsEventFilter(QObject* w, QEvent* event)
{
    //do something
    struGsh.nBacklightCounter = 0;
    if (struGsh.nBacklightStat == 0)
    {
        myFlow.setTsBackLight(1);
        if (struCnfg.nEnableBacklightPassword == 1)
        {        // 使能屏保解锁时，密码框才有效
            myFlow.sendTsBackLightOn();
        }
        return true;
    }
#if defined (Q_OS_LINUX)
    return QApplication::eventFilter(w, event);
#endif
    return true;
}

/***
 *   主控件
 */
MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent)
{
    QString text = "";

    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT);
    setFixedSize(QSize(LCD_WIDTH, LCD_HEIGHT));
    config = g_Config::getInstance();
    // 标题栏默认不显示
    setAutoFillBackground(true);
    setWindowFlags(Qt::FramelessWindowHint);
    //    setAttribute(Qt::WA_TranslucentBackground,true);

        //! 初始化图标
    myIcon.init();

    /* 更新系统时间线程 */
    updateStatus = new updateStatusThread;
    updateStatus->start();

    /* 获取报警状态线程 */
    getStatus = new getStatusThread;
    getStatus->start();

    /* 网络上传后台监控线程 */
    createNetworkUploader();

    /* 吹气次数获取线程*/
    createGetEjectTimesThread();

    /* 设备状态网络监听服务 */
    myDevMonitor = new MyDevMonitor();

    // 标题
    titleLabel = new QLabel(QString(""), this);
    titleLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::black);
    titleLabel->setPalette(pe);
    titleLabel->hide();

    // 视频
//    videoBtn = new myPushButton(myLan.sorter_manager, myIcon.Media_Video,
//                                      true,false,this);

//    oprModeBtn = new myPushButton(QString(myLan.pic_customer), QIcon(""),
//                                      true,false,this);
//    oprModeBtn->hide();
//    mainOprBtn = new myPushButton(QString(myLan.schemeNew_select), QIcon(""),
//                                      true,false,this);
//    videoBtn = new myPushButton(QString(myLan.videoNew), QIcon(""),
//                                      true,false,this);
//    oprModeBtn->setFixedSize(BTN_WIDTH-40,BTN_HEIGHT+5);
//    mainOprBtn->setFixedSize(BTN_WIDTH-30,BTN_HEIGHT+5);
//    videoBtn->setFixedSize(BTN_WIDTH-40,BTN_HEIGHT+5);

//    QString strStyle1 = QString("QPushButton {border-image: url(:/res/png/selectUnpress.png);}");
//    oprModeBtn->setStyleSheet(strStyle1);
//    videoBtn->setStyleSheet(strStyle1);

//    QString strStyle2 = QString("QPushButton {border-image: url(:/res/png/selectPress.png);}");
//    mainOprBtn->setStyleSheet(strStyle2);


//    if(struCnfg.nLang == LANG_TURKEY){
//       oprModeBtn->setButtonFont(config->getFont(FONT_SIZE_16));
//       mainOprBtn->setButtonFont(config->getFont(FONT_SIZE_16));
//       videoBtn->setButtonFont(config->getFont(FONT_SIZE_16));
//    }


//    backToHomeBtn = new myPushButton(myLan.page_home, myIcon.Path_Home,
//                                     true, true, this);
//    backToHomeBtn->setHidden(true);

    //获取系统当前时间
    QDateTime time = QDateTime::currentDateTime();
    statInfoBtn = new myPushButton(time.toString("hh:mm:ss"), QIcon(""),
        true, false, this);
    statInfoBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    statInfoBtn->setIconSize(QSize(ICON_WID, ICON_WID));
    statInfoBtn->setIcon(QIcon(":/res/png/time.png"));
    statInfoBtn->setStyleSheet("background-color:transparent; border:none;");

    lineTitle = new QFrame(this);
    lineTitle->setFrameShape(QFrame::HLine);
    lineTitle->setFrameShadow(QFrame::Plain);
    lineTitle->setLineWidth(2);
    lineTitle->setStyleSheet("color: black");

    // 分割线
    lineStatus = new QFrame(this);
    lineStatus->setFrameShape(QFrame::HLine);
    lineStatus->setFrameShadow(QFrame::Plain);
    lineStatus->setLineWidth(2);
    lineStatus->setStyleSheet("color: black");

    // 系统关闭 myLan.halt
//    systemHaltBtn = new myPushButton("",QIcon(""),
//                                       true, false, this);

//    systemHaltBtn = new myPushButton(myLan.halt, QIcon(""),
//                                      true,false,this);
//    systemHaltBtn->setFont(config->getFont(FONT_SIZE_16));
//    QString strStyle3 = QString("QPushButton {border-image: url(:/res/png/shutDown.png);}");
//    systemHaltBtn->setStyleSheet(strStyle3);

    videoBtn = new QToolButton(this);
    videoBtn->setText(myLan.sorter_manager);
    videoBtn->setIconSize(QSize(60, 60));
    videoBtn->setIcon(QIcon(":/res/png/workStart.png"));
    videoBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    videoBtn->setStyleSheet("background-color:transparent; border:none;");
    videoBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    videoBtn->setAutoRaise(true);

    systemHaltBtn = new QToolButton(this);
    systemHaltBtn->setText(myLan.halt);
    systemHaltBtn->setIconSize(QSize(60, 60));
    systemHaltBtn->setIcon(QIcon(":/res/png/shutDown.png"));
    systemHaltBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    systemHaltBtn->setStyleSheet("background-color:transparent; border:none;");
    systemHaltBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    systemHaltBtn->setAutoRaise(true);


    //    saveParamBtn = new myPushButton(myLan.save_parameterNew, QIcon(""),
    //                                      true,false,this);
    //    saveParamBtn->setFont(config->getFont(FONT_SIZE_16));
    //    QString strStyle4 = QString("QPushButton {border-image: url(:/res/png/savePara.png);}");
    //    saveParamBtn->setStyleSheet(strStyle4);

    saveParamBtn = new QToolButton(this);
    saveParamBtn->setText(myLan.save_parameterNew);
    saveParamBtn->setIconSize(QSize(60, 60));
    saveParamBtn->setIcon(QIcon(":/res/png/savePara.png"));
    saveParamBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    saveParamBtn->setStyleSheet("background-color:transparent; border:none;");
    saveParamBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    saveParamBtn->setAutoRaise(true);

    //    feedSwitchBtn = new myPushButton(myLan.working_stop, QIcon(""),
    //                                      true,false,this);
    //    feedSwitchBtn->setFont(config->getFont(FONT_SIZE_16));
    //    QString strStyle5 = QString("QPushButton {border-image: url(:/res/png/workStart.png);"
    //                                "text-align:center}");
    //    feedSwitchBtn->setStyleSheet(strStyle5);

    feedSwitchBtn = new QToolButton(this);
    feedSwitchBtn->setText(myLan.working_stop);
    feedSwitchBtn->setIconSize(QSize(80, 80));
    feedSwitchBtn->setIcon(QIcon(":/res/png/workStart.png"));
    feedSwitchBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    feedSwitchBtn->setStyleSheet("background-color:transparent; border:none;");
    feedSwitchBtn->setFont(config->getFont(DEFAULT_FONT_SIZE));
    feedSwitchBtn->setAutoRaise(true);
    QPalette patlette;
    patlette.setColor(QPalette::ButtonText, Qt::black);
    feedSwitchBtn->setPalette(patlette);

    // 机器和工程师信息
//    machineInfoBtn = new myPushButton(myLan.contact_us,myIcon.System_Contacts,
//                                       true, false, this);
//    machineInfoBtn->setHidden(true);
    // 供料开关
    text = myLan.feed + " " + myLan.off;
    //    feedSwitchBtn = new myPushButton(QString(myLan.working_stop),QIcon(""),
    //                                      true, false, this);
        // 皮带开关
    text = myLan.belt + " " + myLan.off;
    //    beltSwitchBtn = new myPushButton(text, myIcon.Sorter_Feeder, true, true, this);
    //    beltSwitchBtn->hide();

        // 一键放料
    //    m_oneKeyFeedBtn = new myPushButton(myLan.one_key_feed, QIcon(), true, true, this);
    //    m_oneKeyFeedBtn->hide();
    m_bIsFeed = false;

    // 方案参数保存
//    saveParamBtn = new myPushButton(QString(myLan.save_parameterNew), QIcon(""),
//                                      true, false,this);

    // 权限设置
//    setPurviewBtn = new myPushButton(myLan.set_purview,myIcon.System_Lock,
//                                      true, false,this);
//    setPurviewBtn->setHidden(true);


    // 主堆栈页面
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedWidth(LCD_WIDTH - 10);
    stackedWidget->setFixedHeight(LCD_HEIGHT - LCD_TITLE_HEIGHT - 20);

    // 通信检测页面
    pageComm = new communication(this);
    stackedWidget->addWidget(pageComm);

    // 通信检测列表页面
    pageCommList = new communicationList(this);
    stackedWidget->addWidget(pageCommList);

    // 初始化页面
    pageInit = new initialization(this);
    stackedWidget->addWidget(pageInit);

    // 预热页面
    pagePreheat = new preheat(this);
    stackedWidget->addWidget(pagePreheat);

    // 主页面
    pageHome = new QWidget(this);
    stackedWidget->addWidget(pageHome);
    CreateHomeWidget();

    // 主页面-T1机型
    pageHomeTea = new QWidget(this);
    stackedWidget->addWidget(pageHomeTea);
    CreateHomeTeaWidget();

    // 设置供料页面
//    if(struCnfe.nProduct == OPT_PRODUCT_ANCOO_RS
//            || struCnfe.nProduct == OPT_PRODUCT_ANCOO_RDC
//            || struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC
//            || struCnfe.nEnableCGMachine == 1) {
//        pageSetFeedRS = new setFeedPage();
//        stackedWidget->addWidget(pageSetFeedRS);
//    } else {
//        pageSetFeed = new QWidget();
//        stackedWidget->addWidget(pageSetFeed);
//        CreateSetFeedWidget();
//    }

    // 设置清灰页面
//    pageSetWipe = new QWidget();
//    stackedWidget->addWidget(pageSetWipe);
//    CreateSetWipeWidget();

//    // 喷阀自检页面
//    pageTestEject = new QWidget();
//    stackedWidget->addWidget(pageTestEject);
//    CreateTestEjectWidget();

    // 系统状态页面
    pageStatusInfo = new StatusInfoPage(this);
    stackedWidget->addWidget(pageStatusInfo);

    // 方案选择页面
    pageSelectScheme = new QWidget(this);
    stackedWidget->addWidget(pageSelectScheme);
    CreateSelectSchemeWidget();

    // 方案管理页面
//    pageManageScheme = new ManageSchemePage();
//    stackedWidget->addWidget(pageManageScheme);

    // 新建方案页面
    pageManageNewScheme = new ManageNewScheme();
    stackedWidget->addWidget(pageManageNewScheme);

    // 权限设置页面
    pagePurview = new QWidget(this);
    stackedWidget->addWidget(pagePurview);
    CreateSetPurviewWidget();

    // 厂家设置页面
//        pageFactory = new factorySet();
//        stackedWidget->addWidget(pageFactory);

    // 方案参数页面
//        pageSchemeParams = new MySchemeParams();
//        stackedWidget->addWidget(pageSchemeParams);

    // 智能分析页面
#if AI_MODE == 1
    pageAI = new AIAnalysis();
#else
    pageAI = new AIMainWidget();
#endif
    stackedWidget->addWidget(pageAI);

    // 工程模式页面
    pageEngineer = new manageEngineer();
    stackedWidget->addWidget(pageEngineer);

    // 灵敏度页面
//    if(struCnfe.nProduct == OPT_PRODUCT_ANCOO_RS) {
//        pageSensRS = new setmaterialSensRS();
//        stackedWidget->addWidget(pageSensRS);
//    } else {
//        pageSens = new setMaterialSens();
//        stackedWidget->addWidget(pageSens);
//    }

    // 一键自动
    pageAutoSet = new autoSetPage(this);
    stackedWidget->addWidget(pageAutoSet);

    pageCopyMajorParams = new CopyMajorParams(this);
    stackedWidget->addWidget(pageCopyMajorParams);

    /* 灵敏度偏置页面 */
    PageSensBias = new SensBias(this);
    stackedWidget->addWidget(PageSensBias);
    connect(PageSensBias, SIGNAL(goToSetSensPageSig()), this, SLOT(onGoToSetSensPageSlot()));

    //! 电机控制页面
    pageMotorCtrl = new QWidget(this);
    stackedWidget->addWidget(pageMotorCtrl);
    CreateMotorCtrlWidget();

    //! 信号校正页面
    pageAutoSigWid = new autoSigWidget(this);
    stackedWidget->addWidget(pageAutoSigWid);

    //! 视频图像页面
    pageVideo = new MyVideo(this);
    stackedWidget->addWidget(pageVideo);

    //! 实时产量页面
    pageThroughput = new Throughput(this);
    stackedWidget->addWidget(pageThroughput);

    aiModelSetWidget = new AiModelSet(this);
    stackedWidget->addWidget(aiModelSetWidget);

    //! 无监督学习
    pageUnsupervisedAIWidget = new UnsupervisedAIMainWidget(this);
    stackedWidget->addWidget(pageUnsupervisedAIWidget);

    //! 一致性
    pageConsistenceWidget = new setSorterConsistenceWidget(this);
    stackedWidget->addWidget(pageConsistenceWidget);

    // 页面布局
    mainGridLayout = new QGridLayout(this);
    mainGridLayout->setContentsMargins(5, 10, 10, 10);
    mainGridLayout->setHorizontalSpacing(20);
    mainGridLayout->setRowMinimumHeight(0, LCD_TITLE_HEIGHT - 10);
    //    mainGridLayout->setRowMinimumHeight(4 ,LCD_TITLE_HEIGHT-10);

        /* 尺寸限制 */
    titleLabel->setMaximumHeight(BTN_HEIGHT);
    titleLabel->setMaximumWidth(BTN_WIDTH);
    //    backToHomeBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    feedSwitchBtn->setFixedSize(BTN_WIDTH + 40, BTN_HEIGHT + 10);
    systemHaltBtn->setFixedSize(BTN_WIDTH + 30, BTN_HEIGHT + 10);
    saveParamBtn->setFixedSize(BTN_WIDTH + 30, BTN_HEIGHT + 10);
    videoBtn->setFixedSize(BTN_WIDTH + 30, BTN_HEIGHT + 10);

    //    setPurviewBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    statInfoBtn->setFixedSize(BTN_WIDTH + 20, BTN_HEIGHT + 10);
    //    m_oneKeyFeedBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    //    machineInfoBtn->setFixedSize(BTN_WIDTH+15,BTN_HEIGHT);


    mainGridLayout->setVerticalSpacing(0);

    //    leftBackGbx  = new myGroupBox(QString(""), this);
    //    leftBackGbx->setStyleSheet("QGroupBox {background-image: url(:/res/png/leftBack.png);}");
    //    leftBackGbx->setStyleSheet("QGroupBox {border: 0;}");

    //    QHBoxLayout *upHboxLayout = new QHBoxLayout(leftBackGbx);
      //    upHboxLayout->addWidget(titleLabel);
      //    upHboxLayout->addWidget(machineInfoBtn);
      //    upHboxLayout->addSpacing(50);
      //    upHboxLayout->addWidget(feedSwitchBtn);



    //      leftBackGbx->setFixedSize(490,50);
    //      upHboxLayout->setContentsMargins(20, 2, 40, 2);
    //      upHboxLayout->addWidget(oprModeBtn);
    //      upHboxLayout->addStretch(1);
    //      upHboxLayout->addWidget(mainOprBtn);
    //      upHboxLayout->addStretch(1);
    //      upHboxLayout->addWidget(videoBtn);

    //    oprModeBtn->setHidden(true);
    //    mainOprBtn->setHidden(true);
    //    videoBtn->setHidden(true);

    //    downHboxLayout->addStretch(1);
    //    downHboxLayout->addWidget(saveParamBtn);
    //    downHboxLayout->addStretch(1);
    //    downHboxLayout->addWidget(feedSwitchBtn);
    ////    downHboxLayout->addStretch(1);
    ////    downHboxLayout->addWidget(videoBtn);
    //    downHboxLayout->addStretch(1);
    //    downHboxLayout->addWidget(statInfoBtn,1,Qt::AlignRight);

    mainGridLayout->addWidget(titleLabel, 0, 0, 1, 1);

    mainGridLayout->addWidget(videoBtn, 0, 1, 1, 1);

    //      mainGridLayout->addWidget(leftBackGbx, 0, 0, 2, 3, Qt::AlignLeft);
    //      mainGridLayout->setSpacing(10);
    mainGridLayout->addWidget(systemHaltBtn, 0, 2, 1, 1, Qt::AlignRight);
    //      mainGridLayout->setSpacing(2);
    mainGridLayout->addWidget(saveParamBtn, 0, 3, 1, 1, Qt::AlignRight);
    //      mainGridLayout->setSpacing(2);
    mainGridLayout->addWidget(feedSwitchBtn, 0, 4, 1, 1, Qt::AlignRight);
    //      mainGridLayout->setSpacing(2);
    mainGridLayout->addWidget(statInfoBtn, 0, 6, 1, 1, Qt::AlignLeft);

    mainGridLayout->addWidget(lineTitle, 1, 0, 1, 7);
    //    mainGridLayout->addWidget(pageHome, 2, 0, 1, 2);
    mainGridLayout->addWidget(stackedWidget, 2, 0, 7, 7, Qt::AlignBottom);
    //    mainGridLayout->addWidget(lineStatus, 3, 0, 1, 2, Qt::AlignBottom);
    //    mainGridLayout->addLayout(downHboxLayout, 4, 0, 1, 2, Qt::AlignBottom);

      // 警示框
    infoWidget = new myInfoWidget(this);
    infoWidget->raise();
    infoWidget->hide();

    // 警示框
    aiInfoWidget = new myAIInfoWidget(this);
    aiInfoWidget->raise();
    aiInfoWidget->hide();

    //信息提示框
    infoList = new myInfoListWidget("", this);
    infoList->raise();
    infoList->hide();

    // 槽函数
    /*开始通信自检（自动运行）*/
    connect(this, SIGNAL(goToComPageSig()), pageComm, SLOT(startComSlt()));
    /*自动进入通信列表页面*/
    connect(pageComm, SIGNAL(goToCommListPageSig()), this, SLOT(goToCommListPageSlt()));
    /*更新通信列表*/
    connect(pageComm, SIGNAL(updateCommunicationSig()), pageCommList, SLOT(updateCommunicationResultSlt()));
    /*自动进入系统初始化页面*/
    connect(pageComm, SIGNAL(goToInitPageSig()), this, SLOT(goToInitPageSlt()));
    /*重新自检*/
    connect(pageCommList, SIGNAL(goToCommPageSig()), this, SLOT(goToCommPageSlt()));

    /*进入系统初始化页面（发送参数）*/
    connect(pageCommList, SIGNAL(goToInitPageSig()), this, SLOT(goToInitPageSlt()));
    /*自动发送参数*/
    connect(this, SIGNAL(goToInitPageSig()), pageInit, SLOT(startInitSlt()));
    /*初始化完成后，进入预热界面*/
    connect(pageInit, SIGNAL(goToPreheatPageSig()), this, SLOT(GoToHomePageSlt()));
    /*自动预热开始*/
    connect(this, SIGNAL(goToPreheatPageSig()), pagePreheat, SLOT(startPreheatSlt()));
    /*进入主界面*/
    connect(pagePreheat, SIGNAL(goToHomePageSig()), this, SLOT(GoToHomePageSlt()));

    /*进入机器和工程师信息显示界面*/
//    connect(machineInfoBtn, SIGNAL(pressed()), this, SLOT(state_machine_engineer_info()));
    /*供料开关*/
    connect(feedSwitchBtn, SIGNAL(clicked()), this, SLOT(onFeedSwitchBtnClickedSlt()));
    /*皮带开关*/
//    connect(beltSwitchBtn, SIGNAL(clicked()), this, SLOT(onBeltSwitchBtnClickedSlt()));

    /* 一键放料 */
//    connect(m_oneKeyFeedBtn, SIGNAL(pressed()), this, SLOT(onOneKeyFeedBtnPressed()));

    /*关闭系统*/
    connect(systemHaltBtn, SIGNAL(pressed()), this, SLOT(onSystemHaltBtnClickedSlt()));
    /*保存参数*/
    connect(saveParamBtn, SIGNAL(pressed()), this, SLOT(onSaveParamBtnClickedSlt()));
    /*权限设置*/
//    connect(setPurviewBtn        ,SIGNAL(pressed())             , this, SLOT(onSetPurviewBtnClickedSlt()));
//    /*视频采集*/
    connect(videoBtn, SIGNAL(pressed()), this, SLOT(onVideoBtnClickedSlt()));
    /*系统信息*/
    connect(statInfoBtn, SIGNAL(pressed()), this, SLOT(onStatInfoBtnClickedSlt()));
    connect(updateStatus, SIGNAL(sWipeStart(int)), this, SLOT(onStartWipeSlt(int)));
    connect(updateStatus, SIGNAL(sUpdateSystemTime()), this, SLOT(TimeOutSlt()));
    /*方案管理页面返回主界面*/
    connect(pageManageScheme, SIGNAL(goToHomePageSig()), this, SLOT(GoToHomePageSlt()));
    /*工程模式页面返回主界面*/
    connect(pageEngineer, SIGNAL(goToHomePageSig()), this, SLOT(GoToHomePageSlt()));

    /*料位监控界面一键放料按钮状态*/
    connect(pageSens, SIGNAL(goToSensBiasPageSig()), this, SLOT(onGoToSensBiasPageSlt()));
    connect(pageSens, SIGNAL(goToHomePageSig()), this, SLOT(GoToHomePageSlt()));

    /*工程设置页面返回主界面*/
    connect(pageFactory, SIGNAL(goToHomePage()), this, SLOT(GoToHomePageSlt()));
    connect(pageFactory, SIGNAL(hideTitleBtns()), this, SLOT(onHideTitleBtnsSlt()));
    connect(this, SIGNAL(factoryToFirstSig()), pageFactory, SLOT(gotoFirstPageSlt()));
    //        connect(this                 ,SIGNAL(factoryUpdateSig())   ,  pageFactory     , SLOT(factoryUpdateSlt()));

        /*方案参数页面返回主界面*/
    connect(pageSchemeParams, SIGNAL(goToHomePage()), this, SLOT(GoToHomePageSlt()));
    connect(this, SIGNAL(schParToFirstSig()), pageSchemeParams, SLOT(refreshSchemeParams()));
    connect(this, SIGNAL(shutdownCurveBg()), pageSchemeParams, SLOT(shutdownCurveBackGround()));

    /*智能算法页面返回主界面*/
    connect(pageAI, SIGNAL(goToHomePage()), this, SLOT(GoToHomePageSlt()));
    connect(this, SIGNAL(AIToFirsteSig()), pageAI, SLOT(gotoFirstPageSlt()));
    connect(pageSchemeParams, SIGNAL(goToAIMainWidgetSig(int)), this, SLOT(onAIComputeBtnClickedSlt(int)));
    connect(pageAI, SIGNAL(goToIdentifyPagesig()), this, SLOT(onSchemeParamBtnClickedSlt1()));

    connect(pageManageNewScheme, SIGNAL(goToManageSchemeSig()), this, SLOT(onManageSchemeBtnClickedSlt1()));
    connect(pageManageNewScheme, SIGNAL(goToManageSchemeSig()), pageManageScheme, SLOT(updateListWidget()));
    // 视频图像采集返回主界面
    connect(pageVideo, SIGNAL(backToHomePageSig()), this, SLOT(GoToHomePageSlt()));

    // ai模型返回主页面
    connect(aiModelSetWidget, SIGNAL(backToHomePageSig()), this, SLOT(GoToHomePageSlt()));

    // 实时产量页面返回主页面
    connect(pageThroughput, SIGNAL(backToHomePageSig()), this, SLOT(GoToHomePageSlt()));

    /* 进入新建方案页面 */
    connect(pageManageScheme, SIGNAL(goToNewSchemePageSig()), this, SLOT(GoToManageNewSchemePageSlt()));

    /* 系统状态页面返回主界面 */
    connect(pageStatusInfo, SIGNAL(goToHomePageSig()), this, SLOT(GoToHomePageSlt()));
    //    connect(backToHomeBtn,        SIGNAL(pressed())             , this, SLOT(onBackToHomeBtnClicked()));
    connect(this, SIGNAL(statusToFirstSig()), pageStatusInfo, SLOT(gotoFirstPageSlt()));
    connect(this, SIGNAL(sManageEngineerSig()), pageEngineer, SLOT(gotoFirstPageSlt()));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetIndexChangedSlt(int)));
    connect(&myFlow, SIGNAL(backlightOnSig()), this, SLOT(onBackgroundLightOnSlt()));
    connect(getStatus, SIGNAL(closeFeederSig(int)), this, SLOT(onCloseFeederSlt(int)));
    connect(getStatus, SIGNAL(openFeederSig(int)), this, SLOT(onOpenFeederSlt(int)));
    connect(getStatus, SIGNAL(showPressureAlarmMsgSig(bool)), this, SLOT(showPressureAlarmMsgSlt(bool)));
    connect(pageAutoSigWid, SIGNAL(goToHomePage()), this, SLOT(GoToHomePageSlt()));
    connect(pageUnsupervisedAIWidget, SIGNAL(goToHomePage()), this, SLOT(GoToHomePageSlt()));
    connect(pageConsistenceWidget, SIGNAL(goToSetSensPageSig()), this, SLOT(onGoToSetSensPageSlot()));
    connect(myDevMonitor, SIGNAL(closeFeederSig(int)), this, SLOT(onCloseFeederSlt(int)));
    connect(myDevMonitor, SIGNAL(openFeederSig(int)), this, SLOT(onOpenFeederSlt(int)));
    connect(myDevMonitor, SIGNAL(startWipeSig(int)), this, SLOT(onStartWipeSlt(int)));
    connect(this, SIGNAL(openFeederSig(int)), this, SLOT(onOpenFeederSlt(int)));
    connect(this, SIGNAL(closeFeederSig(int)), this, SLOT(onCloseFeederSlt(int)));

    /** RSC及RDC机型供料器分次或独立 */
    if (struCnfe.nEnableCGMachine == 1)
    {
        connect(pageSetFeedRS, SIGNAL(goToHomePageSig()), this, SLOT(GoToHomePageSlt()));
    }

    titleLabel->setText("");

    QTimer::singleShot(100, pageComm, SLOT(startComSlt()));

    QPalette palette;
    QPixmap pixmap(":/res/png/back.png");
    pixmap = pixmap.scaled(this->size());

    palette.setBrush(backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
}

MainWidget::~MainWidget()
{
}

#if ENABLE_CAPTURE_SCREEN == 1
/* 屏幕截图 */
void MainWidget::keyPressEvent(QKeyEvent* event)
{
    QPixmap originalPixmap;

    if (event->key() == Qt::Key_F1)
    {
        originalPixmap = QPixmap::grabWindow(this->winId(), 0, 0, LCD_WIDTH, LCD_HEIGHT);
        QString format = "png";

        QString fileName;
        fileName = QString("%1.png").arg(struGsh.nCounter);

        if (!fileName.isEmpty())
        {
            originalPixmap.save(fileName, format.toLatin1());
        }
    }
}
#endif

/* 点亮背光，需要输入密码 */
void MainWidget::onBackgroundLightOnSlt()
{
    int ret;
    myInputPanel inputDlg(passwdType, 0, 0, 0);

UPDATE:
    ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        QString str = inputDlg.getText();

        if (str == QString().fromLocal8Bit(struCnfg.sBacklightPassword))
        {
            return;
        }
        else
        {
            infoWidget->setLabelText(myLan.msg_password_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
            goto UPDATE;
        }
    }
    else
    {
        goto UPDATE;
    }
}

/* 返回主界面 */
void MainWidget::onBackToHomeBtnClicked()
{
    myFlow.updateOnoff(1);  //退出主界面时发送校准开始命令

    switch (stackedWidget->currentIndex())
    {
        //    case SM_SET_FACTORY:
        //        emit factoryToFirstSig();
        //        break;
        //    case SM_SCHEME_PARAM_PAGE:
        //        emit schParToFirstSig();
        //        break;
    case SM_AI_PAGE:
    case SM_UNSUPERVISED_AI:
        emit AIToFirsteSig();
        break;
    case SM_ENGINEER_PAGE:
        emit sManageEngineerSig();
        break;
    default:
        break;
    }

    startBtn->setEnabled(true);
    stopBtn->setEnabled(false);
    layerBtn->setEnabled(true);

    if (threadStart->isRunning())
    {
        threadStart->stop();
        threadStart->wait();
    }

    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

/**
 * 转到通信检测页面
 */
void MainWidget::goToCommPageSlt()
{
    this->stackedWidget->setCurrentIndex(SM_COMM_PAGE);
    myFlow.sleep(2);
    emit goToComPageSig();
}

/**
 * 转到通信检测列表页面
 */
void MainWidget::goToCommListPageSlt()
{
    this->stackedWidget->setCurrentIndex(SM_COMM_LIST_PAGE);
}

/**
 * 转到系统初始化页面
 */
void MainWidget::goToInitPageSlt()
{
    this->stackedWidget->setCurrentIndex(SM_INIT_PAGE);
    myFlow.sleep(2);
    emit goToInitPageSig();
}

/**
 * 转到预热页面
 */
void MainWidget::goToPreheatPageSlt()
{
    this->stackedWidget->setCurrentIndex(SM_PREHEAT_PAGE);
    emit goToPreheatPageSig();
}

/*!
 * \brief MainWidget::onStartWipeSlt
 * 清灰请求响应处理流程
 */
void MainWidget::onStartWipeSlt(int trigger)
{
    //! 主屏模式忽略网络同步清灰控制请求，仅支持手动、定时清灰触发请求
    if (struCnfg.nRunMode == MODE_RUN_MASTER && trigger == TRIGGER_SYNC)
    {
        return;
    }

    //! 从屏模式处理网络同步清灰控制请求，忽略手动、定时清灰触发请求
    if (struCnfg.nRunMode == MODE_RUN_SLAVE && trigger != TRIGGER_SYNC)
    {
        return;
    }

    //! 主屏模式广播网络同步清灰指令
    if (struCnfg.nRunMode == MODE_RUN_MASTER)
    {
        QByteArray args;
        args.append((quint8)struCnfp.struGroupCtrl[ONE_LEVEL].nWipeDelay);
        myDevMonitor->writeDatagram(CMD_SCREEN_WIPE_CONTROL, args, strNetInfo.groupAddress, strNetInfo.selfNetControlPort);
    }

    //! 从屏模式延迟特定时间开始清灰
    if (struCnfg.nRunMode == MODE_RUN_SLAVE && trigger == TRIGGER_SYNC)
    {
        myFlow.sleep(struCnfp.struGroupCtrl[ONE_LEVEL].nWipeDelay);
    }

    //! 开始清灰操作
    infoWidget->setLabelText(myLan.msg_wiping);
    infoWidget->delayShow();
    myFlow.startWipe();
    infoWidget->hide();

    //! 触发大数据上传
    if (struGsh.bStatisticFinished)
    {
        struGsh.bStatisticFinished = !struGsh.bStatisticFinished;
        emit statisticFinished();
    }
}

/*************************************************************************************
*
* 创建主页面及其相关功能
*
*************************************************************************************/
/**
  * Description : 创建T1小茶叶机型定制主页面
  */
void MainWidget::CreateHomeTeaWidget()
{
    int leftMargin = 40;
    int upMargin = 20;

    // 选择方案
    myGroupBox* schemeGroup = new myGroupBox(myLan.scheme_select, pageHomeTea);
    schemeBtn = new myPushButton(myLan.scheme_select, myIcon.Sorter, pageHomeTea);

    // 设置供料
    myGroupBox* feederGroup = new myGroupBox(myLan.feeder_set, pageHomeTea);
    feederValue = new MyValueBar();
    feederValue->setRange(0, 100);

    // 设置灵敏度
    myGroupBox* sensGroup = new myGroupBox(myLan.sensitivity_set, pageHomeTea);
    sensLabel = new myLabel(myLan.sensitivity, pageHomeTea);
    sensValue = new myLineEdit("0", pageHomeTea);

    // 设置纯度
    percentLabel = new myLabel(myLan.bad_number, pageHomeTea);
    percentValue = new myLineEdit("0", pageHomeTea);

    updateHomeTeaPage();

    // 尺寸限制
    schemeBtn->setMaximumHeight(BTN_HEIGHT);
    sensValue->setMinimumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    percentValue->setMinimumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QHBoxLayout* schemeLayout = new QHBoxLayout(schemeGroup);
    schemeLayout->addWidget(schemeBtn);

    QHBoxLayout* feederLayout = new QHBoxLayout(feederGroup);
    feederLayout->addWidget(feederValue);

    QFormLayout* sensLayout = new QFormLayout;
    sensLayout->addRow(sensLabel, sensValue);
    QFormLayout* percentLayout = new QFormLayout;
    percentLayout->addRow(percentLabel, percentValue);

    QHBoxLayout* valueLayout = new QHBoxLayout(sensGroup);
    valueLayout->addLayout(sensLayout);
    valueLayout->addLayout(percentLayout);
    valueLayout->setContentsMargins(11, 20, 11, 0);

    QVBoxLayout* mainLayout = new QVBoxLayout(pageHomeTea);
    mainLayout->setContentsMargins(leftMargin, upMargin, leftMargin, upMargin);
    mainLayout->addWidget(schemeGroup);
    mainLayout->addWidget(feederGroup);
    mainLayout->addWidget(sensGroup);

    // 槽函数
    connect(schemeBtn, SIGNAL(pressed()), this, SLOT(onManageSchemeBtnClickedSlt()));
    connect(feederValue, SIGNAL(valueChanged(int)), this, SLOT(setFeederValueSlt()));
    connect(sensValue, SIGNAL(pressed()), this, SLOT(setSensValueSlt()));
    connect(percentValue, SIGNAL(pressed()), this, SLOT(setPercentValueSlt()));
}

int MainWidget::getPrefixedProfileIndex(QString strProfile)
{
    QString strPrefix;

    for (int i = 0; i < struCnfe.nParentPrefixedProfileTotal; i++)
    {
        if (struCnfe.struParentPrefixedProfile[i].nIsAble == 0)
            continue;
        for (int j = 0; j < struCnfe.struParentPrefixedProfile[i].nSonProfileCount; j++)
        {
            if (struCnfe.struParentPrefixedProfile[i].struSonProfile[j].nIsAble == 0)
                continue;

            strPrefix.sprintf("%s", struCnfe.struParentPrefixedProfile[i].struSonProfile[j].sMaterialName);
            if (strPrefix == strProfile)
                return j;
        }
    }

    return -1;
}

void MainWidget::updateHomeTeaPage()
{
    QString text;
    int sens;

    // 方案名称
    text.sprintf("%s", struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName);
    schemeBtn->setText(text);

    // 供料量
    feederValue->setValue(struCnfp.struGroupCtrl[0].nFeeder[0]);

    for (int i = 0; i < struCnfe.nArithmeticTotal; i++)
    {
        if (struCnfp.nArithmeticEnableLevel[struGsh.nLevel][i] == 0)
            continue;

        switch (i)
        {
        case ARITH_GREY_A:
        case ARITH_GREY_B:
            if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nMode == 0)
            {
                sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMin * 0.1;
                sensLabel->setText(myLan.grain_sensitivity);
                percentLabel->setText(myLan.grain_size);
            }
            else
            {
                sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMax * 0.1;
                sensLabel->setText(myLan.slice_sensitivity);
                percentLabel->setText(myLan.slice_size);
            }
            text.sprintf("%d", sens);
            sensValue->setText(text);

            text.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nPercent);
            percentValue->setText(text);
            sensLabel->show();
            sensValue->show();
            percentValue->show();
            percentValue->show();
            break;
        case ARITH_DISCOLOR_A:
        case ARITH_DISCOLOR_B:
            if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nMode == 0)
            {
                sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMin * 0.01;
                sensLabel->setText(myLan.grain_sensitivity);
                percentLabel->setText(myLan.grain_size);
            }
            else
            {
                sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMax * 0.01;
                sensLabel->setText(myLan.slice_sensitivity);
                percentLabel->setText(myLan.slice_size);
            }
            text.sprintf("%d", sens);
            sensValue->setText(text);

            text.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nPercent);
            percentValue->setText(text);
            sensLabel->show();
            sensValue->show();
            percentValue->show();
            percentValue->show();
            break;
        case ARITH_CROSS:
            break;
        case ARITH_SHAPE:
            if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struShape.nMode == 0)
                sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struShape.nAreaMax * 0.1;
            else
                sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struShape.nAreaMin * 0.1;
            text.sprintf("%d", sens);
            sensLabel->setText(myLan.sensitivity);
            sensValue->setText(text);

            sensLabel->show();
            sensValue->show();
            percentLabel->hide();
            percentValue->hide();
            break;
        case ARITH_SCALE:
            sens = struCnfp.struGroupIdentify[struGsh.nLevel][0].struScale[0].nValue;
            text.sprintf("%d", sens);
            sensLabel->setText(myLan.sensitivity);
            sensValue->setText(text);

            sensLabel->show();
            sensValue->show();
            percentLabel->hide();
            percentValue->hide();
            break;
        case ARITH_BUD_1:
        case ARITH_BUD_2:
        case ARITH_PEANUT_A:
        case ARITH_PEANUT_B:
        case ARITH_PEANUT_C:
            break;
        case ARITH_INTEL_A:
        case ARITH_INTEL_B:
        case ARITH_INTEL_C:
        case ARITH_INTEL_D:
            text.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][0].struIntel[i - ARITH_INTEL_A].nPercent);
            percentLabel->setText(myLan.pole_size);
            percentValue->setText(text);

            sensLabel->hide();
            sensValue->hide();
            percentLabel->show();
            percentValue->show();
            break;
        }
    }
}

/**
  * Description : 设置交流振动器供料量
  */
void MainWidget::setFeederValueSlt()
{
    int feeder = feederValue->getValue();

    struCnfp.struGroupCtrl[0].nFeeder[0] = feeder;
    myFlow.resetFeeder(0);
    LOG_INFO_STM("Feeder 0:" << struCnfp.struGroupCtrl[0].nFeeder[0]);
}

/**
  * Description : 设置灵敏度
  */
void MainWidget::setSensValueSlt()
{
    int sens;
    sens = sensValue->text().toDouble();

    myInputPanel inputDlg(floatType, 0, 100, sens);
    if (inputDlg.exec() == QDialog::Accepted)
    {
        sens = inputDlg.getValue();
        for (int i = 0; i < struCnfe.nArithmeticTotal; i++)
        {
            if (struCnfp.nArithmeticEnableLevel[struGsh.nLevel][i] == 0)
            {
                continue;
            }

            switch (i)
            {
            case ARITH_GREY_A:
            case ARITH_GREY_B:
                if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nMode == 0)
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMin = sens * 10 + 0.5;
                else
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMax = sens * 10 + 0.5;
                break;
            case ARITH_DISCOLOR_A:
            case ARITH_DISCOLOR_B:
                if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nMode == 0)
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMin = sens * 100 + 0.5;
                else
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nSensMax = sens * 100 + 0.5;
                break;
            case ARITH_CROSS:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struCross.nSens = sens * 10 + 0.5;
                break;
            case ARITH_SHAPE:
                if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struShape.nMode == 0)
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struShape.nAreaMax = sens * 10 + 0.5;
                else
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struShape.nAreaMin = sens * 10 + 0.5;
                break;
            case ARITH_SCALE:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struScale[0].nValue = sens;
                break;
            case ARITH_BUD_1:
            case ARITH_BUD_2:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struBud[i - ARITH_BUD_1].nArea = sens;
                break;
            case ARITH_PEANUT_A:
            case ARITH_PEANUT_B:
            case ARITH_PEANUT_C:
                if (struCnfp.struGroupIdentify[struGsh.nLevel][0].struPeanutAbcd[i - ARITH_PEANUT_A].nMode == 0)
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struPeanutAbcd[i - ARITH_PEANUT_A].nSensMin = sens * 10 + 0.5;
                else
                    struCnfp.struGroupIdentify[struGsh.nLevel][0].struPeanutAbcd[i - ARITH_PEANUT_A].nSensMax = sens * 10 + 0.5;
                break;
            case ARITH_INTEL_A:
            case ARITH_INTEL_B:
            case ARITH_INTEL_C:
            case ARITH_INTEL_D:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struIntel[i - ARITH_INTEL_A].nSens = sens;
                break;
            }

            // 下发参数
            myFlow.materialCopyAssemble(struGsh.nLevel, 0, 0, i, 0);
            myFlow.materialResetGroupAssemble(struGsh.nLevel, 0, 0, i, 0);
        }
        sensValue->setText(QString("%1").arg(sens));
    }
}

/**
  * Description : 设置坏点数
  */
void MainWidget::setPercentValueSlt()
{
    int per;

    per = percentValue->text().toDouble();

    myInputPanel inputDlg(intType, 0, 128, per);

    if (inputDlg.exec() == QDialog::Accepted)
    {
        per = inputDlg.getValue();
        for (int i = 0; i < struCnfe.nArithmeticTotal; i++)
        {
            if (struCnfp.nArithmeticEnableLevel[struGsh.nLevel][i] == 0)
            {
                continue;
            }

            switch (i)
            {
            case ARITH_GREY_A:
            case ARITH_GREY_B:
            case ARITH_DISCOLOR_A:
            case ARITH_DISCOLOR_B:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struGreyColor[i - ARITH_GREY_A].nPercent = per;
                break;
            case ARITH_CROSS:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struCross.nPercent = per;
                break;
            case ARITH_SHAPE:
            case ARITH_SCALE:
                break;
            case ARITH_BUD_1:
            case ARITH_BUD_2:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struBud[i - ARITH_BUD_1].nPercent = per;
                break;
            case ARITH_PEANUT_A:
            case ARITH_PEANUT_B:
            case ARITH_PEANUT_C:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struPeanutAbcd[i - ARITH_PEANUT_A].nPercent = per;
                break;
            case ARITH_INTEL_A:
            case ARITH_INTEL_B:
            case ARITH_INTEL_C:
            case ARITH_INTEL_D:
                struCnfp.struGroupIdentify[struGsh.nLevel][0].struIntel[i - ARITH_INTEL_A].nPercent = per;
                break;
            }

            // 下发参数
            myFlow.materialCopyAssemble(struGsh.nLevel, 0, 0, i, 0);
            myFlow.materialResetGroupAssemble(struGsh.nLevel, 0, 0, i, 0);
        }
        percentValue->setText(QString("%1").arg(per));
    }
}

/**
  * Description : 创建通用机型主页面
  */
void MainWidget::CreateHomeWidget()
{
    int leftMargin = 5;
    int upMargin = 5;

    // 状态列表
//    operateListWidget = new myListWidget();
//    operateListWidget->setViewMode(QListView::IconMode);
//    operateListWidget->setIconSize(QSize(ICON_WID, ICON_HEI));
//    operateListWidget->setMovement(QListView::Static);
//    operateListWidget->setFixedWidth(BTN_WIDTH);

    QString strStyle = QString("QPushButton {border-image: url(:/res/png/backSelect.png);text-align:center;}"
        "QToolTip {border-image: url(:/res/png/backSelect.png);}"
        //                               "QPushButton:hover {border-image: url(:/res/png/selectPress.png);}"
        "QPushButton:pressed {border-image: url(:/res/png/selectPress.png);}");


    //    QString strStyle  = QString("QPushButton {border-image: url(:/res/png/backSelect.png);}"
    //                                "QToolTip {border-image: url(:/res/png/backSelect.png);}");
    for (int i = 0; i < 7; i++)
    {
        operateButton[i] = new myPushButton(QString(""), QIcon(), pageHome);
        operateButton[i]->setFixedSize(BTN_WIDTH - 15, BTN_HEIGHT + 10);
        operateButton[i]->setStyleSheet(strStyle);
        //        operateButton[i]->setIconSize(QSize(ICON_WID, ICON_HEI+30));


        //        operateListWidget->addItem(operateButton[i]);
    }

    //    operateButton[0]->setText(QString("方案选择"));
    operateButton[0]->setText(QString(myLan.schemeNew_select));
    //    operateButton[1]->setText(QString("感度调节"));
    operateButton[1]->setText(QString(myLan.set_sens));
    operateButton[2]->setText(QString(myLan.feeder_speed));
    operateButton[3]->setText(QString(myLan.test_ejector));
    operateButton[4]->setText(QString(myLan.set_wiper));
    //    operateButton[4]->setText(QString("清扫"));
    operateButton[5]->setText(QString(myLan.set_function));
    //    operateButton[6]->setText(QString("系统参数"));
    operateButton[6]->setText(QString(myLan.operation_level));

    operateButton[5]->setEnabled(false);


    //    operateButton[0]->setIcon(QIcon(":/res/png/materialSelect.png"));
    //    operateButton[1]->setIcon(QIcon(":/res/png/sensSelect.png"));
    //    operateButton[2]->setIcon(QIcon(":/res/png/feederSelect.png"));
    //    operateButton[3]->setIcon(QIcon(":/res/png/ejectorSelect.png"));
    //    operateButton[4]->setIcon(QIcon(":/res/png/wiperSelect.png"));
    //    operateButton[5]->setIcon(QIcon(":/res/png/funcSelect.png"));
    //    operateButton[6]->setIcon(QIcon(":/res/png/operSelect.png"));


    if (struCnfg.nLang == LANG_TURKEY)
    {
        for (int i = 0; i < 7; i++)
        {
            operateButton[i]->setButtonFont(config->getFont(FONT_SIZE_16));
        }
    }


    homePageStackedWidget = new QStackedWidget(pageHome);
    homePageStackedWidget->setMaximumWidth(LCD_WIDTH - 10);
    homePageStackedWidget->setFixedHeight(LCD_HEIGHT - 110);
    homePageStackedWidget->setContentsMargins(leftMargin, 0, leftMargin, 0);
    // 样本选择页面
    pageManageScheme = new ManageSchemePage(pageHome);
    homePageStackedWidget->addWidget(pageManageScheme);

    // 设置灵敏度页面
    pageSens = new setMaterialSens(pageHome);
    homePageStackedWidget->addWidget(pageSens);

    // 设置供料速度页面
    pageSetFeedRS = new setFeedPage(pageHome);
    homePageStackedWidget->addWidget(pageSetFeedRS);

    // 设置喷阀检测页面
    pageTestEject = new QWidget();
    homePageStackedWidget->addWidget(pageTestEject);
    CreateTestEjectWidget();

    // 设置清灰页面
    pageSetWipe = new QWidget();
    homePageStackedWidget->addWidget(pageSetWipe);
    CreateSetWipeWidget();

    //算法设置页面
    pageSchemeParams = new MySchemeParams();
    homePageStackedWidget->addWidget(pageSchemeParams);

    //操作级别页面
    pageOprType = new QWidget();
    homePageStackedWidget->addWidget(pageOprType);
    CreatePageOprTypeWidget();

    //高级设置界面
    pageFactory = new factorySet();
    homePageStackedWidget->addWidget(pageFactory);

    pageAiSet = new AisMainWidget();
    homePageStackedWidget->addWidget(pageAiSet);

    // 选择方案
    QString text;
    text.sprintf("%d-%s", struCnfg.nProfile + 1, struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName);
    QString selectSchemeText = QString("%1 : %2").arg(myLan.scheme_select).arg(text);
    selectSchemeBtn = new myPushButton(selectSchemeText, myIcon.Path_Favorite);

    // 一键自动
    autoSetBtn = new myPushButton(myLan.super_brain, myIcon.Action_OneKey);

    // 设置供料
    setFeedBtn = new myPushButton(myLan.feeder_set, myIcon.Sorter_Feeder);

    // 设置灵敏度
    setSensBtn = new myPushButton(myLan.sensitivity_set, myIcon.Action_Parameter);
    // 设置清灰
    setWipeBtn = new myPushButton(myLan.wipe_set, myIcon.Action_Clean);

    // 喷阀自检
    testEjectBtn = new myPushButton(myLan.eject_test, myIcon.Sorter_EjectorTest);

    // 方案参数
    schemeParamBtn = new myPushButton(myLan.scheme_parameter, myIcon.Sorter_SchemeParameter);

    // 管理方案
    manageSchemeBtn = new myPushButton(myLan.scheme_manage, myIcon.Sorter_SchemeManage);

    // 工厂设置
    enteryFactoryBtn = new myPushButton(myLan.factory_set, myIcon.Action_Configure);

    // 相机信号
    cameraSignalBtn = new myPushButton(myLan.camera_signal, myIcon.Chart_Line);

    //    throughputBtn = new myPushButton("实时产量",myIcon.Sorter_FeederTest);

    selectSchemeBtn->setMaximumHeight(90);
    autoSetBtn->setMaximumHeight(90);
    setFeedBtn->setMaximumHeight(90);
    setSensBtn->setMaximumHeight(90);
    setWipeBtn->setMaximumHeight(90);
    testEjectBtn->setMaximumHeight(90);
    cameraSignalBtn->setMaximumHeight(90);
    schemeParamBtn->setMaximumHeight(90);
    enteryFactoryBtn->setMaximumHeight(90);

    //    operatorGbx  = new myGroupBox(QString(""), pageHome);
    //    engineerGbx  = new myGroupBox(myLan.engineer, pageHome);

        // 页面布局
    operatorLayout = new QHBoxLayout();
    operatorLayout->setSpacing(5);
    operatorLayout->setContentsMargins(5, 0, 5, 0);

    operatorLayout->addWidget(operateButton[0]);
    operatorLayout->addWidget(operateButton[1]);
    operatorLayout->addWidget(operateButton[4]);
    operatorLayout->addWidget(operateButton[3]);
    operatorLayout->addWidget(operateButton[2]);
    operatorLayout->addWidget(operateButton[5]);
    operatorLayout->addWidget(operateButton[6]);



    //    engineerLayout = new QGridLayout(engineerGbx);
    //    engineerLayout->setSpacing(10);
    //    engineerLayout->setContentsMargins(11,11,11,11);

    //    homeLayout = new QGridLayout(pageHome);
    //    homeLayout->setSpacing(20);
    //    homeLayout->setContentsMargins(leftMargin, upMargin, leftMargin, upMargin);

    operatePageHBLayout = new QVBoxLayout(pageHome);
    operatePageHBLayout->setContentsMargins(5, 0, 5, 0);


    operatePageHBLayout->addWidget(homePageStackedWidget);
    operatePageHBLayout->addWidget(lineStatus, Qt::AlignBottom);
    operatePageHBLayout->addStretch();
    operatePageHBLayout->addLayout(operatorLayout);

    //    if (struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC) {
    //        operatorLayout->addWidget(selectSchemeBtn, 0, 0, 1, 1);
    //        operatorLayout->addWidget(autoSetBtn, 0, 1, 1, 1);
    //        operatorLayout->addWidget(setFeedBtn, 1, 0, 1, 1);
    //        operatorLayout->addWidget(setSensBtn, 1, 1, 1, 1);
    //        operatorLayout->addWidget(setWipeBtn, 2, 0, 1, 1);
    //        operatorLayout->addWidget(testEjectBtn, 2, 1, 1, 1);

    //        engineerLayout->addWidget(schemeParamBtn, 0, 0, 1, 1);
    //        engineerLayout->addWidget(enteryFactoryBtn, 0, 1, 1, 1);

    //        homeLayout->addWidget(operatorGbx, 0, 0, 4, 1);
    //        homeLayout->addWidget(engineerGbx, 5, 0, 2, 1);
    //    } else if (struCnfe.nProduct == OPT_PRODUCT_ANCOO_RS) {
    //        operatorLayout->addWidget(selectSchemeBtn, 0, 0, 1, 1);
    //        operatorLayout->addWidget(autoSetBtn, 0, 1, 1, 1);
    //        operatorLayout->addWidget(setFeedBtn, 1, 0, 1, 1);
    //        operatorLayout->addWidget(setSensBtn, 1, 1, 1, 1);
    //        operatorLayout->addWidget(setWipeBtn, 2, 0, 1, 1);
    //        operatorLayout->addWidget(testEjectBtn, 2, 1, 1, 1);

    //        engineerLayout->addWidget(cameraSignalBtn, 0, 0, 1, 1);
    //        engineerLayout->addWidget(schemeParamBtn, 0, 1, 1, 1);
    //        engineerLayout->addWidget(enteryFactoryBtn, 1, 1, 1, 1);

    //        homeLayout->addWidget(operatorGbx, 0, 0, 3, 1);
    //        homeLayout->addWidget(engineerGbx, 4, 0, 2, 1);
    //    } else if (struCnfe.nEnableCGMachine) {
    //        operatorLayout->addWidget(selectSchemeBtn, 0, 0, 1, 2);
    //        operatorLayout->addWidget(setFeedBtn, 1, 0, 1, 1);
    //        operatorLayout->addWidget(setSensBtn, 1, 1, 1, 1);
    //        operatorLayout->addWidget(setWipeBtn, 2, 0, 1, 1);
    //        operatorLayout->addWidget(testEjectBtn, 2, 1, 1, 1);

    //        engineerLayout->addWidget(schemeParamBtn, 0, 0, 1, 1);
    //        engineerLayout->addWidget(enteryFactoryBtn, 0, 1, 1, 1);

    //        homeLayout->addWidget(operatorGbx, 0, 0, 4, 1);
    //        homeLayout->addWidget(engineerGbx, 5, 0, 2, 1);
    //    } else {
    //        operatorLayout->addWidget(selectSchemeBtn, 0, 0, 1, 2);
    //        operatorLayout->addWidget(setFeedBtn, 1, 0, 1, 1);
    //        operatorLayout->addWidget(setSensBtn, 1, 1, 1, 1);
    //        operatorLayout->addWidget(setWipeBtn, 2, 0, 1, 1);
    //        operatorLayout->addWidget(testEjectBtn, 2, 1, 1, 1);

    //        engineerLayout->addWidget(schemeParamBtn, 0, 0, 1, 1);
    //        engineerLayout->addWidget(enteryFactoryBtn, 0, 1, 1, 1);

    //        homeLayout->addWidget(operatorGbx, 0, 0, 4, 1);
    //        homeLayout->addWidget(engineerGbx, 5, 0, 2, 1);
    //    }



        /* 槽函数 */
    connect(operateButton[0], SIGNAL(pressed()), this, SLOT(onManageSchemeBtnClickedSlt()));
    connect(operateButton[1], SIGNAL(pressed()), this, SLOT(onSetSensBtnClickedSlt()));
    connect(operateButton[2], SIGNAL(pressed()), this, SLOT(onSetFeedBtnClickedSlt()));

    connect(operateButton[3], SIGNAL(pressed()), this, SLOT(onTestEjectBtnClickedSlt()));
    connect(operateButton[4], SIGNAL(pressed()), this, SLOT(onSetWipeBtnClickedSlt()));
    connect(operateButton[5], SIGNAL(pressed()), this, SLOT(onSchemeParamBtnClickedSlt()));

    connect(operateButton[6], SIGNAL(pressed()), this, SLOT(onOprTypeBtnClickedSlt()));

    connect(homePageStackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onHomePageStackedWidgetIndexChangedSlt(int)));

    connect(pageFactory, SIGNAL(goToHomePage()), this, SLOT(GoToOprTypePageSlt()));
    connect(pageAiSet, SIGNAL(backToHomePageSig()), this, SLOT(GoToOprTypePageSlt()));
    connect(this, SIGNAL(factoryUpdateSig()), pageFactory, SLOT(factoryUpdateSlt()));

    connect(manageSchemeBtn, SIGNAL(pressed()), this, SLOT(onManageSchemeBtnClickedSlt()));
    connect(enteryFactoryBtn, SIGNAL(pressed()), this, SLOT(onEnterFactoryBtnClickedSlt()));
    connect(schemeParamBtn, SIGNAL(pressed()), this, SLOT(onSchemeParamBtnClickedSlt()));
    connect(setWipeBtn, SIGNAL(pressed()), this, SLOT(onSetWipeBtnClickedSlt()));
    connect(autoSetBtn, SIGNAL(pressed()), this, SLOT(onAutoSetBtnClickedSlt()));
    connect(throughputBtn, SIGNAL(clicked()), this, SLOT(onThroughputBtnClickedSlt()));
}

/**
 * Description :  返回操作级别界面
 */
void MainWidget::GoToOprTypePageSlt()
{
    if (struGsh.nAuthenticationLevel != AUTHENTICATION_LEVEL_OPERATOR)
    {
        this->homePageStackedWidget->setCurrentIndex(HOME_OPRLVL_PAGE_INDEX);
    }
    else
    {
        this->homePageStackedWidget->setCurrentIndex(HOME_SCHEME_PAGE_INDEX);
    }
}

/**
 * Description :  操作级别页面
 */
void MainWidget::onOprTypeBtnClickedSlt()
{
    myInputPanel inputDlg(passwdType, 0, 0, 0);
    if (struGsh.nAuthenticationLevel != 0)
    {
        customOprButton->show();
        highSetOprButton->show();
        if (struCnfg.aiEnable == 1)
        {
            aiSetBtn->show();
            aiModelSetBtn->show();
        }
        else
        {
            aiSetBtn->hide();
            aiModelSetBtn->hide();
        }
        homePageStackedWidget->setCurrentIndex(HOME_OPRLVL_PAGE_INDEX);
        return;
    }

    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        QString str = inputDlg.getText();
        //        if (str == "0429" || str == "0421") {
        if (str == "249945")
        {
            struGsh.tempPwd = str;
            operateButton[5]->setEnabled(true);
            customOprButton->show();
            highSetOprButton->show();
            if (struCnfg.aiEnable == 1)
            {
                aiSetBtn->show();
                aiModelSetBtn->show();
            }
            else
            {
                aiSetBtn->hide();
                aiModelSetBtn->hide();
            }
            struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_ENGINEER;
            homePageStackedWidget->setCurrentIndex(HOME_OPRLVL_PAGE_INDEX);
        }
        else
        {
            infoWidget->setLabelText(myLan.msg_password_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
        }
    }

    //    if(struCnfg.aiEnable == 1){
    //        aiSetBtn->show();
    //    }else{
    //        aiSetBtn->hide();
    //    }
}

void MainWidget::CreatePageOprTypeWidget()
{
    customOprButton = new myPushButton(QString(myLan.customer_model), QIcon(), pageOprType);
    highSetOprButton = new myPushButton(QString(myLan.sys_set), QIcon(), pageOprType);
    // Ai设置
    aiSetBtn = new myPushButton("AI" + myLan.set, QIcon(""),
        true, true, this);
    aiSetBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    throughputBtn = new myPushButton("实时产量", QIcon(), pageOprType);
    //    videoBtn = new myPushButton(myLan.sorter_manager, QIcon(),pageOprType);
    aiModelSetBtn = new myPushButton("Ai模型", QIcon(), pageOprType);

    //    videoBtn->hide();
    throughputBtn->hide();

    customOprButton->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    highSetOprButton->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    throughputBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    //    videoBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    aiModelSetBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    customOprButton->hide();
    highSetOprButton->hide();
    if (struCnfg.nLang == LANG_TURKEY)
    {
        customOprButton->setButtonFont(config->getFont(FONT_SIZE_16));
        highSetOprButton->setButtonFont(config->getFont(FONT_SIZE_16));
    }

    QHBoxLayout* operaLay = new QHBoxLayout();
    operaLay->addWidget(customOprButton, Qt::AlignLeft);
    //    operaLay->addSpacing(50);
    operaLay->addWidget(highSetOprButton, Qt::AlignLeft);

    QHBoxLayout* plusLay = new QHBoxLayout();
    plusLay->addWidget(throughputBtn, Qt::AlignLeft);
    plusLay->addWidget(aiModelSetBtn, Qt::AlignLeft);
    plusLay->addWidget(aiSetBtn, Qt::AlignLeft);


    QGridLayout* mainLay = new QGridLayout(pageOprType);
    mainLay->addLayout(operaLay, 0, 0, 1, 3);
    mainLay->addLayout(plusLay, 1, 0, 1, 3);

    connect(customOprButton, SIGNAL(pressed()), this, SLOT(onCustomOprBtnClickedSlt()));
    connect(highSetOprButton, SIGNAL(pressed()), this, SLOT(onHighSetOprBtnClickedSlt()));
    connect(throughputBtn, SIGNAL(pressed()), this, SLOT(onThroughputBtnClickedSlt()));
    //    connect(videoBtn, SIGNAL(pressed()), this, SLOT(onVideoBtnClickedSlt()));
    connect(aiModelSetBtn, SIGNAL(pressed()), this, SLOT(onAiModelSetBtnClickedSlt()));
    connect(aiSetBtn, SIGNAL(pressed()), this, SLOT(onAiSetBtnClickedSlt()));


}

void MainWidget::onHighSetOprBtnClickedSlt()
{
    //    operateButton[5]->setEnabled(false);
    //    customOprButton->hide();
    //    highSetOprButton->hide();
    //    struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_OPERATOR;
    myInputPanel inputDlg(passwdType, 0, 0, 0);

    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        QString str = inputDlg.getText();
        if (struGsh.tempPwd == "249945")
        {
            if (str == "242582")
            {
                struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_ENGINEER;
                homePageStackedWidget->setCurrentIndex(HIGH_SET_OPR_PAGE_INDEX);
            }
            else if (str == "19243589")
            {
                struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_FACTORY;
                homePageStackedWidget->setCurrentIndex(HIGH_SET_OPR_PAGE_INDEX);
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_password_error);
                infoWidget->delayShow();
                myFlow.sleep(3);
                infoWidget->hide();
            }
            //       }else if(struGsh.tempPwd == "0421"){
            //           if (str == "110421") {
            //               struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_ENGINEER;
            //               homePageStackedWidget->setCurrentIndex(HIGH_SET_OPR_PAGE_INDEX);
            //           } else if(str == "354311") {
            //               struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_FACTORY;
            //               homePageStackedWidget->setCurrentIndex(HIGH_SET_OPR_PAGE_INDEX);
            //           }else {
            //               infoWidget->setLabelText(myLan.msg_password_error);
            //               infoWidget->delayShow();
            //               myFlow.sleep(3);
            //               infoWidget->hide();
            //           }
        }
        else
        {
            infoWidget->setLabelText(myLan.msg_password_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
        }

    }


}

void MainWidget::onCustomOprBtnClickedSlt()
{
    struGsh.nAuthenticationLevel = 0;
    operateButton[5]->setEnabled(false);
    customOprButton->hide();
    highSetOprButton->hide();
    aiSetBtn->hide();
    aiModelSetBtn->hide();
    //    oprModeBtn->setText(myLan.pic_customer);

    struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_OPERATOR;

    homePageStackedWidget->setCurrentIndex(HOME_OPRLVL_PAGE_INDEX);
}

/* 响应当前页索引发生变化的情况 */
void MainWidget::onHomePageStackedWidgetIndexChangedSlt(int index)
{
    QString title;

    emit shutdownCurveBg();

    switch (index)
    {
    case HOME_SCHEME_PAGE_INDEX:
        title = QString(myLan.schemeNew_select);
        break;
    case HOME_SENS_PAGE_INDEX:
        title = QString(myLan.set_sens);
        break;
    case HOME_FEED_PAGE_INDEX:
        title = QString(myLan.feeder_speed);
        break;
    case HOME_EJECT_PAGE_INDEX:
        title = QString(myLan.test_ejector);
        break;
    case HOME_WIPE_PAGE_INDEX:
        title = QString(myLan.set_wiper);
        break;
    case HOME_ALGO_PAGE_INDEX:
        title = QString(myLan.set_function);
        break;
    case HOME_OPRLVL_PAGE_INDEX:
        title = QString(myLan.operation_level);
        break;
    case HIGH_SET_OPR_PAGE_INDEX:
        if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_ENGINEER)
        {
            title = QString(myLan.sys_set + "Ⅰ");
        }
        else if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_FACTORY)
        {
            title = QString(myLan.sys_set + "Ⅱ");
        }
        else
        {
            title = QString(myLan.sys_set + "Ⅰ");
        }
        break;
    default:
        title = QString(myLan.schemeNew_select);
        break;
    }
    //    mainOprBtn->setEnabled(true);
    //    mainOprBtn->setText(title);
    //    mainOprBtn->setRedColor(RED);
    //    QString strStyle2 = QString("QPushButton {border-image: url(:/res/png/selectPress.png);}");
    //    mainOprBtn->setStyleSheet(strStyle2);
    //    mainOprBtn->setHidden(true);
    if (struGsh.nAuthenticationLevel == 0)
    {
        operateButton[5]->setEnabled(false);
    }
    else
    {
        operateButton[5]->setEnabled(true);
    }

    switch (index)
    {
    case HIGH_SET_OPR_PAGE_INDEX:
    case HIGH_SET_AI_PAGE_INDEX:
        //        operatorGbx->hide();
        for (int i = 0; i < 7; i++)
        {
            operateButton[i]->hide();
        }
        homePageStackedWidget->setFixedWidth(LCD_WIDTH - 10);
        emit factoryUpdateSig();
        break;
    default:
        homePageStackedWidget->setFixedWidth(LCD_WIDTH - 10);

        //        operatorGbx->setHidden(false);
        for (int i = 0; i < 7; i++)
        {
            operateButton[i]->show();
        }
        break;
    }

    switch (index)
    {
    case HOME_OPRLVL_PAGE_INDEX:
    case HIGH_SET_OPR_PAGE_INDEX:
        //        oprModeBtn->setText(myLan.engineer);
        break;
    default:
        //        oprModeBtn->setText(myLan.pic_customer);
        break;
    }

    switch (index)
    {
    case HIGH_SET_OPR_PAGE_INDEX:
    case HIGH_SET_AI_PAGE_INDEX:
        homePageStackedWidget->setFixedHeight(LCD_HEIGHT - 60);
        for (int i = 0; i < 7; i++)
        {
            operateButton[i]->hide();
        }
        lineStatus->setHidden(true);
        break;
    default:
        homePageStackedWidget->setFixedHeight(LCD_HEIGHT - 110);
        for (int i = 0; i < 7; i++)
        {
            operateButton[i]->show();
        }
        lineStatus->setHidden(false);
        break;
    }

    //    switch (index) {
    //        case HOME_EJECT_PAGE_INDEX:
    //        case HIGH_SET_OPR_PAGE_INDEX:
    //            feedSwitchBtn->setEnabled(false);
    //            break;
    //        default:
    //            feedSwitchBtn->setEnabled(true);
    //            break;
    //    }

    switch (index)
    {
    case HIGH_SET_OPR_PAGE_INDEX:
        videoBtn->setEnabled(false);
        systemHaltBtn->setEnabled(false);
        saveParamBtn->setEnabled(false);
        feedSwitchBtn->setEnabled(false);
        statInfoBtn->setEnabled(false);
        videoBtn->setHidden(true);
        systemHaltBtn->setHidden(true);
        saveParamBtn->setHidden(true);
        feedSwitchBtn->setHidden(true);
        statInfoBtn->setHidden(true);
        lineTitle->setHidden(true);
        break;
    default:
        videoBtn->setEnabled(true);
        systemHaltBtn->setEnabled(true);
        saveParamBtn->setEnabled(true);
        feedSwitchBtn->setEnabled(true);
        statInfoBtn->setEnabled(true);
        videoBtn->setHidden(false);
        systemHaltBtn->setHidden(false);
        saveParamBtn->setHidden(false);
        feedSwitchBtn->setHidden(false);
        statInfoBtn->setHidden(false);
        lineTitle->setHidden(false);
        break;
    }

}

void MainWidget::updateHomePage()
{
    QString text;
    QString selectSchemeText;

    text = QString("%1-%2").arg(struCnfg.nProfile + 1)
        .arg(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName);

    selectSchemeText = QString("%1 : %2").arg(myLan.scheme_select).arg(text);
    selectSchemeBtn->setText(selectSchemeText);
}

/**
 * Description :  选择方案页面
 */
void MainWidget::onSelectSchemeBtnClickedSlt()
{
    QString strScheme;
    listWidget->clear();

    for (int i = 0; i < struCnfg.nProfileTotal; i++)
    {
        QString srcFilename = QString("%1/userdata/cnf/%2%3").arg(APP_PATH).arg(struCnfg.struProfileIndex[i].sProfileName).arg(".bak");
        if (QFile(srcFilename).exists())
        {
            QFileInfo fi(srcFilename);
            QString time = fi.lastModified().toString("yyyy-MM-dd hh:mm");
            QByteArray ctime = time.toLatin1();
            strScheme.sprintf("%d %s (%s%s)", i + 1, struCnfg.struProfileIndex[i].sMaterialName, "bak@", ctime.data());
        }
        else
        {
            strScheme.sprintf("%d %s", i + 1, struCnfg.struProfileIndex[i].sMaterialName);
        }
        myListWidgetItem* schemeListItem = new myListWidgetItem(strScheme, QIcon(),
            QSize(listWidget->width() - 20, ICON_HEI * 2));

        listWidget->addItem(schemeListItem);
    }

    // 默认选中列
    listWidget->setCurrentRow(struCnfg.nProfile);

    this->stackedWidget->setCurrentIndex(SM_SELECT_SCHEME_PAGE);
    updateHomePage();
}

/**
 * Description :  转到供料设置页面
 */
void MainWidget::onSetFeedBtnClickedSlt()
{
    this->homePageStackedWidget->setCurrentIndex(HOME_FEED_PAGE_INDEX);
    if (struCnfe.nEnableCGMachine == 1)
    {
        pageSetFeedRS->updatePage();
    }
    else
    {
        updateFeederPage();
    }
}
/**
 * @brief MainWidget::onSetSensBtnClickedSlt
 * 转到物料灵敏度设置页面
 */
void MainWidget::onSetSensBtnClickedSlt()
{
    /*自更新状态页面*/
    pageSens->updateAll();
    this->homePageStackedWidget->setCurrentIndex(HOME_SENS_PAGE_INDEX);
}

/* 相机信号页面 */
void MainWidget::onCameraSignalBtnClicked()
{
    emit cameraSignalRefreshSig();
    stackedWidget->setCurrentIndex(SM_CAMERA_SIGNAL_PAGE);
}

/**
 * Description :  清灰设置页面
 */
void MainWidget::onSetWipeBtnClickedSlt()
{
    updateWipePage();   //! 更新页面
    this->homePageStackedWidget->setCurrentIndex(HOME_WIPE_PAGE_INDEX);
}

/**
 * Description :  喷阀自检页面
 */
void MainWidget::onTestEjectBtnClickedSlt()
{
    if (struGsh.bStatFeed == 1)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->showSecs();
        return;
    }

    /*更新滑动条状态*/
    updateEjextorSlider();
    updateLayerBtnSlt();

    /*切换到喷阀自检页面*/
    this->homePageStackedWidget->setCurrentIndex(HOME_EJECT_PAGE_INDEX);
}

/**
 * Description :  智能计算页面
 */
void MainWidget::onAIComputeBtnClickedSlt(int nIndex)
{
    //! 若智能算法切换则清空物料组合关系
    pageAI->onBtnMapClicked(nIndex);

    //! 刷新页面显示
    pageAI->refreshDisplaySlt();

    stackedWidget->setCurrentIndex(SM_AI_PAGE);
}

/**
 * Description :  方案管理页面
 */
void MainWidget::onManageSchemeBtnClickedSlt()
{
    if (struGsh.bStatFeed == 1)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->showSecs();
        return;
    }

    pageManageScheme->updateListWidget();
    this->homePageStackedWidget->setCurrentIndex(HOME_SCHEME_PAGE_INDEX);
    emit manageSchemeUpdateListWidgetSig();
}

void MainWidget::onManageSchemeBtnClickedSlt1()
{
    if (struGsh.bStatFeed == 1)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->showSecs();
        return;
    }

    pageManageScheme->updateListWidget();
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
    emit manageSchemeUpdateListWidgetSig();
}

/**
 * Description :  厂家设置页面
 */
void MainWidget::onEnterFactoryBtnClickedSlt()
{
    if (struGsh.bStatFeed == 1)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->showSecs();
        return;
    }

    //	this->stackedWidget->setCurrentIndex(SM_SET_FACTORY);

    emit factoryUpdateSig();
}

/**
 * Description :  方案参数页面
 */
void MainWidget::onSchemeParamBtnClickedSlt()
{
    //    qDebug()<<"test1"<<endl;
    homePageStackedWidget->setCurrentIndex(HOME_ALGO_PAGE_INDEX);

    emit schParToFirstSig();
}

/**
 * Description :  方案参数页面
 */
void MainWidget::onSchemeParamBtnClickedSlt1()
{
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
    //    homePageStackedWidget->setCurrentIndex(HOME_ALGO_PAGE_INDEX);


}

/**
 * Description :  返回主界面
 */
void MainWidget::GoToHomePageSlt()
{
    //    if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_OPERATOR) {
    //        if (struCnfe.nMachine == MACHINE_T1) {
    //            this->stackedWidget->setCurrentIndex(SM_HOME_TEA_PAGE);
    //            updateHomeTeaPage();
    //            return;
    //        }
    //    }
    updateHomePage();
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

/**
  * Description : 验证权限卡
  */
bool verifyUSB(void)
{
#ifdef Q_OS_UNIX
    //    unsigned char usbkey[64] = {};

    ///* verify usb key */
    //    /* prevent re-open */
    //    if (myUsbDev != NULL)
    //        usb.myusbFree();

    //    /* open usb device node */
    //    usb.myusbInit();
    //    if (myUsbDev  == NULL) {
    //        return false;
    //    }

    //    /* read usb buffer */
    //    usb_bulk_read(myUsbDev, USB_EP_IN, (char *)usbkey, 6, 500);
    //    if (usbkey[0] != 0x35 || usbkey[1] != 0xB7 || usbkey[2] != 0xC4
    //            || usbkey[3] != 0x5A || usbkey[4] != 0x99 || usbkey[5] != 0xEF) {
    //        usb.myusbFree();
    //        return false;
    //    }

    //    usb.myusbFree();
    //    return false;
#else
    return (QDateTime::currentDateTime().toTime_t() % 2) ? true : false;
#endif
}

/**
 * Description :  系统状态页面
 */
void MainWidget::onStatInfoBtnClickedSlt()
{
    emit shutdownCurveBg();
    emit schParToFirstSig();
    //    if (verifyUSB()) { // 若插入权限卡，则进入工程设置模式页面，否则跳转到系统状态页面
    //        titleStr = titleLabel->text();
    //        this->stackedWidget->setCurrentIndex(SM_ENGINEER_PAGE);
    //    } else {
    emit statusToFirstSig();        // 让系统状态页面索引为0
    pageStatusInfo->updateAll();

    titleStr = titleLabel->text();
    this->stackedWidget->setCurrentIndex(SM_STATUS_INFO_PAGE);
    //    }
}

/* 系统状态返回 */
void MainWidget::onStatInfoBackBtnClicked()
{
    //    titleLabel->setText(titleStr);
    titleLabel->setText("");

    updateHomePage();
}

/**
 * Description :  关闭系统
 */
void MainWidget::onSystemHaltBtnClickedSlt()
{
    emit shutdownCurveBg();

    myMessageBox msgBox(MSG_QUES, myLan.cfm_shut_down);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        infoWidget->setLabelText(myLan.msg_saving_params);
        infoWidget->delayShow();

        //! 系统关闭前须先关闭供料
        if (struGsh.bStatFeed == 1)
        {
            emit closeFeederSig(TRIGGER_MANUAL);
        }

        bool status1 = myFlow.saveCamera();
        bool status2 = myFlow.saveGlobal();
        bool status3 = myFlow.saveProfile();

        if ((!status1) || (!status2) || (!status3))
        {
            infoWidget->setLabelText(myLan.msg_params_save_error);
            myFlow.sleep(2);
        }

        infoWidget->hide();

        infoWidget->setLabelText(myLan.msg_shutdown);
        infoWidget->delayShow();
        myFlow.sleep(3);

        updateStatus->quit();
        uploaderThread->quit();
        myFlow.sleep(1);

        infoWidget->hide();
        this->close();
        LOG_INFO_STM("Power: App off, App has run:" << struGsh.nCounter << " seconds at this time, total:"
            << struCnfg.nCounter << " seconds.");

#ifndef Q_OS_WIN
        myFlow.sleep(1);
        system("shutdown now");
#endif
    }
}

/**
 * Description :  定时器
 */
void MainWidget::TimeOutSlt()
{
    QDateTime time = QDateTime::currentDateTime();
    statInfoBtn->setText(time.toString("yyyy-MM-dd\n hh:mm:ss"));
    if ((struCnfg.nAlarmEnable[ALARM_ID_PRESSURE] == 1 && ((struGsh.bAlarmPressure & 0x03) != 0))
        || (struCnfg.nAlarmEnable[ALARM_ID_BLOWER] == 1 && struGsh.bAlarmBlower == 1)
        || (struCnfg.nAlarmEnable[ALARM_ID_BELT] == 1 && struGsh.bStatBelt[0] == 1)
        || (struCnfg.nAlarmEnable[ALARM_ID_BELT] == 1 && struGsh.bStatBelt[1] == 1)
        || (struCnfg.nAlarmEnable[ALARM_ID_TEMPERATURE] == 1 && (struGsh.nTemperatureMax >= struCnfg.nTemperatureThreshold))
        || (struGsh.nAlarmLevel != ALARM_LEVEL_NULL)
        || (struGsh.nCommStat == 1)
        || (struCnfg.nAlarmEnable[ALARM_ID_SLAVE_COMM] == 1 && struGsh.bAlarmSlaveComm == 1))
    {
        struGsh.bAlarmStatus = 1;
    }
    else
    {
        struGsh.bAlarmStatus = 0;
    }

    //! 根据报警状态更新短信模块信息
    if (struGsh.bIsMC && struCnfg.nCounter % 10 == 0)
    {
        if (struGsh.bAlarmStatus == 1)
        {
            pageStatusInfo->sendAlarmSms();
        }
        else
        {
            pageStatusInfo->updateAlarmStatus();
        }
    }

    QPalette patlette;
    patlette.setColor(QPalette::ButtonText, Qt::black);

    QPalette patlette2;
    patlette2.setColor(QPalette::ButtonText, Qt::red);


    //! 根据报警状态更新按钮颜色
    if (struGsh.bAlarmStatus == 1)
    {
        if (struCnfg.nCounter % 2 == 0)
        {
            //            statInfoBtn->setRedColor(RED);
            statInfoBtn->setPalette(patlette2);

        }
        else
        {
            //            statInfoBtn->setRedColor(DEF);
            statInfoBtn->setPalette(patlette);

        }
    }
    else
    {
        statInfoBtn->setPalette(patlette);

        //        statInfoBtn->setRedColor(DEF);
    }
}

//显示气压报警提示信息
void MainWidget::showPressureAlarmMsgSlt(bool isShow)
{
    //气压报警提示窗口
    if (isShow)
    {
        QString str;
        QStringList pressureInfoList;

        str = QString("");
        pressureInfoList.append(str);

        str = QString("\t%1").arg(myLan.msg_pressure_alarm);
        pressureInfoList.append(str);

        str = QString("\t\t%1").arg(myLan.msg_pressure_alarm_check_pressure);
        pressureInfoList.append(str);

        str = QString(myLan.warm);

        infoList->setlistInfoText(pressureInfoList, str);
        infoList->show();
    }
    else
    {
        infoList->hide();
    }
}

/**
 * Description :  新建方案页面
 */
void MainWidget::GoToManageNewSchemePageSlt()
{
    // 默认选中列
    listWidget->setCurrentRow(struCnfg.nProfile);
    this->stackedWidget->setCurrentIndex(SM_MANAGE_NEW_SCHEME_PAGE);
}

/* 复制重要参数 */
void MainWidget::goToCopyMajorParamsPageSlt()
{
    listWidget->setCurrentRow(struCnfg.nProfile);
    pageCopyMajorParams->refreshDisplay();
    stackedWidget->setCurrentIndex(SM_COPY_MAJOR_PARAMS_PAGE);
}

/**
 * Description :  权限设置页面
 */
void MainWidget::onSetPurviewBtnClickedSlt()
{
    pageIndex = stackedWidget->currentIndex();
    titleStr = titleLabel->text();
    this->stackedWidget->setCurrentIndex(SM_SET_PURVIEW);

    if (threadStart->isRunning())
    {
        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
        layerBtn->setEnabled(true);
        threadStart->stop();
        threadStart->wait();
    }
}

/* 创建和设置显示联系我们页面 */
void MainWidget::state_machine_engineer_info()
{
    MyContact msgBox(myLan.contact_us);
    msgBox.exec();
}

/*!
 * \brief MainWidget::onFeedSwitchBtnClickedSlt
 * 供料开关按钮响应处理
 */
void MainWidget::onFeedSwitchBtnClickedSlt()
{
    emit shutdownCurveBg();

    //! 从屏模式下提示操作无效
    if (struCnfg.nRunMode == MODE_RUN_SLAVE)
    {
        myMessageBox msgBox(MSG_WARN, myLan.slave);
        if (msgBox.exec() == QDialog::Rejected)
        {
            return;
        }
    }

    //! 供料开关请求处理
    if (struGsh.bStatFeed == MODE_CLOSE)
    {
        //! 豆类定制机型处理
        if (struCnfe.nEnableBeanMachine == 1 && struGsh.isPeaMachineBeanMat == false)
        {
            QString textOff = myLan.feed + " " + myLan.off;
            feedSwitchBtn->setText(myLan.working_stop);
            QString strStyle5 = QString("QPushButton {border-image: url(:/res/png/workStart.png);}");
            feedSwitchBtn->setStyleSheet(strStyle5);
            QPalette patlette;
            patlette.setColor(QPalette::ButtonText, Qt::black);
            feedSwitchBtn->setPalette(patlette);
            //            feedSwitchBtn->setRedColor(DEF);
            return;
        }

        //! 用户确认提示
        myMessageBox msgBox(MSG_QUES, myLan.cfm_turn_on);
        if (msgBox.exec() == QDialog::Accepted)
        {
            emit openFeederSig(TRIGGER_MANUAL);
        }
    }
    else
    {
        emit closeFeederSig(TRIGGER_MANUAL);
    }

    //! 用户点击开关下料时清空报警关闭下料标志
    struGsh.bAlarmCloseFeed = false;
}

/*!
 * \brief MainWidget::onBeltSwitchBtnClickedSlt
 * 皮带开关按钮响应处理
 */
void MainWidget::onBeltSwitchBtnClickedSlt()
{

}

/**
 * Description :  参数保存
 */
void MainWidget::onSaveParamBtnClickedSlt()
{
    emit shutdownCurveBg();

    myMessageBox msgBox(MSG_QUES, myLan.cfm_save_parameter);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        infoWidget->setLabelText(myLan.msg_saving_params);
        infoWidget->delayShow();
        bool status1 = myFlow.saveCamera();
        bool status2 = myFlow.saveGlobal();
        bool status3 = myFlow.saveProfile();

        if ((!status1) || (!status2) || (!status3))
        {
            infoWidget->setLabelText(myLan.msg_params_save_error);
            myFlow.sleep(2);
        }

        infoWidget->hide();
    }
}

/***********************************************************************************
*
* 创建方案选择页面及其相关功能
*
************************************************************************************/
void MainWidget::CreateSelectSchemeWidget()
{
    listWidget = new myListWidget(QSize(ICON_WID, ICON_HEI), pageSelectScheme);

    // btnlist
    okSelectSchemeBtn = new myPushButton(myLan.select, myIcon.Sorter_SchemeSelect);
    cancelSelectSchemeBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    okSelectSchemeBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    cancelSelectSchemeBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    //页面布局
    schemePageGridLayout = new QGridLayout(pageSelectScheme);
    schemePageGridLayout->setVerticalSpacing(10);
    schemePageGridLayout->addWidget(listWidget, 0, 0, 1, 1);

    schemePageHBLayout = new QHBoxLayout();
    schemePageHBLayout->addWidget(okSelectSchemeBtn);
    horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    schemePageHBLayout->addItem(horizontalSpacer);
    schemePageHBLayout->addWidget(cancelSelectSchemeBtn);
    schemePageGridLayout->addLayout(schemePageHBLayout, 1, 0, 1, 1);

    // 槽函数
    connect(okSelectSchemeBtn, SIGNAL(pressed()), this, SLOT(onOkSelectSchemeBtnClickedSlt()));
    connect(cancelSelectSchemeBtn, SIGNAL(pressed()), this, SLOT(onCancelSelectSchemeBtnClickedSlt()));
}

/**
 * 方案选择页面 确定选择
 */
void MainWidget::onOkSelectSchemeBtnClickedSlt()
{
    /* if feeding on, feed off it first */
    if (struGsh.bStatFeed == 1)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->showSecs();
        return;
    }

    myMessageBox msgBox(MSG_QUES, myLan.cfm_select_current_scheme);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        myFlow.saveProfile();

        //! 记录上一方案的启动模式
        int nIntMode = struCnfp.nInterfaceBoardMode;
        int nColorMode = struCnfp.nColorBoardMode;

        struCnfg.nProfile = listWidget->currentRow();
        myFlow.getProfile();

        LOG_INFO_STM("select a profile:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName)
            << " (in file:" << std::string(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName) << ")");

        //! 检查启动模式
        myFlow.checkAndResetFPGAMode(nIntMode, nColorMode);

        myFlow.initSendAllParams();

        pageSens->updateAll();
        infoWidget->hide();

        updateHomePage();
        stackedWidget->setCurrentIndex(SM_HOME_PAGE);
    }
}

/**
 * 方案选择页面 取消选择
 */
void MainWidget::onCancelSelectSchemeBtnClickedSlt()
{
    updateHomePage();
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

/***************************************************************************************
*
* 创建供料设置页面及其相关功能
*
****************************************************************************************/
void MainWidget::CreateSetFeedWidget()
{
    QString strFeed;

    feederSignalMapper = new QSignalMapper(pageSetFeed);

    for (int i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            strFeed = QString("%1 %2").arg(myLan.feeder).arg(i + 1);
            feedLabel[i] = new myLabel(strFeed, pageSetFeed);

            strFeed = QString("%1").arg(struCnfp.struGroupCtrl[0].nFeeder[i]);
            feedLbe[i] = new myLineEdit(strFeed, pageSetFeed);
            feedLbe[i]->setReadOnly(true);
            feedLbe[i]->setMaximumHeight(BTN_HEIGHT - 5);
        }
        else
        {
            strFeed = QString("%1 %2").arg(myLan.feeder).arg(i + 1);
            feedLabel[i] = new myLabel(strFeed, pageSetFeed);

            strFeed.sprintf("%d", struCnfp.struGroupCtrl[0].nFeeder[i]);
            feedLbe[i] = new myLineEdit(strFeed, pageSetFeed);
            feedLbe[i]->setReadOnly(true);
            feedLbe[i]->setMaximumHeight(BTN_HEIGHT - 5);
        }

        /*供料按钮布局*/
        feederHBLayout[i] = new QHBoxLayout();
        feederHBLayout[i]->setContentsMargins(30, 0, 30, 0);
        feederHBLayout[i]->addWidget(feedLabel[i]);
        feederHBLayout[i]->addWidget(feedLbe[i]);

        /* 使用信号栈方式实现各供料设置 */
        feederSignalMapper->setMapping(feedLbe[i], i);
        connect(feedLbe[i], SIGNAL(pressed()), feederSignalMapper, SLOT(map()));
    }

    cancelSetFeedBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    cancelSetFeedBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    feederMonitorBtn = new MyCheckBox(myLan.feeder_control_enable, struCnfg.nFeederCtrlEn, QBoxLayout::LeftToRight);

    /* 整个页面布局 */
    feederPageGridLayout = new QGridLayout(pageSetFeed);
    feederPageGridLayout->setContentsMargins(20, 20, 20, 10);
    feederPageGridLayout->setAlignment(Qt::AlignRight);
    feederPageGridLayout->setHorizontalSpacing(50);
    feederPageGridLayout->setVerticalSpacing(20);
    for (int j = 0;j < 10;j++)
    {
        if (j % 2 == 0)
        {
            feederPageGridLayout->addLayout(feederHBLayout[j], j / 2, 0, 1, 1);
        }
        else
        {
            feederPageGridLayout->addLayout(feederHBLayout[j], j / 2, 1, 1, 1);
        }
    }
    /* 返回按钮布局 */
    feederPageHBLayout = new QHBoxLayout();

    feederPageHBLayout->addWidget(feederMonitorBtn);
    feederPageHBLayout->addStretch();
    feederPageHBLayout->addWidget(cancelSetFeedBtn);
    feederPageGridLayout->addLayout(feederPageHBLayout, 5, 0, 1, 2);

    feederMonitorBtn->hide();

    updateFeederPage();

    // 槽函数
    connect(feederMonitorBtn, SIGNAL(toggled(bool)), this, SLOT(onFeederMonitorToggled(bool)));
    connect(feederSignalMapper, SIGNAL(mapped(int)), this, SLOT(feederValueChange(int)));
    connect(cancelSetFeedBtn, SIGNAL(pressed()), this, SLOT(onCancelSetFeedBtnClickedSlt()));
}

/**
 * @brief MainWidget::updateFeederPage
 * 根据不同机型配置振动器数目
 */
void MainWidget::updateFeederPage()
{
    QString strFeed;
    for (int i = 0; i < 10; i++)
    {
        strFeed.sprintf("%d", struCnfp.struGroupCtrl[0].nFeeder[i]);
        feedLbe[i]->setText(strFeed);
        feedLabel[i]->hide();
        feedLbe[i]->hide();
    }

    if (struCnfe.nMachine == MACHINE_CF)
    {
        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal / 2;i++)
        {
            feedLabel[i]->show();
            feedLbe[i]->show();
        }
    }
}

/**
 * @brief MainWidget::feederValueChange
 * @param indexOfFeeder
 * 修改各供料设置
 */
void MainWidget::feederValueChange(int indexOfFeeder)
{
    double nFeeder[10];
    nFeeder[indexOfFeeder] = feedLbe[indexOfFeeder]->text().toDouble();
    myInputPanel inputDlg(intType, 0, 100, nFeeder[indexOfFeeder]);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nFeeder[indexOfFeeder] = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nFeeder[indexOfFeeder] = nFeeder[indexOfFeeder];
        QString str = QString("%1").arg(nFeeder[indexOfFeeder]);
        feedLbe[indexOfFeeder]->setText(str);

        myFlow.resetFeeder(0);
        LOG_INFO_STM("Feeder" << indexOfFeeder << ":" << struCnfp.struGroupCtrl[0].nFeeder[indexOfFeeder]);
    }
}

/**
 * @brief MainWidget::updateFeederValue
 */
void MainWidget::updateFeederValue()
{
    QString strFeed;

    for (int i = 0; i < 10; i++)
    {
        strFeed.sprintf("%d", struCnfp.struGroupCtrl[0].nFeeder[i]);
        feedLbe[i]->setText(strFeed);
    }
}

void MainWidget::onFeederMonitorToggled(bool isChecked)
{
    if (isChecked)
    {
        struCnfg.nFeederCtrlEn = 1;
    }
    else
    {
        struCnfg.nFeederCtrlEn = 0;
    }
}

/**
 * 供料设置页面返回主页面
 */
void MainWidget::onCancelSetFeedBtnClickedSlt()
{
    updateHomePage();
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}
/*************************************************************************************************************
*
* 创建清灰设置：清灰刷和履带机清灰气帘、喷水页面及其相关功能
*
*************************************************************************************************************/
void MainWidget::CreateSetWipeWidget()
{
    /* page init */
        /* tab widget */
    mainWipeTabWidget = new MyTabBar(pageSetWipe);
    mainWipeTabWidget->addTab(myLan.wipe_set);
    mainWipeTabWidget->addTab(myLan.wipe_wind);
    mainWipeTabWidget->addTab(myLan.wipe_water);

    /* stacked Widget */
    wipeStackedWidget = new QStackedWidget(pageSetWipe);

    /* 清灰刷 */
    pageWipeBrush = new WipeBrushWidget();
    wipeStackedWidget->addWidget(pageWipeBrush);

    /* 清灰气帘 */
    pageWipeWind = new WipeWindWidget();
    wipeStackedWidget->addWidget(pageWipeWind);

    /* 清灰喷水 */
    pageWipeWater = new WipeWaterWidget();
    wipeStackedWidget->addWidget(pageWipeWater);

    pageWipeMode = new WipeModeWidget();
    wipeStackedWidget->addWidget(pageWipeMode);

    wipeModeBtn = new myPushButton("", myIcon.Action_Configure, true, true, pageSetWipe);
    wipeModeBtn->setFixedSize(QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));
    wipeModeBtn->hide();

    /* 返回 */
    cancelSetWipeBtn = new myPushButton(myLan.back, myIcon.Action_Back, true, true, pageSetWipe);
    cancelSetWipeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    cancelSetWipeBtn->setHidden(true);

    /* page update */
    connect(mainWipeTabWidget, SIGNAL(currentChanged(int)), wipeStackedWidget, SLOT(setCurrentIndex(int)));
    connect(cancelSetWipeBtn, SIGNAL(pressed()), this, SLOT(onCancelWipeBtnClickedSlt()));
    connect(wipeModeBtn, SIGNAL(pressed()), this, SLOT(onWipeModeBtnClickedSlt()));

    /* page layout */
    QVBoxLayout* mainLayout = new QVBoxLayout(pageSetWipe);
    QHBoxLayout* downLayout = new QHBoxLayout();
    downLayout->addWidget(wipeModeBtn, 1, Qt::AlignLeft);
    downLayout->addWidget(cancelSetWipeBtn, 1, Qt::AlignRight);

    mainLayout->setContentsMargins(10, 20, 10, 80);
    mainLayout->addWidget(mainWipeTabWidget);
    mainLayout->addWidget(wipeStackedWidget);
    mainLayout->addLayout(downLayout);
}

void MainWidget::updateWipePage()
{
    for (int i = mainWipeTabWidget->count() - 1; i >= 0; i--)
    {
        mainWipeTabWidget->removeTab(i);
    }

    if (struCnfe.nMachine == MACHINE_CF)
    {
        mainWipeTabWidget->insertTab(0, myLan.wipe_set);
    }
    //! 刷新
    pageWipeBrush->updateWipeBrushPage();
    pageWipeWind->updateWipeWindPage();
    pageWipeWater->updateWaterWindPage();
}

/**
 * 返回主界面
 */
void MainWidget::onCancelWipeBtnClickedSlt()
{
    if (wipeStackedWidget->currentIndex() == 3)
    {
        updateWipePage();
        wipeModeBtn->show();
    }
    else
    {
        updateHomePage();
        this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
    }
}
/**
 * 清灰模式设置
 */
void MainWidget::onWipeModeBtnClickedSlt()
{
    for (int i = 0; i < 3; i++)
    {
        mainWipeTabWidget->setTabEnabled(i, false);
    }

    wipeModeBtn->hide();
    wipeStackedWidget->setCurrentIndex(3);
}

/*************************************************************************************************************
*
**创建权限设置页面及其相关功能
*
************************************************************************************************************/
/**
 * 权限设置页面
 */
void MainWidget::CreateSetPurviewWidget()
{
    userOperatorBtn = new myPushButton(myLan.operator_mode, myIcon.System_Unlock,
        true, false, pagePurview);
    userOperatorBtn->setFixedSize(LCD_WIDTH / 3, BTN_HEIGHT);
    userEngineerBtn = new myPushButton(myLan.engineer, myIcon.System_Lock,
        true, true, pagePurview);
    userEngineerBtn->setFixedSize(LCD_WIDTH / 3, BTN_HEIGHT);
    userFactoryBtn = new myPushButton(myLan.vendor_mode, myIcon.System_Lock,
        true, true, pagePurview);
    userFactoryBtn->setFixedSize(LCD_WIDTH / 3, BTN_HEIGHT);

    cancelSetPurviewBtn = new myPushButton(myLan.back, myIcon.Action_Back,
        true, true, pagePurview);
    cancelSetPurviewBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    //页面布局
    purviewPageGridLayout = new QGridLayout(pagePurview);
    purviewPageGridLayout->setVerticalSpacing((LCD_CONTENT_HEIGTH - 4 * BTN_HEIGHT - 60) / 3);
    purviewPageGridLayout->setContentsMargins(10, 50, 10, 20);

    purviewPageHBLayout1 = new QHBoxLayout();
    purviewPageHBLayout1->setContentsMargins(LCD_WIDTH / 3, 0, LCD_WIDTH / 3, 0);
    purviewPageHBLayout1->addWidget(userOperatorBtn);
    purviewPageGridLayout->addLayout(purviewPageHBLayout1, 0, 0, 1, 1);

    purviewPageHBLayout2 = new QHBoxLayout();
    purviewPageHBLayout2->setContentsMargins(LCD_WIDTH / 3, 0, LCD_WIDTH / 3, 0);
    purviewPageHBLayout2->addWidget(userEngineerBtn);
    purviewPageGridLayout->addLayout(purviewPageHBLayout2, 1, 0, 1, 1);

    purviewPageHBLayout3 = new QHBoxLayout();
    purviewPageHBLayout3->setContentsMargins(LCD_WIDTH / 3, 0, LCD_WIDTH / 3, 0);
    purviewPageHBLayout3->addWidget(userFactoryBtn);
    purviewPageGridLayout->addLayout(purviewPageHBLayout3, 2, 0, 1, 1);

    purviewPageHBLayout4 = new QHBoxLayout();
    horizontalSpacer_9 = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    purviewPageHBLayout4->addItem(horizontalSpacer_9);
    purviewPageHBLayout4->addWidget(cancelSetPurviewBtn);
    purviewPageGridLayout->addLayout(purviewPageHBLayout4, 3, 0, 1, 3);

    //槽函数
    connect(userOperatorBtn, SIGNAL(pressed()), this, SLOT(onUserOperatorBtnClickedSlt()));
    connect(userEngineerBtn, SIGNAL(pressed()), this, SLOT(onUserEngineerBtnClickedSlt()));
    connect(userFactoryBtn, SIGNAL(pressed()), this, SLOT(onUserFactoryBtnClickedSlt()));
    connect(cancelSetPurviewBtn, SIGNAL(pressed()), this, SLOT(onCancelSetPurviewBtnClickedSlt()));
}

/**
 * @brief MainWidget::onUserOperatorBtnClickedSlt
 * 操作工权限
 */
void MainWidget::onUserOperatorBtnClickedSlt()
{
    userOperatorBtn->setEnabled(false);
    userEngineerBtn->setEnabled(true);
    userFactoryBtn->setEnabled(true);
    userEngineerBtn->setIcon(myIcon.System_Lock);
    userFactoryBtn->setIcon(myIcon.System_Lock);


    schemeParamBtn->setEnabled(false);
    enteryFactoryBtn->setEnabled(false);
    cameraSignalBtn->setEnabled(false);

    struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_OPERATOR;
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

/**
 * @brief MainWidget::onUserEngineerBtnClickedSlt
 * 工程师权限
 */
void MainWidget::onUserEngineerBtnClickedSlt()
{
    myInputPanel inputDlg(passwdType, 0, 0, 0);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        QString str = inputDlg.getText();
        if (str == "20130621")
        {
            userEngineerBtn->setIcon(myIcon.System_Unlock);
            userFactoryBtn->setIcon(myIcon.System_Lock);
            userOperatorBtn->setEnabled(true);
            userEngineerBtn->setEnabled(false);
            userFactoryBtn->setEnabled(true);
            schemeParamBtn->setEnabled(true);
            enteryFactoryBtn->setEnabled(false);
            cameraSignalBtn->setEnabled(true);

            struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_ENGINEER;
            this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
        }
        else
        {
            infoWidget->setLabelText(myLan.msg_password_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
        }
    }
}

/**
 * @brief MainWidget::onUserFactoryBtnClickedSlt
 * 厂家权限
 */
void MainWidget::onUserFactoryBtnClickedSlt()
{
    myInputPanel inputDlg(passwdType, 0, 0, 0);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        QString str = inputDlg.getText();
        int isNeedKey = 0;
#ifndef Q_OS_UNIX
        isNeedKey = 1;
#else
        isNeedKey = 0;
#endif
        if (str == "20180701" || isNeedKey)
        {
            userEngineerBtn->setIcon(myIcon.System_Lock);
            userFactoryBtn->setIcon(myIcon.System_Unlock);
            userOperatorBtn->setEnabled(true);
            userEngineerBtn->setEnabled(true);
            userFactoryBtn->setEnabled(false);
            schemeParamBtn->setEnabled(true);
            enteryFactoryBtn->setEnabled(true);
            cameraSignalBtn->setEnabled(true);

            struGsh.nAuthenticationLevel = AUTHENTICATION_LEVEL_FACTORY;
            this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
        }
        else
        {
            infoWidget->setLabelText(myLan.msg_password_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
        }
    }
}

/* 隐藏标题栏上的按钮 */
void MainWidget::onHideTitleBtnsSlt()
{
    //    backToHomeBtn->hide();
    feedSwitchBtn->hide();
}

/**
 * 权限设置页面 取消设置功能
 */
void MainWidget::onCancelSetPurviewBtnClickedSlt()
{
    updateHomePage();
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

/* 一键放料 */
void MainWidget::onOneKeyFeedBtnPressed()
{
    m_bIsFeed = !m_bIsFeed;

    /* 若选择一键放料则关闭自动供料 */
    if (m_bIsFeed)
    {
        //        m_oneKeyFeedBtn->setRedColor(GREEN);
        struGsh.bStatFeedCtrlEn = 0;

        /* 打开所有振动器 */
        for (int i = 0; i < ((struCnfg.struLevelInfo[0].nViewTotal == 1) ? struCnfg.struLevelInfo[0].nUnitLevelTotal / 2 : struCnfg.struLevelInfo[0].nUnitLevelTotal / 4) / 2; i++)
        {
            struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
        }
        myFlow.resetFeeder(0);
    }
    else
    {
        //        m_oneKeyFeedBtn->setRedColor(DEF);
        struGsh.bStatFeedCtrlEn = 1;
    }
}

/*!
 * \brief MainWidget::onCloseFeederSlt
 * 关闭下料请求响应处理
 * \param trigger
 */
void MainWidget::onCloseFeederSlt(int trigger)
{
    //! 供料状态判断，避免重复操作
    if (struGsh.bStatFeed == MODE_CLOSE)
    {
        return;
    }

    //! 从屏模式仅处理网络同步触发请求，忽略报警、PLC、手动、定时等其他触发方式
    if (struCnfg.nRunMode == MODE_RUN_SLAVE && trigger != TRIGGER_SYNC)
    {
        return;
    }

    //! 主屏模式忽略网络同步触发请求，仅处理手动、定时、报警、PLC等其他触发方式
    if (struCnfg.nRunMode == MODE_RUN_MASTER && trigger == TRIGGER_SYNC)
    {
        return;
    }

    //! 开始关闭供料操作
    infoWidget->setLabelText(myLan.msg_turning_off);
    infoWidget->delayShow();
    myFlow.onOff();
    infoWidget->hide();

    //! 同步刷新按钮显示
    QString textOff = myLan.feed + " " + myLan.off;
    //    QString strStyle5 = QString("QPushButton {border-image: url(:/res/png/workStart.png);}");
    //    feedSwitchBtn->setStyleSheet(strStyle5);
    feedSwitchBtn->setText(myLan.working_stop);
    feedSwitchBtn->setIcon(QIcon(":/res/png/workStart.png"));

    QPalette patlette;
    patlette.setColor(QPalette::ButtonText, Qt::black);
    feedSwitchBtn->setPalette(patlette);

    //    feedSwitchBtn->setRedColor(DEF);

        //! 隐藏一键放料按钮（米机料位监控）
    emit hideOneKeyFeedSig();

    //! 豆类定制机型处理
    if (struCnfe.nEnableBeanMachine == 1)
    {
        struGsh.enableJudgeBeanMachine = false;
    }

    //! 主屏模式广播同步供料控制请求
    if (struCnfg.nRunMode == MODE_RUN_MASTER)
    {
        QByteArray args;
        args.append(MODE_CLOSE);
        myDevMonitor->writeDatagram(CMD_SCREEN_FEED_CONTROL, args, strNetInfo.groupAddress, strNetInfo.selfNetControlPort);
    }
}

/*!
 * \brief MainWidget::onOpenFeederSlt
 * 打开下料请求响应处理
 * \param trigger
 */
void MainWidget::onOpenFeederSlt(int trigger)
{
    //! 供料状态判断，避免重复操作
    if (struGsh.bStatFeed == MODE_OPEN)
    {
        return;
    }

    //! 特定页面状态下禁止打开下料（通信自检、初始化、预热、喷阀自检、厂家设置等）
    int sm = stackedWidget->currentIndex();
    if (sm == SM_COMM_PAGE
        || sm == SM_COMM_LIST_PAGE
        || sm == SM_INIT_PAGE
        || sm == SM_PREHEAT_PAGE
        //        || sm == SM_TEST_EJECT_PAGE
        /*        || sm == SM_SET_FACTORY*/)
    {
        return;
    }

    int hsm = homePageStackedWidget->currentIndex();
    if (hsm == HOME_EJECT_PAGE_INDEX
        || hsm == HIGH_SET_OPR_PAGE_INDEX)
    {
        return;
    }

    //! 大数据采集同步状态下禁止打开下料
    if (struGsh.isQueryingCom3)
    {
        return;
    }

    //! 从屏模式仅处理网络同步触发请求，忽略手动、定时、报警、PLC等其他触发方式
    if (struCnfg.nRunMode == MODE_RUN_SLAVE && trigger != TRIGGER_SYNC)
    {
        return;
    }

    //! 主屏模式忽略网络同步触发请求，仅处理手动、定时、报警、PLC等其他触发方式
    if (struCnfg.nRunMode == MODE_RUN_MASTER && trigger == TRIGGER_SYNC)
    {
        return;
    }

    //! 主屏模式广播同步供料控制请求
    if (struCnfg.nRunMode == MODE_RUN_MASTER)
    {
        QByteArray args;
        args.append(MODE_OPEN);
        myDevMonitor->writeDatagram(CMD_SCREEN_FEED_CONTROL, args, strNetInfo.groupAddress, strNetInfo.selfNetControlPort);
    }

    //! 开始打开供料操作
    infoWidget->setLabelText(myLan.msg_turning_on);
    infoWidget->delayShow();
    myFlow.onOff();
    infoWidget->hide();

    //! 同步刷新按钮显示
    QString textOn = myLan.feed + " " + myLan.on;
    feedSwitchBtn->setText(myLan.working_start);
    //    QString strStyle5 = QString("QPushButton {border-image: url(:/res/png/workStop.png);}");
    //    feedSwitchBtn->setStyleSheet(strStyle5);
    feedSwitchBtn->setIcon(QIcon(":/res/png/workStop.png"));
    QPalette patlette;
    patlette.setColor(QPalette::ButtonText, Qt::red);
    feedSwitchBtn->setPalette(patlette);
    //    feedSwitchBtn->setRedColor(GREEN);

        //! RS机型料位监控下显示一键放料按钮
    emit showOneKeyFeedSig();

    //! 豆类定制机型处理
    if (struCnfe.nEnableBeanMachine == 1)
    {
        struGsh.enableJudgeBeanMachine = true;
    }
}

void MainWidget::createNetworkUploader()
{
#if 1
    // network uploader 
    MyUploader* uploader = new MyUploader;

    // uploader timer
    QTimer* uploaderTimer = new QTimer;
    uploaderTimer->setInterval(DEF_TIMEOUT_LOG);

    // uploader thread
    uploaderThread = new QThread(this);

    // move all to thread
    uploader->moveToThread(uploaderThread);
    uploaderTimer->moveToThread(uploaderThread);

    // response to log 
    connect(uploaderTimer, SIGNAL(timeout()), uploader, SLOT(uploadLog()));

    // response to alarm
    connect(getStatus, SIGNAL(alarmReady()), uploader, SLOT(uploadAlarm()));

    // response to signal statistic info
    connect(this, SIGNAL(statisticFinished()), uploader, SLOT(uploadStatistic()));

    // start thread
    connect(uploaderThread, SIGNAL(finished()), uploader, SLOT(deleteLater()));
    uploaderThread->start();
#endif
}

/* 响应跳转至灵敏度偏置页面信号 */
void MainWidget::onGoToSensBiasPageSlt()
{
    PageSensBias->updatePage();
    stackedWidget->setCurrentIndex(SM_SENS_BIAS_PAGE);
}

/* 响应跳转至设置灵敏度页面信号 */
void MainWidget::onGoToSetSensPageSlot()
{
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

//响应跳转到视频页面信号
void MainWidget::onGoToVideoPageSlot()
{
    pageVideo->updateVideoWidget();
    //pageVideo->videoImageDisplay();
    stackedWidget->setCurrentIndex(SM_VIDEO_PAGE);
}

//响应跳转到碎米率分析页面信号
void MainWidget::onGoToAutoAnalysisPageSlot()
{
    pageAutoAnalysisWidget->updatePage();
    stackedWidget->setCurrentIndex(SM_AUTO_ANALYSIS_PAGE);
}

/* 吹气次数获取线程 */
void MainWidget::createGetEjectTimesThread()
{
    g_ejectTimesThread = new EjectTimesThread;

    if (struCnfp.isFeederAIEnable == 1 && struCnfp.autoFeedEnablePageFlag == 1)
    {
        g_ejectTimesThread->start();
    }
}
/*******************************************************************************************************
 *
 * 创建喷阀自检页面及相关功能
 *
 *******************************************************************************************************/
 /**
  * 创建喷阀自检线程响应函数
  */
TestEjectThread::TestEjectThread()
{
    stopped = false;
}

void TestEjectThread::run()
{
    emit ejectorChangeSig();
    stopped = false;
    while (!stopped)
    {
        myFlow.sleep(1);
    }
}
void TestEjectThread::ejectTestDoneSlt()
{
    if (!stopped)
    {
        emit ejectorChangeSig();
    }
}

void TestEjectThread::stop()
{
    stopped = true;
}

/**
 * 创建喷阀自检页面
 */
void MainWidget::CreateTestEjectWidget()
{
    int margin = 60;

    if (LCD_WIDTH == 640)
    {
        margin = 20;
    }

    bFlagLoop = false;
    nDisplayNum = 1;
    ejectTestInterval = 300;
    ejectTestInterval2 = 100;
    aiEjectDelayTime = 800;

    // 线程创建
    threadStart = new TestEjectThread;

    currentLabel = new myLabel(myLan.ejector_current, pageTestEject);
    lcdNumber = new myLCDNumber(3, nDisplayNum, pageTestEject);
    minusBtn = new myPushButton("", myIcon.Action_Minus, pageTestEject);
    addBtn = new myPushButton("", myIcon.Action_Plus, pageTestEject);
    ejectorSlider = new mySlider(Qt::Horizontal, 1, 1, nDisplayNum, pageTestEject);
    //    loopCbx = new myCustomCheckBox("单喷阀测试", false,CB_STYLE_APPLY);
    loopCbx = new myCustomCheckBox(myLan.ejector_keep_loop, false, CB_STYLE_APPLY);
    layerBtn = new myPushButton(myLan.upper_layer, QIcon(), pageTestEject);

    highSpeedCbx = new myCustomCheckBox(myLan.ai_high_speed, false, CB_STYLE_APPLY);
    lowSpeedCbx = new myCustomCheckBox(myLan.ai_low_speed, true, CB_STYLE_APPLY);

    /* 更新滑动条及层数状态 */
    updateEjextorSlider();

    startBtn = new myPushButton(myLan.start, QIcon(""), true, pageTestEject);
    stopBtn = new myPushButton(myLan.stop, QIcon(""), false, pageTestEject);
    cancelTestEjectBtn = new myPushButton(myLan.back, myIcon.Action_Back, pageTestEject);
    cancelTestEjectBtn->hide();

    /* 尺寸限制 */
    minusBtn->setMaximumSize(QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));
    addBtn->setMaximumSize(QSize(SMALL_BTN_WIDTH, BTN_HEIGHT));
    lcdNumber->setFixedSize(100, BTN_HEIGHT);
    startBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    stopBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    layerBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    cancelTestEjectBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    // 页面布局
    QVBoxLayout* mainLay = new QVBoxLayout(pageTestEject);
    ejectPageGridLayout = new QGridLayout;
    ejectPageGridLayout->setContentsMargins(20, 20, 20, 10);

    ejectPageHBlayout1 = new QHBoxLayout();
    ejectPageHBlayout1->setContentsMargins(margin, 0, margin, 0);
    horizontalSpacer_3 = new QSpacerItem(ICON_WID + 60, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    ejectPageHBlayout1->addItem(horizontalSpacer_3);
    ejectPageHBlayout1->addWidget(currentLabel);
    ejectPageHBlayout1->addWidget(lcdNumber);
    horizontalSpacer_4 = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ejectPageHBlayout1->addItem(horizontalSpacer_4);
    ejectPageGridLayout->addLayout(ejectPageHBlayout1, 1, 0, 1, 1);

    ejectPageHBlayout2 = new QHBoxLayout();
    ejectPageHBlayout2->setContentsMargins(margin, 0, margin, 0);
    ejectPageHBlayout2->setSpacing(50);
    ejectPageHBlayout2->addWidget(minusBtn);
    ejectPageHBlayout2->addWidget(ejectorSlider);
    ejectPageHBlayout2->addWidget(addBtn);
    ejectPageGridLayout->addLayout(ejectPageHBlayout2, 2, 0, 1, 1);

    ejectPageHBlayout3 = new QHBoxLayout();
    ejectPageHBlayout3->setContentsMargins(margin, 0, margin, 0);
    horizontalSpacer_5 = new QSpacerItem(ICON_WID + 60, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    horizontalSpacer_7 = new QSpacerItem(ICON_WID + 60, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    horizontalSpacer_8 = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ejectPageHBlayout3->addItem(horizontalSpacer_5);
    ejectPageHBlayout3->addWidget(loopCbx);
    ejectPageHBlayout3->addItem(horizontalSpacer_7);
    ejectPageHBlayout3->addWidget(highSpeedCbx);
    ejectPageHBlayout3->addItem(horizontalSpacer_8);
    ejectPageHBlayout3->addWidget(lowSpeedCbx);
    horizontalSpacer_6 = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ejectPageHBlayout3->addItem(horizontalSpacer_6);
    ejectPageHBlayout3->addWidget(layerBtn);
    ejectPageGridLayout->addLayout(ejectPageHBlayout3, 0, 0, 1, 1);

    ejectPageHBlayout4 = new QHBoxLayout();
    ejectPageHBlayout4->setContentsMargins(margin, 0, margin, 0);
    //    horizontalSpacer_7 = new QSpacerItem(ICON_WID+60, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    //    ejectPageHBlayout4->addItem(horizontalSpacer_7);
    //    ejectPageHBlayout4->addWidget(highSpeedCbx);
    //    horizontalSpacer_8 = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    //    ejectPageHBlayout4->addItem(horizontalSpacer_8);
    //    ejectPageHBlayout4->addWidget(lowSpeedCbx);
    //    ejectPageGridLayout->addLayout(ejectPageHBlayout4,3,0,1,1);

    ejectPageHBlayout5 = new QHBoxLayout();
    ejectPageHBlayout5->setContentsMargins(margin, 0, margin, 0);
    ejectPageHBlayout5->addWidget(startBtn);
    horizontalSpacer_10 = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ejectPageHBlayout5->addItem(horizontalSpacer_10);
    ejectPageHBlayout5->addWidget(stopBtn);
    ejectPageGridLayout->addLayout(ejectPageHBlayout5, 3, 0, 1, 1);

    ejectPageHBlayout6 = new QHBoxLayout();
    ejectPageHBlayout6->addStretch();
    ejectPageHBlayout6->addWidget(cancelTestEjectBtn);
    mainLay->addLayout(ejectPageGridLayout);
    mainLay->addLayout(ejectPageHBlayout6);

    /*更新层按钮状态*/
    updateLayerBtnSlt();

    // 槽函数
    connect(cancelTestEjectBtn, SIGNAL(pressed()), this, SLOT(onCancelTestEjectBtnClickedSlt()));
    connect(ejectorSlider, SIGNAL(valueChanged(int)), this, SLOT(ejectPageDisplayLcdNumerSlt()));
    connect(addBtn, SIGNAL(pressed()), this, SLOT(onAddBtnClickedSlt()));
    connect(minusBtn, SIGNAL(pressed()), this, SLOT(onMinusBtnClickedSlt()));
    connect(loopCbx, SIGNAL(pressed()), this, SLOT(onLoopCbxClickedSlt()));
    connect(highSpeedCbx, SIGNAL(pressed()), this, SLOT(onHighSpeedCbxClickedSlt()));
    connect(lowSpeedCbx, SIGNAL(pressed()), this, SLOT(onLowSpeedCbxClickedSlt()));
    connect(startBtn, SIGNAL(pressed()), this, SLOT(onStartStopBtnClickedSlt()));
    connect(stopBtn, SIGNAL(pressed()), this, SLOT(onStartStopBtnClickedSlt()));
    connect(layerBtn, SIGNAL(pressed()), this, SLOT(onLayerBtnClickedSlt()));
    connect(threadStart, SIGNAL(ejectorChangeSig()), this, SLOT(ejectPageThreadStartSlt()));
    connect(this, SIGNAL(ejectTestDonesig()), threadStart, SLOT(ejectTestDoneSlt()));

}
/**
 * @brief MainWidget::updateEjextorSlider
 * 更新滑动条及状态
 */
void MainWidget::updateEjextorSlider()
{
    layerBtn->setHidden(true);
    if (struCnfe.nMachine == MACHINE_CF)
    {
        nMaxEjector = struCnfg.nEjectorsPerChute * struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal / 2;
        ejectorSlider->setRange(1, nMaxEjector);
        if (nDisplayNum > struCnfg.nEjectorsPerChute * struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal / 2)
        {
            nDisplayNum = struCnfg.nEjectorsPerChute * struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal / 2;
            ejectorSlider->setValue(nDisplayNum);
        }
    }
}
/**
 *   返回主界面
 */
void MainWidget::onCancelTestEjectBtnClickedSlt()
{
    int currentGroup = 0;
    if (threadStart->isRunning())
    {
        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
        layerBtn->setEnabled(true);
        threadStart->stop();
        threadStart->wait();

        currentGroup = (nDisplayNum - 1) / struCnfg.nEjectorsPerChute;
        /* 前8个通道喷阀由接口板1控制，后2个通道喷阀由接口板2控制 */
        if (currentGroup < MAX_UNIT_PER_INT / 2)
        {
            MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x00, 0, 0,
                (nDisplayNum - 1) / 256, (nDisplayNum - 1) % 256, 3);
        }
        else
        {
            int nNum = nDisplayNum - 1 - MAX_UNIT_PER_INT / 2 * struCnfg.nEjectorsPerChute;
            MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel + 1, 0, 0x00, 0, 0,
                nNum / 256, nNum % 256, 3);
        }
    }

    updateHomePage();
    this->stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}
/**
 * 增加当前喷阀数
 */
void MainWidget::onAddBtnClickedSlt()
{
    nDisplayNum = lcdNumber->value();

    if (nDisplayNum < nMaxEjector)
        nDisplayNum++;

    lcdNumber->display(nDisplayNum);
    ejectorSlider->setValue(nDisplayNum);
}
/**
 * 减少当前喷阀数
 */
void MainWidget::onMinusBtnClickedSlt()
{
    nDisplayNum = lcdNumber->value();

    if (nDisplayNum > 1)
        nDisplayNum--;

    lcdNumber->display(nDisplayNum);
    ejectorSlider->setValue(nDisplayNum);
}
/**
 * 循环检测
 */
void MainWidget::onLoopCbxClickedSlt()
{
    bFlagLoop = !bFlagLoop;
}
/**
 * @brief MainWidget::onLayerBtnClickedSlt
 */
void MainWidget::onLayerBtnClickedSlt()
{
    switch (struCnfe.nMachine)
    {
    default:
        layerBtn->setHidden(true);
        break;
    }
}
/**
 * @brief MainWidget::updateLayerBtnSlt
 */
void MainWidget::updateLayerBtnSlt()
{
    switch (struCnfe.nMachine)
    {
    default:
        layerBtn->setHidden(true);
        break;
    }
}
/**
 * 开始，停止检测
 */
void MainWidget::onStartStopBtnClickedSlt()
{
    if (threadStart->isRunning())
    {
        threadStart->stop();
        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
        layerBtn->setEnabled(true);
    }
    else
    {
        threadStart->start();
        startBtn->setEnabled(false);
        stopBtn->setEnabled(true);
        layerBtn->setEnabled(false);
    }
}

/**
 * 高速检测
 */
void MainWidget::onHighSpeedCbxClickedSlt()
{
    highSpeedCbx->setChecked(true);
    lowSpeedCbx->setChecked(false);
    ejectTestInterval = 100;
    ejectTestInterval2 = 50;
    aiEjectDelayTime = 400;
}

/**
 * 低速检测
 */
void MainWidget::onLowSpeedCbxClickedSlt()
{
    highSpeedCbx->setChecked(false);
    lowSpeedCbx->setChecked(true);
    ejectTestInterval = 300;
    ejectTestInterval2 = 100;
    aiEjectDelayTime = 800;
}

/**
 *  更新喷阀状态
 */
void MainWidget::ejectPageDisplayLcdNumerSlt()
{
    nDisplayNum = ejectorSlider->value();
    lcdNumber->display(nDisplayNum);

}
/**
 *  喷阀自检
 */
void MainWidget::ejectPageThreadStartSlt()
{
    if (struCnfg.aiEnable != 1)
    {
        if (threadStart->isRunning())
        {
            ejectorSlider->setValue(nDisplayNum);
            lcdNumber->display(nDisplayNum);
            if (myFlow.getProductLineNo() == 0)
            {    //杂粮机型拓展
                int currentGroup = (nDisplayNum - 1) / struCnfg.nEjectorsPerChute;
                /* 前12个通道喷阀由接口板1控制，12以后的通道喷阀由接口板2控制 */
                if (currentGroup < MAX_UNIT_PER_INT / 2)
                {
                    MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x01, 0, 0,
                        (nDisplayNum - 1) / 256, (nDisplayNum - 1) % 256, 3);
                    myFlow.msleep(ejectTestInterval);
                    MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x00, 0, 0,
                        (nDisplayNum - 1) / 256, (nDisplayNum - 1) % 256, 3);
                    myFlow.msleep(ejectTestInterval2);
                }
                else
                {
                    int nNum = nDisplayNum - 1 - MAX_UNIT_PER_INT / 2 * struCnfg.nEjectorsPerChute;
                    MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel + 1, 0, 0x01, 0, 0,
                        nNum / 256, nNum % 256, 3);
                    myFlow.msleep(ejectTestInterval);
                    MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel + 1, 0, 0x00, 0, 0,
                        nNum / 256, nNum % 256, 3);
                    myFlow.msleep(ejectTestInterval2);
                }
            }
            else
            {
                MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x01, 0, 0,
                    (nDisplayNum - 1) / 256, (nDisplayNum - 1) % 256, 3);
                myFlow.msleep(ejectTestInterval);
                MySerial.com1Write(CMD_INT_EJTEST, INT, struGsh.nLevel, 0, 0x00, 0, 0,
                    (nDisplayNum - 1) / 256, (nDisplayNum - 1) % 256, 3);
                myFlow.msleep(ejectTestInterval2);
            }

            if (!bFlagLoop)
            {
                nDisplayNum++;
            }

            if (nDisplayNum > nMaxEjector)
            {
                nDisplayNum = nMaxEjector;
                if (!bFlagLoop)
                {
                    startBtn->setEnabled(true);
                    stopBtn->setEnabled(false);
                    layerBtn->setEnabled(true);
                    threadStart->stop();
                    threadStart->wait();
                    nDisplayNum = 1;
                    ejectorSlider->setValue(nDisplayNum);
                    lcdNumber->display(nDisplayNum);
                }
            }
        }
    }
    else
    {
        if (threadStart->isRunning())
        {
            ejectorSlider->setValue(nDisplayNum);
            lcdNumber->display(nDisplayNum);
            QByteArray args;
            AI_Data_Protocol_D data;
            int ejectorIndex = (nDisplayNum - 1) / struCnfg.nEjectorsPerChute;
            args[0] = 0;
            args[1] = (nDisplayNum - 1) % struCnfg.nEjectorsPerChute;
            args[2] = aiEjectDelayTime / 256;
            args[3] = aiEjectDelayTime % 256;
            MyUpd.writeDatagram(CMD_AI_EJECTOR_FIXED_BGN, ejectorIndex, 4, args, struGsh.addressList.at(ejectorIndex), AI_UDP_SEND_PORT);
            data.nCommandAddress = CMD_AI_EJECTOR_FIXED_BGN;
            MyUpd.readUdpDatagrams(&data, 13);
            args.clear();
            //            myFlow.msleep(100);
            args[0] = 0;
            args[1] = (nDisplayNum - 1) % struCnfg.nEjectorsPerChute;
            MyUpd.writeDatagram(CMD_AI_EJECTOR_FIXED_END, ejectorIndex, 2, args, struGsh.addressList.at(ejectorIndex), AI_UDP_SEND_PORT);
            data.nCommandAddress = CMD_AI_EJECTOR_FIXED_END;
            MyUpd.readUdpDatagrams(&data, 13);
            myFlow.msleep(aiEjectDelayTime / 10);
            if (!bFlagLoop)
            {
                nDisplayNum++;
            }
            if (nDisplayNum > nMaxEjector)
            {
                nDisplayNum = nMaxEjector;
                if (!bFlagLoop)
                {
                    startBtn->setEnabled(true);
                    stopBtn->setEnabled(false);
                    threadStart->stop();
                    threadStart->wait();
                    nDisplayNum = 1;
                    ejectorSlider->setValue(nDisplayNum);
                    lcdNumber->display(nDisplayNum);

                }
            }
        }

    }

    emit ejectTestDonesig();
}

void MainWidget::onFunctionKeyChangedSlt(int index)
{
    /* title */
        /* default hide status */
    //    backToHomeBtn->setHidden(true);
    //    beltSwitchBtn->setHidden(true);
    //    feedSwitchBtn->setHidden(false);
    //    machineInfoBtn->setHidden(true);

        /* default enable status */
    //    backToHomeBtn->setEnabled(true);
    //    feedSwitchBtn->setEnabled(true);
    //    beltSwitchBtn->setEnabled(true);
    //    machineInfoBtn->setEnabled(true);

    /* statusline */
        /* default hide status */
    //    lineStatus->setHidden(true);
    //    systemHaltBtn->setHidden(false);
    //    saveParamBtn->setHidden(false);
    //    setPurviewBtn->setHidden(true);
    //    statInfoBtn->setHidden(false);
    //    videoBtn->setHidden(false);

        /* default enable status */
    //    systemHaltBtn->setEnabled(false);
    //    saveParamBtn->setEnabled(true);
    //    setPurviewBtn->setEnabled(true);
    //    statInfoBtn->setEnabled(false);
    //    videoBtn->setEnabled(true);



    switch (index)
    {
    case SM_COMM_PAGE:
    case SM_COMM_LIST_PAGE:
    case SM_INIT_PAGE:
    case SM_PREHEAT_PAGE:
        videoBtn->setEnabled(false);
        systemHaltBtn->setEnabled(false);
        saveParamBtn->setEnabled(false);
        feedSwitchBtn->setEnabled(false);
        statInfoBtn->setEnabled(false);
        videoBtn->setHidden(false);
        systemHaltBtn->setHidden(false);
        saveParamBtn->setHidden(false);
        feedSwitchBtn->setHidden(false);
        statInfoBtn->setHidden(false);
        lineTitle->setHidden(false);
        break;
    case SM_HOME_PAGE:
    case SM_HOME_TEA_PAGE:
        videoBtn->setEnabled(true);
        systemHaltBtn->setEnabled(true);
        saveParamBtn->setEnabled(true);
        feedSwitchBtn->setEnabled(true);
        statInfoBtn->setEnabled(true);
        videoBtn->setHidden(false);
        systemHaltBtn->setHidden(false);
        saveParamBtn->setHidden(false);
        feedSwitchBtn->setHidden(false);
        statInfoBtn->setHidden(false);
        lineTitle->setHidden(false);
        break;
    case SM_AI_PAGE:
        //    case SM_SCHEME_PARAM_PAGE:
    case SM_VIDEO_PAGE:
    case SM_AI_MODEL_SET_PAGE:
    case SM_AUTO_ANALYSIS_PAGE:
        videoBtn->setEnabled(false);
        systemHaltBtn->setEnabled(false);
        saveParamBtn->setEnabled(false);
        feedSwitchBtn->setEnabled(false);
        statInfoBtn->setEnabled(false);
        videoBtn->setHidden(true);
        systemHaltBtn->setHidden(true);
        saveParamBtn->setHidden(true);
        feedSwitchBtn->setHidden(true);
        statInfoBtn->setHidden(true);
        lineTitle->setHidden(true);
        break;
        //    case SM_SET_FACTORY:
        //        feedSwitchBtn->setHidden(true);
        //        beltSwitchBtn->setHidden(true);
        //        lineStatus->setHidden(true);
        //        systemHaltBtn->setHidden(true);
        //        saveParamBtn->setHidden(true);
        //        setPurviewBtn->setHidden(true);
        //        statInfoBtn->setHidden(true);
        //        machineInfoBtn->setHidden(true);
        //        videoBtn->setHidden(true);
        //        break;
    case SM_STATUS_INFO_PAGE:
        videoBtn->setEnabled(false);
        systemHaltBtn->setEnabled(false);
        saveParamBtn->setEnabled(false);
        feedSwitchBtn->setEnabled(false);
        statInfoBtn->setEnabled(false);
        statInfoBtn->setHidden(false);
        videoBtn->setHidden(false);
        saveParamBtn->setHidden(false);
        systemHaltBtn->setHidden(false);
        feedSwitchBtn->setHidden(false);
        lineStatus->setHidden(false);

        break;
        //    case SM_TEST_EJECT_PAGE:
        //        feedSwitchBtn->setEnabled(false);
    default:
        break;
    }
    //    oprModeBtn->setHidden(true);
    //    mainOprBtn->setHidden(true);
    //    videoBtn->setHidden(true);

}

/* 响应当前页索引发生变化的情况 */
void MainWidget::onStackedWidgetIndexChangedSlt(int index)
{
    QString title;
    stackedWidget->setFixedHeight(LCD_HEIGHT - LCD_TITLE_HEIGHT - LCD_STATUS_HEIGHT - 10);
    switch (index)
    {
        //    case SM_AI_PAGE:
        ////    case SM_SCHEME_PARAM_PAGE:
    case SM_VIDEO_PAGE:
        //    case SM_AUTO_ANALYSIS_PAGE:
        stackedWidget->setFixedHeight(LCD_HEIGHT - LCD_TITLE_HEIGHT);
        break;
        //    case SM_SET_FACTORY:
        //        stackedWidget->setFixedHeight(LCD_HEIGHT-LCD_TITLE_HEIGHT);
        //        break;
    default:
        stackedWidget->setFixedHeight(LCD_HEIGHT - LCD_TITLE_HEIGHT - 20);
        break;
    }

    switch (index)
    {
    case SM_COMM_PAGE:
    case SM_COMM_LIST_PAGE:
    case SM_INIT_PAGE:
    case SM_PREHEAT_PAGE:
    case SM_HOME_PAGE:
    case SM_HOME_TEA_PAGE:
        //! 定制标题
        title = QString().fromLocal8Bit(struCnfe.sTitle);
        //! 默认标题
        if (title.isEmpty())
        {
            title = QString("%1").arg(myLan.page_home);
        }
        //! 显示模式
        if (struCnfg.nRunMode != MODE_RUN_STANDALONE)
        {
            title = title + QString(" - %1").arg(myFlow.getRunModeName(struCnfg.nRunMode));
        }
        break;
        //    case SM_SET_FEED_PAGE:
        //        title = QString("%1").arg(myLan.feeder_set);
        //        break;
        //    case SM_SET_WIPE_PAGE:
        //        title = QString("%1").arg(myLan.wipe_set);
        //        break;
        //    case SM_TEST_EJECT_PAGE:
        //        title = QString("%1").arg(myLan.eject_test);
        //        break;
    case SM_STATUS_INFO_PAGE:
        title = QString("%1").arg(myLan.system_stat);
        break;
        //    case SM_MANAGE_SCHEME_PAGE:
    case SM_MANAGE_NEW_SCHEME_PAGE:
    case SM_COPY_MAJOR_PARAMS_PAGE:
        title = QString("%1").arg(myLan.scheme_manage);
        break;
    case SM_SET_PURVIEW:
        title = QString("%1").arg(myLan.set_purview);
        break;
        //    case SM_SET_FACTORY:
        //        title = QString("%1").arg(myLan.factory_set);
        //        break;
        //    case SM_SCHEME_PARAM_PAGE:
        //        title = QString("%1").arg(myLan.scheme_parameter);
        //        break;
    case SM_AI_PAGE:
        title = QString("%1").arg(myLan.scheme_parameter);
        break;
    case SM_ENGINEER_PAGE:
        title = QString("%1").arg(myLan.engineering_mode);
        break;
        //    case SM_SET_SENS_PAGE:
        //        if(struCnfe.nProduct == OPT_PRODUCT_ANCOO_RS) {
        //            title = QString("%1").arg(myLan.select_sensitivity);
        //        } else {
        //            title = QString("%1").arg(myLan.sensitivity_set);
        //        }
        //        break;
    case SM_AUTO_PAGE:
        title = QString("%1").arg(myLan.super_brain);
        break;
    case SM_AUTO_SIGNAL_PAGE:
    case SM_UNSUPERVISED_AI:
        title = QString("%1").arg(myLan.super_brain);
        break;
    case SM_VIDEO_PAGE:
        title = QString("%1").arg(myLan.video);
        break;
    case SM_AUTO_ANALYSIS_PAGE:
        title = QString("%1").arg(myLan.auto_analysis);
    default:
        break;
    }
    titleLabel->setText("");
    //    qDebug()<<"index："<<index<<endl;


        /* function key */
    onFunctionKeyChangedSlt(index);
}

/*!
 * \brief MainWidget::onAutoSetBtnClickedSlt
 */
void MainWidget::onAutoSetBtnClickedSlt()
{
    this->stackedWidget->setCurrentIndex(SM_UNSUPERVISED_AI);
    pageUnsupervisedAIWidget->updateAIMainWidget();
    //    backToHomeBtn->setHidden(true);
}

/*!
 * \brief MainWidget::onVideoBtnClickedSlt
 */
void MainWidget::onVideoBtnClickedSlt()
{
    pageVideo->updateVideoWidget();
    this->stackedWidget->setCurrentIndex(SM_VIDEO_PAGE);
}

void MainWidget::onAiSetBtnClickedSlt()
{
    pageAiSet->getParaInfo();
    //    pageVideo->getParaInfo();
    homePageStackedWidget->setCurrentIndex(HIGH_SET_AI_PAGE_INDEX);
}


/*!
 * \brief MainWidget::onThroughputBtnClickedSlt
 */
void MainWidget::onThroughputBtnClickedSlt()
{
    pageThroughput->updatePage();
    this->stackedWidget->setCurrentIndex(SM_THROUGHPUT_PAGE);
}

/*!
 * \brief MainWidget::onAiModelSetBtnClickedSlt
 */
void MainWidget::onAiModelSetBtnClickedSlt()
{
    aiModelSetWidget->updateWidget();
    this->stackedWidget->setCurrentIndex(SM_AI_MODEL_SET_PAGE);
}

/*!
 * \brief MainWidget::CreateMotorCtrlWidget
 */
void MainWidget::CreateMotorCtrlWidget()
{
    m_motorPageStartCbx = new myCustomCheckBox(myLan.pmt_run, true);
    m_motorPageBackBtn = new myPushButton(myLan.back, myIcon.Action_Back, pageMotorCtrl);
    m_motorPageStartCbx->setMaximumHeight(BTN_HEIGHT);
    m_motorPageBackBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    if (struCnfg.nPMTRun)
    {
        m_motorPageStartCbx->setChecked(true);
    }
    else
    {
        m_motorPageStartCbx->setChecked(false);
    }

    QHBoxLayout* upLay = new QHBoxLayout;
    QHBoxLayout* downLay = new QHBoxLayout;
    QVBoxLayout* mainLay = new QVBoxLayout(pageMotorCtrl);

    upLay->addWidget(m_motorPageStartCbx);
    upLay->setContentsMargins(this->width() / 3, 0, this->width() / 3, 0);
    downLay->addStretch();
    downLay->addWidget(m_motorPageBackBtn);
    mainLay->addStretch();
    mainLay->addLayout(upLay);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(m_motorPageStartCbx, SIGNAL(pressed()), this, SLOT(onMotorPageStartCbxSlt()));
    connect(m_motorPageBackBtn, SIGNAL(pressed()), this, SLOT(onMotorPageBackBtnSlt()));
}

/*!
 * \brief MainWidget::onMotorPageStartCbxSlt
 */
void MainWidget::onMotorPageStartCbxSlt()
{
    if (m_motorPageStartCbx->getChecked())
    {
        struCnfg.nPMTRun = 1;
    }
    else
    {
        struCnfg.nPMTRun = 0;
    }
    for (int i = 0; i < struCnfg.nLevelTotal;i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal;j++)
        {
            MySerial.com1Write(CMD_UNIT_MOTOR_CTRL, UNIT, i, j, 0, 0, 0, struCnfg.nPMTNum, struCnfg.nPMTRun, 3);
        }
    }
}

/*!
 * \brief MainWidget::onMotorPageBackBtnSlt
 */
void MainWidget::onMotorPageBackBtnSlt()
{
    stackedWidget->setCurrentIndex(SM_HOME_PAGE);
}

/*!
 * \brief I机型老化测试
 */
void MainWidget::machineTestForI()
{
    /* I机型老化实验 */
    if (struGsh.nCounter < 180)
    {   // 3分钟后开始
        return;
    }

    if (struGsh.nCounter % 180 != 0)
    {  // 3分钟做一次
        return;
    }

    char tmpBuf[3600];//1024*3+512+16
    char infraBuf[1040];
    int ret_color = 0;
    int ret_infra = 0;
    memset(tmpBuf, 0, sizeof(tmpBuf));
    memset(infraBuf, 0, sizeof(infraBuf));
    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
    {   //彩色
        ret_color = 0;
        ret_infra = 0;

        MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, i, 0, 0, 0, 0, 0, 1);

        myFlow.msleep(400);
        MySerial.com1Read(tmpBuf, 3600);
        if (tmpBuf[0] != 0xAA || tmpBuf[1] != 0xAA || tmpBuf[2] != 0xAA || tmpBuf[3] != 0xAA)
        {
            ret_color = 1;
        }
        myFlow.msleep(10);

        if ((i % 2 == 0) && (struGsh.nDevTypeAddrBias != 0))
        {   //红外
            MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, i + struGsh.nDevTypeAddrBias, 0, 0, 0, 0, 0, 1);
            myFlow.msleep(400);
            MySerial.com1Read(infraBuf, 1040);
            if (infraBuf[0] != 0xAA || infraBuf[1] != 0xAA || infraBuf[2] != 0xAA || infraBuf[3] != 0xAA)
            {
                ret_infra = 1;
            }
        }
        myFlow.msleep(10);

        // 彩色相机存文件
        QString tmp;
        int countR = 0;
        int countG = 0;
        int countB = 0;
        if (ret_color == 1)
        {
            tmp.sprintf("%s%s", qPrintable(tmp), "error");
        }
        else
        {
            for (int j = 10; j < 3600 - 6; j++)
            {
                tmp.sprintf("%s%02x,", qPrintable(tmp), tmpBuf[j] & 0xff);
                if (j % 3 == 1)
                {
                    countR += tmpBuf[j] & 0xff;
                }
                if (j % 3 == 2)
                {
                    countG += tmpBuf[j] & 0xff;
                }
                if (j % 3 == 0)
                {
                    countB += tmpBuf[j] & 0xff;
                }
            }
        }

        QFile file(QString("%1/userdata/%2%3").arg(APP_PATH).arg(i).arg(".txt"));
        if (!file.open(QIODevice::Append | QIODevice::Text))
        {
            return;
        }
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("hh:mm:ss") << "  ave(R,G,B)==>(" << countR / 1024.0 << "," << countG / 1024.0 << "," << countB / 1024.0 << ")" << "\n" << qPrintable(tmp) << "\n\n";
        file.close();

        // 红外相机存文件
        if ((i % 2 == 0) && (struGsh.nDevTypeAddrBias != 0))
        {   //红外
            QString tmp1;
            if (ret_infra == 1)
            {
                tmp1.sprintf("%s%s", qPrintable(tmp1), "error");
            }
            else
            {
                for (int j = 10; j < 1040 - 6; j++)
                {
                    tmp1.sprintf("%s%02x,", qPrintable(tmp1), infraBuf[j] & 0xff);
                    if (j % 2 == 1)
                    {
                        countR += infraBuf[j] & 0xff;
                    }
                    if (j % 2 == 0)
                    {
                        countG += infraBuf[j] & 0xff;
                    }
                }
            }

            QFile file1(QString("%1/userdata/%2%3").arg(APP_PATH).arg(i + struGsh.nDevTypeAddrBias).arg(".txt"));
            if (!file1.open(QIODevice::Append | QIODevice::Text))
            {
                return;
            }
            QTextStream out1(&file1);
            out1 << QDateTime::currentDateTime().toString("hh:mm:ss") << "  ave(R,G)==>(" << countR / 512.0 << "," << countG / 512.0 << ")" << "\n" << qPrintable(tmp1) << "\n\n";
            file1.close();
        }
    }
}
