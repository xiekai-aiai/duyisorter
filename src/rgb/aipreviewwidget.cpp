/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aipreviewwidget.cpp
 * \brief       彩色机型智能分析图片预览界面源文件
 * \date        2015.01.14
 */
#include "aipreviewwidget.h"

/* 用于尺寸缩放时显示区域在原始区域中的索引号 */
static quint8 oneToTwo[4] = {0,2,8,10};
static quint8 twoToOne[16] = {0,0,1,1,0,0,1,1,2,2,3,3,2,2,3,3};
static quint8 twoToFour[16] = {0,2,4,6,16,18,20,22,32,34,36,38,48,50,52,54};
static quint8 fourToTwo[64] = {0,0,1,1,2,2,3,3,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,4,4,5,5,6,6,7,7,
                       8,8,9,9,10,10,11,11,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,12,12,13,13,14,14,15,15};
static quint8 fourToEight[64] = {0,2,4,6,8,10,12,14,32,34,36,38,40,42,44,46,
                                 64,66,68,70,72,74,76,78,96,98,100,102,104,106,108,110,
                                 128,130,132,134,136,138,140,142,160,162,164,166,168,170,172,174,
                                 192,194,196,198,200,202,204,206,224,226,228,230,232,234,236,238};
static quint8 eightToFour[256] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,
                                 8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,
                                 16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23,16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23,
                                 24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,
                                 32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,
                                 40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,
                                 48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,
                                 56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63,56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63};

/* 用于图片显示的缓冲区 */
static char oriBuf[IMAGE_WIDTH*IMAGE_HEIGHT*3];          // 用于存储原始图片的数据
static uchar chgBuf[IMAGE_WIDTH*IMAGE_HEIGHT*3];         // 用于存储被修改的图片数据
static uchar disBuf[DISPLAY_WIDTH*DISPLAY_HEIGHT*3];     // 用于存储显示的图片信息

/* 智能分析相关参数 */
static AIUniversalParams uniParams;
static AIAloneParams aloneParams[IMG_MAX_NUM];

AIPreviewWidget::AIPreviewWidget(QWidget *parent) :
    QWidget(parent)
{
    if (LCD_WIDTH == 640) {
        setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT);
    } else {
        setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT);
    }

    m_config = g_Config::getInstance();
    initAIAloneParams();
    initAIDisParams();
    createAIPreviewPage();
}

/* 初始化智能分析中的显示参数 */
void AIPreviewWidget::initAIDisParams()
{
    m_bIsSimulate = false;
    m_bIsPurSim = false;
}

/* 初始化智能分析中的独立参数 */
void AIPreviewWidget::initAIAloneParams()
{
    for (int i = 0; i < IMG_MAX_NUM; i++) {
        aloneParams[i].nSelTimes = 0;
        aloneParams[i].nSelPixelSum = 0;
        aloneParams[i].nDisPixelSum = 0;
    }
}

/* 重置智能分析中的公有参数 */
void AIPreviewWidget::initAIUniParams()
{
    uniParams.nScale = 1;
    uniParams.nHMoveTimes = 0;
    uniParams.nVMoveTimes = 0;
    uniParams.nMaxMoveTimes = 2*uniParams.nScale-1;
    uniParams.nIndex = 0;
    uniParams.nSpaceX = 0;
    uniParams.nSpaceY = 0;
    uniParams.bHasTar = false;
}

/* 生成智能分析预览页面 */
void AIPreviewWidget::createAIPreviewPage()
{
    QVBoxLayout *upRightLay = new QVBoxLayout;
    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *midLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(this);

    /* 图片预览页面上半部分控件的布局 */
    m_imgLabel = new myLabel;
    m_imgLabel->setFixedSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    m_imgLabel->setScaledContents(true);

    QGroupBox *zoomGroup = new QGroupBox;
    zoomGroup->setMaximumHeight(BTN_HEIGHT*2);
    QHBoxLayout *zoomHboxLayout = new QHBoxLayout(zoomGroup);
    QSize btnSize = m_config->getBtnSize(SMALL_BTN_SIZE);
    m_enlargeBtn = new myPushButton(myIcon.Edit_ZoomIn, btnSize);
    m_reduceBtn = new myPushButton(myIcon.Edit_ZoomOut, btnSize);
    zoomHboxLayout->addWidget(m_enlargeBtn);
    zoomHboxLayout->addWidget(m_reduceBtn);

    QGroupBox *basicGroup = new QGroupBox;
    QGridLayout *basicLay = new QGridLayout(basicGroup);
    btnSize = m_config->getBtnSize(DEFAULT_BTN_SIZE);
    m_simulateBtn = new myPushButton(myLan.ai_simulate, QIcon());
    m_simulateBtn->setFixedSize(btnSize);
    m_puritySimBtn = new myPushButton(myLan.ai_start_purity, QIcon());
    m_puritySimBtn->setFixedSize(btnSize);

    AISensLabel = new myLabel(myLan.sensitivity);
    AISensLabel->setFixedSize(70,BTN_HEIGHT);
    QString strAISens = QString("%1").arg(struCnfp.struGroupIdentify[0][0].struIntel[struGsh.curIntelArithNo].nSens);
    AISensLbe = new myLineEdit(strAISens);
    AISensLbe->setFixedSize(70,BTN_HEIGHT);

    AIRowLabel = new myLabel(myLan.scale);
    AIRowLabel->setFixedSize(50,BTN_HEIGHT);
    QString strAIRow = QString("%1").arg(struCnfp.struGroupIdentify[0][0].struIntel[struGsh.curIntelArithNo].nRow);
    AIRowLbe = new myLineEdit(strAIRow);
    AIRowLbe->setFixedSize(70,BTN_HEIGHT);
    AIPercentLabel = new myLabel(myLan.purity);
    AIPercentLabel->setFixedSize(50,BTN_HEIGHT);
    QString strAIPercent = QString("%1").arg(struCnfp.struGroupIdentify[0][0].struIntel[struGsh.curIntelArithNo].nPercent);
    AIPercentLbe = new myLineEdit(strAIPercent);
    AIPercentLbe->setFixedSize(70,BTN_HEIGHT);

    basicLay->addWidget(m_simulateBtn, 0, 0, 1, 2,Qt::AlignCenter);
    basicLay->addWidget(m_puritySimBtn, 1, 0, 1, 2,Qt::AlignCenter);
    basicLay->addWidget(AISensLabel,3,0,1,1);
    basicLay->addWidget(AISensLbe, 3, 1, 1, 1);
    basicLay->addWidget(AIRowLabel, 4, 0, 1, 1);
    basicLay->addWidget(AIRowLbe, 4, 1, 1, 1);
    basicLay->addWidget(AIPercentLabel, 5, 0, 1, 1);
    basicLay->addWidget(AIPercentLbe, 5, 1, 1, 1);
    basicLay->setContentsMargins(0, 5, 0, 0);
    basicLay->setSpacing(5);
    basicGroup->setMinimumHeight(4*btnSize.height()+40);

    m_moreGroup = new QGroupBox;
    QGridLayout *moreLay = new QGridLayout(m_moreGroup);
    btnSize = m_config->getBtnSize(DEFAULT_BTN_SIZE);
    m_infModeBtn = new myPushButton(myLan.infra, QIcon());
    m_infModeBtn->setFixedSize(btnSize);
    m_samColorBtn = new myPushButton("", QIcon());
    m_samColorBtn->setFixedSize(btnSize);
    m_samModeBtn = new myPushButton("", QIcon());
    m_samModeBtn->setFixedSize(btnSize);
    m_samSensLcd = new myLCDNumber;
    m_samSensLcd->setDigitCount(3);
    m_samSensLcd->setFixedHeight(btnSize.height());

    moreLay->addWidget(m_infModeBtn, 0, 0,1,1);
    moreLay->addWidget(m_samColorBtn, 1, 0,1,1);
    moreLay->addWidget(m_samModeBtn, 2, 0,1,1);
    moreLay->addWidget(m_samSensLcd,3,0,1,1);
    moreLay->setContentsMargins(0, 5, 0, 0);

    upRightLay->addWidget(zoomGroup);
    upRightLay->addWidget(basicGroup);
    upRightLay->addWidget(m_moreGroup);

    if(LCD_WIDTH == 640)
        upRightLay->setContentsMargins(0, 0, 0, 0);
    else if(LCD_WIDTH == 1024)
        upRightLay->setContentsMargins(10, 10, 10, 10);
    upLay->addWidget(m_imgLabel);
    if(LCD_WIDTH == 1024)
        upLay->addSpacing(25);
    upLay->addLayout(upRightLay);
    if(LCD_WIDTH == 640)
        upLay->setContentsMargins(0, 0, 0, 0);

    /* 图片预览页面中间部分控件的布局 */
    m_matStatusLabel = new myLabel;
    m_matStatusLabel->setAlignment(Qt::AlignLeft);
    if(LCD_WIDTH == 640)
        m_matStatusLabel->setMinimumSize(200, 18);
    else
        m_matStatusLabel->setFixedSize(400, 30);
    m_pixelColorLabel = new myLabel;
    m_pixelColorLabel->setMinimumWidth(400);
    m_pixelColorLabel->setAlignment(Qt::AlignLeft);
    midLay->addWidget(m_matStatusLabel);
    midLay->addWidget(m_pixelColorLabel);

    /* 图片预览页面下半部分控件的布局 */
    int height = btnSize.height();
    m_relastBtn = new myPushButton(myLan.ai_reselect_last, QIcon());
    m_relastBtn->setFixedHeight(height);
    m_optSamBtn = new myPushButton(myLan.ai_optimize_sample, QIcon());
    m_optSamBtn->setFixedHeight(height);
    m_backBtn = new myPushButton(myLan.back, QIcon());
    m_backBtn->setFixedHeight(height);
    downLay->addWidget(m_relastBtn);
    downLay->addWidget(m_optSamBtn);
    downLay->addWidget(m_backBtn);
    if(LCD_WIDTH == 640)
        downLay->setSpacing(100);
    else if(LCD_WIDTH == 1024)
        downLay->setSpacing(200);

    /* 图片预览页面的整体布局 */
    mainLay->addLayout(upLay);
    if(LCD_WIDTH == 1024)
        mainLay->addSpacing(10);
    mainLay->addLayout(midLay);
    if(LCD_WIDTH == 1024)
        mainLay->addSpacing(10);
    mainLay->addLayout(downLay);
    if(LCD_WIDTH == 640)
        mainLay->setContentsMargins(0, 0, 10, 10);
    else if(LCD_WIDTH == 1024)
        mainLay->addSpacing(10);
    createMoveBtns();

    /* 连接预览页面中的信号与槽 */
    connect(m_upBtn, SIGNAL(pressed()), this, SLOT(onUpBtnClicked()));
    connect(m_downBtn, SIGNAL(pressed()), this, SLOT(onDownBtnClicked()));
    connect(m_leftBtn, SIGNAL(pressed()), this, SLOT(onLeftBtnClicked()));
    connect(m_rightBtn, SIGNAL(pressed()), this, SLOT(onRightBtnClicked()));
    connect(m_enlargeBtn, SIGNAL(pressed()), this, SLOT(onEnlargeBtnClicked()));
    connect(m_reduceBtn, SIGNAL(pressed()), this, SLOT(onReduceBtnClicked()));
    connect(m_simulateBtn, SIGNAL(pressed()), this, SLOT(onSimulateBtnClicked()));
    connect(m_puritySimBtn, SIGNAL(pressed()), this, SLOT(onStartPurSimBtnClicked()));
    connect(m_infModeBtn, SIGNAL(pressed()), this, SLOT(onInfModeBtnClicked()));
    connect(m_samColorBtn, SIGNAL(pressed()), this, SLOT(onSamColorBtnClicked()));
    connect(m_samModeBtn, SIGNAL(pressed()), this, SLOT(onSamModeBtnClicked()));
    connect(m_samSensLcd, SIGNAL(pressed()), this, SLOT(onSamSensLcdPressed()));
    connect(m_relastBtn, SIGNAL(pressed()), this, SLOT(onReLastBtnClicked()));
    connect(m_optSamBtn, SIGNAL(pressed()), this, SLOT(onOptSamBtnClicked()));
    connect(m_backBtn, SIGNAL(clicked()), this, SLOT(onBackBtnClicked()));
    connect(AISensLbe,SIGNAL(pressed()),this,SLOT(onAISensLetPressed()));
    connect(AIRowLbe,SIGNAL(pressed()),this,SLOT(onAIRowLetPressed()));
    connect(AIPercentLbe,SIGNAL(pressed()),this,SLOT(onAIPercentLetPressed()));
}

/* 生成上下左右按钮 */
void AIPreviewWidget::createMoveBtns()
{
    switch(LCD_WIDTH){
    case 640:
        m_nDistanceX = 0;
#ifdef Q_OS_UNIX
        m_nDistanceY = 0;
#else
        m_nDistanceY = 3;
#endif
        break;
    case 800:
        m_nDistanceX = 9;
        m_nDistanceY = 17;
        break;
    case 1024:
        m_nDistanceX = 9;
        m_nDistanceY = 48;
        break;
    default:
        break;
    }

    QSize btnSize = m_config->getBtnSize(SMALL_BTN_SIZE);
    int upX = m_nDistanceX+(DISPLAY_WIDTH-btnSize.width())/2;
    int upY = m_nDistanceY;
    int downX = upX;
    int downY = m_nDistanceY+DISPLAY_HEIGHT-btnSize.height();
    int leftX = m_nDistanceX;
    int leftY = m_nDistanceY+(DISPLAY_HEIGHT-btnSize.height())/2;
    int rightX = m_nDistanceX+DISPLAY_WIDTH-btnSize.width();
    int rightY = leftY;

    m_upBtn = new myPushButton("", myIcon.Arrow_Up, this);
    m_upBtn->setGeometry(QRect(QPoint(upX, upY), btnSize));
    m_downBtn = new myPushButton("", myIcon.Arrow_Down, this);
    m_downBtn->setGeometry(QRect(QPoint(downX, downY), btnSize));
    m_leftBtn = new myPushButton("", myIcon.Arrow_Left, this);
    m_leftBtn->setGeometry(QRect(QPoint(leftX, leftY), btnSize));
    m_rightBtn = new myPushButton("", myIcon.Arrow_Right, this);
    m_rightBtn->setGeometry(QRect(QPoint(rightX, rightY), btnSize));
}

/* 根据图片索引获取图片数据 */
void AIPreviewWidget::getDataAndShow()
{
    QString fileName;

    fileName = QString("%1.png").arg(m_nImgIndex);
    getImageData(fileName);
    getSelPointsFromFile();

    if(m_bIsSimulate)
        displaySimulate();
    else
        refreshImageDisplay();

    refreshSamDisplay();
    refreshBtnStatus();
    refreshMatStatusLabel();
}

/* 根据图片索引获取数据 */
bool AIPreviewWidget::getImageData(QString fileName)
{
    QFile file(QString("/tmp/%1").arg(fileName));
    if(!file.exists()){
        qDebug("%s: file open error!", fileName.toLatin1().data());
        return false;
    }

    QImage image(QString("/tmp/%1").arg(fileName));
    int oriIndex;
    for(int i=0; i<IMAGE_HEIGHT; i++){
        for(int j=0; j<IMAGE_WIDTH; j++){
            QColor rgb = image.pixel(j, i);
            oriIndex = (i*IMAGE_WIDTH+j)*3;

            oriBuf[oriIndex] = rgb.red();
            oriBuf[oriIndex+1] = rgb.green();
            oriBuf[oriIndex+2] = rgb.blue();
        }
    }
    memcpy(chgBuf, oriBuf, IMAGE_WIDTH*IMAGE_HEIGHT*3);

    return true;
}

/* 刷新取样组的显示 */
void AIPreviewWidget::refreshSamDisplay()
{
    switch(struGsh.nIntelSampColor[m_nImgIndex])
    {
    case COLOR_RED:
        m_samColorBtn->setText(myLan.red);
        break;
    case COLOR_GREEN:
        m_samColorBtn->setText(myLan.green);
        break;
    case COLOR_BLUE:
        m_samColorBtn->setText(myLan.blue);
        break;
    case COLOR_RED_GREEN:
        m_samColorBtn->setText(myLan.red_green);
        break;
    case COLOR_RED_BLUE:
        m_samColorBtn->setText(myLan.red_blue);
        break;
    case COLOR_GREEN_BLUE:
        m_samColorBtn->setText(myLan.green_blue);
        break;
    default:
        break;
    }

    if(struGsh.nIntelSampMode[m_nImgIndex] == 0)
        m_samModeBtn->setText(myLan.dark);
    else
        m_samModeBtn->setText(myLan.light);

    m_samSensLcd->display(struGsh.nIntelSampSens[m_nImgIndex]);
}

/* 刷新智能按钮的状态 */
void AIPreviewWidget::refreshAIParamsDisplay()
{
    int group = -1;
    int unit = struGsh.nUnit;
    for(int i=0; i<MAX_GROUP_IDTNTIFY; i++){
        for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitCount; j++){
            if(unit == struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitId[j]) {
                group = i;
            }
        }
    }

    QString strAISens = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nSens);
    AISensLbe->setText(strAISens);

    QString strAIRow = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nRow);
    AIRowLbe->setText(strAIRow);

    QString strAIPercent = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nPercent);
    AIPercentLbe->setText(strAIPercent);
}

/* 根据当前图片的尺寸刷新按钮的状态 */
void AIPreviewWidget::refreshBtnStatus()
{
    m_infModeBtn->setEnabled(false);
    m_infModeBtn->setRedColor(DEF);
    m_optSamBtn->setEnabled(true);
    m_simulateBtn->setEnabled(true);
    m_samColorBtn->setEnabled(true);
    m_samModeBtn->setEnabled(true);
    m_samSensLcd->setEnabled(true);

    if(myAIShare.m_bIsOptSam[m_nImgIndex])
        m_optSamBtn->setRedColor(GREEN);
    else
        m_optSamBtn->setRedColor(DEF);

    if(aloneParams[m_nImgIndex].nSelTimes > 0)
        m_relastBtn->setEnabled(true);
    else
        m_relastBtn->setEnabled(false);

    if(uniParams.nScale == MIN_SCALE){         //设置图片显示的最小尺寸为1
        m_enlargeBtn->setEnabled(true);
        m_reduceBtn->setEnabled(false);
    }
    else if(uniParams.nScale>=MIN_SCALE && uniParams.nScale<MAX_SCALE){
        m_enlargeBtn->setEnabled(true);
        m_reduceBtn->setEnabled(true);
    }
    else if(uniParams.nScale == MAX_SCALE){    //设置图片显示的最大尺寸为8
        m_enlargeBtn->setEnabled(false);
        m_reduceBtn->setEnabled(true);
    }

    if(uniParams.nHMoveTimes == 0){
        m_leftBtn->setEnabled(false);
        m_rightBtn->setEnabled(true);
    }
    else if(uniParams.nHMoveTimes>0 && uniParams.nHMoveTimes<uniParams.nMaxMoveTimes){
        m_leftBtn->setEnabled(true);
        m_rightBtn->setEnabled(true);
    }
    else if(uniParams.nHMoveTimes == uniParams.nMaxMoveTimes){
        m_leftBtn->setEnabled(true);
        m_rightBtn->setEnabled(false);
    }

    if(uniParams.nVMoveTimes == 0){
        m_upBtn->setEnabled(false);
        m_downBtn->setEnabled(true);
    }
    else if(uniParams.nVMoveTimes>0 && uniParams.nVMoveTimes<uniParams.nMaxMoveTimes){
        m_upBtn->setEnabled(true);
        m_downBtn->setEnabled(true);
    }
    else if(uniParams.nVMoveTimes == uniParams.nMaxMoveTimes){
        m_upBtn->setEnabled(true);
        m_downBtn->setEnabled(false);
    }

    if(uniParams.nScale==1 && DISPLAY_HEIGHT>=IMAGE_HEIGHT){
        m_upBtn->setEnabled(false);
        m_downBtn->setEnabled(false);
    }

    /* 仿真模式或物料未分类情况下按钮状态 */
    if(m_bIsSimulate || myAIShare.m_nMatStatus[m_nImgIndex]==CLASS_NULL)
    {
        m_relastBtn->setEnabled(false);
        m_optSamBtn->setEnabled(false);
    }

    if(m_bIsSimulate)
    {
        m_simulateBtn->setRedColor(GREEN);
        m_puritySimBtn->setEnabled(true);
    }
    else
    {
        m_simulateBtn->setRedColor(DEF);
        m_puritySimBtn->setEnabled(false);
    }

    if(m_bIsPurSim)
        m_puritySimBtn->setRedColor(GREEN);
    else
        m_puritySimBtn->setRedColor(DEF);
}

/* 根据当前状态刷新物料状态的显示 */
void AIPreviewWidget::refreshMatStatusLabel()
{
    QString statusStr;
    if(myAIShare.m_nMatStatus[m_nImgIndex] == CLASS_GOOD)
        statusStr = myLan.ai_good;
    else if(myAIShare.m_nMatStatus[m_nImgIndex] == CLASS_BAD)
        statusStr = myLan.ai_bad;

    QString str;
    if(myAIShare.m_nMatStatus[m_nImgIndex] != CLASS_NULL)
        str = QString("%1 %2-%3/%4").arg(myLan.ai_material).arg(m_nImgIndex+1).arg(statusStr)
                .arg(aloneParams[m_nImgIndex].nDisPixelSum);
    else
        str = QString("%1 %2-%3").arg(myLan.ai_material).arg(m_nImgIndex+1).arg(myLan.ai_preview_mode);

    m_matStatusLabel->setText(str);
}

/* 将缓冲区中的数据更新到显示页面中 */
void AIPreviewWidget::updateImage()
{
    uniParams.nSpaceX = (IMAGE_WIDTH-DISPLAY_WIDTH/uniParams.nScale)/uniParams.nMaxMoveTimes;
    uniParams.nSpaceY = (IMAGE_HEIGHT-DISPLAY_HEIGHT/uniParams.nScale)/uniParams.nMaxMoveTimes;

    int tmpWid = DISPLAY_WIDTH/uniParams.nScale;
    int tmpHei = DISPLAY_HEIGHT/uniParams.nScale;
    int disbufIndex;
    int chgBufIndex;
    for(int i=0; i<tmpHei; i++){
        for(int j=0; j<tmpWid; j++){
            disbufIndex = (i*uniParams.nScale*DISPLAY_WIDTH+j*uniParams.nScale)*3;
            chgBufIndex = ((i+uniParams.nIndex/(uniParams.nScale*2)*uniParams.nSpaceY)*IMAGE_WIDTH
                              +j+uniParams.nIndex%(uniParams.nScale*2)*uniParams.nSpaceX)*3;
            disBuf[disbufIndex] = chgBuf[chgBufIndex];
            disBuf[disbufIndex+1] = chgBuf[chgBufIndex+1];
            disBuf[disbufIndex+2] = chgBuf[chgBufIndex+2];
        }
    }

    /* 若尺寸比原始尺寸大，则需要填充显示buf中未赋值的像素点 */
    int changeIndex;
    int baseIndex;
    if(uniParams.nScale > 1){
        tmpWid = DISPLAY_WIDTH/uniParams.nScale;
        tmpHei = DISPLAY_HEIGHT/uniParams.nScale;
        for(int i=0; i<tmpHei; i++){
            for(int j=0; j<tmpWid; j++){
                for(int k=0; k<uniParams.nScale; k++){
                    for(int l=0; l<uniParams.nScale; l++){
                        if(k!=0 || l!=0){
                            changeIndex = ((i*uniParams.nScale+k)*DISPLAY_WIDTH+j*uniParams.nScale+l)*3;
                            baseIndex = ((i*uniParams.nScale)*DISPLAY_WIDTH+j*uniParams.nScale)*3;
                            disBuf[changeIndex] = disBuf[baseIndex];
                            disBuf[changeIndex+1] = disBuf[baseIndex+1];
                            disBuf[changeIndex+2] = disBuf[baseIndex+2];
                        }//end if
                    }//end for(l)
                }//end for(k)
            }//end for(j)
        }//end for(i)

        /* 解决图片放大八倍后最后四行无数据的问题 */
        tmpHei = DISPLAY_HEIGHT-1-DISPLAY_HEIGHT%uniParams.nScale;
        for(int i=DISPLAY_HEIGHT-1; i>tmpHei; i--){
            for(int j=0; j<DISPLAY_WIDTH; j++){
                baseIndex = ((DISPLAY_HEIGHT-1-DISPLAY_HEIGHT%uniParams.nScale)*DISPLAY_WIDTH+j)*3;
                changeIndex = (i*DISPLAY_WIDTH+j)*3;
                disBuf[changeIndex] = disBuf[baseIndex];
                disBuf[changeIndex+1] = disBuf[baseIndex+1];
                disBuf[changeIndex+2] = disBuf[baseIndex+2];
            }
        }
    }//end if

    QImage img(disBuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, QImage::Format_RGB888);
    m_imgLabel->setPixmap(QPixmap::fromImage(img));
}

/* 将红外图片缓冲区中的数据更新到显示页面中 */
void AIPreviewWidget::updateInfImage()
{
    QImage img((uchar *)oriBuf, INF_WIDTH, INF_HEIGHT, QImage::Format_RGB888);
    m_imgLabel->setPixmap(QPixmap::fromImage(img));
}

/* 根据勾选的情况修改缓冲区中的数据 */
void AIPreviewWidget::changeImage()
{
    int pointIndex = 0;
    aloneParams[m_nImgIndex].nDisPixelSum = 0;
    for(int i=0; i<aloneParams[m_nImgIndex].nSelTimes-1; i++){
        pointIndex += aloneParams[m_nImgIndex].nSelPixelNum[i];
        aloneParams[m_nImgIndex].nDisPixelSum += aloneParams[m_nImgIndex].nDisPixelNum[i];
    }

    for(int i=pointIndex; i<aloneParams[m_nImgIndex].nSelPixelSum; i++){
        int x = uniParams.selectedPoint[i].x();
        int y = uniParams.selectedPoint[i].y();

        if(myAIShare.m_bIsOptSam[m_nImgIndex]){
            if(pixelIsEdge(x, y))
                continue;
        }

        chgBuf[(y*IMAGE_WIDTH+x)*3] = aloneParams[m_nImgIndex].displayColor.red();
        chgBuf[(y*IMAGE_WIDTH+x)*3+1] = aloneParams[m_nImgIndex].displayColor.green();
        chgBuf[(y*IMAGE_WIDTH+x)*3+2] = aloneParams[m_nImgIndex].displayColor.blue();

        aloneParams[m_nImgIndex].nDisPixelSum++;
    }
}

/* 根据优化取样的标志位刷新显示 */
void AIPreviewWidget::refreshImageDisplay()
{
    int displayNum;
    int beginIndex = 0;
    uchar rgb[3];

    aloneParams[m_nImgIndex].nDisPixelSum = 0;
    for (int i = 0; i < aloneParams[m_nImgIndex].nSelTimes; i++) {
        displayNum = 0;
        for (int j = 0; j < aloneParams[m_nImgIndex].nSelPixelNum[i]; j++) {
            int x = uniParams.selectedPoint[beginIndex+j].x();
            int y = uniParams.selectedPoint[beginIndex+j].y();
            int index = (y*IMAGE_WIDTH+x)*3;

            memcpy(rgb, oriBuf+index, 3);
            memcpy(chgBuf+index, rgb, 3);

            if (myAIShare.m_bIsOptSam[m_nImgIndex]) {
                if (pixelIsEdge(x, y)) {
                    continue;
                }
            }

            chgBuf[index] = aloneParams[m_nImgIndex].displayColor.red();
            chgBuf[index+1] = aloneParams[m_nImgIndex].displayColor.green();
            chgBuf[index+2] = aloneParams[m_nImgIndex].displayColor.blue();

            displayNum++;
            aloneParams[m_nImgIndex].nDisPixelSum++;
        }
        aloneParams[m_nImgIndex].nDisPixelNum[i] = displayNum;
        beginIndex += aloneParams[m_nImgIndex].nSelPixelNum[i];
    }

    updateImage();
}

/* 普通智能算法下的仿真 */
void AIPreviewWidget::normalAISimulate(uchar *pStatus)
{
    int goodSum = 0;
    int badSum = 0;
    bool bIsBad = false;

    /* 判断物料设置选择对应的方程 */
    switch(struCnfp.nMatIntelSeq)
    {
    case 0:
        goodSum = 0;
        badSum = 0;
        break;
    case 1:
        goodSum = 1;
        badSum = 1;
        break;
    case 2:
        goodSum = 1;
        badSum = 2;
        break;
    case 3:
        goodSum = 1;
        badSum = 3;
        break;
    case 4:
        goodSum = 2;
        badSum = 1;
        break;
    case 5:
        goodSum = 3;
        badSum = 1;
        break;
    case 6:
        goodSum = 2;
        badSum = 2;
        break;
    default:
        break;
    }

    for(int i=0; i<IMAGE_HEIGHT; i++){
        for(int j=0; j<IMAGE_WIDTH; j++){
            bIsBad = false;
            uchar r = oriBuf[(i*IMAGE_WIDTH+j)*3];
            uchar g = oriBuf[(i*IMAGE_WIDTH+j)*3+1];
            uchar b = oriBuf[(i*IMAGE_WIDTH+j)*3+2];

            if(myAIShare.pixelIsBackgroundcomm(r, g, b,j))

                continue;

            /* 设置物料点的属性 */
            pStatus[i*IMAGE_WIDTH+j] = PIXEL_MATTER;
            switch(goodSum){
            case 1:
                if(badSum == 1){
                    if(pixelIsBad(0, r, g, b))
                        bIsBad = true;
                }
                else if(badSum == 2){
                    if(pixelIsBad(0, r, g, b) || pixelIsBad(1, r, g, b))
                        bIsBad = true;
                }
                else if(badSum == 3){
                    if(pixelIsBad(0, r, g, b) || pixelIsBad(1, r, g, b) || pixelIsBad(2, r, g, b))
                    bIsBad = true;
                }
                break;
            case 2:
                if(badSum == 1){
                    if(pixelIsBad(0, r, g, b) && pixelIsBad(1, r, g, b))
                        bIsBad = true;
                }
                else if(badSum == 2){
                    if((pixelIsBad(0, r, g, b) && pixelIsBad(2, r, g, b))
                            || (pixelIsBad(1, r, g, b) && pixelIsBad(3, r, g, b)))
                        bIsBad = true;
                }
                break;
            case 3:
                if(badSum == 1){
                    if(pixelIsBad(0, r, g, b) && pixelIsBad(1, r, g, b) && pixelIsBad(2, r, g, b))
                        bIsBad = true;
                }
                break;
            default:
                break;
            }

            if(bIsBad)
                pStatus[i*IMAGE_WIDTH+j] = PIXEL_MATTER_BAD;
        }
    }
}

/* 多分类算法下的仿真 */
void AIPreviewWidget::multiAISimulate(uchar *pStatus)
{
    HSVType hsv;
    m_bFlgTest = 1;
    for(int i=0; i<IMAGE_HEIGHT; i++){
        for(int j=0; j<IMAGE_WIDTH; j++){
            uchar r = oriBuf[(i*IMAGE_WIDTH+j)*3];
            uchar g = oriBuf[(i*IMAGE_WIDTH+j)*3+1];
            uchar b = oriBuf[(i*IMAGE_WIDTH+j)*3+2];

            if(myAIShare.pixelIsBackgroundcomm(r, g, b,j))
                continue;

             pStatus[i*IMAGE_WIDTH+j] = PIXEL_MATTER;
            if (struCnfp.nColorSpace) {
                hsv = myAIShare.RGBToHSV((int)r,(int)g,(int)b);
                if(multiPixelIsBad(hsv.x, hsv.y, hsv.z))
                    pStatus[i*IMAGE_WIDTH+j] = PIXEL_MATTER_BAD;
            }
            else {
                if(multiPixelIsBad((int)r, (int)g, (int)b))
                    pStatus[i*IMAGE_WIDTH+j] = PIXEL_MATTER_BAD;
            }
        }
    }
}

/* 显示图片仿真后的效果 */
void AIPreviewWidget::displaySimulate()
{
    uchar *pPixelIsBad = (uchar *)malloc(IMAGE_HEIGHT*IMAGE_WIDTH);

    memset(pPixelIsBad, 0, IMAGE_WIDTH*IMAGE_HEIGHT);

    if(struCnfp.nIntelMode == MODE_USUAL)        // 普通智能算法
        normalAISimulate(pPixelIsBad);
    else if(struCnfp.nIntelMode == MODE_MULTIPLE)   // 多分类智能算法
        multiAISimulate(pPixelIsBad);

    changeImgBySim(pPixelIsBad);
    updateImage();

    free(pPixelIsBad);
}

/* 根据仿真的效果修改显示的图像 */
void AIPreviewWidget::changeImgBySim(uchar *pStatus)
{
    bool bNeedChg;
    int nBadPixelNum;
    int nGroupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
    int nIndex = struGsh.curIntelArithNo;
    int row = struCnfp.struGroupIdentify[struGsh.nLevel][nGroupNum].struIntel[nIndex].nRow;
    int col = struCnfp.struGroupIdentify[struGsh.nLevel][nGroupNum].struIntel[nIndex].nColumn;
    int per = struCnfp.struGroupIdentify[struGsh.nLevel][nGroupNum].struIntel[nIndex].nPercent;

    memcpy(chgBuf, oriBuf, IMAGE_HEIGHT*IMAGE_WIDTH*3);

    /* 根据设置的行列纯度进行仿真 */
    if(m_bIsPurSim) {
        for(int i=0; i<IMAGE_HEIGHT; i++) {
            for(int j=0; j<IMAGE_WIDTH; j++) {
                if(pStatus[i*IMAGE_WIDTH+j] != PIXEL_BACKGROUND) {
                    bNeedChg = true;
                    nBadPixelNum = 0;

                    int nRowBegin = i-(row-1)/2-(row-1)%2;
                    int nRowEnd = i+(row-1)/2;
                    int nColBegin = j-(col-1)/2-(col-1)%2;
                    int nColEnd = j+(col-1)/2;

                    if(nColBegin < 0)
                        nColBegin = 0;
                    if(nColEnd > IMAGE_WIDTH-1)
                        nColEnd = IMAGE_HEIGHT-1;
                    if(nRowBegin < 0)
                        nRowBegin = 0;
                    if(nRowEnd > IMAGE_HEIGHT-1)
                        nRowEnd = IMAGE_HEIGHT-1;

                    for(int m=nRowBegin; m<=nRowEnd; m++) {
                        for(int n=nColBegin; n<=nColEnd; n++) {
                            if(pStatus[m*IMAGE_WIDTH+n] == PIXEL_MATTER_BAD)
                                nBadPixelNum++;
                        }//end n
                    }//end m

                    if(nBadPixelNum <= per)
                        bNeedChg = false;

                    if(bNeedChg) {
                        chgBuf[(i*IMAGE_WIDTH+j)*3] = aloneParams[m_nImgIndex].displayColor.red();
                        chgBuf[(i*IMAGE_WIDTH+j)*3+1] = aloneParams[m_nImgIndex].displayColor.green();
                        chgBuf[(i*IMAGE_WIDTH+j)*3+2] = aloneParams[m_nImgIndex].displayColor.blue();
                    }
                }
            }//end j
        }//end i
    }//end if
    /* 不考虑行列纯度进行仿真 */
    else {
        for(int i=0; i<IMAGE_HEIGHT; i++) {
            for(int j=0; j<IMAGE_WIDTH; j++) {
                if(pStatus[i*IMAGE_WIDTH+j] == PIXEL_MATTER_BAD) {
                    chgBuf[(i*IMAGE_WIDTH+j)*3] = aloneParams[m_nImgIndex].displayColor.red();
                    chgBuf[(i*IMAGE_WIDTH+j)*3+1] = aloneParams[m_nImgIndex].displayColor.green();
                    chgBuf[(i*IMAGE_WIDTH+j)*3+2] = aloneParams[m_nImgIndex].displayColor.blue();
                }//end if
            }//end j
        }//end i
    }//end else
}

/* 重置高亮显示的颜色 */
void AIPreviewWidget::resetDisplayColor()
{
    switch(struGsh.nIntelSampColor[m_nImgIndex]){
    case COLOR_RED:
    case COLOR_RED_GREEN:
        aloneParams[m_nImgIndex].displayColor.setRed(struGsh.nIntelSampSens[m_nImgIndex]);
        aloneParams[m_nImgIndex].displayColor.setGreen(0);
        aloneParams[m_nImgIndex].displayColor.setBlue(255);
        break;
    case COLOR_GREEN:
    case COLOR_RED_BLUE:
        aloneParams[m_nImgIndex].displayColor.setRed(255);
        aloneParams[m_nImgIndex].displayColor.setGreen(struGsh.nIntelSampSens[m_nImgIndex]);
        aloneParams[m_nImgIndex].displayColor.setBlue(0);
        break;
    case COLOR_BLUE:
    case COLOR_GREEN_BLUE:
        aloneParams[m_nImgIndex].displayColor.setRed(255);
        aloneParams[m_nImgIndex].displayColor.setGreen(0);
        aloneParams[m_nImgIndex].displayColor.setBlue(struGsh.nIntelSampSens[m_nImgIndex]);
        break;
    default:
        break;
    }
}

/* 绘制手选的框 */
void AIPreviewWidget::drawSelRect()
{
    uchar upLineBuf[IMAGE_WIDTH*3];
    uchar DownLineBuf[IMAGE_WIDTH*3];
    uchar leftLineBuf[IMAGE_HEIGHT*3];
    uchar rightLineBuf[IMAGE_HEIGHT*3];

    // 将框选的数据拷贝到缓冲区中
    memcpy(upLineBuf, disBuf+(m_nDisStartY*DISPLAY_WIDTH+m_nDisStartX)*3, (m_nDisEndX-m_nDisStartX+1)*3);
    memcpy(DownLineBuf, disBuf+(m_nDisEndY*DISPLAY_WIDTH+m_nDisStartX)*3, (m_nDisEndX-m_nDisStartX+1)*3);

    int lineIndex, disbufIndex;
    for(int i=m_nDisStartY; i<m_nDisEndY+1; i++)
    {
        lineIndex = (i-m_nDisStartY)*3;
        disbufIndex = (i*DISPLAY_WIDTH+m_nDisStartX)*3;
        leftLineBuf[lineIndex] = disBuf[disbufIndex];
        leftLineBuf[lineIndex+1] = disBuf[disbufIndex+1];
        leftLineBuf[lineIndex+2] = disBuf[disbufIndex+2];
        disbufIndex = (i*DISPLAY_WIDTH+m_nDisEndX)*3;
        rightLineBuf[lineIndex] = disBuf[disbufIndex];
        rightLineBuf[lineIndex+1] = disBuf[disbufIndex+1];
        rightLineBuf[lineIndex+2] = disBuf[disbufIndex+2];
    }

    // 实现框选后显示外部框
    for(int i=m_nDisStartX; i<m_nDisEndX+1; i++)
    {
        disbufIndex = (m_nDisStartY*DISPLAY_WIDTH+i)*3;
        disBuf[disbufIndex] = 255;
        disBuf[disbufIndex+1] = 255;
        disBuf[disbufIndex+2] = 255;
        disbufIndex = (m_nDisEndY*DISPLAY_WIDTH+i)*3;
        disBuf[disbufIndex] = 255;
        disBuf[disbufIndex+1] = 255;
        disBuf[disbufIndex+2] = 255;
    }

    for(int i=m_nDisStartY; i<m_nDisEndY+1; i++)
    {
        disbufIndex = (i*DISPLAY_WIDTH+m_nDisStartX)*3;
        disBuf[disbufIndex] = 255;
        disBuf[disbufIndex+1] = 255;
        disBuf[disbufIndex+2] = 255;
        disbufIndex = (i*DISPLAY_WIDTH+m_nDisEndX)*3;
        disBuf[disbufIndex] = 255;
        disBuf[disbufIndex+1] = 255;
        disBuf[disbufIndex+2] = 255;
    }

    // 刷新界面上的显示
    QImage img(disBuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, QImage::Format_RGB888);
    m_imgLabel->setPixmap(QPixmap::fromImage(img));

    // 取消框选后显示的外部框
    memcpy(disBuf+(m_nDisStartY*DISPLAY_WIDTH+m_nDisStartX)*3, upLineBuf, (m_nDisEndX-m_nDisStartX+1)*3);
    memcpy(disBuf+(m_nDisEndY*DISPLAY_WIDTH+m_nDisStartX)*3, DownLineBuf, (m_nDisEndX-m_nDisStartX+1)*3);
    for(int i=m_nDisStartY; i<m_nDisEndY+1; i++)
    {
        disbufIndex = (i*DISPLAY_WIDTH+m_nDisStartX)*3;
        lineIndex = (i-m_nDisStartY)*3;
        disBuf[disbufIndex] = leftLineBuf[lineIndex];
        disBuf[disbufIndex+1] = leftLineBuf[lineIndex+1];
        disBuf[disbufIndex+2] = leftLineBuf[lineIndex+2];

        disbufIndex = (i*DISPLAY_WIDTH+m_nDisEndX)*3;
        lineIndex = (i-m_nDisStartY)*3;
        disBuf[disbufIndex] = rightLineBuf[lineIndex];
        disBuf[disbufIndex+1] = rightLineBuf[lineIndex+1];
        disBuf[disbufIndex+2] = rightLineBuf[lineIndex+2];
    }
}

/* 从文件中获取已选物料点的坐标信息 */
void AIPreviewWidget::getSelPointsFromFile()
{
    if(aloneParams[m_nImgIndex].nSelPixelSum == 0)
        return;

    QString fileName = QString("/tmp/%1").arg(m_nImgIndex);
    QFile file(fileName);
    QTextStream out(&file);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug("%s : File open error!", fileName.toLatin1().data());
        return;
    }

    int tmpX, tmpY;
    for (int i = 0; i < aloneParams[m_nImgIndex].nSelPixelSum; i++) {
        out>>tmpX;
        out>>tmpY;

        uniParams.selectedPoint[i].setX(tmpX);
        uniParams.selectedPoint[i].setY(tmpY);
    }

    file.close();
}

/* 在退出时将选择的物料点保存到文件中 */
void AIPreviewWidget::saveSelPointsToFile()
{
    if (aloneParams[m_nImgIndex].nSelPixelSum == 0) {
        return;
    }

    QString fileName = QString("/tmp/%1").arg(m_nImgIndex);
    QFile file(fileName);
    QTextStream in(&file);

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        qDebug("%s : File open error!", fileName.toLatin1().data());
        return;
    }

    for (int i = 0; i < aloneParams[m_nImgIndex].nSelPixelSum; i++) {
        int x = uniParams.selectedPoint[i].x();
        int y = uniParams.selectedPoint[i].y();
        in<<x<<"\t"<<y<<"\n";
    }

    file.close();
}

/* 智能算法参数复制设别组号设置 */
int AIPreviewWidget::getUnitGroup(int level, int unit)
{
    int group = -1;

    for (int i = 0; i < MAX_GROUP_IDTNTIFY; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitCount; j++) {
            if (unit == struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitId[j]) {
                group = i;
            }
        }
    }

    return group;
}

/* 判断像素点是否为物料点 */
bool AIPreviewWidget::pixelIsMatter(uchar r, uchar g, uchar b, int pixelIndex)
{
    if(myAIShare.pixelIsBackgroundcomm(r,g,b,pixelIndex))
        return false;

    switch(struGsh.nIntelSampColor[m_nImgIndex]){
    case COLOR_RED:         //! 红
        if(struGsh.nIntelSampMode[m_nImgIndex] == 0){  //选暗
            if(r > struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        } else {   //选亮
            if(r < struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        }
        break;
    case COLOR_GREEN:       //! 绿
        if(struGsh.nIntelSampMode[m_nImgIndex] == 0){  //选暗
            if(g > struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        } else {   //选亮
            if(g < struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        }
        break;
    case COLOR_BLUE:        //! 蓝
        if(struGsh.nIntelSampMode[m_nImgIndex] == 0){  //选暗
            if(b > struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        } else {   //选亮
            if(b < struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        }
        break;
    case COLOR_RED_GREEN:   //! 红绿色差
        if (r+g <= 0) {
            return false;
        }
        if (struGsh.nIntelSampMode[m_nImgIndex] == 0) { // 选暗
            if ((255.0*r/(r+g)) > struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        } else { // 选亮
            if ((255.0*r/(r+g)) < struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        }
        break;
    case COLOR_RED_BLUE:    //! 红蓝色差
        if (r+b <= 0) {
            return false;
        }
        if (struGsh.nIntelSampMode[m_nImgIndex] == 0) { // 选暗
            if ((255.0*r/(r+b)) > struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        } else { // 选亮
            if ((255.0*r/(r+b)) < struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        }
        break;
    case COLOR_GREEN_BLUE:  //! 绿蓝色差
        if (g+b <= 0) {
            return false;
        }
        if (struGsh.nIntelSampMode[m_nImgIndex] == 0) { // 选暗
            if ((255.0*g/(g+b)) > struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        } else { // 选亮
            if ((255.0*g/(g+b)) < struGsh.nIntelSampSens[m_nImgIndex])
                return false;
        }
        break;
    default:
        break;
    }

    return true;
}

/* 判断图像中像素点为原始数据点还是被标记的点 */
bool AIPreviewWidget::pixelIsSelected(uchar r, uchar g, uchar b)
{
    if(r==aloneParams[m_nImgIndex].displayColor.red() && g==aloneParams[m_nImgIndex].displayColor.green()
            && b==aloneParams[m_nImgIndex].displayColor.blue())
        return true;
    return false;
}

/* 优化取样时判断像素点是否为边缘点 */
bool AIPreviewWidget::pixelIsEdge(int x, int y)
{
    if(x==0 || y==0 || x==IMAGE_WIDTH-1 || y==IMAGE_HEIGHT-1)
        return false;

    uchar r, g, b;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            if(i!=0 || j!=0){
                r = oriBuf[((y+j)*IMAGE_WIDTH+(x+i))*3];
                g = oriBuf[((y+j)*IMAGE_WIDTH+(x+i))*3+1];
                b = oriBuf[((y+j)*IMAGE_WIDTH+(x+i))*3+2];
                if(myAIShare.pixelIsBackgroundcomm(r, g, b,(x+i)))
                    return true;
            }
        }
    }
    return false;
}

/* 根据选定的方程判断像素点是否为坏点 */
bool AIPreviewWidget::pixelIsBad(int index, uchar r, uchar g, uchar b)
{
    int groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
    int W, X, Y, Z;
    int nSens = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nSens;

    // 常数项（需根据灵敏度进行修正）
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nCons[0] == 0){
        if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nSens > 100)
            W = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nCons[1]*(1-(nSens-100)/100.0);
        else
            W = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nCons[1]*(1+(100-nSens)/100.0);
    }
    else{
        if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nSens > 100)
            W = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nCons[1]*(1+(nSens-100)/100.0);
        else
            W = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nCons[1]*(1-(100-nSens)/100.0);
    }

    // 系数1
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo1[0] == 0)
        X = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo1[1];
    else
        X = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo1[1];

    // 系数2
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo2[0] == 0)
        Y = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo2[1];
    else
        Y = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo2[1];

    // 系数3
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo3[0] == 0)
        Z = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo3[1];
    else
        Z = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[index].nQuo3[1];

    if(W+X*r+Y*g+Z*b < 0)
        return true;

    return false;
}

/* 多分类智能算法下判断像素点是否为坏点 */
bool AIPreviewWidget::multiPixelIsBad(int r, int g, int b)
{
    uint rg = r*g;
    uint rb = r*b;
    uint gb = g*b;
    uint rr = r*r;
    uint gg = g*g;
    uint bb = b*b;

    int	indexOfArithIntel = struGsh.curIntelArithNo;

    int nCons, nQuo0, nQuo1, nQuo2, nQuo3;
    int nQuo4, nQuo5, nQuo6, nQuo7, nQuo8;
    int groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
    int nSens = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nSens;
    int nConsAdj;
    if ((abs(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]) < 200000)&&(abs(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]) != 255)){
        nConsAdj = 200000;
    }
    else {
        nConsAdj = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1];
    }
    // 常数项
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nModeDfl == 0){    // 正选
        if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[0] == 0){
            if(nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0;
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0;
        }
        else{
            if(nSens > 100)
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0);
            else
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0);
        }
    }
    else{       // 反选
        if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[0] == 0){
            if(nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0;
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0;
        }
        else{
            if(nSens > 100)
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]-nConsAdj*struCnfg.nMaterPer3/10.0*(nSens-100)/100.0);
            else
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nConsDfl[1]+nConsAdj*struCnfg.nMaterPer3/10.0*(100-nSens)/100.0);
        }
    }

    if (m_bFlgTest) {
        m_bFlgTest = 0;
        qDebug()<<nCons;
    }
    // 系数1
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoR[0] == 0)
        nQuo0 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoR[1];
    else
        nQuo0 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoR[1];

    // 系数2
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoG[0] == 0)
        nQuo1 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoG[1];
    else
        nQuo1 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoG[1];

    // 系数3
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoB[0] == 0)
        nQuo2 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoB[1];
    else
        nQuo2 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoB[1];

    // 系数4
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRG[0] == 0)
        nQuo3 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRG[1];
    else
        nQuo3 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRG[1];

    // 系数5
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRB[0] == 0)
        nQuo4 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRB[1];
    else
        nQuo4 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRB[1];

    // 系数6
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGB[0] == 0)
        nQuo5 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGB[1];
    else
        nQuo5 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGB[1];

    // 系数7
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRR[0] == 0)
        nQuo6 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRR[1];
    else
        nQuo6 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoRR[1];

    // 系数8
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGG[0] == 0)
        nQuo7 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGG[1];
    else
        nQuo7 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoGG[1];

    // 系数9
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoBB[0] == 0)
        nQuo8 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoBB[1];
    else
        nQuo8 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nQuoBB[1];

    int value = nCons+nQuo0*r+nQuo1*g+nQuo2*b+nQuo3*rg+nQuo4*rb+nQuo5*gb+nQuo6*rr+nQuo7*gg+nQuo8*bb;
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[indexOfArithIntel].nModeDfl == 0){     // 正选
        if(value < 0)
            return true;
        else
            return false;
    }
    else{       // 反选
        if(value > 0)
            return true;
        else
            return false;
    }
}

/* 响应鼠标按下事件 */
void AIPreviewWidget::mousePressEvent(QMouseEvent *event)
{
    int tmpX = event->pos().x()-m_nDistanceX;
    int tmpY = event->pos().y()-m_nDistanceY;

    if(tmpX<0 || tmpX>=m_imgLabel->width() || tmpY<0 || tmpY>=m_imgLabel->height())
        return;

    m_nDisStartX = tmpX;
    m_nDisStartY = tmpY;
    QString str;
    uchar r, g, b;

    int x = m_nDisStartX/uniParams.nScale+uniParams.nIndex%(uniParams.nScale*2)*uniParams.nSpaceX;
    int y = m_nDisStartY/uniParams.nScale+uniParams.nIndex/(uniParams.nScale*2)*uniParams.nSpaceY;

    r = oriBuf[(y*IMAGE_WIDTH+x)*3];
    g = oriBuf[(y*IMAGE_WIDTH+x)*3+1];
    b = oriBuf[(y*IMAGE_WIDTH+x)*3+2];
    int max = 0;
    int min = 256;
    if (r > max){
        max = r;
    }
    if (r < min) {
        min = r;
    }
    if (g > max){
        max = g;
    }
    if (g < min){
        min = g;
    }
    if (b > max){
        max = b;
    }
    if (b < min) {
        min = b;
    }
    if (max != 0) {
        str = QString("x=%1,y=%2  R:%3,G:%4,B:%5,RS:%6").arg(m_nDisStartX).arg(m_nDisStartY).arg(r).arg(g).arg(b).arg((max-min)*255/max);
    }
    else {
        str = QString("x=%1,y=%2  R:%3,G:%4,B:%5").arg(m_nDisStartX).arg(m_nDisStartY).arg(r).arg(g).arg(b);
    }
    m_pixelColorLabel->setText(str);
}


/* 响应鼠标移动事件 */
void AIPreviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(myAIShare.m_nMatStatus[m_nImgIndex]==CLASS_NULL || m_bIsSimulate)
        return;

    m_nDisEndX = event->pos().x()-m_nDistanceX;
    m_nDisEndY = event->pos().y()-m_nDistanceY;

    if(m_nDisEndX<0 || m_nDisEndX>=m_imgLabel->width() || m_nDisEndY<0 || m_nDisEndY>=m_imgLabel->height())
        return;

    if(m_nDisStartX>m_nDisEndX || m_nDisStartY>m_nDisEndY)
        return;

    if(aloneParams[m_nImgIndex].nSelTimes >= MAX_SELECT_TIMES){
        infoWidget->setLabelText(myLan.ai_exceed_max_times);
        infoWidget->showSecs();
        return;
    }

    // 实现选中后取消选中
    if(uniParams.bHasTar)
    {
        int disbufIndex;
        int chgBufIndex;

        for(int i=0; i<m_nDisPixelNum; i++)
        {
            int x = m_disPoints[i].x();
            int y = m_disPoints[i].y();
            disbufIndex = ((m_nDisStartY-m_nDisStartY%uniParams.nScale+y*uniParams.nScale)*DISPLAY_WIDTH
                           +m_nDisStartX-m_nDisStartX%uniParams.nScale+x*uniParams.nScale)*3;
            chgBufIndex = ((m_nOriStartY+y)*IMAGE_WIDTH+m_nOriStartX+x)*3;
            disBuf[disbufIndex] = chgBuf[chgBufIndex];
            disBuf[disbufIndex+1] = chgBuf[chgBufIndex+1];
            disBuf[disbufIndex+2] = chgBuf[chgBufIndex+2];
        }

        if(uniParams.nScale > 1)
        {
            for(int i=0; i<m_nDisPixelNum; i++)
            {
                int x = m_disPoints[i].x();
                int y = m_disPoints[i].y();
                for(int k=0; k<uniParams.nScale; k++)
                {
                    for(int l=0; l<uniParams.nScale; l++)
                    {
                        if(k!=0 || l!=0){
                            disbufIndex = ((m_nDisStartY-m_nDisStartY%uniParams.nScale+y*uniParams.nScale+k)*DISPLAY_WIDTH
                                           +m_nDisStartX-m_nDisStartX%uniParams.nScale+x*uniParams.nScale+l)*3;
                            chgBufIndex = ((m_nOriStartY+y)*IMAGE_WIDTH+m_nOriStartX+x)*3;
                            disBuf[disbufIndex] = chgBuf[chgBufIndex];
                            disBuf[disbufIndex+1] = chgBuf[chgBufIndex+1];
                            disBuf[disbufIndex+2] = chgBuf[chgBufIndex+2];
                        }//end if
                    }//end for(l)
                }//end for(k)
            }
        }

        QImage img(disBuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, QImage::Format_RGB888);
        m_imgLabel->setPixmap(QPixmap::fromImage(img));
    }

    uniParams.bHasTar = false;
    int pixelNum = 0;
    m_nDisPixelNum = 0;

    // 将鼠标移动形成的方框坐标信息映射到原始图片上
    m_nOriStartX = m_nDisStartX/uniParams.nScale+uniParams.nIndex%(uniParams.nScale*2)*uniParams.nSpaceX;
    m_nOriStartY = m_nDisStartY/uniParams.nScale+uniParams.nIndex/(uniParams.nScale*2)*uniParams.nSpaceY;
    m_nOriEndX = m_nDisEndX/uniParams.nScale+uniParams.nIndex%(uniParams.nScale*2)*uniParams.nSpaceX;
    m_nOriEndY = m_nDisEndY/uniParams.nScale+uniParams.nIndex/(uniParams.nScale*2)*uniParams.nSpaceY;
    uniParams.oriRect = QRect(QPoint(m_nOriStartX,m_nOriStartY),QPoint(m_nOriEndX,m_nOriEndY));

    aloneParams[m_nImgIndex].nSelPixelSum = 0;
    for(int i=0; i<aloneParams[m_nImgIndex].nSelTimes; i++){
        aloneParams[m_nImgIndex].nSelPixelSum += aloneParams[m_nImgIndex].nSelPixelNum[i];
    }
    aloneParams[m_nImgIndex].nDisPixelSum = 0;
    for(int i=0; i<aloneParams[m_nImgIndex].nSelTimes; i++){
        aloneParams[m_nImgIndex].nDisPixelSum += aloneParams[m_nImgIndex].nDisPixelNum[i];
    }

    // 利用预先设置好的阈值筛选出需要的物料像素点
    int tmpWid = uniParams.oriRect.width();
    int tmpHei = uniParams.oriRect.height();
    for(int i=0; i<tmpHei; i++)
    {
        for(int j=0; j<tmpWid; j++)
        {
            int x = m_nOriStartX+j;
            int y = m_nOriStartY+i;
            uchar r = chgBuf[(y*IMAGE_WIDTH+x)*3];
            uchar g = chgBuf[(y*IMAGE_WIDTH+x)*3+1];
            uchar b = chgBuf[(y*IMAGE_WIDTH+x)*3+2];

            if(!pixelIsMatter(r,g,b,x) || pixelIsSelected(r,g,b))
                continue;

            if(aloneParams[m_nImgIndex].nSelPixelSum >= MAX_PIXEL_NUM)
                continue;

            uniParams.selectedPoint[aloneParams[m_nImgIndex].nSelPixelSum] = QPoint(x, y);
            pixelNum++;
            aloneParams[m_nImgIndex].nSelPixelSum++;

            if(myAIShare.m_bIsOptSam[m_nImgIndex]){
                if(pixelIsEdge(x, y))
                    continue;
            }

            uniParams.bHasTar = true;
            m_disPoints[m_nDisPixelNum] = QPoint(j,i);
            m_nDisPixelNum++;
            aloneParams[m_nImgIndex].nDisPixelSum++;
        }
    }

    aloneParams[m_nImgIndex].nSelPixelNum[aloneParams[m_nImgIndex].nSelTimes] = pixelNum;
    aloneParams[m_nImgIndex].nDisPixelNum[aloneParams[m_nImgIndex].nSelTimes] = m_nDisPixelNum;
    refreshMatStatusLabel();

    // 将数据根据一定的缩放比列显示出来
    if(uniParams.bHasTar)
    {
        int disbufIndex;
        for(int i=0; i<m_nDisPixelNum; i++)
        {
            int x = m_disPoints[i].x();
            int y = m_disPoints[i].y();
            disbufIndex = ((m_nDisStartY-m_nDisStartY%uniParams.nScale+y*uniParams.nScale)*DISPLAY_WIDTH
                           +m_nDisStartX-m_nDisStartX%uniParams.nScale+x*uniParams.nScale)*3;
            disBuf[disbufIndex] = aloneParams[m_nImgIndex].displayColor.red();
            disBuf[disbufIndex+1] = aloneParams[m_nImgIndex].displayColor.green();
            disBuf[disbufIndex+2] = aloneParams[m_nImgIndex].displayColor.blue();
        }

        if(uniParams.nScale > 1)
        {
            for(int i=0; i<m_nDisPixelNum; i++)
            {
                int x = m_disPoints[i].x();
                int y = m_disPoints[i].y();
                for(int k=0; k<uniParams.nScale; k++)
                {
                    for(int l=0; l<uniParams.nScale; l++)
                    {
                        if(k!=0 || l!=0)
                        {
                            disbufIndex = ((m_nDisStartY-m_nDisStartY%uniParams.nScale+y*uniParams.nScale+k)*DISPLAY_WIDTH
                                           +m_nDisStartX-m_nDisStartX%uniParams.nScale+x*uniParams.nScale+l)*3;
                            disBuf[disbufIndex] = aloneParams[m_nImgIndex].displayColor.red();
                            disBuf[disbufIndex+1] = aloneParams[m_nImgIndex].displayColor.green();
                            disBuf[disbufIndex+2] = aloneParams[m_nImgIndex].displayColor.blue();
                        }//end if
                    }//end for(l)
                }//end for(k)
            }
        }
    }

    // 绘制手选的框
    drawSelRect();
}

/* 响应鼠标释放事件 */
void AIPreviewWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(myAIShare.m_nMatStatus[m_nImgIndex]==CLASS_NULL || m_bIsSimulate)
        return;

    if(aloneParams[m_nImgIndex].nSelTimes >= MAX_SELECT_TIMES)
        return;

    if(uniParams.bHasTar){
        aloneParams[m_nImgIndex].nSelTimes++;
        m_relastBtn->setEnabled(true);
    }

    /*----- 刷新界面上的显示 -----*/
    QImage img(disBuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, QImage::Format_RGB888);
    m_imgLabel->setPixmap(QPixmap::fromImage(img));

    uniParams.bHasTar = false;
    changeImage();
}

/* 进入图片预览页面时刷新页面显示状态 */
void AIPreviewWidget::refreshDisplaySlt()
{
    m_nImgIndex = myAIShare.m_nImgIndex;
    initAIUniParams();
    resetDisplayColor();

    getDataAndShow();
    refreshAIParamsDisplay();
}

/* 响应智能主页面删除图像的槽函数 */
void AIPreviewWidget::onDelCurImgSlt()
{
    m_nImgIndex = myAIShare.m_nImgIndex;
    for(int i=myAIShare.m_nImgIndex+1; i<IMG_MAX_NUM; i++)
    {
        /* 将保存已选像素点位置的文件重命名 */
        QFile tmpFile(QString("/tmp/%1").arg(i));

        if (tmpFile.exists()) {
            char cmd[64];
            sprintf(cmd, "mv /tmp/%d /tmp/%d", i, i-1);
            system(cmd);
        }

        /* 将保存已选像素点的信息重新分配 */
        aloneParams[i-1].nSelTimes = aloneParams[i].nSelTimes;
        aloneParams[i-1].nDisPixelSum = aloneParams[i].nDisPixelSum;
        aloneParams[i-1].nSelPixelSum = aloneParams[i].nSelPixelSum;
        struGsh.nIntelSampColor[i-1] = struGsh.nIntelSampColor[i];
        struGsh.nIntelSampMode[i-1] = struGsh.nIntelSampMode[i];
        struGsh.nIntelSampSens[i-1] = struGsh.nIntelSampSens[i];
        for(int j=0; j<aloneParams[i].nSelTimes; j++)
        {
            aloneParams[i-1].nDisPixelNum[j] = aloneParams[i].nDisPixelNum[j];
            aloneParams[i-1].nSelPixelNum[j] = aloneParams[i].nDisPixelNum[j];
        }
    }

    aloneParams[IMG_MAX_NUM-1].nSelTimes = 0;
    aloneParams[IMG_MAX_NUM-1].nDisPixelSum = 0;
    aloneParams[IMG_MAX_NUM-1].nSelPixelSum = 0;
    struGsh.nIntelSampColor[IMG_MAX_NUM-1] = 1;
    struGsh.nIntelSampMode[IMG_MAX_NUM-1] = 0;
    struGsh.nIntelSampSens[IMG_MAX_NUM-1] = 255;
    resetDisplayColor();
}
/* 响应智能主页面删除全部图像的槽函数 */
void AIPreviewWidget::onDelAllImgSlt()
{
    for(int i = 0; i < IMG_MAX_NUM;i++) {
        aloneParams[i].nSelTimes = 0;
        aloneParams[i].nDisPixelSum = 0;
        aloneParams[i].nSelPixelSum = 0;
        struGsh.nIntelSampColor[i] = 1;
        struGsh.nIntelSampMode[i] = 0;
        struGsh.nIntelSampSens[i] = 255;
    }
}

/* 响应图片预览页面中的重选上次按钮 */
void AIPreviewWidget::onReLastBtnClicked()
{
    aloneParams[m_nImgIndex].nSelTimes--;
    if(aloneParams[m_nImgIndex].nSelTimes < 1)
        m_relastBtn->setEnabled(false);

    int tmpSum = aloneParams[m_nImgIndex].nSelPixelSum;
    int index;
    aloneParams[m_nImgIndex].nSelPixelSum -= aloneParams[m_nImgIndex].nSelPixelNum[aloneParams[m_nImgIndex].nSelTimes];
    aloneParams[m_nImgIndex].nDisPixelSum -= aloneParams[m_nImgIndex].nDisPixelNum[aloneParams[m_nImgIndex].nSelTimes];
    for(int i=aloneParams[m_nImgIndex].nSelPixelSum; i<tmpSum; i++){
        int x = uniParams.selectedPoint[i].x();
        int y = uniParams.selectedPoint[i].y();
        index = (y*IMAGE_WIDTH+x)*3;
        memcpy(chgBuf+index, oriBuf+index, 3);
    }

    updateImage();
    refreshMatStatusLabel();
}

/* 响应图片预览页面中的优化样本按钮 */
void AIPreviewWidget::onOptSamBtnClicked()
{
    myAIShare.m_bIsOptSam[m_nImgIndex] = !myAIShare.m_bIsOptSam[m_nImgIndex];
    refreshImageDisplay();
    refreshBtnStatus();
    refreshMatStatusLabel();
}

/* 响应图片预览页面中的返回按钮 */
void AIPreviewWidget::onBackBtnClicked()
{
    myAIShare.m_nSelPixelNum[m_nImgIndex] = aloneParams[m_nImgIndex].nSelPixelSum;
    myAIShare.m_nDisPixelNum[m_nImgIndex] = aloneParams[m_nImgIndex].nDisPixelSum;

    saveSelPointsToFile();
    emit backToAIMainPageSig();
}

/* 响应图片预览页面中的上移按钮 */
void AIPreviewWidget::onUpBtnClicked()
{
    uniParams.nVMoveTimes--;
    uniParams.nIndex -= 2*uniParams.nScale;

    refreshBtnStatus();
    updateImage();
}

/* 响应图片预览页面中的下移按钮 */
void AIPreviewWidget::onDownBtnClicked()
{
    uniParams.nVMoveTimes++;
    uniParams.nIndex += 2*uniParams.nScale;

    refreshBtnStatus();
    updateImage();
}

/* 响应图片预览页面中的左移按钮 */
void AIPreviewWidget::onLeftBtnClicked()
{
    uniParams.nHMoveTimes--;
    uniParams.nIndex--;

    refreshBtnStatus();
    updateImage();
}

/* 响应图片预览页面中的右移按钮 */
void AIPreviewWidget::onRightBtnClicked()
{
    uniParams.nHMoveTimes++;
    uniParams.nIndex++;

    refreshBtnStatus();
    updateImage();
}

/* 响应图片预览页面中的放大按钮 */
void AIPreviewWidget::onEnlargeBtnClicked()
{
    uniParams.nScale = uniParams.nScale*2;
    uniParams.nMaxMoveTimes = 2*uniParams.nScale-1;
    uniParams.nHMoveTimes *= 2;
    uniParams.nVMoveTimes *= 2;

    switch(uniParams.nScale){
    case 2:
        uniParams.nIndex = oneToTwo[uniParams.nIndex];
        break;
    case 4:
        uniParams.nIndex = twoToFour[uniParams.nIndex];
        break;
    case 8:
        uniParams.nIndex = fourToEight[uniParams.nIndex];
        break;
    default:
        break;
    }

    refreshBtnStatus();
    updateImage();
}

/* 响应图片预览页面中的缩小按钮 */
void AIPreviewWidget::onReduceBtnClicked()
{
    uniParams.nScale = uniParams.nScale/2;
    uniParams.nMaxMoveTimes = 2*uniParams.nScale-1;
    uniParams.nHMoveTimes /= 2;
    uniParams.nVMoveTimes /= 2;

    switch(uniParams.nScale){
    case 1:
        uniParams.nIndex = twoToOne[uniParams.nIndex];
        break;
    case 2:
        uniParams.nIndex = fourToTwo[uniParams.nIndex];
        break;
    case 4:
        uniParams.nIndex = eightToFour[uniParams.nIndex];
        break;
    default:
        break;
    }

    refreshBtnStatus();
    updateImage();
}

/* 响应图片预览页面中的仿真按钮 */
void AIPreviewWidget::onSimulateBtnClicked()
{
    m_bIsSimulate = !m_bIsSimulate;

    if (m_bIsSimulate) {
        displaySimulate();
    } else {
	    memcpy(chgBuf, oriBuf, IMAGE_WIDTH*IMAGE_HEIGHT*3);
        refreshImageDisplay();
    }

    refreshBtnStatus();
}

/* 响应图片预览页面中的启用纯度按钮 */
void AIPreviewWidget::onStartPurSimBtnClicked()
{
    m_bIsPurSim = !m_bIsPurSim;
    refreshBtnStatus();

    displaySimulate();
}

/* 响应图片预览页面中的红外模式按钮 */
void AIPreviewWidget::onInfModeBtnClicked()
{
    QString fileName = QString("%1.png").arg(m_nImgIndex);
    getImageData(fileName);
    refreshImageDisplay();

    refreshBtnStatus();
    refreshMatStatusLabel();
}

/* 响应图片预览页面中的取样颜色按钮 */
void AIPreviewWidget::onSamColorBtnClicked()
{
    switch(struGsh.nIntelSampColor[m_nImgIndex])
    {
    case COLOR_RED:     //! 红
        struGsh.nIntelSampColor[m_nImgIndex] = COLOR_GREEN;
        m_samColorBtn->setText(myLan.green);
        break;
    case COLOR_GREEN:   //! 绿
        struGsh.nIntelSampColor[m_nImgIndex] = COLOR_BLUE;
        m_samColorBtn->setText(myLan.blue);
        break;
    case COLOR_BLUE:    //! 蓝
        struGsh.nIntelSampColor[m_nImgIndex] = COLOR_RED_GREEN;
        m_samColorBtn->setText(myLan.red_green);
        break;
    case COLOR_RED_GREEN:   //! 红绿
        struGsh.nIntelSampColor[m_nImgIndex] = COLOR_RED_BLUE;
        m_samColorBtn->setText(myLan.red_blue);
        break;
    case COLOR_RED_BLUE:    //! 红蓝
        struGsh.nIntelSampColor[m_nImgIndex] = COLOR_GREEN_BLUE;
        m_samColorBtn->setText(myLan.green_blue);
        break;
    case COLOR_GREEN_BLUE:  //! 绿蓝
        struGsh.nIntelSampColor[m_nImgIndex] = COLOR_RED;
        m_samColorBtn->setText(myLan.red);
        break;
    default:
        break;
    }

    resetDisplayColor();
    refreshImageDisplay();
}

/* 响应图片预览页面中的取样模式按钮 */
void AIPreviewWidget::onSamModeBtnClicked()
{
    if (struGsh.nIntelSampMode[m_nImgIndex] == 0) {
        struGsh.nIntelSampMode[m_nImgIndex] = 1;
        m_samModeBtn->setText(myLan.light);
    } else if (struGsh.nIntelSampMode[m_nImgIndex] == 1) {
        struGsh.nIntelSampMode[m_nImgIndex] = 0;
        m_samModeBtn->setText(myLan.dark);
    }
}

/* 响应图片预览页面中的取样灵敏度按钮 */
void AIPreviewWidget::onSamSensLcdPressed()
{
    int tmp = m_samSensLcd->value();
    myInputPanel inputDlg(intType, 0, 255, tmp);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        m_samSensLcd->display(inputDlg.getValue());
        struGsh.nIntelSampSens[m_nImgIndex] = m_samSensLcd->value();
    }

    resetDisplayColor();
    refreshImageDisplay();
}

/* 响应图片预览页面中的智能灵敏度参数设置 */
void AIPreviewWidget::onAISensLetPressed()
{
    int group = -1;
    int unit = struGsh.nUnit;
    for(int i=0; i<MAX_GROUP_IDTNTIFY; i++){
        for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitCount; j++){
            if(unit == struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitId[j]) {
                group = i;
            }
        }
    }

    double nSens = 0;
    int ret;
    myInputPanel inputDlg1(intType,1,200,AISensLbe->text().toInt());
    nSens = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nSens;
    inputDlg1.setValue(nSens);

    ret  = inputDlg1.exec();
    if (ret == QDialog::Accepted)
    {
        nSens = inputDlg1.getValue();
        QString str;
        struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nSens = nSens;
        str = QString("%1").arg(nSens);
        AISensLbe->setText(str);

        /*参数发送*/
        myFlow.materialCopyAssemble(struGsh.nLevel      , group, 0, ARITH_INTEL_A+struGsh.curIntelArithNo, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, group, 0, ARITH_INTEL_A+struGsh.curIntelArithNo, 0);
    }
}

/* 响应图片预览页面中的智能行参数设置 */
void AIPreviewWidget::onAIRowLetPressed()
{
    int group = -1;
    int unit = struGsh.nUnit;
    for(int i=0; i<MAX_GROUP_IDTNTIFY; i++){
        for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitCount; j++){
            if(unit == struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitId[j]) {
                group = i;
            }
        }
    }

    double nRow;
    int ret, row, col, per;
    nRow = AIRowLbe->text().toDouble();
    myInputPanel inputDlg2(intType,1,struGsh.maxRowNumber,nRow);
    ret  = inputDlg2.exec();
    if (ret == QDialog::Accepted)
    {
        nRow = inputDlg2.getValue();
        struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nRow    = row = nRow;
        struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nColumn = col = nRow*struGsh.rowColumnRelation;
        per = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nPercent;
        if (per > row * col) {
            per = row * col;
            struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nPercent = per;
        }
        QString str = QString("%1").arg(nRow);
        AIRowLbe->setText(str);

        str = QString("%1").arg(per);
        AIPercentLbe->setText(str);

        /*参数发送*/
        myFlow.materialCopyAssemble(struGsh.nLevel      , group, 0, ARITH_INTEL_A+struGsh.curIntelArithNo, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, group, 0, ARITH_INTEL_A+struGsh.curIntelArithNo, 0);
    }
}

/* 响应图片预览页面中的智能病癍参数设置 */
void AIPreviewWidget::onAIPercentLetPressed()
{
    int group = -1;
    int unit = struGsh.nUnit;
    for(int i=0; i<MAX_GROUP_IDTNTIFY; i++){
        for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitCount; j++){
            if(unit == struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[i].nUnitId[j]) {
                group = i;
            }
        }
    }

    double nPercent;
    int ret, row, col;
    nPercent = AIPercentLbe->text().toDouble();
    row = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nRow;
    col = struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nColumn;
    myInputPanel inputDlg3(intType,1,row*col,nPercent);
    ret  = inputDlg3.exec();
    if (ret == QDialog::Accepted)
    {
        nPercent = inputDlg3.getValue();
        QString str = QString("%1").arg(nPercent);
        AIPercentLbe->setText(str);

        struCnfp.struGroupIdentify[struGsh.nLevel][group].struIntel[struGsh.curIntelArithNo].nPercent = nPercent;
        myFlow.materialCopyAssemble(struGsh.nLevel      , group, 0, ARITH_INTEL_A+struGsh.curIntelArithNo, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, group, 0, ARITH_INTEL_A+struGsh.curIntelArithNo, 0);
    }
}

