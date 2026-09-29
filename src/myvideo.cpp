/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myvideo.cpp
 * \brief       视频图像显示界面源文件
 * \date        2015.08.17
 */
#include "myvideo.h"
#include "unilog.h"
#include "aihelper.h"
#include "cmdworker.h"
#include "sortertypes.h"
#include "cmdudpmanager.h"
#include "configmgr.h"
#include "udpimagereceiver.h"

int nSmallMatArea = 35;
/*!
 * \brief getHead
 * \param mem
 * \param size
 * \param str
 * \return
 */
char* getHead(const char* mem, int size, char* str)
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
 * \brief 碎米率计算构造函数
 */
MyCalSmallMatThread::MyCalSmallMatThread()
{
    m_config = g_Config::getInstance();
#ifdef Q_OS_UNIX
    m_nImgWid = VIDEO_IMG_WID;
    m_nImgHei = VIDEO_ALL_IMG_HEI;
#else
    m_nImgWid = 1024;
    m_nImgHei = 500;
#endif
    m_pImgData = new uchar[m_nImgWid * m_nImgHei * 3];
    m_pDispData = new uchar[m_nImgWid * m_nImgHei];

    m_nNum = 0;
    m_nTotal = 0;
    m_nSmall = 0;

    memset(m_backMax, 0, IMAGE_WIDTH_MAX * 3);
    memset(m_backMin, 0, IMAGE_WIDTH_MAX * 3);
}

/*!
 * \brief 析构函数
 */
MyCalSmallMatThread::~MyCalSmallMatThread()
{
    if (m_pImgData != NULL)
        delete m_pImgData;
    if (m_pDispData != NULL)
        delete m_pDispData;
}

//获取用于计算碎米率的图像(RBG)
bool MyCalSmallMatThread::getImageData()
{
    LOG_TRACE_STM("Get image data ");
    QByteArray imageByteArray;
    QImage img;
    imageQMutex.lock();
    if (!ImageQueue.isEmpty())
    {
        imageByteArray = ImageQueue.first();
        imageQMutex.unlock();
    }
    else
    {
        imageQMutex.unlock();
        return false;
    }

    if (!img.loadFromData(imageByteArray, "JPEG"))
    {
        qWarning("Failed to load image from JPEG data");
        return false;
    }

    if (img.isNull())
    {
        return false;
    }

    int oriIndex;
    uchar r, g, b;
    QColor rgb;
    for (int i = 0; i < m_nImgHei; i++)
    {
        for (int j = 0; j < m_nImgWid; j++)
        {
            rgb = img.pixel(j, i);
            oriIndex = (i * m_nImgWid + j);
            r = rgb.red();
            g = rgb.green();
            b = rgb.blue();
            m_pImgData[oriIndex * 3] = r;
            m_pImgData[oriIndex * 3 + 1] = g;
            m_pImgData[oriIndex * 3 + 2] = b;
        }
    }

    //自动背景信息校验   倒数第0、1行相同，第2、3行相同
    return true;
}


/* 图像二值化 */
void MyCalSmallMatThread::gray()
{
    int nIndex;
    m_nPixelNum = 0;
    bool grayFlag, rFlag, gFlag, bFlag;
    for (int i = 0; i < m_nImgHei; i++)
    {
        for (int j = 0; j < m_nImgWid; j++)
        {
            nIndex = i * m_nImgWid + j;
            uchar r = m_pImgData[nIndex * 3];
            uchar g = m_pImgData[nIndex * 3 + 1];
            uchar b = m_pImgData[nIndex * 3 + 2];

            grayFlag = myAIShare.pixelIsBackgroundcomm(r, g, b, j);
            if (grayFlag)
            {
                m_pDispData[nIndex] = 255;
            }
            else
            {
                //m_nPixelNum++;                      //计算物料点总数
                m_pDispData[nIndex] = 0;
            }
        }
    }
}


/* 图像的腐蚀运算 */
void MyCalSmallMatThread::erosion()
{
    int flag;
    uchar* pTmpData = new uchar[m_nImgWid * m_nImgHei];
    memcpy(pTmpData, m_pDispData, m_nImgWid * m_nImgHei);

    for (int i = 1; i < m_nImgHei - 1; i++)
    {
        for (int j = 1; j < m_nImgWid - 1; j++)
        {
            flag = 1;
            for (int m = i - 1; m < i + 2; m++)
            {
                for (int n = j - 1; n < j + 2; n++)
                {
                    if (pTmpData[m * m_nImgWid + n] == 255)
                    {
                        flag = 0;
                        break;
                    }
                }

                if (flag == 0)
                {
                    break;
                }
            }
            if (flag == 0)
            {
                m_pDispData[i * m_nImgWid + j] = 255;
            }
            else
            {
                m_pDispData[i * m_nImgWid + j] = 0;
            }
        }
    }
    delete[]pTmpData;
}

/* 图像的膨胀运算 */
void MyCalSmallMatThread::dilation()
{
    int flag;
    uchar* pTmpData = new uchar[m_nImgWid * m_nImgHei];
    memcpy(pTmpData, m_pDispData, m_nImgWid * m_nImgHei);

    for (int i = 1; i < m_nImgHei - 1; i++)
    {
        for (int j = 1; j < m_nImgWid - 1; j++)
        {
            flag = 1;
            for (int m = i - 1; m < i + 2; m++)
            {
                for (int n = j - 1; n < j + 2; n++)
                {
                    if (pTmpData[m * m_nImgWid + n] == 0)
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 0)
                {
                    break;
                }
            }
            if (flag == 0)
            {
                m_pDispData[i * m_nImgWid + j] = 0;
            }
            else
            {
                m_pDispData[i * m_nImgWid + j] = 255;
            }
        }
    }

    delete[]pTmpData;
}

/* 是否为背景点 */
bool MyCalSmallMatThread::IsBack(int x, int y)
{
    if (x<0 || x>m_nImgWid - 1 || y<0 || y>m_nImgHei - 1)
    {
        return true;
    }

    if (m_pDispData[y * m_nImgWid + x] == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/* DFS深度搜索 */
bool MyCalSmallMatThread::DFS(int x, int y, int label)
{
    if (IsBack(x, y))
    {
        return false;
    }
    //考虑加限制，避免找物料陷入死循环
#if 0
    if (m_nNum > 500)
    {
        return false;
    }
#endif
    m_pDispData[y * m_nImgWid + x] = 255;//搜索过的物料点标记为非0（背景）的点
    for (int i = 0; i < 4; i++)
    {
        if (!IsBack(x + pDirection[i][0], y + pDirection[i][1]))
        {
            m_nNum++;
            DFS(x + pDirection[i][0], y + pDirection[i][1], label);
        }
    }
    return true;
}

/* 寻找物料点 */
void MyCalSmallMatThread::findMaterial()
{
    int nMatSum = 1;
    m_nNum = 1;
    m_vMatParams.clear();
    m_nPixelNum = 0;
    for (int i = 0; i < m_nImgHei; i++)
    {
        for (int j = 0; j < m_nImgWid; j++)
        {
            if (DFS(j, i, nMatSum))
            {
                MaterialParams matParams;
                matParams.nLabel = nMatSum;
                matParams.nNum = m_nNum;
                m_vMatParams.push_back(matParams);
                m_nPixelNum += m_nNum;
                m_nNum = 1;
                nMatSum++;
            }
        }
    }
}

/* 寻找小物料并显示 */
void MyCalSmallMatThread::findSmallAndDisplay()
{
    char pSmallLabel[1024];      // 小物料的索引号
    int nSmallNum = 0;          // 小物料的数目
    int nMatArea = nSmallMatArea;
    //int nMatArea = 0.2 * (m_nPixelNum/ m_vMatParams.size());
    //qDebug("m_nPixelNum = %d, m_vMatParamsSize = %d\n", m_nPixelNum, m_vMatParams.size());
    double k = 0.0001;//一个物料点的重量g

    /* 1.寻找小物料 */
    for (int i = 0; i < m_vMatParams.size(); i++)
    {
        if (m_vMatParams.at(i).nNum <= nMatArea)
        {
            pSmallLabel[nSmallNum++] = i;
        }
    }
    /* 2.刷新碎米率 */
    m_nTotal = m_vMatParams.size();
    m_nSmall = nSmallNum;

    if (m_nTotal != 0)
        m_smallMatPer = (float)m_nSmall / m_nTotal * 100;
    else
        m_smallMatPer = 0;

    if (m_nPixelNum == 0)
    {
        m_realThroughPut = 0;
    }
    else
    {
        m_realThroughPut = (double)((double)(m_nPixelNum * k) / 1000.0 * (3600 * 1000.0 / 50.0));
    }
    g_fRealSmallMatPer = m_smallMatPer;
    printf("totalMat = %d, smallMat = %d, smallPer = %.2f%\n", m_nTotal, m_nSmall, m_smallMatPer);
    fflush(0);
}


/*!
 * \brief 线程入口
 */
void MyCalSmallMatThread::run(void)
{
    bIsRunning = true;

    while (bIsRunning)
    {

        if (!getImageData())
        {
            myFlow.msleep(10);

            continue;
        }

        gray();

        /* 膨胀 */
        //dilation();
#if 1
        /* 腐蚀 */
        erosion();

        /* 根据设置的物料面积显示图片 */
        findMaterial();

        /* 寻找小物料并显示 */
        findSmallAndDisplay();
#endif
        myFlow.sleep(1);
    }
}

/* 终止线程 */
void MyCalSmallMatThread::stop()
{
    bIsRunning = false;
}

/*!
 * \brief 视频图像显示控制类构造函数
 */
MyVideo::MyVideo(QWidget* parent) :
    QWidget(parent)
{
    m_bIsCapturing = false;
    m_pCalSmallMatThread = new MyCalSmallMatThread;

    // 视频图像显示
    m_pVideoLabel = new myLabel("");
    m_pVideoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pVideoLabel->setFixedSize(1000, 500);

    m_pVideoLabel2 = new myLabel("");
    m_pVideoLabel2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pVideoLabel2->setFixedSize(1000, 500);

    infoLbl = new myLabel("");
    infoLbl->setFixedHeight(BTN_HEIGHT);
    infoLbl->setStyleSheet("color:rgb(230,0,45)");

    myPushButton* setBackButton = new myPushButton(myLan.back, myIcon.Action_Back);
    setBackButton->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    QHBoxLayout* downLay = new QHBoxLayout;
    QVBoxLayout* Lay = new QVBoxLayout;
    // 通道设置
    m_pUnitGroup = new myGroupBox(myLan.chute);

    QSize btnSize = g_Config::getInstance()->getBtnSize(SMALL_BTN_SIZE);
    m_pUnitGroup->setFont(g_Config::getInstance()->getFont());
    m_pUnitGroup->setFixedHeight(100);

    myLabel* m_RowBtn = new myLabel(myLan.row);
    m_RowBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_Row = new QLCDNumber;
    m_Row->setDigitCount(3);
    m_Row->display(g_nRow);
    m_Row->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_RowMinusBtn = new myPushButton("", myIcon.Action_Minus);
    m_RowPlusBtn = new myPushButton("", myIcon.Action_Plus);
    m_RowMinusBtn->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_RowPlusBtn->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);

    m_pUnitViewBtn = new myPushButton(myLan.front_view, QIcon(), true, true);
    m_pUnitPlusBtn = new myPushButton("", myIcon.Action_Plus);
    m_pUnitLcdNum = new QLCDNumber;
    m_pUnitLcdNum->setDigitCount(2);
    m_pUnitLcdNum->setFixedSize(SMALL_BTN_WIDTH + 20, BTN_HEIGHT);
    m_pUnitLcdNum->display(struGsh.nUnit / 2 + 1);
    m_pUnitMinusBtn = new myPushButton("", myIcon.Action_Minus);
    m_pCaptureBtn = new myPushButton("", myIcon.Media_Pause);
    m_pSimulateBtn = new myPushButton("", myIcon.Sorter_AI);
    m_pSimulateBtn->hide();


    m_pAutoAnalysisBtn = new myPushButton("", myIcon.Sorter_AI);
    m_pAutoAnalysisBtn->hide();

    udp_img_receiver = new UdpImageReceiver(SELF_UPD_VIDEO_PORT, CAM_PIXEL_WIDTH, 64);
    img_display_timer = new QTimer(this);

    connect(m_pUnitPlusBtn, SIGNAL(pressed()), this, SLOT(onUnitPlusBtnClicked()));
    connect(m_pUnitMinusBtn, SIGNAL(pressed()), this, SLOT(onUnitMinusBtnClicked()));
    connect(m_pCaptureBtn, SIGNAL(pressed()), this, SLOT(onCaptureBtnClicked()));
    connect(m_pUnitViewBtn, SIGNAL(pressed()), this, SLOT(onUnitViewClicked()));
    connect(this, SIGNAL(sBoardNumChanged()), this, SLOT(updateBoardNum()));
    connect(setBackButton, SIGNAL(pressed()), this, SLOT(onSetBackBtnClicked()));
    connect(m_RowPlusBtn, SIGNAL(pressed()), this, SLOT(onRowPlusBtnClicked()));
    connect(m_RowMinusBtn, SIGNAL(pressed()), this, SLOT(onRowMinusBtnClicked()));
    connect(m_pSimulateBtn, SIGNAL(pressed()), this, SLOT(onSimulateBtnClicked()));
    connect(m_pAutoAnalysisBtn, SIGNAL(pressed()), this, SLOT(onAutoAnalysisBtnClicked()));
    connect(udp_img_receiver, &UdpImageReceiver::imageReady, this, &MyVideo::onImageReady);
    connect(img_display_timer, &QTimer::timeout, this, &MyVideo::onDisplayImage);

    // 页面布局
    m_pUnitGridLayout = new QHBoxLayout(m_pUnitGroup);

    m_pUnitGridLayout->addWidget(m_pUnitViewBtn, 0);
    m_pUnitGridLayout->addSpacing(20);
    m_pUnitGridLayout->addWidget(m_pSimulateBtn, 1);
    m_pUnitGridLayout->addWidget(m_pUnitMinusBtn, 2);
    m_pUnitGridLayout->addWidget(m_pUnitLcdNum, 3);
    m_pUnitGridLayout->addWidget(m_pUnitPlusBtn, 4);
    m_pUnitGridLayout->addSpacing(30);
    m_pUnitGridLayout->addWidget(m_pCaptureBtn, 5);
    m_pUnitGridLayout->addSpacing(30);
    m_pUnitGridLayout->addWidget(m_pAutoAnalysisBtn, 6);

    m_pUnitViewBtn->setFixedSize(btnSize);
    m_pUnitViewBtn->setFixedWidth(SMALL_BTN_WIDTH + 20);
    m_pUnitPlusBtn->setFixedSize(btnSize);
    m_pUnitMinusBtn->setFixedSize(btnSize);
    m_pCaptureBtn->setFixedSize(btnSize);
    m_pSimulateBtn->setFixedSize(SMALL_BTN_WIDTH + 20, BTN_HEIGHT);
    m_pAutoAnalysisBtn->setFixedSize(btnSize);
    m_pVideoLayout = new QVBoxLayout;
    m_pVideoLayout->addWidget(m_pVideoLabel);
    m_pVideoLayout->addSpacing(10);
    m_pVideoLayout->addWidget(m_pVideoLabel2);
    m_pVideoLayout->addStretch();
    m_pVideoLayout->addWidget(m_pUnitGroup);
    m_pVideoLayout->setContentsMargins(0, 0, 0, 0);

    downLay->addWidget(infoLbl);
    downLay->addStretch();
    downLay->addWidget(setBackButton);


    m_RowBtn->hide();
    m_RowMinusBtn->hide();
    m_RowPlusBtn->hide();
    m_Row->hide();

    Lay->addLayout(m_pVideoLayout);
    Lay->addLayout(downLay);
    Lay->addSpacing(10);

    setLayout(Lay);
}

MyVideo::~MyVideo()
{

}

/* 上中下层或者前后视切换按钮 */
void MyVideo::onUnitViewClicked()
{
    struGsh.nUnit += (struGsh.nUnit % 2 == 0) ? 1 : -1;
    if (struGsh.nUnit % 2 == 0)
    {
        m_pUnitViewBtn->setText(myLan.front_view);
    }
    else
    {
        m_pUnitViewBtn->setText(myLan.rear_view);
    }
    emit sBoardNumChanged();
}

/* 板号增加按钮 */
void MyVideo::onUnitPlusBtnClicked()
{
    int idTotal = 0;
    switch (struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal)
    {
    case 1:     // 单视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal * 2;
        break;
    case 2:     // 双视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
    case 4:
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
    }

    if (struGsh.nUnit < idTotal - 2)
    {
        struGsh.nUnit += 2;
        emit sBoardNumChanged();
    }
    m_pUnitLcdNum->display(struGsh.nUnit / 2 + 1);
}

/* 板号减少按钮 */
void MyVideo::onUnitMinusBtnClicked()
{
    if (struGsh.nUnit > 1)
    {
        struGsh.nUnit -= 2;
        emit sBoardNumChanged();
    }
    m_pUnitLcdNum->display(struGsh.nUnit / 2 + 1);
}

/* 信号开始/暂停按钮 */
void MyVideo::onCaptureBtnClicked()
{
    LOG_INFO_STM("click capture, is capturing:" << m_bIsCapturing);
    m_bIsCapturing = !m_bIsCapturing;
    startCapture(m_bIsCapturing);
}

//计算实时碎米率
void MyVideo::startCalSmallMatPerRSC(bool bIsCapturing)
{
    if (bIsCapturing)
    {
        m_pCalSmallMatThread->start();
    }
    else
    {
        if (m_pCalSmallMatThread->isRunning())
        {
            m_pCalSmallMatThread->stop();
        }
    }
}

void MyVideo::startCapture(bool bIsCapturing)
{
    // 视频预览操作
    m_bIsCapturing = bIsCapturing;
    QByteArray img;
    if (m_bIsCapturing)
    {
        infoWidget->setLabelText(myLan.video_starting_info);
        infoWidget->delayShow();
        img_display_timer->start(40);
        udp_img_receiver->SetImageHegiht(ConfigMgr::Instance().GetAiCfgInfo().video_view_height_);
        udp_img_receiver->start();
        LOG_INFO_STM("start view, nunit:" << struGsh.nUnit << ", level:" << struGsh.nLevel);
        onViewRequest(struGsh.nUnit, true);
        MySerial.com1Write(0x0111, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, 0, 1, 3);

        myFlow.msleep(100);
        infoWidget->hide();
    }
    else
    {
        infoWidget->setLabelText(myLan.video_stoping_info);
        infoWidget->delayShow();

        img_display_timer->stop();
        LOG_INFO_STM("stop view, nunit:" << struGsh.nUnit << ", level:" << struGsh.nLevel);
        onViewRequest(struGsh.nUnit, false);
        MySerial.com1Write(0x0111, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, 0, 0, 3);
        udp_img_receiver->stop();
        myFlow.sleep(2);
        infoWidget->hide();
    }

    resetCaptureState(m_bIsCapturing);
}


void MyVideo::resetCaptureMode(bool bIsUsb)
{
    int nCapMode = bIsUsb;
    //    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
    //		MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, nCapMode, 3);
    //	}
}

void MyVideo::onViewRequest(int idx, bool is_start)
{
    ViewParam vp;
    vp.flag_ = is_start ? 1 : 2;

    QByteArray request = cmdworker::ViewParamRequest2(vp);
    QString ip = ai_helper::GetAiIpByIndex(idx);

    bool ok = CmdUdpManager::instance().onSendCommand(QHostAddress(ip), AI_UPD_CMD_PORT, request);
    if (!ok)
    {
        LOG_ERROR_STM("video view opr index:" << idx << " ip:" << ip.toStdString() << " send command failed! requst body:" << request.toHex(' ').toUpper().toStdString());
        return;
    }
}

void MyVideo::onDisplayImage()
{
    QImage img;
    {
        QMutexLocker lock(&img_mutex);
        img = lastest_img;
    }
    const QSize size = m_pVideoLabel->size();
    QImage displayImage = img.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation);
    m_pVideoLabel->setPixmap(QPixmap::fromImage(displayImage));
}

void MyVideo::updateVideoWidget()
{
    updateVideo();
}

void MyVideo::updateBoardNum()
{
    // 切换通道和前后视时，调用
    LOG_INFO_STM("updateBoardNum, m_bIsCapturing:" << m_bIsCapturing);
}

void MyVideo::resetCaptureState(bool bState)
{
    if (bState)
    {
        m_pCaptureBtn->setIcon(myIcon.Media_Pause);
    }
    else
    {
        m_pCaptureBtn->setIcon(myIcon.Media_Start);
    }

}

void MyVideo::onSetBackBtnClicked()
{
    startCapture(false);
    myFlow.initInterfaceTransMode(0);
    emit backToHomePageSig();
}

void MyVideo::onRowPlusBtnClicked()
{
    if (g_nRow < 256)
    {
        g_nRow += 64;
    }
    m_Row->display(g_nRow);
    m_pVideoLabel->setFixedSize(1000, 500);
    m_pVideoLabel2->setFixedSize(1000, 500);
}

void MyVideo::onRowMinusBtnClicked()
{
    if (g_nRow > 64)
    {
        g_nRow -= 64;
    }
    m_Row->display(g_nRow);
    m_pVideoLabel->setFixedSize(1000, 500);
    m_pVideoLabel2->setFixedSize(1000, 500);
}

void MyVideo::onSimulateBtnClicked()
{
    g_bIsSim = !g_bIsSim;
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++)
        {
            int nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
            //            MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nAddr, 0, 0, 0, 0, g_bIsSim, 3);
        }
    }
    if (g_bIsSim)
    {
        m_pSimulateBtn->setRedColor(GREEN);
    }
    else
    {
        m_pSimulateBtn->setRedColor(DEF);
    }
}

void MyVideo::onImageReady(const QImage& img)
{
    QMutexLocker lock(&img_mutex);
    lastest_img = img;
}


void MyVideo::onAutoAnalysisBtnClicked()
{
    LOG_INFO_STM("onAutoAnalysisBtnClicked m_bIsCapturing:" << m_bIsCapturing);
    if (m_bIsCapturing)
    {
        m_bOriCapStat = m_bIsCapturing;
        m_bIsCapturing = false;
        startCapture(false);
        infoWidget->setLabelText(myLan.video_stoping_info);
        infoWidget->delayShow();
        myFlow.sleep(4);
        infoWidget->hide();
        resetCaptureState(false);
    }
    else
    {
        m_bOriCapStat = m_bIsCapturing;
    }
    emit goToAutoAnalysisSig();
}

/*!
 * \brief 视频图像信息刷新
 *
 */
void MyVideo::updateVideo()
{
    resetLocalParams();

    // unit view
    m_pUnitLcdNum->display(struGsh.nUnit / 2 + 1);
    m_pUnitViewBtn->show();
    if (struGsh.nUnit % 2 == 1)
    {
        m_pUnitViewBtn->setText(myLan.rear_view);
    }
    else
    {
        m_pUnitViewBtn->setText(myLan.front_view);
    }
    if (m_bIsCapturing)
    {
        m_pCaptureBtn->setIcon(myIcon.Media_Pause);
    }
    else
    {
        m_pCaptureBtn->setIcon(myIcon.Media_Start);
    }

    m_pUnitGroup->show();
}

void MyVideo::resetLocalParams()
{
    g_fRealThroughPut = 0;
    g_fRealDirtPer = 0;
    m_bIsViewOrUnitChanged = false;
}
