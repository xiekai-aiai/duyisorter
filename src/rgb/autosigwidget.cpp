#include "autosigwidget.h"
#include "unilog.h"

autoSigWidget::autoSigWidget(QWidget* parent) :
    QWidget(parent)
{
    m_stackedSigWid = new QStackedWidget(this);
    m_stackedSigWid->setFixedSize(LCD_WIDTH, LCD_HEIGHT - LCD_TITLE_HEIGHT - LCD_STATUS_HEIGHT - 10);

    m_pageSigMainWid = new QWidget;
    m_stackedSigWid->addWidget(m_pageSigMainWid);
    createSigMainWidget();

    m_pageSigSettingWid = new QWidget;
    m_stackedSigWid->addWidget(m_pageSigSettingWid);
    createSigSettingWidget();

    m_stackedSigWid->setCurrentIndex(SIG_MAIN_PAGE);
}
/*!
 * \brief autoSigWidget::createSigMainWidget
 */
void autoSigWidget::createSigMainWidget()
{
    m_autoSigBtn = new myPushButton(myLan.super_brain, myIcon.Action_OneKey);
    m_autoSigBtn->setFixedSize(BTN_WIDTH + 50, BTN_HEIGHT);
    m_autoSigInitBtn = new myPushButton(myLan.init_autoSig, myIcon.Chart_Dot);
    m_autoSigInitBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_autoSigSettingBtn = new myPushButton(myLan.set, myIcon.Action_Configure);
    m_autoSigSettingBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_autoSigBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_autoSigBackBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    //! 页面布局
    QHBoxLayout* upLay = new QHBoxLayout;
    upLay->setContentsMargins(0, 250, 0, 260);
    upLay->addWidget(m_autoSigBtn);

    QHBoxLayout* downLay = new QHBoxLayout;
    downLay->addWidget(m_autoSigInitBtn);
    downLay->addStretch();
    downLay->addWidget(m_autoSigSettingBtn);
    downLay->addStretch();
    downLay->addWidget(m_autoSigBackBtn);

    QVBoxLayout* mainLay = new QVBoxLayout(m_pageSigMainWid);
    mainLay->setContentsMargins(20, 0, 20, 0);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);

    updateAutoSigWid();

    //! 槽函数
    connect(m_autoSigBackBtn, SIGNAL(pressed()), this, SLOT(onAutoSigBackBtnPressedSlt()));
    connect(m_autoSigBtn, SIGNAL(pressed()), this, SLOT(onAutoSigBtnPressedSlt()));
    connect(m_autoSigSettingBtn, SIGNAL(pressed()), this, SLOT(onAutoSigSettingBtnPressedSlt()));
    connect(m_autoSigInitBtn, SIGNAL(pressed()), this, SLOT(onAutoSigInitBtnPressedSlt()));
}
/*!
 * \brief autoSigWidget::createSigSettingWidget
 */
void autoSigWidget::createSigSettingWidget()
{
    m_chuteGroup = new myGroupBox("");
    m_chuteLbe = new myLabel(myLan.reference_chute);
    m_minusBtn = new myPushButton(myIcon.Arrow_Left, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH));
    m_lcdNum = new QLCDNumber(2);
    m_plusBtn = new myPushButton(myIcon.Arrow_Right, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH));
    m_lcdNum->setFixedSize(SMALL_BTN_WIDTH + 20, BTN_HEIGHT);
    m_chuteGroup->setMinimumWidth(LCD_WIDTH / 2);
    m_lcdNum->display(struCnfg.nAutoReferChute);
    m_feedSetLbe = new myLabel(myLan.current_feeder);
    m_feedLne = new myLineEdit(QString("%1").arg(struCnfg.nMaterPer2));
    m_feedLne->setMaximumWidth(BTN_WIDTH);
    m_feedLne->setMaximumHeight(BTN_HEIGHT);
    m_enableCbx = new myCustomCheckBox(myLan.enable, struCnfg.nAutoSigEnable);

    QGridLayout* chuteGridLay = new QGridLayout(m_chuteGroup);
    chuteGridLay->addWidget(m_chuteLbe, 0, 0, 1, 1);
    chuteGridLay->addWidget(m_minusBtn, 0, 1, 1, 1);
    chuteGridLay->addWidget(m_lcdNum, 0, 2, 1, 1);
    chuteGridLay->addWidget(m_plusBtn, 0, 3, 1, 1);
    chuteGridLay->addWidget(m_feedSetLbe, 1, 0, 1, 1);
    chuteGridLay->addWidget(m_feedLne, 1, 1, 1, 3);
    chuteGridLay->addWidget(m_enableCbx, 2, 0, 1, 4);

    QHBoxLayout* upHbLay = new QHBoxLayout;
    upHbLay->addStretch();
    upHbLay->addWidget(m_chuteGroup);
    upHbLay->addStretch();

    m_settingBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_settingBackBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QHBoxLayout* downHbLay = new QHBoxLayout;
    downHbLay->addStretch();
    downHbLay->addWidget(m_settingBackBtn);

    QVBoxLayout* mainVLay = new QVBoxLayout(m_pageSigSettingWid);
    mainVLay->addLayout(upHbLay);
    mainVLay->addLayout(downHbLay);

    connect(m_minusBtn, SIGNAL(pressed()), this, SLOT(onSettingPageCommonProcessedSlt()));
    connect(m_plusBtn, SIGNAL(pressed()), this, SLOT(onSettingPageCommonProcessedSlt()));
    connect(m_feedLne, SIGNAL(pressed()), this, SLOT(onSettingPageCommonProcessedSlt()));
    connect(m_enableCbx, SIGNAL(pressed()), this, SLOT(onSettingPageCommonProcessedSlt()));
    connect(m_settingBackBtn, SIGNAL(pressed()), this, SLOT(onSettingPageCommonProcessedSlt()));
}

/*!
 * \brief autoSigWidget::adjustBackground
 * \param index
 */
void autoSigWidget::adjustBackground(int nLevelId, bool bIsFront)
{
    int nPacketSize = 1040;
    char buf[nPacketSize];
    int i;
    memset(buf, 0, sizeof(buf));
    int nAddr, reference;

    int  nWaittime;
    int  nDataLength;
    char* p = struGsh.sRgbRow;
    //! 
    if (bIsFront)
    {
        reference = struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp];
        nAddr = (struCnfg.nAutoReferChute - 1) * 2;
    }
    else
    {
        reference = struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp];
        nAddr = (struCnfg.nAutoReferChute - 1) * 2 + 1;
    }


    //! 前视背景调节
    for (i = 0; i < 16; i++)
    {
        MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, nLevelId, nAddr, 0, 0, 0, 0, 0, 1);

        switch (struCnfc.nSensorPixelLevel[nLevelId])
        {
        case SENSOR_PIXELS_1024:
            nDataLength = 1024 * 3 + 512 + 16;
            nWaittime = 400000;
            break;
        case SENSOR_PIXELS_2048:
            nDataLength = 2048 * 3 + 16;
            nWaittime = 800000;
            break;
        default:
            nDataLength = 1024 * 3 + 512 + 16;
            nWaittime = 400000;
            break;
        }

        usleep(nWaittime);
        memset(p, 0, sizeof(struGsh.sRgbRow));

        MySerial.com1Read(p, nDataLength);

        if (p[0] == 0xAA && p[1] == 0xAA && p[2] == 0xAA && p[3] == 0xAA)
        {
            int sum1 = 0;
            int sum2 = 0;
            int sum3 = 0;
            int avr1 = 0;
            int avr2 = 0;
            int avr3 = 0;
            if (struCnfc.nSensorPixelLevel[nLevelId] == SENSOR_PIXELS_1024)
            {
                for (int n = 200; n < 800; n++)
                {
                    sum1 += struGsh.sRgbRow[10 + n * 3];
                    sum2 += struGsh.sRgbRow[10 + n * 3 + 1];
                    sum3 += struGsh.sRgbRow[10 + n * 3 + 2];
                }
                avr1 = sum1 / 600;
                avr2 = sum2 / 600;
                avr3 = sum3 / 600;
            }
            else
            {
                for (int n = 400; n < 1600; n++)
                {
                    sum1 += struGsh.sRgbRow[10 + n * 3];
                    sum2 += struGsh.sRgbRow[10 + n * 3 + 1];
                    sum3 += struGsh.sRgbRow[10 + n * 3 + 2];
                }
                avr1 = sum1 / 1200;
                avr2 = sum2 / 1200;
                avr3 = sum3 / 1200;
            }

            printf("Bg r:%d g:%d b:%d\n", avr1, avr2, avr3);
            int tmpAdjVal1_1 = 0, tmpAdjVal1_2 = 0, tmpAdjVal1_3 = 0, tmpAdjVal11 = 0;
            int tmpAdjVal1 = 0, tmpAdjVal2 = 0, tmpAdjVal3 = 0, tmpAdjVal = 0;
            tmpAdjVal1_1 = struCnfp.nAutoBgVal[nLevelId][nAddr][0] - avr1;
            tmpAdjVal1_2 = struCnfp.nAutoBgVal[nLevelId][nAddr][1] - avr2;
            tmpAdjVal1_3 = struCnfp.nAutoBgVal[nLevelId][nAddr][2] - avr3;

            printf("Bg_1 r:%d g:%d b:%d\n", tmpAdjVal1_1, tmpAdjVal1_2, tmpAdjVal1_3);
            tmpAdjVal1 = abs(struCnfp.nAutoBgVal[nLevelId][nAddr][0] - avr1);
            tmpAdjVal2 = abs(struCnfp.nAutoBgVal[nLevelId][nAddr][1] - avr2);
            tmpAdjVal3 = abs(struCnfp.nAutoBgVal[nLevelId][nAddr][2] - avr3);

            if (tmpAdjVal1 > tmpAdjVal2)
            {
                tmpAdjVal = tmpAdjVal1;
                tmpAdjVal1 = tmpAdjVal2;
                tmpAdjVal2 = tmpAdjVal;

                tmpAdjVal11 = tmpAdjVal1_1;
                tmpAdjVal1_1 = tmpAdjVal1_2;
                tmpAdjVal1_2 = tmpAdjVal11;
            }

            if (tmpAdjVal1 > tmpAdjVal3)
            {
                tmpAdjVal = tmpAdjVal1;
                tmpAdjVal1 = tmpAdjVal3;
                tmpAdjVal3 = tmpAdjVal;

                tmpAdjVal11 = tmpAdjVal1_1;
                tmpAdjVal1_1 = tmpAdjVal1_3;
                tmpAdjVal1_3 = tmpAdjVal11;
            }

            if (tmpAdjVal2 > tmpAdjVal3)
            {
                tmpAdjVal = tmpAdjVal2;
                tmpAdjVal2 = tmpAdjVal3;
                tmpAdjVal3 = tmpAdjVal;

                tmpAdjVal11 = tmpAdjVal1_2;
                tmpAdjVal1_2 = tmpAdjVal1_3;
                tmpAdjVal1_3 = tmpAdjVal11;
            }

            if (tmpAdjVal3 >= 25)
            {
                reference += tmpAdjVal1_3;
                if (reference > 2000)
                {
                    reference = 2000;
                }
                if (reference < 0)
                {
                    reference = 0;
                }
            }
            else
            {
                break;
            }
            printf("ref:%d\n", reference);
            //! 参考滑槽前视光源亮度值校准
            if (bIsFront)
            {
                MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, 1, 0, 0, 2, reference / 256, reference % 256, 3);
            }
            else
            {
                MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, 0, 0, 0, 2, reference / 256, reference % 256, 3);
            }
            myFlow.sleep(1);
        }
    }
    //!
    if (bIsFront)
    {
        struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp] = reference;
    }
    else
    {
        struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp] = reference;
    }
}

/*!
 * \brief autoSigWidget::adjust
 * \param nLevelId
 * \param nAddr
 */
int autoSigWidget::adjust(int nLevelId, int nAddr, int reference)
{
    int  nWaittime;
    int  nDataLength;
    char* p = struGsh.sRgbRow;

    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, nLevelId, nAddr, 0, 0, 0, 0, 0, 1);

    switch (struCnfc.nSensorPixelLevel[nLevelId])
    {
    case SENSOR_PIXELS_1024:
        nDataLength = 1024 * 3 + 512 + 16;
        nWaittime = 400000;
        break;
    case SENSOR_PIXELS_2048:
        nDataLength = 2048 * 3 + 16;
        nWaittime = 800000;
        break;
    default:
        nDataLength = 1024 * 3 + 512 + 16;
        nWaittime = 400000;
        break;
    }

    usleep(nWaittime);
    memset(p, 0, sizeof(struGsh.sRgbRow));

    MySerial.com1Read(p, nDataLength);

    if (p[0] == 0xAA && p[1] == 0xAA && p[2] == 0xAA && p[3] == 0xAA)
    {
        int sum1 = 0;
        int sum2 = 0;
        int sum3 = 0;
        int avr1 = 0;
        int avr2 = 0;
        int avr3 = 0;
        if (struCnfc.nSensorPixelLevel[nLevelId] == SENSOR_PIXELS_1024)
        {
            for (int n = 200; n < 800; n++)
            {
                sum1 += struGsh.sRgbRow[10 + n * 3];
                sum2 += struGsh.sRgbRow[10 + n * 3 + 1];
                sum3 += struGsh.sRgbRow[10 + n * 3 + 2];
            }
            avr1 = sum1 / 600;
            avr2 = sum2 / 600;
            avr3 = sum3 / 600;
        }
        else
        {
            for (int n = 400; n < 1600; n++)
            {
                sum1 += struGsh.sRgbRow[10 + n * 3];
                sum2 += struGsh.sRgbRow[10 + n * 3 + 1];
                sum3 += struGsh.sRgbRow[10 + n * 3 + 2];
            }
            avr1 = sum1 / 1200;
            avr2 = sum2 / 1200;
            avr3 = sum3 / 1200;
        }

        printf("Bg r:%d g:%d b:%d\n", avr1, avr2, avr3);
        int tmpAdjVal1_1 = 0, tmpAdjVal1_2 = 0, tmpAdjVal1_3 = 0, tmpAdjVal11 = 0;
        int tmpAdjVal1 = 0, tmpAdjVal2 = 0, tmpAdjVal3 = 0, tmpAdjVal = 0;
        tmpAdjVal1_1 = struCnfp.nAutoBgVal[nLevelId][nAddr][0] - avr1;
        tmpAdjVal1_2 = struCnfp.nAutoBgVal[nLevelId][nAddr][1] - avr2;
        tmpAdjVal1_3 = struCnfp.nAutoBgVal[nLevelId][nAddr][2] - avr3;

        printf("Bg_1 r:%d g:%d b:%d\n", tmpAdjVal1_1, tmpAdjVal1_2, tmpAdjVal1_3);
        tmpAdjVal1 = abs(struCnfp.nAutoBgVal[nLevelId][nAddr][0] - avr1);
        tmpAdjVal2 = abs(struCnfp.nAutoBgVal[nLevelId][nAddr][1] - avr2);
        tmpAdjVal3 = abs(struCnfp.nAutoBgVal[nLevelId][nAddr][2] - avr3);

        if (tmpAdjVal1 > tmpAdjVal2)
        {
            tmpAdjVal = tmpAdjVal1;
            tmpAdjVal1 = tmpAdjVal2;
            tmpAdjVal2 = tmpAdjVal;

            tmpAdjVal11 = tmpAdjVal1_1;
            tmpAdjVal1_1 = tmpAdjVal1_2;
            tmpAdjVal1_2 = tmpAdjVal11;
        }

        if (tmpAdjVal2 > tmpAdjVal3)
        {
            tmpAdjVal = tmpAdjVal2;
            tmpAdjVal2 = tmpAdjVal3;
            tmpAdjVal3 = tmpAdjVal;

            tmpAdjVal11 = tmpAdjVal1_2;
            tmpAdjVal1_2 = tmpAdjVal1_3;
            tmpAdjVal1_3 = tmpAdjVal11;
        }

        if (tmpAdjVal3 >= 5)
        {
            if (tmpAdjVal1_3 > 0)
                reference += tmpAdjVal1_3;
            else
                reference -= tmpAdjVal1_3;
        }
        printf("ref:%d\n", reference);
        //! 参考滑槽前视光源亮度值校准
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, 1, 0, 0, 2, reference / 256, reference % 256, 3);
    }

    return reference;
}

/*!
 * \brief autoSigWidget::getHist
 */
void autoSigWidget::getHist()
{
    int nAddr = 0;
#if 1
    //! 获取信号直方图数据
    QFile fileHist;
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            struGsh.struStatisticInfo.histogramStatistic.clear();
            struGsh.struStatisticInfo.histogramStatistic.resize(MAX_STATISTIC_HISTOGRAM + 16);
            MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_OBTAIN, UNIT, 0, nAddr, 0, 0, 0, 0, 0, 3);
            MySerial.com3Read(struGsh.struStatisticInfo.histogramStatistic.data(), struGsh.struStatisticInfo.histogramStatistic.size(), 1000);
            QString head = struGsh.struStatisticInfo.histogramStatistic.left(8);
            // if (!head.compare("AAAAAAAA")){
            //     continue;
            // }

            struGsh.struStatisticInfo.histogramStatistic.remove(0, 10);
            struGsh.struStatisticInfo.histogramStatistic.truncate(struGsh.struStatisticInfo.histogramStatistic.size() - 6);

            quint64 tmpHist1[256];
            quint64 tmpHist2[256];
            quint64 tmpHist3[256];
            memset(tmpHist1, 0, 256);
            memset(tmpHist2, 0, 256);
            memset(tmpHist3, 0, 256);

            for (int k = 0; k < 256; k++)
            {
                tmpHist1[k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k * 4)) * 256 * 256 * 256 + ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k * 4 + 1)) * 256 * 256 + ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k * 4 + 2)) * 256 + (uchar)struGsh.struStatisticInfo.histogramStatistic.at(k * 4 + 3);
                tmpHist2[k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 + k * 4)) * 256 * 256 * 256 + ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 + k * 4 + 1)) * 256 * 256 + ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 + k * 4 + 2)) * 256 + (uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 + k * 4 + 3);
                tmpHist3[k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 * 2 + k * 4)) * 256 * 256 * 256 + ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 * 2 + k * 4 + 1)) * 256 * 256 + ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 * 2 + k * 4 + 2)) * 256 + (uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024 * 2 + k * 4 + 3);
                //				qDebug()<<tmpHist1[k];
                //				qDebug()<<tmpHist2[k];
                //				qDebug()<<tmpHist3[k];
            }
            //			for (int i = 0; i < struGsh.struStatisticInfo.histogramStatistic.size();i++){
            //					qDebug()<<(uchar)struGsh.struStatisticInfo.histogramStatistic.at(i);
            //			}
            QString fileName = QString("/tmp/%1-%2").arg(i).arg(j);
            fileHist.setFileName(fileName);
            if (!fileHist.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                return;
            }
            QTextStream out(&fileHist);
            for (int n = 0; n < 256; n++)
            {
                QString outStr = QString("%1\t%2\t%3\n").arg(tmpHist1[n]).arg(tmpHist2[n]).arg(tmpHist3[n]);
                out << outStr;
            }
            fileHist.close();

        }
    }
#endif
}

/*!
 * \brief autoSigWidget::getBkgValue
 * \param nUnit
 * \param nVal
 * \param nPer
 */
void autoSigWidget::getBkgValue(int nUnit, int& nVal, int& nPer)
{
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, nUnit, 0, 0, 0, 0, 0, 1);
    int nLen = struCnfc.nSensorPixel * 3 + 16;
    myFlow.msleep(600);

    memset(struGsh.sRgbRow, 0, sizeof(struGsh.sRgbRow));
    int nRet = MySerial.com1Read(struGsh.sRgbRow, nLen);
    uchar* p = (uchar*)struGsh.sRgbRow;

    int nColor;
    int sum = 0;
    int num = 1;
    if (nRet == nLen && p[0] == 0xAA && p[1] == 0xAA
        && p[2] == 0xAA && p[3] == 0xAA)
    {
        if (nUnit % 2 == 0)
        {
            nColor = struCnfp.nBackgroundColor[ONE_LEVEL][0];
        }
        else
        {
            nColor = struCnfp.nBackgroundColor[ONE_LEVEL][1];
        }
        p = (uchar*)struGsh.sRgbRow + 10;

        int nBegin = struCnfc.struLevelCamera[ONE_LEVEL].nChannelBegin[nUnit];
        int nEnd = struCnfc.struLevelCamera[ONE_LEVEL].nChannelEnd[nUnit];

        num = nEnd - nBegin + 1;
        if (num <= 0)
        {
            return;
        }
        nVal = -1;
        nPer = -1;
        switch (nColor)
        {
        case COLOR_OPT_R:
            nPer = ((float)p[0] / (p[0] + p[1] + p[2])) * 100;
            for (int i = nBegin; i < nEnd; i++)
            {
                sum += p[i * 3];
            }
            nVal = sum / num;

            for (int i = nBegin; i < nEnd; i++)
            {
                if (abs(p[i * 3] - nVal) > 30)
                {
                    continue;
                }

                int nTmp = ((float)p[i * 3] / (p[i * 3] + p[3 * i + 1] + p[3 * i + 2])) * 100;
                if (nTmp < nPer)
                {
                    nPer = nTmp;
                }
            }
            break;

        case COLOR_OPT_G:
            nPer = ((float)p[1] / (p[0] + p[1] + p[2])) * 100;
            for (int i = nBegin; i < nEnd; i++)
            {
                sum += p[i * 3 + 1];
            }
            nVal = sum / num;
            for (int i = nBegin; i < nEnd; i++)
            {
                if (abs(p[i * 3 + 1] - nVal) > 30)
                {
                    continue;
                }

                int nTmp = ((float)p[i * 3 + 1] / (p[i * 3] + p[3 * i + 1] + p[3 * i + 2])) * 100;
                if (nTmp < nPer)
                {
                    nPer = nTmp;
                }
            }
            break;

        case COLOR_OPT_B:
            nPer = ((float)p[2] / (p[0] + p[1] + p[2])) * 100;
            for (int i = nBegin; i < nEnd; i++)
            {
                sum += p[i * 3 + 2];
            }
            nVal = sum / num;

            for (int i = nBegin; i < nEnd; i++)
            {
                if (abs(p[i * 3 + 2] - nVal) > 30)
                {
                    continue;
                }

                int nTmp = ((float)p[i * 3 + 2 / (p[i * 3] + p[3 * i + 1] + p[3 * i + 2])]) * 100;
                if (nTmp < nPer)
                {
                    nPer = nTmp;
                }
            }
            break;

        case COLOR_OPT_WHITE:
            for (int i = nBegin; i < nEnd; i++)
            {
                sum += (p[i * 3] + p[i * 3 + 1] + p[i * 3 + 2]) / 3;
            }
            nVal = sum / num;
            break;

        case COLOR_OPT_BLACK:
            for (int i = nBegin; i < nEnd; i++)
            {
                sum += (p[i * 3] + p[i * 3 + 1] + p[i * 3 + 2]) / 3;
            }
            nVal = sum / num;
            break;

        default:
            break;
        }
    }

    printf("%d: %d %d\n", nUnit, nVal, nPer);
}

/*!
 * \brief autoSigWidget::setBkgValueAuto
 * \param bAdjust
 */
void autoSigWidget::setBkgValueAuto(bool bAdjust)
{
    int nChuteNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2;
    int nValOffset = 30;
    int nPerOffset = 5;

    if (bAdjust)
    {
        nValOffset = 15;
        nPerOffset = 5;
    }
    //! 设置前后视背景(i=0为前视, i=1为后视)
    for (int i = 0; i < 2; i++)
    {
        bool bIsChanged = false;
        int nColor = struCnfp.nBackgroundColor[ONE_LEVEL][i];
        int nBkgVal = 255;
        int nBkgPer = 100;
        if (nColor == COLOR_OPT_BLACK)
        {
            nBkgVal = 0;
        }

        int nVal, nPer;
        for (int j = 0; j < nChuteNum; j++)
        {
            getBkgValue(j * 2 + i, nVal, nPer);
            if (nVal != -1)
            {
                if (nColor == COLOR_OPT_BLACK)
                {
                    //! 黑背景寻找最大值
                    if (nVal > nBkgVal)
                    {
                        bIsChanged = true;
                        nBkgVal = nVal;
                    }
                }
                else if (nColor == COLOR_OPT_WHITE)
                {
                    //! 白背景寻找最小值
                    if (nVal < nBkgVal)
                    {
                        bIsChanged = true;
                        nBkgVal = nVal;
                    }
                }
                else
                {
                    //! 红绿蓝背景寻找颜色值最小值及颜色比例
                    if (nVal < nBkgVal)
                    {
                        bIsChanged = true;
                        nBkgVal = nVal;
                    }
                    if (nPer < nBkgPer)
                    {
                        bIsChanged = true;
                        nBkgPer = nPer;
                    }
                }
            }
        }

        if (bIsChanged)
        {
            if (struCnfp.nBackgroundColor[ONE_LEVEL][i] == COLOR_OPT_BLACK)
            {
                struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = nBkgVal + nValOffset;
            }
            else if (struCnfp.nBackgroundColor[ONE_LEVEL][i] == COLOR_OPT_WHITE)
            {
                struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = nBkgVal - nValOffset;
            }
            else
            {
                struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = nBkgVal - nValOffset;
                struCnfp.nBackgroundColorPer[ONE_LEVEL][i] = nBkgPer - nPerOffset;
            }
        }

        //! 判断背景值限制条件
        if (struCnfp.nBackgroundColorValue[ONE_LEVEL][i] < 0)
        {
            struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = 0;
        }

        if (struCnfp.nBackgroundColorValue[ONE_LEVEL][i] > 255)
        {
            struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = 255;
        }

        if (struCnfp.nBackgroundColorPer[ONE_LEVEL][i] < 0)
        {
            struCnfp.nBackgroundColorPer[ONE_LEVEL][i] = 0;
        }

        if (struCnfp.nBackgroundColorPer[ONE_LEVEL][i] > 100)
        {
            struCnfp.nBackgroundColorPer[ONE_LEVEL][i] = 100;
        }
    }
}

/*!
 * \brief autoSigWidget::updateAutoSigWid
 */
void autoSigWidget::updateAutoSigWid()
{
    if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_OPERATOR)
    {
        m_autoSigSettingBtn->setEnabled(false);
    }
    else
    {
        m_autoSigSettingBtn->setEnabled(true);
    }
    if (struCnfg.nAutoSigEnable)
    {
        if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_OPERATOR)
        {
            m_autoSigInitBtn->setEnabled(false);
        }
        else
        {
            m_autoSigInitBtn->setEnabled(true);
        }

        if (struCnfp.nAutoSigInit)
        {
            m_autoSigBtn->setEnabled(true);
        }
        else
        {
            m_autoSigBtn->setEnabled(false);
        }
    }
    else
    {
        m_autoSigInitBtn->setEnabled(false);
        m_autoSigBtn->setEnabled(false);
    }
}

/*!
 * \brief autoSigWidget::onAutoSigBtnPressedSlt
 */
void autoSigWidget::onAutoSigBtnPressedSlt()
{
    int tmpFeedStat = struGsh.bStatFeed;
    char buf[24] = {};
    int nAddr;
    int  sigVal[MAX_LEVEL][MAX_UNIT][3];
    int nFeederEn[10];
    int feederTemp[10];

    if (struGsh.bStatFeed)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    //! init
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        for (int j = 0; j < MAX_UNIT; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                sigVal[i][j][k] = 255;
            }
        }
    }

    myMessageBox msgBox(MSG_QUES, myLan.cfm_auto_set);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        struGsh.bFlagManuwipe = 1;
        myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration + 3);

        //! 复合清灰模式
        if (struCnfg.nWipeMode)
        {
            myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration2);
        }

        infoWidget->setLabelText(myLan.msg_auto_calibrating);
        infoWidget->delayShow();

#if 1
        //! 设置供料量
        int feedEnd;
        feedEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[0].nUnitCount / 2 - 1;
        for (int i = 0;i < 10;i++)
        {
            feederTemp[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
            nFeederEn[i] = struCnfp.struGroupCtrl[0].nFeederEnable[i];
        }
        for (int i = 0;i < 10;i++)
        {
            if (i <= feedEnd)
            {
                if (struCnfg.nAutoFeederSet == 1)
                {
                    struCnfp.struGroupCtrl[0].nFeeder[i] = struCnfg.nMaterPer2 * 10;
                }
                struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
            }
        }
        myFlow.resetFeeder(0);

        for (int i = 0;i < 10;i++)
        {
            LOG_INFO_STM("idx:" << i << " feeder:" << struCnfp.struGroupCtrl[0].nFeeder[i] << " status:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
        }

        //! 开
        if (!tmpFeedStat)
        {
            myFlow.onOff();
            myFlow.sleep(3);
        }
        struGsh.bFlagAutowipe = 0;

        //! start
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                MySerial.com1Write(CMD_UNIT_ADJ_ON_OFF, UNIT, i, nAddr, 0, 0, 0, 0, 1, 3);
            }
        }
        myFlow.sleep(20);

        //! stop
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                MySerial.com1Write(CMD_UNIT_ADJ_ON_OFF, UNIT, i, nAddr, 0, 0, 0, 0, 0, 3);
            }
        }

        //!  关
        if (!tmpFeedStat)
        {
            myFlow.onOff();
        }

        int tmpRetType = 0;
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
                {
                    nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                    for (int k = 0; k < 3; k++)
                    {
                        MySerial.com1Write(CMD_UNIT_GET_LIGHT, UNIT, i, nAddr, 0, 0, 0, 0, 1, 3);
                        usleep(400000);
                        tmpRetType = 255;
                        if ((ret = MySerial.com1Read(buf, 24)) == 24)
                        {
                            if (buf[0] == 0xAA && buf[1] == 0xAA && buf[2] == 0xAA && buf[3] == 0xAA)
                            {
                                tmpRetType = buf[14];

                                if (tmpRetType != 255)
                                {
                                    sigVal[i][nAddr][0] = buf[15];
                                    sigVal[i][nAddr][1] = buf[16];
                                    sigVal[i][nAddr][2] = buf[17];
                                }
                                break;
                            }
                        }
                    }
                    if (tmpRetType == 255)
                    {
                        break;
                    }
                }
                if (tmpRetType == 255)
                {
                    break;
                }
            }
            if (tmpRetType == 255)
            {
                break;
            }
        }

        if (tmpRetType == 255)
        {
            infoWidget->setLabelText(myLan.ai_unsupervised_less_point);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
            return;
        }

        int tmpAdjVal1 = 0, tmpAdjVal2 = 0, tmpAdjVal3 = 0;
        int tmpAutoReferChute;
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
                {
                    nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                    tmpAutoReferChute = (nAddr % 2) ? ((struCnfg.nAutoReferChute - 1) * 2 + 1) : ((struCnfg.nAutoReferChute - 1) * 2);
                    printf("chute cur:%d-r:%d-g:%d-b:%d\n", nAddr, sigVal[i][nAddr][0], sigVal[i][nAddr][1], sigVal[i][nAddr][2]);
                    printf("chute tar:%d-r:%d-g:%d-b:%d\n", nAddr, struCnfp.nAutoSigVal[i][tmpAutoReferChute][0], struCnfp.nAutoSigVal[i][tmpAutoReferChute][1], struCnfp.nAutoSigVal[i][tmpAutoReferChute][2]);
                    if (abs(struCnfp.nAutoSigVal[i][tmpAutoReferChute][0] - sigVal[i][nAddr][0]) > 5
                        || abs(struCnfp.nAutoSigVal[i][tmpAutoReferChute][1] - sigVal[i][nAddr][1]) > 5
                        || abs(struCnfp.nAutoSigVal[i][tmpAutoReferChute][2] - sigVal[i][nAddr][2]) > 5)
                    {
                        if (abs(struCnfp.nAutoSigVal[i][tmpAutoReferChute][0] - sigVal[i][nAddr][0]) > 100
                            || abs(struCnfp.nAutoSigVal[i][tmpAutoReferChute][1] - sigVal[i][nAddr][1]) > 100
                            || abs(struCnfp.nAutoSigVal[i][tmpAutoReferChute][2] - sigVal[i][nAddr][2]) > 100)
                        {
                            continue;
                        }
                        if (sigVal[i][nAddr][0] == 255 && sigVal[i][nAddr][1] == 255 && sigVal[i][nAddr][2] == 255)
                        {
                            continue;
                        }
                        if (sigVal[i][nAddr][0] == 0 || sigVal[i][nAddr][1] == 0 || sigVal[i][nAddr][2] == 0)
                        {
                            continue;
                        }

                        tmpAdjVal1 = (int)((struCnfp.nAutoSigVal[i][tmpAutoReferChute][0] * struCnfp.nAutoAdVal[i][nAddr][0]) / sigVal[i][nAddr][0]);
                        tmpAdjVal2 = (int)((struCnfp.nAutoSigVal[i][tmpAutoReferChute][1] * struCnfp.nAutoAdVal[i][nAddr][1]) / sigVal[i][nAddr][1]);
                        tmpAdjVal3 = (int)((struCnfp.nAutoSigVal[i][tmpAutoReferChute][2] * struCnfp.nAutoAdVal[i][nAddr][2]) / sigVal[i][nAddr][2]);

                        if (tmpAdjVal1 > 1638)
                        {
                            tmpAdjVal1 = 1638;
                        }
                        else if (tmpAdjVal1 < 512)
                        {
                            tmpAdjVal1 = 512;
                        }

                        if (tmpAdjVal2 > 1638)
                        {
                            tmpAdjVal2 = 1638;
                        }
                        else if (tmpAdjVal2 < 512)
                        {
                            tmpAdjVal2 = 512;
                        }

                        if (tmpAdjVal3 >= 1638)
                        {
                            tmpAdjVal3 = 1638;
                        }
                        else if (tmpAdjVal3 < 512)
                        {
                            tmpAdjVal3 = 512;
                        }

                        printf("val-(chute:%d)-(r:%d)-(g:%d)-(b:%d)\n", j, tmpAdjVal1, tmpAdjVal2, tmpAdjVal3);

                        struCnfp.nAutoAdVal[i][nAddr][0] = tmpAdjVal1;
                        struCnfp.nAutoAdVal[i][nAddr][1] = tmpAdjVal2;
                        struCnfp.nAutoAdVal[i][nAddr][2] = tmpAdjVal3;
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, tmpAdjVal1 / 256, tmpAdjVal1 % 256, 3);
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, tmpAdjVal2 / 256, tmpAdjVal2 % 256, 3);
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, tmpAdjVal3 / 256, tmpAdjVal3 % 256, 3);
                    }
                    else
                    {
                        //  tmpAdjVal1 = 1024;
                        //  tmpAdjVal2 = 1024;
                        //  tmpAdjVal3 = 1024;
                        tmpAdjVal1 = struCnfp.nAutoAdVal[i][nAddr][0];
                        tmpAdjVal2 = struCnfp.nAutoAdVal[i][nAddr][1];
                        tmpAdjVal3 = struCnfp.nAutoAdVal[i][nAddr][2];
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, tmpAdjVal1 / 256, tmpAdjVal1 % 256, 3);
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, tmpAdjVal2 / 256, tmpAdjVal2 % 256, 3);
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, tmpAdjVal3 / 256, tmpAdjVal3 % 256, 3);
                    }
                }
            }
        }

        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
                {
                    nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                    tmpAdjVal1 = struCnfp.nAutoAdVal[i][nAddr][0];
                    tmpAdjVal2 = struCnfp.nAutoAdVal[i][nAddr][1];
                    tmpAdjVal3 = struCnfp.nAutoAdVal[i][nAddr][2];
                    MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, tmpAdjVal1 / 256, tmpAdjVal1 % 256, 3);
                    MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, tmpAdjVal2 / 256, tmpAdjVal2 % 256, 3);
                    MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, tmpAdjVal3 / 256, tmpAdjVal3 % 256, 3);
                }
            }
            for (int j = 2; j < 6; j++)
            {
                for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
                {
                    nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                    if (nAddr % 2 == 0)
                    {
                        tmpAutoReferChute = (struCnfg.nAutoReferChute - 1) * 2;
                    }
                    else
                    {
                        tmpAutoReferChute = (struCnfg.nAutoReferChute - 1) * 2 + 1;
                    }
                    tmpAdjVal1 = struCnfp.nAutoAdVal[i][tmpAutoReferChute][0];
                    tmpAdjVal2 = struCnfp.nAutoAdVal[i][tmpAutoReferChute][1];
                    tmpAdjVal3 = struCnfp.nAutoAdVal[i][tmpAutoReferChute][2];
                    MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 1, tmpAdjVal1 / 256, tmpAdjVal1 % 256, 3);
                    MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 2, tmpAdjVal2 / 256, tmpAdjVal2 % 256, 3);
                    MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, 3, tmpAdjVal3 / 256, tmpAdjVal3 % 256, 3);

                }
            }
        }

        //! 恢复供料量
        for (int i = 0; i < 10; i++)
        {
            struCnfp.struGroupCtrl[0].nFeeder[i] = feederTemp[i];
            struCnfp.struGroupCtrl[0].nFeederEnable[i] = nFeederEn[i];
        }
        myFlow.resetFeeder(0);
        for (int i = 0; i < 10; i++)
        {
            LOG_INFO_STM("idx:" << i << " feeder:" << struCnfp.struGroupCtrl[0].nFeeder[i] << " status:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
        }
#endif

#if 0
        getHist();
#endif
        infoWidget->hide();
    }
}

/*!
 * \brief autoSigWidget::onAutoSigInitBtnPressedSlt
 */
void autoSigWidget::onAutoSigInitBtnPressedSlt()
{
    int tmpFeedStat = struGsh.bStatFeed;
    char buf[24] = {};
    int nAddr;
    int nFeederEn[10];
    int feederTemp[10];

    if (struGsh.bStatFeed)
    {
        infoWidget->setLabelText(myLan.msg_turn_off_first);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

    myMessageBox msgBox(MSG_QUES, myLan.cfm_init_autoSig);
    int ret = msgBox.exec();

    if (ret == QDialog::Accepted)
    {
        struGsh.bFlagManuwipe = 1;
        myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration + 3);

        //! 复合清灰模式
        if (struCnfg.nWipeMode)
        {
            myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration2);
        }

        infoWidget->setLabelText(myLan.msg_initing_autoSig);
        infoWidget->delayShow();


#if 1
        //! 设置供料量
        int feedEnd;
        feedEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[0].nUnitCount / 2 - 1;
        for (int i = 0;i < 10;i++)
        {
            feederTemp[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
            nFeederEn[i] = struCnfp.struGroupCtrl[0].nFeederEnable[i];
        }
        for (int i = 0;i < 10;i++)
        {
            if (i <= feedEnd)
            {
                if (struCnfg.nAutoFeederSet == 1)
                {
                    struCnfp.struGroupCtrl[0].nFeeder[i] = struCnfg.nMaterPer2 * 10;
                }
                struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
            }
        }
        myFlow.resetFeeder(0);
        for (int i = 0;i < 10;i++)
        {
            LOG_INFO_STM("idx:" << i << " feeder:" << struCnfp.struGroupCtrl[0].nFeeder[i] << " status:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
        }

        //! 开
        if (!tmpFeedStat)
        {
            myFlow.onOff();
            myFlow.sleep(3);
        }
        struGsh.bFlagAutowipe = 0;

        //! start
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                MySerial.com1Write(CMD_UNIT_ADJ_ON_OFF, UNIT, i, nAddr, 0, 0, 0, 0, 1, 3);
            }
        }

        myFlow.sleep(20);

        //! stop
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
            {
                nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                MySerial.com1Write(CMD_UNIT_ADJ_ON_OFF, UNIT, i, nAddr, 0, 0, 0, 0, 0, 3);
            }
        }

        //!  关
        if (!tmpFeedStat)
        {
            myFlow.onOff();
        }

        int tmpRetType = 0;
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int m = 0; m < struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitCount; m++)
                {
                    nAddr = struCnfg.struLevelInfo[i].struIdentifyGroupInfo[j].nUnitId[m];
                    if (nAddr == (struCnfg.nAutoReferChute - 1) * 2 || nAddr == (struCnfg.nAutoReferChute - 1) * 2 + 1)
                    {
                        printf("cur chute:%d\n", nAddr);
                        for (int k = 0; k < 3;k++)
                        {
                            struCnfp.nAutoSigInit = 0;
                            MySerial.com1Write(CMD_UNIT_GET_LIGHT, UNIT, i, nAddr, 0, 0, 0, 0, 1, 3);
                            usleep(400000);
                            tmpRetType = 0;
                            if ((ret = MySerial.com1Read(buf, 24)) == 24)
                            {
                                if (buf[0] == 0xAA && buf[1] == 0xAA && buf[2] == 0xAA && buf[3] == 0xAA)
                                {
                                    tmpRetType = buf[14];
                                    if (tmpRetType != 255)
                                    {
                                        struCnfp.nAutoSigVal[i][nAddr][0] = buf[15];
                                        struCnfp.nAutoSigVal[i][nAddr][1] = buf[16];
                                        struCnfp.nAutoSigVal[i][nAddr][2] = buf[17];
                                        struCnfp.nAutoSigInit = 1;
                                        printf("sig-chute:%d-r:%d-g:%d-b:%d\n", nAddr, struCnfp.nAutoSigVal[i][nAddr][0], struCnfp.nAutoSigVal[i][nAddr][1], struCnfp.nAutoSigVal[i][nAddr][2]);
                                    }
                                    break;
                                }
                            }
                        }
                        if (struCnfp.nAutoSigInit == 0)
                        {
                            break;
                        }
                    }
                }
                if (nAddr == (struCnfg.nAutoReferChute - 1) * 2 || nAddr == (struCnfg.nAutoReferChute - 1) * 2 + 1)
                {
                    if (struCnfp.nAutoSigInit == 0)
                    {
                        break;
                    }
                }
            }
            if (nAddr == (struCnfg.nAutoReferChute - 1) * 2 || nAddr == (struCnfg.nAutoReferChute - 1) * 2 + 1)
            {
                if (struCnfp.nAutoSigInit == 0)
                {
                    break;
                }
            }
        }

        //! 恢复供料量
        for (int i = 0; i < 10; i++)
        {
            struCnfp.struGroupCtrl[0].nFeeder[i] = feederTemp[i];
            struCnfp.struGroupCtrl[0].nFeederEnable[i] = nFeederEn[i];
        }

        myFlow.resetFeeder(0);
        for (int i = 0; i < 10; i++)
        {
            LOG_INFO_STM("idx:" << i << " feeder:" << struCnfp.struGroupCtrl[0].nFeeder[i] << " status:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
        }
#endif

#if 1
        if (!struCnfp.nAutoSigInit)
        {
            infoWidget->setLabelText(myLan.msg_init_autoSig_fail);
            infoWidget->delayShow();
            myFlow.sleep(3);
        }
#endif
        updateAutoSigWid();

#if 0
        getHist();
#endif
        infoWidget->hide();
    }
}
/*!
 * \brief autoSigWidget::onAutoSigSettingBtnPressedSlt
 */
void autoSigWidget::onAutoSigSettingBtnPressedSlt()
{
    this->m_stackedSigWid->setCurrentIndex(SIG_SETTING_PAGE);
}

/*!
 * \brief autoSigWidget::onAutoSigBackBtnPressedSlt
 */
void autoSigWidget::onAutoSigBackBtnPressedSlt()
{
    emit goToHomePage();
}
/*!
 * \brief autoSigWidget::onSettingPageCommonProcessedSlt
 */
void autoSigWidget::onSettingPageCommonProcessedSlt()
{
    if (sender() == m_plusBtn)
    {
        if (struCnfg.nAutoReferChute < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal / 2)
        {
            struCnfg.nAutoReferChute++;
            m_lcdNum->display(struCnfg.nAutoReferChute);
        }
    }
    if (sender() == m_minusBtn)
    {
        if (struCnfg.nAutoReferChute > 1)
        {
            struCnfg.nAutoReferChute--;
            m_lcdNum->display(struCnfg.nAutoReferChute);
        }
    }
    if (sender() == m_feedLne)
    {
        myInputPanel inputDlg(intType, 0, 100, struCnfg.nMaterPer2);
        inputDlg.exec();
        struCnfg.nMaterPer2 = (int)inputDlg.getValue();
        m_feedLne->setText(QString("%1").arg(struCnfg.nMaterPer2));
    }
    if (sender() == m_settingBackBtn)
    {
        this->m_stackedSigWid->setCurrentIndex(SIG_MAIN_PAGE);
    }
    if (sender() == m_enableCbx)
    {
        if (m_enableCbx->getChecked())
        {
            struCnfg.nAutoSigEnable = 1;
        }
        else
        {
            struCnfg.nAutoSigEnable = 0;
        }
        if (!struCnfg.nAutoSigEnable)
        {
            struCnfp.nAutoSigInit = 0;
            int tmpAdjVal2 = 0, nAddr;
            for (int i = 0; i < struCnfg.nLevelTotal; i++)
            {
                for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
                {
                    nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
                    for (int k = 0; k < 3; k++)
                    {
                        tmpAdjVal2 = 1024;
                        struCnfp.nAutoAdVal[i][nAddr][k] = tmpAdjVal2;
                        MySerial.com1Write(CMD_UNIT_ADJ_COEF, UNIT, i, nAddr, 0, 0, k + 1, tmpAdjVal2 / 256, tmpAdjVal2 % 256, 3);
                    }
                }
            }
        }
        updateAutoSigWid();
    }
}
