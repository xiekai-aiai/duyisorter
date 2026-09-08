/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        globalflow.h
 * \brief       全局流程头文件
 * \date        2015.01.15
 */
#ifndef GLOBALFLOW_H
#define GLOBALFLOW_H

#include <QThread>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QSettings>
#include <QString>
#include <QObject>
#include <QTime>
#include <QCoreApplication>
#include <QtGlobal>
#include <QTcpServer>
#include <QSqlError>
#include <QSqlQuery>
#include "globalparams.h"
#include "mylogger.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "mylanguage.h"
#include "myicon.h"
#include "common/mono_shared/monoflow.h"
#include "common/mono_shared/camerasignal/ejecttimesthread.h"

#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif

struct MODEL_PARA_INFO {
    QString id;        // id
    int levelTotal;
    int unit;
    QString name;      // name
    QString threshold; // threshold
    QString isApply;   // isApply
};

/*!
 * \brief 全局流程类
 */
class GlobalFlow : public QObject           // 必须从QObject继承，否则不能用tr("")方式显示文字
{
    Q_OBJECT

public:
    GlobalFlow();
    ~GlobalFlow();

    /** 全局流程 */
    void sleep(int secs);                           //! 设置秒级的延迟
    void msleep(int mSecs);                         //! 设置毫秒级的延迟
    qint64 getSystemTurnOnTime();                   //! 获取系统应用启动时间
    int getProductLineNo();                         //! 获取当前运行程序对应的产品线类别，0：杂粮，1：大米，2：茶叶，-1：未知
    bool mountUdisk();                              //! 重新挂载U盘，失败返回false,成功返回true
    long getFileSize(char *sFileName);              //! 获取参数文件大小

    void onOff();                                   //! 开关下料
    void onOffMono(void);                           //! 黑白机型的开关下料
    void printArgsForTest(int indexOfTrain = 0);    //! 打印一些智能相关调试参数
    void ejectorOn();                               //! 气压报警正常后开喷阀
    void ejectOff();                                //! 气压不足时关喷阀
    void updateOnoff(int onOff);                    //! 开启/关闭校正，进入相机设置界面关闭校正，退出时打开校正
    void startWipe(void);                           //! 清灰操作
    void startStatistic(int curLevel, int curUnit); //! 开始信息统计
    void initChannelPixelCompensate();              //! 镜头畸变的系数
    void initChannelPixelCompensateDseries();       //! D系列相机镜头畸变系数
    void resetFPGAMode();                           //! FPGA启动模式重置
    void checkAndResetFPGAMode(int nIntMode, int nColorMode);     //! 检查并重置启动模式
    void setLang();                                 //! 设置界面语言
    int  initCommunication();                       //! 开机通讯自检
    void initAll();                                 //! 整机开机初始化工作
    void initMachineType();							//! 初始化机器类型，为LD喷阀拼接所用
    void initSendAllParams();                       //! 整机开机发送所有参数
    void initGsh();                                 //! 初始化全局共享参数
    void resetGroupParams(void);                    //! 重置与组相关的参数
    void initLan();                                 //! 初始化词根
    void initSerial(void);                          //! 初始化串口设备
    void setCom1Baud(void);                         //! 设置波特率1
    bool checkIntBoardCom(int nInt);                //! 查询接口板
    void detectUsbFirmware(void);                   //! 检测USB固件
    void detectNewUsbFirmware();                    //! 通过读取/proc/bus/usb/devices信息的方式查询USB固件版本
    bool checkIntBaud(void);                        //! 检查接口板波特率一致性
    void setCom2Baud(void);                         //! 设置波特率2
    bool checkCtrlBoardCom(int nCtrl);              //! 查询控制板
    bool checkCtrlBaud(void);                       //! 检查控制板波特率一致性
    bool peaMachineJudge();                         //! 是否红小豆机型判断，是红小豆机型时，若物料不是红小豆，则停止下料
    void resetTickAdvParams(void);                  //! 重置剔除高级参数（最大行、行列关系及行延迟）
    void resetLedModeForLD4(void);                  //! 重置LED指示灯（LD4机型）

    void initUdpImagPara();                             //初始化udp img协议
    void initPixelImagPara();                           //初始化udp pixel协议
    void initEjectorDelayPara();                        //初始化udp ejector delay协议
    void initEjectorModePara();                         //初始化udp ejector mode协议

    void initModelType();                               //初始化udp ejector mode协议
    void initModelPara();                               //初始化udp ejector para协议

    void imageInferOnOff(bool onOff);
    void imageSendOnOff(bool onOff);
    int  initAiCommunication();


    void getDefaultPara();
    QString incrementLastOctet(const QString & ip);

    /** 参数保存/获取 */
    void getSetting();
    int  getParamsFileStaus();
    int  saveParamsFileStaus(int value);

    void saveSetting();
    bool saveEngineer();                //! 保存工程模式参数
    void getEngineer();                 //! 获取工程模式参数
    void getEngineerDefault();          //! 获取工程模式中不随工程文件保存的参数
    void getGlobal();                   //! 获取全局参数
    bool saveGlobal();                  //! 保存全局参数
    void getCamera();                   //! 获取相机参数
    bool saveCamera();                  //! 保存相机参数
    void getProfile();                  //! 获取方案参数
    bool saveProfile();                 //! 保存方案参数

    void getServerSetting();
    void saveServerSetting();

    bool getEngineerSetting(const QString & fileName);
    bool saveEngineerSetting(const QString & fileName);
    bool getGlobalSetting(const QString & fileName);
    bool saveGlobalSetting(const QString & fileName);
    bool getCameraSetting(const QString & fileName);
    bool saveCameraSetting(const QString & fileName);
    bool getProfileSetting(const QString & fileName);
    bool saveProfileSetting(const QString & fileName);

    void getGroupIdentify();            //! 初始化识别组模块参数
    void getGroupTick();                //! 初始化剔除组模块参数
    void getLevelInfo_CF();             //! 获取每层内组、通道信息  CF
    void getLevelInfo_RSC();            //! 获取每层内组、通道信息  RSC
    void getLevelInfo_MCC();            //! 获取每层内组、通道信息  RSC_MC
    void getLevelInfo_AFLATOXIN();      //! 获取每层内组、通道信息  AFLATOXIN
    void getLevelInfo_LD2();            //! 获取每层内组、通道信息  LD2
    void getLevelInfo_LD4();            //! 获取每层内组、通道信息  LD4
    void getLevelInfo_LDS();            //! 获取每层内组、通道信息  LD2
    void getLevelInfo_LDS2();           //! 获取每层内组、通道信息  LD2
    void getLevelInfo_TD();             //! 获取每层内组、通道信息  TD
    void getLevelInfo_T1();             //! 获取每层内组、通道信息  T1
    void getLevelInfo_T2();             //! 获取每层内组、通道信息  T2
    void getLevelInfo_RS();             //! 获取每层内组、通道信息  RS
    void getLevelInfo_MC();             //! 获取每层内组、通道信息  MC

    void getArithmeticMachineEnable();  //! 获取当前机型的各个算法使能状态

    /** 发送参数流程 */
    void sendAllParamsRgb(void);        //! 彩色机型开机发送所有参数
    void initInterfaceTransMode(int mode);  //! 刚开机时，将接口板上传数据模式切换成串口模式，防止在USB模式下异常断电
    void initCamera();                  //! 重置相机参数
    void resetSortParams();             //! 重置色选参数
    void resetBackgroundColor();        //! 重置背景颜色设置
    void resetAutoBackgroundColor();    //! 重置自动背景颜色设置
    void resetMaterial(int nLevelId, int nGroupId, int bIsAllSens, int nArithSeq, int bIsReserved);   // 重置物料参数

    void calDivChannelParams(int beginCh, int endCh, int nLevel, int nUnit);

    /** 系统名称相关函数 */
    QString getMachineName(int id);
    QString getProductName(int id);
    void getIdentifyGroupName();                            // 获取各个识别组模块名称
    void getArithmeticName();                               // 获取算法名称
    void getPrefixedName();                                 // 获取预制方案名称
    void getLampName();                                     // 获取不同机型的灯控名称
    void getColorSelectGroupName();                         // 获取色选模式页面组名
    void getColorSelectModeName();                          // 获取色选模式名称

    /** 识别相关函数 */
    void resetArithmeticEnable();                           // 重置算法使能
    void resetFrontRearRelation();                          // 重置前后视算法关系
    void resetReverseSort();                                // 重置正选/反选功能
    int  getIdentifyGroupID(int nUnit, int spliceType);     // 根据相机地址及分割类型获取识别组ID
    void getUnitsForBkg(QList<int> &qlUnits, int nUnit);    // 根据相机地址获取该相机所在背景组的所有相机地址
    int  getIdentifyGroupAddr(int nLevelId, int nGroupId, int nUnitIndex);   // 获取识别组发送模块地址
    int  getBackgroundGroupID(int nUnit);                   // 根据相机地址获取所在背景组号
    int  getIntelSeq(int mod);                              // 获取智能组合物料序号 
    void resetIntelSeq();                                   // 重置智能组合物料序号

	int getUniformRandom(double min, double max);
	int getRandomLevel(int curLevel, int curUnit);
	int getRandomUnit(int curLevel, int curUnit);

    /** 剔除相关函数 */
    bool checkIntelArithParams(int nLevel, int nIdeGroup, int nArith);      // 检查智能算法参数
    void getTickGroupName();                            // 获取各个剔除组模块名称
    void resetTick();                                   // 重置剔除参数
    void getDelayImpure();                              // 获取传染算法延迟损耗
    void getDelayEdge();                                // 获取边缘切除延迟损耗
    void getDelayMaterialSize();                        // 获取物料算法尺寸延迟损耗
    void getArithDelayEdge();
    int  resetArithEdgeCut();
    void resetAccurateTickParams();                     // 精准剔除模式使能发送及参数发送
    int  resetEjectTime();                              // 重置喷阀时间
    int  getTickGroupAddr(int nLevelId, int nGroupId, int nUnitIndex);   // 获取剔除组发送模块地址
    int  getTickGroupId(int nUnit);

    /** 物料算法相关函数 */
    void materialParamsSet(int nCmd, int nLevelId, int nGroupId, int nIndex);
    void materialParamsPacketOrganize(int nCmd, int nLevelId, int nGroupId, int nIndex);
    void materialParamsNewPacketOrganize(int nCmd, int nLevelId, int nGroupId, int nIndex);                         // 物料参数发送组包
    void materialResetGroupAssemble(int nLevelId, int nGroupId, int bIsAllSens, int nArithSeq, int bIsReserved);    // 物料参数发送组地址确定
    void materialParamsCopy(int nLevelIdSrc, int nGroupIdSrc, int nLevelIdDst, int nGroupDst, int bIsAllSens, int nArithSeq, int bIsReserved);// 物料参数复制
    void materialCopyAssemble(int nLevelId, int nGroupId, int bIsAllSens, int nArithSeq, int bIsReserved);          // 物料参数组地址确定
    void materialResetGreyRsvPer(int nLevelId, int nGroupId, int nUnit, int nArithSeq); //! 重置灰度及色差算法的保留比例参数
    void materialResetIntelRsvPer(int nLevelId, int nGroupId, int nUnit);   //! 重置多分类智能的保留比例参数

    void materialAiModelParaCopy(int nLevelId, int nGroupId,int nOldLevelId, int nOldGroupId);
    void materialAiModelParaSet(int nLevelId, int nGroupId);

    /** 控制板相关函数 */
    void getControlGroupName();         // 获取各个控制板组模块名称
    void resetControl();                // 重置控制板所有参数
    void resetFeeder(int flag);         // 重置供料量
    void setTsBackLight(int flag);      // 设置触摸屏背光
    void sendTsBackLightOn();           // 背光控制
    void resetLamp();                   // 重置灯光设置
    void updateLampTotal(void);         //! 更新灯控总数
    void updateLightSrcLampNum(void);   //! 更新每隔恒流源板灯控数量

    /** RSC机型专用函数 */
    QString getNameByModeRSC(int nMode);// 根据当前模式获取名称
    void initLampParamsRSC(void);       // 初始化RSC机型灯光配置
    void initRowAndColRSC(void);        // 初始化RSC机型行列尺寸
    void initSortParamsRSC(void);       // 初始化RSC机型色选参数
    void initDiffPosSortRSC(void);      // 初始化RSC机型异色粒正选参数
    void initDiffNegSortRSC(void);      // 初始化RSC机型异色粒反选参数
    void initWhitePosSortRSC(void);     // 初始化RSC机型乳白粒正选参数
    void initWhiteNegSortRSC(void);     // 初始化RSC机型乳白粒反选参数
    void initYellowWhiteSortRSC(void);  // 初始化RSC机型黄白同选参数
    void initYellowTransSortRSC(void);  // 初始化RSC机型黄透同选参数
    void initAIParamsRSC(int nGroup, int *pVal);        // 初始化RSC机型智能参数

    /** 其他通用 */
    QList<int> getLanguageList();
    QString getLanguageListName(int index, bool local = true);
    QIcon   getLanguageListIcon(int index);
    QList<int> getRunModeList();
    QString getRunModeName(int mode);

    /** 报警相关 */
    void checkSlaveCommStat(void);      //! 检查网络通信状态
    void getSlaveList(void);            //! 获取从设备列表

signals:
   void backlightOnSig();   // 关背光后，点亮背光需要输入密码

private:
    qint64 turnOnTime;      // 开机时刻
    MODEL_PARA_INFO   modeParaStr[10];
};

extern GlobalFlow myFlow;

#endif // GLOBALFLOW_H
