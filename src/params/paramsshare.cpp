#include "globalflow.h"

/*!
 * \brief 初始化D系传感器镜头畸变参数
 */
void GlobalFlow::initChannelPixelCompensateDseries()
{
    switch(struCnfg.nEjectorsPerChute) {
    case 60:
        struGsh.fPixelCompensate[0]  = 0.0;
        struGsh.fPixelCompensate[1]  = -1.1;
        struGsh.fPixelCompensate[2]  = -2.2;
        struGsh.fPixelCompensate[3]  = -3.3;
        struGsh.fPixelCompensate[4]  = -4.2;
        struGsh.fPixelCompensate[5]  = -5.0;
        struGsh.fPixelCompensate[6]  = -5.7;
        struGsh.fPixelCompensate[7]  = -6.3;
        struGsh.fPixelCompensate[8]  = -6.8;
        struGsh.fPixelCompensate[9]  = -7.2;
        struGsh.fPixelCompensate[10] = -7.5;
        struGsh.fPixelCompensate[11] = -7.7;
        struGsh.fPixelCompensate[12] = -7.8;
        struGsh.fPixelCompensate[13] = -7.8;
        struGsh.fPixelCompensate[14] = -7.8;
        struGsh.fPixelCompensate[15] = -7.6;
        struGsh.fPixelCompensate[16] = -7.4;
        struGsh.fPixelCompensate[17] = -7.1;
        struGsh.fPixelCompensate[18] = -6.8;
        struGsh.fPixelCompensate[19] = -6.4;
        struGsh.fPixelCompensate[20] = -5.9;
        struGsh.fPixelCompensate[21] = -5.4;
        struGsh.fPixelCompensate[22] = -4.8;
        struGsh.fPixelCompensate[23] = -4.2;
        struGsh.fPixelCompensate[24] = -3.6;
        struGsh.fPixelCompensate[25] = -2.9;
        struGsh.fPixelCompensate[26] = -2.2;
        struGsh.fPixelCompensate[27] = -1.5;
        struGsh.fPixelCompensate[28] = -0.8;
        struGsh.fPixelCompensate[29] = 0.0;
        struGsh.fPixelCompensate[30] = 0.0;
        struGsh.fPixelCompensate[31] = 0.9;
        struGsh.fPixelCompensate[32] = 1.8;
        struGsh.fPixelCompensate[33] = 2.7;
        struGsh.fPixelCompensate[34] = 3.5;
        struGsh.fPixelCompensate[35] = 4.4;
        struGsh.fPixelCompensate[36] = 5.2;
        struGsh.fPixelCompensate[37] = 5.9;
        struGsh.fPixelCompensate[38] = 6.6;
        struGsh.fPixelCompensate[39] = 7.3;
        struGsh.fPixelCompensate[40] = 7.8;
        struGsh.fPixelCompensate[41] = 8.4;
        struGsh.fPixelCompensate[42] = 8.8;
        struGsh.fPixelCompensate[43] = 9.2;
        struGsh.fPixelCompensate[44] = 9.4;
        struGsh.fPixelCompensate[45] = 9.7;
        struGsh.fPixelCompensate[46] = 9.7;
        struGsh.fPixelCompensate[47] = 9.7;
        struGsh.fPixelCompensate[48] = 9.6;
        struGsh.fPixelCompensate[49] = 9.4;
        struGsh.fPixelCompensate[50] = 9.0;
        struGsh.fPixelCompensate[51] = 8.5;
        struGsh.fPixelCompensate[52] = 7.9;
        struGsh.fPixelCompensate[53] = 7.1;
        struGsh.fPixelCompensate[54] = 6.2;
        struGsh.fPixelCompensate[55] = 5.1;
        struGsh.fPixelCompensate[56] = 4.0;
        struGsh.fPixelCompensate[57] = 2.8;
        struGsh.fPixelCompensate[58] = 1.4;
        struGsh.fPixelCompensate[59] = 0.0;
        break;
    default:
        break;
    }
}

void GlobalFlow::initChannelPixelCompensate()
{
    int i;

    for (i = 0; i < struCnfg.nEjectorsPerChute; i++) {
         struGsh.fPixelCompensate[i] = 0.0;
    }

    if(struCnfc.nLensType == 0) {       // 非美亚定制镜头，不需要镜头畸变补偿系数
        for (i = 0; i < struCnfg.nEjectorsPerChute; i++) {
            struGsh.fPixelCompensate[i] = 0.0;
        }
        return;
    }
    if(struCnfc.nSensorType == SENSOR_D2) {
        initChannelPixelCompensateDseries();
        return;
    }

    //! 根据光学室反馈，镜头畸变受喷嘴及分辨率影响
    switch(struCnfg.nEjectorsPerChute) {
    case 60:
        if (struCnfc.nSensorPixel == SENSOR_PIXELS_2048) {
            struGsh.fPixelCompensate[0] = 0;
            struGsh.fPixelCompensate[1] = -1.1;
            struGsh.fPixelCompensate[2] = -2.1;
            struGsh.fPixelCompensate[3] = -3.0;
            struGsh.fPixelCompensate[4] = -3.8;
            struGsh.fPixelCompensate[5] = -4.4;
            struGsh.fPixelCompensate[6] = -5.1;
            struGsh.fPixelCompensate[7] = -5.6;
            struGsh.fPixelCompensate[8] = -5.9;
            struGsh.fPixelCompensate[9] = -6.2;
            struGsh.fPixelCompensate[10] = -6.5;
            struGsh.fPixelCompensate[11] = -6.6;
            struGsh.fPixelCompensate[12] = -6.6;
            struGsh.fPixelCompensate[13] = -6.6;
            struGsh.fPixelCompensate[14] = -6.5;
            struGsh.fPixelCompensate[15] = -6.4;
            struGsh.fPixelCompensate[16] = -6.2;
            struGsh.fPixelCompensate[17] = -5.9;
            struGsh.fPixelCompensate[18] = -5.6;
            struGsh.fPixelCompensate[19] = -5.2;
            struGsh.fPixelCompensate[20] = -4.8;
            struGsh.fPixelCompensate[21] = -4.4;
            struGsh.fPixelCompensate[22] = -3.9;
            struGsh.fPixelCompensate[23] = -3.4;
            struGsh.fPixelCompensate[24] = -2.9;
            struGsh.fPixelCompensate[25] = -2.4;
            struGsh.fPixelCompensate[26] = -1.8;
            struGsh.fPixelCompensate[27] = -1.2;
            struGsh.fPixelCompensate[28] = -0.6;
            struGsh.fPixelCompensate[29] = 0;
            struGsh.fPixelCompensate[30] = 0.6;
            struGsh.fPixelCompensate[31] = 1.1;
            struGsh.fPixelCompensate[32] = 1.7;
            struGsh.fPixelCompensate[33] = 2.3;
            struGsh.fPixelCompensate[34] = 2.9;
            struGsh.fPixelCompensate[35] = 3.4;
            struGsh.fPixelCompensate[36] = 3.9;
            struGsh.fPixelCompensate[37] = 4.4;
            struGsh.fPixelCompensate[38] = 4.9;
            struGsh.fPixelCompensate[39] = 5.3;
            struGsh.fPixelCompensate[40] = 5.7;
            struGsh.fPixelCompensate[41] = 6.1;
            struGsh.fPixelCompensate[42] = 6.4;
            struGsh.fPixelCompensate[43] = 6.7;
            struGsh.fPixelCompensate[44] = 6.8;
            struGsh.fPixelCompensate[45] = 7.0;
            struGsh.fPixelCompensate[46] = 7.0;
            struGsh.fPixelCompensate[47] = 7.0;
            struGsh.fPixelCompensate[48] = 7.0;
            struGsh.fPixelCompensate[49] = 6.8;
            struGsh.fPixelCompensate[50] = 6.5;
            struGsh.fPixelCompensate[51] = 6.2;
            struGsh.fPixelCompensate[52] = 5.8;
            struGsh.fPixelCompensate[53] = 5.2;
            struGsh.fPixelCompensate[54] = 4.6;
            struGsh.fPixelCompensate[55] = 3.8;
            struGsh.fPixelCompensate[56] = 2.9;
            struGsh.fPixelCompensate[57] = 1.9;
            struGsh.fPixelCompensate[58] = 0.7;
            struGsh.fPixelCompensate[59] = 0;
        } else if (struCnfc.nSensorPixel == SENSOR_PIXELS_1024) {
            struGsh.fPixelCompensate[0]  = 0.0;
            struGsh.fPixelCompensate[1]  = -0.5;
            struGsh.fPixelCompensate[2]  = -1.0;
            struGsh.fPixelCompensate[3]  = -1.4;
            struGsh.fPixelCompensate[4]  = -1.8;
            struGsh.fPixelCompensate[5]  = -2.1;
            struGsh.fPixelCompensate[6]  = -2.4;
            struGsh.fPixelCompensate[7]  = -2.6;
            struGsh.fPixelCompensate[8]  = -2.8;
            struGsh.fPixelCompensate[9]  = -3.0;
            struGsh.fPixelCompensate[10] = -3.1;
            struGsh.fPixelCompensate[11] = -3.2;
            struGsh.fPixelCompensate[12] = -3.2;
            struGsh.fPixelCompensate[13] = -3.2;
            struGsh.fPixelCompensate[14] = -3.2;
            struGsh.fPixelCompensate[15] = -3.1;
            struGsh.fPixelCompensate[16] = -3.0;
            struGsh.fPixelCompensate[17] = -2.9;
            struGsh.fPixelCompensate[18] = -2.8;
            struGsh.fPixelCompensate[19] = -2.6;
            struGsh.fPixelCompensate[20] = -2.4;
            struGsh.fPixelCompensate[21] = -2.2;
            struGsh.fPixelCompensate[22] = -2.0;
            struGsh.fPixelCompensate[23] = -1.8;
            struGsh.fPixelCompensate[24] = -1.6;
            struGsh.fPixelCompensate[25] = -1.3;
            struGsh.fPixelCompensate[26] = -1.1;
            struGsh.fPixelCompensate[27] = -0.8;
            struGsh.fPixelCompensate[28] = -0.4;
            struGsh.fPixelCompensate[29] = -0.3;
            struGsh.fPixelCompensate[30] = -0.0;
            struGsh.fPixelCompensate[31] = 0.3;
            struGsh.fPixelCompensate[32] = 0.6;
            struGsh.fPixelCompensate[33] = 0.8;
            struGsh.fPixelCompensate[34] = 1.1;
            struGsh.fPixelCompensate[35] = 1.3;
            struGsh.fPixelCompensate[36] = 1.6;
            struGsh.fPixelCompensate[37] = 1.8;
            struGsh.fPixelCompensate[38] = 2.1;
            struGsh.fPixelCompensate[39] = 2.3;
            struGsh.fPixelCompensate[40] = 2.5;
            struGsh.fPixelCompensate[41] = 2.6;
            struGsh.fPixelCompensate[42] = 2.8;
            struGsh.fPixelCompensate[43] = 2.9;
            struGsh.fPixelCompensate[44] = 3.1;
            struGsh.fPixelCompensate[45] = 3.1;
            struGsh.fPixelCompensate[46] = 3.2;
            struGsh.fPixelCompensate[47] = 3.2;
            struGsh.fPixelCompensate[48] = 3.2;
            struGsh.fPixelCompensate[49] = 3.2;
            struGsh.fPixelCompensate[50] = 3.1;
            struGsh.fPixelCompensate[51] = 3.0;
            struGsh.fPixelCompensate[52] = 2.8;
            struGsh.fPixelCompensate[53] = 2.6;
            struGsh.fPixelCompensate[54] = 2.4;
            struGsh.fPixelCompensate[55] = 2.1;
            struGsh.fPixelCompensate[56] = 1.8;
            struGsh.fPixelCompensate[57] = 1.4;
            struGsh.fPixelCompensate[58] = 1.0;
            struGsh.fPixelCompensate[59] = 0.5;
        }
        break;
    case 40:
        struGsh.fPixelCompensate[0]  = 0.0;
        struGsh.fPixelCompensate[1]  = -0.7;
        struGsh.fPixelCompensate[2]  = -1.3;
        struGsh.fPixelCompensate[3]  = -1.7;
        struGsh.fPixelCompensate[4]  = -2.0;
        struGsh.fPixelCompensate[5]  = -2.2;
        struGsh.fPixelCompensate[6]  = -2.3;
        struGsh.fPixelCompensate[7]  = -2.4;
        struGsh.fPixelCompensate[8]  = -2.3;
        struGsh.fPixelCompensate[9]  = -2.3;
        struGsh.fPixelCompensate[10] = -2.2;
        struGsh.fPixelCompensate[11] = -2.1;
        struGsh.fPixelCompensate[12] = -2.0;
        struGsh.fPixelCompensate[13] = -1.9;
        struGsh.fPixelCompensate[14] = -1.7;
        struGsh.fPixelCompensate[15] = -1.5;
        struGsh.fPixelCompensate[16] = -1.3;
        struGsh.fPixelCompensate[17] = -1.0;
        struGsh.fPixelCompensate[18] = -0.7;
        struGsh.fPixelCompensate[19] = -0.3;
        struGsh.fPixelCompensate[20] = 0.1;
        struGsh.fPixelCompensate[21] = 0.5;
        struGsh.fPixelCompensate[22] = 0.9;
        struGsh.fPixelCompensate[23] = 1.4;
        struGsh.fPixelCompensate[24] = 1.8;
        struGsh.fPixelCompensate[25] = 2.2;
        struGsh.fPixelCompensate[26] = 2.6;
        struGsh.fPixelCompensate[27] = 3.0;
        struGsh.fPixelCompensate[28] = 3.4;
        struGsh.fPixelCompensate[29] = 3.6;
        struGsh.fPixelCompensate[30] = 3.8;
        struGsh.fPixelCompensate[31] = 3.8;
        struGsh.fPixelCompensate[32] = 3.7;
        struGsh.fPixelCompensate[33] = 3.6;
        struGsh.fPixelCompensate[34] = 3.3;
        struGsh.fPixelCompensate[35] = 3.0;
        struGsh.fPixelCompensate[36] = 2.6;
        struGsh.fPixelCompensate[37] = 2.0;
        struGsh.fPixelCompensate[38] = 2.4;
        struGsh.fPixelCompensate[39] = 0.7;
        break;
    case 50:
        struGsh.fPixelCompensate[0]  = 0.0;
        struGsh.fPixelCompensate[1]  = -0.8;
        struGsh.fPixelCompensate[2]  = -1.4;
        struGsh.fPixelCompensate[3]  = -2.0;
        struGsh.fPixelCompensate[4]  = -2.5;
        struGsh.fPixelCompensate[5]  = -2.9;
        struGsh.fPixelCompensate[6]  = -3.2;
        struGsh.fPixelCompensate[7]  = -3.5;
        struGsh.fPixelCompensate[8]  = -3.6;
        struGsh.fPixelCompensate[9]  = -3.8;
        struGsh.fPixelCompensate[10] = -3.8;
        struGsh.fPixelCompensate[11] = -3.8;
        struGsh.fPixelCompensate[12] = -3.7;
        struGsh.fPixelCompensate[13] = -3.6;
        struGsh.fPixelCompensate[14] = -3.5;
        struGsh.fPixelCompensate[15] = -3.2;
        struGsh.fPixelCompensate[16] = -3.0;
        struGsh.fPixelCompensate[17] = -2.7;
        struGsh.fPixelCompensate[18] = -2.4;
        struGsh.fPixelCompensate[19] = -2.1;
        struGsh.fPixelCompensate[20] = -1.7;
        struGsh.fPixelCompensate[21] = -1.4;
        struGsh.fPixelCompensate[22] = -1.0;
        struGsh.fPixelCompensate[23] = -0.6;
        struGsh.fPixelCompensate[24] = -0.2;
        struGsh.fPixelCompensate[25] = 0.2;
        struGsh.fPixelCompensate[26] = 0.6;
        struGsh.fPixelCompensate[27] = 1.0;
        struGsh.fPixelCompensate[28] = 1.4;
        struGsh.fPixelCompensate[29] = 1.7;
        struGsh.fPixelCompensate[30] = 2.1;
        struGsh.fPixelCompensate[31] = 2.4;
        struGsh.fPixelCompensate[32] = 2.7;
        struGsh.fPixelCompensate[33] = 3.0;
        struGsh.fPixelCompensate[34] = 3.2;
        struGsh.fPixelCompensate[35] = 3.4;
        struGsh.fPixelCompensate[36] = 3.6;
        struGsh.fPixelCompensate[37] = 3.7;
        struGsh.fPixelCompensate[38] = 3.8;
        struGsh.fPixelCompensate[39] = 3.8;
        struGsh.fPixelCompensate[40] = 3.8;
        struGsh.fPixelCompensate[41] = 3.6;
        struGsh.fPixelCompensate[42] = 3.4;
        struGsh.fPixelCompensate[43] = 3.2;
        struGsh.fPixelCompensate[44] = 2.9;
        struGsh.fPixelCompensate[45] = 2.5;
        struGsh.fPixelCompensate[46] = 2.0;
        struGsh.fPixelCompensate[47] = 1.4;
        struGsh.fPixelCompensate[48] = 0.7;
        struGsh.fPixelCompensate[49] = 0.0;
        break;
    default:
        break;
    }
}

/*!
 * \brief 重置剔除高级参数
 */
void GlobalFlow::resetTickAdvParams()
{
    switch (struCnfc.nSensorType) {
    case SENSOR_K:  // Keda
        struGsh.delayPerRow = (float)6/struCnfc.nSensorRowFre;
        struGsh.rowColumnRelation = 2;
        struGsh.maxRowNumber = 16;
        break;
    case SENSOR_T_2566:	// T2
        struGsh.delayPerRow = (float)63*115/(1000*struCnfc.nSensorRowFre);
        struGsh.rowColumnRelation = 2;
        struGsh.maxRowNumber = 16;
        break;
    case SENSOR_T_2564: // T3

            if (struCnfc.nSensorPixelLevel[0] == SENSOR_PIXELS_1024) {
                struGsh.maxRowNumber = 32;
                struGsh.rowColumnRelation = 1;
            } else if (struCnfc.nSensorPixelLevel[0] == SENSOR_PIXELS_2048) {
                struGsh.maxRowNumber = 32;
                struGsh.rowColumnRelation = 2;
            }
        

        if (336.0/(0.1*struCnfc.nSensorRowFre) > 100) {
            struGsh.delayPerRow = 336.0/(100*struCnfc.nSensorRowFre);
        } else {
            struGsh.delayPerRow = 0.1;
        }
        break;
    case SENSOR_D2: // D2
        struGsh.delayPerRow = 0.064;
        struGsh.rowColumnRelation = 1;
        struGsh.maxRowNumber = 32;
        break;
    case SENSOR_P5:
        struGsh.delayPerRow = 0;
        struGsh.rowColumnRelation = 1;
        struGsh.maxRowNumber = 32;
        break;
    default:
        struGsh.delayPerRow = 0.1;
        struGsh.rowColumnRelation = 2;
        struGsh.maxRowNumber = 16;
        break;
    }
}

/* 重置与组相关的参数*/
void GlobalFlow::resetGroupParams()
{
    initChannelPixelCompensate();       //! 获取镜头畸变补偿系数
    resetTickAdvParams();               //! 重置剔除高级参数
    getColorSelectGroupName();          //! 获取色选模式界面显示名称
    getColorSelectModeName();           //! 获取色选模式名称
    getIdentifyGroupName();             //! 获取识别组界面显示名称
    getTickGroupName();                 //! 获取剔除组界面显示名称
    getLampName();                      //! 获取灯控名称

    myMonoFlow.setStringByDirect();
    myMonoFlow.setHasInfByParams();
}

/* 初始化全局不需要保存的参数*/
void GlobalFlow::initGsh()
{
    int i, j, k;
    int nAddr;

    struGsh.nSerialCount1 = 0;
    struGsh.nSerialCount2 = 0;
    struGsh.nSerialCount3 = 0;
    struGsh.nCounter = 0;

    struGsh.isBigDataUpdated = false;
    struGsh.isQueryingCom3 = false;

    struGsh.nDevTypeAddrBias = 0;

    for (i = 0; i < MAX_LEVEL; i++) {
        for (j = 0; j < MAX_UNIT; j++) {
            nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            for (k = 0; k < 8; k++) {
                struGsh.struVer.sUnit[i][j][k] = 255;
            }
        }
    }
    for (i = 0; i < MAX_LEVEL; i++) {
        for (j = 0; j < 3; j++) {
            struGsh.struVer.sInterface[i][j] = 255;
        }
    }

    for (i = 0; i < MAX_CTRL; i++) {
        for (j = 0; j < 2; j++) {
            struGsh.struVer.sControl[i][j] = 255;
        }
    }

    for (i = 0; i < MAX_UNIT/2; i++) {
        struGsh.aiResult[i] = QString("");
    }

    //! 恒流源板版本查询
    for (i = 0; i < MAX_LIGHT_SOURCE; i++) {
        struGsh.struVer.sLightSrc[i][0] = 255;
        struGsh.struVer.sLightSrc[i][1] = 255;
    }
    struGsh.struVer.nUsbFirmware = 255;

    struGsh.bStatFeed = 0;
    for (i = 0; i < MAX_LEVEL; i++) {
        struGsh.bStatBelt[i] = 0;
    }
    struGsh.bStatEjectFpga = 1;
    struGsh.bFlagAutowipe = 0;
    struGsh.bFlagManuwipe = 0;
    struGsh.bFlagRowing = 1;
    struGsh.bFlagMachinetest = 0;
    //! 料位检测
    for (int i = 0; i < MAX_GROUP_TICK; i++) {
        struGsh.nAlarmLevelGroup[i] = 0;
    }
    //! 一键放料标志
    struGsh.bStatFeedCtrlEn = 1;
    struGsh.nAlarmLevel = ALARM_LEVEL_NULL;
    struGsh.nCommStat = 0;
    struGsh.bAlarmPressure = 0;
    struGsh.bAlarmEjector = 0;
    struGsh.bAlarmLamp = 0;
    struGsh.bAlarmBlower = 0;
    struGsh.bAlarmSlaveComm = 0;
    struGsh.bAlarmStatus = 0;
    struGsh.nAlarmPLCReq = 0;   //! PLC请求初始化关闭
    struGsh.nAlarmPLCReqFlag = 0;   //! PLC请求初始化为未跳变
    struGsh.nFeederControlLevel = 0;
    struGsh.nLevel = 0;
    struGsh.nTotalAlarmPreTimes = 0;  //! 本次开机气压报警次数

    struGsh.bAlarmCloseFeed = false;   //! 标志是否因报警关闭下料
    struGsh.bAlarmCloseEject = false;  //! 标志是否因气压不足关闭喷阀

    struGsh.nUnit = 0;
    struGsh.nMatGroup = 0;

    memset(struGsh.sRgbRow, 0, SENSOR_MAX_PIXELS*3+16);
    memset(struGsh.sRowRed, 0, SENSOR_MAX_PIXELS);
    memset(struGsh.sRowGreen, 0, SENSOR_MAX_PIXELS);
    memset(struGsh.sRowBlue, 0, SENSOR_MAX_PIXELS);

    struGsh.nSensCoefficient = CF_SENS_COEFICENT;

    //! 接口板波特率
    for (int i = 0; i < MAX_INT; i++) {
        struGsh.nIntBaud[i] = 0;
    }

    for (i = 0; i < IMAGE_MAX; i++) {
        struGsh.nIntelSampColor[i] = 1;
        struGsh.nIntelSampMode[i] = 0;
        struGsh.nIntelSampSens[i] = 255;
    }
    struGsh.nIntelSampType = 1;
    struGsh.nIntelSampBurst = 0;
    struGsh.nIntelSampAi = 0;
    struGsh.nBacklightStat = 1;
    setTsBackLight(1);
    struGsh.nBacklightCounter = 0;

    struGsh.nPixelBegin = 40;
    struGsh.nPixelEnd   = 2000;
    struGsh.isPeaMachineBeanMat = true;
    struGsh.enableJudgeBeanMachine = true;
    struGsh.bIsAssist = false;
    memset(struGsh.pEjectTimes, 0, sizeof(struGsh.pEjectTimes));

    //! 颜色算法识别结果
    for (int i = 0; i < MAX_UNIT; i++) {
        struGsh.struResult[i].nPixelSum = 0;
        for (int j = 0; j < IDE_RESULT_TYPE; j++) {
            struGsh.struResult[i].nBadNum[j] = 0;
            struGsh.struResult[i].nEn[j] = true;
        }
    }

    //! 统计信息
    struGsh.bStatisticFinished = 0;
    struGsh.nWipeTimes = 0;
    struGsh.bAlarmChanged = false;
    struGsh.nAlarmStatus = 0;

    //! 本次加工时间
    struGsh.nFeedCounter = 0;

    //! TD机型根据每层前后视状态初始化对应识别组位置
    int nIndex = 0;
    for (int i = 0; i < MAX_LEVEL*2; i++) {
        struGsh.idIndex[i] = 0;
    }
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        if (struCnfg.struLevelInfo[i].nViewTotal == 1) {
            struGsh.idIndex[nIndex++] = 2*i;
        } else if (struCnfg.struLevelInfo[i].nViewTotal == 2) {
            struGsh.idIndex[nIndex++] = 2*i;
            struGsh.idIndex[nIndex++] = 2*i + 1;
        }
    }

    //! 温度检测
    for (int i = 0; i < MAX_TEMPERATURE_SENSOR; i++) {
        struGsh.nSensorTemperature[i] = 0;
    }
    struGsh.nTemperatureMax = 0;
    struGsh.nTemperatureMin = 0;

    struGsh.nIntelSampType = 1;

    for(int i=0; i<MAX_UNIT/2; i++){
       struGsh.nThroughtPut[i] = 0;
    }

    //! 重置与组相关的参数
    resetGroupParams();

    //! 获取从设备列表
    myFlow.getSlaveList();

    //! 更新灯控数量
    updateLampTotal();

    //! 更新单个恒流源板的灯控数量
    updateLightSrcLampNum();

    getDefaultPara();
}

void GlobalFlow::getDefaultPara(){
    //ai板子数量初始化
    int idTotal = 0;
    switch(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal) {
    case 1:     // 单视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
        break;
    case 2:     // 双视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
    }
    struGsh.aiDeviceNum = idTotal/2;
    qDebug() << "aiDeviceNum"<<struGsh.aiDeviceNum;

    struGsh.ftpIpAddressList.clear();
    struGsh.addressList.clear();

    struGsh.ftpIpAddressList.append(AI_IP_ADDRESS);
    QString oldIpAddress = AI_IP_ADDRESS;

    for(int i=1; i<struGsh.aiDeviceNum; i++){
        QString newIpAddress = oldIpAddress;
        newIpAddress = incrementLastOctet(oldIpAddress);
        oldIpAddress = newIpAddress;
        struGsh.ftpIpAddressList.append(newIpAddress);
    }
    //json文件解析
    QFile file(DEFAULT_PARA_INIT);
    file.open(QFile::ReadOnly);
    QString json=  QString::fromUtf8(file.readAll());
    file.close();
    bool ok;
    QVariantMap resultMap = QJson::parse(json.toUtf8(), ok).toMap();
    if(!ok){
         qDebug() << "json open failed, using default IpAddress";
    }else{
        for(int i=0; i<struGsh.aiDeviceNum; i++){
            QString key = QString("ftpIp%1").arg(i);
            if(resultMap.keys().contains(key)){
                struGsh.ftpIpAddress = resultMap[key].toString();
                struGsh.ftpIpAddressList.replace(i,struGsh.ftpIpAddress);
            }
        }
    }
    qDebug() << "ftpIpAddressList"<<struGsh.ftpIpAddressList;
    struGsh.address = QHostAddress(struGsh.ftpIpAddressList.at(0));
    struGsh.addressList.append(struGsh.address);
    for(int i=1; i<struGsh.ftpIpAddressList.size(); i++){
        struGsh.addressList.append(QHostAddress(struGsh.ftpIpAddressList.at(i)));
    }
}

// IP地址最后一位+1
QString GlobalFlow::incrementLastOctet(const QString & ip) {
    QStringList parts = ip.split(".");
    int lastOctet = parts[3].toInt();

    // 处理最后一位+1的逻辑
    if (lastOctet < 255) {
        lastOctet++;
    } else {
        // 如果最后一位是255，则保持不变或根据需求处理
        qDebug() << "警告: IP地址最后一位已经是255，无法再增加。" << endl;
        return ip;
    }

    // 构建新的IP地址
    return parts[0] + "." + parts[1] + "." + parts[2] + "." +  QString::number(lastOctet);
}


