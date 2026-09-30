/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        statusinfopage.h
 * \brief       系统状态信息界面头文件
 *              子页面包括系统信息、报警信息、版本信息、远程控制、设置时间及版权信息。
 * \date        2015.01.14
 */
#ifndef STATUSINFOPAGE_H
#define STATUSINFOPAGE_H

#include <QMap>
#include <QVector>
#include <QWidget>
#include <QStackedWidget>
 //#include <QWebView>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputpanel.h"
#include "common/myinputmethod.h"
#include "common/mywid.h"
#include "myvaluebar.h"

#include "globalflow.h"
#include "globalparams.h"
#include "mydelaycode.h"
#include "remotecontrol.h"
#include "setsms.h"
#include "stdint.h"
#include "sortertypes.h"

using QJson::JsonObject;
using QJson::JsonArray;

enum
{
    STATUS_INDEX_INFO,
    STATUS_INDEX_STATISTIC,
    STATUS_INDEX_ALARM,
    STATUS_INDEX_VERSION,
    STATUS_INDEX_NETWORK,
    STATUS_INDEX_TIME,
    STATUS_INDEX_HELP,
    STATUS_INDEX_COPYRIGHT,
    STATUS_INDEX_AUTO,
    STATUS_INDEX_CONNECT
};

enum
{
    STATISTIC_INDEX_STATUS,
    STATISTIC_INDEX_LOG,
    STATISTIC_INDEX_BACKGROUND,
    STATISTIC_INDEX_HISTOGRAM,
    STATISTIC_INDEX_IDENTIFY,
    STATISTIC_INDEX_EJECT_TIMES,
    STATISTIC_INDEX_CONFIG
};

/*!
 * \brief 系统信息界面类
 */
class StatusInfoPage : public QWidget
{
    Q_OBJECT
public:
    explicit StatusInfoPage(QWidget* parent = 0);

    void updateAll();                   // 更新全部内容
    void updateListWidget();            // 更新页面列表
    void CreateGeneralPage();           // 一般状态页面控件
    void updateGeneralPage();           // 更新一般状态页面
    void updateStatisticPage(int index);// 更新大数据信息页面
    void CreateSetAlarmPage();          // 设置报警页面控件
    void CreateSystemTimePage();        // 系统时间页面控件
    void setDateTime(int y, int m, int d, int h, int min, int sec);     // 设置系统时间
    void updateDateTime();              // 更新系统时间
    void CreateSoftwareVersionPage();   // 软件版本页面控件
    void CreateSetNetworkPage();     // 远程控制页面控件
    void CreateInfomationPage();        // 系统信息页面控件
    void CreateAutoCalibratePage();     // 一键校正页面控件（仅适用于T1机型）
    void CreateGetHowToWidget(void);    // 系统帮助页面控件
    void CreateConnectUsPage();         // 联系我们页面控件

    void getVersionMaterialType(int nVer, int nVal, char* materialType);
    void reloadHowTo(void);             // 重新加载帮助页面
    void sendAlarmSms(void);            // 有报警时发送提示短信
    void updateAlarmStatus(void);       // 更新报警状态
    void setLeftBtnStatus();            // 更新左边重新检测按钮显示状态
    void updateAiRunStatus();

    void hideColumn(int index);
    void showColumn(int index);

    qint64 getQByteArrayMax(QByteArray a, int byteNum);//获取a中byteNum(最大值8)字节数组合成的数据类型最大值

    void createPageWifiWidget();
    QString hexStringToChinese(const QString& hexString);
    char hexToByte(const QString& hexString);
    QString convertHexInString(const QString& hexString);
    QString getVpnIpAddress();

    QVector<AiStatusInfo> GetAiStatus();
    QStringList GetAiStatusColInfo(const QVector<AiStatusInfo>& status_vec, int col_no);

signals:
    void updateStatusPageSig();         // 更新系统状态页面
    void updateSetAlarmPageSig();       // 更新报警状态页面
    void updateAlarmPageSig();          // 更新报警状态页面
    void updateSystimePageSig();        // 更新系统时间页面
    void updateVersionSig();            // 更新版本信息
    void updateRemoteControlPageSig();  // 更新网络配置页面
    void updateInfoPageSig();           // 更新系统信息页面
    void updateAutoCalibrateSig();      // 更新一键自动校正页面
    void getVersionSig();
    void goToHomePageSig();

public slots:
    void TimeOutSlt();                  // 定时器
    void gotoFirstPageSlt();            // 页面切至第一页

    void updateSetNetworkPageSlt();     // 更新网络配置页面
    void updateSetAlarmPageSlt();       // 更新报警设置页面
    void updateVersionResultSlt();      // 更新版本
    void onLeftBtnClickedSlt();         // 响应左边按钮
    void onCancelBtnClickedSlt();       // 返回主界面
    void onListWidgetRowChangedSlt(int nIndex); // 响应列表行变化
    void onStatusTabChanged(int index);         // 响应系统状态页面的tab切换
    void onCameraHRefChangedSlt(int num);       // 响应参考相机设置
    void onCameraVRefChangedSlt(int num);
    void onTimeHistogramStaisticLineEditPressed();
    void ontimeMaterialStaisticLineEditPressed();
    void onServerLineEditPressed();

    // PageAlarm
    void getAlarmIndex(int index);      // 报警状态修改
    QString getAlarmName(int type);	// 报警使能名称
    QList<int> getAlarmList();		// 报警使能列表
    void setTemperatureThreshold();	// 设置温度报警阈值

    // pageSystime
    void getTimeIndex(int index);       // 系统时间修改

    // pageVersion
    void versionUpdateInterface();      // 接口板版本信息
    void versionUpdateUSBFirmware();    // USB固件版本信息
    void versionUpdateCtrl();           // 控制板版本信息
    void versionUpdateCamera();         // 相机板版本信息
    void versionUpdateLightSrc(void);   // 恒流源板版本信息
    void versionUpdateCameraLD4();      // LD4相机板版本信息
    void versionUpdateCameraRSC();    	// RSC相机板版本信息
    void versionUpdateCameraTD();       // TD版本信息
    void versionUpdateCameraLD();       // LD版本信息
    void versionUpdateCameraLDS();      // LDS版本信息
    void versionUpdateCameraLDS2();     // LDS2版本信息
    void versionUpdateCameraCF();       // CF版本信息
    void versionUpdateCameraLD2();      // LD2版本信息

    // pageAutoCalibrate
    void autoCalibrateSignal();         // 一键自动校正

    // pageInformation
    void getHowToHelp();

    // pageRemote
    void onStackedIndexChangedSlt(int index);       // 切换页索引变化

    // pageContactUs
    void setinfo();
    void setMachineInfo();
    void setEngineerInfo();
    void setMachineIDInfo();
    void onStatisticEnablePressedSlt();

    void setExpireTime();

    void onWifiScanBtnPressed();
    void onWifiConnectBtnPressed();
    void onWifiDisonnectBtnPressed();
    void result();
    void onPassWordLineEditPressed();

    // wifi.sh 异步执行完成（成功/失败都进这里，用来启动 DHCP 轮询）
    void onWifiProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    // wifi IP 轮询定时器：周期检查 wlan0 IP 是否拿到/变化
    void onWifiIpPollTimeout();
    // iwlist 扫描异步完成：解析结果填充 SSID 下拉框（不阻塞 UI）
    void onWifiScanFinished(int exitCode, QProcess::ExitStatus exitStatus);
    // 启动自动连接（QTimer::singleShot 通过 SLOT() 调用，必须声明为槽）
    void autoConnectWifi();
    // ssidComboBox 切换时自动填密码（存过的就自动填；connect 用 SLOT() 也必须为槽）
    void onSsidComboTextChanged(const QString& ssid);



private:
    g_Config* config;
    QTimer* timer;
    myListWidget* listWidget;
    myListWidgetItem* statusListItem[10];
    QVector<int>      listIndex;

    QStackedWidget* stackedWidget;
    QWidget* pageGeneral;
    QWidget* pageStatistic;
    QWidget* pageAlarm;
    QWidget* pageSetNetwork;
    QWidget* pageSystemTime;
    QWidget* pageInformation;
    QWidget* pageSoftwareVersion;
    QWidget* pageAutoCalibrate;
    QWidget* pageContactUs;

    QGridLayout* statusInfoPageGridLayout;
    QHBoxLayout* statusInfoPageHBLayout;
    QSpacerItem* horizontalSpacer_1;

    // pageGeneral
    myLabel* thisRunLabel;
    myLabel* thisFeedRunLabel;
    myLabel* totalRunLabel;
    myLabel* totalFeedRunLabel;
    myLabel* m_thisPressureAlarmLabel;
    myLabel* m_totalPressAlarmLabel;
    myLabel* feedLabel;
    myLabel* ejectLabel;
    myLabel* blowerLabel;
    myLabel* lampLabel;
    myLabel* pressLabel;
    myLabel* beltStatus[MAX_LEVEL];
    myLabel* m_feedAlarmLabel;
    myLabel* m_commAlarmLabel;
    myLabel* m_temperatureLabel;
    myLabel* m_slaveCommAlarmLabel;
    QVBoxLayout* generalVBLayout;
    QSpacerItem* verticalSpacer_1;
    MyTabBar* statusTabBar;
    myGroupBox* generalInfoGroup;

    // AI状态信息
    myGroupBox* aiInfoGroup;
    // AI状态布局
    QGridLayout* aiGridLayout;
    // AI状态显示信息
    QVector<QVector<myLabel*>> aiStatusLabels;

    // pageAlarm
    myCustomCheckBox* alarmEnableBtn[MAX_ALARM];
    QSignalMapper* alarmSignalMapper;
    myLineEdit* temperatureThreshold;

    // pageSysTime
    QSignalMapper* timeSignalMapper;
    myLabel* dateLabel[6];
    myLineEdit* dateLbe[6];
    QDateTimeEdit* dateTimeEdit;
    QSpacerItem* horizontalSpacer;
    myPushButton* setDateBtn;
    QGridLayout* gridLayout;
    QVBoxLayout* dateVBLayout;
    QSpacerItem* horizontalSpacer_2;
    int            data[6];

    // pageInformation
    myLabel* infoLabel;

    // pageVersion
    myGroupBox* screenListGroup;
    QGridLayout* screenListLayout;
    QLabel* screenListLabel;

    myGroupBox* interfaceBoardListGroup;
    QGridLayout* interfaceListLayout;

    /** 接口板版本查询增加USB固件版本 */
    QLabel* interfaceListLabel[MAX_INT + 1][2];

    myGroupBox* ctrlBoardListGroup;
    QGridLayout* controlListLayout;
    QLabel* controlListLabel[MAX_CTRL][2];

    QGroupBox* cameraBoardListGroup;
    QGridLayout* cameraListLayout;
    /*
    * 用于相机版本和AI板卡信息
    * i: 1行显示前后视，所以这里MAX_UNIT/2, 第一行用于显示标题所以总行数为MAX_UNIT / 2 + 1
    * j: 4: 前后视相机+前后视AI板卡
    */
    QLabel* cameraListLabel[SUM_SHOW_ROW_NUM][SUM_SHOW_COLUMN_NUM];

    /* 恒流源板 */
    QGroupBox* m_lightSrcGroup;
    QLabel* m_lightSrcLabel[MAX_LIGHT_SOURCE][2];
    QGridLayout* m_lightSrcLay;

    QHBoxLayout* backHBLayout;
    QVBoxLayout* versionLayout;

    QGridLayout* versionListGridLayout;
    QHBoxLayout* versionListHBLayout;
    myPushButton* reCheckBtn;

    // pageSetNetwork
    QTabBar* remoteTabBar;
    QStackedWidget* remoteStackedWidget;
    MyNetConfig* pageNetConfig;
    MyRemoteControl* pageRemoteControl;
    QWidget* pageWifiSet;

    MyBigData* pageBigData;
    //     MyLoggerViewer     *pageLogger;

         // pageAutoCalibrate
    myLabel* autoLabel;

    // page contact us
    myLabel* qrCodeLabel;
    myLineEdit* machineLbe;
    myLabel* machineLabel;
    myLineEdit* engineerLbe;
    myLabel* engineerLabel;
    myLineEdit* machineIDLbe;
    myLabel* machineIDLabel;
    myLineEdit* expireTimeLbe;
    myLabel* expireTimeLabel;

    myPushButton* setinfoBtn;

    QGridLayout* machineInfoLayout;

    // common controls
    QPushButton* leftBtn;
    QPushButton* cancelBtn;

    /* 系统帮助页面 */
    QWidget* pageHowTo;
    //     QWebView *helpview;

         /* 设置短信页面 */
    SetSms* m_pageSetSms;
    MySlaveList* m_pageSlaveList;

    QTextEdit* m_textEdit;

    /* 大数据页面 */
//     QWebView	   *webview;
    myLabel* materialNumLabel;
    myLabel* materialBadNumLabel;
    MyCheckBox* statisticEnable;
    myValueBar* cameraHRef;
    myValueBar* cameraVRef;
    myGroupBox* bgStatisticGroup;
    myGroupBox* materialStatisticGroup;
    myGroupBox* histogramGroup;
    myGroupBox* ejectStatisticGroup;
    myGroupBox* cameraRefBox;
    myGroupBox* timeStatisticGroup;
    myGroupBox* serverGroup;
    myLineEdit* timeMaterialStaisticLineEdit;
    myLineEdit* timeHistogramStaisticLineEdit;
    myLineEdit* serverLineEdit;

    myLabel* ssidLabel;
    MyComboBox* ssidComboBox;
    myLabel* passWordLabel;
    myLineEdit* passWordLineEdit;
    QTextEdit* connectResultEdit;
    myPushButton* wifiScanBtn;
    myPushButton* wifiConnectBtn;
    myPushButton* wifiDisconnectBtn;
    QProcess* ycprocess;
    QProcess* m_wifiScanProcess;    // 扫描专用进程，和 ycprocess（连接流程）分开，避免互相打断
    myLabel* wifiAddrLabel;
    myLineEdit* wifiAddrLineEdit;

    // ═══════════════ wifi.cnf 持久化 + 启动自动连接 + IP 实时刷新 ═══════════════
    // wifi.cnf 路径（运行时在 app/cnf/ 下，和 cnf.global 同级）
    QString m_wifiCnfPath;
    void buildWifiCnfPath();
    // 用 wifi.cnf [known] 的历史 SSID 补齐下拉框，并恢复选中 [last]（只追加不清空）
    void reloadSsidCombo();
    // ── 多 SSID 管理（已知网络库） ──
    // 格式: INI, QSettings 读写
    //   [known]    ssid=password   （所有成功连过的）
    //   [last]     ssid=xxx        （最近一次成功连接的 SSID）
    void saveKnownWifi(const QString& ssid, const QString& password);
    QString lookupKnownPassword(const QString& ssid);   // 没存过返回 ""
    bool    readLastSuccess(QString& outSsid, QString& outPassword);
    void    saveLastSuccess(const QString& ssid);

    // 自动连接是否已连上（有 IP 即视为已连接）
    bool isWifiAlreadyConnected();

    // wifi IP 轮询定时器（连接上后每隔 3s 拉一次 IP，直到拿到且稳定）
    QTimer* m_wifiIpPollTimer;
    QString m_lastWifiIp;
    int     m_wifiPollCountdown;
    void    startWifiIpPoll();
    void    stopWifiIpPoll();

    // 连接过程状态：成功拿到 IP 才写 wifi.cnf，避免把错误密码也存进去
    bool    m_wifiConnecting;
    QString m_pendingSsid;
    QString m_pendingPassword;
    void    markWifiConnectedSuccess();   // 拿到 IP 后调用 → 写 known + last + 清 flag

    // 自动连接重试计数器（最多 3 次，每次间隔 1 秒）
    int     m_wifiAutoRetryCount;

};
#endif // STATUSINFOPAGE_H
