/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        monoflow.h
 * \brief       黑白机型全局流程头文件。
 * \date        2015.01.14
 */
#ifndef MONOFLOW_H
#define MONOFLOW_H

#include <QObject>
#include "globalparams.h"
#include "globalflow.h"

/*!
 * \brief 黑白机型全局流程类
 */
class MonoFlow : public QObject
{
    Q_OBJECT
public:
    explicit MonoFlow(QObject *parent = 0);

    /** 黑白机型下发色选参数函数 */
    int initCommunicationMono(void);            //! 通信自检
    int checkMasterCameraBoard(void);           //! 主前置板自检
    int checkInterfaceBoard(void);              //! 接口板自检
    int checkCtrlBoard(void);                   //! 控制板自检
    int checkLightSourceBoard(void);            //! 恒流源板自检
    void sendDiscolorAndAreaParams(void);       //! 发送病点色差与坏料面积参数

    void sendAllParamsMono(void);               //! 开机及开关下料后发送所有参数
    void sendOnOffParams(int nFlag);            //! 整机发送启动/停止命令

    /** 接口板相关参数 */
    void sendInterfaceParams(void);             //! 发送接口板参数
    void resetEjectTimeMono(void);              //! 重置喷阀时间

    /** 相机板相关参数 */
    void sendCameraParams(void);                //! 发送前置板参数
    void sendMasterCameraParams(void);          //! 发送主配前置板参数
    void sendChannelParams(int nUnit);          //! 发送相机象元参数
    void sendAnaGainParams(int nUnit);          //! 发送相机模拟增益参数
    void sendDigGainParams(int nUnit);          //! 发送相机数字增益参数
    void sendGainTargetParams(int nUnit);       //! 发送相机增益目标参数
    void sendInfChannelParams(int nUnit);       //! 发送红外相机象元参数
    void sendInfAnaGainParams(int nUnit);       //! 发送红外相机模拟增益参数
    void sendInfDigGainParams(int nUnit);       //! 发送红外相机数字增益参数
    void sendInfGainTargetParams(int nUnit);    //! 发送红外相机增益目标参数
    void sendEjectorNumParams(int nUnit);       //! 发送喷嘴数目信息
    void sendMatterCodeParams(int nUnit);       //! 发送物料代号信息
    void sendCameraAdvParams(void);             //! 发送相机板高级参数(行频、交叉象元及镜头参数)
    void sendBiasingParams(int nUnit);          //! 发送象元偏置参数
    void sendInfBiasingParams(int nUnit);       //! 发送红外象元偏置参数

    /** 控制板相关参数 */
    void sendCtrlParams(void);                  //! 发送控制板参数
    void sendLampParamsMono(void);              //! 发送灯光控制参数
    void setLampAndBGByMode(int mode);          //! 获取当前模式的灯控等参数
    void sendFeederRemoteCtrlParams(void);      //! 发送振动器远程控制参数

    /** 共享参数相关函数 */
    void setStringByDirect(void);       //! 根据剔除方向设置字符串的显示
    void setHasInfByParams(void);       //! 根据全局参数设置识别组是否有辅助相机标志位
    void clearDelayBias(void);          //! 清空延迟偏置
    QString getNameByMode(int nMode);   //! 根据模式索引号获取模式名称

    /** 色选参数相关函数 */
    void sendSortParams(void);          //! 发送色选参数
    void sendSortMode(void);   			//! 发送正反选模式
    void sendImpuredParams(void);		//! 发送传染参数
    void sendRelationParams(void);      //! 发送前后视逻辑关系
    void sendSensParams(int nIdeGroup, int nUnitIndex);     //! 发送独立灵敏度
    void sendAllSensParams(void);       //! 发送所有相机板灵敏度

    /** 定制机型相关函数 */
    void checkIsCustom(void);           //! 检查是否为黑白定制机型   
    void checkIsMC(void);               //! 判断是否为印度定制机型(MC系列)
    bool checkIsR2(void);               //! 判断是否为R2机型
    
signals:
    
public slots:

private:
    void setTargetByMode(int nMode);    //! 根据模式设置增益目标
    void setLampByMode(int nMode);      //! 根据模式设置灯控
};

/* 将MonoFlow定义为全局参数 */
extern MonoFlow myMonoFlow;

#endif // MONOFLOW_H
