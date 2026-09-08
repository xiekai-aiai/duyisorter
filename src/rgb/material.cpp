/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        material.cpp
 * \brief       彩色机型色选参数整理并下发文件
 * \date        2015.01.14
 */
#include "globalparams.h"
#include "globalflow.h"


/***************************************************************************************************
    名称：      发送给FPGA的物料参数的组包
    功能：      根据物料算法，物料识别参数发送包组包
    nCmd:      命令编码
    nLevelId:  识别组所在的层编号
    nGroupId： 识别组的组号
    nIndex:    使用同一个结构体的几个并列算法的数组标号，如智能模式和灰度杂质、色差杂质
***************************************************************************************************/
void GlobalFlow::materialParamsNewPacketOrganize(int nCmd, int nLevelId, int nGroupId, int nIndex)
{
    int tmpCmd;
    int k;
    int nUnitAddr;
    int nUnitAddrLocInLevelArray = 0;//此地址的相机在全局数组参数中的下标
    int nMin = 0, nMax = 0, nMin2 = 0, nMax2 = 0, nMinp = 0;
    double tmp1=0.0,tmp2=0.0;
    int nSens = 0;
    int nMain = 0, nVice = 0;
    int nRow = 0, nCol = 0, nPer = 0;
    int nSignDfl, nConfDfl, nSignDfl_bak, nSignCons, nSignQuo1, nSignQuo2, nSignQuo3;
    int nTinySens, nConsSens;
    unsigned char value[4];
    int sensorPixelsQuo;
    int lengthLimit;
    int nConsAdj;
    if(struCnfc.nSensorType == SENSOR_T_2564 && struCnfc.nSensorPixelLevel[nLevelId] == SENSOR_PIXELS_1024) {
        sensorPixelsQuo = 2;
    } else {
        sensorPixelsQuo = 1;
    }

    if(struCnfc.nSensorType == SENSOR_T_2564 && struCnfc.nSensorPixelLevel[nLevelId] == SENSOR_PIXELS_2048) {
        sensorPixelsQuo = 2;
	}

    for (k = 0; k < struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitCount; k++) {
       nUnitAddr =  getIdentifyGroupAddr(nLevelId, nGroupId, k);
       for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal; i++) {
           if (struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitId[k]
                   == struCnfg.struLevelInfo[ONE_LEVEL].nUnitId[i]) {
               nUnitAddrLocInLevelArray = i;
               break;
           } else {
               nUnitAddrLocInLevelArray = -1;//找不到则打印错误信息
           }
       }
       if (nUnitAddrLocInLevelArray == -1) {
           nUnitAddrLocInLevelArray = 0;
       }

       switch (nCmd) {
       case CMD_UNIT_CROSS_PARAMS:
           MySerial.com1Write(CMD_UNIT_NEW_CROSS_PARAMS_1, UNIT, nLevelId, nUnitAddr,
                               struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nColor, 0, 0,
                               255 - (struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nSens*0.255+0.5)/1, 255, 3);
           MySerial.com1Write(CMD_UNIT_NEW_CROSS_PARAMS_2, UNIT, nLevelId, nUnitAddr, 0,
                                struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nRow-1,
                                struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nColumn-1,
                                ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nPercent-1)/256),
                               (struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nPercent-1)%256, 3);
           break;
       case CMD_UNIT_SCALE_PARAMS:
           MySerial.com1Write(CMD_UNIT_NEW_SCALE_PARAMS, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nMode,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nValue)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nValue)%256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nValueLimit)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nValueLimit)%256, 3);
           break;
       case CMD_UNIT_SCALE_B_PARAMS:
           MySerial.com1Write(CMD_UNIT_SCALE_B_PARAMS, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nMode,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValue)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValue)%256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValueLimit)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValueLimit)%256, 3);
           break;
       case CMD_UNIT_CANDY:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struCandy.nCycles,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struCandy.nRation2,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struCandy.nRation1,3);
           break;
       case CMD_UNIT_PISTACHIO:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, struCnfp.nArithmeticEnable[ARITH_PISTACHIO], struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nGrayThreshold_3, struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nAreaMin,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nGrayThreshold_2,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nGrayThreshold_1,3);
           break;
       case CMD_UNIT_SHAPE:
            /* 选圆/选长 */
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nMode == 0) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaMax*0.255+0.5)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaMin*0.255+0.5)/1;
           }

           /* 选长/选短 */
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthMode == 0) {
               nMin2 = 0;
               nMax2 = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthMin*sensorPixelsQuo*0.255+0.5)/1;
           } else {
               nMin2 = 255*sensorPixelsQuo+1-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthMax*sensorPixelsQuo*0.255+0.5)/1;
               nMax2 = 255*sensorPixelsQuo;
           }
           lengthLimit = struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthLimit;

           MySerial.com1Write(CMD_UNIT_SHAPE, UNIT, nLevelId, nUnitAddr, struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nMode, nMin, nMax,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaLimit/256,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaLimit%256,
                              3);
           MySerial.com1Write(CMD_UNIT_SHAPE_LENGTH, UNIT, nLevelId, nUnitAddr, lengthLimit/256, (nMin2/256)*16|(nMax2/256), lengthLimit%256, nMin2%256, nMax2%256, 3);
           break;
       case CMD_UNIT_SHAPE_POLE_A:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nArea;
           nPer  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPercent < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPercent;
           nMin  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMin < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMin;
           nMax  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMax < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMax;
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_A, UNIT, nLevelId, nUnitAddr, 0, nSens, nPer, nMin, nMax, 3);
           break;
       case CMD_UNIT_SHAPE_POLE_A_RESERVED:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nMode2 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMax2*0.255+0.5)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMin2*0.255+0.5)/1;
           }
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_A_RESERVED, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nColor2,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMin1/256,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMin1%256,
                              nMin, nMax, 3);
           break;
       case CMD_UNIT_SHAPE_POLE_B:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleArea;
           nPer  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPolePercent < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPolePercent;
           nMin  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMin < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMin;
           nMax  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMax < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMax;
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_B, UNIT, nLevelId, nUnitAddr, 0, nSens, nMin, nMax, nPer, 3);
           break;
       case CMD_UNIT_SHAPE_SLICE:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struSlice.nArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struSlice.nArea;
           MySerial.com1Write(CMD_UNIT_SHAPE_SLICE, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
           break;
       case CMD_UNIT_SHAPE_LEAF:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nThreshold < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nThreshold;
           nRow  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nArea;
           nPer  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nPercent < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nPercent;
           nMin  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMin < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMin;
           nMax  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMax < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMax;
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_A, UNIT, nLevelId, nUnitAddr, nSens, nRow, nPer, nMin, nMax, 3);
           MySerial.com1Write(CMD_UNIT_SHAPE_LEAF,   UNIT, nLevelId, nUnitAddr, nSens, nRow, nPer, nMin, nMax, 3);
           break;
       case CMD_UNIT_EARTHPEA_PARAMS1_MISC:
           MySerial.com1Write(CMD_UNIT_NEW_EARTHPEA_PARAMS1, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nRow-1,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nColumn-1,
                              255-struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nArea*2.55,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nPercentLimit-1)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nPercentLimit-1)%256, 3);
           break;
       case CMD_UNIT_EARTHPEA_PARAMS2_MISC:
           MySerial.com1Write(CMD_UNIT_NEW_EARTHPEA_PARAMS2, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nRow-1,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nColumn-1,
                              255-struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nArea*2.55,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nPercentLimit-1)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nPercentLimit-1)%256, 3);
           break;
       case CMD_UNIT_GREY_A:
       case CMD_UNIT_GREY_B:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMax*0.255)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMin*0.255+0.5)/1;
           }

           /* 加上灵敏度偏置的影响 */
           if(nCmd == CMD_UNIT_GREY_A) {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_GREY_A]*0.255+0.5);
           } else {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_GREY_B]*0.255+0.5);
           }
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = nMin-nSens;
               nMin = (nMin > 0) ? nMin : 0;
               nMin = (nMin < 255) ? nMin : 255;
           } else {
               nMax = nMax+nSens;
               nMax = (nMax > 0) ? nMax : 0;
               nMax = (nMax < 255) ? nMax : 255;
           }

           if (nCmd == CMD_UNIT_GREY_A) {
               MySerial.com1Write(CMD_UNIT_NEW_GREY_A_1, UNIT, nLevelId, nUnitAddr,
                                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColor,
                                  0, 0,  nMin, nMax, 3);
               MySerial.com1Write(CMD_UNIT_NEW_GREY_A_2, UNIT, nLevelId, nUnitAddr, 0,
                                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nRow-1,
                                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColumn-1,
                                  (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)/256,
                                  (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)%256, 3);
           } else {
               MySerial.com1Write(CMD_UNIT_NEW_GREY_B_1, UNIT, nLevelId, nUnitAddr,
                                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColor,
                                  0, 0,  nMin, nMax, 3);
               MySerial.com1Write(CMD_UNIT_NEW_GREY_B_2, UNIT, nLevelId, nUnitAddr, 0,
                                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nRow-1,
                                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColumn-1,
                                  (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)/256,
                                  (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)%256, 3);
           }
           break;
       case CMD_UNIT_BURNT_ENABLE:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nBurntReservedFlag,3);
           break;
       case CMD_UNIT_DISCOLOR_A_1:
       case CMD_UNIT_DISCOLOR_B_1:
           if(nCmd == CMD_UNIT_DISCOLOR_A_1) {
               tmpCmd = CMD_UNIT_NEW_DISCOLOR_A_2;
           }
           else {
               tmpCmd = CMD_UNIT_NEW_DISCOLOR_B_2;
           }

           MySerial.com1Write(tmpCmd, UNIT, nLevelId, nUnitAddr, 0,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nRow-1,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColumn-1,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)/256,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)%256, 3);

           break;
       case CMD_UNIT_DISCOLOR_A_2:
       case CMD_UNIT_DISCOLOR_B_2:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMax*0.0255+0.5);
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMin*0.0255+0.5);
           }

           /*加上灵敏度偏置的影响20141204*/
           if(nCmd == CMD_UNIT_DISCOLOR_A_2) {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_DISCOLOR_A]*0.0255+0.5);
           } else {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_DISCOLOR_B]*0.0255+0.5);
           }
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = nMin-nSens;
               nMin = (nMin > 0) ? nMin : 0;
               nMin = (nMin < 255) ? nMin : 255;
           }
           else {
               nMax = nMax+nSens;
               nMax = (nMax > 0) ? nMax : 0;
               nMax = (nMax < 255) ? nMax : 255;
           }

           switch(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nDiscolor) {
           case 0:
               nMain = 1;
               nVice = 2;
               break;
           case 1:
               nMain = 1;
               nVice = 4;
               break;
           case 2:
               nMain = 2;
               nVice = 4;
               break;
           }

           if(nCmd == CMD_UNIT_DISCOLOR_A_2) {
               tmpCmd = CMD_UNIT_NEW_DISCOLOR_A_1;
           }
           else {
               tmpCmd = CMD_UNIT_NEW_DISCOLOR_B_1;
           }
           MySerial.com1Write(tmpCmd, UNIT, nLevelId, nUnitAddr, 0, nMin, nMax, (nMain << 4)|nVice, struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nLightLimit, 3);
           break;
       case CMD_UNIT_PEANUT_A:
           MySerial.com1Write(CMD_UNIT_NEW_GREY_A_1, UNIT, nLevelId, nUnitAddr,
                              1,//red
                              0,
							  0, 
							  0,
							  struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nSensMin*0.255+0.5,
							  3);
           MySerial.com1Write(CMD_UNIT_NEW_GREY_A_2, UNIT, nLevelId, nUnitAddr, 
						  	  0,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nRow-1,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nColumn-1,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nPercent-1)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nPercent-1)%256, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_1, UNIT, nLevelId, nUnitAddr, 0, ((1<<4)|2), 1, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nSensReserved*0.255+0.5)/1, 255, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_2, UNIT, nLevelId, nUnitAddr, 0, 1, 1, 0, 255, 3);
           break;   //A
       case CMD_UNIT_PEANUT_B:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nMode == 0) {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nSensMin*0.255+0.5)/1;
           }
           else {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nSensMax*0.255)/1;
               nMax = 255;
           }
           MySerial.com1Write(CMD_UNIT_NEW_GREY_B_1, UNIT, nLevelId, nUnitAddr,
                	          4,//blue
            	              0,
		 					  0,
		 					  nMin,
		 				 	  nMax, 3);
           MySerial.com1Write(CMD_UNIT_NEW_GREY_B_2, UNIT, nLevelId, nUnitAddr, 0,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nRow-1,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nColumn-1,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nPercent-1)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nPercent-1)%256, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_1, UNIT, nLevelId, nUnitAddr, 0, (1<<4|2), 2, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nSensReserved*0.255+0.5)/1, 255, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_2, UNIT, nLevelId, nUnitAddr, 0, 1, 2, 0, 255, 3);
           break;   //B
       case CMD_UNIT_PEANUT_C:
           MySerial.com1Write(CMD_UNIT_NEW_DISCOLOR_A_1, UNIT, nLevelId, nUnitAddr,
						   0,
						   0,
						   struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nSensMin*0.255+0.5,
						   (1 << 4) | 2, 
						   50, 3);
           MySerial.com1Write(CMD_UNIT_NEW_DISCOLOR_A_2, UNIT, nLevelId, nUnitAddr, 0,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nRow-1,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nColumn-1,
                            (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nPercent-1)/256,
                            (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nPercent-1)%256, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_1, UNIT, nLevelId, nUnitAddr, 0, (1<<4|2), 3, 0, 255, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_2, UNIT, nLevelId, nUnitAddr, 0, 4, 3, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nSensReserved*0.255+0.5)/1, 255, 3);
           break;   //C

       case CMD_UNIT_PEANUT_D:
           MySerial.com1Write(CMD_UNIT_NEW_DISCOLOR_B_1, UNIT, nLevelId, nUnitAddr,
						   0,
						   0,
						   struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nSensMin*0.255+0.5,
						   (2 << 4)| 4, 
						   50, 3);
           MySerial.com1Write(CMD_UNIT_NEW_DISCOLOR_B_2, UNIT, nLevelId, nUnitAddr, 0,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nRow-1,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nColumn-1,
                            (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nPercent-1)/256,
                            (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nPercent-1)%256, 3);
           break;   //D

       case CMD_UNIT_RESERVED_1:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nMode1 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMax1*0.255)/1;
               nMax = 255;
           }
           else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMin1*0.255+0.5)/1;
           }
           switch(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nDiscolor1) {
           case 0:
               nMain = 1;
               nVice = 2;
               break;
           case 1:
               nMain = 1;
               nVice = 4;
               break;
           case 2:
               nMain = 2;
               nVice = 4;
               break;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, (nMain<<4|nVice), nIndex+1, nMin, nMax, 3);
            break;
       case CMD_UNIT_RESERVED_2:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nMode2 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMax2*0.255)/1;
               nMax = 255;
           }
           else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMin2*0.255+0.5)/1;
           }

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nColor2, nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_NEW_INTEL_RATIO_SENS_1:
       case CMD_UNIT_NEW_INTEL_RATIO_SENS_2:
           nMin = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSensRatio*0.255;
           nMax = 255;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nIntelType, 0, 0,nMin, nMax, 3);
           break;
       case CMD_UNIT_INTEL_A_1:
       case CMD_UNIT_INTEL_B_1:
       case CMD_UNIT_INTEL_C_1:
       case CMD_UNIT_INTEL_D_1:
           if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           nSignQuo1 = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[0];
           nSignQuo2 = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[0];
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0,
                              (nSignQuo1<<7)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[1]/256),
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[1]%256,
                             (nSignQuo2<<7)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[1]/256),
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[1]%256,3);
           break;
       case CMD_UNIT_INTEL_A_2:
       case CMD_UNIT_INTEL_B_2:
       case CMD_UNIT_INTEL_C_2:
       case CMD_UNIT_INTEL_D_2:
           if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           nSignQuo3 = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[0];
           nSignCons = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[0];

           if(nSignCons == 0) { // 常数项为正号
                   if(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens > 100) {
                        nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens-100)/100.0);
                   }
                   else {
                        nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1+(100-struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens)/100.0);
                   }
           }
           else {                                                                               // 常数项为负号
                   if(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens > 100) {
                        nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1+(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens-100)/100.0);
                   }
                   else {
                        nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1-(100-struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens)/100.0);
                   }
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0,
                       (nSignQuo3<<7)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[1]/256),
                       struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[1]%256,
                       (nSignCons<<7)|(nConsSens/256), nConsSens%256, 3);
           break;
       case CMD_UNIT_INTEL_A_3:
       case CMD_UNIT_INTEL_B_3:
       case CMD_UNIT_INTEL_C_3:
       case CMD_UNIT_INTEL_D_3:
           nRow = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRow;
           nCol = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nColumn;

           nTinySens = 0;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                      struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRow-1,
                      ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nColumn-1)<<2)|((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercent-1)/256),
                      ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercent-1))%256, nTinySens,
                      (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nEnable), 3);
               break;
       case CMD_UNIT_INTEL_RESERVED_1:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nMode1 == 1) { 	// 0: grey A , 1: grey B, 2: discolor A, 3: discolor B
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMax1*0.255+0.5)/1;
               nMax = 255;
           }
           else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMin1*0.255+0.5)/1;
           }

           switch(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nDiscolor1) {
           case 0:
               nMain = 1;
               nVice = 2;
               break;
           case 1:
               nMain = 1;
               nVice = 4;
               break;
           case 2:
               nMain = 2;
               nVice = 4;
               break;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, nMain<<4|nVice, nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_INTEL_RESERVED_2:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nMode2 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMax2*0.255+0.5)/1;
               nMax = 255;
           }
           else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMin2*0.255+0.5)/1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nColor2, nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_INTEL_DFL_SIZE:    
            if (struCnfp.nIntelMode == 1) {
                if (myFlow.getProductLineNo() == 0) {
                    if (struCnfp.nArithmeticEnable[ARITH_INTEL_A] == 1) {
                            MySerial.com1Write(CMD_UNIT_NEW_INTEL_DFL_SIZE, UNIT, nLevelId, nUnitAddr,
                                          struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nRow-1,
                                          struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nColumn-1,
                                          (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1)/256,
                                          ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1))%256,
                                          struCnfp.nColorSpace, 3);
                    }
                    if (struCnfp.nArithmeticEnable[ARITH_INTEL_B] == 1){
                        MySerial.com1Write(CMD_UNIT_NEW_INTEL_DFL_SIZE_B, UNIT, nLevelId, nUnitAddr,
                                      struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nRow-1,
                                      struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nColumn-1,
                                      (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nPercent-1)/256,
                                      ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nPercent-1))%256,
                                      struCnfp.nColorSpace, 3);
                    }
                } else {
                    MySerial.com1Write(CMD_UNIT_NEW_INTEL_DFL_SIZE, UNIT, nLevelId, nUnitAddr,
                                      struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nRow-1,
                                      struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nColumn-1,
                                      (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1)/256,
                                      ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1))%256,
                                      struCnfp.nColorSpace, 3);
                }
            }
            break;
       case CMD_UNIT_NEW_INTEL_DFL_SIZE_GARLIC_2:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                             nIndex,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nRow-1,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nColumn-1,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1)/256,
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1))%256,3);
           break;
       case CMD_UNIT_NEW_INTEL_DFL_SIZE_GARLIC_1:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                             nIndex,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nRowRes-1,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nColumnRes-1,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercentRes-1)/256,
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercentRes-1))%256,3);
           break;
       case CMD_UNIT_INTEL_DFL_CONST:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[0];

           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl == 1) {//0正选
               nSignDfl = (nSignDfl == 1) ? 0:1;//0反选
           }

           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens;
           nSens += struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_INTEL_A+nIndex]/10;

           nSens = (nSens > 0) ? nSens : 0;
           nSens = (nSens < 200) ? nSens : 200;

           nSignDfl_bak = nSignDfl;
           if (abs(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]) < 200000) {
               nConsAdj = 200000;
               if(nSignDfl == 1) {
                   /* 灵敏度越大，常数项绝对值越大，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0;
                   } else {   // 灵敏度越小，常数项绝对值越小，选的越小
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0;
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 0;
                           nConfDfl = abs(nConfDfl);
                       }
                   }
               } else {
                   /* 灵敏度越大，常数项越小，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0;
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 1;
                           nConfDfl = abs(nConfDfl);
                       }
                   } else {   // 灵敏度越小，常数项越大，选的越少
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0;
                   }
               }
           }
           else {
               if(nSignDfl == 1) {
                   /* 灵敏度越大，常数项绝对值越大，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1+struCnfg.nMaterPer3/10.0*(nSens-100)/100.0);
                   } else {   // 灵敏度越小，常数项绝对值越小，选的越小
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1-struCnfg.nMaterPer3/10.0*(100-nSens)/100.0);
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 0;
                           nConfDfl = abs(nConfDfl);
                       }
                   }
               } else {
                   /* 灵敏度越大，常数项越小，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1-struCnfg.nMaterPer3/10.0*(nSens-100)/100.0);
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 1;
                           nConfDfl = abs(nConfDfl);
                       }
                   } else {   // 灵敏度越小，常数项越大，选的越少
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1+struCnfg.nMaterPer3/10.0*(100-nSens)/100.0);
                   }
               }
           }

           printf("nSigDef:%d\n",nSignDfl);
           printf("nSigDef_bak:%d\n",nSignDfl_bak);
           printf("nConsDfl:%d\n",nConfDfl);
           value[0] = (nConfDfl&0x000000FF);
           value[1] = (nConfDfl&0x0000FF00)>>8;
           value[2] = (nConfDfl&0x00FF0000)>>16;
           value[3] = (nConfDfl&0x7F000000)>>24;

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl_bak<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RED:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x7F000000)>>24;

           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_GREEN:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_BLUE:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RG:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RB:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x7F000000)>>24;

           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_GB:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RR:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_GG:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_BB:
            if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
            }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[0];;
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case ARITH_WATERMELON:
           MySerial.com1Write(CMD_UNIT_MELON_WHITE_SENS, UNIT, nLevelId, nUnitAddr, //39
						   0,
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nWhitePropMin*0.255+0.5)/1,
						   255,
						   0,
						   (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nWhiteSens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_EDGE_RED, UNIT, nLevelId, nUnitAddr,//3a 
						   0,
						   0,
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeRedRatio*0.255+0.5)/1,
						   0,
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeRedSens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_EDGE_DAMAGED, UNIT, nLevelId, nUnitAddr,//3b
						   0,
						   0,
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeDamagedRatio*0.255+0.5)/1,
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeDamagedSens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_RUGGED_1, UNIT, nLevelId, nUnitAddr,//3c
						   0, 
						   0,
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged1Save*0.255+0.5)/1,
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged1Sens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_RUGGED_2, UNIT, nLevelId, nUnitAddr,//3d 
						   0, 
						   0, 
						   (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged2Save*0.255+0.5)/1,
						   0, 
						   (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged2Sens*0.255+0.5)/1, 3);
           break;
        /*玉米算法*/
        case CMD_UNIT_MAIZE_PARAMS_1:
           /*加上灵敏度偏置的影响20150118*/
           tmp1 = (int)(struCnfp.nSensBiasMaize[nUnitAddrLocInLevelArray][1]*0.255+0.25);
           tmp2  = (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nSensMin2*0.255+0.25);
           nMin  = (int)(tmp2+tmp1);
           nMin  = (nMin > 0) ? nMin : 0;
           nMin  = (nMin < 255) ? nMin : 255;

           tmp1 = (struCnfp.nSensBiasMaize[nUnitAddrLocInLevelArray][2]*0.255+0.25);
           tmp2  = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nSensProport*0.255+0.25);
           nMinp  = (int)(tmp2+tmp1);
           nMinp  = (nMinp > 0) ? nMinp : 0;
           nMinp  = (nMinp < 255) ? nMinp : 255;

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              0,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColor1,
                              nMinp,
                              0,
                              nMin,
                              3);
           break;
       case CMD_UNIT_MAIZE_PARAMS_2:
           /*加上灵敏度偏置的影响20141204*/
           tmp1 = (struCnfp.nSensBiasMaize[nUnitAddrLocInLevelArray][0]*0.255+0.25);
           tmp2  = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nSensMin1*0.255+0.25);
           nMin  = (int)(tmp2+tmp1);
           nMin  = (nMin > 0) ? nMin : 0;
           nMin  = (nMin < 255) ? nMin : 255;
           struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nRow = 7;
           struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColumn = 14;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColor2 ,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nRow-1),
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColumn-1),
                              0,
                              nMin,
                              3);
           break;
       case CMD_UNIT_MAIZE_PARAMS_3:
           struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nPercentTotal = 80;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struReserved.nRow-1),     //还原圈数
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struReserved.nThreshold_r, //r阈值
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struReserved.nThreshold_s, //s阈值
                              ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nPercentTotal-1) >> 8) & 0xff,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nPercentTotal-1) & 0xff,
                              3);
           break;
       default:
           break;
       }
    }
}

/***************************************************************************************************
    名称：      发送给FPGA的物料参数的组包
    功能：      根据物料算法，物料识别参数发送包组包
    nCmd:      命令编码
    nLevelId:  识别组所在的层编号
    nGroupId： 识别组的组号
    nIndex:    使用同一个结构体的几个并列算法的数组标号，如智能模式和灰度杂质、色差杂质
***************************************************************************************************/
void GlobalFlow::materialParamsPacketOrganize(int nCmd, int nLevelId, int nGroupId, int nIndex)
{
    int k;
    int nUnitAddr;
    int nUnitAddrLocInLevelArray = 0;//此地址的相机在全局数组参数中的下标
    double tmp1=0.0,tmp2=0.0;
    int nMin = 0, nMax = 0, nMin2 = 0, nMax2 = 0, nMinp = 0;
    int nMain = 0, nVice = 0;
    int nRow = 0, nCol = 0, nPer = 0;
    int nSignDfl, nConfDfl, nSignDfl_bak;
    int nTinySens, nConsSens;
    unsigned char value[4];
    int scalueValue, scaleLimit;
    int lengthLimit;
    int tmpConvert = 0;
    int nSens, nDarkSens;
    int nConf;
    int nConsAdj;

    for (k = 0; k < struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitCount; k++) {
       nUnitAddr =  getIdentifyGroupAddr(nLevelId, nGroupId, k);
       for (int i = 0; i < struCnfg.struLevelInfo[nLevelId].nUnitLevelTotal; i++) {
           if (struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitId[k]
                   == struCnfg.struLevelInfo[nLevelId].nUnitId[i]) {
               nUnitAddrLocInLevelArray = i;
               break;
           } else {
               nUnitAddrLocInLevelArray = -1;//找不到则打印错误信息
           }
       }
       if (nUnitAddrLocInLevelArray == -1) {
           nUnitAddrLocInLevelArray = 0;
       }

       switch (nCmd) {
       case CMD_UNIT_CROSS_PARAMS:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                               (struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nColor << 4)|((struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nRow-1)),
                               ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nColumn-1) << 2)|((struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nPercent-1)/256),
                               (struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nPercent-1)%256,
                               255 - (struCnfp.struGroupIdentify[nLevelId][nGroupId].struCross.nSens*0.255+0.5)/1, 255, 3);

           break;
       case CMD_UNIT_SCALE_PARAMS:
           scalueValue = struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nValue;
           scaleLimit = struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nValueLimit;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, (scalueValue/256)/16, scaleLimit/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[0].nMode << 4)|((scalueValue/256)%16),
                               scalueValue%256,
                               scaleLimit%256, 3);
           break;
       case CMD_UNIT_SCALE_B_PARAMS:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nMode,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValue)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValue)%256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValueLimit)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struScale[1].nValueLimit)%256, 3);
           break;
       case CMD_UNIT_CANDY:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struCandy.nCycles,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struCandy.nRation2,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struCandy.nRation1,3);
           break;
       case CMD_UNIT_PISTACHIO:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, struCnfp.nArithmeticEnable[ARITH_PISTACHIO], struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nGrayThreshold_3, struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nAreaMin,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nGrayThreshold_2,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPistachio.nGrayThreshold_1,3);
           break;
       case CMD_UNIT_SHAPE:
            /* 选圆/选长 */
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nMode == 0) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaMax*0.255+0.5)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaMin*0.255+0.5)/1;
           }

           /* 选长/选短 */
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthMode == 0) {
               nMin2 = 0;
               nMax2 = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthMin*0.255+0.5)/1;
           } else {
               nMin2 = 255+1-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthMax*0.255+0.5)/1;
               nMax2 = 255;
           }
           lengthLimit = struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nLengthLimit;

           /* RS机型无形状识别矩形度计算 */
           MySerial.com1Write(CMD_UNIT_SHAPE_LENGTH, UNIT, nLevelId, nUnitAddr, lengthLimit/256,
                              (nMin2/256)*16|(nMax2/256), lengthLimit%256, nMin2%256, nMax2%256, 3);
           MySerial.com1Write(CMD_UNIT_SHAPE, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nMode, nMin, nMax,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaLimit/256,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struShape.nAreaLimit%256,
                              3);
           break;
       case CMD_UNIT_SHAPE_POLE_A:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nArea;
           nPer  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPercent < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPercent;
           nMin  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMin < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMin;
           nMax  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMax < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nWidthMax;
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_A, UNIT, nLevelId, nUnitAddr, 0, nSens, nPer, nMin, nMax, 3);
           break;
       case CMD_UNIT_SHAPE_POLE_A_RESERVED:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nMode2 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMax2*0.255+0.5)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMin2*0.255+0.5)/1;
           }
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_A_RESERVED, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nColor2,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMin1/256,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.struReserved.nSensMin1%256,
                              nMin, nMax, 3);
           break;
       case CMD_UNIT_SHAPE_POLE_B:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleArea;
           nPer  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPolePercent < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPolePercent;
           nMin  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMin < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMin;
           nMax  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMax < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struPole.nPoleWidthMax;
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_B, UNIT, nLevelId, nUnitAddr, 0, nMin, nSens, nMax, nPer, 3);
           break;
       case CMD_UNIT_SHAPE_SLICE:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struSlice.nArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struSlice.nArea;
           MySerial.com1Write(CMD_UNIT_SHAPE_SLICE, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
           break;
       case CMD_UNIT_SHAPE_LEAF:
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nThreshold < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nThreshold;
           nRow  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nArea < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nArea;
           nPer  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nPercent < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nPercent;
           nMin  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMin < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMin;
           nMax  = struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMax < 0 ? 0 : struCnfp.struGroupIdentify[nLevelId][nGroupId].struLeaf.nWidthMax;
           MySerial.com1Write(CMD_UNIT_SHAPE_POLE_A, UNIT, nLevelId, nUnitAddr, nSens, nRow, nPer, nMin, nMax, 3);
           MySerial.com1Write(CMD_UNIT_SHAPE_LEAF,   UNIT, nLevelId, nUnitAddr, nSens, nRow, nPer, nMin, nMax, 3);
           break;
       case CMD_UNIT_EARTHPEA_PARAMS1_MISC:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nRow-1,
                              ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nPercent/256),
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nPercent%256,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nPercentLimit, 0, 3);
           break;
       case CMD_UNIT_EARTHPEA_PARAMS1_AREA:
           tmpConvert = (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[0].nArea * 2.55);
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, tmpConvert / 256,tmpConvert % 256, 3);
           break;
       case CMD_UNIT_EARTHPEA_PARAMS2_MISC:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nRow-1,
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nPercent/256),
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nPercent%256,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nPercentLimit, 0, 3);
           break;
       case CMD_UNIT_EARTHPEA_PARAMS2_AREA:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nArea/256,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struBud[1].nArea%256, 3);
           break;
       case CMD_UNIT_GREY_A:
       case CMD_UNIT_GREY_B:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMax*0.255)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMin*0.255+0.5)/1;
           }

           if(nCmd == CMD_UNIT_GREY_A) {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_GREY_A]*0.255+0.5);
           } else {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_GREY_B]*0.255+0.5);
           }
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = nMin-nSens;
               nMin = (nMin > 0) ? nMin : 0;
               nMin = (nMin < 255) ? nMin : 255;
           } else {
               nMax = nMax+nSens;
               nMax = (nMax > 0) ? nMax : 0;
               nMax = (nMax < 255) ? nMax : 255;
           }

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColor<<4)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nRow-1),
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)%256, nMin, nMax, 3);
           materialResetGreyRsvPer(nLevelId, nGroupId, nUnitAddr, nIndex);  //! 重置灰度保留比例参数
           break;
       case CMD_UNIT_BURNT_ENABLE:
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nBurntReservedFlag,3);
           break;
       case CMD_UNIT_DISCOLOR_A_1:
       case CMD_UNIT_DISCOLOR_B_1:
           switch(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nDiscolor) {
           case 0:
               nMain = 1;
               nVice = 2;
               break;
           case 1:
               nMain = 1;
               nVice = 4;
               break;
           case 2:
               nMain = 2;
               nVice = 4;
               break;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nRow-1,
                              ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)/256,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nPercent-1)%256,
                             (nMain<<4|nVice), struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nLightLimit, 3);
           materialResetGreyRsvPer(nLevelId, nGroupId, nUnitAddr, nIndex);  //! 重置灰度保留比例参数
           break;
       case CMD_UNIT_DISCOLOR_A_2:
       case CMD_UNIT_DISCOLOR_B_2:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMax*0.0255)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nSensMin*0.0255+0.5)/1;
           }

           if (nCmd == CMD_UNIT_DISCOLOR_A_2) {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_DISCOLOR_A]*0.0255+0.5)/1;
           } else {
               nSens = (struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_DISCOLOR_B]*0.0255+0.5)/1;
           }
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].nMode == 1) {
               nMin = nMin-nSens;
               nMin = (nMin > 0) ? nMin : 0;
               nMin = (nMin < 255) ? nMin : 255;
           } else {
               nMax = nMax+nSens;
               nMax = (nMax > 0) ? nMax : 0;
               nMax = (nMax < 255) ? nMax : 255;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nMin, nMax, 3);
           break;
       case CMD_UNIT_PEANUT_A:
           MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr,
                    (1<<4)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nRow-1),
                   ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nPercent-1)/256,
                   (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nPercent-1)%256, 0,
                    (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nSensMin*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_1, UNIT, nLevelId, nUnitAddr, 0, ((1<<4)|2), 1, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[0].nSensReserved*0.255+0.5)/1, 255, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_2, UNIT, nLevelId, nUnitAddr, 0, 1, 1, 0, 255, 3);
           break;
       case CMD_UNIT_PEANUT_B:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nMode == 0) {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nSensMin*0.255+0.5)/1;
           }
           else {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nSensMax*0.255)/1;
               nMax = 255;
           }
           MySerial.com1Write(CMD_UNIT_GREY_B, UNIT, nLevelId, nUnitAddr,
                              (4<<4)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nRow-1),
                              ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nPercent-1)/256,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nPercent-1)%256, nMin, nMax, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_1, UNIT, nLevelId, nUnitAddr, 0, (1<<4|2), 2, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[1].nSensReserved*0.255+0.5)/1, 255, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_2, UNIT, nLevelId, nUnitAddr, 0, 1, 2, 0, 255, 3);
           break;
       case CMD_UNIT_PEANUT_C:
           MySerial.com1Write(CMD_UNIT_DISCOLOR_A_1, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nRow-1,
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nPercent-1)/256,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nPercent-1)%256, (1<<4|2), 50, 3);
           MySerial.com1Write(CMD_UNIT_DISCOLOR_A_2, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nSensMin*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_1, UNIT, nLevelId, nUnitAddr, 0, (1<<4|2), 3, 0, 255, 3);
           MySerial.com1Write(CMD_UNIT_RESERVED_2, UNIT, nLevelId, nUnitAddr, 0, 4, 3, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[2].nSensReserved*0.255+0.5)/1, 255, 3);
           break;
       case CMD_UNIT_PEANUT_D:
           MySerial.com1Write(CMD_UNIT_DISCOLOR_B_1, UNIT, nLevelId, nUnitAddr,
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nRow-1,
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nColumn-1)<<2)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nPercent-1)/256,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nPercent-1)%256, (2<<4|4), 50, 3);
           MySerial.com1Write(CMD_UNIT_DISCOLOR_B_2, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struPeanutAbcd[3].nSensMin*0.255+0.5)/1, 255, 3);
           break;
       case CMD_UNIT_RESERVED_1:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nMode1 == 1) { 	// 0: grey A , 1: grey B, 2: discolor A, 3: discolor B
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMax1*0.255)/1;
               nMax = 255;
           }
           else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMin1*0.255+0.5)/1;
           }
           switch(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nDiscolor1) {
           case 0:
               nMain = 1;
               nVice = 2;
               break;
           case 1:
               nMain = 1;
               nVice = 4;
               break;
           case 2:
               nMain = 2;
               nVice = 4;
               break;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, (nMain<<4|nVice), nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_RESERVED_2:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nMode2 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMax2*0.255)/1;
               nMax = 255;
           }
           else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nSensMin2*0.255+0.5)/1;
           }

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nIndex].struReserved.nColor2, nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_NEW_INTEL_RATIO_SENS_1:
       case CMD_UNIT_NEW_INTEL_RATIO_SENS_2:
           nMin = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSensRatio*0.255;
           nMax = 255;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nIntelType, 0, 0,nMin, nMax, 3);
           break;
       case CMD_UNIT_INTEL_A_1:
       case CMD_UNIT_INTEL_B_1:
       case CMD_UNIT_INTEL_C_1:
       case CMD_UNIT_INTEL_D_1:
	       if (struCnfp.nIntelMode == 1) {
               MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
               break;
	       }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0,
                              (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[0]<<7)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[1]/256),
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[1]%256,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[0]<<7)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[1]/256),
                              struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[1]%256,3);
           break;
       case CMD_UNIT_INTEL_A_2:
       case CMD_UNIT_INTEL_B_2:
       case CMD_UNIT_INTEL_C_2:
       case CMD_UNIT_INTEL_D_2:
           if (struCnfp.nIntelMode == 1) {
               MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
               break;
           }

           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[0] == 0) { // 常数项为正号
               if(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens > 100) {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens-100)/100.0);
               }
               else {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1+(100-struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens)/100.0);
               }
           } else {                                                                               // 常数项为负号
               if(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens > 100) {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1+(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens-100)/100.0);
               }
               else {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1-(100-struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens)/100.0);
               }
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0,
                       (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[0]<<7)|(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[1]/256),
                       struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[1]%256,
                       (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[0]<<7)|(nConsSens/256), nConsSens%256, 3);
           break;
       case CMD_UNIT_INTEL_A_3:
       case CMD_UNIT_INTEL_B_3:
       case CMD_UNIT_INTEL_C_3:
       case CMD_UNIT_INTEL_D_3:
           nRow = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRow;
           nCol = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nColumn;

           nTinySens = 0;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
			      struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRow-1,
			      ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nColumn-1)<<2)|((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercent-1)/256),
			      ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercent-1))%256, nTinySens,
                  (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nEnable), 3);
           break;
       case CMD_UNIT_INTEL_RESERVED_1:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nMode1 == 1) { 	// 0: grey A , 1: grey B, 2: discolor A, 3: discolor B
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMax1*0.255+0.5)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMin1*0.255+0.5)/1;
           }

           switch(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nDiscolor1) {
           case 0:
               nMain = 1;
               nVice = 2;
               break;
           case 1:
               nMain = 1;
               nVice = 4;
               break;
           case 2:
               nMain = 2;
               nVice = 4;
               break;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, nMain<<4|nVice, nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_INTEL_RESERVED_2:
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nMode2 == 1) {
               nMin = 255-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMax2*0.255+0.5)/1;
               nMax = 255;
           } else {
               nMin = 0;
               nMax = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nSensMin2*0.255+0.5)/1;
           }

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].struReserved.nColor2, nIndex+1, nMin, nMax, 3);
           break;
       case CMD_UNIT_INTEL_R:
           if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, 0, 0, 0, 0, 3);
                break;
           }
           nConf = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[1];
           value[0] = (nConf&0x000000FF);
           value[1] = (nConf&0x0000FF00)>>8;
           value[2] = (nConf&0x00FF0000)>>16;
           value[3] = (nConf&0x7F000000)>>24;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo1[0]<<7)|value[3],
                   value[2], value[1], value[0],3);
           break;
       case CMD_UNIT_INTEL_G:
           if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, 0, 0, 0, 0, 3);
                break;
           }
           nConf = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[1];
           value[0] = (nConf&0x000000FF);
           value[1] = (nConf&0x0000FF00)>>8;
           value[2] = (nConf&0x00FF0000)>>16;
           value[3] = (nConf&0x7F000000)>>24;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo2[0]<<7)|value[3],
                   value[2], value[1], value[0],3);
           break;
       case CMD_UNIT_INTEL_B:
           if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, 0, 0, 0, 0, 3);
                break;
           }
           nConf = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[1];
           value[0] = (nConf&0x000000FF);
           value[1] = (nConf&0x0000FF00)>>8;
           value[2] = (nConf&0x00FF0000)>>16;
           value[3] = (nConf&0x7F000000)>>24;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuo3[0]<<7)|value[3],
                   value[2], value[1], value[0],3);
           break;
       case CMD_UNIT_INTEL_C:
           if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, 0, 0, 0, 0, 3);
                break;
           }
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[0] == 0) { // 常数项为正号
               if(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens > 100) {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1-(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens-100)/100.0);
               }
               else {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1+(100-struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens)/100.0);
               }
           } else {                                                                               // 常数项为负号
               if(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens > 100) {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1+(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens-100)/100.0);
               }
               else {
                    nConsSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[1]*(1-(100-struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens)/100.0);
               }
           }
           value[0] = (nConsSens&0x000000FF);
           value[1] = (nConsSens&0x0000FF00)>>8;
           value[2] = (nConsSens&0x00FF0000)>>16;
           value[3] = (nConsSens&0x7F000000)>>24;
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nCons[0]<<7)|value[3],
                   value[2], value[1], value[0],3);
           break;
       case CMD_UNIT_INTEL_RESERVED:
           int arg1;
           nRow = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRowRes;

           nPer = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercentRes;

           arg1 = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nEnable|
                   (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nEnable<<1)|
                   (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[2].nEnable<<2)|
                   (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[3].nEnable<<3);

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, arg1,
                  struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRowRes-1,
                  ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nRowRes-1)<<2)|((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercent-1)/256),
                  ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nPercentRes-1))%256, 0, 3);
           break;
       case CMD_UNIT_INTEL_DFL_SIZE:
            if (struCnfp.nIntelMode == 1) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, struCnfp.nColorSpace, 3);
                 if (myFlow.getProductLineNo() == 0) {
                     if (struCnfp.nArithmeticEnable[ARITH_INTEL_A] == 1) {
                         MySerial.com1Write(CMD_UNIT_NEW_INTEL_DFL_SIZE, UNIT, nLevelId, nUnitAddr,
                                       struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nRow-1,
                                       struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nColumn-1,
                                       (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1)/256,
                                       ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nPercent-1))%256,
                                       struCnfp.nColorSpace, 3);
                     }
                     if (struCnfp.nArithmeticEnable[ARITH_INTEL_B] == 1){
                         MySerial.com1Write(CMD_UNIT_NEW_INTEL_DFL_SIZE_B, UNIT, nLevelId, nUnitAddr,
                                       struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nRow-1,
                                       struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nColumn-1,
                                       (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nPercent-1)/256,
                                       ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[1].nPercent-1))%256,
                                       struCnfp.nColorSpace, 3);
                     }
                 }
            }
            break;
       case CMD_UNIT_INTEL_DFL_CONST:
            if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
            }
            nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[0];

           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1) ? 0:1;
           }

           //! 计算灵敏度偏置
           nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nSens;
           nSens += struCnfp.nSensBias[nUnitAddrLocInLevelArray][ARITH_INTEL_A+nIndex]/10;
           nSens = (nSens > 0) ? nSens : 0;
           nSens = (nSens < 200) ? nSens : 200;

           nSignDfl_bak = nSignDfl;
           if (abs(struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]) < 200000) {
               nConsAdj = 200000;
               if(nSignDfl == 1) {
                   /* 灵敏度越大，常数项绝对值越大，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0;
                   } else {   // 灵敏度越小，常数项绝对值越小，选的越小
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0;
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 0;
                           nConfDfl = abs(nConfDfl);
                       }
                   }
               } else {
                   /* 灵敏度越大，常数项越小，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0;
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 1;
                           nConfDfl = abs(nConfDfl);
                       }
                   } else {   // 灵敏度越小，常数项越大，选的越少
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0;
                   }
               }
           }
           else {
               if(nSignDfl == 1) {
                   /* 灵敏度越大，常数项绝对值越大，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1+struCnfg.nMaterPer3/10.0*(nSens-100)/100.0);
                   } else {   // 灵敏度越小，常数项绝对值越小，选的越小
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1-struCnfg.nMaterPer3/10.0*(100-nSens)/100.0);
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 0;
                           nConfDfl = abs(nConfDfl);
                       }
                   }
               } else {
                   /* 灵敏度越大，常数项越小，选的越多 */
                   if (nSens >= 100) {
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1-struCnfg.nMaterPer3/10.0*(nSens-100)/100.0);
                       if (nConfDfl < 0)  {
                           nSignDfl_bak = 1;
                           nConfDfl = abs(nConfDfl);
                       }
                   } else {   // 灵敏度越小，常数项越大，选的越少
                       nConfDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nConsDfl[1]*(1+struCnfg.nMaterPer3/10.0*(100-nSens)/100.0);
                   }
               }
           }

           printf("nSigDef:%d\n",nSignDfl);
           printf("nSigDef_bak:%d\n",nSignDfl_bak);
           printf("nConsDfl:%d\n",nConfDfl);
           value[0] = (nConfDfl&0x000000FF);
           value[1] = (nConfDfl&0x0000FF00)>>8;
           value[2] = (nConfDfl&0x00FF0000)>>16;
           value[3] = (nConfDfl&0x7F000000)>>24;

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl_bak<<7)|value[3], value[2], value[1], value[0], 3);
           materialResetIntelRsvPer(nLevelId, nGroupId, nUnitAddr);
           break;;
       case CMD_UNIT_INTEL_DFL_RED:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[1]&0x7F000000)>>24;

           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoR[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_GREEN:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoG[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_BLUE:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoB[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RG:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRG[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RB:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[1]&0x7F000000)>>24;

           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRB[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_GB:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGB[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_RR:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoRR[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_GG:
           if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
           }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoGG[0];
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }

           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case CMD_UNIT_INTEL_DFL_BB:
            if (struCnfp.nIntelMode == 0) {
                MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                break;
            }
           value[0] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x000000FF);
           value[1] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x0000FF00)>>8;
           value[2] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x00FF0000)>>16;
           value[3] = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[1]&0x7F000000)>>24;
           nSignDfl = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nQuoBB[0];;
           if (struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[nIndex].nModeDfl) {
               nSignDfl = (nSignDfl == 1)?0:1;
           }
           MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr, nIndex, (nSignDfl<<7)|value[3], value[2], value[1], value[0], 3);
           break;
       case ARITH_WATERMELON:
           MySerial.com1Write(CMD_UNIT_MELON_WHITE_SENS, UNIT, nLevelId, nUnitAddr, 
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nWhitePropMin*0.255+0.5)/1, 
						   255, 
						   0, 
						   (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nWhiteSens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_EDGE_RED, UNIT, nLevelId, nUnitAddr, 
						   0, 
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeRedRatio*0.255+0.5)/1,
						   0,
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeRedSens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_EDGE_DAMAGED, UNIT, nLevelId, nUnitAddr, 
						   0, 
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeDamagedRatio*0.255+0.5)/1,
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nEdgeDamagedSens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_RUGGED_1, UNIT, nLevelId, nUnitAddr, 
						   0, 
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged1Save*0.255+0.5)/1,
						   0, 
						   255-(int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged1Sens*0.255+0.5)/1, 3);
           MySerial.com1Write(CMD_UNIT_MELON_RUGGED_2, UNIT, nLevelId, nUnitAddr, 
						   0, 
						   0, 
						   (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged2Save*0.255+0.5)/1,
						   0, 
						   (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMatMelon.nRugged2Sens*0.255+0.5)/1, 3);
           break;
       case CMD_ARITH_RICE:
            if(struCnfg.nParamSetAlone == 1) {
               break;
            }
            nSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struRice.nSens/2;
            nDarkSens = struCnfp.struGroupIdentify[nLevelId][nGroupId].struRice.nDarkSens/2;
            switch(struCnfg.nProfileMode[struCnfg.nProfile]) {
            case RS_DARK_SORT:                   // 深色专选
                if (nUnitAddr%2) {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                } else {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                }
                break;
            case RS_DIFF_POSITIVE_SORT:          // 异色粒正选
                MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                break;
            case RS_DIFF_NEGATIVE_SORT:          // 异色粒反选
                MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
                break;
            case RS_WHITE_POSITIVE_SORT:         // 乳白色正选
                if (nUnitAddr%2) {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
                } else {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                }
                break;
            case RS_WHITE_NEGATIVE_SORT:         // 乳白色反选
                if (nUnitAddr%2) {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, 0, 0, 3);
               } else {
                   MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
               }
               break;
            case RS_YELLOW_WHITE_SORT:           // 黄白同选
                MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                break;
            case RS_YELLOW_TRANS_SORT:           // 黄透同选
                if (nUnitAddr%2) {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                } else {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
                }
                break;
            case RS_YELLOW_YELLOW_NEGATIVE_SORT: // 黄+黄反选
                if(nUnitAddr%2) {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
                } else {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                }
                break;
            case RS_WHITE_YELLOW_NEGATIVE_SORT:  // 黄+白反选
                if(nUnitAddr%2) {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, nSens/256, nSens%256, 0, 0, 3);
                } else {
                    MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, 0, 0, nSens/256, nSens%256, 3);
                }
                break;
            case RS_USER_DEFINED_SORT:           // 自定义模式
                MySerial.com1Write(CMD_UNIT_GREY_A, UNIT, nLevelId, nUnitAddr, 0, nSens/256, nSens%256,
                                    nDarkSens/256, nDarkSens%256,  3);
                break;
            default:
                break;
            }
            break;
       /* 玉米算法 */
       case CMD_UNIT_MAIZE_PARAMS_1:
          tmp1 = (int)(struCnfp.nSensBiasMaize[nUnitAddrLocInLevelArray][1]*0.255+0.25);
          tmp2  = (int)(struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nSensMin2*0.255+0.25);
          nMin  = (int)(tmp2+tmp1);
          nMin  = (nMin > 0) ? nMin : 0;
          nMin  = (nMin < 255) ? nMin : 255;

          tmp1 = (struCnfp.nSensBiasMaize[nUnitAddrLocInLevelArray][2]*0.255+0.25);
          tmp2  = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nSensProport*0.255+0.25);
          nMinp  = (int)(tmp2+tmp1);
          nMinp  = (nMinp > 0) ? nMinp : 0;
          nMinp  = (nMinp < 255) ? nMinp : 255;

          MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                             0,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColor1,
                             nMinp,
                             0,
                             nMin,
                             3);
          break;
      case CMD_UNIT_MAIZE_PARAMS_2:
          tmp1 = (struCnfp.nSensBiasMaize[nUnitAddrLocInLevelArray][0]*0.255+0.25);
          tmp2  = (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nSensMin1*0.255+0.25);
          nMin  = (int)(tmp2+tmp1);
          nMin  = (nMin > 0) ? nMin : 0;
          nMin  = (nMin < 255) ? nMin : 255;
          struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nRow = 7;
          struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColumn = 14;
          MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColor2 ,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nRow-1),
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nColumn-1),
                             0,
                             nMin,
                             3);
          break;
      case CMD_UNIT_MAIZE_PARAMS_3:
          struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nPercentTotal = 80;
          MySerial.com1Write(nCmd, UNIT, nLevelId, nUnitAddr,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struReserved.nRow-1),     //还原圈数
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struReserved.nThreshold_r, //r阈值
                             struCnfp.struGroupIdentify[nLevelId][nGroupId].struReserved.nThreshold_s, //s阈值
                             ((struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nPercentTotal-1) >> 8) & 0xff,
                             (struCnfp.struGroupIdentify[nLevelId][nGroupId].struMaize.nPercentTotal-1) & 0xff,
                             3);
          break;
       default:
           break;
       }
    }
}

/***************************************************************************************************
    名称：         物料参数组包选择
    功能：         根据T2、T3等传感器选择不同的组包函数

***************************************************************************************************/
void GlobalFlow::materialParamsSet(int nCmd, int nLevelId, int nGroupId, int nIndex)
{
    switch(struCnfc.nSensorType) {
    case SENSOR_T_2564:
    case SENSOR_D2:
	case SENSOR_P5:
        switch (struCnfe.nMachine) {
        default:
            materialParamsNewPacketOrganize(nCmd, nLevelId, nGroupId, nIndex);
            break;
        }
        break;
    case SENSOR_T_2566:
        materialParamsPacketOrganize(nCmd, nLevelId, nGroupId, nIndex);
        break;
    default:
        materialParamsPacketOrganize(nCmd, nLevelId, nGroupId, nIndex);
        break;
    }
}

/***************************************************************************************************
    名称：         重置物料参数
    功能：         发送物料参数给FPGA
    nLevelId:     识别组所在的层编号
    nGroupId：    识别组的组号
    bIsAllSens：  判断是发送灵敏度设置界面中的所有算法的灵敏度，还是方案设置中某个识别算法的参数
    nArithSeq：   识别算法序号
    bIsReserved:  是否是发送保留参数
***************************************************************************************************/
void GlobalFlow::resetMaterial(int nLevelId, int nGroupId, int bIsAllSens, int nArithSeq, int bIsReserved)
{
    int nArithmeticEnable[MAX_LEVEL][ARITHMETIC_TOTAL];
    int nArithBegin, nArithEnd;

    if (bIsAllSens == 0) {
        nArithBegin = nArithSeq;
        nArithEnd   = nArithSeq+1;
    } else {
        nArithBegin = 0;
        nArithEnd   = struCnfe.nArithmeticTotal;
    }

    /* get arithmetic enable */
    memset(nArithmeticEnable, 0, sizeof(nArithmeticEnable));
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfe.nArithmeticTotal; j++) {
            nArithmeticEnable[i][j] = struCnfp.nArithmeticEnable[j];
        }
    }

    for (int i = nArithBegin; i < nArithEnd; i++) {
        if (nArithmeticEnable[nLevelId][i] == 1) {
            switch (i) {
            case ARITH_GREY_A :
                materialParamsSet(CMD_UNIT_GREY_A, nLevelId, nGroupId, 0);
                if (struCnfe.nEnableMaizeDoubleView == 1) {
                    materialParamsSet(CMD_UNIT_BURNT_ENABLE, nLevelId, nGroupId, 0);
                }
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_RESERVED_1, nLevelId, nGroupId, 0);
                    materialParamsSet(CMD_UNIT_RESERVED_2, nLevelId, nGroupId, 0);
                }
                break;
            case ARITH_GREY_B:
                materialParamsSet(CMD_UNIT_GREY_B, nLevelId, nGroupId, 1);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_RESERVED_1, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_RESERVED_2, nLevelId, nGroupId, 1);
                }
                break;
            case ARITH_RESERVED:
                materialParamsSet(CMD_UNIT_MAIZE_PARAMS_3, nLevelId, nGroupId, 0);  //保留算法的参数借用玉米参数命令下发
                break;
            case ARITH_DISCOLOR_A:
                materialParamsSet(CMD_UNIT_DISCOLOR_A_1, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_DISCOLOR_A_2, nLevelId, nGroupId, 2);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_RESERVED_1, nLevelId, nGroupId, 2);
                    materialParamsSet(CMD_UNIT_RESERVED_2, nLevelId, nGroupId, 2);
                }
                break;
            case ARITH_DISCOLOR_B:
                materialParamsSet(CMD_UNIT_DISCOLOR_B_1, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_DISCOLOR_B_2, nLevelId, nGroupId, 3);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_RESERVED_1, nLevelId, nGroupId, 3);
                    materialParamsSet(CMD_UNIT_RESERVED_2, nLevelId, nGroupId, 3);
                }
                break;
            case ARITH_CROSS:
                materialParamsSet(CMD_UNIT_CROSS_PARAMS, nLevelId, nGroupId, 0);
                break;
            case ARITH_SHAPE:
            case ARITH_SHAPE_LENGTH:
                materialParamsSet(CMD_UNIT_SHAPE, nLevelId, nGroupId, 0);
                break;
            case ARITH_SHAPE_POLE_A:
                materialParamsSet(CMD_UNIT_SHAPE_POLE_A, nLevelId, nGroupId, 0);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_SHAPE_POLE_A_RESERVED, nLevelId, nGroupId, 0);
                }
                break;
            case ARITH_SHAPE_POLE_B:
                materialParamsSet(CMD_UNIT_SHAPE_POLE_B, nLevelId, nGroupId, 0);
                break;
            case ARITH_SHAPE_SLICE:
                materialParamsSet(CMD_UNIT_SHAPE_SLICE, nLevelId, nGroupId, 0);
                break;
            case ARITH_SHAPE_LEAF:
                materialParamsSet(CMD_UNIT_SHAPE_LEAF, nLevelId, nGroupId, 0);
                break;
            case ARITH_SCALE:
                materialParamsSet(CMD_UNIT_SCALE_PARAMS, nLevelId, nGroupId, 0);
                break;
            case ARITH_SCALE_B:
                materialParamsSet(CMD_UNIT_SCALE_B_PARAMS, nLevelId, nGroupId, 0);
                break;
            case ARITH_BUD_1:
                materialParamsSet(CMD_UNIT_EARTHPEA_PARAMS1_MISC, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_EARTHPEA_PARAMS1_AREA, nLevelId, nGroupId, 0);
                break;
            case ARITH_BUD_2:
                materialParamsSet(CMD_UNIT_EARTHPEA_PARAMS2_MISC, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_EARTHPEA_PARAMS2_AREA, nLevelId, nGroupId, 0);
                break;
            case ARITH_PEANUT_A:
                materialParamsSet(CMD_UNIT_PEANUT_A, nLevelId, nGroupId, 0);
                break;
            case ARITH_PEANUT_B:
                materialParamsSet(CMD_UNIT_PEANUT_B, nLevelId, nGroupId, 0);
                break;
            case ARITH_PEANUT_C:
                materialParamsSet(CMD_UNIT_PEANUT_C, nLevelId, nGroupId, 0);
                break;
            case ARITH_PEANUT_D:
                materialParamsSet(CMD_UNIT_PEANUT_D, nLevelId, nGroupId, 0);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_RESERVED_1, nLevelId, nGroupId, 3);
                    materialParamsSet(CMD_UNIT_RESERVED_2, nLevelId, nGroupId, 3);
                }
                break;
            case ARITH_INTEL_A :
                materialParamsSet(CMD_UNIT_INTEL_A_1, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_A_2, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_A_3, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_RESERVED, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_R, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_G, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_B, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_C, nLevelId, nGroupId, 0);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_1, nLevelId, nGroupId, 0);
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_2, nLevelId, nGroupId, 0);
                }
                /* 多分类中借用智能模式A的尺寸和纯度 */
                materialParamsSet(CMD_UNIT_INTEL_DFL_SIZE, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_NEW_INTEL_RATIO_SENS_1, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_CONST, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_RED, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_GREEN, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_BLUE, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_RG, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_RB, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_GB, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_RR, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_GG, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_INTEL_DFL_BB, nLevelId, nGroupId, 0);
                break;
            case ARITH_INTEL_B :
                materialParamsSet(CMD_UNIT_INTEL_B_1, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_B_2, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_B_3, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_RESERVED, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_R, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_G, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_B, nLevelId, nGroupId, 1);
                materialParamsSet(CMD_UNIT_INTEL_C, nLevelId, nGroupId, 1);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_1, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_2, nLevelId, nGroupId, 1);
                }

                if (myFlow.getProductLineNo() == 0) {
                    materialParamsSet(CMD_UNIT_INTEL_DFL_SIZE, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_NEW_INTEL_RATIO_SENS_2, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_CONST, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_RED, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_GREEN, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_BLUE, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_RG, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_RB, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_GB, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_RR, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_GG, nLevelId, nGroupId, 1);
                    materialParamsSet(CMD_UNIT_INTEL_DFL_BB, nLevelId, nGroupId, 1);
                }
                break;
            case ARITH_INTEL_C :
                materialParamsSet(CMD_UNIT_INTEL_C_1, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_C_2, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_C_3, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_RESERVED, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_R, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_G, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_B, nLevelId, nGroupId, 2);
                materialParamsSet(CMD_UNIT_INTEL_C, nLevelId, nGroupId, 2);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_1, nLevelId, nGroupId, 2);
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_2, nLevelId, nGroupId, 2);
                }
                break;
            case ARITH_INTEL_D :
                materialParamsSet(CMD_UNIT_INTEL_D_1, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_D_2, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_D_3, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_RESERVED, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_R, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_G, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_B, nLevelId, nGroupId, 3);
                materialParamsSet(CMD_UNIT_INTEL_C, nLevelId, nGroupId, 3);
                if (bIsReserved == 1) {
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_1, nLevelId, nGroupId, 3);
                    materialParamsSet(CMD_UNIT_INTEL_RESERVED_2, nLevelId, nGroupId, 3);
                }
                break;
            case ARITH_WATERMELON:
                materialParamsSet(ARITH_WATERMELON, nLevelId, nGroupId, 0);
                break;
            case ARITH_RICE:
                materialParamsSet(CMD_ARITH_RICE, nLevelId, nGroupId, 0);
                break;
            case ARITH_MAIZE:
                materialParamsSet(CMD_UNIT_MAIZE_PARAMS_1, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_MAIZE_PARAMS_2, nLevelId, nGroupId, 0);
                materialParamsSet(CMD_UNIT_MAIZE_PARAMS_3, nLevelId, nGroupId, 0);
            break;
            case ARITH_CANDY:
                materialParamsSet(CMD_UNIT_CANDY, nLevelId, nGroupId, 0);
                break;
            case ARITH_PISTACHIO:
                //                materialParamsSet(CMD_UNIT_PISTACHIO, nLevelId, nGroupId, 0);
                materialAiModelParaSet(nLevelId, nGroupId);
                break;
            default:
                break;
            }
        }
    }
}

void GlobalFlow::materialAiModelParaSet(int nLevelId, int nGroupId){
    int nUnitAddr = 0;
    int ret;
    QByteArray args;
    AI_Data_Protocol_D data;
    int applyCount = 0;
    int seq = 0;
    args.clear();
    if(struCnfg.aiEnable == 1){
        QString modelId =  QString::fromUtf8(struCnfp.struGroupIdentify[nLevelId][nGroupId].struAi.modelId);
        QSqlQuery query;
        int modeParaCount = 0;
        query.prepare("SELECT modelId, id, zhName, enName, threshold, isApply, chgTime FROM modelParaInfo  "
                            "where modelId = ?  and levelTotal = ? and identifyGroupTotal = ? order by id asc");

        query.bindValue(0,modelId);
        query.bindValue(1,nLevelId);
        query.bindValue(2,nGroupId);
        if (!query.exec()) {
            qDebug() << "查询失败：" << query.lastError();
        } else {
            while (query.next()) {
                QString id = query.value(1).toString();
                QString zhName = query.value(2).toString();
                QString threshold = query.value(4).toString();
                QString isApply = query.value(5).toString();

                if(id.isNull() || zhName.isNull()||threshold.isNull() ||isApply.isNull()){
                    break;
                }
                modeParaStr[modeParaCount].id = id;
                modeParaStr[modeParaCount].isApply = isApply;
                modeParaStr[modeParaCount].name = zhName;
                modeParaStr[modeParaCount].threshold = threshold;

                if(modeParaStr[modeParaCount].isApply.toInt() == 1){
                    applyCount++;
                    args[seq++] = modeParaStr[modeParaCount].id.toInt();
                    //阈值=100-灵敏度
                    args[seq++] = 100-modeParaStr[modeParaCount].threshold.toInt();
                }
                modeParaCount++;
            }
        }

        if(applyCount == 0){
            return;
        }

        for (int k = 0; k < struCnfg.struLevelInfo[nLevelId].struIdentifyGroupInfo[nGroupId].nUnitCount; k++) {
            nUnitAddr =getIdentifyGroupAddr(nLevelId, nGroupId, k);
            if(nUnitAddr%2==0){
                MyUpd.writeDatagram(CMD_AI_MODEL_PARA, nUnitAddr/2, applyCount*2, args, struGsh.addressList.at(nUnitAddr/2), AI_UDP_SEND_PORT);
                data.nCommandAddress = CMD_AI_MODEL_PARA;
                ret = MyUpd.readUdpDatagrams(&data,13);
                if(ret !=0){
                    qDebug("nUnitAddr/2: %d, ret: %d",  nUnitAddr/2, ret);
                }
            }
       }
    }
}

/***************************************************************************************************
    名称：         识别组发送编号确定
    功能：         根据物料参数设置方式，计算获取准备发送物料参数的识别组编号
    nLevelId:     识别组所在的层编号
    nGroupId：    识别组的组号
    bIsAllSens：  判断是发送灵敏度设置界面中的所有算法的灵敏度，还是方案设置中某个识别算法的参数
    nArithSeq：   识别算法序号
    bIsReserved:  是否是发送保留参数
***************************************************************************************************/
void GlobalFlow::materialResetGroupAssemble(int nLevelId, int nGroupId, int bIsAllSens, int nArithSeq, int bIsReserved)
{
    int caseValue = struCnfp.nMatAssembleMode;
    switch (struCnfe.nMachine) {
    case MACHINE_CF:
        switch (nGroupId) {
        case 0: // first_front
            resetMaterial(0, 0, bIsAllSens, nArithSeq, bIsReserved);
            switch (caseValue) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                resetMaterial(0, 1, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
                resetMaterial(0, 2, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 4, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 6, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_ALL_SAME:
                resetMaterial(0, 1, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 2, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 3, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 4, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 5, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 6, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 7, bIsAllSens, nArithSeq, bIsReserved);
                break;
            }
            break;
        case 1: // first_rear
            resetMaterial(0, 1, bIsAllSens, nArithSeq, bIsReserved);
            switch (caseValue) {
            case PARAMS_ALL_SEPARATE:
            case PARAMS_FRONT_REAR_SAME:
                break;
            case PARAMS_FIRST_SECOND_SAME:
                resetMaterial(0, 3, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 5, bIsAllSens, nArithSeq, bIsReserved);
                resetMaterial(0, 7, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_ALL_SAME:
                break;
            }
            break;
        case 2: // second_front
            resetMaterial(0, 2, bIsAllSens, nArithSeq, bIsReserved);
            switch (caseValue) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                resetMaterial(0, 3, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
            case PARAMS_ALL_SAME:
                break;
            }
            break;
        case 3: // second_rear
            resetMaterial(0, 3, bIsAllSens, nArithSeq, bIsReserved);
            break;
        case 4: // third_front
            resetMaterial(0, 4, bIsAllSens, nArithSeq, bIsReserved);
            switch (caseValue) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                resetMaterial(0, 5, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
            case PARAMS_ALL_SAME:
                break;
            }
            break;
        case 5: // third_rear
            resetMaterial(0, 5, bIsAllSens, nArithSeq, bIsReserved);
            break;
	case 6: // fourth_front
            resetMaterial(0, 6, bIsAllSens, nArithSeq, bIsReserved);
            switch (caseValue) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                resetMaterial(0, 7, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
            case PARAMS_ALL_SAME:
                break;
            }
	    break;
	case 7: // fourth_rear
            resetMaterial(0, 7, bIsAllSens, nArithSeq, bIsReserved);
	    break;
        default:
            break;
        }
        break;
    }
}

/***************************************************************************************************
    名称：         物料参数组间复制
    功能：         物料参数不同识别组直间的复制
    nLevelIdSrc:  准备复制的源识别组所在的层编号
    nGroupIdSrc： 准备复制的源识别组编号
    nnLevelIdDst：准备复制的目的识别组所在的层编号
    nGroupDst：   准备复制的目的识别组编号
    bIsAllSens：  判断是复制灵敏度设置界面中的所有算法的灵敏度，还是方案设置中某个识别算法的参数
    nArithSeq：   识别算法序号
    bIsReserved:  是否是复制保留参数
***************************************************************************************************/
void GlobalFlow::materialParamsCopy(int nLevelIdSrc, int nGroupIdSrc, int nLevelIdDst, int nGroupDst, int bIsAllSens, int nArithSeq, int bIsReserved)
{
    int nArithmeticEnable[MAX_LEVEL][ARITHMETIC_TOTAL];
    int nArithBegin, nArithEnd;
    int i, l1, l2, k1, k2;

    if (bIsAllSens == 0) {
        nArithBegin = nArithSeq;
        nArithEnd   = nArithSeq+1;
    } else {
        nArithBegin = 0;
        nArithEnd   = struCnfe.nArithmeticTotal;
    }

    /* get arithmetic enable */
    memset(nArithmeticEnable, 0, sizeof(nArithmeticEnable));
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfe.nArithmeticTotal; j++) {
            nArithmeticEnable[i][j] = struCnfp.nArithmeticEnable[j];
        }
    }

    l1 = nLevelIdSrc;
    l2 = nLevelIdDst;

    k1 = nGroupIdSrc;
    k2 = nGroupDst;
    for (i = nArithBegin; i < nArithEnd; i++) {
        if (nArithmeticEnable[nLevelIdSrc][i] == 1 && nArithmeticEnable[nLevelIdDst][i] == 1) {
           switch(i) {
           case ARITH_GREY_A:
           case ARITH_GREY_B:
           case ARITH_DISCOLOR_A:
           case ARITH_DISCOLOR_B:
               {
                   if (bIsAllSens == 1) {
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nSensMin = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nSensMin;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nSensMax = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nSensMax;
                       break;
                   }
                   if (bIsReserved == 1){
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nMode1 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nMode1;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nDiscolor1 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nDiscolor1;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMin1 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMin1;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMax1 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMax1;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nMode2 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nMode2;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nColor2 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nColor2;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMin2 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMin2;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMax2 = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].struReserved.nSensMax2;
                  } else {
                       memcpy(struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].sName, struCnfp.struGroupIdentify[l1][k2].struGreyColor[i-ARITH_GREY_A].sName,
                              sizeof(struCnfp.struGroupIdentify[l1][k2].struGreyColor[i-ARITH_GREY_A].sName));
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nSensMin = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nSensMin;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nSensMax = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nSensMax;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nMode = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nMode;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nRow = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nRow;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nColumn  = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nColumn;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nPercent = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nPercent;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nDiscolor = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nDiscolor;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nColor = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nColor;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nLightLimit = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nLightLimit;
                       struCnfp.struGroupIdentify[l2][k2].struGreyColor[i-ARITH_GREY_A].nBurntReservedFlag = struCnfp.struGroupIdentify[l1][k1].struGreyColor[i-ARITH_GREY_A].nBurntReservedFlag;
                   }
               }
               break;
           case ARITH_RESERVED:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struReserved.sName, struCnfp.struGroupIdentify[l1][k2].struReserved.sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struReserved.sName));
               struCnfp.struGroupIdentify[l2][k2].struReserved.nThreshold_s = struCnfp.struGroupIdentify[l1][k1].struReserved.nThreshold_s;
               struCnfp.struGroupIdentify[l2][k2].struReserved.nThreshold_r  = struCnfp.struGroupIdentify[l1][k1].struReserved.nThreshold_r;
               struCnfp.struGroupIdentify[l2][k2].struReserved.nRow  = struCnfp.struGroupIdentify[l1][k1].struReserved.nRow;
               break;
           case ARITH_CROSS:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struCross.sName, struCnfp.struGroupIdentify[l1][k2].struCross.sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struCross.sName));
               struCnfp.struGroupIdentify[l2][k2].struCross.nSens    = struCnfp.struGroupIdentify[l1][k1].struCross.nSens;
               struCnfp.struGroupIdentify[l2][k2].struCross.nRow     = struCnfp.struGroupIdentify[l1][k1].struCross.nRow;
               struCnfp.struGroupIdentify[l2][k2].struCross.nColumn  = struCnfp.struGroupIdentify[l1][k1].struCross.nColumn;
               struCnfp.struGroupIdentify[l2][k2].struCross.nColor   = struCnfp.struGroupIdentify[l1][k1].struCross.nColor;
               struCnfp.struGroupIdentify[l2][k2].struCross.nPercent = struCnfp.struGroupIdentify[l1][k1].struCross.nPercent;
               break;
           case ARITH_SHAPE:
           case ARITH_SHAPE_LENGTH:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struShape.sName, struCnfp.struGroupIdentify[l1][k2].struShape.sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struShape.sName));
               struCnfp.struGroupIdentify[l2][k2].struShape.nAreaMax =  struCnfp.struGroupIdentify[l1][k1].struShape.nAreaMax;
               struCnfp.struGroupIdentify[l2][k2].struShape.nAreaLimit =  struCnfp.struGroupIdentify[l1][k1].struShape.nAreaLimit;
               struCnfp.struGroupIdentify[l2][k2].struShape.nAreaMin = struCnfp.struGroupIdentify[l1][k1].struShape.nAreaMin;
               struCnfp.struGroupIdentify[l2][k2].struShape.nMode     = struCnfp.struGroupIdentify[l1][k1].struShape.nMode;           
               memcpy(struCnfp.struGroupIdentify[l2][k2].struShape.sLengthName, struCnfp.struGroupIdentify[l1][k2].struShape.sLengthName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struShape.sLengthName));
               struCnfp.struGroupIdentify[l2][k2].struShape.nLengthMin =  struCnfp.struGroupIdentify[l1][k1].struShape.nLengthMin;
               struCnfp.struGroupIdentify[l2][k2].struShape.nLengthMax = struCnfp.struGroupIdentify[l1][k1].struShape.nLengthMax;
               struCnfp.struGroupIdentify[l2][k2].struShape.nLengthMode  = struCnfp.struGroupIdentify[l1][k1].struShape.nLengthMode;
               struCnfp.struGroupIdentify[l2][k2].struShape.nLengthLimit  = struCnfp.struGroupIdentify[l1][k1].struShape.nLengthLimit;
               break;
           case ARITH_SHAPE_POLE_A:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struPole.sName, struCnfp.struGroupIdentify[l1][k2].struPole.sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struPole.sName));
               struCnfp.struGroupIdentify[l2][k2].struPole.nArea =  struCnfp.struGroupIdentify[l1][k1].struPole.nArea;
               struCnfp.struGroupIdentify[l2][k2].struPole.nPercent =  struCnfp.struGroupIdentify[l1][k1].struPole.nPercent;
               struCnfp.struGroupIdentify[l2][k2].struPole.nWidthMin =  struCnfp.struGroupIdentify[l1][k1].struPole.nWidthMin;
               struCnfp.struGroupIdentify[l2][k2].struPole.nWidthMax =  struCnfp.struGroupIdentify[l1][k1].struPole.nWidthMax;

               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nMode1 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nMode1;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nDiscolor1 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nDiscolor1;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nSensMin1 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nSensMin1;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nSensMax1 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nSensMax1;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nMode2 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nMode2;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nColor2 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nColor2;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nSensMin2 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nSensMin2;
               struCnfp.struGroupIdentify[l2][k2].struPole.struReserved.nSensMax2 =  struCnfp.struGroupIdentify[l1][k1].struPole.struReserved.nSensMax2;
               break;
	   case ARITH_SHAPE_POLE_B:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struPole.sPoleName, struCnfp.struGroupIdentify[l1][k2].struPole.sPoleName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struPole.sPoleName));
               struCnfp.struGroupIdentify[l2][k2].struPole.nPoleArea =  struCnfp.struGroupIdentify[l1][k1].struPole.nPoleArea;
               struCnfp.struGroupIdentify[l2][k2].struPole.nPolePercent =  struCnfp.struGroupIdentify[l1][k1].struPole.nPolePercent;
               struCnfp.struGroupIdentify[l2][k2].struPole.nPoleWidthMin =  struCnfp.struGroupIdentify[l1][k1].struPole.nPoleWidthMin;
               struCnfp.struGroupIdentify[l2][k2].struPole.nPoleWidthMax =  struCnfp.struGroupIdentify[l1][k1].struPole.nPoleWidthMax;
	       break;
	   case ARITH_SHAPE_SLICE:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struSlice.sName, struCnfp.struGroupIdentify[l1][k2].struSlice.sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struSlice.sName));
               struCnfp.struGroupIdentify[l2][k2].struSlice.nArea =  struCnfp.struGroupIdentify[l1][k1].struSlice.nArea;
	       break;
	   case ARITH_SHAPE_LEAF:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struLeaf.sName, struCnfp.struGroupIdentify[l1][k2].struLeaf.sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struLeaf.sName));
               struCnfp.struGroupIdentify[l2][k2].struLeaf.nArea =  struCnfp.struGroupIdentify[l1][k1].struLeaf.nArea;
               struCnfp.struGroupIdentify[l2][k2].struLeaf.nPercent =  struCnfp.struGroupIdentify[l1][k1].struLeaf.nPercent;
               struCnfp.struGroupIdentify[l2][k2].struLeaf.nWidthMin =  struCnfp.struGroupIdentify[l1][k1].struLeaf.nWidthMin;
               struCnfp.struGroupIdentify[l2][k2].struLeaf.nWidthMax =  struCnfp.struGroupIdentify[l1][k1].struLeaf.nWidthMax;
               struCnfp.struGroupIdentify[l2][k2].struLeaf.nThreshold =  struCnfp.struGroupIdentify[l1][k1].struLeaf.nThreshold;
	       break;
           case ARITH_SCALE:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struScale[0].sName, struCnfp.struGroupIdentify[l1][k2].struScale[0].sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struScale[0].sName));
               struCnfp.struGroupIdentify[l2][k2].struScale[0].nMode        = struCnfp.struGroupIdentify[l1][k1].struScale[0].nMode;
               struCnfp.struGroupIdentify[l2][k2].struScale[0].nValue       = struCnfp.struGroupIdentify[l1][k1].struScale[0].nValue;
               struCnfp.struGroupIdentify[l2][k2].struScale[0].nValueLimit = struCnfp.struGroupIdentify[l1][k1].struScale[0].nValueLimit;
               break;
           case ARITH_SCALE_B:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struScale[1].sName, struCnfp.struGroupIdentify[l1][k2].struScale[1].sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struScale[1].sName));
               struCnfp.struGroupIdentify[l2][k2].struScale[1].nMode        = struCnfp.struGroupIdentify[l1][k1].struScale[1].nMode;
               struCnfp.struGroupIdentify[l2][k2].struScale[1].nValue       = struCnfp.struGroupIdentify[l1][k1].struScale[1].nValue;
               struCnfp.struGroupIdentify[l2][k2].struScale[1].nValueLimit = struCnfp.struGroupIdentify[l1][k1].struScale[1].nValueLimit;
               break;
           case ARITH_BUD_1:
           case ARITH_BUD_2:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struBud[i-ARITH_BUD_1].sName, struCnfp.struGroupIdentify[l1][k2].struBud[i-ARITH_BUD_1].sName,
                      sizeof(struCnfp.struGroupIdentify[l1][k2].struBud[i-ARITH_BUD_1].sName));
               struCnfp.struGroupIdentify[l2][k2].struBud[i-ARITH_BUD_1].nPercent       = struCnfp.struGroupIdentify[l1][k1].struBud[i-ARITH_BUD_1].nPercent;
               struCnfp.struGroupIdentify[l2][k2].struBud[i-ARITH_BUD_1].nRow           = struCnfp.struGroupIdentify[l1][k1].struBud[i-ARITH_BUD_1].nRow;
               struCnfp.struGroupIdentify[l2][k2].struBud[i-ARITH_BUD_1].nColumn        = struCnfp.struGroupIdentify[l1][k1].struBud[i-ARITH_BUD_1].nColumn;
               struCnfp.struGroupIdentify[l2][k2].struBud[i-ARITH_BUD_1].nPercentLimit = struCnfp.struGroupIdentify[l1][k1].struBud[i-ARITH_BUD_1].nPercentLimit;
               struCnfp.struGroupIdentify[l2][k2].struBud[i-ARITH_BUD_1].nArea          = struCnfp.struGroupIdentify[l1][k1].struBud[i-ARITH_BUD_1].nArea;
               break;
           case ARITH_PEANUT_A:
           case ARITH_PEANUT_B:
           case ARITH_PEANUT_C:
           case ARITH_PEANUT_D:
               if (bIsAllSens == 1) {
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMin = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMin;
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMax = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMax;
               }
               else {
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMin = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMin;
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMax = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nSensMax;
                   memcpy(struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].sName, struCnfp.struGroupIdentify[l1][k2].struPeanutAbcd[i-ARITH_PEANUT_A].sName,
                          sizeof(struCnfp.struGroupIdentify[l1][k2].struPeanutAbcd[i-ARITH_PEANUT_A].sName));
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nRow           = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nRow;
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nColumn        = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nColumn;
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nPercent       = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nPercent;
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nMode          = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nMode;
                   struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].nSensReserved = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].nSensReserved;
                   if ((i == ARITH_PEANUT_D) || (bIsReserved == 1)){
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nMode1 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nMode1;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nDiscolor1 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nDiscolor1;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMin1 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMin1;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMax1 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMax1;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nMode2 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nMode2;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nColor2 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nColor2;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMin2 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMin2;
                       struCnfp.struGroupIdentify[l2][k2].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMax2 = struCnfp.struGroupIdentify[l1][k1].struPeanutAbcd[i-ARITH_PEANUT_A].struReserved.nSensMax2;
                   }
               }
               break;
           case ARITH_INTEL_A:
           case ARITH_INTEL_B:
           case ARITH_INTEL_C:
           case ARITH_INTEL_D:
               if (bIsAllSens == 1) {
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nCons[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nCons[0];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nCons[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nCons[1];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nSens    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nSens;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nSensRatio    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nSensRatio;
                   break;
               }
               if((i == ARITH_INTEL_A) || ((myFlow.getProductLineNo() == 0) && struCnfp.nIntelMode == 1)) {
                   memcpy(struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].sName, struCnfp.struGroupIdentify[l1][k2].struIntel[i-ARITH_INTEL_A].sName,
                          sizeof(struCnfp.struGroupIdentify[l1][k2].struIntel[i-ARITH_INTEL_A].sName));
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nRow        = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nRow;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nColumn     = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nColumn;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nPercent    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nPercent;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nRowRes        = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nRowRes;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nColumnRes     = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nColumnRes;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nPercentRes    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nPercentRes;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nModeDfl        = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nModeDfl;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nSens    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nSens;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nIntelType    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nIntelType;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nRatioMode    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nRatioMode;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nSensRatio    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nSensRatio;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nWholeEdge    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nWholeEdge;

                   if (struCnfp.nMatAssembleModeIntel == 1) {
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nConsDfl[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nConsDfl[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nConsDfl[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nConsDfl[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoR[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoR[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoR[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoR[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoG[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoG[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoG[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoG[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoB[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoB[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoB[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoB[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoRR[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoRR[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoRR[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoRR[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoGG[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoGG[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoGG[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoGG[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoBB[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoBB[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoBB[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoBB[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoRG[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoRG[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoRG[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoRG[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoRB[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoRB[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoRB[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoRB[1];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoGB[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoGB[0];
                       struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuoGB[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuoGB[1];
                   }
               }
               if (bIsReserved == 1){
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nMode1     = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nMode1;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nDiscolor1 = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nDiscolor1;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nSensMin1 = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nSensMin1;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nSensMax1 = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nSensMax1;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nMode2     = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nMode2;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nColor2    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nColor2;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nSensMin2 = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nSensMin2;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].struReserved.nSensMax2 = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].struReserved.nSensMax2;
               }
               else {
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nCons[0] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nCons[0];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nCons[1] = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nCons[1];
                   memcpy(struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].sName, struCnfp.struGroupIdentify[l1][k2].struIntel[i-ARITH_INTEL_A].sName,
                          sizeof(struCnfp.struGroupIdentify[l1][k2].struIntel[i-ARITH_INTEL_A].sName));
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nRow        = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nRow;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nColumn     = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nColumn;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nPercent    = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nPercent;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nSens       = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nSens;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nWholeEdge  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nWholeEdge;
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nCons[0]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nCons[0];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nCons[1]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nCons[1];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuo1[0]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuo1[0];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuo1[1]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuo1[1];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuo2[0]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuo2[0];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuo2[1]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuo2[1];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuo3[0]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuo3[0];
                   struCnfp.struGroupIdentify[l2][k2].struIntel[i-ARITH_INTEL_A].nQuo3[1]  = struCnfp.struGroupIdentify[l1][k1].struIntel[i-ARITH_INTEL_A].nQuo3[1];
               }
               break;
           case ARITH_WATERMELON:
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nEdgeRedSens      = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nEdgeRedSens;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nEdgeRedRatio     = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nEdgeRedRatio;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nEdgeDamagedRatio = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nEdgeDamagedRatio;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nEdgeDamagedSens  = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nEdgeDamagedSens;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nRugged1Sens      = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nRugged1Sens;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nRugged1Save      = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nRugged1Save;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nRugged2Sens      = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nRugged2Sens;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nRugged2Save      = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nRugged2Save;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nWhitePropMin     = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nWhitePropMin;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nWhitePropMax     = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nWhitePropMax;
               struCnfp.struGroupIdentify[l2][k2].struMatMelon.nWhiteSens        = struCnfp.struGroupIdentify[l1][k1].struMatMelon.nWhiteSens;
               break;
           case ARITH_MAIZE:
               struCnfp.struGroupIdentify[l2][k2].struMaize.nRow     = struCnfp.struGroupIdentify[l1][k1].struMaize.nRow;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nColumn     = struCnfp.struGroupIdentify[l1][k1].struMaize.nColumn;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nPercentTotal     = struCnfp.struGroupIdentify[l1][k1].struMaize.nPercentTotal;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nPercentGood     = struCnfp.struGroupIdentify[l1][k1].struMaize.nPercentGood;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nPercentBad1     = struCnfp.struGroupIdentify[l1][k1].struMaize.nPercentBad1;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nPercentBad2     = struCnfp.struGroupIdentify[l1][k1].struMaize.nPercentBad2;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nMode1     = struCnfp.struGroupIdentify[l1][k1].struMaize.nMode1;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nColor1     = struCnfp.struGroupIdentify[l1][k1].struMaize.nColor1;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nSensMin1    = struCnfp.struGroupIdentify[l1][k1].struMaize.nSensMin1;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nSensMax1     = struCnfp.struGroupIdentify[l1][k1].struMaize.nSensMax1;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nMode2     = struCnfp.struGroupIdentify[l1][k1].struMaize.nMode2;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nColor2     = struCnfp.struGroupIdentify[l1][k1].struMaize.nColor2;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nSensProport     = struCnfp.struGroupIdentify[l1][k1].struMaize.nSensProport;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nSensMin2    = struCnfp.struGroupIdentify[l1][k1].struMaize.nSensMin2;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nSensMax2     = struCnfp.struGroupIdentify[l1][k1].struMaize.nSensMax2;
               struCnfp.struGroupIdentify[l2][k2].struMaize.nThreshold = struCnfp.struGroupIdentify[l1][k1].struMaize.nThreshold;
               break;
           case ARITH_CANDY:
               struCnfp.struGroupIdentify[l2][k2].struCandy.nRation1 = struCnfp.struGroupIdentify[l1][k1].struCandy.nRation1;
               struCnfp.struGroupIdentify[l2][k2].struCandy.nRation2 = struCnfp.struGroupIdentify[l1][k1].struCandy.nRation2;
               struCnfp.struGroupIdentify[l2][k2].struCandy.nRow = struCnfp.struGroupIdentify[l1][k1].struCandy.nRow;
               struCnfp.struGroupIdentify[l2][k2].struCandy.nCycles = struCnfp.struGroupIdentify[l1][k1].struCandy.nCycles;
               break;
           case ARITH_PISTACHIO:
               memcpy(struCnfp.struGroupIdentify[l2][k2].struAi.modelId , struCnfp.struGroupIdentify[l1][k1].struAi.modelId,
                      sizeof(struCnfp.struGroupIdentify[l1][k1].struAi.modelId));
               materialAiModelParaCopy(l2,k2,l1,k1);
//               struCnfp.struGroupIdentify[l2][k2].struPistachio.nAreaMin = struCnfp.struGroupIdentify[l1][k1].struPistachio.nAreaMin;
//               struCnfp.struGroupIdentify[l2][k2].struPistachio.nGrayThreshold_1 = struCnfp.struGroupIdentify[l1][k1].struPistachio.nGrayThreshold_1;
//               struCnfp.struGroupIdentify[l2][k2].struPistachio.nGrayThreshold_2 = struCnfp.struGroupIdentify[l1][k1].struPistachio.nGrayThreshold_2;
//               struCnfp.struGroupIdentify[l2][k2].struPistachio.nGrayThreshold_3 = struCnfp.struGroupIdentify[l1][k1].struPistachio.nGrayThreshold_3;
               break;
           default:
               break;
           }
        }
    }
}

void GlobalFlow::materialAiModelParaCopy(int nLevelId, int nGroupId,int nOldLevelId, int nOldGroupId){
    int k2 = nLevelId;
    int l2 = nGroupId;
    int k1 = nOldLevelId;
    int l1 = nOldGroupId;
    QString modelId =  QString::fromUtf8(struCnfp.struGroupIdentify[l2][k2].struAi.modelId);
    QString threshold,isApply,id;
    QSqlQuery query;
    query.prepare("SELECT modelId, id, zhName, enName, threshold, isApply, chgTime FROM modelParaInfo  "
                        "where modelId = ?  and levelTotal = ? and identifyGroupTotal = ? order by id asc");

    query.bindValue(0,modelId);
    query.bindValue(1,l1);
    query.bindValue(2,k1);
    int modeParaCount = 0;

    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError();
    } else {
        while (query.next()) {
            id = query.value(1).toString();
            threshold = query.value(4).toString();
            isApply = query.value(5).toString();
            modeParaCount++;

            QSqlQuery sql_update;
            sql_update.prepare("update  modelParaInfo  set threshold = ? , isApply = ? WHERE modelId = ? and id =? and levelTotal = ? and identifyGroupTotal = ? ");
            sql_update.bindValue(2, modelId);
            sql_update.bindValue(4, l2);
            sql_update.bindValue(5, k2);

            sql_update.bindValue(0, threshold);
            sql_update.bindValue(1, isApply);
            sql_update.bindValue(3, id);
            if(!sql_update.exec()){
                qDebug() << "update modelParaInfo 失败"<< sql_update.lastError();;
            }
        }
    }
}

/***************************************************************************************************
    名称：         识别组复制编号确定
    功能：         根据物料参数设置方式，计算获取物料参数需要复制的组编号
    nLevelId:     识别组所在的层编号
    nGroupId：    识别组的组编号
    bIsAllSens：  判断是复制灵敏度设置界面中的所有算法的灵敏度，还是方案设置中某个识别算法的参数
    nArithSeq：   识别算法序号
    bIsReserved:  是否是复制保留参数
***************************************************************************************************/
void GlobalFlow::materialCopyAssemble(int nLevelId, int nGroupId, int bIsAllSens, int nArithSeq, int bIsReserved)
{
    int caseTmp = struCnfp.nMatAssembleMode;
    switch (struCnfe.nMachine) {
    case MACHINE_CF:
        switch (nGroupId) {
        case 0: // first_front
            switch (caseTmp) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                materialParamsCopy(0, 0, 0, 1, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
                materialParamsCopy(0, 0, 0, 2, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 4, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 6, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_ALL_SAME:
                materialParamsCopy(0, 0, 0, 1, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 2, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 3, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 4, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 5, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 6, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 0, 0, 7, bIsAllSens, nArithSeq, bIsReserved);
                break;
            }
            break;
        case 1: // first_rear
            switch (struCnfp.nMatAssembleMode) {
            case PARAMS_ALL_SEPARATE:
            case PARAMS_FRONT_REAR_SAME:
                break;
            case PARAMS_FIRST_SECOND_SAME:
                materialParamsCopy(0, 1, 0, 3, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 1, 0, 5, bIsAllSens, nArithSeq, bIsReserved);
                materialParamsCopy(0, 1, 0, 7, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_ALL_SAME:
                break;
            }
            break;
        case 2: // second_front
            switch (struCnfp.nMatAssembleMode) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                materialParamsCopy(0, 2, 0, 3, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
            case PARAMS_ALL_SAME:
                break;
            }
            break;
        case 3: // second_rear
            break;
        case 4: // third_front
            switch (struCnfp.nMatAssembleMode) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                materialParamsCopy(0, 4, 0, 5, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
            case PARAMS_ALL_SAME:
                break;
            }
            break;
        case 5: // third_rear
            break;
	case 6: // fourth_front
            switch (struCnfp.nMatAssembleMode) {
            case PARAMS_ALL_SEPARATE:
                break;
            case PARAMS_FRONT_REAR_SAME:
                materialParamsCopy(0, 6, 0, 7, bIsAllSens, nArithSeq, bIsReserved);
                break;
            case PARAMS_FIRST_SECOND_SAME:
            case PARAMS_ALL_SAME:
                break;
            }
	    break;
	case 7: // fourth_rear
	    break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/*!
 * \brief GlobalFlow::materialResetGreyRsvPer
 * \param nLevelId 对应层
 * \param nGroupId 对应识别组
 * \param nArithSeq 对应算法序号
 */
void GlobalFlow::materialResetGreyRsvPer(int nLevelId, int nGroupId, int nUnit, int nArithSeq)
{
    int nRow = struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nArithSeq].nRow;
//    int nCol = struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nArithSeq].nColumn;
    int nCol = 16;
    int nPer = struCnfp.struGroupIdentify[nLevelId][nGroupId].struGreyColor[nArithSeq].struReserved.nPercent;

    int nRsvArea = nRow*nCol*((float)(100-nPer)/100.0) + 0.5;
    MySerial.com1Write(CMD_UNIT_RESERVED_PERCENT, UNIT, nLevelId, nUnit, 0, 0, nArithSeq+1, nRsvArea/256, nRsvArea%256, 3);
}

/*!
 * \brief GlobalFlow::materialResetIntelRsvPer
 * \param nLevelId 对应层
 * \param nGroupId 对应识别组
 * \param nUnit 相机板地址
 */
void GlobalFlow::materialResetIntelRsvPer(int nLevelId, int nGroupId, int nUnit)
{
    int nRow = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nRow;
//    int nCol = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].nColumn;
    int nCol = 16;
    int nPer = struCnfp.struGroupIdentify[nLevelId][nGroupId].struIntel[0].struReserved.nPercent;

    int nRsvArea = nRow*nCol*((float)(100-nPer)/100.0) + 0.5;
    MySerial.com1Write(CMD_UNIT_RESERVED_PERCENT, UNIT, nLevelId, nUnit, 0, 0, 5, nRsvArea/256, nRsvArea%256, 3);
}
