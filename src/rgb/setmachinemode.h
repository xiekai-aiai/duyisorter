/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setmachinemode.h
 * \brief       彩色机型设置机型界面头文件
 * \date        2015.01.14
 */
#ifndef SETMACHINEMODE_H
#define SETMACHINEMODE_H

#include <QWidget>
#include <QtCore>
#include <QListWidget>
#include <QPushButton>
#include <QLCDNumber>
#include <QLayout>
#include <QGroupBox>
#include <QMap>

#include "globalparams.h"
#include "globalflow.h"
#include "common/mymutex.h"
#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mywid.h"

/* 定义设置机型中的最大识别组数 */
const int TYPE_MAX_IDETIFY_GROUP = 8;

typedef struct MachineModeConfig {
    QList<int> qlTypeList;  // 允许配置机型列表
    QStringList qlModeList; // 机型对应规格列表
    int nTypeIndex;	        // 当前机型对应序号
    int nModeIndex;	        // 当前规格对应序号
    int	nTickPerLevel;      // 通道剔除组数(每层）
    float nChuteRatio;	    // 通道配置比例
    int nEjectStep;	        // 喷嘴调节步长
    bool bEnableEject;	    // 是否使能喷阀设置
    bool bEnableView;	    // 是否使能视角设置
    bool bEnableLED;	    // 是否使能LED切换
    bool bEnableRunMode;    // 是否使能屏模式设定
} glMachineModeConfig;

/*!
 * 允许配置机型列表
 */
enum {
    // 杂粮
    MAC_TYPE_CF,
    MAC_TYPE_CF_PLUS,
    MAC_TYPE_CF_W, // CF OEM
    MAC_TYPE_LD2,
    MAC_TYPE_LD2_W, // LD2 OEM
    MAC_TYPE_LD4,
    MAC_TYPE_LDS,
    MAC_TYPE_LDS2,
    MAC_TYPE_CG,
    // 米机
    MAC_TYPE_RC,
    MAC_TYPE_RDC,
    MAC_TYPE_RSC,
    MAC_TYPE_MC,
    // 茶叶
    MAC_TYPE_TD,
    MAC_TYPE_SD,
    MAC_TYPE_T1,
    MAC_TYPE_T2
};

/*!
 * \brief 彩色机型设置机型界面类
 */
class setMachineMode : public QWidget
{
    Q_OBJECT
public:
    explicit setMachineMode(QWidget *parent = 0);
    ~setMachineMode();
    void setMachineModeParams(void);    // 设置机型设置界面相关参数
    void createMachineTypeList();	    // 创建机型选择列表
    void createMachineModeList();	    // 创建机型规格列表
    void createChuteEjectList();	    // 创建通道配置列表

    void refreshMachineTypeList(const glMachineModeConfig &config);  // 刷新机型选择列表
    void refreshMachineModeList(const glMachineModeConfig &config);  // 刷新机型规格列表
    void refreshChuteEjectNum(const glMachineModeConfig &config);    // 刷新通道的喷阀数
    void refreshViewBoxGroup(const glMachineModeConfig &config);     // 刷新视角设置页面
    void refreshDisplay(const glMachineModeConfig &config);          // 根据机型刷新显示界面
    void refreshLedModeDisplay(const glMachineModeConfig &config);   // 刷新LED指示灯模式的显示
    void refreshRunModeDisplay(const glMachineModeConfig &config);
    bool isViewStateChanged();

    void hideChuteRow(int index);           // 根据索引号隐藏通道信息中的某一行
    void showChuteRow(int index);           // 根据索引号显示通道信息中的某一行
    void enableChuteRow(int index);
    void disableChuteRow(int index);

    void setLevelParams(void);              // 设置层级的相关参数
    void setGroupParams(void);              // 设置全局参数中的组参数
    void setCFGroupParams(void);            // 设置CF机型的组参数
    void setLD2GroupParams(void);           // 设置LD2机型的组参数
    void setLD4GroupParams(void);           // 设置LD2机型的组参数
    void setLDSGroupParams(void);           // 设置LDS机型的组参数
    void setLDS2GroupParams(void);          // 设置LDS2机型的组参数
    void setTDGroupParams(void);            // 设置TD机型的组参数
    void setT2GroupParams(void);            // 设置TD机型的组参数
    void setT1GroupParams(void);            // 设置T1机型的组参数

    void printMachineSet(void);             // 打印当前机型设置参数，调试函数
    void resetLampParams(int nTotal);       // RSC机型下重置灯控参数
    void resetLedMode(void);                // 重置LED指示灯模式
    void resetRunMode(void);                // 重置屏运行模式设定

    glMachineModeConfig getMachineModeConfig();
    void createMachineTypeMap();
    QList<int> getMachineTypeList();
    QStringList getMachineModeList(int nTypeIndex);
    QString getMachineTypeName(const QMap<int, QString>&map, int index);


signals:
    void backToFactory();
    void resetCamera(int type = 1);         //重置象元, 不下发
    
public slots:
    void onViewBoxClicked(int index);
    void onTDViewBoxClicked(int index);
    void onSureBtnClicked();
    void onBackBtnClicked();
    void refreshMachineModeDisplaySlt();
    void onMachineModeChangedSlt();
    void onMachineTypeChangedSlt();

    void onChuteMinusBtnClicked(int index);
    void onChutePlusBtnClicked(int index);

    void onEjectMinusBtnClicked();
    void onEjectPlusBtnClicked();
    void onEjectNumBtnClicked();

private:
    enum {
        machineModeNum = 12,                // 机型数目
        tickGroupPerLevel = 4               // 每层剔除组数目
    };
    g_Config *config;

    QListWidget *machineModeList;
    QListWidget *machineTypeList;
    QListWidgetItem *typeItem[MAX_GENERAL_MACHINE_NUM]; 
    QListWidgetItem *modeItem[machineModeNum];
    QLabel *chuteLabel[tickGroupPerLevel], *ejectNumLabel;
    myPushButton *chutePlusBtns[tickGroupPerLevel], *chuteMinusBtns[tickGroupPerLevel];
    myPushButton *ejectNumPlusBtn, *ejectNumMinusBtn;
    QFrame *LineStatus;
    myGroupBox *machineChuteGroup;
    myGroupBox *machineViewBoxGroup;
    myPushButton *sureBtn, *backBtn;
    myLCDNumber *chuteEjectNum[tickGroupPerLevel];
    myLCDNumber *ejectNum;
    MyCheckBox *ledModeCb;
    QVector<MyCheckBox *> viewBox;
    QLabel *runModeLabel;
    MyComboBox *runModeBox;

    QSignalMapper *chuteMinusBtnMapper;
    QSignalMapper *chutePlusBtnMapper;
    QSignalMapper *FrontRearviewBoxMapper;

    QVector<MyCheckBox*> FrontRearviewBox;

    quint8 m_nListIndex[machineModeNum];    // 存储列表中的机型对应的板子数目
    quint8 m_nEjectNumPerBoard;             // 每个通道对应的喷嘴数量
    double m_nChutePerTickGroup[MAX_LEVEL][MAX_GROUP_TICK];    // 每个剔除组中对应通道的数量
    double m_nChutePerLevel[MAX_LEVEL];     // 每层中对应的通道数量
    int    m_nMachineMode;                  // 当前机型
    int    m_nDeriveMachineMode;            // 当前衍生机型，如I机型的配置机型
    int    m_nCGMachineMode;                // 杂粮CS机型
    quint8 m_nLevelTotal;                   // 机型层数
    quint8 m_nTickGroupTotal[MAX_LEVEL];    // 层内剔除组数量
    quint8 m_nIdentifyGroupTotal[MAX_LEVEL];// 层内识别组数量
    int    m_nOrignMachineSeq;              // 记录当前实际机型对应的机型列表索引号
    int    m_nViewState[MAX_LEVEL];	        // 记录TD系机型前后视状态切换次数
    int    m_nViewTotal[MAX_LEVEL];         // 记录当前层前后视选择信息

    QMap<int, QString>qMachineTypeMap;
    glMachineModeConfig qMachineModeConfig;
};

#endif // SETMACHINEMODE_H
