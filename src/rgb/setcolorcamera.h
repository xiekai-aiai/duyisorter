/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setcolorcamera.h
 * \brief       彩色机型设置相机界面头文件
 * \date        2015.01.14
 */
#ifndef SETCOLORCAMERA_H
#define SETCOLORCAMERA_H

#include <QTextEdit>
#include <QStackedWidget>
#include "common/mymutex.h"
#include "common/mypushbutton.h"
#include "common/globalconfig.h"
#include "common/myvaluebar.h"
#include "common/mymessagebox.h"
#include "common/myinputpanel.h"
#include "common/mylcdnumber.h"
#include "common/mycustomcheckbox.h"
#include "common/mywid.h"
#include "mycurve.h"
#include "globalparams.h"
#include "globalflow.h"


class WholeCalConfirm;
class setColorCamera;
/*!
 * \brief 彩色机型设置相机界面类
 */
class setColorCamera : public QWidget
{
    Q_OBJECT
public:
    setColorCamera(QWidget *parent = 0);
    ~setColorCamera();

    void createSetColorCameraPage(void);        // 创建彩色相机设置界面
    void createCalibrationSignalPage(void);     // 创建校准彩色信号界面
    void createCalibrationWavePage(void);       // 创建校正波形界面

    void createSetBiasingPage(void);            // 创建设置偏置界面
    void createGainAdjustPage(void);            // 创建模拟增益界面
    void createSensorParametersPage(void);      // 创建传感器参数界面
    void createLensParametersPage(void);        // 创建镜头参数界面
    void createContrapositionPage(void);        // 创建前后视对位界面

    void setAnaGainParams(void);                // 设置模拟增益的值
    void setAnaGainParams2(void);                // 设置模拟增益的值

    void setDigGainUpperParams(void);           // 设置数字增益粗调的值
    void setDigGainUpperParams2(void);           // 设置数字增益粗调的值

    void setDigGainLowParams(void);             // 设置数字增益细调的值
    void setDigGainLowParams2(void);             // 设置数字增益细调的值

    void calDivChannelParams(int beginCh, int endCh);   // 计算象元划分的参数
    void calDivChannelParams_AFATOXIN(int target);      // 黄曲霉毒素
    void calDivChannelParamsMultiView(int beginCh, int endCh);   // 计算象元划分的参数
    void setSensorParams(void);                         // 设置传感器参数界面的值
    void calDivChannelParamsForWhole(int beginCh, int endCh);   // 整通道划象元
    void calDivChannelParamsForHalf(int beginCh, int endCh);    // 半通道划象元
    int getCameraBodyID(int x, int y);
    void refreshCalDisplay(void);               // 刷新信号校正界面的显示内容
    void refreshBiasingDisplay(void);           // 刷新设置偏置界面的显示内容
    void refreshGainDisplay(void);              // 刷新增益界面的显示内容
    void refreshSensorDisplay(void);            // 刷新传感器参数界面的显示内容

    int  signalIsOk(bool bIsFront, int colorChute);     // 判断当前校正信号是否合适
    void adjustDigiGain(int colorChute);
    void resetCalDisplayRSC(void);              // RSC机型下重置校正系数显示
    void resetCalValueRSC(int nVal);            // RSC机型下重置校正系数参数

    //! 新R自动校正
    void calWholeAutoProcess();                 //整机自动校正过程
    void getAdjustSignal();                     // 获取校正波形信号
    void adjustLampLight();                     // 调整灯光
    void adjustGain(int nLevel, int nUnitAddr); // 调整增益
    bool getAdjustAvrForEachUnit(int level, int nUnitAddr, int& averageA, int& averageG, int& averageB);

    void createDivChannelPage(void);            // 创建划分象元界面
    void createChannelListPage(void);           // 创建象元列表界面
    void createChannelRangePage(void);          // 创建象元范围界面
    void refreshDivDisplay(void);               // 刷新划分象元界面的显示内容
    void refreshDivChannelList2(void);

signals:
    void backToMainWindow();

    void sCalibrationCurveStart();
    void sCalibrationCurveStop();

    void sBiasingCurveStart();
    void sBiasingCurveStop();

    void sGainAdjustCurveStart();
    void sGainAdjustCurveStop();

    void sDivChannelCurveStart();
    void sDivChannelCurveStop();

private slots:
    void gotoFirstPageSlt();
    void onMainBackBtnClicked();
    void onStackWidgetIndexChangedSlt(int index);

    /* 信号校准界面的响应函数 */
    void onCalibrationColorSignalClicked();
    void onCalibrationBackBtnClicked();
    void onCalibrationMinusBtnClicked();
    void onCalibrationPlusBtnClicked();
    void onCalObtainBtnClicked();
    void onCalWholeBtnClicked();
    void onCalWaveBtnClicked();
    void onCalibrationTabBarChangedSlt(int index);
    void refreshCalValue();
	void setCalValue(int nValue);
    void onCalibrationViewChangedSlt();

    /* 校正波形界面的响应函数*/
    void onCalibrateWaveBackBtnClicked();
    void onBoardNumChangedSlt();
    void onCalibrateCopyBtnPressed();

    /* 设置偏置界面的响应函数 */
    void onSetBiasingBtnClicked();
    void onSetBiasingBackBtnClicked();
    void onBiasingTabBarChangedSlt(int index);
    void onBiasingValueChangedSlt(int value);
    void onBiasingBoardNumChangedSlt();

    /* 增益调节的响应函数 */
    void onGainAdjustBtnClicked();
    void onGainAdjustBackBtnClicked();
    void onGainAdjustBoardNumChangedSlt();
    void onGainAdjustTabBarChangedSlt(int index);
    //! 模拟增益的响应函数
    void onAnaGainPlusBtnClicked();
    void onAnaGainMinusBtnClicked();
    void onAnaGainLcdNumD2Pressed();
    //! 数字增益的响应函数
    void onDigUpperPlusBtnClicked();
    void onDigUpperMinusBtnClicked();
    void onDigGainLowValueChangedSlt();
    //移动函数
    //! 模拟增益的响应函数
    void onAnaGainPlusBtn2Clicked();
    void onAnaGainMinusBtn2Clicked();
    void onAnaGainLcdNumD22Pressed();
    //! 数字增益的响应函数
    void onDigUpperPlusBtn2Clicked();
    void onDigUpperMinusBtn2Clicked();
    void onDigGainLowValue2ChangedSlt();

    /* 划分象元界面的响应函数 */
    void onDivChannelBtnClicked();
    void onDivChannelBackBtnClicked();
    void onStartChannelPressed();
    void onEndChannelPressed();
    void onDivChannelListBtnClicked();
    void onCameraBodyBtnClicked();
    void onDivChannelRangeBtnPressed();
    void listLeftViewPage();
    void listRightViewPage();
    void onAutoDivChannelBtnClicked();
    void onAutoDivChannelBtnClickedMultiView_1();   //! 半错位四视，主相机视角为单通道，新增相机视角为半通道
    void onAutoDivChannelBtnClickedMultiView_2();   //! 半错位四视，主相机视角为半通道，新增相机视角为单通道
    void onDivChannelListBackBtnClicked();
    void onDivChannelListPageConvertBtnClicked();
    void onDivChannelListPageConvertBtn2Clicked();
    void onDivChannelListResetBtnClicked(int type = 0);
    void onDivChannelListResetBtnClicked2(int type = 0);

    void onDivChannelBoardNumChangedSlt();
    void onBeginEjectorPressed();
    void onEndEjectorPressed();
    void onPixelBeginBtnPressedSlt();
    void onPixelEndBtnPressedSlt();
    void onChannelRangeBackBtnPressedSlt();

    /* 传感器参数界面的响应函数 */
    void onSensorParametersBtnClicked();
    void onSensorPlusBtnClicked();
    void onSensorMinusBtnClicked();
    void onSensorParametersSureBtnClicked();
    void onSensorParametersBackBtnClicked();
    void onSensorTypeChangedSlt(int index);
    void onSensorModeChangedSlt(int index);

    /* 镜头参数界面的响应函数 */
    void onLensParametersBtnClicked();
    void onLensBtnClicked();
    void onLensSureBtnClicked();
    void onLensBackBtnClicked();

    /* 对位信息获取页面的响应函数*/
    void onContrapositionBtnClicked();
    void onContrapositionBackBtnClicked();
    void onPlusChanNumPressedSlt();
    void onMinusChanNumPressedSlt();
    void onStartBtnPressedSlt();
    void onAcceptbtnPressedSlt();
    void onBiasSendBtnPressedSlt();
    void refreshContrapositionPage();
    void setCorrectBias(int);

private:
    /* 标志彩色相机设置界面不同页面的索引号 */
    enum{
        SET_COLOR_CAMERA_MAIN_PAGE,
        CAL_SIGNAL_PAGE,
        CAL_WAVE_PAGE,
        SET_BIASING_PAGE,
        GAIN_ADJUST_PAGE,
        DIV_CHANNEL_PAGE,
        CHANNEL_LIST_PAGE,
        CHANNEL_RANGE_PAGE,
        SENSOR_PARAMS_PAGE,
        LENS_PARAMS_PAGE,
        CONTRA_POSITION_PAGE
    };

    enum {
        redColor = 0,
        greenColor,
        blueColor,
        RGBNum
    };

    enum {
        ROW_NUM = 13,
        COLUMN_NUM = 11
    };

    QStackedWidget *stackedWidget;
    QWidget *mainWidget;
    g_Config *config;

    /* 定义彩色相机页面的控件 */
    myPushButton *calibrationColorSignalBtn;
    myPushButton *setBiasingBtn;
    myPushButton *gainAdjustBtn;
    myPushButton *divChannelBtn;
    myPushButton *sensorParametersBtn;
    myPushButton *lensParametersBtn;
    myPushButton *contrapositionBtn;
    myPushButton *mainBackBtn;

    /* 定义彩色信号校准页面的控件 */
    QWidget *calibrationColorSignalWiget;
    myPushButton *calibrationGetBtn;
    myPushButton *calibrationWholeBtn;
    myPushButton *calibrationBackBtn;
    myPushButton *calibrationPlusBtn;
    myLCDNumber *calibrationLcdNum;
    QTabBar *calibrationTabBar;
    myPushButton *calibrationMinusBtn;
    QLCDNumber *calibrationBoardNum;
    QTabWidget *calibrationTabWidget;
    QComboBox *comboBox;
    MyCurve *calibrationAxis;
    myPushButton *calibrationWaveBtn;

    /* 定义彩色信号校正波形显示界面控件*/
    QWidget *calibrateWaveWidget;
    MyCurve *calibrateWaveAxis;
    myPushButton *calibrateCopyBtn;
    myValueBar *profileValueBar;
    myPushButton *calibrateWaveBackBtn;

    /* 定义设置偏置页面的控件 */
    QWidget *setBiasingWidget;
    MyCurve *setBiasingAxis;
    myPushButton *setBiasingBackBtn;
    QTabBar *biasingTabBar;
    myValueBar *biasingValueBar;
    QLabel *setBiasingLine[RGBNum];

    /* 定义增益调节界面的控件 */
    QWidget *gainAdjustWidget;
    MyCurve *gainAdjustAxis;
    myPushButton *gainAdjustBackBtn;
    //! 模拟增益相关控件
    QTabBar *GainAdjustTabBar;
    QLCDNumber  *anaGainLcdNum;
    myLCDNumber *anaGainLcdNumD2;
    myPushButton *anaGainPlusBtn;
    myPushButton *anaGainMinusBtn;
    //! 数字增益相关控件 */
    QLCDNumber *digGainUpperLcdNum;
    myValueBar *digGainValueBar;
    QLabel *digGainUpperLabel;
    QLabel *digGainLowLabel;
    myPushButton *digGainUpperPlusBtn;
    myPushButton *digGainUpperMinusBtn;


    /*定义增益调节界面的控件 挪动到相机校正界面 */
    myPushButton *gainAdjustBackBtn2;
    //! 模拟增益相关控件
    QTabBar *GainAdjustTabBar2;
    QLCDNumber  *anaGainLcdNum2;
    myLCDNumber *anaGainLcdNumD22;
    myPushButton *anaGainPlusBtn2;
    myPushButton *anaGainMinusBtn2;
    //! 数字增益相关控件 */
    QLCDNumber *digGainUpperLcdNum2;
    myValueBar *digGainValueBar2;
    QLabel *digGainUpperLabel2;
    QLabel *digGainLowLabel2;
    myPushButton *digGainUpperPlusBtn2;
    myPushButton *digGainUpperMinusBtn2;
    QLabel *analogGainLabel2;
    QLabel *digitalGainLabel2;
    QLabel *targetLabel2;


    /* 定义象元划分界面的控件 */
    QWidget *divChannelWidget;
    QWidget *divChannelListWidget;
    QWidget *divChannelRangeWidget;
    MyCurve *divChannelAxis;

    myPushButton *divChannelListResetBtn;
    myPushButton *divChannelListBtn;
    myPushButton *cameraBodyBtn;
    myPushButton *autoDivChannelBtn;
    myPushButton *divChannelRangeBtn;
    myPushButton *divChannelBackBtn;
    myPushButton *divChannelListBackBtn;
    myPushButton *divChannelListPageConvertBtn;     //! 主从相机页面的切换
    myPushButton *divChannelListPageConvertBtn2;    //! 双视相机页面的切换
    myLCDNumber *beginChannelLcdNum;
    myLCDNumber *endChannelLcdNum;
    myLCDNumber *beginEjectorLcdNum;
    myLCDNumber *endEjectorLcdNum;
    QLabel *listLabel[ROW_NUM][COLUMN_NUM];

    myPushButton *divChannelListResetBtn2;
    QLabel *listLabel2[5][13];

    myLabel *m_pixelBeginLabel;
    myLabel *m_pixelEndLabel;
    myLineEdit *m_pixelBeginLbe;
    myLineEdit *m_pixelEndLbe;
    myPushButton *m_channelRangeBackBtn;
    QTextEdit *m_textEdit;

    int m_ncameraBodyID;    //! 0:左侧,1:右侧
    int m_nChannelPageID;   //! 0:主板页,1:从板页
    int m_nChannelPageID2;  //! 0:双视前页,1:双视后页

    /* 定义传感器参数设置页面的控件 */
    QWidget *sensorParametersWidget;
    myMutex *sensorTypeGroup;
    myMutex *sensorModeGroup;
    QGroupBox *setHorizentalFreGroup;
    QGroupBox *sensorPixelGroup;
    myPushButton *sensorParametersSureBtn;
    myPushButton *sensorParametersBackBtn;
    myPushButton *sensorPlusBtn;
    myPushButton *sensorMinusBtn;
    myLCDNumber *horizontalFreNum;
    myLabel *sensorPixelLabel[MAX_LEVEL];
    MyCheckBox *sensorPixelCbx[MAX_LEVEL];

    int m_nSensorType;
    float m_fRowFre;
    int m_nSensorMode;

    /* 定义镜头参数页面的控件 */
    QWidget *lensParametersWidget;
    myPushButton *lensSelectBtn;
    myPushButton *lensSureBtn;
    myPushButton *lensBackBtn;
    QLabel *lensLabel;
    bool m_bIsSelected;

    /* 定义前后视对位页面的控件*/
    enum {
        ROW_FRONT = 0,      //前视行数
        BIAS_FRONT,     //前视延迟偏置
        CORRECT_FRONT,  //前视延迟修正
        ROW_REAR,       //后视行数
        BIAS_REAR,      //后视延迟偏置
        CORRECT_REAR,   //后视视延迟修正
        LTE_NUM
    };
    QWidget *contrapositionWidget;

    QLCDNumber     *m_chanNum;                      /*!<< LCD: 通道数目 */
    myPushButton   *m_plusChanNumBtn;               /*!<< 按钮：增加通道数目 */
    myPushButton   *m_minusChanNUmBtn;              /*!<< 按钮：减少通道数目 */

    myPushButton    *m_startBtn;                    /*!<< 按钮：开始 */
    myPushButton    *m_acceptBtn;                   /*!<< 按钮：获取 */
    myPushButton    *m_biasSendBtn;                 /*!<< 按钮：偏置下发 */
    myPushButton   *m_contrapositionBackBtn;        /*!<< 按钮：返回上一层 */

    myLabel        *m_lable[5];
    myLineEdit     *m_LineEdit[6];
    QSignalMapper  *m_sigMapper;

    int positionBais[2];
    quint64  rowNum_front;
    quint64  rowNum_rear;
    int    bias_front;
    int    bias_rear;
    int  bias_correct_front;
    int  bias_correct_rear;

};


/*!
 * \brief 整机校正确认控件类
 */
class WholeCalConfirm : public QDialog
{
    Q_OBJECT
public:
    explicit WholeCalConfirm(QString text,QWidget *parent = 0);
    void setchecked(bool ischecked);
    bool isChecked();
signals:

public slots:
    void onOkBtnClicked();
    void onCancelBtnClicked();
    void onAutoCalEnableBtnClicked();
private:
    myPushButton    *m_okBtn;
    myPushButton    *m_checkBtn;
    myPushButton    *m_cancelBtn;
    myLabel         *m_textLabel;
    myLabel         *m_iconLabel;
    bool            m_bIsChecked;

};

#endif // SETCOLORCAMERA_H
