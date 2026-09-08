#include "background.h"

/* 构造背景设置页面 */
PageBackground::PageBackground(QWidget *parent)
    : QWidget(parent)
{
    resetBkgLampIndex();

/* page init */
    createPage();

/* page layout */
    layoutPage();

/* page update */
    updatePage();
}

/* 应用当前背景设置页面参数 */
void PageBackground::setBackgroundParams()
{
    /* write params to memory */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < MAX_BACKGROUND_GROUP; j++) {
            struCnfp.nBackgroundColor[i][j]      = m_nColor[i][j];
            struCnfp.nBackgroundColorValue[i][j] = m_nThreshold[i][j];
            struCnfp.nBackgroundColorPer[i][j]   = m_nPercent[i][j];
        }
    }
}

/* 重置当前背景设置页面参数 */
void PageBackground::resetBackgroundParams()
{
    /* reset background color params */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < MAX_BACKGROUND_GROUP; j++) {
            m_nColor[i][j]     = struCnfp.nBackgroundColor[i][j];
            m_nThreshold[i][j] = struCnfp.nBackgroundColorValue[i][j];
            m_nPercent[i][j]   = struCnfp.nBackgroundColorPer[i][j];
        }
    }

    /* trigger signal to stop myCurve thread */

    /* trigger signal to page update */
    emit pageUpdated();
}

/* 响应背景颜色向左切换设置  */
void PageBackground::setColorMinusSlt()
{
    int nIndex = struGsh.nLevel;
    int backgroundID = 0;
    int color = 0;

    backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);
    color = m_nColor[nIndex][backgroundID];

    switch (color) {
    case COLOR_OPT_BLACK:
        color = COLOR_OPT_WHITE;
        break;
    case COLOR_OPT_R:
        color = COLOR_OPT_BLACK;
        break;
    case COLOR_OPT_G:
        color = COLOR_OPT_R;
        break;
    case COLOR_OPT_B:
        color = COLOR_OPT_G;
        break;
    case COLOR_OPT_WHITE:
        color = COLOR_OPT_B;
        break;
    case COLOR_OPT_AUTO:
        color = COLOR_OPT_WHITE;
        break;
    default:
        break;
    }

    if (color != m_nColor[nIndex][backgroundID]) {
        struCnfp.nBackgroundColor[nIndex][backgroundID] = color;
        m_nColor[nIndex][backgroundID] = color;
        colorLabel->setText(getColorText(color));

        resetBackgroundColorSlt();
    }
}

/* 响应背景颜色向右切换设置  */
void PageBackground::setColorPlusSlt()
{
    int nIndex = struGsh.nLevel;
    int backgroundID = 0;
    int color = 0;

    backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);
    color = m_nColor[nIndex][backgroundID];

    switch (color) {
    case COLOR_OPT_BLACK:
        color = COLOR_OPT_R;
        break;
    case COLOR_OPT_R:
        color = COLOR_OPT_G;
        break;
    case COLOR_OPT_G:
        color = COLOR_OPT_B;
        break;
    case COLOR_OPT_B:
        color = COLOR_OPT_WHITE;
        break;
    case COLOR_OPT_WHITE:
        color = COLOR_OPT_BLACK;
        break;
    case COLOR_OPT_AUTO:
        color = COLOR_OPT_BLACK;
        break;
    default:
        break;
    }

    if (color != m_nColor[nIndex][backgroundID]) {
        struCnfp.nBackgroundColor[nIndex][backgroundID] = color;
        m_nColor[nIndex][backgroundID] = color;
        colorLabel->setText(getColorText(color));
        resetBackgroundColorSlt();
    }
}

void PageBackground::onColorGroupIndexChange(int index)
{
    int backgroundID = 0;
    backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);
}


//自动背景阈值设置  RSC
void PageBackground::setAutoBgThresholdValueSlt_RSC()
{
    /* get text params */
    int nTmp = m_autoThresholdValue_RSC->text().toInt();

    /* trigger input dialog */
    myInputPanel inputDlg(intType, 0, 255, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        nTmp = inputDlg.getValue();
        /* page update */
        QString text = QString("%1").arg(nTmp);
        m_autoThresholdValue_RSC->setText(text);

        struCnfp.nAutoBackgroundThresholdValue_RSC = nTmp;

        qDebug("reset AutoBackground\n");

        myFlow.resetAutoBackgroundColor();
    }
}


/* 响应背景颜色阈值设置 */
void PageBackground::setThresholdValueSlt()
{
    int nIndex = struGsh.nLevel;
    /* get text params */
    int nTmp = thresholdValue->text().toInt();
    int backgroundID = 0;

    backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);

    /* trigger input dialog */
    myInputPanel inputDlg(intType, 0, 255, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nTmp = inputDlg.getValue();

        /* params update */
        struCnfp.nBackgroundColorValue[nIndex][backgroundID] = m_nThreshold[nIndex][backgroundID]= nTmp;


        /* page update */
        QString text = QString("%1").arg(nTmp);
        thresholdValue->setText(text);

        /* trigger */
        resetBackgroundColorSlt();
    }
}

/* 响应背景颜色比例设置 */
void PageBackground::setPercentValueSlt()
{
    int nIndex = struGsh.nLevel;
    /* get text params */
    int backgroundID = 0;
    int nTmp = 0;

    backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);

    /* trigger input dialog */
    int max = 100;
    nTmp = m_nPercent[nIndex][backgroundID];
    myInputPanel inputDlg(intType, 0, max, nTmp);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nTmp = inputDlg.getValue();

        /* params update */
        QString text;
        struCnfp.nBackgroundColorPer[nIndex][backgroundID] = m_nPercent[nIndex][backgroundID] = nTmp;
        text = QString("%1%").arg(nTmp);

        /* page update */
        percentValue->setText(text);

        /* trigger */
        resetBackgroundColorSlt();
    }
}

/* 响应背景角度设置 */
void PageBackground::setBackAngleValueSlt()
{
    int nBackAngleValue;
    int sModuleAddr = 0;
    if (struGsh.nUnit%2){
        nBackAngleValue = struCnfp.struGroupCtrl[0].nBackgroundFront;
    } else {
        nBackAngleValue = struCnfp.struGroupCtrl[0].nBackgroundRear;
    }
    /* trigger input dialog */
    myInputPanel inputDlg(intType, 0, 360, nBackAngleValue);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        nBackAngleValue = inputDlg.getValue();

        /* params update */
        if (struGsh.nUnit%2){
            struCnfp.struGroupCtrl[0].nBackgroundFront = nBackAngleValue;
        } else {
            struCnfp.struGroupCtrl[0].nBackgroundRear = nBackAngleValue;
        }

        /* page update */
        QString text = QString("%1").arg(nBackAngleValue);
        backAngleValue->setText(text);

        /* trigger */

        MySerial.com2Write(CMD_CTRL_BACKGROUND, CTRL_GENERAL, 0, 0,
                           struCnfp.struGroupCtrl[sModuleAddr].nBackgroundRear/256,
                           struCnfp.struGroupCtrl[sModuleAddr].nBackgroundFront/256,
                           struCnfp.struGroupCtrl[sModuleAddr].nBackgroundRear%256,
                           struCnfp.struGroupCtrl[sModuleAddr].nBackgroundFront%256, 3);
    }
}

/* 响应层切换设置 */
void PageBackground::setLevelValueSlt()
{
    /* update widget text */
    displayText(struGsh.nLevel);
}

/* 下发背景参数 */
void PageBackground::resetBackgroundColorSlt()
{
    int backgroundID = 0;

    backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);
    /* write params to fpga */
    switch (struCnfp.nBackgroundColor[struGsh.nLevel][backgroundID]) {
    case COLOR_OPT_BLACK:
    case COLOR_OPT_WHITE:
        MySerial.com1Write(CMD_UNIT_BG_COLOR, UNIT, struGsh.nLevel, struGsh.nUnit, 0,
                           (struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroundID]*3)/256, struCnfp.nBackgroundColor[struGsh.nLevel][backgroundID],
                           (struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroundID]*3)%256, 255*struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroundID]/100, 3);
        break;
    case COLOR_OPT_R:
    case COLOR_OPT_G:
    case COLOR_OPT_B:
    case COLOR_OPT_AUTO:
        MySerial.com1Write(CMD_UNIT_BG_COLOR, UNIT, struGsh.nLevel, struGsh.nUnit, 0,
                           0, struCnfp.nBackgroundColor[struGsh.nLevel][backgroundID],
                           struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroundID], 255*struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroundID]/100, 3);
        break;
    default:
        break;
    }

    MySerial.com1Write(CMD_UNIT_BG_ADJUST, UNIT, struGsh.nLevel, struGsh.nUnit, 0,struCnfp.nAutoBgEnable[struGsh.nLevel][struGsh.nUnit],
            struCnfp.nAutoBgRange[struGsh.nLevel][struGsh.nUnit][0],struCnfp.nAutoBgRange[struGsh.nLevel][struGsh.nUnit][1],
            struCnfp.nAutoBgRange[struGsh.nLevel][struGsh.nUnit][2],3);
}

/*!
 * \brief 自动背景
 */
void PageBackground::onAutoBkgBtnPressed()
{
    //! 提示用户设置前后视背景颜色
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_set_background_color);
    int nRet = mesBox->exec();
    if (nRet == QDialog::Rejected) {
        return;
    }

    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    //! 关闭当前采集波形线程
    bool bIsRunning;
    if (myCurve->isRunning()) {
        bIsRunning = true;
        myCurve->onRowPauseBtnClicked();
    }
    myFlow.sleep(1);

    //! 获取所有相机背景条件
    setBkgValueAuto();

    //! 重置背景颜色值及颜色比例
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < MAX_BACKGROUND_GROUP; j++) {
            m_nThreshold[i][j] = struCnfp.nBackgroundColorValue[i][j];
            m_nPercent[i][j] = struCnfp.nBackgroundColorPer[i][j];
        }
    }

    //! 刷新页面显示
    thresholdValue->setText(QString("%1").setNum(m_nThreshold[ONE_LEVEL][struGsh.nUnit%2]));
    percentValue->setText(QString("%1%").arg(m_nPercent[ONE_LEVEL][struGsh.nUnit%2]));

    //! 恢复采集波形线程
    if (bIsRunning) {
        myCurve->onRowPauseBtnClicked();
    }

    infoWidget->delayHide();
    myFlow.resetBackgroundColor();
}

/*!
 * \brief 背景自适应
 */
void PageBackground::onAutoBkgBoxPressed()
{
    int nEnableFlag = m_autoBkgBox->isChecked();
    QList<int> qlUnits;

    if (nEnableFlag) {
        if (struGsh.bStatFeed == 1) {
            m_autoBkgBox->setChecked(false);
            infoWidget->setLabelText(myLan.msg_turn_off_first);
            infoWidget->showSecs();
            return;
        }
        m_autoBkgBtn->setEnabled(false);
    } else {
        m_autoBkgBtn->setEnabled(true);
    }

    myFlow.getUnitsForBkg(qlUnits,struGsh.nUnit);
    foreach(int tmp, qlUnits) {
        struCnfp.nAutoBgEnable[struGsh.nLevel][tmp] = nEnableFlag;
    }
    myFlow.resetBackgroundColor();
}

/*!
 * \brief 自动背景幅度
 */
void PageBackground::setbgRangeValueSlt(int index)
{
    int valueRGB = struCnfp.nAutoBgRange[struGsh.nLevel][struGsh.nUnit][index];
    QList<int> qlUnits;

    myInputPanel inputDlg(intType, 0, 255, valueRGB);
    if (inputDlg.exec() == QDialog::Accepted) {
        valueRGB = inputDlg.getValue();
        bgRangeValue[index]->setText(QString("%1").arg(valueRGB));
    }

    myFlow.getUnitsForBkg(qlUnits,struGsh.nUnit);
    foreach(int tmp, qlUnits) {
        struCnfp.nAutoBgRange[struGsh.nLevel][tmp][index] = valueRGB;
    }

    resetBackgroundColorSlt();
}

void PageBackground::setFrontBkRangeValueSlt(int index){
    inputType type;
    int min, max, value;

    /* trigger input dialog */
    type = intType;
    min = 0;
    max = 4095;
    value = frontBkRangeValue[index]->text().toInt();
    myInputPanel inputDlg(type, min, max, value);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get value */
        value = inputDlg.getValue();

        /* update params */
        struCnfp.struGroupCtrl[0].nLampLight[12+index]= value;

        /* update page */
        frontBkRangeValue[index]->setText(QString("%1").arg(value));

        /* write params */
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, (12+index)/6, 0, 0, (12+index)%6,    //每个智能恒流源底板最多控制6个灯
                        struCnfp.struGroupCtrl[0].nLampLight[12+index]/256,
                        struCnfp.struGroupCtrl[0].nLampLight[12+index]%256, 3);
    }
}

void PageBackground::setRearBkRangeValueSlt(int index){
    inputType type;
    int min, max, value;

    /* trigger input dialog */
    type = intType;
    min = 0;
    max = 4095;
    value = rearBkRangeValue[index]->text().toInt();
    myInputPanel inputDlg(type, min, max, value);
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get value */
        value = inputDlg.getValue();

        /* update params */
        struCnfp.struGroupCtrl[0].nLampLight[30+index]= value;

        /* update page */
        rearBkRangeValue[index]->setText(QString("%1").arg(value));

        /* write params */
        MySerial.com2Write(CMD_CTRL_SET_LIGHT, CTRL_LED, (30+index)/6, 0, 0, (30+index)%6,    //每个智能恒流源底板最多控制6个灯
                        struCnfp.struGroupCtrl[0].nLampLight[30+index]/256,
                        struCnfp.struGroupCtrl[0].nLampLight[30+index]%256, 3);
    }
}

/* 触发背景波形采集信号 */
void PageBackground::setMyCurveStart(bool start)
{
    if (start) {
        emit sBackgroundColorCurveStart();
    } else {
        emit sBackgroundColorCurveStop();
    }
}

/*!
 * \brief 获取单相机背景值
 * \param nUnit 相机地址
 * \param nVal 返回背景颜色
 * \param nPer 返回背景颜色比例
 */
void PageBackground::getBkgValue(int nUnit, int &nVal, int &nPer)
{
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, nUnit, 0, 0, 0, 0, 0, 1);
    int nLen = struCnfc.nSensorPixel*3+16;
    myFlow.msleep(600);

    memset(struGsh.sRgbRow, 0, sizeof(struGsh.sRgbRow));
    int nRet = MySerial.com1Read(struGsh.sRgbRow, nLen);
    uchar *p = (uchar *)struGsh.sRgbRow;

    int nColor;
    int sum = 0;
    int num = 1;
    if (nRet == nLen && p[0] == 0xAA && p[1] == 0xAA
            && p[2] == 0xAA && p[3] == 0xAA) {
        if (nUnit%2 == 0) {
            nColor = struCnfp.nBackgroundColor[ONE_LEVEL][0];
        } else {
            nColor = struCnfp.nBackgroundColor[ONE_LEVEL][1];
        }
        p = (uchar *)struGsh.sRgbRow+10;

        int nBegin = struCnfc.struLevelCamera[ONE_LEVEL].nChannelBegin[nUnit];
        int nEnd = struCnfc.struLevelCamera[ONE_LEVEL].nChannelEnd[nUnit];
        int r = p[nBegin*3];
        int g = p[nBegin*3+1];
        int b = p[nBegin*3+2];

        //! 检查象元
        num = nEnd-nBegin+1;
        if (num <= 0) {
            return;
        }

        nVal = -1;
        nPer = -1;
        switch (nColor) {
        case COLOR_OPT_R:
            nPer = ((float)r/(r+g+b))*100;
            for (int i = nBegin; i < nEnd; i++) {
                sum += p[i*3];
            }
            nVal = sum/num;

            for (int i = nBegin; i < nEnd; i++) {
                if (abs(p[i*3] - nVal) > 30
                        || (p[i*3]+p[3*i+1]+p[3*i+2]) <= 0) {
                    continue;
                }

                int nTmp = ((float)p[i*3]/(p[i*3]+p[3*i+1]+p[3*i+2]))*100;
                if (nTmp < nPer) {
                    nPer = nTmp;
                }
            }
            break;

        case COLOR_OPT_G:
            nPer = ((float)g/(r+g+b))*100;
            for (int i = nBegin; i < nEnd; i++) {
                sum += p[i*3+1];
            }
            nVal = sum/num;

            for (int i = nBegin; i < nEnd; i++) {
                if (abs(p[i*3+1] - nVal) > 30
                        || (p[i*3]+p[3*i+1]+p[3*i+2]) <= 0) {
                    continue;
                }

                int nTmp = ((float)p[i*3+1]/(p[i*3]+p[3*i+1]+p[3*i+2]))*100;
                if (nTmp < nPer) {
                    nPer = nTmp;
                }
            }
            break;

        case COLOR_OPT_B:
            nPer = ((float)b/(r+g+b))*100;
            for (int i = nBegin; i < nEnd; i++) {
                sum += p[i*3+2];
            }
            nVal = sum/num;

            for (int i = nBegin; i < nEnd; i++) {
                if (abs(p[i*3+2] - nVal) > 30
                        || (p[i*3]+p[3*i+1]+p[3*i+2]) <= 0) {
                    continue;
                }

                int nTmp = ((float)p[i*3+2]/(p[i*3]+p[3*i+1]+p[3*i+2]))*100;
                if (nTmp < nPer) {
                    nPer = nTmp;
                }
            }
            break;

        case COLOR_OPT_WHITE:
            for (int i = nBegin; i < nEnd; i++) {
                sum += (p[i*3]+p[i*3+1]+p[i*3+2])/3;
            }
            nVal = sum/num;
            break;

        case COLOR_OPT_BLACK:
            for (int i = nBegin; i < nEnd; i++) {
                sum += (p[i*3]+p[i*3+1]+p[i*3+2])/3;
            }
            nVal = sum/num;
            break;

        default:
            break;
        }
    }

    qDebug("%d: %d %d", nUnit, nVal, nPer);
}

/*!
 * \brief 自动设置背景值
 */
void PageBackground::setBkgValueAuto()
{
    int nChuteNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal/2;
    int nValOffset = 30;
    int nPerOffset = 5;

    //! 设置前后视背景(i=0为前视, i=1为后视)
    for (int i = 0; i < 2; i++) {
        bool bIsChanged = false;
        int nColor = struCnfp.nBackgroundColor[ONE_LEVEL][i];
        int nBkgVal = 255;
        int nBkgPer = 100;
        if (nColor == COLOR_OPT_BLACK) {
            nBkgVal = 0;
        }

        int nVal = nBkgVal;
		int nPer = nBkgPer;
        for (int j = 0; j < nChuteNum; j++) {
            getBkgValue(j*2+i, nVal, nPer);
            if (nVal != -1) {
                if (nColor == COLOR_OPT_BLACK) {
                    //! 黑背景寻找最大值
                    if (nVal > nBkgVal) {
                        bIsChanged = true;
                        nBkgVal = nVal;
                    }
                } else if (nColor == COLOR_OPT_WHITE) {
                    //! 白背景寻找最小值
                    if (nVal < nBkgVal) {
                        bIsChanged = true;
                        nBkgVal = nVal;
                    }
                } else {
                    //! 红绿蓝背景寻找颜色值最小值及颜色比例
                    if (nVal < nBkgVal) {
                        bIsChanged = true;
                        nBkgVal = nVal;
                    }
                    if (nPer < nBkgPer) {
                        bIsChanged = true;
                        nBkgPer = nPer;
                    }
                }
            }
        }

        if (bIsChanged) {
            if (struCnfp.nBackgroundColor[ONE_LEVEL][i] == COLOR_OPT_BLACK) {
                struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = nBkgVal+nValOffset;
            } else if (struCnfp.nBackgroundColor[ONE_LEVEL][i] == COLOR_OPT_WHITE) {
                struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = nBkgVal-nValOffset;
            } else {
                struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = nBkgVal-nValOffset;
                struCnfp.nBackgroundColorPer[ONE_LEVEL][i] = nBkgPer-nPerOffset;
            }
        }

        //! 判断背景值限制条件
        if (struCnfp.nBackgroundColorValue[ONE_LEVEL][i] < 0) {
            struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = 0;
        }

        if (struCnfp.nBackgroundColorValue[ONE_LEVEL][i] > 255) {
            struCnfp.nBackgroundColorValue[ONE_LEVEL][i] = 255;
        }

        if (struCnfp.nBackgroundColorPer[ONE_LEVEL][i] < 0) {
            struCnfp.nBackgroundColorPer[ONE_LEVEL][i] = 0;
        }

        if (struCnfp.nBackgroundColorPer[ONE_LEVEL][i] > 100) {
            struCnfp.nBackgroundColorPer[ONE_LEVEL][i] = 100;
        }
    }
}

/*!
 * \brief 重置背景灯索引
 */
void PageBackground::resetBkgLampIndex()
{
    /** R2机型：前视对应的背景灯为恒流源板1的第4盏灯
               后视对应的背景灯为恒流源板1的第3盏灯
        其它米机：前视对应的背景灯为恒流源板2的第3盏灯
                后视对应的背景灯为恒流源板1的第3盏灯
    */
    if (myMonoFlow.checkIsR2()) {
        m_nFrontBkgLamp = 5;
        m_nRearBkgLamp = 2;
    } else {
        if (struGsh.bIsAssist) {
            m_nFrontBkgLamp = 11;
            m_nRearBkgLamp = 5;
        } else {
            m_nFrontBkgLamp = 8;
            m_nRearBkgLamp = 2;
        }
    }
}

/* 初始化背景设置页面 */
void PageBackground::createPage()
{
    /* curve area */
    myCurve = new MyCurve(CURVE_WIDGET_WIDTH,CURVE_WIDGET_HEIGHT-20);

    /* function area */
    colorMinus = new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, BTN_HEIGHT-10));
    colorMinus->setIcon(myIcon.Arrow_Left);

    colorPlus  = new myPushButton(QIcon(), QSize(SMALL_BTN_WIDTH, BTN_HEIGHT-10));
    colorPlus->setIcon(myIcon.Arrow_Right);

    colorLabel     = new myLabel(myLan.red);
    colorLabel->setFixedSize(DEFAULT_BTN_WIDTH,BTN_HEIGHT-15);
    colorLabel->setAlignment(Qt::AlignCenter);
    thresholdLabel = new myLabel(myLan.threshold);
    thresholdLabel->setFixedHeight(BTN_HEIGHT-15);
//    thresholdLabel = new myLabel(QString("背景亮度"));

    percentLabel   = new myLabel(myLan.background_proportion);
    percentLabel->setFixedHeight(BTN_HEIGHT-15);

//    percentLabel   = new myLabel(QString("背景角度"));

    backAngleLabel = new myLabel(myLan.background_lamp);
    bgRangeLabel[0] = new myLabel("        "+myLan.red);
    bgRangeLabel[1] = new myLabel("        "+myLan.green);
    bgRangeLabel[2] = new myLabel("        "+myLan.blue);
    backAngleLabel->setFixedHeight(BTN_HEIGHT-15);
    bgRangeLabel[0]->setFixedHeight(BTN_HEIGHT-15);
    bgRangeLabel[1]->setFixedHeight(BTN_HEIGHT-15);
    bgRangeLabel[2]->setFixedHeight(BTN_HEIGHT-15);

    m_colorGroup = new myMutex(Qt::Horizontal,0);
    m_colorGroup->setLabelText(myLan.red, myLan.green);
    m_colorGroup->setFixedSize(BTN_WIDTH*2.6, BTN_HEIGHT*1.25);
    m_colorGroup->hide();

    thresholdValue = new myLineEdit("");
    thresholdValue->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);

    percentValue   = new myLineEdit("");
    percentValue->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);

    backAngleValue = new myLineEdit("");
    backAngleValue->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);

    //! 仅彩色米机使用自动背景
    m_autoBkgBtn = new myPushButton(myLan.automatic, QIcon());
    m_autoBkgBtn->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);
    m_autoBkgBtn->hide();

    //RSC下位机自动背景
    m_autoThresholdLabel_RSC = new myLabel(myLan.threshold);
    m_autoThresholdValue_RSC = new myLineEdit("");
    m_autoThresholdValue_RSC->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);

    //! 杂粮背景自适应
    m_autoBkgBox = new MyCheckBox(myLan.automatic,struCnfp.nAutoBgEnable);
    m_autoBkgBox->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    for (int i = 0; i < 3; i++) {
        bgRangeValue[i] = new myLineEdit(QString("%1").arg(struCnfp.nAutoBgRange[0][0][i]));
        bgRangeValue[i]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);
    }


    frontBkLabel = new myLabel(myLan.front+myLan.background_lamp);
//    frontBkRangeLabel[0] = new myLabel(myLan.red);
//    frontBkRangeLabel[1] = new myLabel(myLan.green);
//    frontBkRangeLabel[2] = new myLabel(myLan.blue);
    frontBkRangeValue[0] = new myLineEdit("");
    frontBkRangeValue[1] = new myLineEdit("");
    frontBkRangeValue[2] = new myLineEdit("");

    frontBkLabel->setFixedSize(DEFAULT_BTN_WIDTH+15, BTN_HEIGHT-15);;
//    frontBkRangeLabel[0]->setFixedHeight(BTN_HEIGHT-15);
//    frontBkRangeLabel[1]->setFixedHeight(BTN_HEIGHT-15);
//    frontBkRangeLabel[2]->setFixedHeight(BTN_HEIGHT-15);
    frontBkRangeValue[0]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);
    frontBkRangeValue[1]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);
    frontBkRangeValue[2]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);

    rearBkLabel = new myLabel(myLan.rear+myLan.background_lamp);
//    rearBackRangeLabel[0] = new myLabel(myLan.red);
//    rearBackRangeLabel[1] = new myLabel(myLan.green);
//    rearBackRangeLabel[2] = new myLabel(myLan.blue);
    rearBkRangeValue[0] = new myLineEdit("");
    rearBkRangeValue[1] = new myLineEdit("");
    rearBkRangeValue[2] = new myLineEdit("");

    rearBkLabel->setFixedSize(DEFAULT_BTN_WIDTH+15, BTN_HEIGHT-15);;
//    rearBkRangeLabel[0]->setFixedHeight(BTN_HEIGHT-15);
//    rearBkRangeLabel[1]->setFixedHeight(BTN_HEIGHT-15);
//    rearBkRangeLabel[2]->setFixedHeight(BTN_HEIGHT-15);
    rearBkRangeValue[0]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);
    rearBkRangeValue[1]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);
    rearBkRangeValue[2]->setFixedSize(DEFAULT_BTN_WIDTH, BTN_HEIGHT-15);

//    if (myFlow.getProductLineNo() == 0)
//    {
//        if (struCnfe.nMachine == MACHINE_CF) {
//            if (LCD_WIDTH == 640) {
//                m_autoBkgBox->hide();
//                for (int i = 0; i < 3; i++) {
//                    bgRangeLabel[i]->hide();
//                    bgRangeValue[i]->hide();
//                }
//            }
//        } else {
//            m_autoBkgBox->hide();
//            for (int i = 0; i < 3; i++) {
//                bgRangeLabel[i]->hide();
//                bgRangeValue[i]->hide();
//            }
//        }

//    }
//    else if(struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC)
//        ;
//    else
//    {
//        m_autoBkgBox->hide();
//        for (int i = 0; i < 3; i++) {
//            bgRangeLabel[i]->hide();
//            bgRangeValue[i]->hide();
//        }
//    }
    QSignalMapper *frontRangeMapper = new QSignalMapper(this);
    for (int i = 0; i < 3; i++) {
        frontRangeMapper->setMapping(frontBkRangeValue[i], i);
        connect(frontBkRangeValue[i], SIGNAL(pressed()), frontRangeMapper, SLOT(map()));
    }
    connect(frontRangeMapper, SIGNAL(mapped(int)), this, SLOT(setFrontBkRangeValueSlt(int)));

    QSignalMapper *rearRangeMapper = new QSignalMapper(this);
    for (int i = 0; i < 3; i++) {
        rearRangeMapper->setMapping(rearBkRangeValue[i], i);
        connect(rearBkRangeValue[i], SIGNAL(pressed()), rearRangeMapper, SLOT(map()));
    }
    connect(rearRangeMapper, SIGNAL(mapped(int)), this, SLOT(setRearBkRangeValueSlt(int)));



    /* signals to slots */
    QSignalMapper *bgRangeMapper = new QSignalMapper(this);
    for (int i = 0; i < 3; i++) {
        bgRangeMapper->setMapping(bgRangeValue[i], i);
        connect(bgRangeValue[i], SIGNAL(pressed()), bgRangeMapper, SLOT(map()));
    }
    connect(bgRangeMapper, SIGNAL(mapped(int)), this, SLOT(setbgRangeValueSlt(int)));
    connect(colorMinus,     SIGNAL(pressed()), this, SLOT(setColorMinusSlt()));
    connect(colorPlus,      SIGNAL(pressed()), this, SLOT(setColorPlusSlt()));
    connect(thresholdValue, SIGNAL(pressed()), this, SLOT(setThresholdValueSlt()));
    connect(percentValue,   SIGNAL(pressed()), this, SLOT(setPercentValueSlt()));
    connect(backAngleValue, SIGNAL(pressed()), this, SLOT(setBackAngleValueSlt()));
    connect(m_colorGroup,   SIGNAL(indexChanged(int)), this, SLOT(onColorGroupIndexChange(int)));
    connect(this,           SIGNAL(sBackgroundColorCurveStart()), myCurve, SLOT(curveStartSlt()));
    connect(this,           SIGNAL(sBackgroundColorCurveStop()),  myCurve, SLOT(curveStopSlt()));
    connect(myCurve,        SIGNAL(boardNumChanged()), this, SLOT(setLevelValueSlt()));
    connect(this,           SIGNAL(pageUpdated()), this, SLOT(updatePage()));
    connect(m_autoBkgBtn,   SIGNAL(pressed()), this, SLOT(onAutoBkgBtnPressed()));
    connect(m_autoBkgBox,   SIGNAL(pressed()), this, SLOT(onAutoBkgBoxPressed()));
    connect(m_autoThresholdValue_RSC,   SIGNAL(pressed()), this, SLOT(setAutoBgThresholdValueSlt_RSC()));
}

/* 显示当前背景设置页面控件文本 */
void PageBackground::displayText(int level)
{
    resetBkgLampIndex();
    /* background color text */
    int backgroundID = myFlow.getBackgroundGroupID(struGsh.nUnit);

    colorLabel->setText(getColorText(m_nColor[level][backgroundID]));
    thresholdValue->setText(QString("%1").setNum(m_nThreshold[level][backgroundID]));
    percentValue->setText(QString("%1%").arg(m_nPercent[level][backgroundID]));
//    if(struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC)//米机
//        m_autoBkgBox->setChecked(struCnfp.nAutoBackgroundEnanble_RSC);
//    else    //杂粮
        m_autoBkgBox->setChecked(struCnfp.nAutoBgEnable[struGsh.nLevel][struGsh.nUnit]);
    m_autoThresholdValue_RSC->setText(QString("%1").setNum(struCnfp.nAutoBackgroundThresholdValue_RSC));

    for (int i = 0; i < 3; i++) {
        bgRangeValue[i]->setText(QString("%1").arg(struCnfp.nAutoBgRange[struGsh.nLevel][struGsh.nUnit][i]));
    }

    for (int i = 0; i < 3; i++) {
        frontBkRangeValue[i]->setText(QString("%1").arg(struCnfp.struGroupCtrl[0].nLampLight[12+i]));
        rearBkRangeValue[i]->setText(QString("%1").arg(struCnfp.struGroupCtrl[0].nLampLight[30+i]));
    }

    percentValue->show();
    percentLabel->show();
    colorLabel->show();
    colorMinus->show();
    colorPlus->show();
    m_colorGroup->hide();
    percentLabel->setText(myLan.background_proportion);
//        percentLabel->setText(QString("背景角度"));
}

/* 更新背景设置页面 */
void PageBackground::updatePage()
{
/* page update */
    myCurve->refreshCurveDisplay();
    myCurve->setModeBtnStatus();

    /* background color */
    displayText(struGsh.nLevel);
}

/* 布局背景设置页面 */
void PageBackground::layoutPage()
{
/* page layout */
    /* color layout */
    QHBoxLayout *colorLayout  = new QHBoxLayout;
    colorLayout->addWidget(m_colorGroup);
    colorLayout->addWidget(colorMinus,Qt::AlignCenter);
    colorLayout->addWidget(colorLabel,Qt::AlignCenter);
    colorLayout->addWidget(colorPlus,Qt::AlignCenter);

    /* threshold layout */
    QHBoxLayout *thresholdLayout = new QHBoxLayout;
    thresholdLayout->addWidget(thresholdLabel,0,Qt::AlignCenter);
    thresholdLayout->setSpacing(20);
    thresholdLayout->addWidget(thresholdValue,1,Qt::AlignCenter);

    /* percent layout */
    QHBoxLayout *percentLayout = new QHBoxLayout;
    percentLayout->addWidget(percentLabel,0,Qt::AlignCenter);
    percentLayout->setSpacing(20);
    percentLayout->addWidget(percentValue,1,Qt::AlignCenter);

    myPushButton *switchToUserBtn = new myPushButton(myLan.user_mode, QIcon(), false, true);
    switchToUserBtn->setMinimumHeight(BTN_HEIGHT);
    percentLayout->addWidget(switchToUserBtn);
    switchToUserBtn->hide();
    connect(switchToUserBtn,SIGNAL(clicked()),this,SLOT(onSwitchBtnTest()));

    /*backAngle layout */
    QHBoxLayout *backAngleLayout = new QHBoxLayout;
    backAngleLayout->addWidget(backAngleLabel,0,Qt::AlignRight);
    backAngleLayout->addWidget(backAngleValue,0,Qt::AlignLeft);

    /*autoBkgValue layout */
//    QHBoxLayout *autoBkgValueLayoutParts[3];
//    for (int i = 0; i < 3; i++) {
//        autoBkgValueLayoutParts[i] = new QHBoxLayout;
//        autoBkgValueLayoutParts[i]->addWidget(bgRangeLabel[i],0,Qt::AlignCenter);
//        autoBkgValueLayoutParts[i]->setSpacing(20);
//        autoBkgValueLayoutParts[i]->addWidget(bgRangeValue[i],0,Qt::AlignCenter);
//    }
    QHBoxLayout *autoBgHBlayout = new QHBoxLayout;
    autoBgHBlayout->addWidget(m_autoThresholdLabel_RSC,0,Qt::AlignCenter);
    autoBgHBlayout->addWidget(m_autoThresholdValue_RSC,0,Qt::AlignLeft);

    /* main layout */
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *colorGroupLayout = new QHBoxLayout;
    QGridLayout *gridLayout = new QGridLayout;
    colorGroupLayout->setContentsMargins(30,0,30,0);
    colorGroupLayout->addLayout(thresholdLayout);
    colorGroupLayout->addStretch(1);
    colorGroupLayout->addLayout(colorLayout);
    colorGroupLayout->addStretch(1);
    colorGroupLayout->addLayout(percentLayout);

    QVBoxLayout *vLayoutParts[4];
    for (int i = 0; i < 4; i++) {
      vLayoutParts[i] = new QVBoxLayout;
    }

    vLayoutParts[0]->addStretch(1);
    vLayoutParts[0]->addWidget(m_autoBkgBox);
    vLayoutParts[0]->addWidget(frontBkLabel);
    vLayoutParts[0]->addWidget(rearBkLabel);

    vLayoutParts[1]->addWidget(bgRangeLabel[0]);
    vLayoutParts[1]->addWidget(bgRangeValue[0]);
    vLayoutParts[1]->addWidget(frontBkRangeValue[0]);
    vLayoutParts[1]->addWidget(rearBkRangeValue[0]);

    vLayoutParts[2]->addWidget(bgRangeLabel[1]);
    vLayoutParts[2]->addWidget(bgRangeValue[1]);
    vLayoutParts[2]->addWidget(frontBkRangeValue[1]);
    vLayoutParts[2]->addWidget(rearBkRangeValue[1]);

    vLayoutParts[3]->addWidget(bgRangeLabel[2]);
    vLayoutParts[3]->addWidget(bgRangeValue[2]);
    vLayoutParts[3]->addWidget(frontBkRangeValue[2]);
    vLayoutParts[3]->addWidget(rearBkRangeValue[2]);

    bgRangeValue[0]->setAlignment(Qt::AlignCenter);
    bgRangeValue[1]->setAlignment(Qt::AlignCenter);
    bgRangeValue[2]->setAlignment(Qt::AlignCenter);
    rearBkRangeValue[0]->setAlignment(Qt::AlignCenter);
    rearBkRangeValue[1]->setAlignment(Qt::AlignCenter);
    rearBkRangeValue[2]->setAlignment(Qt::AlignCenter);
    frontBkRangeValue[0]->setAlignment(Qt::AlignCenter);
    frontBkRangeValue[1]->setAlignment(Qt::AlignCenter);
    frontBkRangeValue[2]->setAlignment(Qt::AlignCenter);


//    gridLayout->setVerticalSpacing(15);
//    gridLayout->setHorizontalSpacing(20);
    gridLayout->setSpacing(5);


//    gridLayout->addLayout(colorLayout,0,1,1,2);
//    gridLayout->addLayout(thresholdLayout,1,0,1,1);
//    gridLayout->addLayout(percentLayout,1,1,1,1);

//    if (struCnfe.nProduct == OPT_PRODUCT_ANCOO_RDC || struCnfe.nProduct == OPT_PRODUCT_ANCOO_RC
//            || struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC) {
//        gridLayout->addLayout(backAngleLayout,1,2,1,1);
//        gridLayout->addWidget(m_autoBkgBtn,1,3,1,1);
//    }

    /* 自动背景布局*/

//    gridLayout->addLayout(autoBkgValueLayoutParts[0],0,1,1,1);
//    gridLayout->addLayout(autoBkgValueLayoutParts[1],0,2,1,1);
//    gridLayout->addLayout(autoBkgValueLayoutParts[2],0,3,1,1);
//    gridLayout->addWidget(m_autoBkgBox,0,0,1,1);


//    QHBoxLayout *frontBkLayoutParts[3];
//    for (int i = 0; i < 3; i++) {
//        frontBkLayoutParts[i] = new QHBoxLayout;
////        frontBkLayoutParts[i]->addStretch(1);
////        frontBkLayoutParts[i]->addWidget(frontBkRangeLabel[i],0,Qt::AlignCenter);
//        frontBkLayoutParts[i]->setSpacing(20);
//        frontBkLayoutParts[i]->addWidget(frontBkRangeValue[i],0,Qt::AlignCenter);
//    }

//    gridLayout->addLayout(frontBkLayoutParts[0],1,1,1,1);
//    gridLayout->addLayout(frontBkLayoutParts[1],1,2,1,1);
//    gridLayout->addLayout(frontBkLayoutParts[2],1,3,1,1);
//    gridLayout->addWidget(frontBkLabel,1,0,1,1);


//    QHBoxLayout *rearBkLayoutParts[3];
//    for (int i = 0; i < 3; i++) {
//        rearBkLayoutParts[i] = new QHBoxLayout;
//        rearBkLayoutParts[i]->addStretch(1);
////        rearBkLayoutParts[i]->addWidget(rearBkRangeLabel[i],0,Qt::AlignCenter);
//        rearBkLayoutParts[i]->setSpacing(20);
//        rearBkLayoutParts[i]->addWidget(rearBkRangeValue[i],0,Qt::AlignCenter);
//    }

//    gridLayout->addLayout(rearBkLayoutParts[0],2,1,1,1);
//    gridLayout->addLayout(rearBkLayoutParts[1],2,2,1,1);
//    gridLayout->addLayout(rearBkLayoutParts[2],2,3,1,1);
//    gridLayout->addWidget(rearBkLabel,2,0,1,1);

//    if (myFlow.getProductLineNo() == 0) {
//        gridLayout->addLayout(autoBkgValueLayoutParts[0],2,1,1,1);
//        gridLayout->addLayout(autoBkgValueLayoutParts[1],2,2,1,1);
//        gridLayout->addLayout(autoBkgValueLayoutParts[2],2,3,1,1);
//        gridLayout->addWidget(m_autoBkgBox,2,0,1,1);
//    }

//    if(struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC)
//    {
//        gridLayout->addLayout(autoBgHBlayout,2,2,1,1);
//        gridLayout->addWidget(m_autoBkgBox,2,3,1,1);
//        m_autoBkgBox->show();
//    }

    gridLayout->setContentsMargins(30,0,30,0);
    gridLayout->addLayout(vLayoutParts[0],0,0,1,1);
    gridLayout->addLayout(vLayoutParts[1],0,2,1,2, Qt::AlignRight);
    gridLayout->addLayout(vLayoutParts[2],0,4,1,2, Qt::AlignRight);
    gridLayout->addLayout(vLayoutParts[3],0,6,1,2, Qt::AlignRight);


    mainLayout->addWidget(myCurve,Qt::AlignLeft);
    mainLayout->addStretch();

    mainLayout->addLayout(colorGroupLayout);
//    mainLayout->addSpacing(0);
    mainLayout->addLayout(gridLayout, Qt::AlignCenter);
//    mainLayout->setContentsMargins(0,0,0,0);

//    mainLayout->addStretch();
    setLayout(mainLayout);
}

void PageBackground::onSwitchBtnTest()
{
    MySerial.fpgaUpdateWrite(COM_UNIT_UPDATE_USER, 2, struGsh.nUnit, 0, 0, 0, 1, struCnfp.nColorBoardMode, 3);
}
