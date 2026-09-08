/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        monoflow.cpp
 * \brief       黑白机型全局流程源文件。
 * \date        2015.01.14
 */
#include "monoflow.h"

/* 实例化MonoFlow */
MonoFlow myMonoFlow;

MonoFlow::MonoFlow(QObject *parent) :
    QObject(parent)
{
}

int MonoFlow::checkMasterCameraBoard()
{
    int ret, nAddr = 0;
    int err = 0;
    char buf[24];

    /*  主配前置板  */
    for (int i = 0; i < struCnfg.nLevelTotal ; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            for (int m = 0; m < 3; m++) {       // 循环次数
                MySerial.com1Write(CMD_UNIT_CAMERA_VERSION, UNIT, i, nAddr, 0, 0, 0, CAMERA_COLOR, 0, 3);
                myFlow.msleep(50);
                memset(buf, 0, sizeof(buf));
                if((ret = MySerial.com1Read(buf, 18)) == 18) {
                    struGsh.struVer.sUnit[i][j][0] = buf[11];
                    struGsh.struVer.sUnit[i][j][1] = buf[10] & 0x0F;
                    break;
                } else {
                    struGsh.struVer.sUnit[i][j][0] = 255;
                    struGsh.struVer.sUnit[i][j][1] = 255;
                    /* 尝试3次都失败后设置错误标志位为1 */
                    if(m == 2) {
                        err = 1;
                    }
                }
            }
        }
    }
    return err;
}

int MonoFlow::checkInterfaceBoard()
{
    int i, j, ret;
    int err = 0;
    char buf[24];

    /* 接口板*/
    for(i = 0; i < struCnfg.nInterfaceBoardTotal ; i++) {
        for (j = 0; j < 3; j++) {               // 循环次数
            MySerial.com1Write(CMD_INT_VERSION, INT, i, 0, 0, 0, 0, 0, 0, 3);
            myFlow.msleep(50);
            memset(buf, 0, sizeof(buf));
            ret = MySerial.com1Read(buf, 18);
            if (ret == 18) {
                struGsh.struVer.sInterface[i][0] = buf[11];
                struGsh.struVer.sInterface[i][1] = buf[10];
                break;
            } else {
                struGsh.struVer.sInterface[i][0] = 255;
                struGsh.struVer.sInterface[i][1] = 255;
                /* 尝试3次都失败后设置错误标志位为1 */
                if(j == 2) {
                    err = 1;
                }
            }
        }
    }
    return err;
}

int MonoFlow::checkCtrlBoard()
{
    int i, j, nRet;
    int err = 0;
    char buf[24];
    /* 控制板 */
    for (i = 0; i < struCnfg.nCtrlBoardTotal; i++) {
        for (j = 0; j < 3; j++) {               // 循环次数
            MySerial.com2Write(CMD_CTRL_VERSION, CTRL_GENERAL, i, 0, 0, 0, 0, 0, 3);
            myFlow.msleep(50);
            memset(buf, 0, sizeof(buf));
            nRet = MySerial.com2Read(buf, 18);
            if (nRet == 18 && buf[0] == 0xAA && buf[1] == 0xAA
                    && buf[2] == 0xAA && buf[3] == 0xAA) {
                struGsh.struVer.sControl[i][0] = buf[11];
                struGsh.struVer.sControl[i][1] = buf[10];
            }else {
                struGsh.struVer.sControl[i][0] = 255;
                struGsh.struVer.sControl[i][1] = 255;
                /* 尝试3次都失败后设置错误标志位为1 */
                if(j == 2) {
                    err = 1;
                }
            }
        }
    }
    return err;
}

int MonoFlow::checkLightSourceBoard()
{
    int err = 0, nRet = 0;
    char buf[24];

    /** 恒流源板 */
    for (int i = 0; i < struCnfg.nLampAINum; i++) {
        /** 若恒流源板未使用则不再检测 */
        if (struCnfg.nCurSrcBoradLampNum[i] == 0) {
            continue;
        }

        for (int j = 0; j < 3; j++) {
            MySerial.com2Write(CMD_CTRL_LIGHT_SRC_VERSION, CTRL_LED, i, 0, 0, 0, 0, 0, 3);
            myFlow.msleep(50);
            memset(buf, 0, sizeof(buf));
            nRet = MySerial.com2Read(buf, 18);
            if (nRet == 18
                    && buf[0] == 0xAA
                    && buf[1] == 0xAA
                    && buf[2] == 0xAA
                    && buf[3] == 0xAA
                    && buf[5] == i
                    && buf[6] == 0x05
                    && buf[7] == 0x03) {
                struGsh.struVer.sLightSrc[i][0] = buf[11];
                struGsh.struVer.sLightSrc[i][1] = buf[10];
                break;
            } else {
                struGsh.struVer.sLightSrc[i][0] = 255;
                struGsh.struVer.sLightSrc[i][1] = 255;
                /** 尝试3次都失败后设置错误标志位为1 */
                if (j == 2) {
                    err = 1;
                }
            }
        }
    }

    return err;
}

/* RS机型通信自检 */
int MonoFlow::initCommunicationMono()
{
    int err = 0;

    if (checkMasterCameraBoard()) {
        err = 1;
    }

    if (checkInterfaceBoard()) {
        err = 1;
    }

    if (checkCtrlBoard()) {
        err = 1;
    }

    if (checkLightSourceBoard()) {
        err = 1;
    }

    /** 根据通信状态设置通信标志位 */
    if (err == 1) {
        struGsh.nCommStat = 1;
    } else {
        struGsh.nCommStat = 0;
    }

    return err;
}

/* 发送病点色差与坏料面积参数 */
void MonoFlow::sendDiscolorAndAreaParams()
{
    for(int i=0; i<struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal; i++) {
        for(int j=0; j<struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitCount; j++) {
            /* 主配 */
            int unitId = myFlow.getIdentifyGroupAddr(struGsh.nLevel, i, j);
            MySerial.com1Write(CMD_UNIT_GREY_B, UNIT, struGsh.nLevel, unitId,
                    struCnfp.struGroupIdentify[struGsh.nLevel][i].struRice.nRow-1,
                    struCnfp.struGroupIdentify[struGsh.nLevel][i].struRice.nCol-1,
                    struCnfp.struGroupIdentify[struGsh.nLevel][i].struRice.nYellowArea-1,
                    struCnfp.struGroupIdentify[struGsh.nLevel][i].struRice.nBadWidth-1,
                    struCnfp.struGroupIdentify[struGsh.nLevel][i].struRice.nBadDiscolor, 3);

        }
    }
}

/* RS重置喷阀时间 */
void MonoFlow::resetEjectTimeMono()
{
    int nEjectDelay;
    int nUnitAddr, tmp;

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                tmp = struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k];
                nUnitAddr = myFlow.getTickGroupAddr(i, j, k);

                /* RS机型吹气延迟较页面上显示固定减3.0ms */
                nEjectDelay = struCnfp.struGroupTick[i][j].nEjectDelay/10-30+struCnfp.nDelayBiasing[tmp/2];

                /* RS机型开传染固定减1.6ms */
                if (struCnfp.struGroupTick[i][j].nImpured == 1) {
                    nEjectDelay = nEjectDelay-16;
                }

                if (nEjectDelay < 0) {
                    nEjectDelay = 0;
                } else if(nEjectDelay > 100) {
                    nEjectDelay = 100;
                }

                MySerial.com1Write(CMD_INT_EJTIME, INT, i, nUnitAddr, 0, 0,
                                   struCnfp.nDegaussTime, nEjectDelay,
                                   struCnfp.struGroupTick[i][j].nEjectDuration/10, 3);
                MySerial.com1Write(CMD_INT_EJECT_WIDTH, INT, i, nUnitAddr, 0, 0, 0, 0,
                                   struCnfp.struGroupTick[i][j].nEjwidth, 3);
            }
        }
    }

    //! 喷阀开关
	int nTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
	for (int i = 0; i < nTotal; i++) {
		MySerial.com1Write(CMD_UNIT_EJECT_ONOFF, UNIT, 0, i,
						0, 0, 0, 0, struGsh.bStatEjectFpga, 3);
	}
}

/* RS机型开机及开关下料后发送所有参数 */
void MonoFlow::sendAllParamsMono()
{
    sendInterfaceParams(); 

    sendCameraParams();

    sendCtrlParams();

    sendSortParams();
}

/*!
 * \brief 整机发送启动/停止命令
 * \param nFlag 0:停止 1:启动
 */
void MonoFlow::sendOnOffParams(int nFlag)
{
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++) {
        MySerial.com1Write(CMD_INT_ONOFF, INT, i, 0, 0, 0, 0, 0, nFlag, 3);
    }
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            int nUnitId = struCnfg.struLevelInfo[i].nUnitId[j];
            MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nUnitId, 0, 0, 0, 0, nFlag, 3);
        }
    }
}

/* 发送接口板参数 */
void MonoFlow::sendInterfaceParams()
{
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++) {
        /** 喷嘴数量 */
        MySerial.com1Write(CMD_INT_EJECTOR_SET, INT, i, 0, 0, 0, struCnfg.nEjectorsPerChute, 0, 0, 3);
    }

    /** 吹气时间及吹气模式 */
    resetEjectTimeMono();

    /** 前后视逻辑关系 */
    sendRelationParams();
}

/* 发送主配前置板参数 */
void MonoFlow::sendMasterCameraParams()
{
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        /* 象元划分、模拟增益、数字增益与增益目标 */
        sendChannelParams(i);
        sendAnaGainParams(i);
        sendDigGainParams(i);
        sendGainTargetParams(i);
        sendEjectorNumParams(i);
        sendMatterCodeParams(i);
    }
    //! 相机高级参数
    sendCameraAdvParams();

    //! 正反选模式
    sendSortMode();

    //! 传染算法
    sendImpuredParams();
}

/* 发送前置板参数 */
void MonoFlow::sendCameraParams()
{
    sendMasterCameraParams();
}

/* 发送控制板参数 */
void MonoFlow::sendCtrlParams()
{
    /* 供料参数 */
    myFlow.resetFeeder(0);
    myFlow.resetFeeder(1);

    /* 清灰与吹风气帘 */
    MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0,
                       0, 0, 0, struCnfp.struGroupCtrl[0].nWipeDuration, 0x00, 3);
    MySerial.com2Write(CMD_CTRL_WIPE, CTRL_GENERAL, 0,
                       0, 0, 0, struCnfp.struGroupCtrl[0].nWipeWindDuration, 0x00, 3);

    /* 灯光控制 */
    sendLampParamsMono();

    /* 振动器远程控制模式 */
    sendFeederRemoteCtrlParams();
}

/* 发送色选参数 */
void MonoFlow::sendSortParams()
{
    /* 算法使能 */
    char cArithEnable = (struCnfp.nArithmeticEnable[ARITH_SCALE] << 1)
                            | struCnfp.nArithmeticEnable[ARITH_SHAPE_LENGTH];
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
        MySerial.com1Write(CMD_UNIT_ARITHMETIC_ENABLE, UNIT, 0, i,
                           struCnfg.nAssistCamEn[i]<<7, 0, 0, 0, cArithEnable, 3);
    }

    /* 坏点色差与坏料面积 */
    sendDiscolorAndAreaParams();

    /* 算法灵敏度 */
    sendAllSensParams();
}

/*!
 * \brief MonoFlow::sendSortModeAndInfect
 *        发送正反选模式及传染算法
 */
void MonoFlow::sendSortMode()
{
    int nUnitAddr = 0;
    int nMode = struCnfg.nProfileMode[struCnfg.nProfile];

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                nUnitAddr = myFlow.getTickGroupAddr(i, j, k);

                /** 根据模式发送正反选参数 */
                switch (nMode) {
                case RS_DARK_SORT:
                    /** 仅后视色选,前视色选模式固定为正选 */
                    if (nUnitAddr%2 == 0) {
                        MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr, 0, 0, 0, 0, 0, 3);
                    } else {
                        MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr,
                                           0, 0, 0, 0, struCnfp.struGroupTick[i][j].nReverseSort, 3);
                    }
                    break;

                case RS_WHITE_NEGATIVE_SORT:
                case RS_WHITE_POSITIVE_SORT:
                    /** 仅前视色选,后视色选模式固定为正选 */
                    if (nUnitAddr%2 == 0) {
                        MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr,
                                           0, 0, 0, 0, struCnfp.struGroupTick[i][j].nReverseSort, 3);
                    } else {
                        MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr, 0, 0, 0, 0, 0, 3);
                    }
                    break;

                default:
                    MySerial.com1Write(CMD_UNIT_REVERSE_SORT, UNIT, i, nUnitAddr,
                                       0, 0, 0, 0, struCnfp.struGroupTick[i][j].nReverseSort, 3);
                    break;
                }
            }
        }
    }
}

/*!
 * \brief MonoFlow::sendImpuredParams
 *        发送传染参数
 */
void MonoFlow::sendImpuredParams()
{
	int nUnitAddr = 0;
	for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
			if (struCnfp.struGroupTick[i][j].nReverseSort == 1
				|| struCnfp.nArithmeticEnable[ARITH_SCALE] == 1
                || struCnfp.nArithmeticEnable[ARITH_SHAPE_LENGTH] == 1) {
            	struCnfp.struGroupTick[i][j].nImpured = 1;
        	} else {
            	struCnfp.struGroupTick[i][j].nImpured = 0;
        	}

            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                nUnitAddr = myFlow.getTickGroupAddr(i, j, k);
                MySerial.com1Write(CMD_UNIT_INFECT, UNIT, i, nUnitAddr,
                                   0, 0, 0, 0, struCnfp.struGroupTick[i][j].nImpured, 3);
			}
		}
	}
}

/*!
 * \brief MonoFlow::sendRelationParams
 *        发送前后视逻辑关系
 */
void MonoFlow::sendRelationParams()
{
    int nRelationLow = 0;
    int nRelationHigh = 0;
    int bit[16] = {0};
    int sIndex = 0;

    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nTickGroupTotal; j++) {
            for (int k = 0; k < struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitCount; k++) {
                sIndex = struCnfg.struLevelInfo[i].struTickGroupInfo[j].nUnitId[k]/2;
                bit[sIndex] = struCnfp.struGroupTick[i][j].nFrontRearMode;
            }
        }
    }

    nRelationLow = bit[0] | (bit[1]<<1) | (bit[2]<<2) | (bit[3]<<3) | (bit[4]<<4) | (bit[5]<<5)
                                | (bit[6]<<6) | (bit[7]<<7);
    nRelationHigh = bit[8] | (bit[9]<<1) | (bit[10]<<2) | (bit[11]<<3) | (bit[12]<<4) | (bit[13]<<5)
                                | (bit[14]<<6) | (bit[15]<<7);

    MySerial.com1Write(CMD_INT_FRONT_REAR_RELATION, INT, 0, 0, 0, 0, 0, 0, nRelationLow, 3);
    MySerial.com1Write(CMD_INT_FRONT_REAR_RELATION, INT, 1, 0, 0, 0, 0, 0, nRelationHigh, 3);
}

/*!
 * \brief 发送独立灵敏度
 * \param nIdeGroup 识别组
 * \param nUnitIndex 相机在识别组中索引号
 */
void MonoFlow::sendSensParams(int nIdeGroup, int nUnitIndex)
{
    int nUnit = struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[nIdeGroup].nUnitId[nUnitIndex];
    int nUnitAddr = myFlow.getIdentifyGroupAddr(0, nIdeGroup, nUnitIndex);

    int nSens = 0;
    int nDarkSens = 0;
    int nSplice = struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[nIdeGroup].nUnitSplice[nUnitIndex];

    //! 参数除2是因为程序保存参数范围为0至1000,参数下发为0至500
    if (struCnfg.nParamSetAlone == 1) {
        //! 独立设置
        if (nSplice == 2) {
            nSens = struCnfp.nSens[nUnit][1]/2;
        } else {
            nSens = struCnfp.nSens[nUnit][0]/2;
        }
        //! 自定义模式下复制nSens为反选灵敏度,nDarkSens为正选灵敏度
        if (struCnfg.nProfileMode[struCnfg.nProfile] == RS_USER_DEFINED_SORT) {
            nDarkSens = nSens;
            nSens = 0;
        }
    } else {
        //! 分次设置
        nSens = struCnfp.struGroupIdentify[ONE_LEVEL][nIdeGroup].struRice.nSens/2;
        nDarkSens = struCnfp.struGroupIdentify[ONE_LEVEL][nIdeGroup].struRice.nDarkSens/2;
    }

    //! 主配灵敏度
    switch(struCnfg.nProfileMode[struCnfg.nProfile]) {
    case RS_DARK_SORT:          //! 深色专选
        if (nUnitAddr%2) {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
        } else {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, 0, 0, 3);
        }
        break;
    case RS_DIFF_POSITIVE_SORT: //! 异色粒正选
        MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
        break;
    case RS_DIFF_NEGATIVE_SORT: //! 异色粒反选
        MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
        break;
    case RS_WHITE_POSITIVE_SORT://! 乳白色正选
        if (nUnit%2) {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, 0, 0, 3);
        } else {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
        }
        break;
    case RS_WHITE_NEGATIVE_SORT://! 乳白色反选
        if (nUnit%2) {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, 0, 0, 3);
        } else {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
        }
        break;
    case RS_YELLOW_WHITE_SORT:  //! 黄白同选
        MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0,nSens/256, nSens%256, 3);
        break;
    case RS_YELLOW_TRANS_SORT:  //! 黄透同选
        if (nUnit%2) {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
        } else {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
        }
        break;
    case RS_YELLOW_YELLOW_NEGATIVE_SORT:    //! 黄+黄反选
        if (nUnit%2) {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
        } else {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
        }
        break;
    case RS_WHITE_YELLOW_NEGATIVE_SORT:     //! 黄+白反选
        if (nUnit%2) {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0,nSens/256, nSens%256, 0, 0, 3);
        } else {
            MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
        }
        break;
    case RS_USER_DEFINED_SORT:              //! 自定义模式
        MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, 0, nUnitAddr,
                           0, nSens/256, nSens%256, nDarkSens/256, nDarkSens%256, 3);
        break;
    default:
        break;
    }
}

/*!
 * \brief 发送所有相机板灵敏度
 */
void MonoFlow::sendAllSensParams()
{
    int nIdeGpTotal = struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal;
    for (int i = 0; i < nIdeGpTotal; i++) {
        int nUnitCount = struCnfg.struLevelInfo[ONE_LEVEL].struIdentifyGroupInfo[i].nUnitCount;
        for (int j = 0; j < nUnitCount; j++) {
            sendSensParams(i, j);
        }
    }
}

/*!
 * \brief 检查是否为黑白定制机型
 */
void MonoFlow::checkIsCustom()
{
    checkIsMC();
}

/* 判断是否为印度定制机型(MC系列) */
void MonoFlow::checkIsMC()
{
    struGsh.bIsMC = false;
}

/*!
 * \brief MonoFlow::checkIsR2
 * \return
 */
bool MonoFlow::checkIsR2()
{
    return false;
}

/*!
 * \brief 根据模式设置增益目标
 * \param nMode 当前模式
 */
void MonoFlow::setTargetByMode(int nMode)
{
    //! 主配增益目标
    switch (nMode) {
    case RS_DIFF_POSITIVE_SORT:
    case RS_DARK_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            struCnfp.nGainTarget[i] = 200;
        }
        break;

    case RS_DIFF_NEGATIVE_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            struCnfp.nGainTarget[i] = 10;
        }
        break;

    case RS_WHITE_POSITIVE_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            struCnfp.nGainTarget[i] = 200;
        }
        break;

    case RS_WHITE_NEGATIVE_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            struCnfp.nGainTarget[i] = 10;
        }
        break;

    case RS_YELLOW_WHITE_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            struCnfp.nGainTarget[i] = 200;
        }
        break;

    case RS_YELLOW_TRANS_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            if (i%2 == 0) {
                struCnfp.nGainTarget[i] = 10;
            } else {
                struCnfp.nGainTarget[i] = 200;
            }
        }
        break;

    case RS_YELLOW_YELLOW_NEGATIVE_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            if (i%2 == 0) {
                struCnfp.nGainTarget[i] = 200;
            } else {
                struCnfp.nGainTarget[i] = 10;
            }
        }
        break;

    case RS_WHITE_YELLOW_NEGATIVE_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            if (i%2 == 0) {
                struCnfp.nGainTarget[i] = 200;
            } else {
                struCnfp.nGainTarget[i] = 10;
            }
        }
        break;

    case RS_USER_DEFINED_SORT:
        for (int i = 0; i < MAX_UNIT; i++) {
            struCnfp.nGainTarget[i] = 150;
        }
        break;

    default:
        break;
    }

}

/*!
 * \brief RF机型根据模式设置灯控
 * \param nMode
 */
void MonoFlow::setLampByMode(int nMode)
{
    QVector <int> vLamp;
    switch (nMode) {
    case RS_DIFF_POSITIVE_SORT:
    case RS_DARK_SORT:
        vLamp << 1 << 1 << 1 << 0 << 0 << 1
                 << 1 << 1 << 1 << 0 << 0 << 1;
        break;

    case RS_DIFF_NEGATIVE_SORT:
        vLamp << 1 << 1 << 0 << 0 << 0 << 1
                 << 1 << 1 << 0 << 0 << 0 << 1;
        break;

    case RS_WHITE_POSITIVE_SORT:
        vLamp << 0 << 0 << 0 << 0 << 0 << 1
                 << 1 << 1 << 1 << 0 << 0 << 1;
        break;

    case RS_WHITE_NEGATIVE_SORT:
        vLamp << 0 << 0 << 0 << 0 << 0 << 1
                 << 1 << 1 << 0 << 0 << 0 << 1;
        break;

    case RS_YELLOW_WHITE_SORT:
        vLamp << 1 << 0 << 1 << 0 << 0 << 1
                 << 1 << 1 << 1 << 0 << 0 << 1;
        break;

    case RS_YELLOW_TRANS_SORT:
        vLamp << 1 << 0 << 1 << 0 << 0 << 1
                 << 1 << 1 << 0 << 0 << 0 << 1;
        break;

    case RS_YELLOW_YELLOW_NEGATIVE_SORT:
        vLamp << 1 << 1 << 0 << 0 << 0 << 1
                 << 1 << 1 << 1 << 0 << 0 << 1;
        break;

    case RS_WHITE_YELLOW_NEGATIVE_SORT:
        vLamp << 0 << 0 << 0 << 0 << 0 << 1
                 << 1 << 1 << 1 << 0 << 0 << 1;
        break;

    case RS_USER_DEFINED_SORT:
        vLamp << 1 << 1 << 1 << 0 << 0 << 1
                 << 1 << 1 << 1 << 0 << 0 << 1;
        break;

    default:
        break;
    }

    for (int i = 0; i < struCnfg.nLampTotal; i++) {
        struCnfp.struGroupCtrl[0].nLamp[i] = vLamp.at(i);
    }
}

/* 发送相机象元参数 */
void MonoFlow::sendChannelParams(int nUnit)
{
    MySerial.com1Write(CMD_UNIT_CHANNEL, UNIT, struGsh.nLevel, nUnit, 0,
                    struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[nUnit]/256,
                    struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[nUnit]%256,
                    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[nUnit]/256,
                    struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[nUnit]%256, 3);
}

/* 发送相机模拟增益参数 */
void MonoFlow::sendAnaGainParams(int nUnit)
{
    MySerial.com1Write(CMD_UNIT_ANA_GAIN, UNIT, 0, nUnit, 0, 0, 0,
                       struCnfp.nAnaGain[nUnit]/256, struCnfp.nAnaGain[nUnit]%256, 3);
}

/* 发送相机数字增益参数 */
void MonoFlow::sendDigGainParams(int nUnit)
{
    MySerial.com1Write(CMD_UNIT_DIGI_GAIN_INCH, UNIT, 0, nUnit, 0, 0, 0,
                       struCnfp.nDigGain[nUnit]/256, struCnfp.nDigGain[nUnit]%256, 3);
}

/* 发送相机增益目标参数 */
void MonoFlow::sendGainTargetParams(int nUnit)
{
    MySerial.com1Write(CMD_UNIT_BG_COLOR, UNIT, 0, nUnit, 0, 0, 0,
                       struCnfp.nGainTarget[nUnit]/256, struCnfp.nGainTarget[nUnit]%256, 3);
}

/*!
 * \brief 发送红外相机象元参数
 * \param nUnit 相机板地址
 */
void MonoFlow::sendInfChannelParams(int nUnit)
{
}

/*!
 * \brief 发送红外相机模拟增益参数
 * \param nUnit 相机板地址
 */
void MonoFlow::sendInfAnaGainParams(int nUnit)
{
}

/*!
 * \brief 发送红外相机数字增益参数
 * \param nUnit 相机板地址
 */
void MonoFlow::sendInfDigGainParams(int nUnit)
{
}

/*!
 * \brief 发送红外相机增益目标参数
 * \param 相机板地址
 */
void MonoFlow::sendInfGainTargetParams(int nUnit)
{
}

/*!
 * \brief 发送喷嘴数目信息
 * \param nUnit 相机板地址
 */
void MonoFlow::sendEjectorNumParams(int nUnit)
{
    MySerial.com1Write(CMD_UNIT_EJECTOR_SET, UNIT, 0, nUnit, 0, 0, struCnfg.nEjectorsPerChute, 0, 0, 3);
}

/*!
 * \brief 发送物料代号信息
 * \param nUnit 相机板地址
 */
void MonoFlow::sendMatterCodeParams(int nUnit)
{
    MySerial.com1Write(CMD_UNIT_MATTER_NUM, UNIT, 0, nUnit, 0, 0, 0, struCnfg.nProfile+1, struCnfg.nProfile+1, 3);
}

/*!
 * \brief 发送相机板高级参数(行频、交叉象元及镜头参数)
 */
void MonoFlow::sendCameraAdvParams()
{
    int nUnitTotal = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
    for (int i = 0; i < nUnitTotal; i++) {
        //! 帧频设置
        if (i%2 == 0) { // 前视
            MySerial.com1Write(CMD_UNIT_FRAME_FRE, UNIT, 0, i, 0, 0, 0, 0, struCnfc.nFrontExpTime, 3);
        } else {        // 后视
            MySerial.com1Write(CMD_UNIT_FRAME_FRE, UNIT, 0, i, 0, 0, 0, 0, struCnfc.nRearExpTime, 3);
        }

        //! 镜头参数
        MySerial.com1Write(CMD_UNIT_LENS_TYPE, UNIT, 0, i, 0, 0, 0, 0, struCnfc.nLensType, 3);
    }

    //! 交叉象元
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitCount; j++) {
            int unitId = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitId[j];
            int addr = myFlow.getTickGroupAddr(0, i, j);
            MySerial.com1Write(CMD_UNIT_CROSS_CHANNEL, UNIT, 0, addr,
                               0, 0, 0, 0, struCnfp.nCrossChannel[i], 3);
        }
    }
}

/*!
 * \brief 发送象元偏置参数
 * \param nUnit 相机地址
 */
void MonoFlow::sendBiasingParams(int nUnit)
{
}

void MonoFlow::sendInfBiasingParams(int nUnit)
{
}


/* 根据模式索引号获取模式名称 */
QString MonoFlow::getNameByMode(int nMode)
{
    QStringList list;
    list.push_back(myLan.mode_dark);
    list.push_back(myLan.mode_discolor);
    list.push_back(myLan.mode_discolor_reverse);
    list.push_back(myLan.mode_milky);
    list.push_back(myLan.mode_milky_reverse);
    list.push_back(myLan.mode_yellow_milky);
    list.push_back(myLan.mode_yellow_trans);
    list.push_back(myLan.mode_yellow_yellow_reverse);
    list.push_back(myLan.mode_yellow_white_reverse);
    list.push_back(myLan.mode_user_defined);

    if (nMode < 0 || nMode > RS_SORT_MODE_TYPE-1) {
        return QString();
    }

    return list.at(nMode);
}

/* 发送RS机型灯光控制参数 */
void MonoFlow::sendLampParamsMono()
{
    for (int i = 0; i < struCnfg.nLampTotal; i++) {
        /* 目前RS机型设计为12盏灯，前分选室的6盏灯由恒流源板1控制，后分选室的由恒流源板2控制 */
        if(struCnfp.struGroupCtrl[0].nLamp[i] == 1) {
            MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, i/MAX_LAMP_PER_BOARD,
                               0,
                               0,
                               i%MAX_LAMP_PER_BOARD,
                               struCnfp.struGroupCtrl[0].nLampLight[i]/256,
                               struCnfp.struGroupCtrl[0].nLampLight[i]%256,
                               3);
        } else {
            MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, i/MAX_LAMP_PER_BOARD,
                               0,
                               0,
                               i%MAX_LAMP_PER_BOARD,
                               0,
                               0,
                               3);
        }
    }
}

/* 新建方案时，获取当前模式的灯控等参数 */
void MonoFlow::setLampAndBGByMode(int mode)
{
    setTargetByMode(mode);

    setLampByMode(mode);
}

/* 发送振动器远程控制参数 */
void MonoFlow::sendFeederRemoteCtrlParams()
{
    MySerial.com2Write(CMD_CTRL_FEEDER_CTRL_MODE, CTRL_GENERAL,
                       0, 0, 0, 0, 0, struCnfg.nFeederRemoteCtrlMode, 3);
}

/* 根据剔除方向设置字符串的显示 */
void MonoFlow::setStringByDirect()
{
    switch(struCnfg.nTickDirect) {
    case 0:     // 1->2->3
        struGsh.first = myLan.first;
        struGsh.second = myLan.second;
        struGsh.third = myLan.third;
        break;
    case 1:     // 2->1->3
        struGsh.first = myLan.second;
        struGsh.second = myLan.first;
        struGsh.third = myLan.third;
        break;
    case 2:     // 3->2->1
        struGsh.first = myLan.third;
        struGsh.second = myLan.second;
        struGsh.third = myLan.first;
        break;
    default:
        break;
    }
}

/* 根据全局参数设置识别组是否有辅助相机标志位 */
void MonoFlow::setHasInfByParams()
{
}

/* 清空延迟偏置 */
void MonoFlow::clearDelayBias()
{
    for (int i = 0; i < MAX_UNIT/2; i++) {
        struCnfp.nDelayBiasing[i] = 0;
    }
}
