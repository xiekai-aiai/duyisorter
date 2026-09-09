/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mainwidget.h
 * \brief       程序主界面头文件
 * \date        2015.01.14
 */
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QApplication>
#include <QEvent>

#include <QWidget>
#include <QBitmap>
#include <QPainter>
#include <QBrush>

#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputpanel.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputmethod.h"
#include "rgb/factoryset.h"
#include "statusinfopage.h"
#include "manageschemepage.h"
#include "rgb/managenewscheme.h"
#include "common/mono_shared/copymajorparams.h"
#include "systeminitialization.h"
#include "manageengineer.h"
#include "rgb/setmaterialsens.h"
#include "common/mono_shared/setfeed.h"
#include "common/mono_shared/autosetrs.h"
#if AI_MODE == 1
#include "rgb/aianalysis.h"
#else
#include "rgb/aimainwidget.h"
#endif
#include "setwipe.h"
#include "globalparams.h"
#include "mythread.h"
#include "globalflow.h"
#include "mydelaycode.h"
#include "common/mono_shared/camerasignal/ejecttimesthread.h"
#include "rgb/sensbias.h"
#include "rgb/autosigwidget.h"
#include "rgb/autosigwidget.h"
#include "rgb/schemeparams/myschemeparams.h"
#include "mycontact.h"
#include "myuploader.h"
#include "src/rgb/throughput.h"
#include "src/unsupervisedaimainwidget.h"
#include "src/rgb/setsorterconsistencewidget.h"
#include "src/rgb/autoanalysiswidget.h"
#include "src/qdatabase.h"
#include "aismainwidget.h"
#include "src/aimodelset.h"

#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif

#if ENABLE_CAPTURE_SCREEN == 1
    #include <QPixmap>
    #include <QtGui>
#endif

class Qtimer;
class myApplication;

/*----------设置界面索引号-------------*/
typedef enum{
    SM_COMM_PAGE,                       // 通信检测页面索引号
    SM_COMM_LIST_PAGE,                  // 通信检测列表页面索引号
    SM_INIT_PAGE,                       // 初始化页面索引号
    SM_PREHEAT_PAGE,                    // 预热页面索引号
    SM_HOME_PAGE,                       // 主页面索引号
    SM_HOME_TEA_PAGE,                   // 茶叶机型主页面索引号
//    SM_SET_FEED_PAGE,                   // 供料设置页面索引号
//    SM_SET_WIPE_PAGE,                   // 清灰设置页面索引号
//    SM_TEST_EJECT_PAGE,                 // 喷阀自检页面索引号
    SM_STATUS_INFO_PAGE,                // 系统状态页面索引号
    SM_SELECT_SCHEME_PAGE,              // 方案选择页面索引号
//    SM_MANAGE_SCHEME_PAGE,              // 管理方案页面索引号
    SM_MANAGE_NEW_SCHEME_PAGE,          // 管理新方案页面索引号
    SM_SET_PURVIEW,                     // 权限管理页面索引号
//    SM_SET_FACTORY,                     // 厂家页面索引号
//    SM_SCHEME_PARAM_PAGE,               // 方案参数页面索引号
    SM_AI_PAGE,                         // 智能分析页面索引号
    SM_ENGINEER_PAGE,                   // 工程模式页面索引号
//    SM_SET_SENS_PAGE,                   // 灵敏度页面索引号
    SM_AUTO_PAGE,                       // 一键自动页面索引号
    SM_COPY_MAJOR_PARAMS_PAGE ,         // 复制重要参数页面索引号
    SM_SENS_BIAS_PAGE,
    SM_MOTOR_CTRL_PAGE,
    SM_AUTO_SIGNAL_PAGE,
    SM_VIDEO_PAGE,                       // 视频图像页面索引号
    SM_THROUGHPUT_PAGE,                  // 实时产量页面索引号
    SM_AI_MODEL_SET_PAGE,                //Ai模型设置页面索引号
    SM_UNSUPERVISED_AI,
    SM_CONSISTENCE_PAGE,
    SM_AUTO_ANALYSIS_PAGE                //碎米率分析页面索引号

}settingWindowIndex;




typedef enum{
    HOME_SCHEME_PAGE_INDEX,                   // 样本选择页面索引号
    HOME_SENS_PAGE_INDEX,                    // 设置灵敏度页面索引号
    HOME_FEED_PAGE_INDEX,                    // 供料速度页面索引号
    HOME_EJECT_PAGE_INDEX,                    // 喷阀检测页面索引号
    HOME_WIPE_PAGE_INDEX,                     // 设置清灰页面索引号
    HOME_ALGO_PAGE_INDEX,                     // 算法设置页面索引号
    HOME_OPRLVL_PAGE_INDEX,                   // 操作级别页面索引号
    HIGH_SET_OPR_PAGE_INDEX,                   // 高级设置页面索引号
    HIGH_SET_AI_PAGE_INDEX                    // 高级设置AI页面索引号


}homePageIndex;

/* 相机信号页面索引号 */
#define SM_CAMERA_SIGNAL_PAGE   SM_AI_PAGE

/***************************************************************/
// 喷阀自检线程
class TestEjectThread : public QThread
{
    Q_OBJECT
public:
   explicit TestEjectThread();
   void stop();

signals:
   void ejectorChangeSig();         // 喷阀自检数据变化信号
protected:
    void run();
private slots:
    void ejectTestDoneSlt();
private:
    volatile bool stopped;
};

/*!
 * \brief 主界面类
 */
class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

    void CreateHomeWidget(void);            // 创建主界面
    void CreateHomeTeaWidget(void);         // 创建T1茶叶小机型主界面
    void CreateSetWipeWidget(void);         // 创建设置清灰界面
    void CreateSetFeedWidget(void);         // 创建供料设置页面
    void CreateSetPurviewWidget(void);      // 创建权限设置页面
    void CreateSelectSchemeWidget(void);    // 方案选择设置页面
    void CreateTestEjectWidget(void);       // 喷阀自检页面
    void CreateMotorCtrlWidget(void);       // 电机控制页面

    void machineTestForI(void);             // I机型老化测试

    void CreatePageOprTypeWidget(void);


protected:
#if ENABLE_CAPTURE_SCREEN == 1
    void keyPressEvent(QKeyEvent *event);
#endif

signals:
    void goToComPageSig();              // 通信检测
    void goToInitPageSig();             // 系统初始化
    void goToPreheatPageSig();          // 预热
    void factoryToFirstSig();           // 工厂模式切至第一页
    void AIToFirsteSig();               // 智能分析切至第一页
    void schParToFirstSig();            // 方案设置切至第一页
    void statusToFirstSig();            // 系统状态切至第一页
    void sManageEngineerSig();          // 工程模式切换到主界面
    void manageSchemeUpdateListWidgetSig(); // 方案管理页面离别更新
    void factoryUpdateSig();                // 更新厂家模式页面
    void cameraSignalRefreshSig();          // 相机信号页面刷新显示的信号
    void ejectTestDonesig();                // 完成每一个喷阀的自检的信号

    void alarmReady();
    void statisticFinished();
    void openFeederSig(int trigger);
    void closeFeederSig(int trigger);
    void hideOneKeyFeedSig();
    void showOneKeyFeedSig();

    void shutdownCurveBg();
private slots:

    void onHomePageStackedWidgetIndexChangedSlt(int index);
    void GoToOprTypePageSlt();


    void onBackgroundLightOnSlt();
    void onFunctionKeyChangedSlt(int index);
    void onStackedWidgetIndexChangedSlt(int index);
    void TimeOutSlt();                  // 定时器
    void showPressureAlarmMsgSlt(bool isShow);
    void onStartWipeSlt(int trigger); // 清灰请求响应

    void goToCommPageSlt();             // 通信检测
    void goToCommListPageSlt();         // 通信检测列表
    void goToInitPageSlt();             // 系统初始化
    void goToPreheatPageSlt();          // 预热

    void GoToHomePageSlt();             // 返回主界面
    void GoToManageNewSchemePageSlt();  // 新建方案
    void goToCopyMajorParamsPageSlt(void);  // 复制重要参数

    void onBackToHomeBtnClicked();      // 返回主界面
    void onStatInfoBtnClickedSlt();     // 系统状态
    void onStatInfoBackBtnClicked();    // 系统状态返回
    void onSystemHaltBtnClickedSlt();   // 系统关闭
    void onFeedSwitchBtnClickedSlt();   // 供料开关
    void onBeltSwitchBtnClickedSlt();   // 皮带开关
    void onSaveParamBtnClickedSlt();    // 参数保存
    void onSetPurviewBtnClickedSlt();   // 权限设置
    void onOneKeyFeedBtnPressed(void);  // 一键放料
    void state_machine_engineer_info(); // 显示机器和工程师信息

    // 主页面
    void onSelectSchemeBtnClickedSlt(); // 方案选择
    void onSetFeedBtnClickedSlt();      // 供料设置
    void onSetWipeBtnClickedSlt();      // 清灰设置
    void onTestEjectBtnClickedSlt();    // 喷阀自检
    void onAIComputeBtnClickedSlt(int nIndex);  // 智能计算
    void onSchemeParamBtnClickedSlt();  // 方案参数
    void onSchemeParamBtnClickedSlt1();  // 方案参数

    void onManageSchemeBtnClickedSlt(); // 管理方案
    void onManageSchemeBtnClickedSlt1(); // 管理方案

    void onEnterFactoryBtnClickedSlt(); // 工厂页面
    void onSetSensBtnClickedSlt();      // 灵敏度页面
    void onCameraSignalBtnClicked();    // 相机信号页面
    void onAutoSetBtnClickedSlt();      // 一键智能页面
    void onVideoBtnClickedSlt();        // 视频图像
    void onThroughputBtnClickedSlt();   // 实时产量
    void onAiModelSetBtnClickedSlt();   // ai模型设置

    void updateHomePage();              // 更新通用机型主页面
    void onAiSetBtnClickedSlt();

    // T1机型定制主页
    void updateHomeTeaPage();           // 更新T1小机型主页面
    void setFeederValueSlt();           // 供料量设置
    void setSensValueSlt();             // 灵敏度设置
    void setPercentValueSlt();          // 病斑设置
    int  getPrefixedProfileIndex(QString strProfile);

    // 方案选择
    void onOkSelectSchemeBtnClickedSlt();
    void onCancelSelectSchemeBtnClickedSlt();

    // 清灰总设置
    void onCancelWipeBtnClickedSlt();
    void onWipeModeBtnClickedSlt();
    void updateWipePage();

    // 供料设置
    void onCancelSetFeedBtnClickedSlt();
    void feederValueChange(int indexOfFeeder);
    void updateFeederPage();
    void updateFeederValue();
    void onFeederMonitorToggled(bool isChecked);

    // 喷阀自检
    void ejectPageDisplayLcdNumerSlt();
    void onMinusBtnClickedSlt();
    void onAddBtnClickedSlt();
    void onStartStopBtnClickedSlt();
    void onLoopCbxClickedSlt();
    void onLayerBtnClickedSlt();
    void onHighSpeedCbxClickedSlt();
    void onLowSpeedCbxClickedSlt();
    void updateLayerBtnSlt();
    void onCancelTestEjectBtnClickedSlt();
    void ejectPageThreadStartSlt();
    void updateEjextorSlider();

    // 权限设置
    void onCancelSetPurviewBtnClickedSlt();
    void onUserOperatorBtnClickedSlt();
    void onUserEngineerBtnClickedSlt();
    void onUserFactoryBtnClickedSlt();

    // 标题栏设置
    void onHideTitleBtnsSlt();

    // 网络同步上传
    void createNetworkUploader(void);

    // 响应开关下料请求
    void onCloseFeederSlt(int trigger);
    void onOpenFeederSlt(int trigger);

    // 响应灵敏度页面信号
    void onGoToSensBiasPageSlt(void);
    void onGoToSetSensPageSlot(void);

    // 电机控制页面
    void onMotorPageStartCbxSlt();
    void onMotorPageBackBtnSlt();

    //响应色选管家页面信号
    void onGoToVideoPageSlot();

    //响应碎米率分析页面信号
    void onGoToAutoAnalysisPageSlot();

    void createGetEjectTimesThread(void);

    void onOprTypeBtnClickedSlt();  // 操作级别

    void onCustomOprBtnClickedSlt();
    void onHighSetOprBtnClickedSlt();


private:
    updateStatusThread *updateStatus;       // 清灰线程
    getStatusThread *getStatus;             // 获取状态
    QThread *uploaderThread;
    g_Config *config;
    bool m_bIsFastTest;                     // 标志是否为高速通信自检

    QString bkPicName;                      // 主窗口背景图片的名称
    QPoint normalTopLeft;                   // 正常显示的窗口左上角坐标
    MyDevMonitor *myDevMonitor;             // 设备状态监控网络服务

    // 标题栏
    myPushButton     *statInfoBtn;
    QFrame           *lineTitle;
//    myPushButton     *backToHomeBtn;
//    myPushButton     *machineInfoBtn;

//    myPushButton     *oprModeBtn;
//    myPushButton     *mainOprBtn;
//    myPushButton     *videoBtn;

    myGroupBox      *leftBackGbx;


    // 主页面操作选择空间
    myListWidget      *operateListWidget;
    myPushButton      *operateButton[7];

    QVBoxLayout       *operatePageHBLayout;
    QHBoxLayout       *operatorLayout;
//    QGridLayout       *operatorLayout;

    QStackedWidget      *homePageStackedWidget;
    QWidget          *pageOprType;
    myPushButton     *customOprButton;
    myPushButton     *highSetOprButton;


    // 状态栏
//    myPushButton     *systemHaltBtn;
    QToolButton     *systemHaltBtn;
    QToolButton     *saveParamBtn;
    QToolButton     *feedSwitchBtn;

//    myPushButton     *feedSwitchBtn;
//    myPushButton     *beltSwitchBtn;
//    myPushButton     *saveParamBtn;
//    myPushButton     *setPurviewBtn;
    QToolButton      *videoBtn;
    myPushButton     *aiModelSetBtn;
    QFrame           *lineStatus;
//    myPushButton     *m_oneKeyFeedBtn;
    bool m_bIsFeed;
    myPushButton     *aiSetBtn;

    // 中控界面
    QStackedWidget      *stackedWidget;
    communication       *pageComm;
    communicationList   *pageCommList;
    initialization  *pageInit;
    preheat         *pagePreheat;
    QWidget         *pageHome;
    QWidget         *pageHomeTea;
    QWidget         *pageSetFeed;
    QWidget         *pageHalt;
    QWidget         *pageSaveParam;
    QWidget         *pagePurview;
    QWidget         *pageSetWipe;
    QWidget         *pageTestEject;
    QWidget         *pageSelectScheme;
    QWidget         *pageMotorCtrl;
    autoSigWidget   *pageAutoSigWid;
    StatusInfoPage  *pageStatusInfo;
    ManageSchemePage *pageManageScheme;
    ManageNewScheme *pageManageNewScheme;
    CopyMajorParams *pageCopyMajorParams;
    factorySet      *pageFactory;
    MySchemeParams  *pageSchemeParams;
#if AI_MODE == 1
    AIAnalysis      *pageAI;
#else
    AIMainWidget    *pageAI;
#endif
    manageEngineer  *pageEngineer;
    setMaterialSens *pageSens;
    setFeedPage     *pageSetFeedRS;
    autoSetPage     *pageAutoSet;
    SensBias        *PageSensBias;
    MyVideo         *pageVideo;
    AisMainWidget   *pageAiSet;
    Throughput      *pageThroughput;
    UnsupervisedAIMainWidget *pageUnsupervisedAIWidget;
    setSorterConsistenceWidget *pageConsistenceWidget;
    AutoAnalysisWidget *pageAutoAnalysisWidget;
    QGridLayout     *mainGridLayout;

    AiModelSet      *aiModelSetWidget;
    // 主页面跳转控件
    myPushButton    *selectSchemeBtn;
    myPushButton    *manageSchemeBtn;
    myPushButton    *setFeedBtn;
    myPushButton    *setSensBtn;
    myPushButton    *setWipeBtn;
    myPushButton    *testEjectBtn;
    myPushButton    *AIComputeBtn;
    myPushButton    *schemeParamBtn;

    myPushButton    *enteryFactoryBtn;
    myPushButton    *autoSetBtn;
    myPushButton    *cameraSignalBtn;
    myPushButton    *throughputBtn;
    myGroupBox      *operatorGbx;
    myGroupBox      *engineerGbx;
    QGridLayout     *homeLayout;
//    QGridLayout     *operatorLayout;
    QGridLayout     *engineerLayout;

    // 茶叶定制页面控件
    myPushButton    *schemeBtn;
    myLabel         *feederLabel;
    myLabel         *sensLabel;
    myLabel         *percentLabel;
    MyValueBar      *feederValue;
    myLineEdit      *sensValue;
    myLineEdit      *percentValue;
    myGroupBox      *operatorGroup;

    // 方案选择控件
    myListWidget      *listWidget;
    myPushButton      *okSelectSchemeBtn;
    myPushButton      *cancelSelectSchemeBtn;
    int               nProfileTotal;
    QGridLayout       *schemePageGridLayout;
    QHBoxLayout       *schemePageHBLayout;
    QSpacerItem       *horizontalSpacer;

    // 设置供料控件
    QSignalMapper  *feederSignalMapper;
    myLabel        *feedLabel[10];
    myLineEdit     *feedLbe[10];
    myPushButton   *cancelSetFeedBtn;
    MyCheckBox     *feederMonitorBtn;
    QHBoxLayout    *feederHBLayout[10];
    QGridLayout    *feederPageGridLayout;
    QHBoxLayout    *feederPageHBLayout;
    QSpacerItem    *horizontalSpacer_1;

    // 设置清灰控件
    MyTabBar            *mainWipeTabWidget;
    QStackedWidget      *wipeStackedWidget;
    WipeBrushWidget		*pageWipeBrush;
    WipeWindWidget		*pageWipeWind;
    WipeWaterWidget		*pageWipeWater;
    WipeModeWidget      *pageWipeMode;
    myPushButton        *cancelSetWipeBtn;
    myPushButton        *wipeModeBtn;

    // 喷阀自检页面
    myLCDNumber *lcdNumber;
    mySlider     *ejectorSlider;
    myLabel     *currentLabel;
    myPushButton *minusBtn;
    myPushButton *addBtn;

    myCustomCheckBox *loopCbx;
    myPushButton     *layerBtn;
    myPushButton *startBtn;
    myPushButton *stopBtn;
    myPushButton *cancelTestEjectBtn;
    myCustomCheckBox *highSpeedCbx;
    myCustomCheckBox *lowSpeedCbx;

    bool        bFlagLoop;
    int         nDisplayNum;
    int         nMaxEjector;
    int         ejectTestInterval;
    int         ejectTestInterval2;
    int         aiEjectDelayTime;

    TestEjectThread    *threadStart;

    QGridLayout  *ejectPageGridLayout;
    QHBoxLayout  *ejectPageHBlayout1;
    QHBoxLayout  *ejectPageHBlayout2;
    QHBoxLayout  *ejectPageHBlayout3;
    QHBoxLayout  *ejectPageHBlayout4;
    QHBoxLayout  *ejectPageHBlayout5;
    QHBoxLayout  *ejectPageHBlayout6;
    QSpacerItem    *horizontalSpacer_3;
    QSpacerItem    *horizontalSpacer_4;
    QSpacerItem    *horizontalSpacer_5;
    QSpacerItem    *horizontalSpacer_6;
    QSpacerItem    *horizontalSpacer_7;
    QSpacerItem    *horizontalSpacer_8;

    QSpacerItem    *horizontalSpacer_10;
    QSpacerItem    *horizontalSpacer_11;

    // 权限设置
    myPushButton *userOperatorBtn;
    myPushButton *userEngineerBtn;
    myPushButton *userFactoryBtn;
    myPushButton *cancelSetPurviewBtn;
    quint8 pageIndex;
    QString titleStr;

    QGridLayout  *purviewPageGridLayout;
    QSpacerItem  *horizontalSpacer_9;

    QHBoxLayout  *purviewPageHBLayout1;
    QHBoxLayout  *purviewPageHBLayout2;
    QHBoxLayout  *purviewPageHBLayout3;
    QHBoxLayout  *purviewPageHBLayout4;

    // 点击控制页面
    myCustomCheckBox    *m_motorPageStartCbx;
    myPushButton        *m_motorPageBackBtn;
};

class myApplication : public QApplication
{
public:
    myApplication(int & argc, char ** argv);
    bool qwsEventFilter(QObject *w, QEvent *event);

};

#endif // MAINWIDGET_H
