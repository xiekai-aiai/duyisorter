#include "globalflow.h"
#include "unilog.h"

/* 从色选方案参数文件获取方案参数 */
bool GlobalFlow::getProfileSetting(const QString& fileName)
{
    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);

    /* get parameters from setting file */
    /* interface board mode */
    struCnfp.nInterfaceBoardMode = setting.value(QString("%1-%2").arg(3).arg(0), struCnfp.nInterfaceBoardMode).toInt();

    /* color board mode */
    struCnfp.nColorBoardMode = setting.value(QString("%1-%2").arg(3).arg(1), struCnfp.nColorBoardMode).toInt();

    /* intel sequence */
    struCnfp.nMatIntelSeq = setting.value(QString("%1-%2").arg(3).arg(3), struCnfp.nMatIntelSeq).toInt();

    /* material assemble mode */
    struCnfp.nMatAssembleMode = setting.value(QString("%1-%2").arg(3).arg(5), struCnfp.nMatAssembleMode).toInt();
    struCnfp.nMatAssembleModeIntel = setting.value(QString("%1-%2-%3").arg(3).arg(5).arg(1), struCnfp.nMatAssembleModeIntel).toInt();
    /* melon enable */
    struCnfp.nMelonUse = setting.value(QString("%1-%2").arg(3).arg(6), struCnfp.nMelonUse).toInt();

    /* sensor mode */
    struCnfp.nSensorMode = setting.value(QString("%1-%2").arg(3).arg(7), struCnfp.nSensorMode).toInt();

    /* background */

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_BACKGROUND_GROUP; j++)
        {
            struCnfp.nBackgroundColor[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(8).arg(i).arg(j), struCnfp.nBackgroundColor[i][j]).toInt();
            struCnfp.nBackgroundColorValue[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(9).arg(i).arg(j), struCnfp.nBackgroundColorValue[i][j]).toInt();
            struCnfp.nBackgroundColorPer[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(10).arg(i).arg(j), struCnfp.nBackgroundColorPer[i][j]).toInt();
        }
    }

    /* arithmetic enable */
    for (int i = 0; i < ARITHMETIC_TOTAL; i++)
    {
        struCnfp.nArithmeticEnable[i] = setting.value(QString("%1-%2-%3").arg(3).arg(11).arg(i), struCnfp.nArithmeticEnable[i]).toInt();
    }

    /* intel mode */
    struCnfp.nIntelMode = setting.value(QString("%1-%2").arg(3).arg(12), struCnfp.nIntelMode).toInt();

    /* intel material name */
    for (int i = 0; i < 4; i++)
    {
        struCnfp.struIntelMat[i].use = setting.value(QString("%1-%2-%3-%4").arg(3).arg(13).arg(i).arg(0), struCnfp.struIntelMat[i].use).toInt();
        struCnfp.struIntelMat[i].good_bad = setting.value(QString("%1-%2-%3-%4").arg(3).arg(13).arg(i).arg(1), struCnfp.struIntelMat[i].good_bad).toInt();
        memcpy(struCnfp.struIntelMat[i].name, setting.value(QString("%1-%2-%3-%4").arg(3).arg(13).arg(i).arg(2), struCnfp.struIntelMat[i].name).toString().toLocal8Bit().constData(), sizeof(struCnfp.struIntelMat[i].name));
    }

    /* identify parameters */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
        {
            /* color */
            for (int k = 0; k < 4; k++)
            {
                /* name */
                memcpy(struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName, setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(0),
                    struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName));
                /* row */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(1), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow).toInt();
                /* column */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(2), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn).toInt();
                /* percent */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(3), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent).toInt();
                /* color */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColor = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(4), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColor).toInt();
                /* discolor */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nDiscolor = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(5), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nDiscolor).toInt();
                /* sens min */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMin = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(6), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMin).toInt();
                /* sens max */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMax = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(7), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMax).toInt();
                /* mode */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nMode = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(8), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nMode).toInt();
                /* light limit */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nLightLimit = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(9), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nLightLimit).toInt();
                /* Burnt Reserved Flag */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].nBurntReservedFlag = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nBurntReservedFlag).toInt();

                /* reserved */
                    /* discolor 1 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nDiscolor1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(0), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nDiscolor1).toInt();
                /* sens min 1 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(1), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin1).toInt();
                /* sens max 1 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(2), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax1).toInt();
                /* mode 1 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(3), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode1).toInt();
                /* color 2 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nColor2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(4), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nColor2).toInt();
                /* sens min 2 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(5), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin2).toInt();
                /* sens max 2 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(6), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax2).toInt();
                /* mode 2 */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(7), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode2).toInt();
                /* percent */
                struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(8), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nPercent).toInt();
            }

            /* cross */
                /* name */
            memcpy(struCnfp.struGroupIdentify[i][j].struCross.sName, setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(0), struCnfp.struGroupIdentify[i][j].struCross.sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struCross.sName));
            /* row */
            struCnfp.struGroupIdentify[i][j].struCross.nRow = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(1), struCnfp.struGroupIdentify[i][j].struCross.nRow).toInt();
            /* column */
            struCnfp.struGroupIdentify[i][j].struCross.nColumn = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(2), struCnfp.struGroupIdentify[i][j].struCross.nColumn).toInt();
            /* percent */
            struCnfp.struGroupIdentify[i][j].struCross.nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(3), struCnfp.struGroupIdentify[i][j].struCross.nPercent).toInt();

            /* sens */
            struCnfp.struGroupIdentify[i][j].struCross.nSens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(4), struCnfp.struGroupIdentify[i][j].struCross.nSens).toInt();

            /* color */
            struCnfp.struGroupIdentify[i][j].struCross.nColor = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(5), struCnfp.struGroupIdentify[i][j].struCross.nColor).toInt();

            /* shape */
                /* name */
            memcpy(struCnfp.struGroupIdentify[i][j].struShape.sName, setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(0), "material").toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struShape.sName));
            /* area min */
            struCnfp.struGroupIdentify[i][j].struShape.nAreaMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(1), struCnfp.struGroupIdentify[i][j].struShape.nAreaMin).toInt();
            /* area max */
            struCnfp.struGroupIdentify[i][j].struShape.nAreaMax = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(2), struCnfp.struGroupIdentify[i][j].struShape.nAreaMax).toInt();
            /* mode */
            struCnfp.struGroupIdentify[i][j].struShape.nMode = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(3), struCnfp.struGroupIdentify[i][j].struShape.nMode).toInt();
            /* length name */
            memcpy(struCnfp.struGroupIdentify[i][j].struShape.sLengthName, setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(4), "material").toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struShape.sLengthName));
            /* length min */
            struCnfp.struGroupIdentify[i][j].struShape.nLengthMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(5), struCnfp.struGroupIdentify[i][j].struShape.nLengthMin).toInt();
            /* length max */
            struCnfp.struGroupIdentify[i][j].struShape.nLengthMax = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(6), struCnfp.struGroupIdentify[i][j].struShape.nLengthMax).toInt();
            /* length mode */
            struCnfp.struGroupIdentify[i][j].struShape.nLengthMode = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(7), struCnfp.struGroupIdentify[i][j].struShape.nLengthMode).toInt();

            /* length limit */
            struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(8), struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit).toInt();
            /* length limit */
            struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(9), struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit).toInt();

            /* scale */
            for (int k = 0; k < 2; k++)
            {
                /* name */
                memcpy(struCnfp.struGroupIdentify[i][j].struScale[k].sName, setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(0 + 4 * k), "material").toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struScale[k].sName));
                /* mode */
                struCnfp.struGroupIdentify[i][j].struScale[k].nMode = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(1 + 4 * k), struCnfp.struGroupIdentify[i][j].struScale[k].nMode).toInt();
                /* value */
                struCnfp.struGroupIdentify[i][j].struScale[k].nValue = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(2 + 4 * k), struCnfp.struGroupIdentify[i][j].struScale[k].nValue).toInt();
                /* value limit */
                struCnfp.struGroupIdentify[i][j].struScale[k].nValueLimit = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(3 + 4 * k), struCnfp.struGroupIdentify[i][j].struScale[k].nValueLimit).toInt();

            }

            /* bud */
            for (int k = 0; k < 2; k++)
            {
                /* name */
                memcpy(struCnfp.struGroupIdentify[i][j].struBud[k].sName, setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(0), struCnfp.struGroupIdentify[i][j].struBud[k].sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struBud[k].sName));
                /* row */
                struCnfp.struGroupIdentify[i][j].struBud[k].nRow = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(1), struCnfp.struGroupIdentify[i][j].struBud[k].nRow).toInt();
                /* column */
                struCnfp.struGroupIdentify[i][j].struBud[k].nColumn = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(2), struCnfp.struGroupIdentify[i][j].struBud[k].nColumn).toInt();
                /* percent */
                struCnfp.struGroupIdentify[i][j].struBud[k].nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(3), struCnfp.struGroupIdentify[i][j].struBud[k].nPercent).toInt();
                /* percent limit */
                struCnfp.struGroupIdentify[i][j].struBud[k].nPercentLimit = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(4), struCnfp.struGroupIdentify[i][j].struBud[k].nPercentLimit).toInt();
                /* area */
                struCnfp.struGroupIdentify[i][j].struBud[k].nArea = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(5), struCnfp.struGroupIdentify[i][j].struBud[k].nArea).toInt();
            }

            /* peanut */
            for (int k = 0; k < 4; k++)
            {
                /* name */
                memcpy(struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName, setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(0), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName).toString().toLocal8Bit().constData(),
                    sizeof(struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName));
                /* row */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nRow = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(1), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nRow).toInt();
                /* column */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nColumn = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(2), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nColumn).toInt();
                /* percent */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(3), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nPercent).toInt();
                /* sens min */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMin = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(4), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMin).toInt();
                /* sens max */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMax = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(5), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMax).toInt();
                /* mode */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nMode = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(6), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nMode).toInt();
                /* sens reserved */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensReserved = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(7), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensReserved).toInt();

                /* reserved */
                    /* discolor 1 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nDiscolor1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(0),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nDiscolor1).toInt();
                /* sens min 1 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(1),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin1).toInt();
                /* sens max 1 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(2),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax1).toInt();
                /* mode 1 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(3),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode1).toInt();
                /* color 2 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nColor2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(4),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nColor2).toInt();
                /* sens min 2 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(5),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin2).toInt();
                /* sens max 2 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(6),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax2).toInt();
                /* mode 2 */
                struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(7),
                    struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode2).toInt();
            }

            /* intel */
            for (int k = 0; k < 4; k++)
            {
                /* name */
                memcpy(struCnfp.struGroupIdentify[i][j].struIntel[k].sName, setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(0),
                    struCnfp.struGroupIdentify[i][j].struIntel[k].sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struIntel[k].sName));
                /* row */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nRow = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(1), struCnfp.struGroupIdentify[i][j].struIntel[k].nRow).toInt();
                /* column */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(2), struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn).toInt();
                /* percent */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(3), struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent).toInt();
                /* balance */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nEnable = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(4), struCnfp.struGroupIdentify[i][j].struIntel[k].nEnable).toInt();
                /* sens */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nSens = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(5), struCnfp.struGroupIdentify[i][j].struIntel[k].nSens).toInt();
                /* whole edge */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nWholeEdge = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(6), struCnfp.struGroupIdentify[i][j].struIntel[k].nWholeEdge).toInt();

                /* reserved */
                /* discolor 1 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nDiscolor1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(0), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nDiscolor1).toInt();
                /* sens min 1 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(1), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin1).toInt();
                /* sens max 1 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(2), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax1).toInt();
                /* mode 1 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(3), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode1).toInt();
                /* color 2 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nColor2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(4), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nColor2).toInt();
                /* sens min 2 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(5), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin2).toInt();
                /* sens max 2 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(6), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax2).toInt();
                /* mode 2 */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(7), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode2).toInt();
                /* percent */
                struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(8), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nPercent).toInt();

                /* mode */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nModeDfl = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(8), struCnfp.struGroupIdentify[i][j].struIntel[k].nModeDfl).toInt();

                /* sensRatio */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nSensRatio = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(10), struCnfp.struGroupIdentify[i][j].struIntel[k].nSensRatio).toInt();
                /* intelType */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nIntelType = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(11), struCnfp.struGroupIdentify[i][j].struIntel[k].nIntelType).toInt();

                for (int m = 0; m < 2; m++)
                {
                    /* normal */
                        /* cons */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nCons[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(9).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nCons[m]).toInt();

                    /* red */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo1[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(10).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo1[m]).toInt();

                    /* green */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo2[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(11).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo2[m]).toInt();

                    /* blue */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo3[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(12).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo3[m]).toInt();
                    /* multi mode */
                        /* cons */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nConsDfl[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(13).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nConsDfl[m]).toInt();
                    /* r */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoR[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(14).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoR[m]).toInt();
                    /* g */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoG[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(15).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoG[m]).toInt();
                    /* b */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoB[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(16).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoB[m]).toInt();
                    /* rr */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRR[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(17).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRR[m]).toInt();
                    /* gg */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGG[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(18).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGG[m]).toInt();
                    /* bb */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoBB[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(19).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoBB[m]).toInt();
                    /* rg */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRG[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(20).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRG[m]).toInt();
                    /* rb */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRB[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(21).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRB[m]).toInt();
                    /* gb */
                    struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGB[m] = setting.value(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(22).arg(m), struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGB[m]).toInt();
                }
                /*ratioMode*/
                struCnfp.struGroupIdentify[i][j].struIntel[k].nRatioMode = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(23), struCnfp.struGroupIdentify[i][j].struIntel[k].nRatioMode).toInt();
                /* rowRes */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nRowRes = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(24), struCnfp.struGroupIdentify[i][j].struIntel[k].nRowRes).toInt();
                /* columnRes */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nColumnRes = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(25), struCnfp.struGroupIdentify[i][j].struIntel[k].nColumnRes).toInt();
                /* percentRes */
                struCnfp.struGroupIdentify[i][j].struIntel[k].nPercentRes = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(26), struCnfp.struGroupIdentify[i][j].struIntel[k].nPercentRes).toInt();
            }

            /* melon */
                /* white sens */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(0), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens).toInt();
            /* white prop min */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(1), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin).toInt();
            /* white prop max */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(2), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax).toInt();
            /* white mode */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(3), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode).toInt();
            /* edge red sens */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(4), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens).toInt();
            /* edge red ratio */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(5), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio).toInt();
            /* edge damage sens */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(6), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens).toInt();
            /* edge damage ratio */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(7), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio).toInt();
            /* rugged 1 sens */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(8), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens).toInt();
            /* rugged 1 save */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Save = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(9), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Save).toInt();
            /* rugged 2 sens */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(10), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens).toInt();
            /* rugged 2 save */
            struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(11), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save).toInt();

            /* RS机型相关参数 */
            struCnfp.struGroupIdentify[i][j].struRice.nSens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(0),
                struCnfp.struGroupIdentify[i][j].struRice.nSens).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(2),
                struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nRow = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(3),
                struCnfp.struGroupIdentify[i][j].struRice.nRow).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nCol = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(4),
                struCnfp.struGroupIdentify[i][j].struRice.nCol).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nYellowArea = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(5),
                struCnfp.struGroupIdentify[i][j].struRice.nYellowArea).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nBadWidth = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(6),
                struCnfp.struGroupIdentify[i][j].struRice.nBadWidth).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nDarkSens = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(12),
                struCnfp.struGroupIdentify[i][j].struRice.nDarkSens).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(14),
                struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1).toInt();
            struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(15),
                struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2).toInt();
            /* maize */
                /* nRow */
            struCnfp.struGroupIdentify[i][j].struMaize.nRow = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(0),
                struCnfp.struGroupIdentify[i][j].struMaize.nRow).toInt();
            /* nPercentTotal */
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(1),
                struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal).toInt();
            /* nPercentGood */
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(2),
                struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood).toInt();
            /* nPercentBad1 */
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(3),
                struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1).toInt();
            /* nPercentBad2 */
            struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(4),
                struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2).toInt();
            /* nSensMin1 */
            struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(5),
                struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1).toInt();
            /* nSensProport */
            struCnfp.struGroupIdentify[i][j].struMaize.nSensProport = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(6),
                struCnfp.struGroupIdentify[i][j].struMaize.nSensProport).toInt();
            /* nSensMin2 */
            struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(7),
                struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2).toInt();
            /* nColum*/
            struCnfp.struGroupIdentify[i][j].struMaize.nColumn = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(8),
                struCnfp.struGroupIdentify[i][j].struMaize.nColumn).toInt();
            /* threshold*/
            struCnfp.struGroupIdentify[i][j].struMaize.nThreshold = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(9),
                struCnfp.struGroupIdentify[i][j].struMaize.nThreshold).toInt();

            /* 选杆模式 */
                /* sName */
            memcpy(struCnfp.struGroupIdentify[i][j].struPole.sName,
                setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(0),
                    struCnfp.struGroupIdentify[i][j].struPole.sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struPole.sName));
            /* nArea */
            struCnfp.struGroupIdentify[i][j].struPole.nArea = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(1),
                struCnfp.struGroupIdentify[i][j].struPole.nArea).toInt();

            /* nPercent */
            struCnfp.struGroupIdentify[i][j].struPole.nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(2),
                struCnfp.struGroupIdentify[i][j].struPole.nPercent).toInt();

            /* nWidthMin */
            struCnfp.struGroupIdentify[i][j].struPole.nWidthMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(3),
                struCnfp.struGroupIdentify[i][j].struPole.nWidthMin).toInt();
            /* nWidthMax */
            struCnfp.struGroupIdentify[i][j].struPole.nWidthMax = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(4),
                struCnfp.struGroupIdentify[i][j].struPole.nWidthMax).toInt();

            /* reserved */
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(0),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(1),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(2),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(3),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(4),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(5),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(6),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2).toInt();
            struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2 = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(7),
                struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2).toInt();

            /* wide pole name */
            memcpy(struCnfp.struGroupIdentify[i][j].struPole.sPoleName,
                setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(6),
                    struCnfp.struGroupIdentify[i][j].struPole.sPoleName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struPole.sPoleName));
            /* wide pole area */
            struCnfp.struGroupIdentify[i][j].struPole.nPoleArea = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(7),
                struCnfp.struGroupIdentify[i][j].struPole.nPoleArea).toInt();

            /* wide pole percent */
            struCnfp.struGroupIdentify[i][j].struPole.nPolePercent = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(8),
                struCnfp.struGroupIdentify[i][j].struPole.nPolePercent).toInt();

            /* wide pole min */
            struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(9),
                struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin).toInt();

            /* wide pole max */
            struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(10),
                struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax).toInt();

            /* 选片算法 */
                /* slice name */
            memcpy(struCnfp.struGroupIdentify[i][j].struSlice.sName,
                setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(12).arg(0),
                    struCnfp.struGroupIdentify[i][j].struSlice.sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struSlice.sName));
            /* slice area */
            struCnfp.struGroupIdentify[i][j].struSlice.nArea = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(12).arg(1),
                struCnfp.struGroupIdentify[i][j].struSlice.nArea).toInt();

            /* 选芽算法 */
                /* leaf name */
            memcpy(struCnfp.struGroupIdentify[i][j].struLeaf.sName,
                setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(0),
                    struCnfp.struGroupIdentify[i][j].struLeaf.sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struLeaf.sName));

            /* leaf area */
            struCnfp.struGroupIdentify[i][j].struLeaf.nArea = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(1),
                struCnfp.struGroupIdentify[i][j].struLeaf.nArea).toInt();

            /* leaf percent */
            struCnfp.struGroupIdentify[i][j].struLeaf.nPercent = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(2),
                struCnfp.struGroupIdentify[i][j].struLeaf.nPercent).toInt();

            /* leaf width min */
            struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(3),
                struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin).toInt();

            /* leaf width max */
            struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMax = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(4),
                struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMax).toInt();

            /* leaf threshold */
            struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(5),
                struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold).toInt();
            /* 保留算法 */
            memcpy(struCnfp.struGroupIdentify[i][j].struReserved.sName,
                setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(0),
                    struCnfp.struGroupIdentify[i][j].struReserved.sName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struReserved.sName));
            struCnfp.struGroupIdentify[i][j].struReserved.nRow = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(1),
                struCnfp.struGroupIdentify[i][j].struReserved.nRow).toInt();
            struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_s = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(2),
                struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_s).toInt();
            struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_r = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(3),
                struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_r).toInt();
            /* 糖果算法 */
            struCnfp.struGroupIdentify[i][j].struCandy.nRation1 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(0),
                struCnfp.struGroupIdentify[i][j].struCandy.nRation1).toInt();
            struCnfp.struGroupIdentify[i][j].struCandy.nRation2 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(1),
                struCnfp.struGroupIdentify[i][j].struCandy.nRation2).toInt();
            struCnfp.struGroupIdentify[i][j].struCandy.nRow = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(2),
                struCnfp.struGroupIdentify[i][j].struCandy.nRow).toInt();
            struCnfp.struGroupIdentify[i][j].struCandy.nCycles = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(3),
                struCnfp.struGroupIdentify[i][j].struCandy.nCycles).toInt();

            /* 开心果算法 */
            struCnfp.struGroupIdentify[i][j].struPistachio.nAreaMin = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(0),
                struCnfp.struGroupIdentify[i][j].struPistachio.nAreaMin).toInt();
            struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_1 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(1),
                struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_1).toInt();
            struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_2 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(2),
                struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_2).toInt();
            struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_3 = setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(3),
                struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_3).toInt();

            memcpy(struCnfp.struGroupIdentify[i][j].struAi.modelId,
                setting.value(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(17).arg(0),
                    struCnfp.struGroupIdentify[i][j].struAi.modelId).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struAi.modelId));
            //            for(int k=0; k<MAX_PARA; k++){
            //                struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].id    = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(17).arg(1).arg(k),
            //                                                                                        struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].id).toInt();
            //                memcpy(struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName,
            //                       setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(17).arg(0).arg(2).arg(k),
            //                                     struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName).toString().toLocal8Bit().constData(), sizeof(struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName));
            //                struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].threshold    = setting.value(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(17).arg(3).arg(k),
            //                                                                                        struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].threshold).toInt();

            //            }

        }
    }

    /* tick parameters */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_GROUP_TICK; j++)
        {
            /* eject duration */
            struCnfp.struGroupTick[i][j].nEjectDuration = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(0), struCnfp.struGroupTick[i][j].nEjectDuration).toInt();

            /* eject delay */
            struCnfp.struGroupTick[i][j].nEjectDelay = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(1), struCnfp.struGroupTick[i][j].nEjectDelay).toInt();

            /* front & rear mode */
            struCnfp.struGroupTick[i][j].nFrontRearMode = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(2), struCnfp.struGroupTick[i][j].nFrontRearMode).toInt();

            /* infect enable flag*/
            struCnfp.struGroupTick[i][j].nImpuredFlag = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(3), struCnfp.struGroupTick[i][j].nImpuredFlag).toInt();

            /* eject width */
            struCnfp.struGroupTick[i][j].nEjwidth = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(4), struCnfp.struGroupTick[i][j].nEjwidth).toInt();

            /* edge cut */
            struCnfp.struGroupTick[i][j].nEdgeCut = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(5), struCnfp.struGroupTick[i][j].nEdgeCut).toInt();

            /* bad num */
            struCnfp.struGroupTick[i][j].nBadNum = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(6), struCnfp.struGroupTick[i][j].nBadNum).toInt();

            /* reverse sort */
            struCnfp.struGroupTick[i][j].nReverseSort = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(7), struCnfp.struGroupTick[i][j].nReverseSort).toInt();

            /* material width */
            struCnfp.struGroupTick[i][j].nMatWidth = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(8), struCnfp.struGroupTick[i][j].nMatWidth).toInt();

            /* good num */
            struCnfp.struGroupTick[i][j].nGoodNum = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(9), struCnfp.struGroupTick[i][j].nGoodNum).toInt();

            /* edge revise */
            struCnfp.struGroupTick[i][j].nEdgeRevise = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(10), struCnfp.struGroupTick[i][j].nEdgeRevise).toInt();

            /* enable balance */
            struCnfp.struGroupTick[i][j].nEnableBalance = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(11), struCnfp.struGroupTick[i][j].nEnableBalance).toInt();

            /* 吹气次数限制 */
            struCnfp.struGroupTick[i][j].nEjectTimesLimit = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(12),
                struCnfp.struGroupTick[i][j].nEjectTimesLimit).toInt();

            /* eject degauss */
            struCnfp.struGroupTick[i][j].nEjectDegauss = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(13), struCnfp.struGroupTick[i][j].nEjectDegauss).toInt();

            /* material area */
            struCnfp.struGroupTick[i][j].nMatArea = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(14), struCnfp.struGroupTick[i][j].nMatArea).toInt();

            /* eject duration */
            struCnfp.struGroupTick[i][j].nEjectDurationCent = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(30), struCnfp.struGroupTick[i][j].nEjectDurationCent).toInt();

            /* eject delay */
            struCnfp.struGroupTick[i][j].nEjectDelayCent = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(31), struCnfp.struGroupTick[i][j].nEjectDelayCent).toInt();

            /* infect num */
            struCnfp.struGroupTick[i][j].nImpured = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(34), struCnfp.struGroupTick[i][j].nImpured).toInt();

            /* Corrupted num */
            struCnfp.struGroupTick[i][j].nCorrupted = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(35), struCnfp.struGroupTick[i][j].nCorrupted).toInt();

            /* Expanded num */
            struCnfp.struGroupTick[i][j].nExpanded = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(36), struCnfp.struGroupTick[i][j].nExpanded).toInt();

            /* shape arith infect num */
            struCnfp.struGroupTick[i][j].nShapeImpured = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(37), struCnfp.struGroupTick[i][j].nShapeImpured).toInt();

            /* shape arith Corrupted num */
            struCnfp.struGroupTick[i][j].nShapeCorrupted = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(38), struCnfp.struGroupTick[i][j].nShapeCorrupted).toInt();

            /* shape arith Expanded num */
            struCnfp.struGroupTick[i][j].nShapeExpanded = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(39), struCnfp.struGroupTick[i][j].nShapeExpanded).toInt();

            /* 辅配吹气时间 */
            struCnfp.struGroupTick[i][j].nAssistEjectTime = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(40), struCnfp.struGroupTick[i][j].nAssistEjectTime).toInt();

            /* 辅配延迟时间 */
            struCnfp.struGroupTick[i][j].nAssistDelayTime = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(41), struCnfp.struGroupTick[i][j].nAssistDelayTime).toInt();

            /* 去除拼接的边缘切除圈数 */
            struCnfp.struGroupTick[i][j].nRemoveJoint = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(42), struCnfp.struGroupTick[i][j].nRemoveJoint).toInt();

            struCnfp.struGroupTick[i][j].nEjectDelayBias = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(43), struCnfp.struGroupTick[i][j].nEjectDelayBias).toInt();
            struCnfp.struGroupTick[i][j].nEjectDelayBiasCent = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(44), struCnfp.struGroupTick[i][j].nEjectDelayBiasCent).toInt();

            /* eject truncation */
            struCnfp.struGroupTick[i][j].nEjectTruncation = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(45), struCnfp.struGroupTick[i][j].nEjectTruncation).toInt();

            /* eject degauss */
            struCnfp.struGroupTick[i][j].nAssistEjectDegauss = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(47), struCnfp.struGroupTick[i][j].nAssistEjectDegauss).toInt();
            /* Assist eject truncation */
            struCnfp.struGroupTick[i][j].nAssistEjectTruncation = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(49), struCnfp.struGroupTick[i][j].nAssistEjectTruncation).toInt();

            /* 吹气宽度 huff width */
            struCnfp.struGroupTick[i][j].nhuffWidth = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(51), struCnfp.struGroupTick[i][j].nhuffWidth).toInt();

            /* CG形选前后视与或关系及正反选 */
            struCnfp.struGroupTick[i][j].nShapeLogicMode = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(54), struCnfp.struGroupTick[i][j].nShapeLogicMode).toInt();
            struCnfp.struGroupTick[i][j].nShapeSortMode = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(55), struCnfp.struGroupTick[i][j].nShapeSortMode).toInt();

            /* 新R精选剔除模式参数 */
            struCnfp.struGroupTick[i][j].nEjectLenthMax = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(56), struCnfp.struGroupTick[i][j].nEjectLenthMax).toInt();
            struCnfp.struGroupTick[i][j].nEjectLenthDif = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(57), struCnfp.struGroupTick[i][j].nEjectLenthDif).toInt();
            struCnfp.struGroupTick[i][j].nEjectProportion1 = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(58), struCnfp.struGroupTick[i][j].nEjectProportion1).toInt();
            struCnfp.struGroupTick[i][j].nEjectProportion2 = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(59), struCnfp.struGroupTick[i][j].nEjectProportion2).toInt();
            struCnfp.struGroupTick[i][j].nEjectThresHold = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(60), struCnfp.struGroupTick[i][j].nEjectThresHold).toInt();
            struCnfp.struGroupTick[i][j].nTickMode = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(61), struCnfp.struGroupTick[i][j].nTickMode).toInt();

            /* 三次反选圈数*/
            struCnfp.struGroupTick[i][j].nRounds = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(62), struCnfp.struGroupTick[i][j].nRounds).toInt();

            struCnfp.struGroupTick[i][j].aiEjectorDelay = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(63), struCnfp.struGroupTick[i][j].aiEjectorDelay).toInt();
            struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(64), struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay).toInt();
            struCnfp.struGroupTick[i][j].aiEjectorBlowTime = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(65), struCnfp.struGroupTick[i][j].aiEjectorBlowTime).toInt();

        }
    }

    /* control parameters */
    for (int i = 0; i < MAX_CTRL; i++)
    {
        /* feeder */
        for (int j = 0; j < 10; j++)
        {
            struCnfp.struGroupCtrl[i].nFeeder[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(0).arg(j), struCnfp.struGroupCtrl[i].nFeeder[j]).toInt();
        }

        /* wipe duration */
        struCnfp.struGroupCtrl[i].nWipeDuration = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(1), struCnfp.struGroupCtrl[i].nWipeDuration).toInt();

        /* wipe interval */
        struCnfp.struGroupCtrl[i].nWipeInterval = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(2), struCnfp.struGroupCtrl[i].nWipeInterval).toInt();

        /* wipe wind duration */
        struCnfp.struGroupCtrl[i].nWipeWindDuration = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(3), struCnfp.struGroupCtrl[i].nWipeWindDuration).toInt();

        /* wipe wind interval */
        struCnfp.struGroupCtrl[i].nWipeWindInterval = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(4), struCnfp.struGroupCtrl[i].nWipeWindInterval).toInt();

        /* background front */
        struCnfp.struGroupCtrl[i].nBackgroundFront = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(5), struCnfp.struGroupCtrl[i].nBackgroundFront).toInt();

        /* background Rear */
        struCnfp.struGroupCtrl[i].nBackgroundRear = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(6), struCnfp.struGroupCtrl[i].nBackgroundRear).toInt();

        /* lamp control */
        for (int j = 0; j < MAX_LAMP; j++)
        {
            struCnfp.struGroupCtrl[i].nLamp[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(7).arg(j), struCnfp.struGroupCtrl[i].nLamp[j]).toInt();
        }

        /* wipe water duration */
        struCnfp.struGroupCtrl[i].nWipeWaterDuration = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(8), struCnfp.struGroupCtrl[i].nWipeWaterDuration).toInt();
        struCnfp.struGroupCtrl[i].nWipeWaterDelay = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(8).arg(1), struCnfp.struGroupCtrl[i].nWipeWaterDelay).toInt();

        /* 供料器使能 */
        for (int j = 0; j < 10; j++)
        {
            struCnfp.struGroupCtrl[i].nFeederEnable[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(9).arg(j),
                struCnfp.struGroupCtrl[i].nFeederEnable[j]).toInt();
        }

        /* 灯光强 */
        for (int j = 0; j < MAX_LAMP; j++)
        {
            struCnfp.struGroupCtrl[i].nLampLight[j] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(10).arg(j),
                struCnfp.struGroupCtrl[i].nLampLight[j]).toInt();
        }

        /* wipe lift before */
        struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(11), struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe).toInt();

        /* wipe lift after */
        struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(12), struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe).toInt();

        /* wipe duration2 */
        struCnfp.struGroupCtrl[i].nWipeDuration2 = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(13), struCnfp.struGroupCtrl[i].nWipeDuration2).toInt();

        /* wipe delay */
        struCnfp.struGroupCtrl[i].nWipeDelay = setting.value(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(14), struCnfp.struGroupCtrl[i].nWipeDelay).toInt();
    }
    struCnfp.nDegaussTime = setting.value(QString("%1-%2").arg(3).arg(20), struCnfp.nDegaussTime).toInt();
    for (int i = 0; i < MAX_UNIT; i++)
    {
        struCnfp.nAnaGain[i] = setting.value(QString("%1-%2-%3").arg(3).arg(21).arg(i),
            struCnfp.nAnaGain[i]).toInt();
        struCnfp.nDigGain[i] = setting.value(QString("%1-%2-%3").arg(3).arg(22).arg(i),
            struCnfp.nDigGain[i]).toInt();
        struCnfp.nGainTarget[i] = setting.value(QString("%1-%2-%3").arg(3).arg(23).arg(i),
            struCnfp.nGainTarget[i]).toInt();
        for (int j = 0; j < 2; j++)
        {
            struCnfp.nSens[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(27).arg(i).arg(j),
                struCnfp.nSens[i][j]).toInt();
        }
    }
    for (int i = 0; i < MAX_GROUP_TICK; i++)
    {
        struCnfp.nCrossChannel[i] = setting.value(QString("%1-%2-%3").arg(3).arg(29).arg(i),
            struCnfp.nCrossChannel[i]).toInt();
    }
    for (int i = 0; i < MAX_UNIT / 2; i++)
    {
        struCnfp.nDelayBiasing[i] = setting.value(QString("%1-%2-%3").arg(3).arg(31).arg(i),
            struCnfp.nDelayBiasing[i]).toInt();
        struCnfp.nFeederBias[i] = setting.value(QString("%1-%2-%3").arg(3).arg(32).arg(i),
            struCnfp.nFeederBias[i]).toInt();
    }
    struCnfp.nColorSpace = setting.value(QString("%1-%2").arg(3).arg(33),
        struCnfp.nColorSpace).toInt();
    struCnfp.nAIMode = setting.value(QString("%1-%2").arg(3).arg(34),
        struCnfp.nAIMode).toInt();

    /* arithmetic enable for tea */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            struCnfp.nArithmeticEnableLevel[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(38).arg(i).arg(j),
                struCnfp.nArithmeticEnableLevel[i][j]).toInt();
        }
    }

    /* 2014-09-12 RD-C定制添加,2015-1-19 玉米定制添加 */
    /* 灵敏度偏置 */
    for (int i = 0; i < MAX_UNIT; i++)
    {
        for (int j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            struCnfp.nSensBias[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(39).arg(i).arg(j),
                struCnfp.nSensBias[i][j]).toInt();
        }
        for (int j = 0; j < MAX_MAIZE_SENSNUM_TOTAL; j++)
        {
            struCnfp.nSensBiasMaize[i][j] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(39).arg(i).arg(j).arg(0),
                struCnfp.nSensBiasMaize[i][j]).toInt();
        }
    }

    for (int i = 0; i < MAX_UNIT; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            struCnfp.nDarkSens[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(40).arg(i).arg(j),
                struCnfp.nDarkSens[i][j]).toInt();
        }
    }

    /*智能供料调节相关变量*/
    struCnfp.isFeederAIEnable =
        setting.value(QString("%1-%2-%3").arg(3).arg(42).arg(1), struCnfp.isFeederAIEnable).toInt();
    struCnfp.nFeederCapMin =
        setting.value(QString("%1-%2-%3").arg(3).arg(42).arg(2), struCnfp.nFeederCapMin).toInt();
    struCnfp.nFeederCapMax =
        setting.value(QString("%1-%2-%3").arg(3).arg(42).arg(3), struCnfp.nFeederCapMax).toInt();
    struCnfp.nEjectTimesMin =
        setting.value(QString("%1-%2-%3").arg(3).arg(42).arg(4), struCnfp.nEjectTimesMin).toInt();
    struCnfp.nEjectTimesMax =
        setting.value(QString("%1-%2-%3").arg(3).arg(42).arg(5), struCnfp.nEjectTimesMax).toInt();
    struCnfp.autoFeedEnablePageFlag =
        setting.value(QString("%1-%2-%3").arg(3).arg(42).arg(6), struCnfp.autoFeedEnablePageFlag).toInt();

    //! auto sig
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                struCnfp.nAutoSigVal[i][j][k] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(43).arg(i).arg(j).arg(k),
                    struCnfp.nAutoSigVal[i][j][k]).toInt();
                struCnfp.nAutoAdVal[i][j][k] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(44).arg(i).arg(j).arg(k),
                    struCnfp.nAutoAdVal[i][j][k]).toInt();
            }
        }
    }

    struCnfp.nAutoSigInit = setting.value(QString("%1-%2").arg(3).arg(45),
        struCnfp.nAutoSigInit).toInt();
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                struCnfp.nAutoBgVal[i][j][k] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(46).arg(i).arg(j).arg(k),
                    struCnfp.nAutoBgVal[i][j][k]).toInt();
            }
        }
    }

    // 性能参数统计设置参数
    struCnfp.nMaterialQueryInterval = setting.value(QString("%1-%2").arg(3).arg(47),
        struCnfp.nMaterialQueryInterval).toInt();
    struCnfp.nHistogramQueryTime = setting.value(QString("%1-%2").arg(3).arg(48),
        struCnfp.nHistogramQueryTime).toInt();
    struCnfp.nCameraRef = setting.value(QString("%1-%2").arg(3).arg(50),
        struCnfp.nCameraRef).toInt();

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_COLOR_MODE; j++)
        {
            struCnfp.nTeaProfileCustomType[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(49).arg(i).arg(j),
                struCnfp.nTeaProfileCustomType[i][j]).toInt();
        }
    }

    // 背景自适应
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            struCnfp.nAutoBgEnable[i][j] = setting.value(QString("%1-%2-%3-%4").arg(3).arg(51).arg(i).arg(j),
                struCnfp.nAutoBgEnable[i][j]).toInt();
        }
    }
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                struCnfp.nAutoBgRange[i][j][k] = setting.value(QString("%1-%2-%3-%4-%5").arg(3).arg(52).arg(i).arg(j).arg(k),
                    struCnfp.nAutoBgRange[i][j][k]).toInt();
            }
        }
    }

    //下位机自动背景使能
    struCnfp.nAutoBackgroundEnanble_RSC = setting.value(QString("%1-%2").arg(3).arg(53), struCnfp.nAutoBackgroundEnanble_RSC).toInt();
    //下位机自动背景阈值
    struCnfp.nAutoBackgroundThresholdValue_RSC = setting.value(QString("%1-%2").arg(3).arg(54), struCnfp.nAutoBackgroundThresholdValue_RSC).toInt();

    if (setting.status() == QSettings::NoError)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* 保存当前色选参数至方案配置文件 */
bool GlobalFlow::saveProfileSetting(const QString& fileName)
{
    /* construct setting file */
    QSettings setting(fileName, QSettings::IniFormat);
    /* save parameters */
        /* interface board mode */
    if (struCnfp.nInterfaceBoardMode != _t_struCnfp.nInterfaceBoardMode)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(0), struCnfp.nInterfaceBoardMode);
    }

    /* color board mode */
    if (struCnfp.nColorBoardMode != _t_struCnfp.nColorBoardMode)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(1), struCnfp.nColorBoardMode);
    }

    /* intel sequence */
    if (struCnfp.nMatIntelSeq != _t_struCnfp.nMatIntelSeq)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(3), struCnfp.nMatIntelSeq);
    }

    /* material assemble mode */
    if (struCnfp.nMatAssembleMode != _t_struCnfp.nMatAssembleMode)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(5), struCnfp.nMatAssembleMode);
    }
    if (struCnfp.nMatAssembleModeIntel != _t_struCnfp.nMatAssembleModeIntel)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(5).arg(1), struCnfp.nMatAssembleModeIntel);
    }

    /* melon enable */
    if (struCnfp.nMelonUse != _t_struCnfp.nMelonUse)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(6), struCnfp.nMelonUse);
    }

    /* sensor mode */
    if (struCnfp.nSensorMode != _t_struCnfp.nSensorMode)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(7), struCnfp.nSensorMode);
    }

    /* background */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_BACKGROUND_GROUP; j++)
        {
            /* background color type */
            if (struCnfp.nBackgroundColor[i][j] != _t_struCnfp.nBackgroundColor[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(8).arg(i).arg(j), struCnfp.nBackgroundColor[i][j]);
                LOG_INFO_FMT("bg(level:%d,bg No.:%d) color: %d", i, j, struCnfp.nBackgroundColor[i][j]);
            }
            /* background color value */
            if (struCnfp.nBackgroundColorValue[i][j] != _t_struCnfp.nBackgroundColorValue[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(9).arg(i).arg(j), struCnfp.nBackgroundColorValue[i][j]);
                LOG_INFO_FMT("bg(level:%d,bg No.:%d) color value: %d", i, j, struCnfp.nBackgroundColorValue[i][j]);
            }
            /* background color percent */
            if (struCnfp.nBackgroundColorPer[i][j] != _t_struCnfp.nBackgroundColorPer[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(10).arg(i).arg(j), struCnfp.nBackgroundColorPer[i][j]);
                LOG_INFO_FMT("bg(level:%d,bg No.:%d) color percent: %d", i, j, struCnfp.nBackgroundColorPer[i][j]);
            }
        }
    }

    /* arithmetic enable */
    for (int i = 0; i < ARITHMETIC_TOTAL; i++)
    {
        if (struCnfp.nArithmeticEnable[i] != _t_struCnfp.nArithmeticEnable[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(11).arg(i), struCnfp.nArithmeticEnable[i]);
            LOG_INFO_FMT("arith name :%s, enable status: %d", myString.sArithmeticName[i].toStdString(),
                struCnfp.nArithmeticEnable[i]);
        }
    }

    /* intel mode */
    if (struCnfp.nIntelMode != _t_struCnfp.nIntelMode)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(12), struCnfp.nIntelMode);
        LOG_INFO_FMT("intel mode: %d", struCnfp.nIntelMode);
    }

    /* intel material name */
    for (int i = 0; i < 4; i++)
    {
        if (struCnfp.struIntelMat[i].use != _t_struCnfp.struIntelMat[i].use)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(13).arg(i).arg(0), struCnfp.struIntelMat[i].use);
        }
        if (struCnfp.struIntelMat[i].good_bad != _t_struCnfp.struIntelMat[i].good_bad)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(13).arg(i).arg(1), struCnfp.struIntelMat[i].good_bad);
        }
        if (memcmp(struCnfp.struIntelMat[i].name, _t_struCnfp.struIntelMat[i].name, sizeof(struCnfp.struIntelMat[i].name)) != 0)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(13).arg(i).arg(2), struCnfp.struIntelMat[i].name);
        }
    }

    /* identify parameters */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
        {
            /* color */
            for (int k = 0; k < 4; k++)
            {
                /* name */
                if (memcmp(struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName, _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName,
                    sizeof(struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName)) != 0)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(0),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].sName);
                }
                /* row */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(1),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, row: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nRow);
                }
                /* column */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(2),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, column: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColumn);
                }
                /* percent */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(3),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure percent: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nPercent);
                }
                /* color */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColor != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColor)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(4),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColor);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, grey color: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nColor);
                }
                /* discolor */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nDiscolor != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nDiscolor)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(5),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nDiscolor);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, discolor: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nDiscolor);
                }
                /* sens min */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMin != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMin)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(6),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMin);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens min: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMin);
                }
                /* sens max */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMax != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMax)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(7),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMax);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens max: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nSensMax);
                }
                /* mode */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nMode != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nMode)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(8),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nMode);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, mode: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nMode);
                }
                /* light limit */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nLightLimit != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nLightLimit)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(9),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nLightLimit);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, light limit: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nLightLimit);
                }
                /* nBurntReservedFlag */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].nBurntReservedFlag != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].nBurntReservedFlag)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].nBurntReservedFlag);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, Burnt Reserved Flag: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].nBurntReservedFlag);
                }

                /* reserved */
                /* discolor 1 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nDiscolor1 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nDiscolor1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(0),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nDiscolor1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nDiscolor1);
                }
                /* sens min 1 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin1 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(1),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens min: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin1);
                }
                /* sens max 1 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax1 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(2),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens max: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax1);
                }
                /* mode 1 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode1 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(3),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor mode: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode1);
                }
                /* color 2 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nColor2 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nColor2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(4),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nColor2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey color: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nColor2);
                }
                /* sens min 2 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin2 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(5),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens min: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMin2);
                }
                /* sens max 2 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax2 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(6),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens max: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nSensMax2);
                }
                /* mode 2 */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode2 != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(7),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey mode: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nMode2);
                }
                /* percent */
                if (struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nPercent != _t_struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nPercent)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(0).arg(k).arg(10).arg(8),
                        struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nPercent);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey percent: %d",
                        i, j, myString.sArithmeticName[k].toStdString(), struCnfp.struGroupIdentify[i][j].struGreyColor[k].struReserved.nPercent);
                }

            }
            /* cross */
                /* name */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struCross.sName, _t_struCnfp.struGroupIdentify[i][j].struCross.sName,
                sizeof(struCnfp.struGroupIdentify[i][j].struCross.sName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(0),
                    struCnfp.struGroupIdentify[i][j].struCross.sName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                    i, j, myString.sArithmeticName[ARITH_CROSS].toStdString(), struCnfp.struGroupIdentify[i][j].struCross.sName);
            }
            /* row */
            if (struCnfp.struGroupIdentify[i][j].struCross.nRow != _t_struCnfp.struGroupIdentify[i][j].struCross.nRow)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(1),
                    struCnfp.struGroupIdentify[i][j].struCross.nRow);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, row: %d",
                    i, j, myString.sArithmeticName[ARITH_CROSS].toStdString(), struCnfp.struGroupIdentify[i][j].struCross.nRow);
            }
            /* column */
            if (struCnfp.struGroupIdentify[i][j].struCross.nColumn != _t_struCnfp.struGroupIdentify[i][j].struCross.nColumn)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(2),
                    struCnfp.struGroupIdentify[i][j].struCross.nColumn);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, column: %d",
                    i, j, myString.sArithmeticName[ARITH_CROSS].toStdString(), struCnfp.struGroupIdentify[i][j].struCross.nColumn);
            }

            /* percent */
            if (struCnfp.struGroupIdentify[i][j].struCross.nPercent != _t_struCnfp.struGroupIdentify[i][j].struCross.nPercent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(3),
                    struCnfp.struGroupIdentify[i][j].struCross.nPercent);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure percent: %d",
                    i, j, myString.sArithmeticName[ARITH_CROSS].toStdString(), struCnfp.struGroupIdentify[i][j].struCross.nPercent);
            }

            /* sens */
            if (struCnfp.struGroupIdentify[i][j].struCross.nSens != _t_struCnfp.struGroupIdentify[i][j].struCross.nSens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(4),
                    struCnfp.struGroupIdentify[i][j].struCross.nSens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens: %d",
                    i, j, myString.sArithmeticName[ARITH_CROSS].toStdString(), struCnfp.struGroupIdentify[i][j].struCross.nSens);
            }

            /* color */
            if (struCnfp.struGroupIdentify[i][j].struCross.nColor != _t_struCnfp.struGroupIdentify[i][j].struCross.nColor)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(1).arg(5),
                    struCnfp.struGroupIdentify[i][j].struCross.nColor);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, color: %d",
                    i, j, myString.sArithmeticName[ARITH_CROSS].toStdString(), struCnfp.struGroupIdentify[i][j].struCross.nColor);
            }

            /* shape */
                /* name */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struShape.sName, _t_struCnfp.struGroupIdentify[i][j].struShape.sName,
                sizeof(struCnfp.struGroupIdentify[i][j].struShape.sName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(0),
                    struCnfp.struGroupIdentify[i][j].struShape.sName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                    i, j, myString.sArithmeticName[ARITH_SHAPE].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.sName);
            }
            /* area min */
            if (struCnfp.struGroupIdentify[i][j].struShape.nAreaMin != _t_struCnfp.struGroupIdentify[i][j].struShape.nAreaMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(1),
                    struCnfp.struGroupIdentify[i][j].struShape.nAreaMin);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, area min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nAreaMin);
            }
            /* area max */
            if (struCnfp.struGroupIdentify[i][j].struShape.nAreaMax != _t_struCnfp.struGroupIdentify[i][j].struShape.nAreaMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(2),
                    struCnfp.struGroupIdentify[i][j].struShape.nAreaMax);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, area min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nAreaMin);
            }
            /* mode */
            if (struCnfp.struGroupIdentify[i][j].struShape.nMode != _t_struCnfp.struGroupIdentify[i][j].struShape.nMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(3),
                    struCnfp.struGroupIdentify[i][j].struShape.nMode);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, mode: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nMode);
            }
            /* length name */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struShape.sLengthName, _t_struCnfp.struGroupIdentify[i][j].struShape.sLengthName,
                sizeof(struCnfp.struGroupIdentify[i][j].struShape.sLengthName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(4),
                    struCnfp.struGroupIdentify[i][j].struShape.sLengthName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LENGTH].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.sLengthName);
            }

            /* length min */
            if (struCnfp.struGroupIdentify[i][j].struShape.nLengthMin != _t_struCnfp.struGroupIdentify[i][j].struShape.nLengthMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(5),
                    struCnfp.struGroupIdentify[i][j].struShape.nLengthMin);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, length min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LENGTH].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nLengthMin);
            }
            /* length max */
            if (struCnfp.struGroupIdentify[i][j].struShape.nLengthMax != _t_struCnfp.struGroupIdentify[i][j].struShape.nLengthMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(6),
                    struCnfp.struGroupIdentify[i][j].struShape.nLengthMax);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, length max: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LENGTH].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nLengthMax);
            }
            /* length mode */
            if (struCnfp.struGroupIdentify[i][j].struShape.nLengthMode != _t_struCnfp.struGroupIdentify[i][j].struShape.nLengthMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(7),
                    struCnfp.struGroupIdentify[i][j].struShape.nLengthMode);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, length mode: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LENGTH].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nLengthMode);
            }

            /* length limit */
            if (struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit != _t_struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(8),
                    struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, length limit: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LENGTH].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nLengthLimit);
            }
            /* length limit */
            if (struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit != _t_struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(2).arg(9),
                    struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, length limit: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE].toStdString(), struCnfp.struGroupIdentify[i][j].struShape.nAreaLimit);
            }
            /* scale */
            for (int k = 0; k < 2; k++)
            {
                /* name */
                if (memcmp(struCnfp.struGroupIdentify[i][j].struScale[k].sName, _t_struCnfp.struGroupIdentify[i][j].struScale[k].sName,
                    sizeof(struCnfp.struGroupIdentify[i][j].struScale[k].sName)) != 0)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(0 + 4 * k),
                        struCnfp.struGroupIdentify[i][j].struScale[k].sName);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                        i, j, myString.sArithmeticName[ARITH_SCALE + k * (ARITH_SCALE - ARITH_SCALE_B)].toStdString(), struCnfp.struGroupIdentify[i][j].struScale[k].sName);
                }
                /* mode */
                if (struCnfp.struGroupIdentify[i][j].struScale[k].nMode != _t_struCnfp.struGroupIdentify[i][j].struScale[k].nMode)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(1 + 4 * k), struCnfp.struGroupIdentify[i][j].struScale[k].nMode);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, mode: %d",
                        i, j, myString.sArithmeticName[ARITH_SCALE + k * (ARITH_SCALE - ARITH_SCALE_B)].toStdString(), struCnfp.struGroupIdentify[i][j].struScale[k].nMode);
                }
                /* value */
                if (struCnfp.struGroupIdentify[i][j].struScale[k].nValue != _t_struCnfp.struGroupIdentify[i][j].struScale[k].nValue)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(2 + 4 * k),
                        struCnfp.struGroupIdentify[i][j].struScale[k].nValue);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens: %d",
                        i, j, myString.sArithmeticName[ARITH_SCALE + k * (ARITH_SCALE - ARITH_SCALE_B)].toStdString(), struCnfp.struGroupIdentify[i][j].struScale[k].nValue);
                }
                /* value limit */
                if (struCnfp.struGroupIdentify[i][j].struScale[k].nValueLimit != _t_struCnfp.struGroupIdentify[i][j].struScale[k].nValueLimit)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(3).arg(3 + 4 * k),
                        struCnfp.struGroupIdentify[i][j].struScale[k].nValueLimit);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, limit: %d",
                        i, j, myString.sArithmeticName[ARITH_SCALE + k * (ARITH_SCALE - ARITH_SCALE_B)].toStdString(), struCnfp.struGroupIdentify[i][j].struScale[k].nValueLimit);
                }
            }

            /* bud */
            for (int k = 0; k < 2; k++)
            {
                /* name */
                if (memcmp(struCnfp.struGroupIdentify[i][j].struBud[k].sName, _t_struCnfp.struGroupIdentify[i][j].struBud[k].sName,
                    sizeof(struCnfp.struGroupIdentify[i][j].struBud[k].sName)) != 0)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(0),
                        struCnfp.struGroupIdentify[i][j].struBud[k].sName);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                        i, j, myString.sArithmeticName[ARITH_BUD_1 + k].toStdString(), struCnfp.struGroupIdentify[i][j].struBud[k].sName);
                }
                /* row */
                if (struCnfp.struGroupIdentify[i][j].struBud[k].nRow != _t_struCnfp.struGroupIdentify[i][j].struBud[k].nRow)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(1),
                        struCnfp.struGroupIdentify[i][j].struBud[k].nRow);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, row: %d",
                        i, j, myString.sArithmeticName[ARITH_BUD_1 + k].toStdString(), struCnfp.struGroupIdentify[i][j].struBud[k].nRow);
                }
                /* column */
                if (struCnfp.struGroupIdentify[i][j].struBud[k].nColumn != _t_struCnfp.struGroupIdentify[i][j].struBud[k].nColumn)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(2),
                        struCnfp.struGroupIdentify[i][j].struBud[k].nColumn);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, column: %d",
                        i, j, myString.sArithmeticName[ARITH_BUD_1 + k].toStdString(), struCnfp.struGroupIdentify[i][j].struBud[k].nColumn);
                }
                /* percent */
                if (struCnfp.struGroupIdentify[i][j].struBud[k].nPercent != _t_struCnfp.struGroupIdentify[i][j].struBud[k].nPercent)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(3),
                        struCnfp.struGroupIdentify[i][j].struBud[k].nPercent);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure percent: %d",
                        i, j, myString.sArithmeticName[ARITH_BUD_1 + k].toStdString(), struCnfp.struGroupIdentify[i][j].struBud[k].nPercent);
                }
                /* percent limit */
                if (struCnfp.struGroupIdentify[i][j].struBud[k].nPercentLimit != _t_struCnfp.struGroupIdentify[i][j].struBud[k].nPercentLimit)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(4),
                        struCnfp.struGroupIdentify[i][j].struBud[k].nPercentLimit);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure limit: %d",
                        i, j, myString.sArithmeticName[ARITH_BUD_1 + k].toStdString(), struCnfp.struGroupIdentify[i][j].struBud[k].nPercentLimit);
                }
                /* area */
                if (struCnfp.struGroupIdentify[i][j].struBud[k].nArea != _t_struCnfp.struGroupIdentify[i][j].struBud[k].nArea)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(4).arg(k).arg(5),
                        struCnfp.struGroupIdentify[i][j].struBud[k].nArea);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, area(sens): %d",
                        i, j, myString.sArithmeticName[ARITH_BUD_1 + k].toStdString(), struCnfp.struGroupIdentify[i][j].struBud[k].nArea);
                }
            }

            /* peanut */
            for (int k = 0; k < 4; k++)
            {
                /* name */
                if (memcmp(struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName, _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName,
                    sizeof(struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName)) != 0)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(0),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].sName);
                }
                /* row */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nRow != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nRow)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(1),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nRow);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, row: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nRow);
                }
                /* column */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nColumn != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nColumn)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(2),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nColumn);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, column: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nColumn);
                }
                /* percent */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nPercent != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nPercent)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(3),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nPercent);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure percent: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nPercent);
                }
                /* sens min */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMin != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMin)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(4),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMin);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens min: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMin);
                }
                /* sens max */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMax != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMax)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(5),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMax);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens max: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensMax);
                }
                /* mode */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nMode != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nMode)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(6),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nMode);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, mode: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nMode);
                }
                /* sens reserved */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensReserved != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensReserved)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(7),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensReserved);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens reserved: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].nSensReserved);
                }

                /* reserved */
                    /* discolor 1 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nDiscolor1 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nDiscolor1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(0),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nDiscolor1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nDiscolor1);
                }
                /* sens min 1 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin1 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(1),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens min: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin1);
                }
                /* sens max 1 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax1 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(2),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens max: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax1);
                }
                /* mode 1 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode1 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(3),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor mode: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode1);
                }
                /* color 2 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nColor2 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nColor2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(4),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nColor2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey color: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nColor2);
                }
                /* sens min 2 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin2 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(5),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens min: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMin2);
                }
                /* sens max 2 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax2 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(6),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens max: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nSensMax2);
                }
                /* mode 2 */
                if (struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode2 != _t_struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(5).arg(k).arg(8).arg(7),
                        struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey mode: %d",
                        i, j, myString.sArithmeticName[ARITH_PEANUT_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struPeanutAbcd[k].struReserved.nMode2);
                }

            }

            /* intel */
            for (int k = 0; k < 4; k++)
            {
                /* name */
                if (memcmp(struCnfp.struGroupIdentify[i][j].struIntel[k].sName, _t_struCnfp.struGroupIdentify[i][j].struIntel[k].sName,
                    sizeof(struCnfp.struGroupIdentify[i][j].struIntel[k].sName)) != 0)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(0),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].sName);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].sName);
                }
                /* row */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nRow != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nRow)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(1),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nRow);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, row: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nRow);
                }
                /* column */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(2),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, column: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nColumn);
                }
                /* percent */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(3),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, percent: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nPercent);
                }
                /* balance */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nEnable != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nEnable)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(4),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nEnable);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, balance: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nEnable);
                }
                /* sens */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nSens != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nSens)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(5),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nSens);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nSens);
                }
                /* whole edge */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nWholeEdge != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nWholeEdge)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(6),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nWholeEdge);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, whole edge: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nWholeEdge);
                }


                /* reserved */
                    /* discolor 1 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nDiscolor1 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nDiscolor1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(0),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nDiscolor1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nDiscolor1);

                }
                /* sens min 1 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin1 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(1),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens min: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin1);
                }
                /* sens max 1 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax1 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(2),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens max: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax1);
                }
                /* mode 1 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode1 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode1)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(3),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode1);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor mode: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode1);
                }
                /* color 2 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nColor2 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nColor2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(4),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nColor2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey color: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nColor2);
                }
                /* sens min 2 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin2 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(5),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens min: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMin2);
                }
                /* sens max 2 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax2 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(6),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens max: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nSensMax2);
                }
                /* mode 2 */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode2 != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode2)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(7),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode2);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey mode: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nMode2);
                }
                /* percent */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nPercent != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nPercent)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(7).arg(8),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nPercent);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey percent: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].struReserved.nPercent);
                }

                /* mode */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nModeDfl != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nModeDfl)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(8),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nModeDfl);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sort mode dfl: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nModeDfl);
                }

                /* sensRatio */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nSensRatio != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nSensRatio)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(10),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nSensRatio);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, nSensRatio: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nSensRatio);
                }
                /* nIntelType */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nIntelType != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nIntelType)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(11),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nIntelType);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, nIntelType: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nIntelType);
                }

                for (int m = 0; m < 2; m++)
                {
                    /* normal */
                        /* cons */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nCons[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nCons[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(9).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nCons[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, constant[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nCons[m]);
                    }

                    /* red */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo1[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo1[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(10).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo1[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, quo1[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo1[m]);
                    }

                    /* green */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo2[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo2[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(11).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo2[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, quo2[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo2[m]);
                    }

                    /* blue */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo3[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo3[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(12).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo3[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, quo3[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuo3[m]);
                    }

                    /* multi mode */
                        /* cons */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nConsDfl[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nConsDfl[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(13).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nConsDfl[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, dfl constant[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nConsDfl[m]);
                    }
                    /* r */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoR[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoR[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(14).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoR[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, R quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoR[m]);

                    }
                    /* g */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoG[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoG[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(15).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoG[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, G quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoG[m]);
                    }
                    /* b */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoB[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoB[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(16).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoB[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, B quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoB[m]);
                    }
                    /* rr */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRR[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRR[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(17).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRR[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, RR quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRR[m]);
                    }
                    /* gg */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGG[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGG[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(18).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGG[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, GG quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGG[m]);
                    }
                    /* bb */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoBB[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoBB[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(19).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoBB[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, BB quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoBB[m]);
                    }
                    /* rg */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRG[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRG[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(20).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRG[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, RG quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRG[m]);
                    }
                    /* rb */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRB[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRB[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(21).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRB[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, RB quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoRB[m]);
                    }
                    /* gb */
                    if (struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGB[m] != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGB[m])
                    {
                        setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(22).arg(m),
                            struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGB[m]);
                        LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, GB quo[%d]: %d",
                            i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), m, struCnfp.struGroupIdentify[i][j].struIntel[k].nQuoGB[m]);
                    }
                }
                /* ratio mode */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nRatioMode != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nRatioMode)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(23),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nRatioMode);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sort mode ratio: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nRatioMode);
                }
                /* rowRes */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nRowRes != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nRowRes)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(24),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nRowRes);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, rowRes: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nRowRes);
                }
                /* columnRes */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nColumnRes != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nColumnRes)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(25),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nColumnRes);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, columnRes: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nColumnRes);
                }
                /* percentRes */
                if (struCnfp.struGroupIdentify[i][j].struIntel[k].nPercentRes != _t_struCnfp.struGroupIdentify[i][j].struIntel[k].nPercentRes)
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(6).arg(k).arg(26),
                        struCnfp.struGroupIdentify[i][j].struIntel[k].nPercentRes);
                    LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, percentRes: %d",
                        i, j, myString.sArithmeticName[ARITH_INTEL_A + k].toStdString(), struCnfp.struGroupIdentify[i][j].struIntel[k].nPercentRes);
                }
            }


            /* melon */
                /* white sens */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(0),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, white sens: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteSens);
            }
            /* white prop min */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(1),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, white propt min: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMin);
            }
            /* white prop max */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(2),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, white propt max: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhitePropMax);
            }
            /* white mode */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(3),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, white mode: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nWhiteMode);
            }
            /* edge red sens */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(4),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, edge red sens: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedSens);
            }
            /* edge red ratio */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(5),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, edge red ratio: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeRedRatio);
            }
            /* edge damage sens */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(6),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, edge damage sens: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedSens);
            }
            /* edge damage ratio */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(7),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, edge damage ratio: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nEdgeDamagedRatio);
            }
            /* rugged 1 sens */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(8),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, rugged 1 sens: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Sens);
            }
            /* rugged 1 save */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Save != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Save)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(9),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Save);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, rugged 1 save: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged1Save);
            }
            /* rugged 2 sens */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(10),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, rugged 2 sens: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Sens);
            }
            /* rugged 2 save */
            if (struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save != _t_struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(8).arg(11),
                    struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, rugged 2 save: %d",
                    i, j, myString.sArithmeticName[ARITH_WATERMELON].toStdString(), struCnfp.struGroupIdentify[i][j].struMatMelon.nRugged2Save);
            }

            /* RS机型相关参数 */
            if (struCnfp.struGroupIdentify[i][j].struRice.nSens != _t_struCnfp.struGroupIdentify[i][j].struRice.nSens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(0),
                    struCnfp.struGroupIdentify[i][j].struRice.nSens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master sens: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nSens);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor != _t_struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(2),
                    struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master bad discolor: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nBadDiscolor);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nRow != _t_struCnfp.struGroupIdentify[i][j].struRice.nRow)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(3),
                    struCnfp.struGroupIdentify[i][j].struRice.nRow);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master row: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nRow);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nCol != _t_struCnfp.struGroupIdentify[i][j].struRice.nCol)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(4),
                    struCnfp.struGroupIdentify[i][j].struRice.nCol);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master column: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nCol);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nYellowArea != _t_struCnfp.struGroupIdentify[i][j].struRice.nYellowArea)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(5),
                    struCnfp.struGroupIdentify[i][j].struRice.nYellowArea);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master area: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nYellowArea);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nBadWidth != _t_struCnfp.struGroupIdentify[i][j].struRice.nBadWidth)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(6),
                    struCnfp.struGroupIdentify[i][j].struRice.nBadWidth);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master bad width: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nBadWidth);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nDarkSens != _t_struCnfp.struGroupIdentify[i][j].struRice.nDarkSens)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(12),
                    struCnfp.struGroupIdentify[i][j].struRice.nDarkSens);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, master dark sens: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nDarkSens);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1 != _t_struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(14),
                    struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, slave reserve percent: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nReservePercent1);
            }
            if (struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2 != _t_struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(9).arg(15),
                    struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, slave reserve percent: %d",
                    i, j, myString.sArithmeticName[ARITH_RICE].toStdString(), struCnfp.struGroupIdentify[i][j].struRice.nReservePercent2);
            }

            /* 玉米定制算法 */
            /* maize */
              /* nRow */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nRow != _t_struCnfp.struGroupIdentify[i][j].struMaize.nRow)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(0),
                    struCnfp.struGroupIdentify[i][j].struMaize.nRow);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, row: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nRow);
            }
            /* nPercentTotal */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal != _t_struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(1),
                    struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, total pure percent: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nPercentTotal);
            }
            /* nPercentGood */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood != _t_struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(2),
                    struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, total good percent: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nPercentGood);
            }
            /* nPercentBad1 */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1 != _t_struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(3),
                    struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, total bad 1 percent: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad1);
            }
            /* nPercentBad2 */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2 != _t_struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(4),
                    struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, total bad 2 percent: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nPercentBad2);
            }
            /* nSensMin1 */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1 != _t_struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(5),
                    struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens 1: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nSensMin1);
            }
            /* nSensProport */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nSensProport != _t_struCnfp.struGroupIdentify[i][j].struMaize.nSensProport)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(6),
                    struCnfp.struGroupIdentify[i][j].struMaize.nSensProport);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, proport sens: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nSensProport);
            }
            /* nSensMin2 */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2 != _t_struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(7),
                    struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, sens 2: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nSensMin2);
            }
            /* nColumn */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nColumn != _t_struCnfp.struGroupIdentify[i][j].struMaize.nColumn)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(8),
                    struCnfp.struGroupIdentify[i][j].struMaize.nColumn);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, column: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nColumn);
            }
            /* nColumn */
            if (struCnfp.struGroupIdentify[i][j].struMaize.nThreshold != _t_struCnfp.struGroupIdentify[i][j].struMaize.nThreshold)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(10).arg(9),
                    struCnfp.struGroupIdentify[i][j].struMaize.nThreshold);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, threshold: %d",
                    i, j, myString.sArithmeticName[ARITH_MAIZE].toStdString(), struCnfp.struGroupIdentify[i][j].struMaize.nThreshold);
            }

            /* 选杆算法 */
                /* sName */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struPole.sName, _t_struCnfp.struGroupIdentify[i][j].struPole.sName,
                sizeof(struCnfp.struGroupIdentify[i][j].struPole.sName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(0),
                    struCnfp.struGroupIdentify[i][j].struPole.sName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material name: %s",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.sName);
            }
            /* nArea */
            if (struCnfp.struGroupIdentify[i][j].struPole.nArea != _t_struCnfp.struGroupIdentify[i][j].struPole.nArea)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(1),
                    struCnfp.struGroupIdentify[i][j].struPole.nArea);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, area: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nArea);
            }
            /* nPercent */
            if (struCnfp.struGroupIdentify[i][j].struPole.nPercent != _t_struCnfp.struGroupIdentify[i][j].struPole.nPercent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(2),
                    struCnfp.struGroupIdentify[i][j].struPole.nPercent);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure percent: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nPercent);
            }
            /* nWidthMin */
            if (struCnfp.struGroupIdentify[i][j].struPole.nWidthMin != _t_struCnfp.struGroupIdentify[i][j].struPole.nWidthMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(3),
                    struCnfp.struGroupIdentify[i][j].struPole.nWidthMin);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, width min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nWidthMin);
            }
            /* nWidthMax */
            if (struCnfp.struGroupIdentify[i][j].struPole.nWidthMax != _t_struCnfp.struGroupIdentify[i][j].struPole.nWidthMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(4),
                    struCnfp.struGroupIdentify[i][j].struPole.nWidthMax);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, width max: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nWidthMax);
            }

            /* reserved */
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(0),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nDiscolor1);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(1),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor mode: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode1);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(2),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin1);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(3),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved discolor sens max: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax1);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(4),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey color: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nColor2);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(5),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey mode: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nMode2);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(6),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMin2);
            }
            if (struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2 != _t_struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(11).arg(5).arg(7),
                    struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, reserved grey sens max: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.struReserved.nSensMax2);
            }

            /* wide pole name */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struPole.sPoleName, _t_struCnfp.struGroupIdentify[i][j].struPole.sPoleName,
                sizeof(struCnfp.struGroupIdentify[i][j].struPole.sPoleName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(6),
                    struCnfp.struGroupIdentify[i][j].struPole.sPoleName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material(wide pole) name: %s",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.sPoleName);
            }

            /* wide pole area */
            if (struCnfp.struGroupIdentify[i][j].struPole.nPoleArea != _t_struCnfp.struGroupIdentify[i][j].struPole.nPoleArea)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(7),
                    struCnfp.struGroupIdentify[i][j].struPole.nPoleArea);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, wide pole area: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nPoleArea);
            }

            /* wide pole percent */
            if (struCnfp.struGroupIdentify[i][j].struPole.nPolePercent != _t_struCnfp.struGroupIdentify[i][j].struPole.nPolePercent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(8),
                    struCnfp.struGroupIdentify[i][j].struPole.nPolePercent);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, wide pole percent: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nPolePercent);
            }

            /* wide pole min */
            if (struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin != _t_struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(9),
                    struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, wide pole width min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMin);
            }

            /* wide pole max */
            if (struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax != _t_struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(11).arg(10),
                    struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, wide pole width max: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_POLE].toStdString(), struCnfp.struGroupIdentify[i][j].struPole.nPoleWidthMax);
            }

            /* 选片算法 */
                /* slice name */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struSlice.sName, _t_struCnfp.struGroupIdentify[i][j].struSlice.sName,
                sizeof(struCnfp.struGroupIdentify[i][j].struSlice.sName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(12).arg(0),
                    struCnfp.struGroupIdentify[i][j].struSlice.sName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material(slice name) name: %s",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_SLICE].toStdString(), struCnfp.struGroupIdentify[i][j].struSlice.sName);
            }
            /* slice area */
            if (struCnfp.struGroupIdentify[i][j].struSlice.nArea != _t_struCnfp.struGroupIdentify[i][j].struSlice.nArea)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(12).arg(1),
                    struCnfp.struGroupIdentify[i][j].struSlice.nArea);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, area: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_SLICE].toStdString(), struCnfp.struGroupIdentify[i][j].struSlice.nArea);
            }

            /* 选芽算法 */
                /* leaf name */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struLeaf.sName, _t_struCnfp.struGroupIdentify[i][j].struLeaf.sName,
                sizeof(struCnfp.struGroupIdentify[i][j].struLeaf.sName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(0),
                    struCnfp.struGroupIdentify[i][j].struLeaf.sName);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, material(leaf name) name: %s",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LEAF].toStdString(), struCnfp.struGroupIdentify[i][j].struLeaf.sName);
            }

            /* leaf area */
            if (struCnfp.struGroupIdentify[i][j].struLeaf.nArea != _t_struCnfp.struGroupIdentify[i][j].struLeaf.nArea)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(1),
                    struCnfp.struGroupIdentify[i][j].struLeaf.nArea);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, area: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LEAF].toStdString(), struCnfp.struGroupIdentify[i][j].struLeaf.nArea);
            }

            /* leaf percent */
            if (struCnfp.struGroupIdentify[i][j].struLeaf.nPercent != _t_struCnfp.struGroupIdentify[i][j].struLeaf.nPercent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(2),
                    struCnfp.struGroupIdentify[i][j].struLeaf.nPercent);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, pure percent: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LEAF].toStdString(), struCnfp.struGroupIdentify[i][j].struLeaf.nPercent);
            }

            /* leaf width min */
            if (struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin != _t_struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(3),
                    struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, width min: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LEAF].toStdString(), struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin);
            }

            /* leaf width max */
            if (struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMax != _t_struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(4),
                    struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMax);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, width max: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LEAF].toStdString(), struCnfp.struGroupIdentify[i][j].struLeaf.nWidthMin);
            }

            /* leaf threshold */
            if (struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold != _t_struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(13).arg(5),
                    struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold);
                LOG_INFO_FMT("level: %d, identify group: %d, arith name: %s, threshold: %d",
                    i, j, myString.sArithmeticName[ARITH_SHAPE_LEAF].toStdString(), struCnfp.struGroupIdentify[i][j].struLeaf.nThreshold);
            }

            /* 保留算法 */
            if (memcmp(struCnfp.struGroupIdentify[i][j].struReserved.sName, _t_struCnfp.struGroupIdentify[i][j].struReserved.sName,
                sizeof(struCnfp.struGroupIdentify[i][j].struReserved.sName)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(0),
                    struCnfp.struGroupIdentify[i][j].struReserved.sName);
            }
            if (struCnfp.struGroupIdentify[i][j].struReserved.nRow != _t_struCnfp.struGroupIdentify[i][j].struReserved.nRow)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(1),
                    struCnfp.struGroupIdentify[i][j].struReserved.nRow);
            }
            if (struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_s != _t_struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_s)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(2),
                    struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_s);
            }
            if (struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_r != _t_struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_r)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(14).arg(3),
                    struCnfp.struGroupIdentify[i][j].struReserved.nThreshold_r);
            }
            /* 糖果算法 */
            if (struCnfp.struGroupIdentify[i][j].struCandy.nRation1 != _t_struCnfp.struGroupIdentify[i][j].struCandy.nRation1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(0),
                    struCnfp.struGroupIdentify[i][j].struCandy.nRation1);
            }
            if (struCnfp.struGroupIdentify[i][j].struCandy.nRation2 != _t_struCnfp.struGroupIdentify[i][j].struCandy.nRation2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(1),
                    struCnfp.struGroupIdentify[i][j].struCandy.nRation2);
            }
            if (struCnfp.struGroupIdentify[i][j].struCandy.nRow != _t_struCnfp.struGroupIdentify[i][j].struCandy.nRow)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(2),
                    struCnfp.struGroupIdentify[i][j].struCandy.nRow);
            }
            if (struCnfp.struGroupIdentify[i][j].struCandy.nCycles != _t_struCnfp.struGroupIdentify[i][j].struCandy.nCycles)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(15).arg(3),
                    struCnfp.struGroupIdentify[i][j].struCandy.nCycles);
            }

            /* 开心果算法 */

            if (struCnfp.struGroupIdentify[i][j].struPistachio.nAreaMin != _t_struCnfp.struGroupIdentify[i][j].struPistachio.nAreaMin)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(0),
                    struCnfp.struGroupIdentify[i][j].struPistachio.nAreaMin);
            }
            if (struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_1 != _t_struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(1),
                    struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_1);
            }
            if (struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_2 != _t_struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(2),
                    struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_2);
            }
            if (struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_3 != _t_struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_3)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(16).arg(3),
                    struCnfp.struGroupIdentify[i][j].struPistachio.nGrayThreshold_3);
            }

            if (memcmp(struCnfp.struGroupIdentify[i][j].struAi.modelId, _t_struCnfp.struGroupIdentify[i][j].struAi.modelId,
                sizeof(struCnfp.struGroupIdentify[i][j].struAi.modelId)) != 0)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5-%6").arg(3).arg(14).arg(i).arg(j).arg(17).arg(0),
                    struCnfp.struGroupIdentify[i][j].struAi.modelId);
            }

            //            for(int k=0; k<MAX_PARA; k++){
            //                if (struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].id != _t_struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].id) {
            //                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(17).arg(1).arg(k),
            //                            struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].id);
            //                }
            //                if (memcmp(struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName,_t_struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName,
            //                           sizeof(struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName)) != 0) {
            //                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(17).arg(2).arg(k),
            //                            struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].materialName);
            //                }
            //                if (struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].threshold != _t_struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].threshold) {
            //                    setting.setValue(QString("%1-%2-%3-%4-%5-%6-%7").arg(3).arg(14).arg(i).arg(j).arg(17).arg(3).arg(k),
            //                            struCnfp.struGroupIdentify[i][j].struAi.struAiPara[k].threshold);
            //            }
            //          }
        }
    }

    /* tick parameters */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_GROUP_TICK; j++)
        {
            /* eject duration */
            if (struCnfp.struGroupTick[i][j].nEjectDuration != _t_struCnfp.struGroupTick[i][j].nEjectDuration)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(0), struCnfp.struGroupTick[i][j].nEjectDuration);
                LOG_INFO_FMT("level: %d, reject group: %d, eject duration: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDuration);
            }

            /* eject delay */
            if (struCnfp.struGroupTick[i][j].nEjectDelay != _t_struCnfp.struGroupTick[i][j].nEjectDelay)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(1), struCnfp.struGroupTick[i][j].nEjectDelay);
                LOG_INFO_FMT("level: %d, reject group: %d, eject delay: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDelay);
            }

            /* front & rear mode */
            if (struCnfp.struGroupTick[i][j].nFrontRearMode != _t_struCnfp.struGroupTick[i][j].nFrontRearMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(2), struCnfp.struGroupTick[i][j].nFrontRearMode);
                LOG_INFO_FMT("level: %d, reject group: %d, front & rear mode: %d", i, j, struCnfp.struGroupTick[i][j].nFrontRearMode);
            }

            /* infect enable */
            if (struCnfp.struGroupTick[i][j].nImpuredFlag != _t_struCnfp.struGroupTick[i][j].nImpuredFlag)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(3), struCnfp.struGroupTick[i][j].nImpuredFlag);
                LOG_INFO_FMT("level: %d, reject group: %d, infect enable: %d", i, j, struCnfp.struGroupTick[i][j].nImpuredFlag);
            }

            /* eject width */
            if (struCnfp.struGroupTick[i][j].nEjwidth != _t_struCnfp.struGroupTick[i][j].nEjwidth)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(4), struCnfp.struGroupTick[i][j].nEjwidth);
                LOG_INFO_FMT("level: %d, reject group: %d, eject width: %d", i, j, struCnfp.struGroupTick[i][j].nEjwidth);
            }

            /* edge cut */
            if (struCnfp.struGroupTick[i][j].nEdgeCut != _t_struCnfp.struGroupTick[i][j].nEdgeCut)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(5), struCnfp.struGroupTick[i][j].nEdgeCut);
                LOG_INFO_FMT("level: %d, reject group: %d, eject cut: %d", i, j, struCnfp.struGroupTick[i][j].nEdgeCut);
            }

            /* bad num */
            if (struCnfp.struGroupTick[i][j].nBadNum != _t_struCnfp.struGroupTick[i][j].nBadNum)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(6), struCnfp.struGroupTick[i][j].nBadNum);
                LOG_INFO_FMT("level: %d, reject group: %d, bad num: %d", i, j, struCnfp.struGroupTick[i][j].nBadNum);
            }

            /* reverse sort */
            if (struCnfp.struGroupTick[i][j].nReverseSort != _t_struCnfp.struGroupTick[i][j].nReverseSort)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(7), struCnfp.struGroupTick[i][j].nReverseSort);
                LOG_INFO_FMT("level: %d, reject group: %d, reverse sort: %d", i, j, struCnfp.struGroupTick[i][j].nReverseSort);
            }

            /* material width */
            if (struCnfp.struGroupTick[i][j].nMatWidth != _t_struCnfp.struGroupTick[i][j].nMatWidth)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(8), struCnfp.struGroupTick[i][j].nMatWidth);
                LOG_INFO_FMT("level: %d, reject group: %d, material width: %d", i, j, struCnfp.struGroupTick[i][j].nMatWidth);
            }

            /* good num */
            if (struCnfp.struGroupTick[i][j].nGoodNum != _t_struCnfp.struGroupTick[i][j].nGoodNum)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(9), struCnfp.struGroupTick[i][j].nGoodNum);
                LOG_INFO_FMT("level: %d, reject group: %d, good num: %d", i, j, struCnfp.struGroupTick[i][j].nGoodNum);
            }

            /* edge revise */
            if (struCnfp.struGroupTick[i][j].nEdgeRevise != _t_struCnfp.struGroupTick[i][j].nEdgeRevise)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(10), struCnfp.struGroupTick[i][j].nEdgeRevise);
                LOG_INFO_FMT("level: %d, reject group: %d, edge revise: %d", i, j, struCnfp.struGroupTick[i][j].nEdgeRevise);
            }

            /* enable balance */
            if (struCnfp.struGroupTick[i][j].nEnableBalance != _t_struCnfp.struGroupTick[i][j].nEnableBalance)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(11), struCnfp.struGroupTick[i][j].nEnableBalance);
                LOG_INFO_FMT("level: %d, reject group: %d, enable balance: %d", i, j, struCnfp.struGroupTick[i][j].nEnableBalance);
            }

            /* 吹气次数限制 */
            if (struCnfp.struGroupTick[i][j].nEjectTimesLimit != _t_struCnfp.struGroupTick[i][j].nEjectTimesLimit)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(12),
                    struCnfp.struGroupTick[i][j].nEjectTimesLimit);
                LOG_INFO_FMT("level: %d, reject group: %d, eject times limit: %d", i, j, struCnfp.struGroupTick[i][j].nEjectTimesLimit);
            }

            /* eject degauss */
            if (struCnfp.struGroupTick[i][j].nEjectDegauss != _t_struCnfp.struGroupTick[i][j].nEjectDegauss)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(13), struCnfp.struGroupTick[i][j].nEjectDegauss);
                LOG_INFO_FMT("level: %d, reject group: %d, eject degauss: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDegauss);
            }

            /* material area */
            if (struCnfp.struGroupTick[i][j].nMatArea != _t_struCnfp.struGroupTick[i][j].nMatArea)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(14), struCnfp.struGroupTick[i][j].nMatArea);
                LOG_INFO_FMT("level: %d, reject group: %d, material area: %d", i, j, struCnfp.struGroupTick[i][j].nMatArea);
            }

            /* eject duration */
            if (struCnfp.struGroupTick[i][j].nEjectDurationCent != _t_struCnfp.struGroupTick[i][j].nEjectDurationCent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(30), struCnfp.struGroupTick[i][j].nEjectDurationCent);
                LOG_INFO_FMT("level: %d, reject group: %d, eject duration cent: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDurationCent);
            }

            /* eject delay */
            if (struCnfp.struGroupTick[i][j].nEjectDelayCent != _t_struCnfp.struGroupTick[i][j].nEjectDelayCent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(31), struCnfp.struGroupTick[i][j].nEjectDelayCent);
                LOG_INFO_FMT("level: %d, reject group: %d, eject delay cent: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDelayCent);
            }

            /* infect num */
            if (struCnfp.struGroupTick[i][j].nImpured != _t_struCnfp.struGroupTick[i][j].nImpured)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(34), struCnfp.struGroupTick[i][j].nImpured);
                LOG_INFO_FMT("level: %d, reject group: %d, infect num: %d", i, j, struCnfp.struGroupTick[i][j].nImpured);
            }

            /* Corrupted num */
            if (struCnfp.struGroupTick[i][j].nCorrupted != _t_struCnfp.struGroupTick[i][j].nCorrupted)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(35), struCnfp.struGroupTick[i][j].nCorrupted);
                LOG_INFO_FMT("level: %d, reject group: %d, corrupted num: %d", i, j, struCnfp.struGroupTick[i][j].nCorrupted);
            }

            /* Expanded num */
            if (struCnfp.struGroupTick[i][j].nExpanded != _t_struCnfp.struGroupTick[i][j].nExpanded)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(36), struCnfp.struGroupTick[i][j].nExpanded);
                LOG_INFO_FMT("level: %d, reject group: %d, expanded num: %d", i, j, struCnfp.struGroupTick[i][j].nExpanded);
            }

            /* shape infect num */
            if (struCnfp.struGroupTick[i][j].nShapeImpured != _t_struCnfp.struGroupTick[i][j].nShapeImpured)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(37), struCnfp.struGroupTick[i][j].nShapeImpured);
                LOG_INFO_FMT("level: %d, reject group: %d, shape infect num: %d", i, j, struCnfp.struGroupTick[i][j].nShapeImpured);
            }

            /* shape Corrupted num */
            if (struCnfp.struGroupTick[i][j].nShapeCorrupted != _t_struCnfp.struGroupTick[i][j].nShapeCorrupted)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(38), struCnfp.struGroupTick[i][j].nShapeCorrupted);
                LOG_INFO_FMT("level: %d, reject group: %d, shape corrupted num: %d", i, j, struCnfp.struGroupTick[i][j].nShapeCorrupted);
            }

            /* shape Expanded num */
            if (struCnfp.struGroupTick[i][j].nShapeExpanded != _t_struCnfp.struGroupTick[i][j].nShapeExpanded)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(39), struCnfp.struGroupTick[i][j].nShapeExpanded);
                LOG_INFO_FMT("level: %d, reject group: %d, shape expanded num: %d", i, j, struCnfp.struGroupTick[i][j].nShapeExpanded);
            }

            /* 辅配吹气时间 */
            if (struCnfp.struGroupTick[i][j].nAssistEjectTime != _t_struCnfp.struGroupTick[i][j].nAssistEjectTime)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(40), struCnfp.struGroupTick[i][j].nAssistEjectTime);
                LOG_INFO_FMT("level: %d, reject group: %d, assist eject time: %d", i, j, struCnfp.struGroupTick[i][j].nAssistEjectTime);
            }

            /* 辅配延迟时间 */
            if (struCnfp.struGroupTick[i][j].nAssistDelayTime != _t_struCnfp.struGroupTick[i][j].nAssistDelayTime)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(41), struCnfp.struGroupTick[i][j].nAssistDelayTime);
                LOG_INFO_FMT("level: %d, reject group: %d, assist delay time: %d", i, j, struCnfp.struGroupTick[i][j].nAssistDelayTime);
            }

            /* 去除拼接的边缘切除圈数 */
            if (struCnfp.struGroupTick[i][j].nRemoveJoint != _t_struCnfp.struGroupTick[i][j].nRemoveJoint)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(42), struCnfp.struGroupTick[i][j].nRemoveJoint);
                LOG_INFO_FMT("level: %d, reject group: %d, remove joint: %d", i, j, struCnfp.struGroupTick[i][j].nRemoveJoint);
            }

            if (struCnfp.struGroupTick[i][j].nRemoveJoint != _t_struCnfp.struGroupTick[i][j].nRemoveJoint)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(42), struCnfp.struGroupTick[i][j].nRemoveJoint);
                LOG_INFO_FMT("level: %d, reject group: %d, remove joint: %d", i, j, struCnfp.struGroupTick[i][j].nRemoveJoint);
            }
            if (struCnfp.struGroupTick[i][j].nEjectDelayBias != _t_struCnfp.struGroupTick[i][j].nEjectDelayBias)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(43), struCnfp.struGroupTick[i][j].nEjectDelayBias);
                LOG_INFO_FMT("level: %d, reject group: %d, nEjectDelayBias: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDelayBias);
            }
            if (struCnfp.struGroupTick[i][j].nEjectDelayBiasCent != _t_struCnfp.struGroupTick[i][j].nEjectDelayBiasCent)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(44), struCnfp.struGroupTick[i][j].nEjectDelayBiasCent);
                LOG_INFO_FMT("level: %d, reject group: %d, nEjectDelayBiasCent: %d", i, j, struCnfp.struGroupTick[i][j].nEjectDelayBiasCent);
            }
            /* eject truncation */
            if (struCnfp.struGroupTick[i][j].nEjectTruncation != _t_struCnfp.struGroupTick[i][j].nEjectTruncation)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(45), struCnfp.struGroupTick[i][j].nEjectTruncation);
                LOG_INFO_FMT("level: %d, reject group: %d, eject truncation: %d", i, j, struCnfp.struGroupTick[i][j].nEjectTruncation);
            }
            /* Assist eject degauss */
            if (struCnfp.struGroupTick[i][j].nAssistEjectDegauss != _t_struCnfp.struGroupTick[i][j].nAssistEjectDegauss)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(47), struCnfp.struGroupTick[i][j].nAssistEjectDegauss);
                LOG_INFO_FMT("level: %d, reject group: %d,Assist eject degauss: %d", i, j, struCnfp.struGroupTick[i][j].nAssistEjectDegauss);
            }
            /* Assist eject truncation */
            if (struCnfp.struGroupTick[i][j].nAssistEjectTruncation != _t_struCnfp.struGroupTick[i][j].nAssistEjectTruncation)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(49), struCnfp.struGroupTick[i][j].nAssistEjectTruncation);
                LOG_INFO_FMT("level: %d, reject group: %d,Assist eject truncation: %d", i, j, struCnfp.struGroupTick[i][j].nAssistEjectTruncation);
            }
            /* 吹气宽度 huff width */
            if (struCnfp.struGroupTick[i][j].nhuffWidth != _t_struCnfp.struGroupTick[i][j].nhuffWidth)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(51), struCnfp.struGroupTick[i][j].nhuffWidth);
                LOG_INFO_FMT("level: %d, reject group: %d, huff Width: %d", i, j, struCnfp.struGroupTick[i][j].nhuffWidth);
            }

            /* CG前后视与或关系及正反选 */
            if (struCnfp.struGroupTick[i][j].nShapeLogicMode != _t_struCnfp.struGroupTick[i][j].nShapeLogicMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(54), struCnfp.struGroupTick[i][j].nShapeLogicMode);
            }
            if (struCnfp.struGroupTick[i][j].nShapeSortMode != _t_struCnfp.struGroupTick[i][j].nShapeSortMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(55), struCnfp.struGroupTick[i][j].nShapeSortMode);
            }

            /* 新R精选剔除模式参数 */
            if (struCnfp.struGroupTick[i][j].nEjectLenthMax != _t_struCnfp.struGroupTick[i][j].nEjectLenthMax)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(56), struCnfp.struGroupTick[i][j].nEjectLenthMax);
            }
            if (struCnfp.struGroupTick[i][j].nEjectLenthDif != _t_struCnfp.struGroupTick[i][j].nEjectLenthDif)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(57), struCnfp.struGroupTick[i][j].nEjectLenthDif);
            }
            if (struCnfp.struGroupTick[i][j].nEjectProportion1 != _t_struCnfp.struGroupTick[i][j].nEjectProportion1)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(58), struCnfp.struGroupTick[i][j].nEjectProportion1);
            }
            if (struCnfp.struGroupTick[i][j].nEjectProportion2 != _t_struCnfp.struGroupTick[i][j].nEjectProportion2)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(59), struCnfp.struGroupTick[i][j].nEjectProportion2);
            }
            if (struCnfp.struGroupTick[i][j].nEjectThresHold != _t_struCnfp.struGroupTick[i][j].nEjectThresHold)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(60), struCnfp.struGroupTick[i][j].nEjectThresHold);
            }
            if (struCnfp.struGroupTick[i][j].nTickMode != _t_struCnfp.struGroupTick[i][j].nTickMode)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(61), struCnfp.struGroupTick[i][j].nTickMode);
            }
            /* 三次反选圈数*/
            if (struCnfp.struGroupTick[i][j].nRounds != _t_struCnfp.struGroupTick[i][j].nRounds)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(62), struCnfp.struGroupTick[i][j].nRounds);
            }
            if (struCnfp.struGroupTick[i][j].aiEjectorDelay != _t_struCnfp.struGroupTick[i][j].aiEjectorDelay)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(63), struCnfp.struGroupTick[i][j].aiEjectorDelay);
            }
            if (struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay != _t_struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(64), struCnfp.struGroupTick[i][j].aiEjectorDynamicDelay);
            }
            if (struCnfp.struGroupTick[i][j].aiEjectorBlowTime != _t_struCnfp.struGroupTick[i][j].aiEjectorBlowTime)
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(15).arg(i).arg(j).arg(65), struCnfp.struGroupTick[i][j].aiEjectorBlowTime);
            }
        }
    }

    /* control parameters */
    for (int i = 0; i < MAX_CTRL; i++)
    {
        /* feeder */
        for (int j = 0; j < 10; j++)
        {
            if (struCnfp.struGroupCtrl[i].nFeeder[j] != _t_struCnfp.struGroupCtrl[i].nFeeder[j])
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(0).arg(j), struCnfp.struGroupCtrl[i].nFeeder[j]);
                LOG_INFO_FMT("control No.: %d, feeder No.: %d, feeder value: %d", i, j, struCnfp.struGroupCtrl[i].nFeeder[j]);
            }
        }

        /* wipe duration */
        if (struCnfp.struGroupCtrl[i].nWipeDuration != _t_struCnfp.struGroupCtrl[i].nWipeDuration)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(1), struCnfp.struGroupCtrl[i].nWipeDuration);
            LOG_INFO_FMT("control No.: %d, wipe duration: %d", i, struCnfp.struGroupCtrl[i].nWipeDuration);
        }

        /* wipe interval */
        if (struCnfp.struGroupCtrl[i].nWipeInterval != _t_struCnfp.struGroupCtrl[i].nWipeInterval)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(2), struCnfp.struGroupCtrl[i].nWipeInterval);
            LOG_INFO_FMT("control No.: %d, wipe interval: %d", i, struCnfp.struGroupCtrl[i].nWipeInterval);
        }

        /* wipe lift before */
        if (struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe != _t_struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(11), struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe);
            LOG_INFO_FMT("control No.: %d, wipe duration: %d", i, struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe);
        }

        /* wipe lift after */
        if (struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe != _t_struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(12), struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe);
            LOG_INFO_FMT("control No.: %d, wipe duration: %d", i, struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe);
        }

        /* wipe wind duration */
        if (struCnfp.struGroupCtrl[i].nWipeWindDuration != _t_struCnfp.struGroupCtrl[i].nWipeWindDuration)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(3), struCnfp.struGroupCtrl[i].nWipeWindDuration);
            LOG_INFO_FMT("control No.: %d, wipe wind duration: %d", i, struCnfp.struGroupCtrl[i].nWipeWindDuration);
        }

        /* wipe wind interval */
        if (struCnfp.struGroupCtrl[i].nWipeWindInterval != _t_struCnfp.struGroupCtrl[i].nWipeWindInterval)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(4), struCnfp.struGroupCtrl[i].nWipeWindInterval);
            LOG_INFO_FMT("control No.: %d, wipe wind interval: %d", i, struCnfp.struGroupCtrl[i].nWipeWindInterval);
        }

        /* background front */
        if (struCnfp.struGroupCtrl[i].nBackgroundFront != _t_struCnfp.struGroupCtrl[i].nBackgroundFront)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(5), struCnfp.struGroupCtrl[i].nBackgroundFront);
            LOG_INFO_FMT("control No.: %d, front background angle: %d", i, struCnfp.struGroupCtrl[i].nBackgroundFront);
        }

        /* background Rear */
        if (struCnfp.struGroupCtrl[i].nBackgroundRear != _t_struCnfp.struGroupCtrl[i].nBackgroundRear)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(6), struCnfp.struGroupCtrl[i].nBackgroundRear);
            LOG_INFO_FMT("control No.: %d, rear background angle: %d", i, struCnfp.struGroupCtrl[i].nBackgroundRear);
        }

        /* lamp control */
        for (int j = 0; j < MAX_LAMP; j++)
        {
            if (struCnfp.struGroupCtrl[i].nLamp[j] != _t_struCnfp.struGroupCtrl[i].nLamp[j])
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(7).arg(j), struCnfp.struGroupCtrl[i].nLamp[j]);
                LOG_INFO_FMT("lamp(level:%d,lamp No.:%d) switch: %d", i, j, struCnfp.struGroupCtrl[i].nLamp[j]);
            }
        }

        /* wipe water duration and delay*/
        if (struCnfp.struGroupCtrl[i].nWipeWaterDuration != _t_struCnfp.struGroupCtrl[i].nWipeWaterDuration)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(8), struCnfp.struGroupCtrl[i].nWipeWaterDuration);
            LOG_INFO_FMT("control No.: %d, wipe water duration: %d", i, struCnfp.struGroupCtrl[i].nWipeWaterDuration);
        }
        if (struCnfp.struGroupCtrl[i].nWipeWaterDelay != _t_struCnfp.struGroupCtrl[i].nWipeWaterDelay)
        {
            setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(8).arg(1), struCnfp.struGroupCtrl[i].nWipeWaterDelay);
            LOG_INFO_FMT("control No.: %d, wipe water delay: %d", i, struCnfp.struGroupCtrl[i].nWipeWaterDelay);
        }

        /* 供料器使能 */
        for (int j = 0; j < 10; j++)
        {
            if (struCnfp.struGroupCtrl[i].nFeederEnable[j] != _t_struCnfp.struGroupCtrl[i].nFeederEnable[j])
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(9).arg(j),
                    struCnfp.struGroupCtrl[i].nFeederEnable[j]);
                LOG_INFO_FMT("control No.: %d, feeder No.: %d, feeder status: %d", i, j, struCnfp.struGroupCtrl[i].nFeederEnable[j]);
            }
        }

        /* 灯光强 */
        for (int j = 0; j < MAX_LAMP; j++)
        {
            if (struCnfp.struGroupCtrl[i].nLampLight[j] != _t_struCnfp.struGroupCtrl[i].nLampLight[j])
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(16).arg(i).arg(10).arg(j),
                    struCnfp.struGroupCtrl[i].nLampLight[j]);
                LOG_INFO_FMT("lamp(level:%d,lamp No.:%d) strength: %d", i, j, struCnfp.struGroupCtrl[i].nLampLight[j]);
            }
        }

        /* wipe duration2 */
        if (struCnfp.struGroupCtrl[i].nWipeDuration2 != _t_struCnfp.struGroupCtrl[i].nWipeDuration2)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(13), struCnfp.struGroupCtrl[i].nWipeDuration2);
            LOG_INFO_FMT("control No.: %d, wipe duration2: %d", i, struCnfp.struGroupCtrl[i].nWipeDuration2);
        }

        /* wipe delay */
        if (struCnfp.struGroupCtrl[i].nWipeDelay != _t_struCnfp.struGroupCtrl[i].nWipeDelay)
        {
            setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(16).arg(i).arg(14), struCnfp.struGroupCtrl[i].nWipeDelay);
            LOG_INFO_FMT("control No.: %d, wipe delay: %d", i, struCnfp.struGroupCtrl[i].nWipeDelay);
        }
    }

    if (struCnfp.nDegaussTime != _t_struCnfp.nDegaussTime)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(20), struCnfp.nDegaussTime);
    }
    for (int i = 0; i < MAX_UNIT; i++)
    {
        if (struCnfp.nAnaGain[i] != _t_struCnfp.nAnaGain[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(21).arg(i), struCnfp.nAnaGain[i]);
        }
        if (struCnfp.nDigGain[i] != _t_struCnfp.nDigGain[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(22).arg(i), struCnfp.nDigGain[i]);
        }
        if (struCnfp.nGainTarget[i] != _t_struCnfp.nGainTarget[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(23).arg(i), struCnfp.nGainTarget[i]);
        }
        for (int j = 0; j < 2; j++)
        {
            if (struCnfp.nSens[i][j] != _t_struCnfp.nSens[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(27).arg(i).arg(j), struCnfp.nSens[i][j]);
            }
        }
    }
    for (int i = 0; i < MAX_GROUP_TICK; i++)
    {
        if (struCnfp.nCrossChannel[i] != _t_struCnfp.nCrossChannel[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(29).arg(i), struCnfp.nCrossChannel[i]);
        }
    }
    for (int i = 0; i < MAX_UNIT / 2; i++)
    {
        if (struCnfp.nDelayBiasing[i] != _t_struCnfp.nDelayBiasing[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(31).arg(i), struCnfp.nDelayBiasing[i]);
        }
        if (struCnfp.nFeederBias[i] != _t_struCnfp.nFeederBias[i])
        {
            setting.setValue(QString("%1-%2-%3").arg(3).arg(32).arg(i), struCnfp.nFeederBias[i]);
        }
    }
    if (struCnfp.nColorSpace != _t_struCnfp.nColorSpace)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(33), struCnfp.nColorSpace);
    }
    if (struCnfp.nAIMode != _t_struCnfp.nAIMode)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(34), struCnfp.nAIMode);
    }

    /* only for tea */
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            if (struCnfp.nArithmeticEnableLevel[i][j] != _t_struCnfp.nArithmeticEnableLevel[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(38).arg(i).arg(j), struCnfp.nArithmeticEnableLevel[i][j]);
                LOG_INFO_FMT("level:%d, arith name: %s, enable status: %d", i, myString.sArithmeticName[j].toStdString(), struCnfp.nArithmeticEnableLevel[i][j]);
            }
        }
    }

    /* sens biasing RD-C/MAIZE */
    for (int i = 0; i < MAX_UNIT; i++)
    {
        for (int j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            if (struCnfp.nSensBias[i][j] != _t_struCnfp.nSensBias[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(39).arg(i).arg(j), struCnfp.nSensBias[i][j]);
                LOG_INFO_FMT("camera No.:%d, arith name: %s, sens bias: %d", i, myString.sArithmeticName[j].toStdString(), struCnfp.nSensBias[i][j]);
            }
        }
        for (int j = 0; j < MAX_MAIZE_SENSNUM_TOTAL; j++)
        {
            if (struCnfp.nSensBiasMaize[i][j] != _t_struCnfp.nSensBiasMaize[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(39).arg(i).arg(j).arg(0), struCnfp.nSensBiasMaize[i][j]);
                LOG_INFO_FMT("camera No.:%d, arith name: %s, sens No. %d, bias: %d", i, myString.sArithmeticName[ARITH_MAIZE].toStdString(), i, struCnfp.nSensBiasMaize[i][j]);
            }
        }
    }

    for (int i = 0; i < MAX_UNIT; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (struCnfp.nDarkSens[i][j] != _t_struCnfp.nDarkSens[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(40).arg(i).arg(j), struCnfp.nDarkSens[i][j]);
            }
        }
    }

    /*智能供料调节相关变量*/
    if (struCnfp.isFeederAIEnable != _t_struCnfp.isFeederAIEnable)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(42).arg(1), struCnfp.isFeederAIEnable);
    }
    if (struCnfp.nFeederCapMin != _t_struCnfp.nFeederCapMin)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(42).arg(2), struCnfp.nFeederCapMin);
    }
    if (struCnfp.nFeederCapMax != _t_struCnfp.nFeederCapMax)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(42).arg(3), struCnfp.nFeederCapMax);
    }
    if (struCnfp.nEjectTimesMin != _t_struCnfp.nEjectTimesMin)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(42).arg(4), struCnfp.nEjectTimesMin);
    }
    if (struCnfp.nEjectTimesMax != _t_struCnfp.nEjectTimesMax)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(42).arg(5), struCnfp.nEjectTimesMax);
    }
    if (struCnfp.autoFeedEnablePageFlag != _t_struCnfp.autoFeedEnablePageFlag)
    {
        setting.setValue(QString("%1-%2-%3").arg(3).arg(42).arg(6), struCnfp.autoFeedEnablePageFlag);
    }

    //! auto sig
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (struCnfp.nAutoSigVal[i][j][k] != _t_struCnfp.nAutoSigVal[i][j][k])
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(43).arg(i).arg(j).arg(k), struCnfp.nAutoSigVal[i][j][k]);
                }
                if (struCnfp.nAutoAdVal[i][j][k] != _t_struCnfp.nAutoAdVal[i][j][k])
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(44).arg(i).arg(j).arg(k), struCnfp.nAutoAdVal[i][j][k]);
                }
            }

        }
    }

    if (struCnfp.nAutoSigInit != _t_struCnfp.nAutoSigInit)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(45), struCnfp.nAutoSigInit);
    }

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (struCnfp.nAutoBgVal[i][j][k] != _t_struCnfp.nAutoBgVal[i][j][k])
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(46).arg(i).arg(j).arg(k), struCnfp.nAutoBgVal[i][j][k]);
                }
            }
        }
    }

    // 性能参数统计设置参数
    if (struCnfp.nMaterialQueryInterval != _t_struCnfp.nMaterialQueryInterval)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(47), struCnfp.nMaterialQueryInterval);
    }
    if (struCnfp.nHistogramQueryTime != _t_struCnfp.nHistogramQueryTime)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(48), struCnfp.nHistogramQueryTime);
    }
    if (struCnfp.nCameraRef != _t_struCnfp.nCameraRef)
    {
        setting.setValue(QString("%1-%2").arg(3).arg(50), struCnfp.nCameraRef);
    }

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_COLOR_MODE; j++)
        {
            if (struCnfp.nTeaProfileCustomType[i][j] != _t_struCnfp.nTeaProfileCustomType[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(49).arg(i).arg(j), struCnfp.nTeaProfileCustomType[i][j]);
            }
        }
    }

    // 背景自适应
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            if (struCnfp.nAutoBgEnable[i][j] != _t_struCnfp.nAutoBgEnable[i][j])
            {
                setting.setValue(QString("%1-%2-%3-%4").arg(3).arg(51).arg(i).arg(j), struCnfp.nAutoBgEnable[i][j]);
                LOG_INFO_FMT("bg(level:%d,unit.:%d) bg auto enable: %d", i, j, struCnfp.nAutoBgEnable[i][j]);
            }
        }
    }
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (struCnfp.nAutoBgRange[i][j][k] != _t_struCnfp.nAutoBgRange[i][j][k])
                {
                    setting.setValue(QString("%1-%2-%3-%4-%5").arg(3).arg(52).arg(i).arg(j).arg(k), struCnfp.nAutoBgRange[i][j][k]);
                    LOG_INFO_FMT("bg(level:%d,unit:%d,rgb:%d) bg auto range: %d", i, j, k, struCnfp.nAutoBgRange[i][j][k]);
                }
            }
        }
    }

    //下位机自动背景使能
    setting.setValue(QString("%1-%2").arg(3).arg(53), struCnfp.nAutoBackgroundEnanble_RSC);
    LOG_INFO_FMT("autoBackground enable: %d", struCnfp.nAutoBackgroundEnanble_RSC);
    //下位机自动背景阈值
    setting.setValue(QString("%1-%2").arg(3).arg(54), struCnfp.nAutoBackgroundThresholdValue_RSC);
    LOG_INFO_FMT("autoBackground ThresholdValue: %d", struCnfp.nAutoBackgroundThresholdValue_RSC);

    if (setting.status() == QSettings::NoError)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* 获取色选方案参数 */
void GlobalFlow::getProfile()
{
    int i, j;
    QFile file;
    int nWipeDuration, nWipeInterval, nFeeder;

    memset(&struCnfp, 0, sizeof(struCnfProfile));
    memset(&_t_struCnfp, 0, sizeof(struCnfProfile));

    struCnfp.nInterfaceBoardMode = 1;
    struCnfp.nColorBoardMode = 1;
    struCnfp.nMatIntelSeq = 0;

    struCnfp.nMaterialQueryInterval = 5;
    struCnfp.nHistogramQueryTime = 10;
    struCnfp.nCameraRef = 0;

    struCnfp.nMatAssembleModeIntel = 1; //! 默认独立设置
    struCnfp.nMatAssembleMode = PARAMS_ALL_SAME;

    struCnfp.nMelonUse = 0;
    struCnfp.nSensorMode = 1;
    struCnfp.nIntelMode = 0;
    if (myFlow.getProductLineNo() == 0)
    {
        struCnfp.nIntelMode = 1;
    }

    for (i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_BACKGROUND_GROUP; j++)
        {
            struCnfp.nBackgroundColor[i][j] = 4;
            struCnfp.nBackgroundColorPer[i][j] = 40;
            struCnfp.nBackgroundColorValue[i][j] = 70;
        }
    }

    for (i = 0; i < ARITHMETIC_TOTAL; i++)
    {
        struCnfp.nArithmeticEnable[i] = 0;
    }

    for (i = 0; i < MAX_LEVEL; i++)
    {
        for (j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            struCnfp.nArithmeticEnableLevel[i][j] = 0;
        }
    }

    for (i = 0; i < 4; i++)
    {
        struCnfp.struIntelMat[i].use = 0;
        struCnfp.struIntelMat[i].good_bad = 2;
        memcpy(struCnfp.struIntelMat[i].name, "Name", sizeof("Name"));
    }

    /* RS机型对应参数 */
    for (int i = 0; i < MAX_UNIT; i++)
    {
        struCnfp.nAnaGain[i] = 200;
        struCnfp.nDigGain[i] = 2048;
        struCnfp.nGainTarget[i] = 200;

        for (int j = 0; j < 2; j++)
        {
            struCnfp.nSens[i][j] = 0;
            struCnfp.nDarkSens[i][j] = 0;
        }
    }
    for (int i = 0; i < MAX_UNIT / 2; i++)
    {
        struCnfp.nDelayBiasing[i] = 0;
        struCnfp.nFeederBias[i] = 0;
    }
    struCnfp.nCrossChannel[0] = 1;
    for (int i = 1; i < MAX_GROUP_TICK; i++)
    {
        struCnfp.nCrossChannel[i] = 0;
    }
    struCnfp.nDegaussTime = 8;

    /* 更新算法名称 */
    getArithmeticName();

    /* 识别组参数 */
    getGroupIdentify();

    /* 剔除组参数 */
    getGroupTick();

    for (int i = 0; i < MAX_UNIT; i++)
    {
        for (int j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            struCnfp.nSensBias[i][j] = 0;
        }
        for (int j = 0; j < MAX_MAIZE_SENSNUM_TOTAL; j++)
        {
            struCnfp.nSensBiasMaize[i][j] = 0;
        }
    }

    /* 控制板参数 */
    nWipeDuration = 16;
    nWipeInterval = 30;
    nFeeder = 0;
    for (i = 0; i < MAX_CTRL; i++)
    {
        struCnfp.struGroupCtrl[i].nWipeDuration = nWipeDuration;
        struCnfp.struGroupCtrl[i].nWipeInterval = nWipeInterval;
        struCnfp.struGroupCtrl[i].nLiftTimeBeforeWipe = 0;
        struCnfp.struGroupCtrl[i].nLiftTimeAfterWipe = 0;
        struCnfp.struGroupCtrl[i].nWipeWindDuration = 3;    // 0.3s
        struCnfp.struGroupCtrl[i].nWipeWindInterval = 15;   // 15s
        struCnfp.struGroupCtrl[i].nWipeWaterDuration = 0;
        struCnfp.struGroupCtrl[i].nWipeWaterDelay = 10;
        struCnfp.struGroupCtrl[i].nBackgroundFront = 100;
        struCnfp.struGroupCtrl[i].nBackgroundRear = 100;
        struCnfp.struGroupCtrl[i].nWipeDuration2 = 3;
        struCnfp.struGroupCtrl[i].nWipeDelay = 0;

        for (j = 0; j < 10; j++)
        {
            struCnfp.struGroupCtrl[i].nFeeder[j] = nFeeder;
            struCnfp.struGroupCtrl[i].nFeederEnable[j] = 1;
        }

        /* 物料灯光强默认为140*通道数,背景灯光强默认为400 */
        int nChuteNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2;

        int nLight = 140 * nChuteNum;
        for (j = 0; j < MAX_LAMP; j++)
        {
            struCnfp.struGroupCtrl[i].nLamp[j] = 1;
            struCnfp.struGroupCtrl[i].nLampLight[j] = nLight;
        }

        //! RF背景灯编号为2和8,其它机型为2和7
        if (myFlow.getProductLineNo() != 0)
        {
            struCnfp.struGroupCtrl[i].nLampLight[FRONT_LAMP_3_BACKGROUD] = 400;
            int nRearBkgLamp = struCnfg.nLampTotal / 2 + 2;
            struCnfp.struGroupCtrl[i].nLampLight[nRearBkgLamp] = 400;
        }
    }

    struCnfp.nAIMode = 1;
    struCnfp.nColorSpace = 0;
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            if (j % 2 == 0)
            {
                struCnfp.nAutoSigVal[i][j][0] = 142;
                struCnfp.nAutoSigVal[i][j][1] = 157;
                struCnfp.nAutoSigVal[i][j][2] = 172;
            }
            else
            {
                struCnfp.nAutoSigVal[i][j][0] = 196;
                struCnfp.nAutoSigVal[i][j][1] = 196;
                struCnfp.nAutoSigVal[i][j][2] = 181;
            }
            for (int k = 0; k < 3; k++)
            {
                struCnfp.nAutoAdVal[i][j][k] = 1024;
            }
        }
    }

    struCnfp.nAutoSigInit = 0;

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            if (j % 2 == 0)
            {
                struCnfp.nAutoBgVal[i][j][0] = 30;
                struCnfp.nAutoBgVal[i][j][1] = 30;
                struCnfp.nAutoBgVal[i][j][2] = 225;
            }
            else
            {
                struCnfp.nAutoBgVal[i][j][0] = 225;
                struCnfp.nAutoBgVal[i][j][1] = 225;
                struCnfp.nAutoBgVal[i][j][2] = 225;
            }
        }
    }

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            struCnfp.nAutoBgEnable[i][j] = 0;
            for (int k = 0; k < 3; k++)
            {
                struCnfp.nAutoBgRange[i][j][k] = 50;
            }
        }
    }

    struCnfp.isFeederAIEnable = 0;
    struCnfp.autoFeedEnablePageFlag = 0;
    struCnfp.nFeederCapMin = 0;
    struCnfp.nFeederCapMax = 0;
    struCnfp.nEjectTimesMin = 0;
    struCnfp.nEjectTimesMax = 0;

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_COLOR_MODE; j++)
        {
            struCnfp.nTeaProfileCustomType[i][j] = 0;
        }
    }

    // 算法边缘切除圈数
    for (int i = 0; i < MAX_UNIT; i++)
    {
        for (int j = 0; j < ARITHMETIC_TOTAL; j++)
        {
            struCnfp.nArithEdgeCut[i][j] = 0;
        }
    }

    memcpy(&_t_struCnfp, &struCnfp, sizeof(struCnfProfile));

    QString fileName = QString("%1/userdata/cnf/%2").arg(APP_PATH).arg(QString().fromLatin1(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName));

    if (struCnfs.value != SCHEME_FILE_ERROR)
    {
        if (QFileInfo(fileName).exists() && QFileInfo(fileName).size() > 0)
        {
            if (!getProfileSetting(fileName))
            {
                qDebug("getProfile(): can not get profile parameters, using default");
                return;
            }
            file.remove(fileName + "_2");
            file.copy(fileName, fileName + "_2");
        }
        memcpy(&_t_struCnfp, &struCnfp, sizeof(struCnfProfile));
    }
    else
    {
        if (QFileInfo(fileName + "_2").exists() && QFileInfo(fileName + "_2").size() > 0)
        {
            if (!getProfileSetting(fileName + "_2"))
            {
                qDebug("getProfile(): can not get profile parameters, using default");
                return;
            }
            file.remove(fileName);
            file.copy(fileName + "_2", fileName);
        }
        memcpy(&_t_struCnfp, &struCnfp, sizeof(struCnfProfile));
    }


    return;
}

/* 保存色选方案参数 */
bool GlobalFlow::saveProfile()
{
    int i;
    QFile file;

    QString fileName = QString("%1/userdata/cnf/%2").arg(APP_PATH).arg(QString().fromLatin1(struCnfg.struProfileIndex[struCnfg.nProfile].sProfileName));
    saveParamsFileStaus(SCHEME_FILE_ERROR);
    for (i = 0; i < 3; i++)
    {
        if (saveProfileSetting(fileName))
        {
            saveParamsFileStaus(ALL_PARAMS_FILE_OK);
            break;
        }
    }

    if (i == 3)
        return false;

    if (QFileInfo(fileName).exists() && QFileInfo(fileName).size() > 0)
    {
        file.remove(fileName + "_2");
        file.copy(fileName, fileName + "_2");
    }

    memcpy(&_t_struCnfp, &struCnfp, sizeof(struCnfProfile));    // 更新_t_struCnfp
    return true;
}
