/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        factoryset.h
 * \brief       彩色机型厂家设置界面头文件
 * \date        2015.01.14
 */
#ifndef FACTORYSET_H
#define FACTORYSET_H

#include <QMainWindow>
#include <QtCore>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QListWidget>
#include <QTime>
#include <QLayout>

#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mymessagebox.h"
#include "common/mywid.h"
#include "setcolorcamera.h"
#include "setmachinemode.h"
#include "common/mono_shared/venderset/lampctrl.h"
#include "fpga.h"
#include "upgradefpga/upgradeprogramwidget.h"
#include "globalparams.h"
#include "globalflow.h"
#include "common/mono_shared/venderset/feederremotectrl.h"
#include "autosiglistwidget.h"
#include "degausstime.h"
#include "throughput.h"
#include "tickmodewidget.h"
#include "setencryt.h"

enum{
    langMaxNum = 6,
    userModeNum = 3
};

/*!
 * \brief 彩色机型厂家设置界面类
 */
class factorySet : public QMainWindow
{
    Q_OBJECT
public:
    factorySet(QWidget *parent = 0);
    ~factorySet();
    void createFactorySetPage(void);        //! 创建工厂设置界面
    void createSetLanguagePage(void);       //! 创建设置语言界面
    void createAgeingTestPage(void);        //! 创建老化测试界面
    void createOutputTestPage(void);        //! 创建产量测试界面
    void createSetFPGAModePage(void);       //! 创建设置下位机启动模式界面
    void createBacklightPasswordPage(void); //! 创建设置下位机启动模式界面
    void createSetFeederVoltage(void);      //! 创建设置振动器高低电压界面
    void createDegaussTimePage(void);       //! 创建消磁时间设置页面
    void setFPGAModeParams();               //! FPGA模式的初始化
    void setAiParams();                     //! ai参数的初始化
//    void onMachineDecrpt(QString);
private slots:
    void backToFactorySet();
    void onFactoryBackBtnClicked();
    void onSetColorCameraBtnClicked();



    //! 设置语言界面的槽函数
    void onSetLanguageBtnClicked();
    void onLangSureBtnClicked();
    void onLangCancelBtnClicked();

    //! 设置机型按钮的响应函数
    void onSetMachineModeBtnClicked();

    //! 清零累计运行时间的响应函数
    void onClearRunningTimeBtnClicked();

    //! 校准屏幕的响应函数
    void onCalibrationBtnClicked();

    //! 老化测试界面的槽函数
    void onAgeingTestBtnClicked();
    void onAgeingTestSureBtnClicked();
    void onAgeingTestBackBtnClicked();
    void onAgeingTestValueChangedSlt(int value);
    void onAgeingTestFlagChangedSlt(int index);

    //! 产量测试界面的槽函数
    void onOutputTestBtnClicked();
    void onOutputTestPlusBtnClicked();
    void onOutputTestMinusBtnClicked();
    void onOutputTestStartBtnClicked();
    void onOutputTestBackBtnClicked();
    void outputTimeoutSlt();

    //! 转到启动函数
    void onSwitchToCFBtnClicked();

    //! 上位机远程升级函数
    void onUpdateARMBtnClicked();

    //! 下位机远程升级函数
    void onUpdateFPGABtnClicked();

    //! 设置FPGA启动模式界面的槽函数
    void onSetFPGAStartModeBtnClicked();
    void onSetModeSureBtnClicked();
    void onSetModeBackBtnClicked();
    void onIntModeChangedSlt(int index);
    void onColorModeChangedSlt(int index);

    //! 设置使能点亮背光输入密码界面的槽函数
    void onSetBacklightPasswordBtnClicked();
    void onSetBacklightPasswordEnableBtnClicked();
    void onSetNewPasswordClicked();
    void onSetNewPasswordConfirmClicked();
    void onSetBacklightPasswordSureBtnClicked();
    void onSetBacklightPasswordCancelBtnClicked();

    //! 设置振动器高低电压界面的槽函数
    void onSetFeederVolatageBtnClicked();
    void onSetFeederVoltageSureBtnClicked();
    void onSetFeederVoltageBackBtnClicked();
    void onSetFeederVoltageChangedSlt(int index);
    void onAutoFeedEnablePressedSlt();
    void onStatisticEnablePressedSlt();

    //! 灯控设置页面
    void onLampCtrlBtnClicked();

    //! 设置消磁、截断时间页面
    void onDegaussTimeBtnClicked();

    void onDegaussTimeBackBtnClicked();

    //! 料位外接控制
    void onFeederCtrlBtnPressed(void);

    //! 吞吐量页面
    void onThroughPutBtnPressedSlt();

    //! 剔除模式页面
    void onTickModeBtnPressedSlt();

    void onEncryptBtnBtnClickedSlt();

    void onAiEnbaleChxBtnClicked();
    void onAiSetBtnPressedSlt();
    void onImgFetchHeightEditPressedSlt();
    void onImgInferHeightEditPressedSlt();
    void onImgPicHeightEditPressedSlt();
    void onImgVideoHeightEditPressedSlt();
    void onSetAiSureBtnClicked();
    void onSetAiBackBtnClicked();

public slots:
    void factoryUpdateSlt();
    void gotoFirstPageSlt();
    void onStackedWidgetIndexChangedSlt(int index);

signals:
    void goToHomePage();
    void sonIntGoToFirstSig();          // 子界面返回至第一页
    void refreshMachineModeDisplaySig();// 刷新机型设置界面的显示
    void refreshMachineModeDisplaySigMaize();// 刷新机型设置界面的显示
    void hideTitleBtns();               // 隐藏标题栏上的按钮

private:
    /* 标志厂家设置界面不同页面的索引号 */
    enum{
        FACTORY_SET_PAGE,           /*!< 厂家设置主界面 */
        SET_COLOR_CAMERA_PAGE,      /*!< 设置彩色相机 */
        SET_LANG_PAGE,              /*!< 设置界面语言 */
        SET_PRODUCT_PAGE,           /*!< 设置机型 */
        AGEING_TEST_PAGE,           /*!< 老化测试 */
        OUTPUT_TEST_PAGE,           /*!< 产量测试 */
        UPDATE_FPGA_PAGE,           /*!< 下位机升级 */
        SET_START_MODE_PAGE,        /*!< 设置启动模式 */
        SET_BACKLIGHT_PASSWORD_PAGE,/*!< 设置背光密码 */
        SET_FEEDER_VOLTAGE,         /*!< 设置振动器电压 */
        LAMP_CTRCL_PAGE,            /*!< 设置灯控 */
        DEGAUSS_TIME_PAGE,          /*!< 设置消磁时间 */
        FEEDER_REMOTE_CONTROL_PAGE, /*!< 振动器外接控制 */
        AUTO_SIG_LIST_PAGE,         /*!< 信号校正参数列表 */
        THROUGH_PUT_PAGE,           /*!< R+通过量测试页面 */
        TICK_MODE_PAGE,             /*!< R+精准剔除参数页面*/
        AI_SET_PAGE,                /*!< ai参数页面*/
    };

    QStackedWidget *stackedWidget;
    QWidget *factorySetWidget;
    g_Config *config;

    //! 厂家设置主界面
    myPushButton *setColorCameraBtn;
    myPushButton *setLanguageBtn;
    myPushButton *setMachineModeBtn;
    myPushButton *clearRunningTimeBtn;
    myPushButton *calibrationTSBtn;
    myPushButton *ageingTestBtn;
    myPushButton *outPutTestBtn;
    myPushButton *updateARMBtn;
    myPushButton *updateFPGABtn;
    myPushButton *setFPGAStartModeBtn;
    myPushButton *switchToCFBtn;
    myPushButton *setBacklightPasswordBtn;
    myPushButton *setFeederVoltageBtn;
    myPushButton *factorySetBackBtn;
    myPushButton *lampCtrlBtn;
    myPushButton *autoFeedEnable;
    myPushButton *statisticEnable;
    myPushButton *degaussTimeBtn;
    myPushButton *m_feederCtrlBtn;
    myPushButton *m_througPutBtn;
    myPushButton *m_tickModeBtn;

    myPushButton *m_encryptBtn;
    myPushButton *m_setNetWorkBtn;
    myPushButton *m_sysHelpBtn;
    myPushButton *m_sysInfoBtn;

    myPushButton *setAiBtn;

    QGridLayout *upLay;
    QHBoxLayout *downLay;
    QVBoxLayout *mainLay;

    //! 语言设置界面的控件
    QWidget *langWidget;
    QListWidget *langList;
    myPushButton *langSureBtn;
    myPushButton *langCancelBtn;
    quint32 nLangIndex[MAX_LANG];
    QListWidgetItem *items[MAX_LANG];

    //! 老化测试
    QWidget *ageingTestWidget;
    myMutex *runningModeGroup;
    myValueBar *ageingTestValueBar;
    myPushButton *ageingTestSureBtn;
    myPushButton *ageingTestBackBtn;
    quint8 m_nDetectFre;
    quint8 m_nMachineTestFlag;

    //! 产量测试
    QWidget *outputTestWidget;
    myPushButton *outputTestPlusBtn;
    myPushButton *outputTestMinusBtn;
    myPushButton *outputTestStartBtn;
    myPushButton *outputTestBackBtn;
    QSlider *outputTestSlider;
    QLCDNumber *outputTestTimeNum;
    QTimer *outputTestTimer;
    quint8 m_nTestTime;

    //! 下位机启动模式
    QWidget *setFPGAUserModeWidget;
    myMutex *intModeGroup;
    myMutex *colorModeGroup;
    myPushButton *setModeSureBtn;
    myPushButton *setModeBackbtn;
    quint8 m_nIntMode;
    quint8 m_nColorMode;

    //! 设置使能点亮背光密码
    QWidget *setBacklightPasswordWidget;
    myCustomCheckBox *enablePasswordChx;
    myPushButton *setBacklightPasswordSureBtn;
    myPushButton *setBacklightPasswordCancelBtn;
    bool tmpEnablePassword;

    myLabel *newPasswordLabel;
    myLabel *newPasswordConfrimLabel;
    myLineEdit *newPasswordLineEdit;
    myLineEdit *newPasswordConfirmLineEdit;

    QString tmpPassword;
    QString tmpPasswordConfirm;

    //! 设置振动器电压
    QWidget *setFeederVoltageWidget;
    myMutex *feederVoltageGroup;
    myPushButton *setFeederVoltageSureBtn;
    myPushButton *setFeederVoltageBackbtn;
    quint8 m_nFeederVoltage;

    //! FPGA升级
    UpgradeProgramWidget *upgradeFpgaWidget;

    //! 灯控设置
    LampCtrl *pageLampCtrl;

    //! 振动器外接控制
    FeederRemoteCtrl *m_feederCtrlWidget;

    //! 信号校正列表
    autoSigListWidget *m_autoSigListWidget;

    //! 消磁时间
    DegaussTime *m_degaussTimeWidget;

    //! 通过量测试
    Throughput *m_througPutWidget;

    //!剔除模式页面
    tickModeWidget *m_tickModeWidget;

    QWidget *setAiWidget;
    int imgFetchHeight;
    int imgInferHeight;
    int imgPicHeight;
    int imgVideoHeight;

    myPushButton *setAiSureBtn;
    myPushButton *setAiBackbtn;
    bool aiEnable;
    myCustomCheckBox *aiEnbaleChx;

    //图像高度页面
    QWidget           *pageImgHeight;
    myLabel           *imgFetchHeightLbl;
    myLineEdit        *imgFetchHeightEdit;
    myLabel           *imgInferHeightLbl;
    myLineEdit        *imgInferHeightEdit;
    myLabel           *imgPicHeightLbl;
    myLineEdit        *imgPicHeightEdit;
    myLabel           *imgVideoHeightLbl;
    myLineEdit        *imgVideoHeightEdit;
};

#endif // FACTORYSET_H
