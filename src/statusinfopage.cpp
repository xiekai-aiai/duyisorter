    /*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        statusinfopage.cpp
 * \brief       系统状态信息界面源文件
 *              子页面包括系统信息、报警信息、版本信息、远程控制、设置时间及版权信息。
 * \date        2015.01.14
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <QtNetwork>
#include <QSettings>
//#include <QWebView>
//#include <QWebFrame>
//#include <QWebElement>
#include "statusinfopage.h"
#include "src/qdatabase.h"


StatusInfoPage::StatusInfoPage(QWidget *parent) :
    QWidget(parent)
{
    config = g_Config::getInstance();

    // 状态列表
    listWidget = new myListWidget();
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(ICON_WID, ICON_HEI));
    listWidget->setMovement(QListView::Static);
    listWidget->setFixedWidth(BTN_WIDTH);

    for(int i = 0; i < 10; i++) {
        statusListItem[i] = new myListWidgetItem(QString(""),QIcon(),QSize());
        statusListItem[i]->setSizeHint(QSize(100,50));
        listWidget->addItem(statusListItem[i]);
    }

    // 默认选中第一列
    listWidget->setCurrentRow(0);
    stackedWidget = new QStackedWidget();

    // 一般状态页面
    pageGeneral = new QWidget();
    stackedWidget->addWidget(pageGeneral);
    CreateGeneralPage();

    // 数据中心页面
    pageStatistic = new QWidget();
    stackedWidget->addWidget(pageStatistic);

    // 报警设置页面
    pageAlarm = new QWidget();
    stackedWidget->addWidget(pageAlarm);
    CreateSetAlarmPage();

    // 系统时间页面
    pageSystemTime = new QWidget();
    stackedWidget->addWidget(pageSystemTime);
    CreateSystemTimePage();

    // 软件版本页面
    pageSoftwareVersion = new QWidget();
    stackedWidget->addWidget(pageSoftwareVersion);
    CreateSoftwareVersionPage();

    // 设置网络页面
    pageSetNetwork = new QWidget();
    stackedWidget->addWidget(pageSetNetwork);
    CreateSetNetworkPage();

    // 帮助文档页面
    pageHowTo = new QWidget();
    stackedWidget->addWidget(pageHowTo);
    CreateGetHowToWidget();

    // 系统信息页面
    pageInformation = new QWidget();
    stackedWidget->addWidget(pageInformation);
    CreateInfomationPage();

    // 一键自动校正页面
    pageAutoCalibrate  = new QWidget();
    stackedWidget->addWidget(pageAutoCalibrate);
    CreateAutoCalibratePage();

    // 联系我们页面
    pageContactUs = new QWidget();
    stackedWidget->addWidget(pageContactUs);
    CreateConnectUsPage();

    leftBtn = new myPushButton(myLan.recheck, myIcon.Action_Refresh);
    leftBtn->hide();
    cancelBtn = new myPushButton(myLan.back, myIcon.Action_Back);

    /* 尺寸限制 */
    listWidget->setMaximumWidth(LCD_WIDTH/5);
    listWidget->setMinimumWidth(LCD_WIDTH/5);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setStyleSheet("background-color:transparent");

    leftBtn->setFixedSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    cancelBtn->setFixedSize(QSize(BTN_WIDTH,BTN_HEIGHT));

    /* 整体页面布局 */
    QHBoxLayout *backHBLayout  = new QHBoxLayout();
    backHBLayout->addWidget(leftBtn);
    horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    backHBLayout->addItem(horizontalSpacer);
    backHBLayout->addWidget(cancelBtn);
    backHBLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *statusInfoPageVBLayout = new QVBoxLayout();
    statusInfoPageVBLayout->addWidget(stackedWidget);
    statusInfoPageVBLayout->addLayout(backHBLayout);

    QHBoxLayout *statusInfoPageHBLayout  = new QHBoxLayout(this);
    statusInfoPageHBLayout->addWidget(listWidget);
    statusInfoPageHBLayout->setSpacing(10);
    statusInfoPageHBLayout->addLayout(statusInfoPageVBLayout);
    setLayout(statusInfoPageHBLayout);

    // 槽函数
    connect(leftBtn, SIGNAL(pressed()), this, SLOT(onLeftBtnClickedSlt()));
    connect(cancelBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnClickedSlt()));
    connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onListWidgetRowChangedSlt(int)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeOutSlt()));
}

void StatusInfoPage::TimeOutSlt()
{
    updateGeneralPage();
    updateDateTime();
}

void StatusInfoPage::updateAll()
{
    updateListWidget();
    updateGeneralPage();
    updateSetAlarmPageSlt();
    versionUpdateCamera();
    versionUpdateCtrl();
    versionUpdateInterface();
    versionUpdateUSBFirmware();
    versionUpdateLightSrc();
    updateDateTime();
    updateSetNetworkPageSlt();
    setExpireTime();
    timer->start(1000);
}

void StatusInfoPage::updateListWidget()
{
    // 清空listWidget控件
    listWidget->clear();
    listIndex.clear();

    // 清空堆栈页面管理器
    stackedWidget->removeWidget(pageGeneral);
    stackedWidget->removeWidget(pageStatistic);
    stackedWidget->removeWidget(pageAlarm);
    stackedWidget->removeWidget(pageSystemTime);
    stackedWidget->removeWidget(pageSoftwareVersion);
    stackedWidget->removeWidget(pageSetNetwork);
    stackedWidget->removeWidget(pageInformation);
    stackedWidget->removeWidget(pageAutoCalibrate);
    stackedWidget->removeWidget(pageHowTo);
    stackedWidget->removeWidget(pageContactUs);
    stackedWidget->removeWidget(pageStatistic);

    // 根据机型及系统加密状态填充内容
    if(LCD_WIDTH == 640){
        for(int i = 0; i < 10; i++) {
            statusListItem[i] = new myListWidgetItem(QString(""), QIcon(), QSize(100, 58));
        }
    } else {
        for(int i = 0; i < 10; i++) {
            statusListItem[i] = new myListWidgetItem(QString(""), QIcon(), QSize(BTN_WIDTH, ICON_HEI*2));
        }
    }

    statusListItem[STATUS_INDEX_INFO]->setText(myLan.system_stat);
    statusListItem[STATUS_INDEX_INFO]->setIcon(QIcon(":/res/png/System_Status.png"));
    statusListItem[STATUS_INDEX_STATISTIC]->setText(myLan.big_data);
    statusListItem[STATUS_INDEX_STATISTIC]->setIcon(myIcon.Message_Info);
    statusListItem[STATUS_INDEX_ALARM]->setText(myLan.alarm_set);;
    statusListItem[STATUS_INDEX_ALARM]->setIcon(QIcon(":/res/png/System_Alerm.png"));
    statusListItem[STATUS_INDEX_TIME]->setText(myLan.system_time);
    statusListItem[STATUS_INDEX_TIME]->setIcon(QIcon(":/res/png/System_Time.png"));
    statusListItem[STATUS_INDEX_VERSION]->setText(myLan.software_version);
    statusListItem[STATUS_INDEX_VERSION]->setIcon(QIcon(":/res/png/System_Version.png"));
    statusListItem[STATUS_INDEX_NETWORK]->setText(myLan.network_config);
    statusListItem[STATUS_INDEX_NETWORK]->setIcon(QIcon(":/res/png/System_Network.png"));
    statusListItem[STATUS_INDEX_HELP]->setText(myLan.howto);
    statusListItem[STATUS_INDEX_HELP]->setIcon(myIcon.Message_Help);
    statusListItem[STATUS_INDEX_COPYRIGHT]->setText(myLan.information);
    statusListItem[STATUS_INDEX_COPYRIGHT]->setIcon(myIcon.Message_Info);
    statusListItem[STATUS_INDEX_AUTO]->setText(myLan.auto_calibrate);
    statusListItem[STATUS_INDEX_AUTO]->setIcon(myIcon.Chart_Line);
    statusListItem[STATUS_INDEX_CONNECT]->setText(myLan.device_info);
    statusListItem[STATUS_INDEX_CONNECT]->setIcon(QIcon(":/res/png/System_Device.png"));

    {
        listWidget->addItem(statusListItem[STATUS_INDEX_INFO]);
        listIndex.append(STATUS_INDEX_INFO);
        stackedWidget->addWidget(pageGeneral);
        listWidget->addItem(statusListItem[STATUS_INDEX_STATISTIC]);
        listIndex.append(STATUS_INDEX_STATISTIC);
        stackedWidget->addWidget(pageStatistic);//数据中心
        listWidget->addItem(statusListItem[STATUS_INDEX_ALARM]);
        listIndex.append(STATUS_INDEX_ALARM);
        stackedWidget->addWidget(pageAlarm);
        listWidget->addItem(statusListItem[STATUS_INDEX_VERSION]);
        listIndex.append(STATUS_INDEX_VERSION);
        stackedWidget->addWidget(pageSoftwareVersion);
//        if (paramDelayCode.ena != 0xAA) {
//            listWidget->addItem(statusListItem[STATUS_INDEX_TIME]);
//            listIndex.append(STATUS_INDEX_TIME);
//            stackedWidget->addWidget(pageSystemTime);
//        }
//        if (struGsh.nAuthenticationLevel != AUTHENTICATION_LEVEL_OPERATOR) {
            listWidget->addItem(statusListItem[STATUS_INDEX_TIME]);
            listIndex.append(STATUS_INDEX_TIME);
            stackedWidget->addWidget(pageSystemTime);
//        }
        listWidget->addItem(statusListItem[STATUS_INDEX_NETWORK]);
        listIndex.append(STATUS_INDEX_NETWORK);
        stackedWidget->addWidget(pageSetNetwork);
        listWidget->addItem(statusListItem[STATUS_INDEX_HELP]);
        listIndex.append(STATUS_INDEX_HELP);
        stackedWidget->addWidget(pageHowTo);
        listWidget->addItem(statusListItem[STATUS_INDEX_COPYRIGHT]);
        listIndex.append(STATUS_INDEX_COPYRIGHT);
        stackedWidget->addWidget(pageInformation);
        listWidget->addItem(statusListItem[STATUS_INDEX_CONNECT]);
        listIndex.append(STATUS_INDEX_CONNECT);
        stackedWidget->addWidget(pageContactUs);
    }

    for(int i =0; i<10; i++){
        statusListItem[i]->setHidden(true);
    }

    statusListItem[STATUS_INDEX_INFO]->setHidden(false);
    statusListItem[STATUS_INDEX_ALARM]->setHidden(false);

    statusListItem[STATUS_INDEX_VERSION]->setHidden(false);
    statusListItem[STATUS_INDEX_NETWORK]->setHidden(false);

    statusListItem[STATUS_INDEX_TIME]->setHidden(false);
    statusListItem[STATUS_INDEX_CONNECT]->setHidden(false);


    // 默认选中第一列
    listWidget->setCurrentRow(0);
}

void StatusInfoPage::setLeftBtnStatus()
{
    if (listWidget->currentRow() == -1)
        return;

    switch(listIndex[listWidget->currentRow()]) {
    case STATUS_INDEX_INFO:
        if (statusTabBar->currentIndex() > STATISTIC_INDEX_STATUS
        && statusTabBar->currentIndex() < STATISTIC_INDEX_CONFIG) {
            leftBtn->show();
        } else {
            leftBtn->hide();
        }
        break;
    case STATUS_INDEX_VERSION:
        leftBtn->show();
        break;
    case STATUS_INDEX_AUTO:
        leftBtn->setText(myLan.auto_calibrate);
        leftBtn->show();
        break;
    case STATUS_INDEX_HELP:
        leftBtn->show();
        break;
    case STATUS_INDEX_NETWORK:
        leftBtn->hide();
        break;
    case STATUS_INDEX_CONNECT:
        leftBtn->hide();
        break;
    default:
        leftBtn->hide();
        break;
    }
}

/**
 *  返回主界面
 */
void StatusInfoPage::onCancelBtnClickedSlt()
{
//    webview->stop();
    timer->stop();
    close();
    emit goToHomePageSig();
}

/*!
 * \brief StatusInfoPage::onListWidgetRowChangedSlt
 * \param nIndex
 */
void StatusInfoPage::onListWidgetRowChangedSlt(int nIndex)
{
    //! 设置左侧按钮状态
    setLeftBtnStatus();

    //! 切换页面
    stackedWidget->setCurrentIndex(nIndex);
}

void StatusInfoPage::onStatusTabChanged(int index)
{
    generalInfoGroup->hide();
//    pageLogger->hide();
//    webview->hide();
    cameraRefBox->hide();
    timeStatisticGroup->hide();
    statisticEnable->hide();
    serverGroup->hide();
    leftBtn->hide();

    switch(index) {
    case STATISTIC_INDEX_STATUS:
        generalInfoGroup->show();
        break;
    case STATISTIC_INDEX_LOG:
//        pageLogger->show();
        leftBtn->show();
        break;
    case STATISTIC_INDEX_BACKGROUND:
//        webview->show();
        leftBtn->show();
        break;
    case STATISTIC_INDEX_IDENTIFY:
//        webview->show();
        leftBtn->show();
        break;
    case STATISTIC_INDEX_HISTOGRAM:
//        webview->show();
        leftBtn->show();
        break;
    case STATISTIC_INDEX_EJECT_TIMES:
//        webview->show();
        leftBtn->show();
        break;
    case STATISTIC_INDEX_CONFIG:
        statisticEnable->show();
        timeStatisticGroup->show();
        serverGroup->show();
        break;
    default:
        break;
    }
    if ((index >= 2) && (index != statusTabBar->count()-1)) {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        updateStatisticPage(index);
        infoWidget->hide();
    }
}

void StatusInfoPage::onCameraHRefChangedSlt(int num)
{
    int tmp = struCnfp.nCameraRef;

    if (tmp%2 == 1) {
        tmp -= 1;
    }

    struCnfp.nCameraRef = tmp + ((num-1)%2);

    myLog->debug("ref:%d",struCnfp.nCameraRef);
}

void StatusInfoPage::onCameraVRefChangedSlt(int num)
{
    struCnfp.nCameraRef = (num-1)*2;

    myLog->debug("ref:%d",struCnfp.nCameraRef);
}

void StatusInfoPage::onTimeHistogramStaisticLineEditPressed()
{
    myInputPanel *inputDlg = new myInputPanel(intType,1,180,struCnfp.nHistogramQueryTime);

    inputDlg->setTitle(timeHistogramStaisticLineEdit->text());

    if(inputDlg->exec() == QDialog::Accepted)
    {
        timeHistogramStaisticLineEdit->setText(inputDlg->getText());
        struCnfp.nHistogramQueryTime = inputDlg->getText().toInt();
    }
}

void StatusInfoPage::ontimeMaterialStaisticLineEditPressed()
{
    myInputPanel *inputDlg = new myInputPanel(intType,1,180,struCnfp.nMaterialQueryInterval);

    inputDlg->setTitle(timeMaterialStaisticLineEdit->text());

    if(inputDlg->exec() == QDialog::Accepted)
    {
        timeMaterialStaisticLineEdit->setText(inputDlg->getText());
        struCnfp.nMaterialQueryInterval = inputDlg->getText().toInt();
    }
}

void StatusInfoPage::onServerLineEditPressed()
{
    QString server;
    myInputMethod inputDlg(myLan.server, myString.sServer);
    if (inputDlg.exec() == QDialog::Accepted) {
        // get input value
        server = inputDlg.getText();
        serverLineEdit->setText(server);

        // save to global param
        myString.sServer = server;
        // save to config file
        myFlow.saveServerSetting();
    }
}

void StatusInfoPage::onLeftBtnClickedSlt()
{
    if (listWidget->currentRow() == -1)
        return;

    switch(listIndex[listWidget->currentRow()]) {
    case STATUS_INDEX_INFO:
	updateStatisticPage(statusTabBar->currentIndex());
        break;
    case STATUS_INDEX_VERSION:
        infoWidget->setLabelText(myLan.msg_require_version);
        infoWidget->delayShow();
        myFlow.initCommunication();
        myFlow.initAiCommunication();
        infoWidget->delayHide();
        emit updateVersionSig();
        break;
    case STATUS_INDEX_AUTO:
        autoCalibrateSignal();
        break;
    case STATUS_INDEX_COPYRIGHT:
        getHowToHelp();
        break;
    case STATUS_INDEX_NETWORK:
	break;
    case STATUS_INDEX_HELP:
        reloadHowTo();
        break;
    default:
        break;
    }
}

/**********************************************************************************/
/**
 *  创建一般状态页面
 */
void StatusInfoPage::CreateGeneralPage()
{
    QString str = "";

    //! 本次运行时间
    thisRunLabel = new myLabel(str);
    thisRunLabel->setMaximumHeight(30);

    //! 累计运行时间
    totalRunLabel = new myLabel(str);
    totalRunLabel->setMaximumHeight(30);

    //! 本次加工时间
    thisFeedRunLabel = new myLabel(str);
    thisFeedRunLabel->setMaximumHeight(30);
    
    //! 累计加工时间
    totalFeedRunLabel = new myLabel(str);
    totalFeedRunLabel->setMaximumHeight(30);

    //! 本次下料气压报警次数
    m_thisPressureAlarmLabel = new myLabel(str);
    m_thisPressureAlarmLabel->setMaximumHeight(30);

    //! 累计气压报警次数
    m_totalPressAlarmLabel = new myLabel(str);
    m_totalPressAlarmLabel->setMaximumHeight(30);

    //! 供料状态
    feedLabel = new myLabel(str);
    feedLabel->setMaximumHeight(30);

    //! 喷阀状态
    ejectLabel = new myLabel(str);
    ejectLabel->setMaximumHeight(30);

    //! 气压报警
    pressLabel = new myLabel(str);
    pressLabel->setMaximumHeight(30);

    //! 风机报警
    blowerLabel = new myLabel(str);
    blowerLabel->setMaximumHeight(30);

    //! 灯报警
    lampLabel = new myLabel(str);
    lampLabel->setMaximumHeight(30);

    //! 皮带状态
    beltStatus[0] = new myLabel(str);
    beltStatus[0]->setMaximumHeight(30);

    beltStatus[1]  =  new myLabel(str);
    beltStatus[1]->setMaximumHeight(30);

    //! 料位报警
    m_feedAlarmLabel = new myLabel(str);
    m_feedAlarmLabel->setMaximumHeight(30);

    //! 通信报警
    m_commAlarmLabel = new myLabel(str);
    m_commAlarmLabel->setMaximumHeight(30);

    //! 温度监控
    m_temperatureLabel = new myLabel(str);
    m_temperatureLabel->setMaximumHeight(30);

    //! 从设备通信报警
    m_slaveCommAlarmLabel = new myLabel(str);
    m_slaveCommAlarmLabel->setMaximumHeight(30);

    generalInfoGroup = new myGroupBox(myLan.state_info);
    generalVBLayout = new QVBoxLayout(generalInfoGroup);
    generalVBLayout->setSpacing(10);
    verticalSpacer_1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    generalVBLayout->addWidget(thisRunLabel);
    generalVBLayout->addWidget(totalRunLabel);
    generalVBLayout->addWidget(thisFeedRunLabel);
    generalVBLayout->addWidget(totalFeedRunLabel);
    generalVBLayout->addWidget(m_thisPressureAlarmLabel);
    generalVBLayout->addWidget(m_totalPressAlarmLabel);
    generalVBLayout->addWidget(feedLabel);
    generalVBLayout->addWidget(ejectLabel);
    generalVBLayout->addWidget(pressLabel);
    generalVBLayout->addWidget(blowerLabel);
    generalVBLayout->addWidget(m_feedAlarmLabel);
    generalVBLayout->addWidget(lampLabel);
    generalVBLayout->addWidget(beltStatus[0]);
    generalVBLayout->addWidget(beltStatus[1]);
    generalVBLayout->addWidget(m_commAlarmLabel);
    generalVBLayout->addWidget(m_temperatureLabel);
    generalVBLayout->addWidget(m_slaveCommAlarmLabel);
    generalVBLayout->addItem(verticalSpacer_1);

    // 统计信息展示
//    webview = new QWebView;
//    webview->load(QUrl("qrc:/res/html/statistic.html"));

    /* 构建参考相机的布局 */
    statisticEnable = new MyCheckBox(myLan.value_added, struCnfg.nStatisticEnable);
    statisticEnable->setMaximumWidth(BTN_WIDTH*2);

    cameraVRef    = new myValueBar(1,10,1,5,1);  // 相机行
    cameraHRef    = new myValueBar(1,4,1,2,1);   // 相机列

    cameraVRef->hideFastChangeBtns();
    cameraVRef->setValue(struCnfp.nCameraRef/2+1);
    cameraHRef->hide();

    cameraRefBox   = new myGroupBox(myLan.chute);
    QHBoxLayout *cameraVRefHLayout = new QHBoxLayout(cameraRefBox);
    cameraVRefHLayout->addWidget(cameraVRef);

    myLabel *timeMaterialStaisticLabel  = new myLabel(myLan.identify+"(s)");
    myLabel *timeHistogramStaisticLabel = new myLabel(myLan.material+"(s)");
    timeMaterialStaisticLabel->setFont(g_Config::getInstance()->getFont());
    timeHistogramStaisticLabel->setFont(g_Config::getInstance()->getFont());
    timeMaterialStaisticLineEdit  = new myLineEdit(QString("%1").arg(struCnfp.nMaterialQueryInterval));
    timeHistogramStaisticLineEdit = new myLineEdit(QString("%1").arg(struCnfp.nHistogramQueryTime));

    timeStatisticGroup   = new myGroupBox(myLan.time_statistic);
    QGridLayout *timeStatisticLayout = new QGridLayout(timeStatisticGroup);
    timeStatisticLayout->addWidget(timeMaterialStaisticLabel,0,0);
    timeStatisticLayout->addWidget(timeMaterialStaisticLineEdit,0,1);
    timeStatisticLayout->addWidget(timeHistogramStaisticLabel,1,0);
    timeStatisticLayout->addWidget(timeHistogramStaisticLineEdit,1,1);

    myLabel *serverLabel = new myLabel(myLan.server);
    serverLineEdit = new myLineEdit(myString.sServer);
    serverGroup = new myGroupBox(myLan.server);
    QHBoxLayout *serverLayout = new QHBoxLayout(serverGroup);
    serverLayout->addWidget(serverLabel);
    serverLayout->addWidget(serverLineEdit);

    statusTabBar = new MyTabBar();
    if (LCD_HEIGHT == 768) {
        statusTabBar->setStyleSheet("QTabBar::tab{""min-height:50;min-width:100;}");
    } else {
        statusTabBar->setStyleSheet("QTabBar::tab{""min-height:35;min-width:100;}");
    }
    statusTabBar->addTab(myLan.state);
//    statusTabBar->addTab(myLan.log);
//    statusTabBar->addTab(myLan.backgroud);
//    statusTabBar->addTab(myLan.material);
//    statusTabBar->addTab(myLan.identify);
//    statusTabBar->addTab(myLan.eject);
//    statusTabBar->addTab(myLan.configration);

//    pageLogger = new MyLoggerViewer();

    QVBoxLayout *statusVBLayout = new QVBoxLayout(pageGeneral);
    statusVBLayout->addWidget(statusTabBar);
//    statusVBLayout->addWidget(pageLogger);
    statusVBLayout->addWidget(generalInfoGroup);
//    statusVBLayout->addWidget(webview);
    statusVBLayout->addWidget(statisticEnable);
    statusVBLayout->addWidget(cameraRefBox);
    statusVBLayout->addWidget(timeStatisticGroup);
    statusVBLayout->addWidget(serverGroup);

//    pageLogger->hide();
//    webview->hide();
    statisticEnable->hide();
    cameraRefBox->hide();
    timeStatisticGroup->hide();
    serverGroup->hide();

    connect(cameraHRef, SIGNAL(valueChanged(int)), this, SLOT(onCameraHRefChangedSlt(int)));
    connect(cameraVRef, SIGNAL(valueChanged(int)), this, SLOT(onCameraVRefChangedSlt(int)));
    connect(statusTabBar,SIGNAL(currentChanged(int)),this,SLOT(onStatusTabChanged(int)));
    connect(timeHistogramStaisticLineEdit,SIGNAL(pressed()),this,SLOT(onTimeHistogramStaisticLineEditPressed()));
    connect(timeMaterialStaisticLineEdit,SIGNAL(pressed()),this,SLOT(ontimeMaterialStaisticLineEditPressed()));
    connect(serverLineEdit, SIGNAL(pressed()), this, SLOT(onServerLineEditPressed()));
    connect(statisticEnable, SIGNAL(pressed()), this, SLOT(onStatisticEnablePressedSlt()));
}

void StatusInfoPage::updateGeneralPage()
{
    QString str;

    // 本次运行时间
    str = QString("%1: %2:%3:%4").arg(myLan.this_run).arg(struGsh.nCounter/3600).arg((struGsh.nCounter%3600)/60).arg(struGsh.nCounter%60);
    thisRunLabel->setText(str);

    // 累计运行时间
    str = QString("%1: %2:%3:%4").arg(myLan.total_run).arg(struCnfg.nCounter/3600).arg((struCnfg.nCounter%3600)/60).arg(struCnfg.nCounter%60);
    totalRunLabel->setText(str);

    // 本次加工时间
    str = QString("%1: %2:%3:%4").arg(myLan.this_feed_run).arg(struGsh.nFeedCounter/3600).arg((struGsh.nFeedCounter%3600)/60).arg(struGsh.nFeedCounter%60);
    thisFeedRunLabel->setText(str);
    
    // 累计加工时间
    str = QString("%1: %2:%3:%4").arg(myLan.total_feed_run).arg(struCnfg.nFeedCounter/3600).arg((struCnfg.nFeedCounter%3600)/60).arg(struCnfg.nFeedCounter%60);
    totalFeedRunLabel->setText(str);

    // 本次下料气压报警次数
    str = QString("%1: %2").arg(myLan.this_pressure_alarm_times).arg(struGsh.nTotalAlarmPreTimes);
    m_thisPressureAlarmLabel->setText(str);

    // 累计气压报警次数
    str = QString("%1: %2").arg(myLan.total_pressure_alarm).arg(struCnfg.nPressureAlarmCount);
    m_totalPressAlarmLabel->setText(str);

    // 供料状态
    if (struGsh.bStatFeed){
        str = QString("%1: %2").arg(myLan.feed_stat).arg(myLan.on);
        feedLabel->setStyleSheet("color:red");
    } else {
        str = QString("%1: %2").arg(myLan.feed_stat).arg(myLan.off);
        feedLabel->setStyleSheet("color:black");
    }
    feedLabel->setText(str);

    // 喷阀状态
    if (struGsh.bStatEjectFpga){
        str = QString("%1: %2").arg(myLan.eject_stat).arg(myLan.on);
    } else {
        str = QString("%1: %2").arg(myLan.eject_stat).arg(myLan.off);
    }
    ejectLabel->setText(str);

     // 气压报警
     if ((struGsh.bAlarmPressure & 0x02) != 0) {
         str = QString("%1: %2").arg(myLan.pressure_alarm).arg(myLan.abnormal);
         pressLabel->setStyleSheet("color:red");
     } else if ((struGsh.bAlarmPressure & 0x01) != 0){
         str = QString("%1: %2").arg(myLan.pressure_alarm).arg(myLan.pressure_not_enough);
         pressLabel->setStyleSheet("color:red");
     }else {
         str = QString("%1: %2").arg(myLan.pressure_alarm).arg(myLan.normal);
         pressLabel->setStyleSheet("color:black");
     }
    pressLabel->setText(str);

    // 风机报警
    if (struGsh.bAlarmBlower) {
        str = QString("%1: %2").arg(myLan.blower_alarm).arg(myLan.abnormal);
        blowerLabel->setStyleSheet("color:red");
    } else {
        str = QString("%1: %2").arg(myLan.blower_alarm).arg(myLan.normal);
        blowerLabel->setStyleSheet("color:black");
    }
    blowerLabel->setText(str);

    // 灯报警
    if (struGsh.bAlarmLamp) {
        str = QString("%1: %2").arg(myLan.lamp_alarm).arg(myLan.abnormal);
        lampLabel->setStyleSheet("color:red");
    } else {
        str = QString("%1: %2").arg(myLan.lamp_alarm).arg(myLan.normal);
        lampLabel->setStyleSheet("color:black");
    }
    lampLabel->setText(str);

    // 皮带状态
    if (struGsh.bStatBelt[0]) {
        str = QString("%1: %2").arg(myLan.belt_1_status).arg(myLan.on);
        beltStatus[0]->setStyleSheet("color:red");
    } else {
        str = QString("%1: %2").arg(myLan.belt_1_status).arg(myLan.off);
        beltStatus[0]->setStyleSheet("color:black");
    }
    beltStatus[0]->setText(str);

    // 皮带状态
    if (struGsh.bStatBelt[1]) {
        str = QString("%1: %2").arg(myLan.belt_2_status).arg(myLan.on);
        beltStatus[1]->setStyleSheet("color:red");
    } else {
        str = QString("%1: %2").arg(myLan.belt_2_status).arg(myLan.off);
        beltStatus[1]->setStyleSheet("color:black");
    }
    beltStatus[1]->setText(str);

    // 料位报警
    str = "";
    if (struGsh.nAlarmLevel) {
        str = QString("%1:   ").arg(myLan.level_stat);
        for (int i = 0; i < MAX_GROUP_TICK; i++) {
            switch(struGsh.nAlarmLevelGroup[i]) {
            case ALARM_LEVEL_EMPTY:
                str += QString("%1%2  ").arg(myString.sTickGroupName[0][i]).arg(myLan.level_empty_alarm);
                break;
            default:
                break;
            }
        }
        m_feedAlarmLabel->setText(str);
        m_feedAlarmLabel->setStyleSheet("color:red");
        m_feedAlarmLabel->show();
    } else {
        m_feedAlarmLabel->hide();
    }

    /* 通信报警 */
    if (struGsh.nCommStat == 1) {
        str = QString("%1: %2").arg(myLan.communication_alarm).arg(myLan.abnormal);
        m_commAlarmLabel->setText(str);
        m_commAlarmLabel->setStyleSheet("color:red");
        m_commAlarmLabel->show();
    } else {
        m_commAlarmLabel->hide();
    }

    /* 温度监控 */
    str = QString("%1: %2 °C").arg(myLan.temperature).arg(struGsh.nTemperatureMax, 0, 'f', 1);
    m_temperatureLabel->setText(str);
    if (struCnfg.nAlarmEnable[ALARM_ID_TEMPERATURE] == 1 && struGsh.nTemperatureMax >= struCnfg.nTemperatureThreshold) {
        m_temperatureLabel->setStyleSheet("color:red");
    } else {
        m_temperatureLabel->setStyleSheet("color:black");
    }

    /* 显示或屏蔽报警状态 */
    if(struCnfg.nAlarmEnable[ALARM_ID_PRESSURE]) {
        pressLabel->show();
    } else {
        pressLabel->hide();
    }
    if(struCnfg.nAlarmEnable[ALARM_ID_BLOWER]) {
        blowerLabel->show();
    } else {
        blowerLabel->hide();
    }

    if(struCnfg.nAlarmEnable[ALARM_ID_LAMP]) {
        lampLabel->show();
    } else {
        lampLabel->hide();
    }

    if(struCnfg.nAlarmEnable[ALARM_ID_BELT]) {
        beltStatus[0]->show();
        beltStatus[1]->hide();
    } else {
        beltStatus[0]->hide();
        beltStatus[1]->hide();
    }

    m_temperatureLabel->hide();

    if (struCnfg.nAlarmEnable[ALARM_ID_SLAVE_COMM]) {
        m_slaveCommAlarmLabel->show();
        if (struGsh.bAlarmSlaveComm == 1) {
            m_slaveCommAlarmLabel->setText(QString("%1: %2").arg(myLan.communication_alarm).arg(myLan.abnormal));
            m_slaveCommAlarmLabel->setStyleSheet("color:red");
        } else {
            m_slaveCommAlarmLabel->setText(QString("%1: %2").arg(myLan.communication_alarm).arg(myLan.normal));
            m_slaveCommAlarmLabel->setStyleSheet("color:black");
        }
    } else {
        m_slaveCommAlarmLabel->hide();
    }
}

/**********************************************************************************/
/**
 *  创建设置报警页面
 */
void StatusInfoPage::CreateSetAlarmPage()
{
    /* 页面布局 */
    alarmSignalMapper = new QSignalMapper(pageAlarm);
    QVBoxLayout *alarmPageVBLayout = new QVBoxLayout(pageAlarm);

    for (int i = 0; i < MAX_ALARM; i++) {
        alarmEnableBtn[i] = new myCustomCheckBox(getAlarmName(i), false);
        // 温度报警时添加阈值设定
        if (i == ALARM_ID_TEMPERATURE) {
            temperatureThreshold = new myLineEdit("0 °C");
            temperatureThreshold->setMaximumHeight(BTN_HEIGHT);
            connect(temperatureThreshold, SIGNAL(pressed()), this, SLOT(setTemperatureThreshold()));

            QHBoxLayout *alarmTemperature = new QHBoxLayout;
            alarmTemperature->addWidget(alarmEnableBtn[i]);
            alarmTemperature->addWidget(temperatureThreshold);
            alarmPageVBLayout->addLayout(alarmTemperature);
        } else {
            alarmPageVBLayout->addWidget(alarmEnableBtn[i]);
        }
        alarmSignalMapper->setMapping(alarmEnableBtn[i], i);
        connect(alarmEnableBtn[i], SIGNAL(pressed()), alarmSignalMapper, SLOT(map()));
    }

    /* 槽函数 */
    connect(alarmSignalMapper, SIGNAL(mapped(int)), this, SLOT(getAlarmIndex(int)));
}

/*!
 * \brief StatusInfoPage::getAlarmList
 *        获取报警使能列表
 * \return
 */
QList<int> StatusInfoPage::getAlarmList()
{
    QList<int> list;

    // 默认报警列表
    list << ALARM_ID_PRESSURE << ALARM_ID_BLOWER;

    return list;
}

/*!
 * \brief StatusInfoPage::getAlarmName
 *        获取报警使能文本
 * \param type
 * \return
 */
QString StatusInfoPage::getAlarmName(int type)
{
    QString text = myLan.enable;
    switch (type) {
    case ALARM_ID_PRESSURE:
        text = myLan.enable_press_alarm;
        break;
    case ALARM_ID_BLOWER:
        text = myLan.enable_blower_alarm;
        break;
    case ALARM_ID_LAMP:
        text = myLan.enable_lamp_alarm;
        break;
    case ALARM_ID_BELT:
        text = myLan.enable_belt_status;
        break;
    case ALARM_ID_TEMPERATURE:
        text = myLan.enable_temperature_alarm;
        break;
    case ALARM_ID_SLAVE_COMM:
        text = myLan.enable_communication_alarm;
        break;
    default:
        break;
    }
    return text;
}

/*!
 * \brief StatusInfoPage::setTemperatureThreshold
 *        温度报警阈值设定
 */
void StatusInfoPage::setTemperatureThreshold()
{
    myInputPanel inputDlg(intType, 0, 200, struCnfg.nTemperatureThreshold);

    if (inputDlg.exec() == QDialog::Accepted) {
	struCnfg.nTemperatureThreshold = inputDlg.getText().toInt();
	QString text = QString("%1 °C").arg(inputDlg.getText());
	temperatureThreshold->setText(text);
    }
}

/*!
 * \brief StatusInfoPage::updateSetAlarmPageSlt
 *        刷新报警设置页面
 */
void StatusInfoPage::updateSetAlarmPageSlt()
{
    // 默认隐藏所有报警使能
    for (int i = 0; i < MAX_ALARM; i++) {
        alarmEnableBtn[i]->hide();
    }

    // 默认隐藏所有报警阈值
    temperatureThreshold->setText(QString("%1 °C").arg(struCnfg.nTemperatureThreshold));
    temperatureThreshold->hide();

    // 根据机型放开报警使能
    QList<int> list = getAlarmList();
    for (int i = 0; i < list.size(); i++) {
        int index = list[i];
        alarmEnableBtn[index]->show();

        // 更新当前报警使能勾选状态
        if (struCnfg.nAlarmEnable[index]) {
            alarmEnableBtn[index]->setChecked(true);
            if (index == ALARM_ID_TEMPERATURE) {
                temperatureThreshold->show();
            }
        } else {
            alarmEnableBtn[index]->setChecked(false);
            if (index == ALARM_ID_TEMPERATURE) {
                temperatureThreshold->hide();
            }
        }
    }
}

/*!
 * \brief StatusInfoPage::getAlarmIndex
 *        响应报警使能按钮设置
 * \param index
 */
void StatusInfoPage::getAlarmIndex(int index)
{
    //取消气压报警需要输入密码
    if(struCnfg.nAlarmEnable[index]){
        myInputPanel inputDlg(passwdType,0,0,0);
        int ret  = inputDlg.exec();
        if (ret == QDialog::Accepted) {
            QString str = inputDlg.getText();
            if (str == "002690") {
                struCnfg.nAlarmEnable[index] = !struCnfg.nAlarmEnable[index];
                alarmEnableBtn[index]->setChecked(struCnfg.nAlarmEnable[index]);
                myLog->info(LOG_WARM,getAlarmName(index) + myLan.off);
            }else{
                alarmEnableBtn[index]->setChecked(struCnfg.nAlarmEnable[index]);
                infoWidget->setLabelText(myLan.msg_password_error);
                infoWidget->showSecs();
            }
        } else {
            alarmEnableBtn[index]->setChecked(struCnfg.nAlarmEnable[index]);
        }
    }else{
        struCnfg.nAlarmEnable[index] = !struCnfg.nAlarmEnable[index];
        if (struCnfg.nAlarmEnable[index]) {
            alarmEnableBtn[index]->setChecked(true);
            if (index == ALARM_ID_TEMPERATURE) {
                temperatureThreshold->show();
            }
        } else {
            alarmEnableBtn[index]->setChecked(false);
            if (index == ALARM_ID_TEMPERATURE) {
                temperatureThreshold->hide();
            }
        }
        myLog->info(LOG_WARM,getAlarmName(index) + myLan.on);
    }
    printf("index:%d    value:%d\n",index,struCnfg.nAlarmEnable[index]);
    fflush(0);
}

/**********************************************************************************/
/**
 *  创建系统时间页面
 */
void StatusInfoPage::CreateSystemTimePage()
{
    gridLayout = new QGridLayout(pageSystemTime);
    gridLayout->setHorizontalSpacing(50);
    gridLayout->setContentsMargins(20, 90, 40, 90);

    timeSignalMapper = new QSignalMapper(pageSystemTime);

    for (int i = 0; i < 6;i++) {
        dateLabel[i] = new myLabel("", pageSystemTime);
        dateLbe[i] = new myLineEdit("", pageSystemTime);
        switch(i){
        case 0:
            dateLabel[i]->setText(myLan.year);
            break;
        case 1:
            dateLabel[i]->setText(myLan.month);
            break;
        case 2:
            dateLabel[i]->setText(myLan.day);
            break;
        case 3:
            dateLabel[i]->setText(myLan.hour);
            break;
        case 4:
            dateLabel[i]->setText(myLan.minute);
            break;
        case 5:
            dateLabel[i]->setText(myLan.sec);
            break;
        default:
            break;
        }

        dateLbe[i]->setReadOnly(true);
        dateLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (i < 3) {
            gridLayout->addWidget(dateLabel[i],i/3  ,i%3,1,1,Qt::AlignCenter);
            gridLayout->addWidget(dateLbe[i]  ,i/3+1,i%3,1,1);
        } else {
            gridLayout->addWidget(dateLabel[i],i/3+1,i%3,1,1,Qt::AlignCenter);
            gridLayout->addWidget(dateLbe[i]  ,i/3+2,i%3,1,1);
        }

        timeSignalMapper->setMapping(dateLbe[i], i);
        connect(dateLbe[i], SIGNAL(pressed()), timeSignalMapper, SLOT(map()));
    }

    updateDateTime();
    connect(timeSignalMapper, SIGNAL(mapped(int)), this, SLOT(getTimeIndex(int)));
}

void StatusInfoPage::updateDateTime()
{
    QString str;
    QDateTime dateTime = QDateTime::currentDateTime();

    data[0] = dateTime.date().year();
    data[1] = dateTime.date().month();
    data[2] = dateTime.date().day();
    data[3] = dateTime.time().hour();
    data[4] = dateTime.time().minute();
    data[5] = dateTime.time().second();

    for (int i = 0; i < 6;i++) {
        str = QString("%1").arg(data[i]);
        dateLbe[i]->setText(str);
    }
}

void StatusInfoPage::setDateTime(int y, int m, int d, int h, int min, int sec)
{
#ifdef Q_OS_UNIX
    char time[64];
    sprintf(time, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"", y, m, d, h, min, sec);
    system(time);
    popen("hwclock -w","r");
#endif
}

void StatusInfoPage::getTimeIndex(int index)
{
    int ret;
    QString str;
    int nMin,nMax;
    switch(index)
    {
    case 0://year
        nMin = 1;
        nMax = 2049;
        break;
    case 1://month
        nMin = 1;
        nMax = 12;
        break;
    case 2://day
        nMin = 1;
        nMax = 31;
        break;
    case 3://hour
        nMin = 0;
        nMax = 23;
        break;
    case 4://mintue
        nMin = 0;
        nMax = 59;
        break;
    case 5://second
        nMin = 0;
        nMax = 59;
        break;
    }

    myInputPanel inputDlg(intType,nMin,nMax,data[index]);
    ret  = inputDlg.exec();

    if (ret == QDialog::Accepted)
     {
        data[index] = inputDlg.getValue();
        str = QString("%1").arg(data[index]);
        dateLbe[index]->setText(str);
        setDateTime(data[0],data[1],data[2],data[3],data[4],data[5]);
    }
}
/**********************************************************************************/
/**
 *  创建软件版本页面
 */
void StatusInfoPage::CreateSoftwareVersionPage()
{
    int i, j;
    QString infoStr;

    /*布局管理*/
    versionListGridLayout = new QGridLayout(pageSoftwareVersion);
    versionListGridLayout->setSpacing(10);

    /* 屏 */
    screenListGroup = new myGroupBox(myLan.screen);
    screenListLayout = new QGridLayout(screenListGroup);
    screenListLabel = new QLabel;
    screenListLabel->setAlignment(Qt::AlignCenter);
    screenListLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    screenListLayout->addWidget(screenListLabel);
//    screenListLabel->setText(QString("%1_%2").arg(PLATFORM_NAME).arg(myString.sAppVersion));
    screenListLabel->setText(QString("%1").arg("V1.00"));


    /* 接口板 */
    interfaceBoardListGroup = new myGroupBox(myLan.interface_board);
    interfaceListLayout = new QGridLayout(interfaceBoardListGroup);

    /* 控制板 */
    ctrlBoardListGroup = new myGroupBox(myLan.control_board);
    controlListLayout = new QGridLayout(ctrlBoardListGroup);

    /* 相机板  */
    cameraBoardListGroup = new myGroupBox(myLan.color_board);
    cameraListLayout = new QGridLayout(cameraBoardListGroup);

    cameraBoardListGroup->setStyleSheet("background-color:transparent");
//    cameraBoardListGroup->setFrameStyle(QFrame::NoFrame);
    /* 恒流源板 */
    m_lightSrcGroup = new myGroupBox(myLan.light_src_board);
    m_lightSrcLay = new QGridLayout(m_lightSrcGroup);

    for (i = 0; i<MAX_LIGHT_SOURCE; i++) {
        for (j = 0; j < 2; j++) {
            m_lightSrcLabel[i][j] = new QLabel;
            m_lightSrcLabel[i][j]->setAlignment(Qt::AlignCenter);
            m_lightSrcLabel[i][j]->setFont(config->getFont());
            m_lightSrcLay->addWidget(m_lightSrcLabel[i][j], i%2, i/2+i/2+j);
        }
    }

    /* 系统信息 */
    if(struCnfe.nEnableBeanMachine == 1){
        infoStr.sprintf("Version %s_%s_%d-BEAN, Compiled on %s %s",
                PLATFORM_NAME,
                myString.sAppVersion.toLocal8Bit().constData(),
                myString.sAppSVN.toInt(),
                __DATE__, __TIME__);
    }else {
        infoStr.sprintf("Version %s_%s_%d, Compiled on %s %s",
                PLATFORM_NAME,
                myString.sAppVersion.toLocal8Bit().constData(),
                myString.sAppSVN.toInt(),
                __DATE__, __TIME__);
    }
//    infoLabel = new myLabel(infoStr, pageSoftwareVersion);
//    infoLabel->setGeometry(QRect(20, 20, pageSoftwareVersion->width(), 100));

    /* 设置布局管理器的布局 */
    versionListGridLayout->addWidget(screenListGroup, 0, 0, 2, 1);
    versionListGridLayout->addWidget(interfaceBoardListGroup, 2, 0, 3, 1);
    versionListGridLayout->addWidget(ctrlBoardListGroup, 5, 0, 2, 1);
    if(struCnfg.nEnableLampAI == 1){
//            versionListGridLayout->addWidget(m_lightSrcGroup, 7, 0, 3, 3);
//            versionListGridLayout->addWidget(cameraBoardListGroup, 0, 1, 7, 2);
//            versionListGridLayout->addWidget(infoLabel,10,0,1,3,Qt::AlignBottom);
    }else{
//            versionListGridLayout->addWidget(cameraBoardListGroup, 0, 1, 7, 2);
//            versionListGridLayout->addWidget(infoLabel,7,0,1,3,Qt::AlignBottom);
    }
    versionListGridLayout->addWidget(cameraBoardListGroup, 0, 1, 7, 2);

    for(i = 0; i < MAX_INT+1; i++){
        for(j = 0; j < 2; j++){
            interfaceListLabel[i][j] = new QLabel;
            interfaceListLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            interfaceListLabel[i][j]->setAlignment(Qt::AlignCenter);
            interfaceListLayout->addWidget(interfaceListLabel[i][j],i,j);
        }
    }

    for(i = 0; i < MAX_CTRL; i++){
        for(j = 0; j < 2; j++){
            controlListLabel[i][j] = new QLabel;
            controlListLabel[i][j]->setAlignment(Qt::AlignCenter);
            controlListLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            controlListLayout->addWidget(controlListLabel[i][j],i,j);
        }
    }

    /* 2.09及之后版本使用文本编辑框显示版本（针对2014.11.24之后的FPGA定版程序）*/
    m_textEdit = new QTextEdit();
    m_textEdit->setFont(config->getFont());
    m_textEdit->setReadOnly(true);
    m_textEdit->setWordWrapMode(QTextOption::NoWrap);
    m_textEdit->setFrameStyle(QFrame::NoFrame);
    cameraListLayout->addWidget(m_textEdit);

    for(i = 0; i < MAX_UNIT/2+1; i++){
        for(j = 0; j < MAX_BACKGROUND_GROUP+3; j++){
            cameraListLabel[i][j] = new QLabel;
            cameraListLabel[i][j]->setFont(config->getFont(DEFAULT_FONT_SIZE));
            cameraListLabel[i][j]->setAlignment(Qt::AlignHCenter);
        }
    }

    /* 槽函数 */
    connect(this, SIGNAL(updateVersionSig()), this, SLOT(updateVersionResultSlt()));
}


/* 隐藏某一列 */
void StatusInfoPage::hideColumn(int index)
{
    for(int i=0; i<MAX_UNIT/2+1; i++) {
        cameraListLabel[i][index-1]->hide();
    }
}

/* 显示某一列 */
void StatusInfoPage::showColumn(int index)
{
    for(int i=0; i<MAX_UNIT/2+1; i++) {
        cameraListLabel[i][index-1]->show();
    }
}

/***
 *
 * 获取用户模式代号
 * --- 参数说明 ---
 * 1.nVer:大版本号，判断是否为工厂模式
 *
********************/
void StatusInfoPage::getVersionMaterialType(int nVer, int nVal, char *materialType)
{
    /* 用户模式代号显示为数字，直接将数字转化为字符*/
    if (((nVer>>4) & 0xF) == 0xF) {
        sprintf(materialType, "F");
    } else {
        sprintf(materialType, "%d", nVal);
    }
    return;
}

/* 更新控制板版本信息 */
void StatusInfoPage::versionUpdateCtrl()
{
    int i, j;

    for(i = 0; i < MAX_CTRL; i++){
        for(j = 0; j < 2; j++){
            controlListLabel[i][j]->show();
        }
    }
    for(i = 0; i < struCnfg.nCtrlBoardTotal; i++){
        controlListLabel[i][0]->setText(QString("%1").arg(i+1));

        if((struGsh.struVer.sControl[i][0] != 255) && (struGsh.struVer.sControl[i][1] != 255)) {
//              controlListLabel[i][1]->setText(QString("V%1.0%2").arg(struGsh.struVer.sControl[i][1])
//                      .arg(struGsh.struVer.sControl[i][0]));
              controlListLabel[i][1]->setText(QString("V1.00"));
              controlListLabel[i][1]->setStyleSheet("color:black");
        }
        else {
              controlListLabel[i][1]->setText("NULL");
              controlListLabel[i][1]->setStyleSheet("color:red");
        }
    }
    for (i = struCnfg.nCtrlBoardTotal; i < MAX_CTRL; i++) {
        for (j = 0; j < 2; j++) {
            controlListLabel[i][j]->hide();
        }
    }
}

/* 更新接口板版本信息 */
void StatusInfoPage::versionUpdateInterface()
{
    int i, j;
    char materialType[8];
    memset(materialType, 0, sizeof(materialType));

    for (i = 0; i < MAX_INT; i++) {
        for(j = 0; j < 2; j++){
            interfaceListLabel[i][j]->show();
        }
    }

    for (i = 0; i < struCnfg.nInterfaceBoardTotal; i++) {
        interfaceListLabel[i][0]->setText(QString("%1").arg(i+1));
        if((struGsh.struVer.sInterface[i][0] != 255) && (struGsh.struVer.sInterface[i][1] != 255)) {
            getVersionMaterialType(struGsh.struVer.sInterface[i][1],
                                struGsh.struVer.sInterface[i][2],
                                materialType);
            QString str;
//                str.sprintf("50%02d%02d%02d_V%d.%02d_%s",
//                        struGsh.struVer.sInterface[i][5],
//                        struGsh.struVer.sInterface[i][4],
//                        struGsh.struVer.sInterface[i][3],
//                        struGsh.struVer.sInterface[i][1]&0x0F,
//                        struGsh.struVer.sInterface[i][0],
//                        materialType);
//                interfaceListLabel[i][1]->setText(str);
            interfaceListLabel[i][1]->setText(QString("V1.00"));
            interfaceListLabel[i][1]->setStyleSheet("color:black");
        } else {
            interfaceListLabel[i][1]->setText("NULL");
            interfaceListLabel[i][1]->setStyleSheet("color:red");
        }
    }

    for (i = struCnfg.nInterfaceBoardTotal; i < MAX_INT; i++){
        for(j = 0; j < 2; j++){
            interfaceListLabel[i][j]->hide();
        }
    } 
}

/*!
 * \brief USB固件版本信息
 */
void StatusInfoPage::versionUpdateUSBFirmware()
{
    interfaceListLabel[MAX_INT][0]->setText("USB");
    if (struGsh.struVer.nUsbFirmware == 1) {
        interfaceListLabel[MAX_INT][1]->setText("V1.00");
        interfaceListLabel[MAX_INT][1]->setStyleSheet("color:black");
    } else if (struGsh.struVer.nUsbFirmware == 2) {
        interfaceListLabel[MAX_INT][1]->setText("V2.00");
        interfaceListLabel[MAX_INT][1]->setStyleSheet("color:black");
    } else {
        interfaceListLabel[MAX_INT][1]->setText("NULL");
        interfaceListLabel[MAX_INT][1]->setStyleSheet("color:red");
    }
}
void StatusInfoPage::versionUpdateCameraLD4()
{
    /*
        struGsh.struVer.sUnit[0][0][0] = buf[17];//小版本
        struGsh.struVer.sUnit[0][0][1] = buf[16];//大版本
        struGsh.struVer.sUnit[0][0][2] = buf[15];//用户模式代号
        struGsh.struVer.sUnit[0][0][3] = buf[14];//物料编码7-8
        struGsh.struVer.sUnit[0][0][4] = buf[13];//物料编码5-6
        struGsh.struVer.sUnit[0][0][5] = buf[12];//物料编码3-4
    *   显示：物料编码+大版本+小版本+用户模式
    */

        QString str;
        str.sprintf("%-s\t%-22s\t%-22s\t%-22s\t%s\n",qPrintable(myLan.chute),qPrintable(myLan.upper_master_view),qPrintable(myLan.lower_master_view),
                    qPrintable(myLan.upper_slave_view),qPrintable(myLan.lower_slave_view));

        for (int i = 0; i < (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) / 4; i++) {
            char materialType[8] = "\0";
            QString strChuteVerTmp[4];
            getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2][1],
                    struGsh.struVer.sUnit[ONE_LEVEL][i*2][2],
                    materialType);
            if (struGsh.struVer.sUnit[ONE_LEVEL][i*2][1] == 255) {
                strChuteVerTmp[0].sprintf("%s","NULL                        ");
            } else {
                strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2][5],
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2][4],
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2][3],
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2][1]&0x0F,
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2][0],
                                          materialType);
            }

            getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1],
                    struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][2],
                    materialType);
            if (struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1] == 255) {
                strChuteVerTmp[1].sprintf("%s","NULL                        ");
            } else {
                strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][5],
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][4],
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][3],
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1]&0x0F,
                                          struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][0],
                                          materialType);
            }

            if (i != (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) / 4 - 1) {
                getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][1],
                        struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][2],
                        materialType);
                if (struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL                        ");
                } else {
                    strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][5],
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][4],
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][3],
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][1]&0x0F,
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][1],
                        struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][2],
                        materialType);
                if (struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][1] == 255) {
                    strChuteVerTmp[3].sprintf("%s","NULL                        ");
                } else {
                    strChuteVerTmp[3].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][5],
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][4],
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][3],
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[ONE_LEVEL][i*2+(struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+2)/2+1][0],
                                              materialType);
                }
            }

            QString strTmp;
            strTmp.sprintf("   %d\t%s\t%s\t%s\t%s\n",
                           i+1,//通道从1计数
                           qPrintable(strChuteVerTmp[0]),
                           qPrintable(strChuteVerTmp[1]),
                           qPrintable(strChuteVerTmp[2]),
                           qPrintable(strChuteVerTmp[3]));
            str += strTmp;
        }
        m_textEdit->setText(str);
}
/* 更新RSC机型前置板版本 */
void StatusInfoPage::versionUpdateCameraRSC()
{
    QString tmpStr, frontStr, rearStr, assistFrontStr, assistRearStr;
    QString str;

    /* str1:电路板类型  str2:物料号高8位  str3:物料号低8位  str4:小版本号*/
    char str1[2], str2[2], str3[2], str4[2];

    //! 辅配前置板
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;

    if(struCnfg.bAssitCameraEnable)
    {
        str = QString("%1\t%2\t\t%3\t\t%4  %5\n").arg(myLan.chute).arg(myLan.front_view).arg(myLan.rear_view).arg(myLan.front_view+myLan.config_assist).arg(myLan.rear_view+myLan.config_assist);
    }
    else{
        str = QString("%1\t\t%2\t\t%3\n").arg(myLan.chute).arg(myLan.front_view).arg(myLan.rear_view);
    }
    for (int i = 0; i < nUnitTotal/2; i++) {
        char materialType[8] = "\0";
        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i*2][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i*2][0] == 255
                        || struGsh.struVer.sUnit[ONE_LEVEL][i*2][1] == 255) {
            frontStr = "NULL";
        } else {
            sprintf(str1, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2][5]);
            sprintf(str2, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2][4]);
            sprintf(str3, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2][3]);
            sprintf(str4, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2][0]);
            frontStr = QString("50%1%2%3_V%4.%5_%6")
                        .arg(str1)
                        .arg(str2)
                        .arg(str3)
                        .arg(struGsh.struVer.sUnit[ONE_LEVEL][i*2][1] & 0x0F)
                        .arg(str4)
                        .arg(materialType);
        }
        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][0] == 255
                        || struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1] == 255) {
            rearStr = "\tNULL";
        } else {
            sprintf(str1, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][5]);
            sprintf(str2, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][4]);
            sprintf(str3, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][3]);
            sprintf(str4, "%02d", struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][0]);
            rearStr = QString("50%1%2%3_V%4.%5_%6")
                        .arg(str1)
                        .arg(str2)
                        .arg(str3)
                        .arg(struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1]&0x0F)
                        .arg(str4)
                        .arg(materialType);
        }

        assistFrontStr = "--\t ";

        assistRearStr = "  --";
        if(struCnfg.bAssitCameraEnable)
        {
            tmpStr = QString("%1\t%2\t%3\t\t%4   %5\n").arg(i+1).arg(frontStr).arg(rearStr).arg(assistFrontStr).arg(assistRearStr);
        }else{
            tmpStr = QString("%1\t\t%2\t%3\n").arg(i+1).arg(frontStr).arg(rearStr);
        }
        str += tmpStr;
    }
    m_textEdit->setText(str);
}

/* 更新TD机型前置板版本 */
void StatusInfoPage::versionUpdateCameraTD()
{
    QString str;

    if (struCnfg.nLevelTotal == 3) {
        if (struCnfg.struLevelInfo[0].nViewTotal == 1 &&struCnfg.struLevelInfo[1].nViewTotal == 1
                && struCnfg.struLevelInfo[2].nViewTotal == 1) {
            str.sprintf("%-10s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                      qPrintable(myLan.mid_layer),qPrintable(myLan.lower_layer)); // 通道 上层/中层/下层
            for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++) {
                char materialType[8] = "\0";
                QString strChuteVerTmp[3];// 上层、中层、下层

                for (int j = 0; j < struCnfg.nLevelTotal; j++) {
                    getVersionMaterialType(struGsh.struVer.sUnit[j][i][1],
                          struGsh.struVer.sUnit[j][i][2],
                          materialType);
                    if (struGsh.struVer.sUnit[j][i][1] == 255) {
                        strChuteVerTmp[j].sprintf("%s","NULL");
                    } else {
                        strChuteVerTmp[j].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[j][i][5],
                                                struGsh.struVer.sUnit[j][i][4],
                                                struGsh.struVer.sUnit[j][i][3],
                                                struGsh.struVer.sUnit[j][i][1]&0x0F,
                                                struGsh.struVer.sUnit[j][i][0],
                                                materialType);
                    }
                }

                QString strTmp;
                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%s  \n",
                             i+1,//通道从1计数
                             qPrintable(strChuteVerTmp[0]),
                             qPrintable(strChuteVerTmp[1]),
                             qPrintable(strChuteVerTmp[2]));

                str += strTmp;
            }
        }

        if (struCnfg.struLevelInfo[0].nViewTotal == 2 &&struCnfg.struLevelInfo[1].nViewTotal == 1
                && struCnfg.struLevelInfo[2].nViewTotal == 1) {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper + "-" +myLan.front),
                        qPrintable(myLan.upper + "-" + myLan.rear),qPrintable(myLan.mid_layer),qPrintable(myLan.lower_layer)); // 通道 上前/上后/中层/下层
            char materialType[8] = "\0";
            QString strChuteVerTmp[4];//上前/上后/中层/下层

            for (int i = 0;  i< struCnfg.struLevelInfo[1].nUnitLevelTotal; i++) {
                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2][1],
                          struGsh.struVer.sUnit[0][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                } else {
                    strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2][5],
                                              struGsh.struVer.sUnit[0][i*2][4],
                                              struGsh.struVer.sUnit[0][i*2][3],
                                              struGsh.struVer.sUnit[0][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2+1][1],
                          struGsh.struVer.sUnit[0][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2+1][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2+1][5],
                                              struGsh.struVer.sUnit[0][i*2+1][4],
                                              struGsh.struVer.sUnit[0][i*2+1][3],
                                              struGsh.struVer.sUnit[0][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2+1][0],
                                              materialType);
                }

                for (int j = 1; j < struCnfg.nLevelTotal; j++) {
                    getVersionMaterialType(struGsh.struVer.sUnit[j][i][1],
                           struGsh.struVer.sUnit[j][i][2],
                           materialType);
                    if (struGsh.struVer.sUnit[j][i][1] == 255) {
                         strChuteVerTmp[j+1].sprintf("%s","NULL");
                    } else {
                        strChuteVerTmp[j+1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                     struGsh.struVer.sUnit[j][i][5],
                                                     struGsh.struVer.sUnit[j][i][4],
                                                     struGsh.struVer.sUnit[j][i][3],
                                                     struGsh.struVer.sUnit[j][i][1]&0x0F,
                                                     struGsh.struVer.sUnit[j][i][0],
                                                     materialType);
                    }
                }

                QString strTmp;
                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]),
                               qPrintable(strChuteVerTmp[3]));

                str += strTmp;

            }
        }

        if (struCnfg.struLevelInfo[0].nViewTotal == 1 &&struCnfg.struLevelInfo[1].nViewTotal == 2
                && struCnfg.struLevelInfo[2].nViewTotal == 1) {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                        qPrintable(myLan.mid + "-" + myLan.front),qPrintable(myLan.mid + "-" + myLan.rear),
                        qPrintable(myLan.lower_layer)); // 通道 上层/中前/中后/下层
            char materialType[8] = "\0";
            QString strChuteVerTmp[4];//上层/中前/中后/下层
            for(int i = 0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2][1],
                          struGsh.struVer.sUnit[1][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                } else {
                    strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2][5],
                                              struGsh.struVer.sUnit[1][i*2][4],
                                              struGsh.struVer.sUnit[1][i*2][3],
                                              struGsh.struVer.sUnit[1][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2+1][1],
                          struGsh.struVer.sUnit[1][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2+1][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2+1][5],
                                              struGsh.struVer.sUnit[1][i*2+1][4],
                                              struGsh.struVer.sUnit[1][i*2+1][3],
                                              struGsh.struVer.sUnit[1][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2+1][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i][1],
                          struGsh.struVer.sUnit[0][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[0][i][5],
                                                struGsh.struVer.sUnit[0][i][4],
                                                struGsh.struVer.sUnit[0][i][3],
                                                struGsh.struVer.sUnit[0][i][1]&0x0F,
                                                struGsh.struVer.sUnit[0][i][0],
                                                materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i][1],
                          struGsh.struVer.sUnit[2][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i][1] == 255) {
                    strChuteVerTmp[3].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[3].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[2][i][5],
                                                struGsh.struVer.sUnit[2][i][4],
                                                struGsh.struVer.sUnit[2][i][3],
                                                struGsh.struVer.sUnit[2][i][1]&0x0F,
                                                struGsh.struVer.sUnit[2][i][0],
                                                materialType);
                }

                QString strTmp;
                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]),
                               qPrintable(strChuteVerTmp[3]));

                str += strTmp;
            }
        }

        if (struCnfg.struLevelInfo[0].nViewTotal == 1 &&struCnfg.struLevelInfo[1].nViewTotal == 1
                && struCnfg.struLevelInfo[2].nViewTotal == 2) {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                        qPrintable(myLan.mid_layer),qPrintable(myLan.lower + "-" + myLan.front),
                        qPrintable(myLan.lower + "-" + myLan.rear)); // 通道 上层/中层/下前/下后
            char materialType[8] = "\0";
            QString strChuteVerTmp[4];//上层/中层/下前/下后
            for(int i = 0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[2][i*2][1],
                          struGsh.struVer.sUnit[2][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i*2][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL");
                } else {
                    strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[2][i*2][5],
                                              struGsh.struVer.sUnit[2][i*2][4],
                                              struGsh.struVer.sUnit[2][i*2][3],
                                              struGsh.struVer.sUnit[2][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[2][i*2][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i*2+1][1],
                          struGsh.struVer.sUnit[2][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i*2+1][1] == 255) {
                    strChuteVerTmp[3].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[3].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[2][i*2+1][5],
                                              struGsh.struVer.sUnit[2][i*2+1][4],
                                              struGsh.struVer.sUnit[2][i*2+1][3],
                                              struGsh.struVer.sUnit[2][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[2][i*2+1][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i][1],
                          struGsh.struVer.sUnit[0][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[0][i][5],
                                                struGsh.struVer.sUnit[0][i][4],
                                                struGsh.struVer.sUnit[0][i][3],
                                                struGsh.struVer.sUnit[0][i][1]&0x0F,
                                                struGsh.struVer.sUnit[0][i][0],
                                                materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i][1],
                          struGsh.struVer.sUnit[1][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[1][i][5],
                                                struGsh.struVer.sUnit[1][i][4],
                                                struGsh.struVer.sUnit[1][i][3],
                                                struGsh.struVer.sUnit[1][i][1]&0x0F,
                                                struGsh.struVer.sUnit[1][i][0],
                                                materialType);
                }

                QString strTmp;
                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]),
                               qPrintable(strChuteVerTmp[3]));

                str += strTmp;
            }
        }

        if (struCnfg.struLevelInfo[0].nViewTotal == 2 &&struCnfg.struLevelInfo[1].nViewTotal == 2
                && struCnfg.struLevelInfo[2].nViewTotal == 1) {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper + "-" +myLan.front),
                        qPrintable(myLan.upper + "-" +myLan.rear),qPrintable(myLan.mid + '-' + myLan.front),
                        qPrintable(myLan.mid + "-" + myLan.rear), qPrintable(myLan.lower_layer)); // 通道 上前/上后/中前/中后/下层
            char materialType[8] = "\0";
            QString strChuteVerTmp[5];//上前/上后/中前/中后/下层
            for(int i = 0; i<struCnfg.struLevelInfo[2].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2][1],
                          struGsh.struVer.sUnit[0][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                } else {
                    strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2][5],
                                              struGsh.struVer.sUnit[0][i*2][4],
                                              struGsh.struVer.sUnit[0][i*2][3],
                                              struGsh.struVer.sUnit[0][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2+1][1],
                          struGsh.struVer.sUnit[0][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2+1][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                } else {
                   strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2+1][5],
                                              struGsh.struVer.sUnit[0][i*2+1][4],
                                              struGsh.struVer.sUnit[0][i*2+1][3],
                                              struGsh.struVer.sUnit[0][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2+1][0],
                                              materialType);
                }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2][1],
                          struGsh.struVer.sUnit[1][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2][5],
                                              struGsh.struVer.sUnit[1][i*2][4],
                                              struGsh.struVer.sUnit[1][i*2][3],
                                              struGsh.struVer.sUnit[1][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2+1][1],
                          struGsh.struVer.sUnit[1][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2+1][1] == 255) {
                    strChuteVerTmp[3].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[3].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2+1][5],
                                              struGsh.struVer.sUnit[1][i*2+1][4],
                                              struGsh.struVer.sUnit[1][i*2+1][3],
                                              struGsh.struVer.sUnit[1][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2+1][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i][1],
                          struGsh.struVer.sUnit[2][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i][1] == 255) {
                    strChuteVerTmp[4].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[4].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[2][i][5],
                                                struGsh.struVer.sUnit[2][i][4],
                                                struGsh.struVer.sUnit[2][i][3],
                                                struGsh.struVer.sUnit[2][i][1]&0x0F,
                                                struGsh.struVer.sUnit[2][i][0],
                                                materialType);
                     }

                QString strTmp;

                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]),
                               qPrintable(strChuteVerTmp[3]),
                               qPrintable(strChuteVerTmp[4]));

                str += strTmp;

            }

        }

        if(struCnfg.struLevelInfo[0].nViewTotal == 2 &&struCnfg.struLevelInfo[1].nViewTotal == 1
                && struCnfg.struLevelInfo[2].nViewTotal == 2)
        {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper + "-" +myLan.front),
                        qPrintable(myLan.upper + "-" +myLan.rear),qPrintable(myLan.mid_layer),
                        qPrintable(myLan.lower + "-" + myLan.front), qPrintable(myLan.lower + "-" + myLan.rear)); // 通道 上前/上后/中层/下前/下后
            char materialType[8] = "\0";
            QString strChuteVerTmp[5];//上前/上后/中层/下前/下后
            for(int i = 0; i<struCnfg.struLevelInfo[1].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2][1],
                          struGsh.struVer.sUnit[0][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2][5],
                                              struGsh.struVer.sUnit[0][i*2][4],
                                              struGsh.struVer.sUnit[0][i*2][3],
                                              struGsh.struVer.sUnit[0][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2+1][1],
                          struGsh.struVer.sUnit[0][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2+1][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2+1][5],
                                              struGsh.struVer.sUnit[0][i*2+1][4],
                                              struGsh.struVer.sUnit[0][i*2+1][3],
                                              struGsh.struVer.sUnit[0][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2+1][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i*2][1],
                          struGsh.struVer.sUnit[2][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i*2][1] == 255) {
                    strChuteVerTmp[3].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[3].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[2][i*2][5],
                                              struGsh.struVer.sUnit[2][i*2][4],
                                              struGsh.struVer.sUnit[2][i*2][3],
                                              struGsh.struVer.sUnit[2][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[2][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i*2+1][1],
                          struGsh.struVer.sUnit[2][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i*2+1][1] == 255) {
                    strChuteVerTmp[4].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[4].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[2][i*2+1][5],
                                              struGsh.struVer.sUnit[2][i*2+1][4],
                                              struGsh.struVer.sUnit[2][i*2+1][3],
                                              struGsh.struVer.sUnit[2][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[2][i*2+1][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i][1],
                          struGsh.struVer.sUnit[1][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[1][i][5],
                                                struGsh.struVer.sUnit[1][i][4],
                                                struGsh.struVer.sUnit[1][i][3],
                                                struGsh.struVer.sUnit[1][i][1]&0x0F,
                                                struGsh.struVer.sUnit[1][i][0],
                                                materialType);
                     }

                QString strTmp;

                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]),
                               qPrintable(strChuteVerTmp[3]),
                               qPrintable(strChuteVerTmp[4]));

                str += strTmp;

            }

        }

        if(struCnfg.struLevelInfo[0].nViewTotal == 1 &&struCnfg.struLevelInfo[1].nViewTotal == 2
                && struCnfg.struLevelInfo[2].nViewTotal == 2)
        {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                        qPrintable(myLan.mid + "-" +myLan.front),qPrintable(myLan.mid + "-" + myLan.rear),
                        qPrintable(myLan.lower + "-" + myLan.front), qPrintable(myLan.lower + "-" + myLan.rear)); // 通道 上层/中前/中后/下前/下后
            char materialType[8] = "\0";
            QString strChuteVerTmp[5];//上层/中前/中后/下前/下后
            for(int i = 0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2][1],
                          struGsh.struVer.sUnit[1][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2][5],
                                              struGsh.struVer.sUnit[1][i*2][4],
                                              struGsh.struVer.sUnit[1][i*2][3],
                                              struGsh.struVer.sUnit[1][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2+1][1],
                          struGsh.struVer.sUnit[1][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2+1][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2+1][5],
                                              struGsh.struVer.sUnit[1][i*2+1][4],
                                              struGsh.struVer.sUnit[1][i*2+1][3],
                                              struGsh.struVer.sUnit[1][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2+1][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i*2][1],
                          struGsh.struVer.sUnit[2][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i*2][1] == 255) {
                    strChuteVerTmp[3].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[3].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[2][i*2][5],
                                              struGsh.struVer.sUnit[2][i*2][4],
                                              struGsh.struVer.sUnit[2][i*2][3],
                                              struGsh.struVer.sUnit[2][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[2][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[2][i*2+1][1],
                          struGsh.struVer.sUnit[2][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[2][i*2+1][1] == 255) {
                    strChuteVerTmp[4].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[4].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[2][i*2+1][5],
                                              struGsh.struVer.sUnit[2][i*2+1][4],
                                              struGsh.struVer.sUnit[2][i*2+1][3],
                                              struGsh.struVer.sUnit[2][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[2][i*2+1][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i][1],
                          struGsh.struVer.sUnit[0][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[0][i][5],
                                                struGsh.struVer.sUnit[0][i][4],
                                                struGsh.struVer.sUnit[0][i][3],
                                                struGsh.struVer.sUnit[0][i][1]&0x0F,
                                                struGsh.struVer.sUnit[0][i][0],
                                                materialType);
                     }

                QString strTmp;

                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]),
                               qPrintable(strChuteVerTmp[3]),
                               qPrintable(strChuteVerTmp[4]));

                str += strTmp;

            }

        }

        if(struCnfg.struLevelInfo[0].nViewTotal == 2 &&struCnfg.struLevelInfo[1].nViewTotal == 2
                && struCnfg.struLevelInfo[2].nViewTotal == 2)
        {
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),
                        qPrintable(myLan.upper + "-" +myLan.front),qPrintable(myLan.upper + "-" +myLan.rear),
                        qPrintable(myLan.mid + "-" +myLan.front),qPrintable(myLan.mid + "-" +myLan.rear),
                        qPrintable(myLan.lower + "-" + myLan.front), qPrintable(myLan.lower + "-" + myLan.rear)); // 通道 上前/上后/中前/中后/下前/下后
            char materialType[8] = "\0";
            QString strChuteVerTmp[6];//上前/上后/中前/中后/下前/下后
            QString strTmp;
             for(int i = 0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal/2;i++){
                    for(int j = 0; j<struCnfg.nLevelTotal; j++){

                    getVersionMaterialType(struGsh.struVer.sUnit[j][i*2][1],
                              struGsh.struVer.sUnit[j][i*2][2],
                              materialType);
                    if (struGsh.struVer.sUnit[j][i*2][1] == 255) {
                        strChuteVerTmp[2*j].sprintf("%s","NULL");
                     } else {
                        strChuteVerTmp[2*j].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                  struGsh.struVer.sUnit[j][i*2][5],
                                                  struGsh.struVer.sUnit[j][i*2][4],
                                                  struGsh.struVer.sUnit[j][i*2][3],
                                                  struGsh.struVer.sUnit[j][i*2][1]&0x0F,
                                                  struGsh.struVer.sUnit[j][i*2][0],
                                                  materialType);
                     }
                    getVersionMaterialType(struGsh.struVer.sUnit[j][i*2+1][1],
                              struGsh.struVer.sUnit[j][i*2+1][2],
                              materialType);
                    if (struGsh.struVer.sUnit[j][i*2+1][1] == 255) {
                        strChuteVerTmp[2*j+1].sprintf("%s","NULL");
                     } else {
                       strChuteVerTmp[2*j+1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                  struGsh.struVer.sUnit[j][i*2+1][5],
                                                  struGsh.struVer.sUnit[j][i*2+1][4],
                                                  struGsh.struVer.sUnit[j][i*2+1][3],
                                                  struGsh.struVer.sUnit[j][i*2+1][1]&0x0F,
                                                  struGsh.struVer.sUnit[j][i*2+1][0],
                                                  materialType);
                     }

                }
                    strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%-22s\t%-22s\t%s  \n",
                                   i+1,//通道从1计数
                                   qPrintable(strChuteVerTmp[0]),
                                   qPrintable(strChuteVerTmp[1]),
                                   qPrintable(strChuteVerTmp[2]),
                                   qPrintable(strChuteVerTmp[3]),
                                   qPrintable(strChuteVerTmp[4]),
                                   qPrintable(strChuteVerTmp[5]));

                    str += strTmp;
             }

        }


    } else {

        if(struCnfg.struLevelInfo[0].nViewTotal == 1 && struCnfg.struLevelInfo[1].nViewTotal == 1){
            str.sprintf("%-10s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                        qPrintable(myLan.lower_layer)); // 通道 上层/下层
            for(int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++){
                char materialType[8] = "\0";
                QString strChuteVerTmp[2];// 上层下层

                for(int j = 0; j<struCnfg.nLevelTotal; j++){
                    getVersionMaterialType(struGsh.struVer.sUnit[j][i][1],
                            struGsh.struVer.sUnit[j][i][2],
                            materialType);
                    if(struGsh.struVer.sUnit[j][i][1] == 255){
                        strChuteVerTmp[j].sprintf("%s","NULL");
                    } else {
                        strChuteVerTmp[j].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                  struGsh.struVer.sUnit[j][i][5],
                                                  struGsh.struVer.sUnit[j][i][4],
                                                  struGsh.struVer.sUnit[j][i][3],
                                                  struGsh.struVer.sUnit[j][i][1]&0x0F,
                                                  struGsh.struVer.sUnit[j][i][0],
                                                  materialType);
                    }
                }
                QString strTmp;

                strTmp.sprintf("   %-10d\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]));

                str += strTmp;
            }
        }

        if(struCnfg.struLevelInfo[0].nViewTotal == 2 && struCnfg.struLevelInfo[1].nViewTotal == 1){
            str.sprintf("%-10s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper + "-" +myLan.front),
                        qPrintable(myLan.upper + "-" + myLan.rear),qPrintable(myLan.lower_layer)); // 通道 上前/上后/下层
            char materialType[8] = "\0";
            QString strChuteVerTmp[3];//上前/上后/下层
            for(int i = 0; i<struCnfg.struLevelInfo[1].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2][1],
                          struGsh.struVer.sUnit[0][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2][5],
                                              struGsh.struVer.sUnit[0][i*2][4],
                                              struGsh.struVer.sUnit[0][i*2][3],
                                              struGsh.struVer.sUnit[0][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i*2+1][1],
                          struGsh.struVer.sUnit[0][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i*2+1][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[0][i*2+1][5],
                                              struGsh.struVer.sUnit[0][i*2+1][4],
                                              struGsh.struVer.sUnit[0][i*2+1][3],
                                              struGsh.struVer.sUnit[0][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[0][i*2+1][0],
                                              materialType);
                 }

                for (int j = 1; j < struCnfg.nLevelTotal; j++ ){
                     getVersionMaterialType(struGsh.struVer.sUnit[j][i][1],
                               struGsh.struVer.sUnit[j][i][2],
                               materialType);
                     if (struGsh.struVer.sUnit[j][i][1] == 255) {
                         strChuteVerTmp[j+1].sprintf("%s","NULL");
                      } else {
                        strChuteVerTmp[j+1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                     struGsh.struVer.sUnit[j][i][5],
                                                     struGsh.struVer.sUnit[j][i][4],
                                                     struGsh.struVer.sUnit[j][i][3],
                                                     struGsh.struVer.sUnit[j][i][1]&0x0F,
                                                     struGsh.struVer.sUnit[j][i][0],
                                                     materialType);
                          }
                      }
                QString strTmp;

                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]));

                str += strTmp;

            }

        }

        if(struCnfg.struLevelInfo[0].nViewTotal == 1 && struCnfg.struLevelInfo[1].nViewTotal == 2){
            str.sprintf("%-10s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                        qPrintable(myLan.lower + "-" + myLan.front),qPrintable(myLan.lower + "-" + myLan.rear)); // 通道 上层/下前/下后
            char materialType[8] = "\0";
            QString strChuteVerTmp[4];//上层/下前/下后
            for(int i = 0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal;i++){
                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2][1],
                          struGsh.struVer.sUnit[1][i*2][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2][1] == 255) {
                    strChuteVerTmp[1].sprintf("%s","NULL");
                 } else {
                    strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2][5],
                                              struGsh.struVer.sUnit[1][i*2][4],
                                              struGsh.struVer.sUnit[1][i*2][3],
                                              struGsh.struVer.sUnit[1][i*2][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[1][i*2+1][1],
                          struGsh.struVer.sUnit[1][i*2+1][2],
                          materialType);
                if (struGsh.struVer.sUnit[1][i*2+1][1] == 255) {
                    strChuteVerTmp[2].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[2].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                              struGsh.struVer.sUnit[1][i*2+1][5],
                                              struGsh.struVer.sUnit[1][i*2+1][4],
                                              struGsh.struVer.sUnit[1][i*2+1][3],
                                              struGsh.struVer.sUnit[1][i*2+1][1]&0x0F,
                                              struGsh.struVer.sUnit[1][i*2+1][0],
                                              materialType);
                 }

                getVersionMaterialType(struGsh.struVer.sUnit[0][i][1],
                          struGsh.struVer.sUnit[0][i][2],
                          materialType);
                if (struGsh.struVer.sUnit[0][i][1] == 255) {
                    strChuteVerTmp[0].sprintf("%s","NULL");
                 } else {
                   strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                struGsh.struVer.sUnit[0][i][5],
                                                struGsh.struVer.sUnit[0][i][4],
                                                struGsh.struVer.sUnit[0][i][3],
                                                struGsh.struVer.sUnit[0][i][1]&0x0F,
                                                struGsh.struVer.sUnit[0][i][0],
                                                materialType);
                     }

                QString strTmp;

                strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%s  \n",
                               i+1,//通道从1计数
                               qPrintable(strChuteVerTmp[0]),
                               qPrintable(strChuteVerTmp[1]),
                               qPrintable(strChuteVerTmp[2]));

                str += strTmp;

            }

        }

        if(struCnfg.struLevelInfo[0].nViewTotal == 2 && struCnfg.struLevelInfo[1].nViewTotal == 2){
            str.sprintf("%-10s\t%-22s\t%-22s\t%-22s\t%s  \n",qPrintable(myLan.chute),
                        qPrintable(myLan.upper + "-" +myLan.front),qPrintable(myLan.upper + "-" +myLan.rear),
                        qPrintable(myLan.lower + "-" + myLan.front), qPrintable(myLan.lower + "-" + myLan.rear)); // 通道 上前/上后/下前/下后
            char materialType[8] = "\0";
            QString strChuteVerTmp[4];//上前/上后/下前/下后
            QString strTmp;
             for(int i = 0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal/2;i++){
                 for(int j = 0; j<struCnfg.nLevelTotal; j++){

                    getVersionMaterialType(struGsh.struVer.sUnit[j][i*2][1],
                              struGsh.struVer.sUnit[j][i*2][2],
                              materialType);
                    if (struGsh.struVer.sUnit[j][i*2][1] == 255) {
                        strChuteVerTmp[2*j].sprintf("%s","NULL");
                     } else {
                        strChuteVerTmp[2*j].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                  struGsh.struVer.sUnit[j][i*2][5],
                                                  struGsh.struVer.sUnit[j][i*2][4],
                                                  struGsh.struVer.sUnit[j][i*2][3],
                                                  struGsh.struVer.sUnit[j][i*2][1]&0x0F,
                                                  struGsh.struVer.sUnit[j][i*2][0],
                                                  materialType);
                     }

                    getVersionMaterialType(struGsh.struVer.sUnit[j][i*2+1][1],
                              struGsh.struVer.sUnit[j][i*2+1][2],
                              materialType);
                    if (struGsh.struVer.sUnit[j][i*2+1][1] == 255) {
                        strChuteVerTmp[2*j+1].sprintf("%s","NULL");
                     } else {
                       strChuteVerTmp[2*j+1].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                                  struGsh.struVer.sUnit[j][i*2+1][5],
                                                  struGsh.struVer.sUnit[j][i*2+1][4],
                                                  struGsh.struVer.sUnit[j][i*2+1][3],
                                                  struGsh.struVer.sUnit[j][i*2+1][1]&0x0F,
                                                  struGsh.struVer.sUnit[j][i*2+1][0],
                                                  materialType);
                     }

                }

                 strTmp.sprintf("   %-10d\t%-22s\t%-22s\t%-22s\t%s  \n",
                                i+1,//通道从1计数
                                qPrintable(strChuteVerTmp[0]),
                                qPrintable(strChuteVerTmp[1]),
                                qPrintable(strChuteVerTmp[2]),
                                qPrintable(strChuteVerTmp[3]));

                 str += strTmp;

             }

        }
    }


    m_textEdit->setText(str);

}

/* 更新LD机型前置板版本 */
void StatusInfoPage::versionUpdateCameraLD()
{
    QString str;
    /* 通道 相机 只有一层 */
    str.sprintf("%-10s\t%-18s\t\n",qPrintable(myLan.chute),qPrintable(myLan.color_camera));

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        char materialType[8] = "\0";
        QString strChuteVerTmp;

        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i][1] == 255) {
            strChuteVerTmp.sprintf("%s","NULL");
        } else {
            strChuteVerTmp.sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                      struGsh.struVer.sUnit[ONE_LEVEL][i][5],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i][4],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i][3],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i][1]&0x0F,
                                      struGsh.struVer.sUnit[ONE_LEVEL][i][0],
                                      materialType);
        }

        QString strTmp;
        strTmp.sprintf("   %-7d\t%s\n",
                       i+1,//通道从1计数
                       qPrintable(strChuteVerTmp));
        str += strTmp;
    }
    m_textEdit->setText(str);
}

/* 更新LDS机型前置板版本 */
void StatusInfoPage::versionUpdateCameraLDS()
{
    QString str;
    str.sprintf("%-10s\t%-18s\t%s\n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer),
                qPrintable(myLan.lower_layer));//通道 上层 下层

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        char materialType[8] = "\0";
        QString strChuteVerTmp[2];// 上层、下层

        for (int j = 0; j < struCnfg.nLevelTotal; j++ ){
            getVersionMaterialType(struGsh.struVer.sUnit[j][i][1],
                    struGsh.struVer.sUnit[j][i][2],
                    materialType);
            if (struGsh.struVer.sUnit[j][i][1] == 255) {
                strChuteVerTmp[j].sprintf("%s","NULL");
            } else {
                strChuteVerTmp[j].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                          struGsh.struVer.sUnit[j][i][5],
                                          struGsh.struVer.sUnit[j][i][4],
                                          struGsh.struVer.sUnit[j][i][3],
                                          struGsh.struVer.sUnit[j][i][1]&0x0F,
                                          struGsh.struVer.sUnit[j][i][0],
                                          materialType);
            }
        }

        QString strTmp;
        strTmp.sprintf("   %-7d\t%-18s\t%s\n",
                       i+1,//通道从1计数
                       qPrintable(strChuteVerTmp[0]),
                       qPrintable(strChuteVerTmp[1]));
        str += strTmp;
    }
    m_textEdit->setText(str);
}

void StatusInfoPage::versionUpdateCameraLDS2()
{
    QString str;    //通道 上层前 上层后 下层前 下层后
    str.sprintf("%-10s\t%-18s\t%-18s\t%-18s\t%s\n",qPrintable(myLan.chute),qPrintable(myLan.upper_layer+"-"+myLan.upper),
                qPrintable(myLan.upper_layer+"-"+myLan.lower), qPrintable(myLan.lower_layer+"-"+myLan.upper), qPrintable(myLan.lower_layer+"-"+myLan.lower));

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2; i++) {
        char materialType[8] = "\0";
        QString strChuteVerTmp[4];// 上层前 上层后 下层前 下层后
        for (int j = 0; j < 4; j++ ){
            getVersionMaterialType(struGsh.struVer.sUnit[j/2][i*2+j%2][1],
                    struGsh.struVer.sUnit[j/2][i*2+j%2][2],
                    materialType);
            if (struGsh.struVer.sUnit[j/2][i*2+j%2][1] == 255) {
                strChuteVerTmp[j].sprintf("%s","NULL");
            } else {
                strChuteVerTmp[j].sprintf("50%02d%02d%02d_V%d.%02d_%s",
                                          struGsh.struVer.sUnit[j/2][i*2+j%2][5],
                                          struGsh.struVer.sUnit[j/2][i*2+j%2][4],
                                          struGsh.struVer.sUnit[j/2][i*2+j%2][3],
                                          struGsh.struVer.sUnit[j/2][i*2+j%2][1]&0x0F,
                                          struGsh.struVer.sUnit[j/2][i*2+j%2][0],
                                          materialType);
            }
        }

        QString strTmp;
        strTmp.sprintf("   %-7d\t%-18s\t%-18s\t%-18s\t%s\n",
                       i+1,//通道从1计数
                       qPrintable(strChuteVerTmp[0]),
                       qPrintable(strChuteVerTmp[1]),
                       qPrintable(strChuteVerTmp[2]),
                       qPrintable(strChuteVerTmp[3]));
        str += strTmp;
    }
    m_textEdit->setText(str);
}

/* 更新CF机型前置板版本 */
void StatusInfoPage::versionUpdateCameraCF()
{
    QString str;

    str.sprintf("%-s",qPrintable(myLan.chute));
    if (struCnfe.nDerivedDevType & 0x0001) {
        str.sprintf("%s\t%-22s",qPrintable(str),qPrintable(myLan.front_view));
    }
    if (struCnfe.nDerivedDevType & 0x0002) {
        str.sprintf("%s\t%-22s",qPrintable(str),qPrintable(myLan.rear_view));
    }
    if(struCnfg.aiEnable == 1) {
        str.sprintf("%s\t%-20s",qPrintable(str),qPrintable("AI"));
    }

    if ((struCnfe.nDerivedDevType & 0x01000100) == 0x01000100) {
        str.sprintf("%s\t%-22s",qPrintable(str),qPrintable(myLan.infra+"-"+myLan.front));
    }
    if ((struCnfe.nDerivedDevType & 0x01000200) == 0x01000200) {
        str.sprintf("%s\t%-22s",qPrintable(str),qPrintable(myLan.infra+"-"+myLan.rear));
    }
    str += "\n";

    int versionFlag =0;

    if (struCnfc.nSensorPixelLevel[0] == SENSOR_PIXELS_1024) {
        versionFlag = 0;

    }else  if (struCnfc.nSensorPixelLevel[0] == SENSOR_PIXELS_2048){
        versionFlag = 1;
    }

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++) {
        char materialType[8] = "\0";
        QString strChuteVerTmp[4];// 前视、后视、红外-前、红外-后

        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i*2][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i*2][1] == 255) {
            strChuteVerTmp[0].sprintf("\t%s","NULL                        ");

        } else {
//            strChuteVerTmp[0].sprintf("\t50%02d%02d%02d_V%d.%02d_%s",//前视
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][5],
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][4],
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][3],
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][1]&0x0F,
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][0],
//                                      materialType);
            if(versionFlag ==0){
                strChuteVerTmp[0].sprintf("\t%s","V1.00_1                     ");
            }
            if(versionFlag ==1){
                strChuteVerTmp[0].sprintf("\t%s","V1.00_2                     ");
            }

        }

        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1] == 255) {
            strChuteVerTmp[1].sprintf("\t%s","NULL                        ");

        } else {
//            strChuteVerTmp[1].sprintf("\t50%02d%02d%02d_V%d.%02d_%s",//后视
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][5],
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][4],
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][3],
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1]&0x0F,
//                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][0],
//                                      materialType);
            if(versionFlag ==0){
                strChuteVerTmp[1].sprintf("\t%s","V1.00_1                     ");
            }
            if(versionFlag ==1){
                strChuteVerTmp[1].sprintf("\t%s","V1.00_2                     ");
            }
//            strChuteVerTmp[1].sprintf("\t%s","V1.00                       ");

        }



        QString strTmp;
        if (!(struCnfe.nDerivedDevType & 0x0001)) {
            strChuteVerTmp[0] = "";
        }
        if (!(struCnfe.nDerivedDevType & 0x0002)) {
            strChuteVerTmp[1] = "";
        }
        if ((struCnfe.nDerivedDevType  & 0x01000100) != 0x01000100) {
            strChuteVerTmp[2] = "";
        }
        if ((struCnfe.nDerivedDevType  & 0x01000200) != 0x01000200) {
            strChuteVerTmp[3] = "";
        }

        if(struCnfg.aiEnable == 1) {
            if (struGsh.aiResult[i] == QString("")) {
                strChuteVerTmp[2].sprintf("\t%s","NULL");
            } else {
                strChuteVerTmp[2] = struGsh.aiResult[i]+QString("               ");
            }
        }

        strTmp.sprintf("   %d%s%s%s%s\n",
                       i+1,//通道从1计数
                       qPrintable(strChuteVerTmp[0]),
                       qPrintable(strChuteVerTmp[1]),
                       qPrintable(strChuteVerTmp[2]),
                       qPrintable(strChuteVerTmp[3]));
        str += strTmp;
    }
    m_textEdit->setText(str);
}

/* 更新LD2机型前置板版本 */
void StatusInfoPage::versionUpdateCameraLD2()
{
    QString str;
    str.sprintf("%-10s\t%-18s\t%s\n",qPrintable(myLan.chute),qPrintable(myLan.upper_view),
                qPrintable(myLan.lower_view));  // 上视、下视

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++) {
        char materialType[8] = "\0";
        QString strChuteVerTmp[3];          // 上视、下视、红外

        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i*2][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i*2][1] == 255) {
            strChuteVerTmp[0].sprintf("%s","NULL");
        } else {
            strChuteVerTmp[0].sprintf("50%02d%02d%02d_V%d.%02d_%s", // 上视
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][5],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][4],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][3],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][1]&0x0F,
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2][0],
                                      materialType);
        }

        getVersionMaterialType(struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1],
                struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][2],
                materialType);
        if (struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1] == 255) {
            strChuteVerTmp[1].sprintf("%s","NULL");
        } else {
            strChuteVerTmp[1].sprintf("50%02d%02d%02d_V%d.%02d_%s", // 下视
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][5],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][4],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][3],
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][1]&0x0F,
                                      struGsh.struVer.sUnit[ONE_LEVEL][i*2+1][0],
                                      materialType);
        }


        QString strTmp;
        strTmp.sprintf("   %-7d\t%-18s\t%s\n",
                       i+1,     // 通道从1计数
                       qPrintable(strChuteVerTmp[0]),
                       qPrintable(strChuteVerTmp[1]));
        str += strTmp;
    }
    m_textEdit->setText(str);
}

/* 更新前置板版本 */
void StatusInfoPage::versionUpdateCamera()
{
    versionUpdateCameraCF();
}

/* 更新RS机型前置板版本 */
void StatusInfoPage::versionUpdateCameraRS()
{
    int nUnitTmp = 0;
    int front_rear = 0;

    //! 主配前置板
    for(int i = 1; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal+1; i++){
        cameraListLabel[(i+1)/2][0]->setText(QString("%1").arg(i/2));
        for (int j = 0; j < struCnfg.nLevelTotal; j++) {
            nUnitTmp = struCnfg.struLevelInfo[j].nUnitId[i-1];
            front_rear = (nUnitTmp%2 == 0) ? 1 : 2;
            if((struGsh.struVer.sUnit[j][nUnitTmp][0] != 255) && (struGsh.struVer.sUnit[j][nUnitTmp][1] != 255)) {
                  cameraListLabel[(i+1)/2][front_rear]->setText(QString("V%1.0%2").arg(struGsh.struVer.sUnit[j][nUnitTmp][1])
                          .arg(struGsh.struVer.sUnit[j][nUnitTmp][0]));
                  cameraListLabel[(i+1)/2][front_rear]->setStyleSheet("color:black");
            } else {
                  cameraListLabel[(i+1)/2][front_rear]->setText("NULL");
                  cameraListLabel[(i+1)/2][front_rear]->setStyleSheet("color:red");
            }
        }
    }
    cameraListLabel[0][0]->setText(myLan.chute);
    cameraListLabel[0][1]->setText(myLan.front_view);
    cameraListLabel[0][2]->setText(myLan.rear_view);

    //! 辅配前置板
    bool bAssistExist = false;
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
    for (int i = 0; i < nUnitTotal; i++) {
        if (struCnfg.nAssistCamEn[i] == 1) {
            bAssistExist = true;
            break;
        }
    }
    if (bAssistExist) {
        cameraListLabel[0][3]->setText(myLan.front_view+myLan.config_assist);
        cameraListLabel[0][4]->setText(myLan.rear_view+myLan.config_assist);
        for (int i = 1; i < nUnitTotal/2+1; i++) {
            cameraListLabel[i][3]->setText("--");
            cameraListLabel[i][3]->setStyleSheet("color: black");

            cameraListLabel[i][4]->setText("--");
            cameraListLabel[i][4]->setStyleSheet("color: black");
        }

        for(int i = 0; i < nUnitTotal/2+1; i++) {
            cameraListLabel[i][3]->show();
            cameraListLabel[i][4]->show();
        }
    } else {
        for(int i = 0; i < nUnitTotal/2+1; i++) {
            cameraListLabel[i][3]->hide();
            cameraListLabel[i][4]->hide();
        }
    }

    /* 根据当前通道数隐藏多余前置板 */
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2+1; i++) {
        for (int j = 0; j < MAX_BACKGROUND_GROUP+3; j++) {
            cameraListLabel[i][j]->show();
        }
    }
    for (int i = struCnfg.struLevelInfo[0].nUnitLevelTotal/2+1; i < MAX_UNIT/2+1; i++) {
        for (int j = 0; j < MAX_BACKGROUND_GROUP+3; j++) {
            cameraListLabel[i][j]->hide();
        }
    }
}

/* 恒流源板版本信息 */
void StatusInfoPage::versionUpdateLightSrc()
{
    for (int i = 0; i < struCnfg.nLampAINum; i++) {
        m_lightSrcLabel[i][0]->show();
        m_lightSrcLabel[i][1]->show();
        m_lightSrcLabel[i][0]->setText(QString("%1").arg(i+1));

        /** 若恒流源板未使用则显示"--" */
        if (struCnfg.nCurSrcBoradLampNum[i] == 0) {
            m_lightSrcLabel[i][1]->setText("--");
            m_lightSrcLabel[i][1]->setStyleSheet("color:black");
        } else if ((struGsh.struVer.sLightSrc[i][0] != 255) && (struGsh.struVer.sLightSrc[i][1] != 255)) {
            m_lightSrcLabel[i][1]->setText(QString("V%1.0%2").arg(struGsh.struVer.sLightSrc[i][1])
                    .arg(struGsh.struVer.sLightSrc[i][0]));
            m_lightSrcLabel[i][1]->setStyleSheet("color:black");
        } else {
            m_lightSrcLabel[i][1]->setText("NULL");
            m_lightSrcLabel[i][1]->setStyleSheet("color:red");
        }
    }

    for (int i = struCnfg.nLampAINum; i < MAX_LIGHT_SOURCE; i++) {
        m_lightSrcLabel[i][0]->hide();
        m_lightSrcLabel[i][1]->hide();
    }
}

/* 更新版本查询结果 */
void StatusInfoPage::updateVersionResultSlt()
{
    versionUpdateInterface();

    versionUpdateUSBFirmware();

    versionUpdateCtrl();

    versionUpdateCamera();

    versionUpdateLightSrc();
}

void StatusInfoPage::updateStatisticPage(int index)
{
    JsonObject option;
    JsonObject data;
    JsonArray array;
    int step = 2;
    int range = 0;
    QString type("line");

    switch (index) {
    case STATISTIC_INDEX_LOG:
//        pageLogger->updateText();
	return;
    case STATISTIC_INDEX_BACKGROUND:
	// 清灰前
	for (int color = 0; color < 3; color++) {
	    array.clear();
	    for (int i = 0; i < struCnfc.nSensorPixelLevel[ONE_LEVEL]; i++) {
		if (i*3 >= struGsh.struStatisticInfo.bgBeforeWipeStatistic.size()){
		    break;
		}
		array << struGsh.struStatisticInfo.bgBeforeWipeStatistic.at(i*3+color);
	    }
	    QString index = QString("1st_%1").arg(color+1);
	    data[index] =  array;
	}
	// 清灰后
	for (int color = 0; color < 3; color++) {
	    array.clear();
	    for (int i = 0; i < struCnfc.nSensorPixelLevel[ONE_LEVEL]; i++) {
		if (i*3 >= struGsh.struStatisticInfo.bgAfterWipeStatistic.size()){
		    break;
		}
		array << struGsh.struStatisticInfo.bgAfterWipeStatistic.at(i*3+color);
	    }
	    QString index = QString("2nd_%1").arg(color+1);
	    data[index] =  array;
	}
	range = struCnfc.nSensorPixelLevel[ONE_LEVEL];
	break;
    case STATISTIC_INDEX_IDENTIFY:
#if DEF_VERSION_STATISTIC == 1
	step = 8;
#endif
	array.clear();
	for (int i = step; i < struGsh.struStatisticInfo.materialStatistic.size(); i += step) {
	    quint64 tmp = 0;
	    for (int j = 0; j < step; j++) {
		tmp += ((quint64)struGsh.struStatisticInfo.materialStatistic.at(i+j)) << ((step-1-j)*8);
	    }
	    array << tmp;
	}
	range = 50;
	type  = QString("%1").arg("bar");
	data["bad_point"] = array;
	break;
    case STATISTIC_INDEX_HISTOGRAM:
	step = 4;
	for (int color = 0; color < 3; color++) {
	    array.clear();
	    int base = color*struGsh.struStatisticInfo.histogramStatistic.size()/3;
	    for (int i = 0; i < struGsh.struStatisticInfo.histogramStatistic.size()/3; i += step) {
		quint64 tmp = 0;
		for (int j = 0; j < step; j++) {
		    tmp += ((quint64)struGsh.struStatisticInfo.histogramStatistic.at(i+j+base)) << ((step-1-j)*8);
		}
		array << tmp;
	    }
	    QString index = QString("%1").arg(color+1);
	    data[index] =  array;
	}
	range = 256;
	break;
    case STATISTIC_INDEX_EJECT_TIMES:
#if DEF_VERSION_STATISTIC == 1
	step = 4;
#endif
	for (int front_rear = 0; front_rear < 2; front_rear++) {
	    array.clear();
	    int base = front_rear * struGsh.struStatisticInfo.ejectStatistic.size()/2;
	    for (int i = 0; i < struGsh.struStatisticInfo.ejectStatistic.size()/2; i += step) {
		quint64 tmp = 0;
		for (int j = 0; j < step; j++) {
		    tmp += ((quint64)struGsh.struStatisticInfo.ejectStatistic.at(i+j+base)) << ((step-1-j)*8);
		}
		array << tmp;
	    }
	    QString index = QString("%1").arg(front_rear+1);
	    data[index] =  array;
	}
	type  = QString("%1").arg("bar");
	range = 60;
	break;
    case STATISTIC_INDEX_CONFIG:
	cameraVRef->setValue(struCnfp.nCameraRef/2);
	timeHistogramStaisticLineEdit->setText(QString("%1").arg(struCnfp.nHistogramQueryTime));
	timeMaterialStaisticLineEdit->setText(QString("%1").arg(struCnfp.nMaterialQueryInterval));
	return;
    default:
	return;
    }

    // chart data
    option["range"] = range;
    option["data"]  = data;
    option["type"]  = type;
	option["level"] = struCnfg.nCurCapLevel;
	option["unit"] = struCnfg.nCurCapUnit;
    QString script = QString("refreshData('%1')").arg(QJson::serializeStr(option));
    qDebug() << script;
//    webview->page()->mainFrame()->evaluateJavaScript(script);
}


/**
 *  创建远程控制页面
 */
void StatusInfoPage::CreateSetNetworkPage()
{
/* page init */
    /* tab widget */
    remoteTabBar = new QTabBar();
    remoteTabBar->setFont(config->getFont());
    if (LCD_HEIGHT == 768) {
        setStyleSheet("QTabBar::tab {min-height:45; min-width:180;}");
    } else {
        setStyleSheet("QTabBar::tab {min-height:32;}");
    }
    remoteTabBar->addTab(myLan.local_host);
    remoteTabBar->addTab(myLan.remote_control);
    if (struGsh.bIsMC) {    //! 仅MC机型显示设置短信页面
        remoteTabBar->addTab(myLan.set_sms);
    }
    remoteTabBar->addTab("WIFI");


    /* stacked Widget */
    remoteStackedWidget = new QStackedWidget(pageSetNetwork);

    /* network config */
    pageNetConfig = new MyNetConfig();
    remoteStackedWidget->addWidget(pageNetConfig);

    /* remote control */
    pageRemoteControl = new MyRemoteControl();
    remoteStackedWidget->addWidget(pageRemoteControl);

    pageWifiSet = new QWidget(this);
    remoteStackedWidget->addWidget(pageWifiSet);
    createPageWifiWidget();

    if (struGsh.bIsMC) {
        /* 设置短信 */
        m_pageSetSms = new SetSms();
        remoteStackedWidget->addWidget(m_pageSetSms);
    } else {
        /* big data */
        pageBigData = new MyBigData();
        remoteStackedWidget->addWidget(pageBigData);

    }

    //! 从设备列表
    m_pageSlaveList = new MySlaveList;
    remoteStackedWidget->addWidget(m_pageSlaveList);

    /* page update */
    connect(remoteTabBar, SIGNAL(currentChanged(int)), this, SLOT(onStackedIndexChangedSlt(int)));

    /* page layout */
    QVBoxLayout *mainLayout = new QVBoxLayout(pageSetNetwork);
    mainLayout->setContentsMargins(10, 0, 10, 0);
    mainLayout->addWidget(remoteTabBar);
    mainLayout->addWidget(remoteStackedWidget);
}

void StatusInfoPage::createPageWifiWidget(){
    ycprocess = new QProcess(this);
    connect(ycprocess, SIGNAL(readyReadStandardOutput()),this, SLOT(result()));
    connect(ycprocess, SIGNAL(readyReadStandardError()),this, SLOT(result()));

    ssidLabel = new myLabel("SSID");
    ssidComboBox = new MyComboBox();
//    ssidComboBox->setEditable(true);
    ssidComboBox->setFixedHeight(BTN_HEIGHT);
    ssidComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    ssidComboBox->setFont(config->getFont(DEFAULT_FONT_SIZE));

    passWordLabel = new myLabel(myLan.password);
    passWordLineEdit = new myLineEdit();
//    ssidComboBox->setFixedSize(BTN_WIDTH*3, BTN_HEIGHT);
//    ssidComboBox->view()->setFixedWidth(passWordLineEdit->width());
    passWordLineEdit->setFixedSize(BTN_WIDTH*3,BTN_HEIGHT);
    passWordLineEdit->setFont(config->getFont());
    wifiScanBtn = new myPushButton("SCAN", QIcon());
    wifiConnectBtn = new myPushButton(myLan.connect, QIcon());
    wifiDisconnectBtn = new myPushButton(myLan.disconnect, QIcon());
    wifiDisconnectBtn->setEnabled(false);
    wifiScanBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    wifiConnectBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    wifiDisconnectBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    connectResultEdit= new QTextEdit();
    connectResultEdit->setFont(config->getFont());
    connectResultEdit->setReadOnly(true);
    connectResultEdit->setWordWrapMode(QTextOption::NoWrap);

    wifiAddrLabel= new myLabel("WIFI "+myLan.ip_address);
    wifiAddrLineEdit= new myLineEdit("0.0.0.0");
    wifiAddrLineEdit->setFixedHeight(BTN_HEIGHT);

    QVBoxLayout *mainLay = new QVBoxLayout(pageWifiSet);
    QGridLayout *upLayout = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;

    upLayout->addWidget(ssidLabel, 0, 0, 1, 1);
    upLayout->addWidget(ssidComboBox, 0, 1, 1, 1);
    upLayout->addWidget(passWordLabel, 1, 0, 1, 1);
    upLayout->addWidget(passWordLineEdit, 1, 1, 1, 1);
    upLayout->addWidget(wifiAddrLabel, 2, 0, 1, 1);
    upLayout->addWidget(wifiAddrLineEdit, 2, 1, 1, 1);
    upLayout->addWidget(connectResultEdit, 3, 0, 1, 2);
    downLay->addWidget(wifiConnectBtn, 1);
    downLay->addWidget(wifiScanBtn, 1);
    downLay->addWidget(wifiDisconnectBtn, 1);
    downLay->setContentsMargins(0, 5, 0, 0);
    downLay->setSpacing(150);
    mainLay->addLayout(upLayout,2);
    mainLay->addLayout(downLay,1);
    mainLay->setContentsMargins(10, 0, 10, 20);
    connect(wifiScanBtn, SIGNAL(pressed()), this, SLOT(onWifiScanBtnPressed()));
    connect(wifiConnectBtn, SIGNAL(pressed()), this, SLOT(onWifiConnectBtnPressed()));
    connect(wifiDisconnectBtn, SIGNAL(pressed()), this, SLOT(onWifiDisonnectBtnPressed()));
    connect(passWordLineEdit ,SIGNAL(pressed()),this, SLOT(onPassWordLineEditPressed()));

}

void StatusInfoPage::onPassWordLineEditPressed(){
    myInputMethod inputDlg("wifi"+myLan.password,passWordLineEdit->text());
    if(inputDlg.exec() == QDialog::Accepted)
    {
        passWordLineEdit->setText(inputDlg.getText());
        struGsh.wifiPassWord = passWordLineEdit->text();
    }
}
void StatusInfoPage::result()
{
    QString abc = ycprocess->readAllStandardOutput();
    connectResultEdit->append(abc.trimmed());
    QString efg = ycprocess->readAllStandardError();
    if(efg.length()>1)connectResultEdit->append(efg.trimmed());
}

void StatusInfoPage::onWifiScanBtnPressed(){
    ssidComboBox->clear();
    char cmdStr[1024];
    FILE *fp;
    QString tmp;
    int i;
    int allwifiitem=0;
    char readStr[1024];

    memset(cmdStr,0,1024);
    sprintf(cmdStr, "> /tmp/wifi.txt");
    system(cmdStr);

    memset(cmdStr,0,1024);
    sprintf(cmdStr, "iwlist mlan0 scan | grep 'ESSID:' | cut -d'\"' -f2 > /tmp/wifi.txt");
    system(cmdStr);

    memset(cmdStr,0,256);
    sprintf(cmdStr, "cat /tmp/wifi.txt | wc -l");//Get "auto eth1" line number
    fp = popen(cmdStr, "r");
    memset(readStr,0,256);
    fread(readStr, 1, sizeof(readStr), fp);
    pclose(fp);
    allwifiitem=atoi(readStr);

    for(i=0;i<allwifiitem;i++)
    {
        memset(cmdStr,0,256);
        sprintf(cmdStr, "cat /tmp/wifi.txt | awk 'NR==%d'",i);//Get "auto eth1" line number
        fp = popen(cmdStr, "r");
        memset(readStr,0,256);
        fread(readStr, 1, sizeof(readStr), fp);
        pclose(fp);
        tmp=QString::fromLocal8Bit(readStr);
        tmp=tmp.left(tmp.size()-1);
        if(!tmp.isEmpty())
        {
            ssidComboBox->addItem(convertHexInString(tmp), i);
        }
    }
}

// 将十六进制字符串转换为字节
char StatusInfoPage:: hexToByte(const QString& hex) {
    bool ok;
    return static_cast<char>(hex.toInt(&ok, 16));
}

// 处理包含英文和 0x 十六进制的 QString
QString StatusInfoPage::convertHexInString(const QString& input) {
    QString result = input;
        // 定义正则表达式来匹配 \x 开头的十六进制部分
        QRegularExpression regex("\\\\x([0-9A-Fa-f]{2})");
        auto matches = regex.globalMatch(input);

        QByteArray byteArray;
        while (matches.hasNext()) {
            auto match = matches.next();
            QString hexPart = match.captured(1);
            char byte = hexToByte(hexPart);
            byteArray.append(byte);
            result.replace(match.captured(), "");
        }

        // 假设使用 GBK 编码进行转换
        QString chinese = QString::fromLocal8Bit(byteArray);
        result.append(chinese);
        return result;
}

// 函数用于将包含 0x 的十六进制字符串转换为汉字
QString StatusInfoPage::hexStringToChinese(const QString& hexString) {
    // 去除所有的 0x
    QString cleanHex = hexString;
    cleanHex.remove("0x");

    // 将十六进制字符串转换为字节数组
    QByteArray byteArray;
    for (int i = 0; i < cleanHex.length(); i += 2) {
        QString hexByte = cleanHex.mid(i, 2);
        bool ok;
        char byte = static_cast<char>(hexByte.toInt(&ok, 16));
        if (ok) {
            byteArray.append(byte);
        }
    }

    // 使用 UTF - 8 编码将字节数组转换为 QString
    return QString::fromUtf8(byteArray);
}

void StatusInfoPage::onWifiConnectBtnPressed(){
    if(!ssidComboBox->currentText().isEmpty())
    {
        char cmdStr[256];
        memset(cmdStr,0,256);
        connectResultEdit->clear();
        sprintf(cmdStr, "./wifi.sh -i mlan0 -s %s -p %s",ssidComboBox->currentText().toLocal8Bit().data(),passWordLineEdit->text().toLocal8Bit().data());//Get "auto eth1" line number
        ycprocess->start(cmdStr);
        wifiDisconnectBtn->setEnabled(true);
        struGsh.wifiSsid = ssidComboBox->currentText();
    }else{
        qDebug() <<"wifi ssid  is empty";
    }

}
void StatusInfoPage::onWifiDisonnectBtnPressed(){
   system("killall -15 wpa_supplicant");
}

void StatusInfoPage::updateSetNetworkPageSlt()
{
    /* trigger to index first page */
    if (remoteTabBar->currentIndex() != 0)
        remoteTabBar->setCurrentIndex(0);

    /* update  remote control config */
	pageNetConfig->getHostAddress();

    //! 根据主从屏模式刷新TabBar显示
    int nMax = 4;
    if (remoteTabBar->count() == nMax) {
        if (struCnfg.nRunMode != MODE_RUN_MASTER) {
            remoteTabBar->removeTab(nMax - 1);
        }
    } else {
        if (struCnfg.nRunMode == MODE_RUN_MASTER) {
            remoteTabBar->addTab(myLan.slave_list);
        }
    }
    m_pageSlaveList->refreshDisplay();
}

/**********************************************************************************/
/**
 *  创建系统信息页面
 */
void StatusInfoPage::CreateInfomationPage()
{
    QString infoStr;
    if(struCnfe.nEnableBeanMachine == 1) {
        infoStr.sprintf("Version %s_%s_%d-BEAN, Compiled on %s %s",
                PLATFORM_NAME,
				myString.sAppVersion.toLocal8Bit().constData(), 
				myString.sAppSVN.toInt(),
				__DATE__, __TIME__);
	}
	else {
        infoStr.sprintf("Version %s_%s_%d, Compiled on %s %s",
                PLATFORM_NAME,
				myString.sAppVersion.toLocal8Bit().constData(), 
				myString.sAppSVN.toInt(),
				__DATE__, __TIME__);
	}
    infoLabel  = new myLabel(infoStr, pageInformation);
    infoLabel->setGeometry(QRect(20, 20, pageInformation->width(), 100));

    QVBoxLayout *mainLayout = new QVBoxLayout(pageInformation);
    mainLayout->addWidget(infoLabel);
    mainLayout->addStretch(1);
}

/* 加载帮助文档 */
void StatusInfoPage::getHowToHelp()
{

}

QString StatusInfoPage::getVpnIpAddress() {
    QProcess process;
    // 使用 sh -c 来执行包含管道的命令
    process.start("sh", QStringList() << "-c" << "ip addr show mlan0 | grep 'inet ' | awk '{print $2}' | cut -d/ -f1");
    process.waitForFinished();

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        output = output.simplified(); // 去除前后空格
        return output;
    } else {
        // 处理命令执行失败的情况
        QString error = process.readAllStandardError();
        qDebug() << "Command execution failed:" << error;
        return "";
    }

}

/* 切换页索引变化 */
void StatusInfoPage::onStackedIndexChangedSlt(int index)
{
    remoteStackedWidget->setCurrentIndex(index);

    if (struGsh.bIsMC && index == 2) {
        m_pageSetSms->refreshDisplay();
    }
    if(index == 2){
        ssidComboBox->setCurrentText(struGsh.wifiSsid);
        passWordLineEdit->setText(struGsh.wifiPassWord);
        wifiAddrLineEdit->setText(getVpnIpAddress());
        ssidComboBox->clear();
        char cmdStr[1024];
        char readStr[1024];
        QString tmp;
        FILE *fp;
        int allwifiitem=0;
        memset(cmdStr,0,1024);
        if(index == 2){
        // 要检查的文件路径
            QString filePath = "/tmp/wifi.txt";
            // 使用 QFile::exists() 方法判断文件是否存在
            if (QFile::exists(filePath)) {
                qDebug() << "文件存在";
                sprintf(cmdStr, "cat /tmp/wifi.txt | wc -l");//Get "auto eth1" line number
                fp = popen(cmdStr, "r");
                memset(readStr,0,256);
                fread(readStr, 1, sizeof(readStr), fp);
                pclose(fp);
                allwifiitem=atoi(readStr);

                for(int i=0;i<allwifiitem;i++)
                {
                    memset(cmdStr,0,256);
                    sprintf(cmdStr, "cat /tmp/wifi.txt | awk 'NR==%d'",i);//Get "auto eth1" line number
                    fp = popen(cmdStr, "r");
                    memset(readStr,0,256);
                    fread(readStr, 1, sizeof(readStr), fp);
                    pclose(fp);
                    tmp=readStr;
                    tmp=tmp.left(tmp.size()-1);
                    if(!tmp.isEmpty())
                    {
                        ssidComboBox->addItem(convertHexInString(tmp), i);
                    }
                }
            }
        }
    }


}

/* 页面切至第一页 */
void StatusInfoPage::gotoFirstPageSlt()
{
    listWidget->setCurrentRow(0);
    stackedWidget->setCurrentIndex(0);
    statusTabBar->setCurrentIndex(0);
}

/**
 *   创建联系我们页面
 */

void StatusInfoPage::CreateConnectUsPage()
{
    QString strMachine,strEngineer,strMachineID;

    qrCodeLabel = new myLabel("");
    if (LCD_HEIGHT == 480) {
        qrCodeLabel->setFixedSize(150, 150);
    } else {
        qrCodeLabel->setFixedSize(300, 300);
    }
    qrCodeLabel->setPixmap(QPixmap(":res/png/QRCode.png").scaled(qrCodeLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QSqlQuery sql_query;

    QString machineNo;
    QString encyStatus;
    QString encyCode;
    QString encyDays;
    QString encyTime;
    QString strEncyInfo;
    sql_query.exec("SELECT * FROM sfio limit 1;");
    bool isShowExpireTime = false;
    while(sql_query.next())
    {
        isShowExpireTime = true;
        machineNo = sql_query.value(0).toString();
        encyStatus = sql_query.value(1).toString();
        encyCode = sql_query.value(2).toString();
        encyDays = sql_query.value(3).toString();
        encyTime = sql_query.value(4).toDateTime().toString("yyyy-MM-dd");
        if(encyStatus =="0"){
            QDateTime encyptDays = QDateTime::fromString(encyTime, "yyyy-MM-dd").addDays(encyDays.toInt());
            strEncyInfo = QString("%1%2").arg(" ").arg(encyptDays.toString("yyyy-MM-dd"));
        }
    }

    strMachine = QString("%1%2").arg(myLan.salesperson).arg(" :");
    strEngineer = QString("%1%2").arg(myLan.telephone).arg(" :");
    strMachineID = QString("%1%2").arg(myLan.device_id).arg(" :");

    machineLbe = new myLineEdit(struCnfg.MachineInfo) ;
    machineLbe->setMaximumHeight(BTN_HEIGHT);
    machineLbe->setMinimumWidth(BTN_WIDTH*3);
    engineerLbe = new myLineEdit(struCnfg.EngineerInfo);
    engineerLbe->setMaximumHeight(BTN_HEIGHT);
    machineIDLbe = new myLineEdit(struCnfg.MachineIDInfo);
    machineIDLbe->setMaximumHeight(BTN_HEIGHT);

    expireTimeLbe = new myLineEdit(strEncyInfo);
    expireTimeLbe->setMaximumHeight(BTN_HEIGHT);
    expireTimeLbe->setReadOnly(true);

    machineLabel = new myLabel(strMachine);
    engineerLabel = new myLabel(strEngineer);
    machineIDLabel = new myLabel(strMachineID);

    expireTimeLabel = new myLabel(myLan.expiration_time+" :");

    setinfoBtn = new myPushButton("" ,myIcon.Message_Warning);
    setinfoBtn->setFixedSize(QSize(BTN_WIDTH/3,BTN_HEIGHT));
    setinfoBtn->hide();

    machineInfoLayout = new QGridLayout(pageContactUs);
    machineInfoLayout->setContentsMargins(80,50,100,250);

//    machineInfoLayout->addWidget(qrCodeLabel,0,0,1,2,Qt::AlignCenter);
//    machineInfoLayout->addWidget(machineIDLabel, 1, 0, 1, 1);
//    machineInfoLayout->addWidget(machineIDLbe, 1, 1, 1, 1);
    machineInfoLayout->addWidget(machineLabel, 2, 0, 1, 1);
    machineInfoLayout->addWidget(machineLbe, 2, 1, 1, 1);
    machineInfoLayout->addWidget(engineerLabel, 3, 0, 1, 1);
    machineInfoLayout->addWidget(engineerLbe, 3, 1, 1, 1);

    if(isShowExpireTime){
        machineInfoLayout->addWidget(expireTimeLabel, 4, 0, 1, 1);
        machineInfoLayout->addWidget(expireTimeLbe, 4, 1, 1, 1);
    }

    connect(setinfoBtn ,SIGNAL(pressed()), this, SLOT(setinfo()));

    connect(machineLbe,SIGNAL(pressed()), this,SLOT(setMachineInfo()));
    connect(engineerLbe,SIGNAL(pressed()),this,SLOT(setEngineerInfo()));
    connect(machineIDLbe ,SIGNAL(pressed()),this, SLOT(setMachineIDInfo()));
}

void StatusInfoPage::setExpireTime(){
    QString machineNo;
    QString encyStatus;
    QString encyCode;
    QString encyDays;
    QString encyTime;
    QString strEncyInfo;
    QSqlQuery sql_query;
    sql_query.exec("SELECT * FROM sfio limit 1;");
    while(sql_query.next())
    {
        machineNo = sql_query.value(0).toString();
        encyStatus = sql_query.value(1).toString();
        encyCode = sql_query.value(2).toString();
        encyDays = sql_query.value(3).toString();
        encyTime = sql_query.value(4).toDateTime().toString("yyyy-MM-dd");
        if(encyStatus =="0"){
            QDateTime encyptDays = QDateTime::fromString(encyTime, "yyyy-MM-dd").addDays(encyDays.toInt());
            strEncyInfo = QString("%1%2").arg(" ").arg(encyptDays.toString("yyyy-MM-dd"));
        }
    }
    machineInfoLayout->addWidget(expireTimeLabel, 4, 0, 1, 1);
    machineInfoLayout->addWidget(expireTimeLbe, 4, 1, 1, 1);
    expireTimeLbe->setText(strEncyInfo);
}

void StatusInfoPage::setinfo()
{
    int ret1 = QDialog::Accepted;
    if (ret1 == QDialog::Accepted) {
        myInputPanel inputDlg(passwdType, 0, 0, 0);
        int ret2 = inputDlg.exec();
        if (ret2 == QDialog::Accepted) {
            QString str = inputDlg.getText();
            if (str =="002009") {
                machineLbe->setPressEnabled(1);
                engineerLbe->setPressEnabled(1);
                machineIDLbe->setPressEnabled(1);
            } else {
                infoWidget->setLabelText(myLan.msg_password_error);
                infoWidget->delayShow();
                myFlow.sleep(1);
                infoWidget->hide();
            }
        }
    }
}

void StatusInfoPage::setMachineInfo()
{
    QString strmachineinfo = strmachineinfo.fromLatin1(struCnfg.MachineInfo);
    myInputMethod inputmachineinfoPanel(myLan.vendor_info,strmachineinfo);
    int ret = inputmachineinfoPanel.exec();
    if (ret == QDialog::Accepted) {
        QString strNewmachineinfo = inputmachineinfoPanel.getText();
        int length = strNewmachineinfo.length();
        if (length > 63) {
            myMessageBox msgBox(MSG_WARN,myLan.msg_length_exceed_limit);
            ret = msgBox.exec();
        } else {
            strcpy(struCnfg.MachineInfo, strNewmachineinfo.toLocal8Bit().constData());
        }
        machineLbe->setText(struCnfg.MachineInfo);
    }
}

void StatusInfoPage::setEngineerInfo()
{
    QString strengineerinfo = strengineerinfo.fromLatin1(struCnfg.EngineerInfo);
    myInputMethod inputengineerinfoPanel(myLan.engineer_info,strengineerinfo);
    int ret = inputengineerinfoPanel.exec();
    if (ret == QDialog::Accepted) {
        QString  strNewengineerinfo = inputengineerinfoPanel.getText();
        int length = strNewengineerinfo.length();
        if (length > 63) {
            myMessageBox msgBox(MSG_WARN,myLan.msg_length_exceed_limit);
            ret = msgBox.exec();
        } else {
            strcpy(struCnfg.EngineerInfo, strNewengineerinfo.toLocal8Bit().constData());
        }
        engineerLbe->setText(struCnfg.EngineerInfo);
    }
}

void StatusInfoPage::setMachineIDInfo()
{
    QString strmachine_idinfo = strmachine_idinfo.fromLatin1(struCnfg.MachineIDInfo);
    myInputMethod inputmachine_idinfoPanel(myLan.devId,strmachine_idinfo);
    int ret = inputmachine_idinfoPanel.exec();
    if (ret == QDialog::Accepted) {
        QString strNewmachine_idinfo = inputmachine_idinfoPanel.getText();
        int length = strNewmachine_idinfo.length();
        if (length > 63) {
            myMessageBox msgBox(MSG_WARN,myLan.msg_length_exceed_limit);
            ret = msgBox.exec();
        } else {
            strcpy(struCnfg.MachineIDInfo, strNewmachine_idinfo.toLocal8Bit().constData());
        }
        machineIDLbe->setText(struCnfg.MachineIDInfo);
    }
}

/* 响应大数据功能使能按钮 */
void StatusInfoPage::onStatisticEnablePressedSlt()
{
    if (statisticEnable->isChecked()) {
        struCnfg.nStatisticEnable = 1;
    } else {
        struCnfg.nStatisticEnable = 0;
    }
    statusTabBar->setCurrentIndex(statusTabBar->count()-1);
}

/**
 *  创建一键自动校正页面 （仅适用于T1机型）
 */
void StatusInfoPage::CreateAutoCalibratePage()
{
    QString infoStr;

    /*
    infoStr.sprintf("自动校正会影响色选效果，请谨慎操作!\n"
                    "请确保按以下步骤进行：\n"
                    "1、确保分选室玻璃干净透明。\n"
                    "2、确保背景板干净无异物。\n"
                    "3、将背景板从蓝色面翻转至白色面。\n"
                    "4、点击自动校正按钮开始一键校正, 等待校正完成。\n"
                    "5、将背景板从白色面翻转至蓝色面。\n"
                    "6、校正结束。");
    */
    infoStr = QString("%1\n\n%2\n%3\n%4\n%5\n%6\n%7")
                    .arg(myLan.auto_calibrate_tips)
                    .arg(myLan.auto_calibrate_step1)
                    .arg(myLan.auto_calibrate_step2)
                    .arg(myLan.auto_calibrate_step3)
                    .arg(myLan.auto_calibrate_step4)
                    .arg(myLan.auto_calibrate_step5)
                    .arg(myLan.auto_calibrate_step6);


    autoLabel = new myLabel(infoStr, pageAutoCalibrate);

    QVBoxLayout *mainLayout = new QVBoxLayout(pageAutoCalibrate);
    mainLayout->addWidget(autoLabel);
}

/* 一键自动校正 */
void StatusInfoPage::autoCalibrateSignal()
{
    myMessageBox *msgBox = new myMessageBox(MSG_QUES, myLan.auto_calibrate_step3);
    int ret = msgBox->exec();
    if (ret == QDialog::Accepted) {
        infoWidget->setLabelText(myLan.msg_auto_calibrating);
        infoWidget->delayShow();

        /* start auto calibration */
        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {//校正系数获取
            int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
            MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr, 0, 1,
                           200, 200, 200, 3);
        }
        myFlow.sleep(2);
        for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal; i++) {//校正系数保存
            int nUnitAddr = struCnfg.struLevelInfo[struGsh.nLevel].nUnitId[i];
            MySerial.com1Write(CMD_UNIT_CAMERA, UNIT, struGsh.nLevel, nUnitAddr, 0, 3,
                               200, 200, 200, 3);
        }
        infoWidget->hide();

        msgBox = new myMessageBox(MSG_QUES, myLan.auto_calibrate_step5);
        msgBox->exec();
    }
}

/* howto page test */
void StatusInfoPage::CreateGetHowToWidget()
{
    /* page init */
//    helpview = new QWebView(pageHowTo);
//    helpview->setUrl(QUrl::fromLocalFile("/userdata/help/index.html"));

//    QVBoxLayout *mainLay = new QVBoxLayout(pageHowTo);
//    mainLay->addWidget(helpview);
}

/* 响应获取系统帮助按钮 */
void StatusInfoPage::reloadHowTo()
{
    QTime time;
    time.start();

//    helpview->setUrl(QUrl::fromLocalFile("/userdata/help/index.html"));
}

/*!
 * \brief StatusInfoPage::sendAlarmSms
 *        有报警时发送提示短信
 */
void StatusInfoPage::sendAlarmSms()
{
    m_pageSetSms->sendAlarmSms();
}

/*!
 * \brief StatusInfoPage::updateAlarmStatus
 *        更新报警状态
 */
void StatusInfoPage::updateAlarmStatus()
{
    m_pageSetSms->updateAlarmStatus();
}


qint64 StatusInfoPage::getQByteArrayMax(QByteArray a, int byteNum)
{
    quint64 max = 0;
    quint64 tmp = 0;

    if (byteNum > 8) {
        return -1;
    }

    if (a.size() >= byteNum) {
        for (int i = 0; i < a.size(); i += byteNum) {
			tmp = 0;
            for (int j = byteNum - 1; j >= 0; j--) {
                tmp += ((quint64)a.at(i+j) << (8*(byteNum - 1 - j)));
            }
            if (max < tmp) {
                max = tmp;
            }
        }
    }

    return max;
}
