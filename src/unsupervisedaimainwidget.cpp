#include "unsupervisedaimainwidget.h"
#include "unilog.h"

uchar   m_cOriImgBuf[CAP_HEI * CAP_WID * 3];
uchar   m_cSimImgBuf[CAP_HEI * CAP_WID * 3];

UnsupervisedAIMainWidget::UnsupervisedAIMainWidget(QWidget* parent) :
    QWidget(parent)
{
    m_nNumOfImg = 0;
    m_nLayer = 1;
    m_nChuteFeed = 15;
    m_nImgTotal = 6;
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setFixedSize(LCD_WIDTH, LCD_HEIGHT - LCD_TITLE_HEIGHT - LCD_STATUS_HEIGHT - 10);

    pageAIMain = new QWidget();
    m_stackedWidget->addWidget(pageAIMain);
    createAIMainWidge();

    pageSimMain = new QWidget();
    m_stackedWidget->addWidget(pageSimMain);
    createSimMainWidget();
}

void UnsupervisedAIMainWidget::updateAIMainWidget()
{
    if (struGsh.nAuthenticationLevel == AUTHENTICATION_LEVEL_OPERATOR)
    {
        m_AISimBtn->setEnabled(false);
    }
    else
    {
        m_AISimBtn->setEnabled(true);
    }
}

void UnsupervisedAIMainWidget::createAIMainWidge()
{
    m_AIStartBtn = new myPushButton(myLan.super_brain, myIcon.Action_OneKey);
    m_AISimBtn = new myPushButton(myIcon.Action_Configure, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH));
    m_AIBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);

    m_AIStartBtn->setMaximumSize(QSize(BTN_WIDTH + 50, BTN_HEIGHT + 50));
    //m_AISimBtn->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    m_AIBackBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QVBoxLayout* mainVbLay = new QVBoxLayout(pageAIMain);
    QHBoxLayout* upHbLay = new QHBoxLayout;
    upHbLay->addWidget(m_AIStartBtn);

    QHBoxLayout* downHbLay = new QHBoxLayout;
    downHbLay->addWidget(m_AISimBtn);
    downHbLay->addStretch();
    downHbLay->addWidget(m_AIBackBtn);

    mainVbLay->addStretch();
    mainVbLay->addLayout(upHbLay);
    mainVbLay->addStretch();
    mainVbLay->addLayout(downHbLay);

    connect(m_AIStartBtn, SIGNAL(pressed()), this, SLOT(onAIStartPressedBtnSlt()));
    connect(m_AISimBtn, SIGNAL(clicked()), this, SLOT(onAISimPressedBtnSlt()));
    connect(m_AIBackBtn, SIGNAL(pressed()), this, SLOT(onAIBackPressedBtnSlt()));
}

void UnsupervisedAIMainWidget::createSimMainWidget()
{
    m_bFlgSim = false;

    m_showImgLbe = new myLabel();
    m_imgSimBtn = new myPushButton(myLan.ai_simulate, myIcon.Sorter_RGB);
    m_simBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_simBackBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_imgScrollArea = new QScrollArea;
    m_imgScrollArea->setStyleSheet("height: 30");
    m_imgScrollArea->setBackgroundRole(QPalette::Dark);
    m_imgScrollArea->setWidget(m_showImgLbe);
    // m_imgScrollArea->setAlignment(Qt::AlignCenter);
    m_imgScrollArea->setWidgetResizable(true);
    m_imgSimBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    //! 参考组
    m_chuteGroup = new myGroupBox("");
    m_chuteLbe = new myLabel(myLan.reference_chute);
    m_minusBtn = new myPushButton(myIcon.Arrow_Left, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH), pageSimMain);
    m_lcdNum = new QLCDNumber(2);
    m_plusBtn = new myPushButton(myIcon.Arrow_Right, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH), pageSimMain);
    m_lcdNum->setFixedSize(SMALL_BTN_WIDTH + 20, BTN_HEIGHT);
    m_chuteGroup->setMinimumWidth(LCD_WIDTH / 2);
    m_lcdNum->display(struCnfg.nAutoReferChute);
    m_materPerLbe = new myLabel("物料含杂");
    m_materPerSetLne = new myLineEdit(QString("%1").arg(struCnfg.nMaterPer1));
    m_materPerSetLne->setMaximumWidth(BTN_WIDTH);
    m_materPerSetLne->setMaximumHeight(BTN_HEIGHT);
    m_feedSetLbe = new myLabel(myLan.current_feeder);
    m_feedLne = new myLineEdit(QString("%1").arg(struCnfg.nMaterPer2));
    m_feedLne->setMaximumWidth(BTN_WIDTH);
    m_feedLne->setMaximumHeight(BTN_HEIGHT);
    //m_materPerLbe->hide();
    //m_materPerSetLne->hide();
    m_imgAISensLbe = new myLabel("灵敏度缩放");
    m_imgAISensLne = new myLineEdit(QString("%1").arg(struCnfg.nMaterPer3 / 10.0));
    m_imgAISensLne->setMaximumHeight(BTN_HEIGHT);

    QGridLayout* chuteGridLay = new QGridLayout(m_chuteGroup);
    chuteGridLay->addWidget(m_chuteLbe, 0, 0, 1, 1);
    chuteGridLay->addWidget(m_minusBtn, 0, 1, 1, 1);
    chuteGridLay->addWidget(m_lcdNum, 0, 2, 1, 1);
    chuteGridLay->addWidget(m_plusBtn, 0, 3, 1, 1);
    chuteGridLay->addWidget(m_feedSetLbe, 1, 0, 1, 1);
    chuteGridLay->addWidget(m_feedLne, 1, 1, 1, 3);
    chuteGridLay->addWidget(m_materPerLbe, 2, 0, 1, 1);
    chuteGridLay->addWidget(m_materPerSetLne, 2, 1, 1, 3);
    chuteGridLay->addWidget(m_imgAISensLbe, 3, 0, 1, 1);
    chuteGridLay->addWidget(m_imgAISensLne, 3, 1, 1, 3);

    //! 图片列表
    m_imgListGbx = new myGroupBox("图片列表");
    m_imgListMinusBtn = new myPushButton(myIcon.Arrow_Left, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH), pageSimMain);
    m_imgListLcdNum = new QLCDNumber(2);
    m_imgListPlusBtn = new myPushButton(myIcon.Arrow_Right, QSize(SMALL_BTN_WIDTH, SMALL_BTN_WIDTH), pageSimMain);
    m_imgListLcdNum->setFixedSize(SMALL_BTN_WIDTH + 20, BTN_HEIGHT);
    m_imgListGbx->setMaximumWidth(200);
    m_imgListLcdNum->display(m_nNumOfImg + 1);
    m_imgListGbx->hide();
    m_imgListLcdNum->hide();
    m_imgListMinusBtn->hide();
    m_imgListPlusBtn->hide();

    QGridLayout* imgListGridLay = new QGridLayout(m_imgListGbx);
    imgListGridLay->addWidget(m_imgListMinusBtn, 0, 0, 1, 1);
    imgListGridLay->addWidget(m_imgListLcdNum, 0, 1, 1, 1);
    imgListGridLay->addWidget(m_imgListPlusBtn, 0, 2, 1, 1);
    imgListGridLay->addWidget(m_imgSimBtn, 1, 0, 1, 3);

    //! 页面布局
    QHBoxLayout* upHbLay = new QHBoxLayout;
    upHbLay->addStretch();
    upHbLay->addWidget(m_chuteGroup);
    upHbLay->addStretch();

    QHBoxLayout* downHbLay = new QHBoxLayout;
    downHbLay->addStretch();
    downHbLay->addWidget(m_simBackBtn);

    QVBoxLayout* mainVbLay = new QVBoxLayout(pageSimMain);
    mainVbLay->addLayout(upHbLay);
    mainVbLay->addLayout(downHbLay);

    //updateImage();

    connect(m_simBackBtn, SIGNAL(pressed()), this, SLOT(onSimBackPressedBtnSlt()));
    connect(m_plusBtn, SIGNAL(pressed()), this, SLOT(onPlusPressedBtnSlt()));
    connect(m_minusBtn, SIGNAL(pressed()), this, SLOT(onMinusPressedBtnSlt()));
    connect(m_feedLne, SIGNAL(pressed()), this, SLOT(onFeedPressedLneSlt()));
    connect(m_materPerSetLne, SIGNAL(pressed()), this, SLOT(onMaterPerSetLneSlt()));
    connect(m_imgListMinusBtn, SIGNAL(pressed()), this, SLOT(onImgListMinusBtnSlt()));
    connect(m_imgListPlusBtn, SIGNAL(pressed()), this, SLOT(onImgListPlusBtnSlt()));
    connect(m_imgSimBtn, SIGNAL(pressed()), this, SLOT(onImgSImPressedBtnSlt()));
    connect(m_imgAISensLne, SIGNAL(pressed()), this, SLOT(onImgAISensLneSlt()));
}

/*!
 * \brief 更新页面图片
 */
void UnsupervisedAIMainWidget::updateImage()
{
    QImage img(m_cSimImgBuf, CAP_WID, CAP_HEI, QImage::Format_RGB888);
    m_showImgLbe->setPixmap(QPixmap::fromImage(img));
    adjustScrollBar(m_imgScrollArea->horizontalScrollBar(), 1.0);
    adjustScrollBar(m_imgScrollArea->verticalScrollBar(), 1.0);
}

/*!
 * \brief 获取图像
 */
bool UnsupervisedAIMainWidget::getImage()
{
    bool retCode = true;

#ifdef Q_OS_UNIX

    int usbReturn;
    int bufSize = 6160 * CAP_HEI + 2240;
    uchar tmpColorBuf[CAP_WID * 3 * CAP_HEI];
    uchar tmpColorBuf1[CAP_WID * 3 * CAP_HEI];
    uchar tmpBuf[bufSize];
    char* pImageHead;
    uchar imageHead[5] = { 0xAA,0xAA,0xAA,0xAA,0 };
    bool bIsLeft = false;
    int tmpFrontUnit, tmpRearUnit;
    tmpFrontUnit = (struCnfg.nAutoReferChute - 1) * 2;
    tmpRearUnit = (struCnfg.nAutoReferChute - 1) * 2 + 1;


    if (myUsbDev != NULL)
    {
        usb.myusbFree();
    }
    usb.myusbInit();
    if (myUsbDev == NULL)
    {
        return false;
    }

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, tmpRearUnit, 0, 0, (CAP_HEI - 1) / 256, (CAP_HEI - 1) % 256, 1, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, tmpFrontUnit, 0, 0, 0, struGsh.nIntelSampBurst / 256, struGsh.nIntelSampBurst % 256, 1);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 0, struGsh.nIntelSampBurst / 256, struGsh.nIntelSampBurst % 256, 1);
    MySerial.com1Write(2, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, tmpFrontUnit, 0, 0, 1, (CAP_HEI - 1) / 256, (CAP_HEI - 1) % 256, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 1, (CAP_HEI - 1) / 256, (CAP_HEI - 1) % 256, 1);

    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, tmpFrontUnit, 0, 0, 0, 0, 0, 1); // 主相机板传输图像

    for (int n = 0; n < 2; n++)
    {
        bzero(tmpBuf, bufSize);
        usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, (char*)tmpBuf, bufSize, 20 * 1000);
        pImageHead = (char*)tmpBuf;
        qDebug("usbReturn = %d", usbReturn);
        if (usbReturn == bufSize)
        {
            for (int i = 0; i < CAP_HEI; i++)
            {
                /* 判断包头数据是否正确 */
                if ((pImageHead = getHead((char*)pImageHead, 6160 * 3, (char*)imageHead)) != NULL)
                {
                    memcpy(tmpColorBuf + CAP_WID * 3 * (CAP_HEI - 1 - i), pImageHead, CAP_WID * 3);

                    memcpy(tmpColorBuf1 + CAP_WID * 3 * (CAP_HEI - 1 - i), pImageHead + CAP_WID * 3, CAP_WID * 3);
                }
                else
                {
                    retCode = false;
                    return retCode;
                }
            }
            MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
        }
        else
        {
            retCode = false;
            return retCode;
        }
        QString colorImageName;

        /* 将图像数据保存成位图文件 */
        colorImageName = QString("%1.png").arg(4 * m_nNumOfImg + n);
        qDebug() << colorImageName;
        saveBMPFile(colorImageName, CAP_WID, CAP_HEI, (uchar*)tmpColorBuf);
        /* 将图像数据保存成位图文件 */
        colorImageName = QString("%1.png").arg(4 * m_nNumOfImg + n + 2);
        qDebug() << colorImageName;
        saveBMPFile(colorImageName, CAP_WID, CAP_HEI, (uchar*)tmpColorBuf1);

        MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 0, 0, 0, 1); // 上层相机板传输图像
    }
#endif
    return retCode;
}
/*!
 * \brief 检测包头
 */
char* UnsupervisedAIMainWidget::getHead(const char* mem, int size, const char* str)
{
    long lmem = size;
    char* cp = NULL;
    char* s1 = NULL;
    char* s2 = NULL;

    cp = (char*)mem;
    if ((mem == NULL) || (str == NULL) || (size <= 0))
    {
        return NULL;
    }

    if (!str)
    {
        return ((char*)mem);
    }
    while (lmem > 0)
    {
        s1 = cp;
        s2 = (char*)str;
        while (*s1 && s2 && !(*s1 - *s2))
        {
            s1++;
            s2++;
        }
        if (!*s2)
        {
            return cp + 10;
        }
        cp++;
        lmem--;
    }
    return NULL;
}
/*!
 * \brief 无监督程序
 */
bool UnsupervisedAIMainWidget::unsupervisedLearning()
{
    int nFlagImg = 1;
    int tmpFrontUnit, tmpRearUnit, tmpUnit;
    tmpFrontUnit = struCnfg.nAutoReferChute * 2;
    tmpRearUnit = struCnfg.nAutoReferChute * 2 + 1;
    //    tmpUnit = struGsh.nUnit;
    QFile file;
    int maxQuo = 0;

    //! 智能
    m_nLayer = 1;
    struGsh.nUnit = tmpRearUnit;
    if (struCnfp.nArithmeticEnable[ARITH_INTEL_A] && (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_DIFF_POSITIVE_SORT || struCnfg.nProfileMode[struCnfg.nProfile] == RSC_YELLOW_WHITE_SORT))
    {
        saveAIConfig(struCnfg.nMaterPer1, m_nImgTotal, nFlagImg, 1, struCnfg.nReservedSens);

        //! 防止屏保
        struGsh.nBacklightCounter = 0;
        //#ifdef Q_OS_UNIX
        //        char cmd[64];
        //        sprintf(cmd,"./myAISample");
        //        system(cmd);
        //#endif
        //#ifdef Q_OS_WIN32
        //        QProcess p(0);
        //        p.start("myAISample.exe");
        //        p.waitForFinished();
        //#endif

        file.setFileName(QString("/tmp/train%1.result").arg(0));
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug("result file open error!");
            return false;
        }

        QTextStream io(&file);

        maxQuo = 3;

        memset(m_struClassQuo, 0, MAX_VIEW_LOCATION * sizeof(struct classQuo));
        for (int i = 0; i < maxQuo; i++)
        {
            io >> m_struClassQuo[m_nLayer].nQuo[i];
            qDebug("nQuo[%d] = %f", i, m_struClassQuo[m_nLayer].nQuo[i]);
        }
        io >> m_struClassQuo[m_nLayer].nCons;     //cons*/
        qDebug("nCons = %f\n", m_struClassQuo[m_nLayer].nCons);
        file.close();
        file.remove(QString("/tmp/train%1.result").arg(0));
        file.remove(QString("/tmp/train%1").arg(0));
        file.remove(QString("/tmp/train%1.range").arg(0));

        //! 智能参数判断
        if (m_struClassQuo[m_nLayer].nQuo[0] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[1] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[2] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[3] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[4] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[5] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[6] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[7] == 0 &&
            m_struClassQuo[m_nLayer].nQuo[8] == 0)
        {
            return false;
        }

        //! 后视全部相同
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
            {
                if (j % 2 == 1)
                {
                    getGlobalAIParam(ARITH_INTEL_A, struGsh.nLevel, j, 0);
                }
            }
        }
    }

    for (m_nLayer = 0; m_nLayer < 2; m_nLayer++)
    {
        if (m_nLayer)
        {
            struGsh.nUnit = tmpRearUnit;
        }
        else
        {
            struGsh.nUnit = tmpFrontUnit;
        }
        saveAIConfig(struCnfg.nMaterPer1, m_nImgTotal, nFlagImg, 2, struCnfg.nReservedSens);

        //! 防止屏保
        struGsh.nBacklightCounter = 0;
        //#ifdef Q_OS_UNIX
        //		char cmd[64];
        //		sprintf(cmd,"./myAISample");
        //		system(cmd);
        //#endif
        //#ifdef Q_OS_WIN32
        //		QProcess p(0);
        //		p.start("myAISample.exe");
        //		p.waitForFinished();
        //#endif

        file.setFileName(QString("/tmp/tradition.result"));
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug("result file open error!");
            return false;
        }

        QTextStream io1(&file);

        for (int i = 0; i < 24; i++)
        {
            io1 >> m_nSens[m_nLayer][i];
            qDebug("nSens[%d] = %d", i, m_nSens[m_nLayer][i]);
        }
        file.close();
        file.remove(QString("/tmp/tradition.result"));

        int nSens = 0;
        if (m_nLayer)
        {
            //! 后视全部相同
            for (int i = 0; i < struCnfg.nLevelTotal; i++)
            {
                for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
                {
                    if (j % 2 == 1)
                    {
                        //! 异色粒正选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_DIFF_POSITIVE_SORT)
                        {
                            //! 选病斑
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 选黄
                            if (struCnfp.nArithmeticEnable[ARITH_DISCOLOR_A])
                            {
                                getGlobalAIParam(ARITH_DISCOLOR_A, struGsh.nLevel, j, ARITH_DISCOLOR_A);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens - 100;
                                }
                            }
                            else
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens - 100;
                                }
                            }
                        }

                        //! 异色粒反选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_DIFF_NEGATIVE_SORT)
                        {
                            //! 选透
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                }
                            }
                            else
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                }
                            }
                        }

                        //! 乳白粒正选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_WHITE_POSITIVE_SORT)
                        {
                            //! 选全白
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 选腹白
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 灵敏度修正	
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nMode)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                                {
                                    if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                    }
                                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin + 120 > struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin) || (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin > 200))
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin = struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin - 100;
                                    }
                                }
                                else
                                {
                                    if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                    }
                                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax + 120 > struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax) || (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax > 200))
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax = struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax - 100;
                                    }
                                }
                            }
                        }

                        //! 乳白粒反选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_WHITE_NEGATIVE_SORT)
                        {
                            //! 选透
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                }
                            }
                            else
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                }
                            }
                        }

                        //! 黄白同选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_YELLOW_WHITE_SORT)
                        {
                            //! 病斑
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens - 100;
                                }
                            }
                            else
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens - 100;
                                }
                            }
                            //! 选白
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                }
                            }
                            else
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                }
                            }
                            //! 选黄
                            if (struCnfp.nArithmeticEnable[ARITH_DISCOLOR_A])
                            {
                                getGlobalAIParam(ARITH_DISCOLOR_A, struGsh.nLevel, j, ARITH_DISCOLOR_A);
                            }
                        }

                        //! 黄透同选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_YELLOW_TRANS_SORT)
                        {
                            //! 选透
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                }
                            }
                            else
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                }
                            }
                            //! 选病斑
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens - 100;
                                }
                            }
                            else
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens - 100;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            //! 前视全部相同
            for (int i = 0; i < struCnfg.nLevelTotal; i++)
            {
                for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
                {
                    if (j % 2 == 0)
                    {
                        //! 异色粒正选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_DIFF_POSITIVE_SORT)
                        {
                            //! 选病斑
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 选黄
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nMode)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                                {
                                    if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin < nSens)
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin = nSens;
                                    }
                                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin + 120 > struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin) || (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin > 200))
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin - 100;
                                    }
                                }
                                else
                                {
                                    if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax < nSens)
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax = nSens;
                                    }
                                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax + 120 > struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax) || (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax > 200))
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax - 100;
                                    }
                                }
                            }
                        }

                        //! 异色粒反选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_DIFF_NEGATIVE_SORT)
                        {
                            //! 选透
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                }
                            }
                            else
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                }
                            }
                        }

                        //! 乳白粒正选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_WHITE_POSITIVE_SORT)
                        {
                            //! 选全白
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 选腹白
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 灵敏度修正	
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nMode)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                                {
                                    if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                    }
                                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin + 120 > struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin) || (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin > 200))
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMin = struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin - 100;
                                    }
                                }
                                else
                                {
                                    if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                    }
                                    if ((struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax + 120 > struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax) || (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax > 200))
                                    {
                                        struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_B].nSensMax = struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax - 100;
                                    }
                                }
                            }
                        }

                        //! 乳白粒反选
//                            if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_WHITE_NEGATIVE_SORT){
//                                //! 选透
//                                if (struCnfp.nArithmeticEnable[ARITH_GREY_A]) {
//                                    getGlobalAIParam(ARITH_GREY_A,struGsh.nLevel,j,ARITH_GREY_B);
//                                }
//                            }

                            //! 黄白同选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_YELLOW_WHITE_SORT)
                        {
                            //! 病斑
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens - 100;
                                }
                            }
                            else
                            {
                                if (nSens > 200)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens - 100;
                                }
                            }
                            //! 选白
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                nSens = getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 灵敏度修正
                            if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nMode == 0)
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMin = nSens;
                                }
                            }
                            else
                            {
                                if (struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax < nSens)
                                {
                                    struCnfp.struGroupIdentify[i][j].struGreyColor[ARITH_GREY_A].nSensMax = nSens;
                                }
                            }
                        }

                        //! 黄透同选
                        if (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_YELLOW_TRANS_SORT)
                        {
                            //! 选黄
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_A])
                            {
                                getGlobalAIParam(ARITH_GREY_A, struGsh.nLevel, j, ARITH_GREY_B);
                            }
                            //! 选病斑
                            if (struCnfp.nArithmeticEnable[ARITH_GREY_B])
                            {
                                getGlobalAIParam(ARITH_GREY_B, struGsh.nLevel, j, ARITH_GREY_A);
                            }
                        }
                    }
                }
            }
        }
    }
    //    struGsh.nUnit = tmpUnit;
    return true;
}

int UnsupervisedAIMainWidget::IsImageOk(int nUnit, int nImgNum)
{
    bool tmpLoad = true;
    int nImageData = 0;
    QImage image;
    QColor rgb;
    int tmpImgNum;
    struGsh.nUnit = nUnit;
    //! 读取图像
    for (int n = 0; n < nImgNum;n++)
    {
        if (struGsh.nUnit % 2)
        {
            tmpImgNum = 2 * n + 1;
        }
        else
        {
            tmpImgNum = 2 * n;
        }

        tmpLoad = image.load(QString("/tmp/%1.png").arg(tmpImgNum));

        if (!tmpLoad)
            return 0;

        int i = 0;
        for (; i < CAP_HEI; i++)
        {
            for (int j = 0; j < CAP_WID; j++)
            {
                rgb = image.pixel(j, i);

                if (myAIShare.pixelIsBackgroundcomm((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue(), j))
                {
                    continue;
                }

                if (pixelIsEdgeImage(&image, j, i))
                {
                    continue;
                }
                nImageData++;
            }
        }
    }

    qDebug() << nImageData;
    //    if (nImageData < 2000){
    //        return false;
    //    }
    return nImageData;
}

bool UnsupervisedAIMainWidget::saveAIConfig(int nMaterPer, int nImgNum, int nFlgView, int nResMode, int nResSens)
{
    QFile file("/tmp/config");
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug("Save Config : File open error");
        return false;
    }
    QTextStream io(&file);

    QString strConfig = QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8\n%9\n%10\n%11\n%12\n%13\n")
        .arg(struCnfp.nBackgroundColor[struGsh.nLevel][struGsh.nUnit % 2])
        .arg(struCnfp.nBackgroundColorValue[struGsh.nLevel][struGsh.nUnit % 2])
        .arg(struCnfp.nBackgroundColorPer[struGsh.nLevel][struGsh.nUnit % 2])
        .arg(nMaterPer)
        .arg(nImgNum)
        .arg(nFlgView)
        .arg(struGsh.nUnit)
        .arg(nResSens)
        .arg(nResMode)
        .arg(2)
        .arg(nResMode)
        .arg(45)
        .arg(0);

    io << strConfig;
    file.close();

    return true;
}

int UnsupervisedAIMainWidget::getGlobalAIParam(int classMode, int levelNum, int groupNum, int indexOfTrain)
{
    int nSens = 0;
    if (classMode == ARITH_INTEL_A)
    {
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[0] = (m_struClassQuo[m_nLayer].nCons >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[1] = abs(m_struClassQuo[m_nLayer].nCons);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[0] = (m_struClassQuo[m_nLayer].nQuo[0] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[1] = abs(m_struClassQuo[m_nLayer].nQuo[0]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[0] = (m_struClassQuo[m_nLayer].nQuo[1] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[1] = abs(m_struClassQuo[m_nLayer].nQuo[1]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[0] = (m_struClassQuo[m_nLayer].nQuo[2] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[1] = abs(m_struClassQuo[m_nLayer].nQuo[2]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[0] = (m_struClassQuo[m_nLayer].nQuo[3] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[1] = abs(m_struClassQuo[m_nLayer].nQuo[3]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[0] = (m_struClassQuo[m_nLayer].nQuo[4] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[1] = abs(m_struClassQuo[m_nLayer].nQuo[4]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[0] = (m_struClassQuo[m_nLayer].nQuo[5] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[1] = abs(m_struClassQuo[m_nLayer].nQuo[5]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[0] = (m_struClassQuo[m_nLayer].nQuo[6] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[1] = abs(m_struClassQuo[m_nLayer].nQuo[6]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[0] = (m_struClassQuo[m_nLayer].nQuo[7] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[1] = abs(m_struClassQuo[m_nLayer].nQuo[7]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[0] = (m_struClassQuo[m_nLayer].nQuo[8] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[1] = abs(m_struClassQuo[m_nLayer].nQuo[8]);

        qDebug("%d-%d", levelNum, groupNum);
        qDebug("cons-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[1]);
        qDebug("R-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[1]);
        qDebug("G-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[1]);
        qDebug("B-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[1]);
        qDebug("RG-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[1]);
        qDebug("RB-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[1]);
        qDebug("GB-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[1]);
        qDebug("RR-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[1]);
        qDebug("GG-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[1]);
        qDebug("BB-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[1]);
    }

    if (classMode == ARITH_GREY_A || classMode == ARITH_GREY_B)
    {
        if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nColor == 1)
        {
            if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nMode == 0)
            {
                nSens = (int)((m_nSens[m_nLayer][indexOfTrain * 6] * 1000.0) / 255 + 0.5);
                if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin > (int)((m_nSens[m_nLayer][indexOfTrain * 6] * 1000.0) / 255 + 0.5))
                {
                    struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin = (int)((m_nSens[m_nLayer][indexOfTrain * 6] * 1000.0) / 255 + 0.5);
                }
            }
            else
            {
                nSens = (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 1]) * 1000.0) / 255 + 0.5);
                if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax > (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 1]) * 1000.0) / 255 + 0.5))
                {
                    struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax = (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 1]) * 1000.0) / 255 + 0.5);
                }
            }
        }

        if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nColor == 2)
        {
            if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nMode == 0)
            {
                nSens = (int)((m_nSens[m_nLayer][indexOfTrain * 6 + 2] * 1000.0) / 255 + 0.5);
                if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin > (int)((m_nSens[m_nLayer][indexOfTrain * 6 + 2] * 1000.0) / 255 + 0.5))
                {
                    struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin = (int)((m_nSens[m_nLayer][indexOfTrain * 6 + 2] * 1000.0) / 255 + 0.5);
                }
            }
            else
            {
                nSens = (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 3]) * 1000.0) / 255 + 0.5);
                if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax > (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 3]) * 1000.0) / 255 + 0.5))
                {
                    struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax = (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 3]) * 1000.0) / 255 + 0.5);
                }
            }
        }

        if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nColor == 4)
        {
            if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nMode == 0)
            {
                nSens = (int)((m_nSens[m_nLayer][indexOfTrain * 6 + 4] * 1000.0) / 255 + 0.5);
                if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin > (int)((m_nSens[m_nLayer][indexOfTrain * 6 + 4] * 1000.0) / 255 + 0.5))
                {
                    struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin = (int)((m_nSens[m_nLayer][indexOfTrain * 6 + 4] * 1000.0) / 255 + 0.5);
                }
            }
            else
            {
                nSens = (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 5]) * 1000.0) / 255 + 0.5);
                if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax > (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 5]) * 1000.0) / 255 + 0.5))
                {
                    struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax = (int)(((255 - m_nSens[m_nLayer][indexOfTrain * 6 + 5]) * 1000.0) / 255 + 0.5);
                }
            }
        }
    }

    if (classMode == ARITH_DISCOLOR_A || classMode == ARITH_DISCOLOR_B)
    {
        if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nDiscolor == 0)
        {
            if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nMode == 0)
            {
                struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin = m_nSens[m_nLayer][indexOfTrain * 6] * 10;
                nSens = m_nSens[m_nLayer][indexOfTrain * 6] * 10;
            }
            else
            {
                struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax = (1000 - m_nSens[m_nLayer][indexOfTrain * 6 + 1]) * 10;
                nSens = (1000 - m_nSens[m_nLayer][indexOfTrain * 6 + 1]) * 10;
            }
        }

        if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nDiscolor == 1)
        {
            if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nMode == 0)
            {
                struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin = m_nSens[m_nLayer][indexOfTrain * 6 + 2] * 10;
                nSens = m_nSens[m_nLayer][indexOfTrain * 6 + 2] * 10;
            }
            else
            {
                struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax = (1000 - m_nSens[m_nLayer][indexOfTrain * 6 + 3]) * 10;
                nSens = (1000 - m_nSens[m_nLayer][indexOfTrain * 6 + 3]) * 10;
            }
        }

        if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nDiscolor == 2)
        {
            if (struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nMode == 0)
            {
                struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin = m_nSens[m_nLayer][indexOfTrain * 6 + 4] * 10;
                nSens = m_nSens[m_nLayer][indexOfTrain * 6 + 4] * 10;
            }
            else
            {
                struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax = (1000 - m_nSens[m_nLayer][indexOfTrain * 6 + 5]) * 10;
                nSens = (1000 - m_nSens[m_nLayer][indexOfTrain * 6 + 5]) * 10;
            }
        }

    }
    qDebug("AG:%d,L:%d-G:%d-Min:%d-Max:%d-S:%d", classMode, levelNum, groupNum, struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMin, struCnfp.struGroupIdentify[levelNum][groupNum].struGreyColor[classMode].nSensMax, nSens);
    return nSens;
}

bool UnsupervisedAIMainWidget::pixelIsEdgeImage(QImage* image, int x, int y)
{
    QColor rgb;
    for (int m = -1; m <= 1; m++)
    {
        for (int n = -1; n <= 1; n++)
        {
            if (m != 0 || n != 0)
            {
                if (x + m >= 0 && x + m < CAP_WID && y + n >= 0 && y + n < CAP_HEI)
                {
                    rgb = image->pixel(x + m, y + n);
                    //已在上层调用方法中更新aishare.m_backMax,Min
                    if (myAIShare.pixelIsBackgroundcomm((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue(), (x + m)))
                        return true;
                }
            }
        }
    }
    return false;
}

/*!
 * \brief 仿真
 * \param r
 * \param g
 * \param b
 * \return
 */
bool UnsupervisedAIMainWidget::pixelIsBad(int r, int g, int b)
{
    uint rg = r * g;
    uint rb = r * b;
    uint gb = g * b;
    uint rr = r * r;
    uint gg = g * g;
    uint bb = b * b;

    int	indexOfArithIntel = struGsh.curIntelArithNo;

    int nCons, nQuo0, nQuo1, nQuo2, nQuo3;
    int nQuo4, nQuo5, nQuo6, nQuo7, nQuo8;
    int groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
    int nSens = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nSens;
    int nConsAdj;
    if (abs(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]) < 200000)
    {
        nConsAdj = 200000;
    }
    else
    {
        nConsAdj = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1];
    }
    // 常数项
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nModeDfl == 0)
    {    // 正选
        if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[0] == 0)
        {
            if (nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] - nConsAdj * (nSens - 100) / 100.0;
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] + nConsAdj * (100 - nSens) / 100.0;
        }
        else
        {
            if (nSens > 100)
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] + nConsAdj * (nSens - 100) / 100.0);
            else
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] - nConsAdj * (100 - nSens) / 100.0);
        }
    }
    else
    {       // 反选
        if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[0] == 0)
        {
            if (nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] + nConsAdj * (nSens - 100) / 100.0;
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] - nConsAdj * (100 - nSens) / 100.0;
        }
        else
        {
            if (nSens > 100)
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] - nConsAdj * (nSens - 100) / 100.0);
            else
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1] + nConsAdj * (100 - nSens) / 100.0);
        }
    }

    // 系数1
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoR[0] == 0)
        nQuo0 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoR[1];
    else
        nQuo0 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoR[1];

    // 系数2
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoG[0] == 0)
        nQuo1 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoG[1];
    else
        nQuo1 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoG[1];

    // 系数3
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoB[0] == 0)
        nQuo2 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoB[1];
    else
        nQuo2 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoB[1];

    // 系数4
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRG[0] == 0)
        nQuo3 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRG[1];
    else
        nQuo3 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRG[1];

    // 系数5
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRB[0] == 0)
        nQuo4 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRB[1];
    else
        nQuo4 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRB[1];

    // 系数6
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGB[0] == 0)
        nQuo5 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGB[1];
    else
        nQuo5 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGB[1];

    // 系数7
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRR[0] == 0)
        nQuo6 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRR[1];
    else
        nQuo6 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRR[1];

    // 系数8
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGG[0] == 0)
        nQuo7 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGG[1];
    else
        nQuo7 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGG[1];

    // 系数9
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoBB[0] == 0)
        nQuo8 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoBB[1];
    else
        nQuo8 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoBB[1];

    int value = nCons + nQuo0 * r + nQuo1 * g + nQuo2 * b + nQuo3 * rg + nQuo4 * rb + nQuo5 * gb + nQuo6 * rr + nQuo7 * gg + nQuo8 * bb;
    if (struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nModeDfl == 0)
    {     // 正选
        if (value < 0)
            return true;
        else
            return false;
    }
    else
    {       // 反选
        if (value > 0)
            return true;
        else
            return false;
    }
}

/*!
 * \brief UnsupervisedAIMainWidget::getUnitGroup
 * \param level
 * \param unit
 * \return
 */
int UnsupervisedAIMainWidget::getUnitGroup(int level, int unit)
{
    int group = -1;

    for (int i = 0; i < MAX_GROUP_IDTNTIFY; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitCount; j++)
        {
            if (unit == struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitId[j])
            {
                group = i;
            }
        }
    }

    return group;
}
/*!
 * \brief UnsupervisedAIMainWidget::saveBMPFile
 * \param name
 * \param width
 * \param height
 * \param data
 */
void UnsupervisedAIMainWidget::saveBMPFile(QString name, int width, int height, uchar* data)
{
    QImage img(data, width, height, QImage::Format_RGB888);
    img.save(QString("/tmp/%1").arg(name), "PNG", 100);
}

/*!
 * \brief UnsupervisedAIMainWidget::adjustScrollBar
 * \param scrollBar
 * \param factor
 */
void UnsupervisedAIMainWidget::adjustScrollBar(QScrollBar* scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}
/*!
 * \brief 返回主界面槽函数
 */
void UnsupervisedAIMainWidget::onSimBackPressedBtnSlt()
{
    this->m_stackedWidget->setCurrentIndex(SM_AI_MAIN_PAGE);
}

/*!
 * \brief 仿真
 */
void UnsupervisedAIMainWidget::onAISimPressedBtnSlt()
{
    this->m_stackedWidget->setCurrentIndex(SM_AI_SIM_PAGE);
    m_bFlgSim = false;
    updateInitImgSlt();
}
/*!
 * \brief UnsupervisedAIMainWidget::onImgSImPressedBtnSlt
 */
void UnsupervisedAIMainWidget::onImgSImPressedBtnSlt()
{
    m_bFlgSim = !m_bFlgSim;
    updateInitImgSlt();
}

/*!
 * \brief UnsupervisedAIMainWidget::onPlusPressedBtnSlt
 */
void UnsupervisedAIMainWidget::onPlusPressedBtnSlt()
{
    if (struCnfg.nAutoReferChute < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal / 2)
    {
        struCnfg.nAutoReferChute++;
        m_lcdNum->display(struCnfg.nAutoReferChute);
    }
}

/*!
 * \brief UnsupervisedAIMainWidget::onMinusPressedBtnSlt
 */
void UnsupervisedAIMainWidget::onMinusPressedBtnSlt()
{
    if (struCnfg.nAutoReferChute > 1)
    {
        struCnfg.nAutoReferChute--;
        m_lcdNum->display(struCnfg.nAutoReferChute);
    }
}

/*!
 * \brief UnsupervisedAIMainWidget::onFeedPressedLneSlt
 */
void UnsupervisedAIMainWidget::onFeedPressedLneSlt()
{
    myInputPanel inputDlg(intType, 0, 100, struCnfg.nMaterPer2);
    inputDlg.exec();
    struCnfg.nMaterPer2 = (int)inputDlg.getValue();
    m_feedLne->setText(QString("%1").arg(struCnfg.nMaterPer2));
}
/*!
 * \brief UnsupervisedAIMainWidget::onMaterPerSetLneSlt
 */
void UnsupervisedAIMainWidget::onMaterPerSetLneSlt()
{
    myInputPanel inputDlg(intType, 0, 100, struCnfg.nMaterPer1);
    inputDlg.exec();
    struCnfg.nMaterPer1 = (int)inputDlg.getValue();
    m_materPerSetLne->setText(QString("%1").arg(struCnfg.nMaterPer1));
}
/*!
 * \brief UnsupervisedAIMainWidget::onImgListPlusBtnSlt
 */
void UnsupervisedAIMainWidget::onImgListPlusBtnSlt()
{
    if (m_nNumOfImg < m_nImgTotal - 1)
    {
        m_nNumOfImg++;
        m_imgListLcdNum->display(m_nNumOfImg + 1);
        updateInitImgSlt();
    }
}
/*!
 * \brief UnsupervisedAIMainWidget::onImgListMinusBtnSlt
 */
void UnsupervisedAIMainWidget::onImgListMinusBtnSlt()
{
    if (m_nNumOfImg > 0)
    {
        m_nNumOfImg--;
        m_imgListLcdNum->display(m_nNumOfImg + 1);
        updateInitImgSlt();
    }
}
/*!
 * \brief UnsupervisedAIMainWidget::onImgAISensLneSlt
 */
void UnsupervisedAIMainWidget::onImgAISensLneSlt()
{
    float nVal = struCnfg.nMaterPer3 / 10.0;

    myInputPanel* inputPanel = new myInputPanel(floatType, 0.5, 5, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal < 0.5)
        {
            nVal = 0.5;
        }
        struCnfg.nMaterPer3 = nVal * 10;
        m_imgAISensLne->setText(QString("%1").arg(struCnfg.nMaterPer3 / 10.0));
    }
}

/*!
 * \brief 开始无监督智能学习
 */
void UnsupervisedAIMainWidget::onAIStartPressedBtnSlt()
{
    int nFeederEn[10];
    int feederTemp[10];
    int nAlarmTmp[16];
    int bStatFeed = struGsh.bStatFeed;

    //! 增加清灰
    struGsh.bFlagManuwipe = 1;
    myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration + 1);

    //! 复合清灰模式
    if (struCnfg.nWipeMode)
    {
        myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration2);
    }

    //! 屏蔽报警
    for (int i = 0; i < 16;i++)
    {
        nAlarmTmp[i] = struCnfg.nAlarmEnable[i];
        struCnfg.nAlarmEnable[i] = 0;
    }

    //! 背景调节中...
    infoWidget->setLabelText(myLan.msg_initing_autoSig);
    infoWidget->delayShow();
    resetBkgLampIndex();
    adjustBackground(ONE_LEVEL, true);
    adjustBackground(ONE_LEVEL, false);
    //setBkgValueAuto(false);
    infoWidget->hide();

    //! 供料打开
    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();

    //! 设置供料量
    int feedEnd;
    feedEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[0].nUnitCount / 2 - 1;
    for (int i = 0; i < 10; i++)
    {
        feederTemp[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
        nFeederEn[i] = struCnfp.struGroupCtrl[0].nFeederEnable[i];
    }

    if (struCnfg.nAutoFeederSet == 1)
    {
        struCnfp.struGroupCtrl[0].nFeeder[struCnfg.nAutoReferChute - 1] = struCnfg.nMaterPer2 * 10;
    }
    struCnfp.struGroupCtrl[0].nFeederEnable[struCnfg.nAutoReferChute - 1] = 1;
    myFlow.resetFeeder(0);

    for (int i = 0; i < 10; i++)
    {
        LOG_INFO_STM("idx:" << i << " Feeder:" << struCnfp.struGroupCtrl[0].nFeeder[i]
            << ", Status:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
    }

    struGsh.nUnit = (struCnfg.nAutoReferChute - 1) * 2 + 1;
    if (bStatFeed != 1)
    {
        myFlow.onOff();
        myFlow.sleep(8);
    }

    //! 若吹气频率线程开启则关闭
    bool bIsRunning = false;
    if (g_ejectTimesThread->isRunning())
    {
        g_ejectTimesThread->stopStatRun();
        bIsRunning = true;
        myFlow.sleep(5);
    }

    m_nNumOfImg = 0;
    bool bGetImageIsOk = false;
    for (m_nNumOfImg = 0; m_nNumOfImg < m_nImgTotal / 2; m_nNumOfImg++)
    {
        bGetImageIsOk = false;
        bGetImageIsOk = getImage();
        if (!bGetImageIsOk)
        {
            break;
        }
    }

    //m_nNumOfImg = 0;

    for (int i = 0; i < 10; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = feederTemp[i];
        struCnfp.struGroupCtrl[0].nFeederEnable[i] = nFeederEn[i];
    }

    //! 图片采集出错
    if (!bGetImageIsOk)
    {
        //! 供料关闭
        if (bStatFeed != 1)
        {
            myFlow.onOff();
        }
        else
        {
            //! 恢复吹气频率线程状态
            if (bIsRunning)
            {
                g_ejectTimesThread->start();
            }
        }
        infoWidget->setLabelText(myLan.ai_image_capture_error);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

    //! 物料点过多
    if ((IsImageOk(0, 1) > IMAGE_HEIGHT * IMAGE_WIDTH * 0.8) || (IsImageOk(1, 1) > IMAGE_HEIGHT * IMAGE_WIDTH * 0.8))
    {
        //! 供料关闭
        if (bStatFeed != 1)
        {
            myFlow.onOff();
        }
        else
        {
            //! 恢复吹气频率线程状态
            if (bIsRunning)
            {
                g_ejectTimesThread->start();
            }
        }
        infoWidget->setLabelText(myLan.ai_unsupervised_more_point);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

    //! 物料点过少
    if ((IsImageOk(0, m_nImgTotal) < 2000) || (IsImageOk(1, m_nImgTotal) < 2000))
    {
        //! 供料关闭
        if (bStatFeed != 1)
        {
            myFlow.onOff();
        }
        else
        {
            //! 恢复吹气频率线程状态
            if (bIsRunning)
            {
                g_ejectTimesThread->start();
            }
        }
        infoWidget->setLabelText(myLan.ai_unsupervised_less_point);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    infoWidget->hide();

    //! 防止屏保
    struGsh.nBacklightCounter = 0;

    //! 主循环
    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();
    bool bResult = unsupervisedLearning();

    if (!bResult)
    {
        //! 供料关闭
        if (bStatFeed != 1)
        {
            myFlow.onOff();
        }
        else
        {
            //! 恢复吹气频率线程状态
            if (bIsRunning)
            {
                g_ejectTimesThread->start();
            }
        }
        infoWidget->setLabelText(myLan.msg_auto_computer_erro);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    //! 供料关闭
    if (bStatFeed != 1)
    {
        myFlow.onOff();
    }
    else
    {
        myFlow.resetSortParams();
        //! 恢复吹气频率线程状态
        if (bIsRunning)
        {
            g_ejectTimesThread->start();
        }
    }
    infoWidget->hide();
}

/*!
 * \brief 第一次进入主界面程序，更新图片
 */
void UnsupervisedAIMainWidget::updateInitImgSlt()
{
    struGsh.nUnit = (struCnfg.nAutoReferChute - 1) * 2 + 1;

    memset(m_cOriImgBuf, 255, CAP_WID * CAP_HEI * 3);
    memset(m_cSimImgBuf, 255, CAP_WID * CAP_HEI * 3);
    QImage image(QString("/tmp/%1.png").arg(m_nNumOfImg));

    if (image.isNull())
    {
        updateImage();
        return;
    }

    int oriIndex;
    QColor rgb;

    for (int i = 0; i < CAP_HEI; i++)
    {
        for (int j = 0; j < CAP_WID; j++)
        {
            rgb = image.pixel(j, i);
            oriIndex = (i * CAP_WID + j) * 3;

            m_cOriImgBuf[oriIndex] = (uchar)rgb.red();
            m_cOriImgBuf[oriIndex + 1] = (uchar)rgb.green();
            m_cOriImgBuf[oriIndex + 2] = (uchar)rgb.blue();
        }
    }
    memcpy(m_cSimImgBuf, m_cOriImgBuf, CAP_WID * CAP_HEI * 3);
    updateSimImg();
}

void UnsupervisedAIMainWidget::updateSimImg()
{
    int group = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
    int nRow = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nRow;
    int nCol = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nColumn;
    int nPur = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nPercent;

    int i = 0;

    if (m_bFlgSim)
    {
        for (; i < CAP_HEI; i++)
        {
            for (int j = 0; j < CAP_WID; j++)
            {
                uchar r = m_cOriImgBuf[(i * CAP_WID + j) * 3];
                uchar g = m_cOriImgBuf[(i * CAP_WID + j) * 3 + 1];
                uchar b = m_cOriImgBuf[(i * CAP_WID + j) * 3 + 2];

                if (myAIShare.pixelIsBackgroundcomm(r, g, b, j))
                    continue;

                int nRowBegin = i - (nRow - 1) / 2 - (nRow - 1) % 2;
                int nRowEnd = i + (nRow - 1) / 2;
                int nColBegin = j - (nCol - 1) / 2 - (nCol - 1) % 2;
                int nColEnd = j + (nCol - 1) / 2;

                if (nColBegin < 0)
                    nColBegin = 0;
                if (nColEnd > IMAGE_WIDTH - 1)
                    nColEnd = IMAGE_HEIGHT - 1;
                if (nRowBegin < 0)
                    nRowBegin = 0;
                if (nRowEnd > IMAGE_HEIGHT - 1)
                    nRowEnd = IMAGE_HEIGHT - 1;

                int nBadPixelNum = 0;
                if (struCnfp.nArithmeticEnable[ARITH_INTEL_A])
                {
                    for (int m = nRowBegin; m <= nRowEnd; m++)
                    {
                        for (int n = nColBegin; n <= nColEnd; n++)
                        {
                            uchar r1 = m_cOriImgBuf[(m * CAP_WID + n) * 3];
                            uchar g1 = m_cOriImgBuf[(m * CAP_WID + n) * 3 + 1];
                            uchar b1 = m_cOriImgBuf[(m * CAP_WID + n) * 3 + 2];

                            if (myAIShare.pixelIsBackgroundcomm(r1, g1, b1, n))
                            {
                                continue;
                            }

                            if (pixelIsBad((int)r1, (int)g1, (int)b1))
                            {
                                nBadPixelNum++;
                            }
                        }//end n
                    }//end m

                    if (nBadPixelNum > nPur)
                    {
                        m_cSimImgBuf[(i * CAP_WID + j) * 3] = 255;
                        m_cSimImgBuf[(i * CAP_WID + j) * 3 + 1] = 0;
                        m_cSimImgBuf[(i * CAP_WID + j) * 3 + 2] = 0;
                    }
                }

                if (struCnfp.nArithmeticEnable[ARITH_DISCOLOR_A])
                {
                    nBadPixelNum = 0;
                    for (int m = nRowBegin; m <= nRowEnd; m++)
                    {
                        for (int n = nColBegin; n <= nColEnd; n++)
                        {
                            uchar r1 = m_cOriImgBuf[(m * CAP_WID + n) * 3];
                            uchar g1 = m_cOriImgBuf[(m * CAP_WID + n) * 3 + 1];
                            uchar b1 = m_cOriImgBuf[(m * CAP_WID + n) * 3 + 2];

                            if (myAIShare.pixelIsBackgroundcomm(r1, g1, b1, n))
                            {
                                continue;
                            }

                            if (struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nDiscolor == 0)
                            {
                                if (struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nMode)
                                {
                                    if (r1 * 10000 > (r1 + g1) * (10000 - struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nSensMax))
                                    {
                                        nBadPixelNum++;
                                    }
                                }
                                else
                                {
                                    if (r1 * 10000 < (r1 + g1) * struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nSensMin)
                                    {
                                        nBadPixelNum++;
                                    }
                                }
                            }
                            if (struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nDiscolor == 1)
                            {
                                if (struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nMode)
                                {
                                    if (r1 * 10000 > (r1 + b1) * (10000 - struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nSensMax))
                                    {
                                        nBadPixelNum++;
                                    }
                                }
                                else
                                {
                                    if (r1 * 10000 < (r1 + b1) * struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nSensMin)
                                    {
                                        nBadPixelNum++;
                                    }
                                }
                            }
                            if (struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nDiscolor == 2)
                            {
                                if (struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nMode)
                                {
                                    if (g1 * 10000 > (g1 + b1) * (10000 - struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nSensMax))
                                    {
                                        nBadPixelNum++;
                                    }
                                }
                                else
                                {
                                    if (g1 * 10000 < (g1 + b1) * struCnfp.struGroupIdentify[struGsh.nLevel][group].struGreyColor[ARITH_DISCOLOR_A].nSensMin)
                                    {
                                        nBadPixelNum++;
                                    }
                                }
                            }
                        }//end n
                    }//end m

                    if (nBadPixelNum > nPur)
                    {
                        m_cSimImgBuf[(i * CAP_WID + j) * 3] = 255;
                        m_cSimImgBuf[(i * CAP_WID + j) * 3 + 1] = 0;
                        m_cSimImgBuf[(i * CAP_WID + j) * 3 + 2] = 0;
                    }
                }
            }
        }
    }
    updateImage();
}
/*!
 * \brief UnsupervisedAIMainWidget::adjustBackground
 * \param nLevelId
 * \param bIsFront
 */
void UnsupervisedAIMainWidget::adjustBackground(int nLevelId, bool bIsFront)
{
    int nAddr, reference, nColor, target = 200;

    int  nWaittime;
    int  nDataLength;
    char* p = struGsh.sRgbRow;

    //! 前视背景
    if (bIsFront)
    {
        reference = struCnfp.struGroupCtrl[0].nLampLight[m_nFrontBkgLamp];
        nAddr = (struCnfg.nAutoReferChute - 1) * 2;
        nColor = struCnfp.nBackgroundColor[ONE_LEVEL][0];
    }
    else
    {
        reference = struCnfp.struGroupCtrl[0].nLampLight[m_nRearBkgLamp];
        nAddr = (struCnfg.nAutoReferChute - 1) * 2 + 1;
        nColor = struCnfp.nBackgroundColor[ONE_LEVEL][1];
    }

    for (int i = 0; i < 16; i++)
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
            nWaittime = 600000;
            break;
        default:
            nDataLength = 1024 * 3 + 512 + 16;
            nWaittime = 400000;
            break;
        }

        usleep(nWaittime);
        memset(p, 0, sizeof(struGsh.sRgbRow));

        MySerial.com1Read(p, nDataLength);

        if (*p == 0xAA && *(p + 1) == 0xAA && *(p + 2) == 0xAA && *(p + 3) == 0xAA)
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

            int max = 0;
            if (avr1 >= avr2 && avr1 >= avr3)
            {
                max = avr1;
            }
            if (avr2 >= avr1 && avr2 >= avr3)
            {
                max = avr2;
            }
            if (avr3 >= avr1 && avr3 >= avr1)
            {
                max = avr3;
            }
            printf("Bg r:%d g:%d b:%d max:%d\n", avr1, avr2, avr3, max);

            if (nColor == COLOR_OPT_BLACK)
            {
                return;
            }

            if (max < target)
            {
                reference += target - max;

            }
            else
            {
                break;
            }

            if (reference > 1600)
            {
                reference = 1600;
                break;
            }
            if (reference < 0)
            {
                reference = 0;
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
 * \brief UnsupervisedAIMainWidget::setBkgValueAuto
 * \param bAdjust
 */
void UnsupervisedAIMainWidget::setBkgValueAuto(bool bAdjust)
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
 * \brief UnsupervisedAIMainWidget::resetBkgLampIndex
 */
void UnsupervisedAIMainWidget::resetBkgLampIndex()
{
    /** R2机型：前视对应的背景灯为恒流源板1的第4盏灯
               后视对应的背景灯为恒流源板1的第3盏灯
        其它米机：前视对应的背景灯为恒流源板2的第3盏灯
                后视对应的背景灯为恒流源板1的第3盏灯
    */
    if (myMonoFlow.checkIsR2())
    {
        m_nFrontBkgLamp = 5;
        m_nRearBkgLamp = 2;
    }
    else
    {
        m_nFrontBkgLamp = 8;
        m_nRearBkgLamp = 2;
    }
}

void UnsupervisedAIMainWidget::getBkgValue(int nUnit, int& nVal, int& nPer)
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
                if (abs(p[i * 3] - nVal) > 30 || (p[i * 3] + p[3 * i + 1] + p[3 * i + 2]) <= 0)
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
                if (abs(p[i * 3 + 1] - nVal) > 30 || (p[i * 3] + p[3 * i + 1] + p[3 * i + 2]) <= 0)
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
                if (abs(p[i * 3 + 2] - nVal) > 30 || (p[i * 3] + p[3 * i + 1] + p[3 * i + 2]) <= 0)
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
 * \brief UnsupervisedAIMainWidget::onAIBackPressedBtnSlt
 */
void UnsupervisedAIMainWidget::onAIBackPressedBtnSlt()
{
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
    {
        MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, 0, 3);
    }
    emit goToHomePage();
}
