#include "globalflow.h"

/* 从相机配置文件获取相机参数 */
bool GlobalFlow::getCameraSetting(const QString & fileName)
{
    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

/* get parameters */
    /* level camera details */
    for (int i = 0; i < MAX_LEVEL; i++) {
        for (int j = 0; j < MAX_UNIT; j++) {
            /* channel begin : color camera */
            struCnfc.struLevelCamera[i].nChannelBegin[j] =
                    setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(0).arg(j), struCnfc.struLevelCamera[i].nChannelBegin[j]).toInt();
            /* channel end : color camera */
            struCnfc.struLevelCamera[i].nChannelEnd[j] =
                    setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(1).arg(j), struCnfc.struLevelCamera[i].nChannelEnd[j]).toInt();

            /*------------------------------------- 玉米机型 象元范围 ------------------------------------------*/
            for (int k = 0; k < 2; k++) {
                struCnfc.struLevelCamera[i].nMaizeChannelBegin[j][k] =
                        setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(2).arg(0).arg(0).arg(0).arg(i).arg(j).arg(k), struCnfc.struLevelCamera[i].nMaizeChannelBegin[j][k]).toInt();
                /* channel end : color camera */
                struCnfc.struLevelCamera[i].nMaizeChannelEnd[j][k] =
                        setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(2).arg(0).arg(0).arg(1).arg(i).arg(j).arg(k), struCnfc.struLevelCamera[i].nMaizeChannelEnd[j][k]).toInt();
            }

            /* channel ejector begin */
            for (int k = 0; k < MAX_EJECTOR; k++) {
             struCnfc.struLevelCamera[i].nChannelEjectorBegin[j][k] = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(2).arg(0).arg(i).arg(2).arg(j).arg(k), struCnfc.struLevelCamera[i].nChannelEjectorBegin[j][k]).toInt();
            }

            /* channel ejector end */
            for (int k = 0; k < MAX_EJECTOR; k++) {
             struCnfc.struLevelCamera[i].nChannelEjectorEnd[j][k] = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(2).arg(0).arg(i).arg(3).arg(j).arg(k), struCnfc.struLevelCamera[i].nChannelEjectorEnd[j][k]).toInt();
            }

            /* biasing : red */
            struCnfc.struLevelCamera[i].nBiasingRed[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(4).arg(j), struCnfc.struLevelCamera[i].nBiasingRed[j]).toInt();

            /* biasing : green */
            struCnfc.struLevelCamera[i].nBiasingGreen[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(5).arg(j), struCnfc.struLevelCamera[i].nBiasingGreen[j]).toInt();

            /* biasing : blue */
            struCnfc.struLevelCamera[i].nBiasingBlue[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(6).arg(j), struCnfc.struLevelCamera[i].nBiasingBlue[j]).toInt();

            /* analog gain : red*/
            struCnfc.struLevelCamera[i].nAnaGainRed[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(7).arg(j), struCnfc.struLevelCamera[i].nAnaGainRed[j]).toInt();

            /* analog gain : green */
            struCnfc.struLevelCamera[i].nAnaGainGreen[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(8).arg(j), struCnfc.struLevelCamera[i].nAnaGainGreen[j]).toInt();

            /* analog gain : blue */
            struCnfc.struLevelCamera[i].nAnaGainBlue[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(9).arg(j), struCnfc.struLevelCamera[i].nAnaGainBlue[j]).toInt();

            /* digit gain : upper red */
            struCnfc.struLevelCamera[i].nDigiGainUpperRed[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(10).arg(j), struCnfc.struLevelCamera[i].nDigiGainUpperRed[j]).toInt();

            /* digit gian : upper green */
            struCnfc.struLevelCamera[i].nDigiGainUpperGreen[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(11).arg(j), struCnfc.struLevelCamera[i].nDigiGainUpperGreen[j]).toInt();

            /* digit gain : upper blue */
            struCnfc.struLevelCamera[i].nDigiGainUpperBlue[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(12).arg(j), struCnfc.struLevelCamera[i].nDigiGainUpperBlue[j]).toInt();

            /* digit gain : lower red */
            struCnfc.struLevelCamera[i].nDigiGainLowRed[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(13).arg(j), struCnfc.struLevelCamera[i].nDigiGainLowRed[j]).toInt();

            /* digit gian : lower green */
            struCnfc.struLevelCamera[i].nDigiGainLowGreen[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(14).arg(j), struCnfc.struLevelCamera[i].nDigiGainLowGreen[j]).toInt();

            /* digit gain : lower blue */
            struCnfc.struLevelCamera[i].nDigiGainLowBlue[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(15).arg(j), struCnfc.struLevelCamera[i].nDigiGainLowBlue[j]).toInt();

            /* ejector begin */
            struCnfc.struLevelCamera[i].nEjectorBegin[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(25).arg(j), struCnfc.struLevelCamera[i].nEjectorBegin[j]).toInt();

            /* ejector end */
            struCnfc.struLevelCamera[i].nEjectorEnd[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(26).arg(j), struCnfc.struLevelCamera[i].nEjectorEnd[j]).toInt();

            /* 三次反选中对位测试相关参数*/
            struCnfc.struLevelCamera[i].nContraPosBias[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(29).arg(j), struCnfc.struLevelCamera[i].nContraPosBias[j]).toInt();
            struCnfc.struLevelCamera[i].nContraPosCorrect[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(30).arg(j), struCnfc.struLevelCamera[i].nContraPosCorrect[j]).toInt();
        }
    }

    /* red reference */
    struCnfc.nRefRed = setting.value(QString("%1-%2").arg(2).arg(3), struCnfc.nRefRed).toInt();

    /* green reference */
    struCnfc.nRefGreen = setting.value(QString("%1-%2").arg(2).arg(4), struCnfc.nRefGreen).toInt();

    /* blue reference */
    struCnfc.nRefBlue = setting.value(QString("%1-%2").arg(2).arg(5), struCnfc.nRefBlue).toInt();

    /* sensor type */
    struCnfc.nSensorType = setting.value(QString("%1-%2").arg(2).arg(6), struCnfc.nSensorType).toInt();

    /* sensor row frequency */
    struCnfc.nSensorRowFre = setting.value(QString("%1-%2").arg(2).arg(7), struCnfc.nSensorRowFre).toInt();

    /* lens type */
    struCnfc.nLensType = setting.value(QString("%1-%2").arg(2).arg(8), struCnfc.nLensType).toInt();

    /* sensor resolution */
    struCnfc.nSensorPixel = setting.value(QString("%1-%2").arg(2).arg(9), struCnfc.nSensorPixel).toInt();

    /* 象元范围 */
    struCnfc.nChannelRangeBegin = setting.value(QString("%1-%2").arg(2).arg(10),
                        struCnfc.nChannelRangeBegin).toInt();
    struCnfc.nChannelRangeEnd = setting.value(QString("%1-%2").arg(2).arg(11),
                        struCnfc.nChannelRangeEnd).toInt();
    struCnfc.nFrontExpTime = setting.value(QString("%1-%2").arg(2).arg(12),
                        struCnfc.nFrontExpTime).toInt();
    struCnfc.nRearExpTime = setting.value(QString("%1-%2").arg(2).arg(13),
                        struCnfc.nRearExpTime).toInt();

    /* sensor resolution by level */
    for (int i = 0; i < MAX_LEVEL; i++) {
        struCnfc.nSensorPixelLevel[i] = setting.value(QString("%1-%2-%3").arg(2).arg(14).arg(i), struCnfc.nSensorPixelLevel[i]).toInt();
    }

    struCnfc.nPMTFrameFront = setting.value(QString("%1-%2").arg(2).arg(15),
                        struCnfc.nPMTFrameFront).toInt();

    struCnfc.nPMTFrameRear = setting.value(QString("%1-%2").arg(2).arg(16),
                        struCnfc.nPMTFrameRear).toInt();

    /* rear red reference */
    struCnfc.nRearRefRed = setting.value(QString("%1-%2").arg(2).arg(17), struCnfc.nRearRefRed).toInt();

    /* rear green reference */
    struCnfc.nRearRefGreen = setting.value(QString("%1-%2").arg(2).arg(18), struCnfc.nRearRefGreen).toInt();

    /* rear blue reference */
    struCnfc.nRearRefBlue = setting.value(QString("%1-%2").arg(2).arg(19), struCnfc.nRearRefBlue).toInt();

    /* return value */
    if (setting.status() == QSettings::NoError)
        return true;
    else
        return false;
}

/* 保存相机参数至相机配置文件 */
bool GlobalFlow::saveCameraSetting(const QString & fileName)
{
/* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

/* save parameters */
    /* level camera details */
    for (int i = 0; i < MAX_LEVEL; i++) {
        for (int j = 0; j < MAX_UNIT; j++) {
            /* channel begin : color camera */
            if(struCnfc.struLevelCamera[i].nChannelBegin[j] != _t_struCnfc.struLevelCamera[i].nChannelBegin[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(0).arg(j), struCnfc.struLevelCamera[i].nChannelBegin[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) channel begin:%d",i,j,struCnfc.struLevelCamera[i].nChannelBegin[j]);
            }

            /* channel end : color camera */
            if(struCnfc.struLevelCamera[i].nChannelEnd[j] != _t_struCnfc.struLevelCamera[i].nChannelEnd[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(1).arg(j), struCnfc.struLevelCamera[i].nChannelEnd[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) channel end:%d",i,j,struCnfc.struLevelCamera[i].nChannelEnd[j]);
            }

            /*------------------------------------- 玉米机型 象元范围 ------------------------------------------*/
            for (int k = 0; k < 2; k++) {
                if(struCnfc.struLevelCamera[i].nMaizeChannelBegin[j][k] != _t_struCnfc.struLevelCamera[i].nMaizeChannelBegin[j][k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(2).arg(0).arg(0).arg(0).arg(i).arg(j).arg(k), struCnfc.struLevelCamera[i].nMaizeChannelBegin[j][k]);
                    myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) channel begin:%d",i,j,struCnfc.struLevelCamera[i].nMaizeChannelBegin[j][k]);
                }
                if(struCnfc.struLevelCamera[i].nMaizeChannelEnd[j][k] != _t_struCnfc.struLevelCamera[i].nMaizeChannelEnd[j][k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(2).arg(0).arg(0).arg(1).arg(i).arg(j).arg(k), struCnfc.struLevelCamera[i].nMaizeChannelEnd[j][k]);
                    myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) channel end:%d",i,j,struCnfc.struLevelCamera[i].nMaizeChannelEnd[j][k]);
                }
            }

            /* channel ejector begin */
            for (int k = 0; k < MAX_EJECTOR; k++) {
                if(struCnfc.struLevelCamera[i].nChannelEjectorBegin[j][k] != _t_struCnfc.struLevelCamera[i].nChannelEjectorBegin[j][k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(2).arg(0).arg(i).arg(2).arg(j).arg(k), struCnfc.struLevelCamera[i].nChannelEjectorBegin[j][k]);
                }
            }

            /* channel ejector end */
            for (int k = 0; k < MAX_EJECTOR; k++) {
                if(struCnfc.struLevelCamera[i].nChannelEjectorEnd[j][k] != _t_struCnfc.struLevelCamera[i].nChannelEjectorEnd[j][k]) {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(2).arg(0).arg(i).arg(3).arg(j).arg(k), struCnfc.struLevelCamera[i].nChannelEjectorEnd[j][k]);
                }
            }

            /* biasing : red */
            if(struCnfc.struLevelCamera[i].nBiasingRed[j] != _t_struCnfc.struLevelCamera[i].nBiasingRed[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(4).arg(j), struCnfc.struLevelCamera[i].nBiasingRed[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) red bias:%d",i,j,struCnfc.struLevelCamera[i].nBiasingRed[j]);
            }

            /* biasing : green */
            if(struCnfc.struLevelCamera[i].nBiasingGreen[j] != _t_struCnfc.struLevelCamera[i].nBiasingGreen[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(5).arg(j), struCnfc.struLevelCamera[i].nBiasingGreen[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) green bias:%d",i,j,struCnfc.struLevelCamera[i].nBiasingGreen[j]);
            }

            /* biasing : blue */
            if(struCnfc.struLevelCamera[i].nBiasingBlue[j] != _t_struCnfc.struLevelCamera[i].nBiasingBlue[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(6).arg(j), struCnfc.struLevelCamera[i].nBiasingBlue[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) blue bias:%d",i,j,struCnfc.struLevelCamera[i].nBiasingBlue[j]);
            }

            /* analog gain : red*/
            if(struCnfc.struLevelCamera[i].nAnaGainRed[j] != _t_struCnfc.struLevelCamera[i].nAnaGainRed[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(7).arg(j), struCnfc.struLevelCamera[i].nAnaGainRed[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) red analog gain:%d",i,j,struCnfc.struLevelCamera[i].nAnaGainRed[j]);
            }

            /* analog gain : green */
            if(struCnfc.struLevelCamera[i].nAnaGainGreen[j] != _t_struCnfc.struLevelCamera[i].nAnaGainGreen[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(8).arg(j), struCnfc.struLevelCamera[i].nAnaGainGreen[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) green analog gain:%d",i,j,struCnfc.struLevelCamera[i].nAnaGainGreen[j]);
            }

            /* analog gain : blue */
            if(struCnfc.struLevelCamera[i].nAnaGainBlue[j] != _t_struCnfc.struLevelCamera[i].nAnaGainBlue[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(9).arg(j), struCnfc.struLevelCamera[i].nAnaGainBlue[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) blue analog gain:%d",i,j,struCnfc.struLevelCamera[i].nAnaGainBlue[j]);
            }

            /* digit gain : upper red */
            if(struCnfc.struLevelCamera[i].nDigiGainUpperRed[j] != _t_struCnfc.struLevelCamera[i].nDigiGainUpperRed[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(10).arg(j), struCnfc.struLevelCamera[i].nDigiGainUpperRed[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) red dig gain(high byte):%d",i,j,struCnfc.struLevelCamera[i].nDigiGainUpperRed[j]);
            }

            /* digit gian : upper green */
            if(struCnfc.struLevelCamera[i].nDigiGainUpperGreen[j] != _t_struCnfc.struLevelCamera[i].nDigiGainUpperGreen[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(11).arg(j), struCnfc.struLevelCamera[i].nDigiGainUpperGreen[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) green dig gain(high byte):%d",i,j,struCnfc.struLevelCamera[i].nDigiGainUpperGreen[j]);
            }

            /* digit gain : upper blue */
            if(struCnfc.struLevelCamera[i].nDigiGainUpperBlue[j] != _t_struCnfc.struLevelCamera[i].nDigiGainUpperBlue[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(12).arg(j), struCnfc.struLevelCamera[i].nDigiGainUpperBlue[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) blue dig gain(high byte):%d",i,j,struCnfc.struLevelCamera[i].nDigiGainUpperBlue[j]);
            }

            /* digit gain : lower red */
            if(struCnfc.struLevelCamera[i].nDigiGainLowRed[j] != _t_struCnfc.struLevelCamera[i].nDigiGainLowRed[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(13).arg(j), struCnfc.struLevelCamera[i].nDigiGainLowRed[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) red dig gain(low byte):%d",i,j,struCnfc.struLevelCamera[i].nDigiGainLowRed[j]);
            }

            /* digit gian : lower green */
            if(struCnfc.struLevelCamera[i].nDigiGainLowGreen[j] != _t_struCnfc.struLevelCamera[i].nDigiGainLowGreen[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(14).arg(j), struCnfc.struLevelCamera[i].nDigiGainLowGreen[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) green dig gain(low byte):%d",i,j,struCnfc.struLevelCamera[i].nDigiGainLowGreen[j]);
            }

            /* digit gain : lower blue */
            if(struCnfc.struLevelCamera[i].nDigiGainLowBlue[j] != _t_struCnfc.struLevelCamera[i].nDigiGainLowBlue[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(15).arg(j), struCnfc.struLevelCamera[i].nDigiGainLowBlue[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) blue dig gain(low byte):%d",i,j,struCnfc.struLevelCamera[i].nDigiGainLowBlue[j]);
            }

            /* ejector begin */
            if(struCnfc.struLevelCamera[i].nEjectorBegin[j] != _t_struCnfc.struLevelCamera[i].nEjectorBegin[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(25).arg(j), struCnfc.struLevelCamera[i].nEjectorBegin[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) ejector start:%d",i,j,struCnfc.struLevelCamera[i].nEjectorBegin[j]);
            }

            /* ejector end */
            if(struCnfc.struLevelCamera[i].nEjectorEnd[j] != _t_struCnfc.struLevelCamera[i].nEjectorEnd[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(26).arg(j), struCnfc.struLevelCamera[i].nEjectorEnd[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) ejector end:%d",i,j,struCnfc.struLevelCamera[i].nEjectorEnd[j]);
            }

            /* 三次反选对位测试参数*/
            if(struCnfc.struLevelCamera[i].nContraPosBias[j] != _t_struCnfc.struLevelCamera[i].nContraPosBias[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(29).arg(j), struCnfc.struLevelCamera[i].nContraPosBias[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) contraPosBias:%d",i,j,struCnfc.struLevelCamera[i].nContraPosBias[j]);
            }

            if(struCnfc.struLevelCamera[i].nContraPosCorrect[j] != _t_struCnfc.struLevelCamera[i].nContraPosCorrect[j]) {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(2).arg(0).arg(i).arg(30).arg(j), struCnfc.struLevelCamera[i].nContraPosCorrect[j]);
                myLog->info(LOG_CAMERA,"camera(level:%d,camera No.:%d) contraPosBias:%d",i,j,struCnfc.struLevelCamera[i].nContraPosCorrect[j]);
            }

        }
    }

    /* red reference */
    if(struCnfc.nRefRed != _t_struCnfc.nRefRed) {
        setting.setValue(QString("%1-%2").arg(2).arg(3), struCnfc.nRefRed);
        myLog->info(LOG_CAMERA,"camera red reference:%d",struCnfc.nRefRed);
    }

    /* green reference */
    if(struCnfc.nRefGreen != _t_struCnfc.nRefGreen) {
        setting.setValue(QString("%1-%2").arg(2).arg(4), struCnfc.nRefGreen);
        myLog->info(LOG_CAMERA,"camera green reference:%d",struCnfc.nRefGreen);
    }

    /* blue reference */
    if(struCnfc.nRefBlue != _t_struCnfc.nRefBlue) {
        setting.setValue(QString("%1-%2").arg(2).arg(5), struCnfc.nRefBlue);
        myLog->info(LOG_CAMERA,"camera blue reference:%d",struCnfc.nRefBlue);
    }

    /* sensor type */
    if(struCnfc.nSensorType != _t_struCnfc.nSensorType) {
        setting.setValue(QString("%1-%2").arg(2).arg(6), struCnfc.nSensorType);
        myLog->info(LOG_CAMERA,"camera blue reference:%d",struCnfc.nRefBlue);
    }

    /* sensor row frequency */
    if(struCnfc.nSensorRowFre != _t_struCnfc.nSensorRowFre) {
        setting.setValue(QString("%1-%2").arg(2).arg(7), struCnfc.nSensorRowFre);
        myLog->info(LOG_CAMERA,"camera sensor row frequency:%d",struCnfc.nSensorRowFre);
    }
    /* lens type */
    if(struCnfc.nLensType != _t_struCnfc.nLensType) {
        setting.setValue(QString("%1-%2").arg(2).arg(8), struCnfc.nLensType);
        myLog->info(LOG_CAMERA,"camera lens type:%d",struCnfc.nLensType);
    }

    /* sensor resolution */
    if(struCnfc.nSensorPixel != _t_struCnfc.nSensorPixel) {
        setting.setValue(QString("%1-%2").arg(2).arg(9), struCnfc.nSensorPixel);
        myLog->info(LOG_CAMERA,"camera sensor pixel:%d",struCnfc.nSensorPixel);
    }

    /* 象元范围 */
    if(struCnfc.nChannelRangeBegin != _t_struCnfc.nChannelRangeBegin) {
        setting.setValue(QString("%1-%2").arg(2).arg(10), struCnfc.nChannelRangeBegin);
        myLog->info(LOG_CAMERA,"camera channel range begin:%d",struCnfc.nChannelRangeBegin);
    }
    if(struCnfc.nChannelRangeEnd != _t_struCnfc.nChannelRangeEnd) {
        setting.setValue(QString("%1-%2").arg(2).arg(11), struCnfc.nChannelRangeEnd);
        myLog->info(LOG_CAMERA,"camera channel range end:%d",struCnfc.nChannelRangeEnd);
    }
    if(struCnfc.nFrontExpTime != _t_struCnfc.nFrontExpTime) {
        setting.setValue(QString("%1-%2").arg(2).arg(12), struCnfc.nFrontExpTime);
        myLog->info(LOG_CAMERA,"camera front exptime:%d",struCnfc.nFrontExpTime);
    }
    if(struCnfc.nRearExpTime != _t_struCnfc.nRearExpTime) {
        setting.setValue(QString("%1-%2").arg(2).arg(13), struCnfc.nRearExpTime);
        myLog->info(LOG_CAMERA,"camera rear exptime:%d",struCnfc.nRearExpTime);
    }

    /* sensor resolution by level */
    for (int i = 0; i < MAX_LEVEL; i++) {
        if(struCnfc.nSensorPixelLevel[i] != _t_struCnfc.nSensorPixelLevel[i]) {
            setting.setValue(QString("%1-%2-%3").arg(2).arg(14).arg(i), struCnfc.nSensorPixelLevel[i]);
            myLog->info(LOG_CAMERA,"camera(level:%d) sensor pixel:%d",i,struCnfc.nSensorPixelLevel[i]);
        }
    }
    /*PMT*/
    if(struCnfc.nPMTFrameFront != _t_struCnfc.nPMTFrameFront) {
        setting.setValue(QString("%1-%2").arg(2).arg(15), struCnfc.nPMTFrameFront);
        myLog->info(LOG_CAMERA,"camera PMT front frame begin:%d",struCnfc.nPMTFrameFront);
    }
    if(struCnfc.nPMTFrameRear != _t_struCnfc.nPMTFrameRear) {
        setting.setValue(QString("%1-%2").arg(2).arg(16), struCnfc.nPMTFrameRear);
        myLog->info(LOG_CAMERA,"camera PMT rear frame begin:%d",struCnfc.nPMTFrameRear);
    }

    /* rear red reference */
    if(struCnfc.nRearRefRed != _t_struCnfc.nRearRefRed) {
        setting.setValue(QString("%1-%2").arg(2).arg(17), struCnfc.nRearRefRed);
        myLog->info(LOG_CAMERA,"camera rear red reference:%d",struCnfc.nRearRefRed);
    }

    /* rear green reference */
    if(struCnfc.nRearRefGreen != _t_struCnfc.nRearRefGreen) {
        setting.setValue(QString("%1-%2").arg(2).arg(18), struCnfc.nRearRefGreen);
        myLog->info(LOG_CAMERA,"camera rear green reference:%d",struCnfc.nRearRefGreen);
    }

    /* rear blue reference */
    if(struCnfc.nRearRefBlue != _t_struCnfc.nRearRefBlue) {
        setting.setValue(QString("%1-%2").arg(2).arg(19), struCnfc.nRearRefBlue);
        myLog->info(LOG_CAMERA,"camera rearblue reference:%d",struCnfc.nRearRefBlue);
    }

/* return value */
    if (setting.status() == QSettings::NoError) {
        return true;
    }
    return false;
}

/* 获取相机参数 */
void GlobalFlow::getCamera()
{
    int i, j, k;
    int nUnitAddr;
    QFile file;
    int nSensorType, nSensorPixel, nSensorRowFre;

    memset(&struCnfc, 0, sizeof(struct struCnfCamera));
    memset(&_t_struCnfc, 0, sizeof(struct struCnfCamera));

    nSensorType   = SENSOR_T_2564;   // CG 机型使用 T_2564 彩色传感器
    nSensorPixel  = SENSOR_PIXELS_1024;
    nSensorRowFre = 38;

    struCnfc.nLensType  = 0;

    struCnfc.nSensorType = nSensorType;
    struCnfc.nSensorRowFre = nSensorRowFre;
    struCnfc.nSensorPixel = nSensorPixel;
    for (int i = 0; i < MAX_LEVEL; i++) {
        struCnfc.nSensorPixelLevel[i] = nSensorPixel;
    }

    for (i = 0; i < struCnfg.nLevelTotal; i++) {
        for (j = 0; j < MAX_UNIT; j++) {
            nUnitAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr] = 100;
            struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr] = 900;
            struCnfc.struLevelCamera[i].nEjectorBegin[nUnitAddr] = 0;
            struCnfc.struLevelCamera[i].nEjectorEnd[nUnitAddr] = struCnfg.nEjectorsPerChute-1;

//            for (k = 0; k < struCnfg.nEjectorsPerChute; k++) {
//                struCnfc.struLevelCamera[i].nChannelEjectorBegin[nUnitAddr][k] = 100;
//                struCnfc.struLevelCamera[i].nChannelEjectorEnd[nUnitAddr][k] = 900;
//            }

            myFlow.calDivChannelParams(struCnfc.struLevelCamera[i].nChannelBegin[nUnitAddr],
                                       struCnfc.struLevelCamera[i].nChannelEnd[nUnitAddr],i,nUnitAddr);

            struCnfc.struLevelCamera[i].nAnaGainRed[nUnitAddr] = 0;
            struCnfc.struLevelCamera[i].nAnaGainGreen[nUnitAddr] = 0;
            struCnfc.struLevelCamera[i].nAnaGainBlue[nUnitAddr] = 0;

            if (struCnfc.nSensorType == SENSOR_D2) {
                struCnfc.struLevelCamera[i].nBiasingRed[nUnitAddr] = 128;
                struCnfc.struLevelCamera[i].nBiasingGreen[nUnitAddr] = 128;
                struCnfc.struLevelCamera[i].nBiasingBlue[nUnitAddr] = 128;
            } else {
                struCnfc.struLevelCamera[i].nBiasingRed[nUnitAddr] = 0;
                struCnfc.struLevelCamera[i].nBiasingGreen[nUnitAddr] = 0;
                struCnfc.struLevelCamera[i].nBiasingBlue[nUnitAddr] = 0;
            }

            struCnfc.struLevelCamera[i].nDigiGainUpperRed[nUnitAddr]  = 3;
            struCnfc.struLevelCamera[i].nDigiGainUpperGreen[nUnitAddr] = 3;
            struCnfc.struLevelCamera[i].nDigiGainUpperBlue[nUnitAddr] = 3;
            struCnfc.struLevelCamera[i].nDigiGainLowRed[nUnitAddr] = 0;
            struCnfc.struLevelCamera[i].nDigiGainLowGreen[nUnitAddr] = 0;
            struCnfc.struLevelCamera[i].nDigiGainLowBlue[nUnitAddr] = 0;
        }
    }

    struCnfc.nRefRed = 150;
    struCnfc.nRefGreen = 150;
    struCnfc.nRefBlue = 150;

    struCnfc.nChannelRangeBegin = 40;
    if (struCnfc.nSensorPixel == SENSOR_PIXELS_2048) {
        struCnfc.nChannelRangeEnd = 2000;
    } else {
        struCnfc.nChannelRangeEnd = 999;
    }
    struCnfc.nFrontExpTime = 0;
    struCnfc.nRearExpTime = 0;

    struCnfc.nPMTFrameFront = 128;
    struCnfc.nPMTFrameRear  = 128;

    memcpy(&_t_struCnfc, &struCnfc, sizeof(struCnfCamera));

    if (struCnfs.value != CAMER_FILE_ERROR) {
        if (QFileInfo(CNF_CAMERA).exists() && QFileInfo(CNF_CAMERA).size() > 0)  {
            if (!getCameraSetting(CNF_CAMERA)) {
                qDebug("getCamera(): can not get camera parameters, using default");
                return;
            }
            file.remove(CNF_CAMERA_B);
            file.copy(CNF_CAMERA, CNF_CAMERA_B);
        }
        memcpy(&_t_struCnfc, &struCnfc, sizeof(struCnfCamera));
    }
    else {
        if (QFileInfo(CNF_CAMERA_B).exists() && QFileInfo(CNF_CAMERA_B).size() > 0) {
            if (!getCameraSetting(CNF_CAMERA_B)) {
                qDebug("getCamera(): can not get camera parameters, using default");
                return;
            }
            file.remove(CNF_CAMERA);
            file.copy(CNF_CAMERA_B, CNF_CAMERA);
        }
        memcpy(&_t_struCnfc, &struCnfc, sizeof(struCnfCamera));
    }

    return;
}

/* 保存相机参数 */
bool GlobalFlow::saveCamera()
{
    int i;
    QFile file;

    saveParamsFileStaus(CAMER_FILE_ERROR);
    for (i = 0; i < 3; i++) {
        if (saveCameraSetting(CNF_CAMERA)) {
            saveParamsFileStaus(ALL_PARAMS_FILE_OK);
            break;
        }
    }

    if (i == 3)
        return false;

    if (QFileInfo(CNF_CAMERA).exists() && QFileInfo(CNF_CAMERA).size() > 0) {
        file.remove(CNF_CAMERA_B);
        file.copy(CNF_CAMERA, CNF_CAMERA_B);
    }

    memcpy(&_t_struCnfc, &struCnfc, sizeof(struCnfCamera));     // 更新_t_struCnfc
    return true;
}

/* 计算象元划分的参数 */
void GlobalFlow::calDivChannelParams(int beginCh, int endCh, int nLevel, int nUnit)
{
    double ave;
    int min, max, range;

    struCnfc.struLevelCamera[nLevel].nChannelBegin[nUnit] = beginCh;
    struCnfc.struLevelCamera[nLevel].nChannelEnd[nUnit] = endCh;

    min = struCnfc.struLevelCamera[nLevel].nEjectorBegin[nUnit];
    max =   struCnfc.struLevelCamera[nLevel].nEjectorEnd[nUnit];
    range = qAbs(max-min)+1;
    ave = (endCh-beginCh)/(double)range;

    // 重置喷嘴像元参数
    for (int i = 0; i < struCnfg.nEjectorsPerChute; i++) {
        struCnfc.struLevelCamera[nLevel].nChannelEjectorBegin[nUnit][i] = 0;
        struCnfc.struLevelCamera[nLevel].nChannelEjectorEnd[nUnit][i] = 0;
    }
    // 动态调整喷嘴像元
    for (int i = 0; i < range; i++) {
        int id = i+min;
        struCnfc.struLevelCamera[nLevel].nChannelEjectorBegin[nUnit][id] = struCnfc.struLevelCamera[nLevel].nChannelBegin[nUnit]+i*ave+struGsh.fPixelCompensate[id]+0.5;
        if (i > 0) {
            struCnfc.struLevelCamera[nLevel].nChannelEjectorEnd[nUnit][id-1] = struCnfc.struLevelCamera[nLevel].nChannelEjectorBegin[nUnit][id]-1;
        }
    }
    struCnfc.struLevelCamera[nLevel].nChannelEjectorEnd[nUnit][max] = struCnfc.struLevelCamera[nLevel].nChannelEnd[nUnit];

//    /* 通道划分 */
//    MySerial.com1Write(CMD_UNIT_CHANNEL, UNIT, nLevel, nUnit, 0,
//        struCnfc.struLevelCamera[nLevel].nChannelBegin[nUnit]/256,
//        struCnfc.struLevelCamera[nLevel].nChannelBegin[nUnit]%256,
//        struCnfc.struLevelCamera[nLevel].nChannelEnd[nUnit]/256,
//        struCnfc.struLevelCamera[nLevel].nChannelEnd[nUnit]%256, 3);
//    /* 喷嘴象元 */
//    for (int i = 0; i < struCnfg.nEjectorsPerChute; i++) {
//        MySerial.com1Write(CMD_UNIT_CHANNEL_EJECTOR, UNIT, nLevel, nUnit, i,
//            struCnfc.struLevelCamera[nLevel].nChannelEjectorBegin[nUnit][i]/256,
//            struCnfc.struLevelCamera[nLevel].nChannelEjectorBegin[nUnit][i]%256,
//            struCnfc.struLevelCamera[nLevel].nChannelEjectorEnd[nUnit][i]/256,
//            struCnfc.struLevelCamera[nLevel].nChannelEjectorEnd[nUnit][i]%256, 3);
//    }
}
