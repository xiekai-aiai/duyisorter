/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aianalysis.cpp
 * \brief       彩色机型智能分析界面源文件
 * \date        2015.01.14
 */
#include "aianalysis.h"

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

AIAnalysis::AIAnalysis(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT);

    config = g_Config::getInstance();
    imgListIndex = 0;
    imgPageCur = 0;
    imgListNum = 0;
    imgListCur = 0;

    for (int i = 0; i < IMG_MAX_NUM;i++) {
        imageListName[i] = QString("%1 %2").arg(myLan.ai_material).arg(i);
        imgListStat[i]   = 0;
    }

    initAIParams();
    initMatterParams();

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(0, 0, width(), height());

    createAIMainPage();
    createAISetPage();
    createAISetMatterPage();
    createAICaptureImage();
    m_nProfile = struCnfg.nProfile;

    /* 初始状态下物料不存在对应图片 */
    if (matterIndex != NO_MATTER) {
        imageOpenError();
        lastBtn->setEnabled(false);
    } else {
        refreshBtnStatus();
    }

    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetIndexChangedSlt(int)));
}

AIAnalysis::~AIAnalysis()
{
}

/* 创建智能分析界面 */
void AIAnalysis::createAIMainPage()
{
    AIWidget = new QWidget;
    AIWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *warnLay = new QHBoxLayout;
    QHBoxLayout *midLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(AIWidget);

    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    imageLabel = new QLabel(AIWidget);
    imageLabel->setFixedSize(DISPLAY_WID,DISPLAY_HEI);
    imageLabel->setScaledContents(true);
    imageLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText(myLan.ai_no_image);

    QGroupBox *group = new QGroupBox;
    group->setFixedHeight(DISPLAY_HEI);
    QGridLayout *layout = new QGridLayout(group);

    enlargeBtn = new myPushButton(myIcon.Edit_ZoomIn, btnSize);
    reduceBtn = new myPushButton(myIcon.Edit_ZoomOut, btnSize);
    simulateBtn = new myPushButton(myIcon.Sorter_AI, btnSize);
    setBtn = new myPushButton(myIcon.Action_Configure, btnSize);
    reLastBtn = new myPushButton(myIcon.Arrow_LeftFirst, btnSize);
    optCalBtn = new myPushButton(myIcon.Edit_Search, btnSize);
    optSampleBtn = new myPushButton(myIcon.Arrow_DownFast, btnSize);
    clearSamBtn = new myPushButton(myIcon.Edit_Delete, btnSize);

    infoLabel = new myLabel("");
    infoLabel->setFixedHeight(BTN_HEIGHT);

    layout->addWidget(infoLabel,0,0,1,2,Qt::AlignCenter);
    layout->addWidget(enlargeBtn, 1, 0);
    layout->addWidget(reLastBtn, 1, 1);
    layout->addWidget(reduceBtn, 2, 0);
    layout->addWidget(optCalBtn, 2, 1);
    layout->addWidget(simulateBtn, 3, 0);
    layout->addWidget(optSampleBtn, 3, 1);
    layout->addWidget(setBtn, 4, 0);
    layout->addWidget(clearSamBtn, 4, 1);
    layout->setContentsMargins(0, 0, 0, 0);

    AIWarningLabel = new QLabel(AIWidget);
    AIWarningLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    AIWarningLabel->setAlignment(Qt::AlignCenter);
    AIWarningLabel->setFixedHeight(25);

    btnSize = QSize(BTN_WIDTH, BTN_HEIGHT);
    imgBtn = new myPushButton(myLan.ai_material_image, QIcon());
    imgBtn->setEnabled(false);
    imgBtn->setFixedSize(btnSize);
    matterSetBtn = new myPushButton(myLan.ai_set_material, QIcon());
    matterSetBtn->setFixedSize(btnSize);
    calBtn = new myPushButton(myLan.ai_calculate, QIcon());
    calBtn->setFixedSize(btnSize);

    btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    lastBtn = new myPushButton(myIcon.Arrow_Left, btnSize);
    mesLabel = new QLabel(myLan.msg_set_material);
    mesLabel->setFixedHeight(btnSize.height());
    mesLabel->setMinimumWidth(200);
    mesLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    mesLabel->setAlignment(Qt::AlignCenter);
    nextBtn = new myPushButton(myIcon.Arrow_Right, btnSize);

    btnSize = QSize(BTN_WIDTH, BTN_HEIGHT);
    AIBackBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    AIBackBtn->setFixedSize(btnSize);
    updateBtn = new myPushButton(myLan.ai_update_sample, QIcon());
    updateBtn->setFixedSize(btnSize);

    upLay->setContentsMargins(0, 0, 0, 0);
    upLay->addWidget(imageLabel);
    upLay->addSpacing(10);
    upLay->addWidget(group);
    warnLay->addWidget(AIWarningLabel);
    warnLay->setContentsMargins(0, 0, 0, 0);

    midLay->addWidget(imgBtn);
    midLay->addWidget(matterSetBtn);
    midLay->addWidget(calBtn);
    midLay->addWidget(updateBtn);
    if(LCD_WIDTH == 1024) {
        midLay->setContentsMargins(10, 10, 10, 10);
    }

    downLay->addWidget(lastBtn, Qt::AlignRight);
    downLay->addWidget(mesLabel);
    downLay->addWidget(nextBtn, Qt::AlignLeft);
    downLay->addSpacing(120);
    downLay->addWidget(AIBackBtn);

    mainLay->addLayout(upLay);
    mainLay->addLayout(warnLay);
    mainLay->addLayout(midLay);
    mainLay->addLayout(downLay);

    if(LCD_WIDTH == 640) {
        mainLay->setContentsMargins(0, 0, 10, 10);
    }
    else {
        mainLay->setContentsMargins(10, 10, 10, 10);
    }

    switch(LCD_WIDTH){
    case 640:
        offsetX = 0;
        offsetY = 8;
        break;
    case 800:
        offsetX = 9;
        offsetY = 17;
        break;
    case 1024:
        offsetX = 10;
        offsetY = 17;
        break;
    default:
        break;
    }

    btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    int upX = offsetX+(DISPLAY_WID-btnSize.width())/2;
    int upY = offsetY;
    int downX = upX;
    int downY = offsetY+DISPLAY_HEI-btnSize.height();
    int leftX = offsetX;
    int leftY = offsetY+(DISPLAY_HEI-btnSize.height())/2;
    int rightX = offsetX+DISPLAY_WID-btnSize.width();
    int rightY = leftY;

    upBtn = new myPushButton("", myIcon.Arrow_Up, AIWidget);
    upBtn->setGeometry(QRect(QPoint(upX, upY), btnSize));
    downBtn = new myPushButton("", myIcon.Arrow_Down, AIWidget);
    downBtn->setGeometry(QRect(QPoint(downX, downY), btnSize));
    leftBtn = new myPushButton("", myIcon.Arrow_Left, AIWidget);
    leftBtn->setGeometry(QRect(QPoint(leftX, leftY), btnSize));
    rightBtn = new myPushButton("", myIcon.Arrow_Right, AIWidget);
    rightBtn->setGeometry(QRect(QPoint(rightX, rightY), btnSize));

    connect(reLastBtn, SIGNAL(pressed()), this, SLOT(onReLastBtnClicked()));
    connect(optCalBtn, SIGNAL(pressed()), this, SLOT(onOptCalBtnClicked()));
    connect(optSampleBtn, SIGNAL(pressed()), this, SLOT(onOptSamBtnClicked()));
    connect(matterSetBtn, SIGNAL(pressed()), this, SLOT(onSetMatterBtnClicked()));
    connect(AIBackBtn, SIGNAL(pressed()), this, SLOT(onAIBackBtnClicked()));
    connect(upBtn, SIGNAL(pressed()), this, SLOT(onUpBtnClicked()));
    connect(downBtn, SIGNAL(pressed()), this, SLOT(onDownBtnClicked()));
    connect(leftBtn, SIGNAL(pressed()), this, SLOT(onLeftBtnClicked()));
    connect(rightBtn, SIGNAL(pressed()), this, SLOT(onRightBtnClicked()));
    connect(enlargeBtn, SIGNAL(pressed()), this, SLOT(onEnlargeBtnClicked()));
    connect(reduceBtn, SIGNAL(pressed()), this, SLOT(onReduceBtnClicked()));
    connect(setBtn, SIGNAL(pressed()), this, SLOT(onAISetBtnClicked()));
    connect(imgBtn, SIGNAL(pressed()), this, SLOT(onImgCapBtnClicked()));
    connect(lastBtn, SIGNAL(pressed()), this, SLOT(onLastBtnClicked()));
    connect(nextBtn, SIGNAL(pressed()), this, SLOT(onNextBtnClicked()));
    connect(calBtn, SIGNAL(pressed()), this, SLOT(onAICalBtnClicked()));
    connect(simulateBtn, SIGNAL(pressed()), this, SLOT(onSimulateBtnClicked()));
    connect(updateBtn, SIGNAL(pressed()), this, SLOT(onUpdateBtnClicked()));
    connect(clearSamBtn, SIGNAL(pressed()), this, SLOT(onClearSamBtnClicked()));

    stackedWidget->addWidget(AIWidget);
}

/*创建物料图片页面*/
void AIAnalysis::createAICaptureImage()
{
    AICapImageWidget = new QWidget();
    AICapImageWidget->setGeometry(0,0,width(),height());

    leftGroup = new myGroupBox("", AICapImageWidget);
    rightGroup = new myGroupBox("", AICapImageWidget);

    captureBtn = new myPushButton("", myIcon.Media_Start);
    layerBtn = new myPushButton("", QIcon());
    layerAddBtn = new myPushButton("", QIcon());
    commBtn = new myPushButton(myLan.ai_high_speed, QIcon());
    boardNum = new QLCDNumber(1);
    boardNum->display(1);
    boardNumPlusBtn = new myPushButton("", myIcon.Action_Plus);
    boardNumMinusBtn = new myPushButton("", myIcon.Action_Minus);
    deletCapBtn = new myPushButton("", myIcon.Edit_Delete);
    deriveBtn = new myPushButton("", myIcon.Action_Backup);
    autoStudyBtn = new myPushButton(myLan.ai_auto_cal, QIcon(), false);

    okCapBtn = new myPushButton(myLan.ok, myIcon.Action_Apply);
    preCapBtn = new myPushButton(myLan.last_page, myIcon.Arrow_Up, false);
    nextCapBtn = new myPushButton(myLan.next_page, myIcon.Arrow_Down);
    cancelCapBtn = new myPushButton(myLan.cancel, myIcon.Action_Cancel);

    QSize btnSize = QSize(ICON_WID+16, ICON_HEI+16);
    captureBtn->setMaximumSize(btnSize);
    layerBtn->setMaximumSize(btnSize);
    commBtn->setMaximumSize(btnSize);
    boardNum->setMaximumSize(btnSize);
    boardNumPlusBtn->setMaximumSize(btnSize);
    boardNumMinusBtn->setMaximumSize(btnSize);
    deletCapBtn->setMaximumSize(btnSize);
    deriveBtn->setMaximumSize(btnSize);
    layerAddBtn->setMaximumSize(btnSize);
    if (LCD_WIDTH == 640) {
        rightGroup->setFixedWidth(ICON_WID*2+50);
    }
    else {
        rightGroup->setFixedWidth(150);
    }
    autoStudyBtn->setMaximumHeight(BTN_HEIGHT);

    okCapBtn->setMaximumHeight(BTN_HEIGHT);
    preCapBtn->setMaximumHeight(BTN_HEIGHT);
    nextCapBtn->setMaximumHeight(BTN_HEIGHT);
    cancelCapBtn->setMaximumHeight(BTN_HEIGHT);

    capImgListGrid = new QGridLayout(leftGroup);
    imgSignalMapper = new QSignalMapper(AICapImageWidget);

    for (int i = 0; i < 6;i++) {
        imageBtn[i] = new myPushButton("", QIcon());

        imageNameBtn[i] = new myPushButton("", QIcon());
        imageNameBtn[i]->setMaximumHeight(BTN_HEIGHT);

        if (i/3 == 0) {
            capImgListGrid->addWidget(imageBtn[i], 0, i%3, 1, 1);
            capImgListGrid->addWidget(imageNameBtn[i], 1, i%3, 1, 1);
        }
        else {
            capImgListGrid->addWidget(imageBtn[i], 2, i%3, 1, 1);
            capImgListGrid->addWidget(imageNameBtn[i], 3, i%3, 1, 1);
        }

        /*使用信号栈方式实现各供料设置*/
        imgSignalMapper->setMapping(imageBtn[i], i*2);
        imgSignalMapper->setMapping(imageNameBtn[i], i*2+1);
        connect(imageBtn[i], SIGNAL(clicked()), imgSignalMapper, SLOT(map()));
        connect(imageNameBtn[i], SIGNAL(pressed()), imgSignalMapper, SLOT(map()));
    }

    capBtnListGridLayout = new QGridLayout(rightGroup);

    capBtnListGridLayout->addWidget(captureBtn, 0, 1, 1, 1);
    capBtnListGridLayout->addWidget(deriveBtn, 2, 0, 1, 1);
    capBtnListGridLayout->addWidget(deletCapBtn, 3, 0, 1, 1);
    capBtnListGridLayout->addWidget(autoStudyBtn, 4, 0, 1, 2);
    capBtnListGridLayout->addWidget(layerBtn, 0, 0, 1, 1);
    capBtnListGridLayout->addWidget(boardNumPlusBtn, 1, 1, 1, 1);
    capBtnListGridLayout->addWidget(boardNum, 2, 1, 1, 1);
    capBtnListGridLayout->addWidget(boardNumMinusBtn, 3, 1, 1, 1);

    capHBLayout  = new QHBoxLayout();
    capHBLayout->addWidget(leftGroup);
    capHBLayout->addWidget(rightGroup);

    capHB1Layout  = new QHBoxLayout();
    capHB1Layout->addWidget(okCapBtn);
    capHB1Layout->addWidget(preCapBtn);
    capHB1Layout->addWidget(nextCapBtn);
    capHB1Layout->addWidget(cancelCapBtn);

    capVBLayout = new QVBoxLayout(AICapImageWidget);
    capVBLayout->setContentsMargins(5,5,10,5);
    capVBLayout->addLayout(capHBLayout);
    capVBLayout->addLayout(capHB1Layout);

    stackedWidget->addWidget(AICapImageWidget);
    reloadImgList();

    connect(captureBtn, SIGNAL(pressed()), this, SLOT(onCaptureBtnClicked()));
    connect(boardNumPlusBtn, SIGNAL(pressed()), this, SLOT(onBoardNumPlusBtnClicked()));
    connect(boardNumMinusBtn, SIGNAL(pressed()), this, SLOT(onBoardNumMinusBtnClicked()));
    connect(layerBtn, SIGNAL(pressed()), this, SLOT(onLayerBtnClicked()));
    connect(okCapBtn, SIGNAL(pressed()), this, SLOT(onOkCapBtnClicked()));
    connect(cancelCapBtn, SIGNAL(pressed()), this, SLOT(onCancelCapBtnClicked()));
    connect(imgSignalMapper, SIGNAL(mapped(int)), this, SLOT(imageListChange(int)));
    connect(preCapBtn, SIGNAL(pressed()), this, SLOT(onPreBtnSlt()));
    connect(nextCapBtn, SIGNAL(pressed()), this, SLOT(onNextBtnSlt()));
    connect(deletCapBtn, SIGNAL(pressed()), this, SLOT(onDeleteBtnSlt()));
    connect(commBtn, SIGNAL(pressed()), this, SLOT(onCommBtnSlt()));
    connect(deriveBtn, SIGNAL(pressed()), this, SLOT(onDeriveBtnClicked()));
    connect(autoStudyBtn, SIGNAL(pressed()), this, SLOT(onAutoStudySlt()));
    connect(layerAddBtn, SIGNAL(pressed()), this, SLOT(onLayerAddBtnClicked()));
}

void AIAnalysis::onOkCapBtnClicked()
{
    imgListIndex = imgListCur;
    updateImgList();
    stackedWidget->setCurrentIndex(AI_MAIN_PAGE);

    param[matterIndex].imgIndex = imgListIndex;
    if(!getImageData(QString("%1.png").arg(imgListIndex))) {
        imageOpenError();
        return;
    }

    if(struCnfp.nIntelMode == 1) {  // 多分类模式
        saveDataToMultiFile(true);
    }
    else {                          // 普通模式
        saveDataToNormalFile(true);
    }

    if(m_bIsSimulate) {
        displaySimulate();
    }
    else {
        refreshBtnStatus();
        updateImage();
    }

    matterPixelSum[matterIndex] += param[matterIndex].iDisplaySum;
    clearSingleImageStatus(matterIndex);
}

void AIAnalysis::onCancelCapBtnClicked()
{
   imgListCur = imgListIndex;
   updateImgList();
   showInfoLabel();
   stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

void AIAnalysis::imageListChange(int index)
{
    if (index%2 == 0) {
        imgListCur = index/2+imgPageCur*6;
        updateImgList();
    }
    else {
        if(!struCnfp.nIntelMode) {
            return;
        }
        switch(imgListStat[index/2+imgPageCur*6]){
        case 0:
            imgListStat[index/2+imgPageCur*6] =1;
            imageListName[index/2+imgPageCur*6] = QString("%1 %2-%3").arg(myLan.ai_material)
                    .arg(index/2+imgPageCur*6)
                    .arg(myLan.ai_good);
            break;
        case 1:
            imgListStat[index/2+imgPageCur*6] =2;
            imageListName[index/2+imgPageCur*6] = QString("%1 %2-%3").arg(myLan.ai_material)
                    .arg(index/2+imgPageCur*6)
                    .arg(myLan.ai_bad);
            break;
        case 2:
            imgListStat[index/2+imgPageCur*6] =0;
            imageListName[index/2+imgPageCur*6] = QString("%1 %2").arg(myLan.ai_material)
                    .arg(index/2+imgPageCur*6);
            break;
        default:
            imageListName[index/2+imgPageCur*6] = QString("%1 %2").arg(myLan.ai_material)
                    .arg(index/2+imgPageCur*6);
            imgListStat[index/2+imgPageCur*6] =0;
            break;
        }
        imageNameBtn[index/2]->setText(imageListName[index/2+imgPageCur*6]);
        int nGoodNum = 0;
        int nBadNum  = 0;
        for (int i = 0; i < IMG_MAX_NUM;i++) {
            if (imgListStat[i] == 1){
              nGoodNum++;
            }
            if (imgListStat[i] == 2){
              nBadNum++;
            }
        }
        if (nGoodNum > 0 && nBadNum > 0) {
            autoStudyBtn->setEnabled(true);
        }
        else {
            autoStudyBtn->setEnabled(false);
        }
    }
}

void AIAnalysis::updateImgList()
{
    for(int i = imgPageCur*6; i < 6+imgPageCur*6;i++){
        if(i == imgListCur && imgPageCur == imgListCur/6) {
            imageBtn[i-imgPageCur*6]->setRedColor(GREEN);
        }
        else {
            imageBtn[i-imgPageCur*6]->setRedColor(DEF);
        }
    }
}

void AIAnalysis::onDeleteBtnSlt()
{
    char str[128];
    myMessageBox dlg(MSG_QUES,myLan.ai_del_img);
    int ret = dlg.exec();
    if (ret == QDialog::Accepted) {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        for(int i = imgListCur;i < IMG_MAX_NUM-1;i++) {
            if (imgListCur == IMG_MAX_NUM-1) {
    #ifdef Q_OS_UNIX
                sprintf(str, "rm -f /tmp/%d.png",i);
                system(str);
    #endif
            }
            else {
    #ifdef Q_OS_UNIX
                sprintf(str, "rm -f /tmp/%d.png",i);
                system(str);
                sprintf(str, "mv -f /tmp/%d.png /tmp/%d.png",i+1,i);
                system(str);
    #endif
            }
        }
        if (imgListNum > 0)
            imgListNum--;
        reloadImgList();
        infoWidget->hide();
    }
}

void AIAnalysis::onPreBtnSlt()
{
    if (imgPageCur > 0) {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        imgPageCur--;
        reloadImgList();
        updateImgList();
    	infoWidget->hide();
    }
    switch(imgPageCur)
    {
        case 0:
            preCapBtn->setEnabled(false);
            nextCapBtn->setEnabled(true);
            break;
        case 1:
            preCapBtn->setEnabled(true);
            nextCapBtn->setEnabled(true);
            break;
        case 2:
            preCapBtn->setEnabled(true);
            nextCapBtn->setEnabled(false);
            break;
    }
}

void AIAnalysis::onNextBtnSlt()
{
    if (imgPageCur < 2) {
    infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        imgPageCur++;
        reloadImgList();
        updateImgList();
    	infoWidget->hide();
    }
    switch(imgPageCur)
    {
        case 0:
            preCapBtn->setEnabled(false);
            nextCapBtn->setEnabled(true);
            break;
        case 1:
            preCapBtn->setEnabled(true);
            nextCapBtn->setEnabled(true);
            break;
        case 2:
            preCapBtn->setEnabled(true);
            nextCapBtn->setEnabled(false);
            break;
    }
}

void AIAnalysis::reloadImgList()
{
    bool bIsImg = true;
    int i = 0;
    QString strImg;
    QImage  image;
    while(i < 6) {
        strImg.sprintf("/tmp/%d.png",i+imgPageCur*6);
        bIsImg = image.load(strImg);
        if (bIsImg) {
            if (LCD_WIDTH == 640)
                imageBtn[i]->setIconSize(0.13*image.size());
            else
                imageBtn[i]->setIconSize(0.25*image.size());
            imageBtn[i]->setIcon(QPixmap::fromImage(image));
            imageBtn[i]->setEnabled(true);
            imageNameBtn[i]->setEnabled(true);
        }
        else {
            imageBtn[i]->setIcon(QPixmap());
            imageBtn[i]->setEnabled(false);
            imageNameBtn[i]->setEnabled(false);
        }
        switch(imgListStat[i+imgPageCur*6]){
        case 1:
            imageListName[i+imgPageCur*6] = QString("%1 %2-%3").arg(myLan.ai_material)
                    .arg(i+imgPageCur*6)
                    .arg(myLan.ai_good);
            break;
        case 2:
            imageListName[i+imgPageCur*6] = QString("%1 %2-%3").arg(myLan.ai_material)
                    .arg(i+imgPageCur*6)
                    .arg(myLan.ai_bad);
            break;
        default:
            imageListName[i+imgPageCur*6] = QString("%1 %2").arg(myLan.ai_material)
                    .arg(i+imgPageCur*6);
            break;
        }

        imageNameBtn[i]->setText(imageListName[i+imgPageCur*6]);

        i++;
    }
}

void AIAnalysis::onCommBtnSlt()
{
    struGsh.nIntelSampType = struGsh.nIntelSampType == 1?0:1;
    if (struGsh.nIntelSampType)
        commBtn->setText(myLan.ai_high_speed);
    else
        commBtn->setText(myLan.ai_low_speed);
}

void AIAnalysis::onAutoStudySlt()
{
    int groupNum = -1;
    int addr = -1;
    QImage image;
    QColor rgb;
    int maxR = 0,maxG = 0,maxB = 0, maxRG = 0, maxRB = 0, maxGB = 0;
    int minR = 255,minG = 255,minB = 255, minRG = 65025, minRB = 65025, minGB = 65025;

    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();
    QFile file("/tmp/trainAuto");
    if(!file.open(QIODevice::WriteOnly)){
        qDebug("Save data : File open error");
        return;
    }
    QTextStream io(&file);

    for (int k = 0; k < IMG_MAX_NUM;k++) {
        if (imgListStat[k] == 1) {
            image.load(QString("/tmp/%1.png").arg(k));
            for(int i=0; i<IMAGE_HEIGHT; i++){
                for(int j=0; j<IMAGE_WIDTH; j++){
                    rgb = image.pixel(j, i);
                    if (pixelIsBackground(rgb.red(),rgb.green(),rgb.blue())){
                        continue;
                    }

                    if(pixelIsEdgeImage(&image,j, i)){
                    	continue;
		    }

		    maxR = (rgb.red() > maxR) ? rgb.red():maxR;
		    minR = (rgb.red() < minR) ? rgb.red():minR;

		    maxG = (rgb.green() > maxG) ? rgb.green():maxG;
		    minG = (rgb.green() < minG) ? rgb.green():minG;

		    maxB = (rgb.blue() > maxB) ? rgb.blue():maxB;
		    minB = (rgb.blue() < minB) ? rgb.blue():minB;

		    maxRG = (rgb.red()*rgb.green() > maxRG) ? rgb.red()*rgb.green():maxRG;
		    minRG = (rgb.red()*rgb.green() < minRG) ? rgb.red()*rgb.green():minRG;

		    maxRB = (rgb.red()*rgb.blue() > maxRB) ? rgb.red()*rgb.blue():maxRB;
		    minRB = (rgb.red()*rgb.blue() < minRB) ? rgb.red()*rgb.blue():minRB;

		    maxGB = (rgb.green()*rgb.blue() > maxGB) ? rgb.green()*rgb.blue():maxGB;
		    minGB = (rgb.green()*rgb.blue() < minGB) ? rgb.green()*rgb.blue():minGB;

		    /*
                    QString str = QString("1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t7:%7\t8:%8\t9:%9\t\n")
                            .arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                            .arg(rgb.red()*rgb.green()).arg(rgb.red()*rgb.blue()).arg(rgb.green()*rgb.blue())
                            .arg(rgb.red()*rgb.red()).arg(rgb.green()*rgb.green()).arg(rgb.blue()*rgb.blue());
		    */
                    QString str = QString("1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t\n")
                            .arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                            .arg(rgb.red()*rgb.green()).arg(rgb.red()*rgb.blue()).arg(rgb.green()*rgb.blue());
                    io<<str;
                }
            }
        }
    }
    for (int k = 0; k < IMG_MAX_NUM;k++) {
        if (imgListStat[k] == 2) {
            image.load(QString("/tmp/%1.png").arg(k));
            for(int i=0; i<IMAGE_HEIGHT; i++){
                for(int j=0; j<IMAGE_WIDTH; j++){
                    rgb = image.pixel(j, i);
                    if (pixelIsBackground(rgb.red(),rgb.green(),rgb.blue())){
                        continue;
                    }
                    if(pixelIsEdgeImage(&image,j, i)){
                    	continue;
                    }
		  
                    maxR = (rgb.red() > maxR) ? rgb.red():maxR;
                    minR = (rgb.red() < minR) ? rgb.red():minR;

                    maxG = (rgb.green() > maxG) ? rgb.green():maxG;
                    minG = (rgb.green() < minG) ? rgb.green():minG;

                    maxB = (rgb.blue() > maxB) ? rgb.blue():maxB;
                    minB = (rgb.blue() < minB) ? rgb.blue():minB;

                    maxRG = (rgb.red()*rgb.green() > maxRG) ? rgb.red()*rgb.green():maxRG;
                    minRG = (rgb.red()*rgb.green() < minRG) ? rgb.red()*rgb.green():minRG;

                    maxRB = (rgb.red()*rgb.blue() > maxRB) ? rgb.red()*rgb.blue():maxRB;
                    minRB = (rgb.red()*rgb.blue() < minRB) ? rgb.red()*rgb.blue():minRB;

                    maxGB = (rgb.green()*rgb.blue() > maxGB) ? rgb.green()*rgb.blue():maxGB;
                    minGB = (rgb.green()*rgb.blue() < minGB) ? rgb.green()*rgb.blue():minGB;

		    /*
                    QString str = QString("-1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t7:%7\t8:%8\t9:%9\t\n")
                            .arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                            .arg(rgb.red()*rgb.green()).arg(rgb.red()*rgb.blue()).arg(rgb.green()*rgb.blue())
                            .arg(rgb.red()*rgb.red()).arg(rgb.green()*rgb.green()).arg(rgb.blue()*rgb.blue());
		    */
                    QString str = QString("-1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t\n")
                            .arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                            .arg(rgb.red()*rgb.green()).arg(rgb.red()*rgb.blue()).arg(rgb.green()*rgb.blue());
                    io<<str;
                }
            }
        }
    }
    file.close();

#if 1
    file.setFileName("/tmp/trainAuto.range");
    if(!file.open(QIODevice::WriteOnly)){
        qDebug("Save data : File open error");
        return;
    }
    /*
    QString str = QString("x\n-1 1\n1 %1 %2\n2 %3 %4\n3 %5 %6\n4 %7 %8\n5 %9 %10\n6 %11 %12\n7 %13 %14\n8 %15 %16\n9 %17 %18\n")
                           .arg(minR).arg(maxR)
                           .arg(minG).arg(maxG)
                           .arg(minB).arg(maxB)
                           .arg(minRG).arg(maxRG)
                           .arg(minRB).arg(maxRB)
                           .arg(minGB).arg(maxGB)
                           .arg(minR*minR).arg(maxR*maxR)
                           .arg(minG*minG).arg(maxG*maxG)
                           .arg(minB*minB).arg(maxB*maxB);
   */
    QString str = QString("x\n-1 1\n1 %1 %2\n2 %3 %4\n3 %5 %6\n4 %7 %8\n5 %9 %10\n6 %11 %12\n")
                           .arg(minR).arg(maxR)
                           .arg(minG).arg(maxG)
                           .arg(minB).arg(maxB)
                           .arg(minRG).arg(maxRG)
                           .arg(minRB).arg(maxRB)
                           .arg(minGB).arg(maxGB);
    io<<str;
    file.close();

    //智能计算并读取计算结果
    char cmd[64];
//    sprintf(cmd, "./normalization_scale -r /tmp/trainAuto.range /tmp/trainAuto");
    system(cmd);
    file.remove("/tmp/trainAuto");
    file.remove("/tmp/trainAuto.range");
//    sprintf(cmd, "./normalization_linear_learn /tmp/trainAuto.scale");
    system(cmd);
    file.remove("/tmp/trainAuto.scale");
    memset(&myMultiQuo, 0, sizeof(myMultiQuo));
    myMultiQuo.nMin[0] = minR;
    myMultiQuo.nMax[0] = maxR;
    myMultiQuo.nMin[1] = minG;
    myMultiQuo.nMax[1] = maxG;
    myMultiQuo.nMin[2] = minB;
    myMultiQuo.nMax[2] = maxB;
    myMultiQuo.nMin[3] = minRG;
    myMultiQuo.nMax[3] = maxRG;
    myMultiQuo.nMin[4] = minRB;
    myMultiQuo.nMax[4] = maxRB;
    myMultiQuo.nMin[5] = minGB;
    myMultiQuo.nMax[5] = maxGB;
    myMultiQuo.nMin[6] = minR*minR;
    myMultiQuo.nMax[6] = maxR*maxR;
    myMultiQuo.nMin[7] = minG*minG;
    myMultiQuo.nMax[7] = maxG*maxG;
    myMultiQuo.nMin[8] = minB*minB;
    myMultiQuo.nMax[8] = maxB*maxB;

    file.setFileName("/tmp/trainAuto.scale.model");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug("result file open error!");
        return;
    }
    for(int i=0; i<6; i++){
        io>>myMultiQuo.nQuo[i];
    }
    io>>myMultiQuo.nCons;     //cons*/
    file.close();

    for (int i=0; i<6; i++) {
	myMultiQuo.nCons -= myMultiQuo.nQuo[i];
    }

    for(int i=0; i<6; i++){
        myMultiQuo.nCons = myMultiQuo.nCons
                -2*myMultiQuo.nMin[i]*myMultiQuo.nQuo[i]/(myMultiQuo.nMax[i]-myMultiQuo.nMin[i]);
        myMultiQuo.nQuo[i] = 2*myMultiQuo.nQuo[i]/(myMultiQuo.nMax[i]-myMultiQuo.nMin[i])*10000000;
        qDebug("nQuo[%d] = %f\n", i, myMultiQuo.nQuo[i]);
    }
    myMultiQuo.nCons = myMultiQuo.nCons*10000000;
    qDebug("nCons = %f\n", myMultiQuo.nCons);
    file.remove("/tmp/trainAuto.scale.model");
#endif
    switch(struCnfp.nMatAssembleMode) {
    case PARAMS_ALL_SEPARATE:     //全部单独设置
        groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);

        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
        break;
    case PARAMS_FRONT_REAR_SAME: //前后视相同设置
        groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
        for (int i = groupNum/2; i < groupNum/2+2; i++) {
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
        }
        break;
    case PARAMS_FIRST_SECOND_SAME: //一二三次相同设置
        groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
        addr = groupNum%2;
        for(int i=0; i<struCnfg.nLevelTotal; i++){
            for(int j=0; j<MAX_GROUP_IDTNTIFY; j++){
                if (j%2 == addr) {
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
                }
            }
        }
        break;
    case PARAMS_ALL_SAME: //全部相同设置
        for(int i=0; i<struCnfg.nLevelTotal; i++){
            for(int j=0; j<MAX_GROUP_IDTNTIFY; j++){
                struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
            }
        }
        break;
    default:
        break;
    }
    infoWidget->hide();
}

/* 创建智能分析设置界面 */
void AIAnalysis::createAISetPage()
{
    AISetWidget = new QWidget;
    AISetWidget->setGeometry(0, 0, width(), height());

    QGridLayout *upLay = new QGridLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(AISetWidget);

    colorGroup = new myMutex(Qt::Vertical, 0, AISetWidget);
    colorGroup->setTitle(myLan.ai_sample_color);
    colorGroup->setNum(6);
    QStringList strList;
    strList << myLan.red << myLan.green << myLan.blue
            << myLan.red_green << myLan.red_blue << myLan.green_blue;
    colorGroup->setLabelText(strList);

    QGroupBox *sensGroup = new QGroupBox(myLan.ai_sample_sens, AISetWidget);
    sensGroup->setFont(config->getFont());
    QHBoxLayout *sensLay = new QHBoxLayout(sensGroup);
    sensValueBar = new myValueBar(0, 255, 1, 10,0);
    sensLay->addWidget(sensValueBar, Qt::AlignVCenter);
    sensLay->setContentsMargins(0, 0, 0, 0);

    colorSpaceGroup = new myMutex(Qt::Vertical, 0, AISetWidget);
    colorSpaceGroup->setTitle(myLan.ai_color_space);
    colorSpaceGroup->setNum(2);
    colorSpaceGroup->setLabelText("RGB", "HSV");
    colorSpaceGroup->hide();

    samModeGroup = new myMutex(Qt::Vertical, 0, AISetWidget);
    samModeGroup->setNum(2);
    samModeGroup->setTitle(myLan.ai_sample_mode);
    samModeGroup->setLabelText(myLan.dark, myLan.light);

    QGroupBox *triGroup = new QGroupBox(myLan.ai_enable_mode);
    triGroup->setFont(config->getFont());
    QHBoxLayout *triLay = new QHBoxLayout(triGroup);
    multiModeBtn = new myPushButton(QIcon(), config->getBtnSize(SMALL_BTN_SIZE));
    myLabel *multiModeLabel = new myLabel(myLan.ai_multi_mode);
    multiModeLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    multiModeLabel->setAlignment(Qt::AlignCenter);
    triLay->addWidget(multiModeBtn);
    triLay->addWidget(multiModeLabel);

    QGroupBox *enableModeGroup = new QGroupBox(myLan.ai_camera_mode);
    enableModeGroup->setFont(config->getFont());
    QGridLayout *lay = new QGridLayout;
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);
    normalModeBtn = new myPushButton(QIcon(), btnSize);
    infModeBtn = new myPushButton(QIcon(), btnSize);

    myLabel *normalModeLabel = new myLabel(myLan.ai_normal);
    normalModeLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    normalModeLabel->setAlignment(Qt::AlignCenter);
    myLabel *infModeLabel = new myLabel(myLan.ai_infrared);
    infModeLabel->setFont(config->getFont(DEFAULT_FONT_SIZE));
    infModeLabel->setAlignment(Qt::AlignCenter);

    lay->addWidget(normalModeBtn, 0, 0);
    lay->addWidget(normalModeLabel, 0, 1);
    lay->addWidget(infModeBtn, 1, 0);
    lay->addWidget(infModeLabel, 1, 1);
    lay->setColumnMinimumWidth(0, 60);
    enableModeGroup->setLayout(lay);

    if (!m_nMultiMode)
        colorSpaceGroup->setEnabled(false);

    AISetSureBtn = new myPushButton(myLan.ok, myIcon.Action_Apply);
    AISetSureBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    AISetBackBtn = new myPushButton(myLan.cancel, myIcon.Action_Cancel);
    AISetBackBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    /*
    upLay->addWidget(colorGroup, 0, 0);
    upLay->addWidget(sensGroup, 0, 1);
    upLay->addWidget(enableModeGroup, 0, 2);
    upLay->addWidget(samModeGroup, 1, 0);
    upLay->addWidget(triGroup, 1, 1);
    upLay->addWidget(colorSpaceGroup, 1, 2);
    */
    upLay->addWidget(colorGroup, 0, 0, 2, 1);
    upLay->addWidget(samModeGroup, 0, 1, 1, 1);
    upLay->addWidget(sensGroup, 0, 2, 1, 1);
    upLay->addWidget(triGroup, 1, 1, 1, 1);
    upLay->addWidget(enableModeGroup, 1, 2, 1, 1);

    if(LCD_WIDTH == 640){
    	upLay->setHorizontalSpacing(10);
    	upLay->setVerticalSpacing(0);
    	upLay->setContentsMargins(10, 0, 15, 5);
    }
    else{
    	upLay->setHorizontalSpacing(10);
    	upLay->setVerticalSpacing(10);
    	upLay->setContentsMargins(10, 10, 15, 10);
    }
    downLay->addWidget(AISetSureBtn);
    downLay->addStretch();
    downLay->addWidget(AISetBackBtn);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);

    connect(AISetSureBtn, SIGNAL(pressed()), this, SLOT(onAISetSureBtnClicked()));
    connect(AISetBackBtn, SIGNAL(pressed()), this, SLOT(onAISetBackBtnClicked()));
    connect(normalModeBtn, SIGNAL(pressed()), this, SLOT(onNormalModeBtnClicked()));
    connect(infModeBtn, SIGNAL(pressed()), this ,SLOT(onInfModeBtnClicked()));
    connect(colorGroup, SIGNAL(indexChanged(int)), this, SLOT(onSamColorIndexChangedSlt(int)));
    connect(colorSpaceGroup, SIGNAL(indexChanged(int)), this, SLOT(onTransModeChangedSlt(int)));
    connect(samModeGroup, SIGNAL(indexChanged(int)), this, SLOT(onSamModeChangedSlt(int)));
    connect(sensValueBar, SIGNAL(valueChanged(int)), this, SLOT(onSensVlaueChangedSlt(int)));
    connect(multiModeBtn, SIGNAL(pressed()), this, SLOT(onMultiModeBtnClicked()));

    stackedWidget->addWidget(AISetWidget);
}

/* 创建物料设置界面 */
void AIAnalysis::createAISetMatterPage()
{
    AIMatterWidget = new QWidget;
    AIMatterWidget->setGeometry(0, 0, width(), height());

    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(AIMatterWidget);

    QGroupBox *group = new QGroupBox(myLan.ai_set_material);
    group->setFont(config->getFont());
    QGridLayout *lay = new QGridLayout(group);
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);

    for(int i=0; i<MAT_MAX; i++){
        labels[i] = new QLabel;
        labels[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        labels[i]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        labels[i]->setFixedHeight(btnSize.height());
        lay->addWidget(labels[i],0,i);

        enableBtns[i] = new myPushButton(QIcon(), btnSize);
        lay->addWidget(enableBtns[i], i+1, 0);

        nameBtns[i] = new myPushButton(QIcon(), QSize(200,btnSize.height()));
        nameBtns[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        lay->addWidget(nameBtns[i], i+1, 1);

        isGoodBtns[i] = new myPushButton(QIcon(), btnSize);
        lay->addWidget(isGoodBtns[i], i+1, 2);

        isBadBtns[i] = new myPushButton(QIcon(), btnSize);
        lay->addWidget(isBadBtns[i], i+1, 3);
    }
    labels[0]->setText(myLan.enable);
    labels[1]->setText(myLan.ai_material_name);
    labels[2]->setText(myLan.ai_good);
    labels[3]->setText(myLan.ai_bad);

    setMatterSureBtn = new myPushButton(myLan.ok, myIcon.Action_Apply, AIMatterWidget);
    setMatterSureBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    setMatterBackBtn = new myPushButton(myLan.cancel, myIcon.Action_Cancel, AIMatterWidget);
    setMatterBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    upLay->addWidget(group);
    upLay->setContentsMargins(20, 20, 20, 10);
    downLay->addWidget(setMatterSureBtn, Qt::AlignLeft);
    downLay->addStretch();
    downLay->addWidget(setMatterBackBtn, Qt::AlignRight);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);

    connect(setMatterSureBtn, SIGNAL(pressed()), this, SLOT(onSetMatterSureBtnClicked()));
    connect(setMatterBackBtn, SIGNAL(pressed()), this, SLOT(onSetMatterBackBtnClicked()));
    connect(enableBtns[0], SIGNAL(pressed()), this, SLOT(onEnableBtn1Clicked()));
    connect(enableBtns[1], SIGNAL(pressed()), this, SLOT(onEnableBtn2Clicked()));
    connect(enableBtns[2], SIGNAL(pressed()), this, SLOT(onEnableBtn3Clicked()));
    connect(enableBtns[3], SIGNAL(pressed()), this, SLOT(onEnableBtn4Clicked()));
    connect(isGoodBtns[0], SIGNAL(pressed()), this, SLOT(onIsGoodBtn1Clicked()));
    connect(isGoodBtns[1], SIGNAL(pressed()), this, SLOT(onIsGoodBtn2Clicked()));
    connect(isGoodBtns[2], SIGNAL(pressed()), this, SLOT(onIsGoodBtn3Clicked()));
    connect(isGoodBtns[3], SIGNAL(pressed()), this, SLOT(onIsGoodBtn4Clicked()));
    connect(isBadBtns[0], SIGNAL(pressed()), this, SLOT(onIsBadBtn1Clicked()));
    connect(isBadBtns[1], SIGNAL(pressed()), this, SLOT(onIsBadBtn2Clicked()));
    connect(isBadBtns[2], SIGNAL(pressed()), this, SLOT(onIsBadBtn3Clicked()));
    connect(isBadBtns[3], SIGNAL(pressed()), this, SLOT(onIsBadBtn4Clicked()));
    connect(nameBtns[0], SIGNAL(pressed()), this, SLOT(onNameBtn1Clicked()));
    connect(nameBtns[1], SIGNAL(pressed()), this, SLOT(onNameBtn2Clicked()));
    connect(nameBtns[2], SIGNAL(pressed()), this, SLOT(onNameBtn3Clicked()));
    connect(nameBtns[3], SIGNAL(pressed()), this, SLOT(onNameBtn4Clicked()));

    stackedWidget->addWidget(AIMatterWidget);
}

/* 初始化智能分析界面各控件的状态 */
void AIAnalysis::initMatterWidgetStatus()
{
    for(int i=0; i<MAT_MAX; i++){
        if(struCnfp.struIntelMat[i].use){
            enableBtns[i]->setIcon(myIcon.Action_Apply);
            nameBtns[i]->setText(QString::fromLocal8Bit(struCnfp.struIntelMat[i].name));
            nameBtns[i]->setEnabled(true);
            isGoodBtns[i]->setEnabled(true);
            isBadBtns[i]->setEnabled(true);
            if(isGood[i]){
                isGoodBtns[i]->setIcon(myIcon.Action_Apply);
                isBadBtns[i]->setIcon(QIcon());
            }
            else{
                isGoodBtns[i]->setIcon(QIcon());
                isBadBtns[i]->setIcon(myIcon.Action_Apply);
            }
        }
        else{
            enableBtns[i]->setIcon(QIcon());
            nameBtns[i]->setEnabled(false);
            nameBtns[i]->setText("Name");
            isGoodBtns[i]->setEnabled(false);
            isGoodBtns[i]->setIcon(QIcon());
            isBadBtns[i]->setEnabled(false);
            isBadBtns[i]->setIcon(QIcon());
        }
    }

    if(struCnfp.nIntelMode == 1){
        enableBtns[2]->setEnabled(false);
        enableBtns[3]->setEnabled(false);
    }
    else{
        enableBtns[2]->setEnabled(true);
        enableBtns[3]->setEnabled(true);
    }

    showMatSel(2);
    showMatSel(3);
}

/* 初始化显示参数 */
void AIAnalysis::initAIParams()
{
    // 初始化显示部分的参数
    arrayIndex = 0;
    m_bIsSimulate = false;
    bRespondMouse = false;
    for(int i=0; i<MAT_MAX; i++){
        matterPixelSum[i] = 0;
        param[i].iScale = 1;
        param[i].iHMoveTimes = 0;
        param[i].iVMoveTimes = 0;
        param[i].iMaxMoveTimes = 2*param[i].iScale-1;
        param[i].bDataIsOk = false;
        param[i].iIndex = 0;
        param[i].iSpaceX = 0;
        param[i].iSpaceY = 0;
        param[i].bHasTar = false;
        param[i].iSelTimes = 0;
        param[i].iPixelSum = 0;
        param[i].iDisplaySum = 0;
        param[i].bIsOptCal = false;
        param[i].bIsOptSam = false;
        param[i].imgIndex  = -1;
        setDisplayColor(i);
    }
}

/* 初始化物料参数 */
void AIAnalysis::initMatterParams()
{
    matterIndex = NO_MATTER;
    bool firstIsFound = false;
    for(int i=0; i<MAT_MAX; i++){
        isEnable[i] = struCnfp.struIntelMat[i].use;
        isSelected[i] = false;
        if(isEnable[i]){
            isSelected[i] = true;
            if(!firstIsFound){
                firstIsFound = true;
                matterIndex = i;
            }
        }

        matterName[i] = QString::fromLocal8Bit(struCnfp.struIntelMat[i].name);
        isGood[i] = (struCnfp.struIntelMat[i].good_bad==1) ? true:false;
    }
}

/* 显示某一个物料设置 */
void AIAnalysis::showMatSel(int index)
{
    enableBtns[index]->show();
    nameBtns[index]->show();
    isGoodBtns[index]->show();
    isBadBtns[index]->show();
}

/* 隐藏某一个物料设置 */
void AIAnalysis::hideMatSel(int index)
{
    enableBtns[index]->hide();
    nameBtns[index]->hide();
    isGoodBtns[index]->hide();
    isBadBtns[index]->hide();
}

/* 更新物料参数 */
void AIAnalysis::updateMatterParams()
{
    for(int i=0; i<MAT_MAX; i++){
        isEnable[i] = struCnfp.struIntelMat[i].use;
        isSelected[i] = false;
        if(isEnable[i]) {
            isSelected[i] = true;
        }

        matterName[i] = QString::fromLocal8Bit(struCnfp.struIntelMat[i].name);
        isGood[i] = (struCnfp.struIntelMat[i].good_bad==1) ? true:false;
    }
}

/* 设置智能分析参数 */
void AIAnalysis::setAIParams()
{
    m_nSamColor = struGsh.nIntelSampColor[matterIndex]/2;
    m_nSamSens = struGsh.nIntelSampSens[matterIndex];
    m_nSamMode = struGsh.nIntelSampMode[matterIndex];
  //  m_nColorSpace = struGsh.nIntelSampType;
    m_nTriThreshold = struGsh.nIntelSampBurst;
    m_nNormalMode = struGsh.nIntelSampAi;
    m_nMultiMode = struCnfp.nIntelMode;
}

/* 根据文件名填充缓冲区数据 */
bool AIAnalysis::getImageData(QString fileName)
{
    QFile file(QString("/tmp/%1").arg(fileName));
    if(!file.exists()){
        qDebug("%s: file open error!", fileName.toLatin1().data());
        bRespondMouse = false;
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
    memcpy(changeBuf, oriBuf, IMAGE_WIDTH*IMAGE_HEIGHT*3);
    bRespondMouse = true;
    return true;
}

/* 用于处理文件打开失败的情况 */
void AIAnalysis::imageOpenError()
{
    matterSetBtn->setEnabled(true);
    setBtn->setEnabled(true);
    updateBtn->setEnabled(true);
    imgBtn->setEnabled(true);
    enlargeBtn->setEnabled(false);
    reduceBtn->setEnabled(false);
    upBtn->setEnabled(false);
    downBtn->setEnabled(false);
    leftBtn->setEnabled(false);
    rightBtn->setEnabled(false);
    optCalBtn->setEnabled(false);
    optCalBtn->setRedColor(DEF);
    optSampleBtn->setEnabled(false);
    optSampleBtn->setRedColor(DEF);
    calBtn->setEnabled(false);
    simulateBtn->setEnabled(false);
    simulateBtn->setRedColor(DEF);
    reLastBtn->setEnabled(false);
    clearSamBtn->setEnabled(false);
    imageLabel->setText(myLan.ai_no_image);
    AIWarningLabel->setText("");
    showMesLabel();
    showInfoLabel();

}

/* 用于清空所有图片的智能参数 */
void AIAnalysis::clearImageStatus(bool clearImgIndex)
{
    for(int i=0; i<MAT_MAX; i++){
        if(clearImgIndex){
            param[i].imgIndex = -1;
            matterPixelSum[i] = 0;
        }
        param[i].iSelTimes = 0;
        param[i].iPixelSum = 0;
        param[i].iDisplaySum = 0;
        param[i].bIsOptCal = false;
        param[i].bIsOptSam = false;
    }
}

/* 用于清空单个物料的智能参数 */
void AIAnalysis::clearSingleImageStatus(int index)
{
    param[index].iSelTimes = 0;
    param[index].iPixelSum = 0;
    param[index].iDisplaySum = 0;
}

/*　根据当前图片的尺寸刷新按钮的状态　*/
void AIAnalysis::refreshBtnStatus()
{
    matterSetBtn->setEnabled(true);
    //若没有图片索引，则上下左右及放大缩小按钮为不可按
    if(matterIndex==NO_MATTER){
        setBtn->setEnabled(false);            
        imgBtn->setEnabled(false);
        updateBtn->setEnabled(false);
        enlargeBtn->setEnabled(false);
        reduceBtn->setEnabled(false);
        upBtn->setEnabled(false);
        downBtn->setEnabled(false);
        leftBtn->setEnabled(false);
        rightBtn->setEnabled(false);
        optCalBtn->setEnabled(false);
        optCalBtn->setRedColor(DEF);
        optSampleBtn->setEnabled(false);
        optSampleBtn->setRedColor(DEF);
        calBtn->setEnabled(false);
        simulateBtn->setEnabled(false);
        simulateBtn->setRedColor(DEF);

        if(matterIndex == NO_MATTER){
            lastBtn->setEnabled(false);
            nextBtn->setEnabled(false);
        }
        reLastBtn->setEnabled(false);
        clearSamBtn->setEnabled(false);

        showInfoLabel();

        return;
    }

    clearSamBtn->setEnabled(true);
    optCalBtn->setEnabled(true);
    optSampleBtn->setEnabled(true);
    calBtn->setEnabled(true);
    simulateBtn->setEnabled(true);
    setBtn->setEnabled(true);
    imgBtn->setEnabled(true);
    updateBtn->setEnabled(true);

    if(param[matterIndex].bIsOptCal)
        optCalBtn->setRedColor(GREEN);
    else
        optCalBtn->setRedColor(DEF);

    if(param[matterIndex].bIsOptSam)
        optSampleBtn->setRedColor(GREEN);
    else
        optSampleBtn->setRedColor(DEF);

    if(param[matterIndex].iSelTimes > 0)
        reLastBtn->setEnabled(true);
    else
        reLastBtn->setEnabled(false);

    if(param[matterIndex].iScale == MIN_SCALE){         //设置图片显示的最小尺寸为1
        enlargeBtn->setEnabled(true);
        reduceBtn->setEnabled(false);
    }
    else if(param[matterIndex].iScale>=MIN_SCALE && param[matterIndex].iScale<MAX_SCALE){
        enlargeBtn->setEnabled(true);
        reduceBtn->setEnabled(true);
    }
    else if(param[matterIndex].iScale == MAX_SCALE){    //设置图片显示的最大尺寸为8
        enlargeBtn->setEnabled(false);
        reduceBtn->setEnabled(true);
    }

    if(param[matterIndex].iHMoveTimes == 0){
        leftBtn->setEnabled(false);
        rightBtn->setEnabled(true);
    }
    else if(param[matterIndex].iHMoveTimes>0 && param[matterIndex].iHMoveTimes<param[matterIndex].iMaxMoveTimes){
        leftBtn->setEnabled(true);
        rightBtn->setEnabled(true);
    }
    else if(param[matterIndex].iHMoveTimes == param[matterIndex].iMaxMoveTimes){
        leftBtn->setEnabled(true);
        rightBtn->setEnabled(false);
    }

    if(param[matterIndex].iVMoveTimes == 0){
        upBtn->setEnabled(false);
        downBtn->setEnabled(true);
    }
    else if(param[matterIndex].iVMoveTimes>0 && param[matterIndex].iVMoveTimes<param[matterIndex].iMaxMoveTimes){
        upBtn->setEnabled(true);
        downBtn->setEnabled(true);
    }
    else if(param[matterIndex].iVMoveTimes == param[matterIndex].iMaxMoveTimes){
        upBtn->setEnabled(true);
        downBtn->setEnabled(false);
    }

    if(param[matterIndex].iScale==1 && DISPLAY_HEI>=IMAGE_HEIGHT){
        upBtn->setEnabled(false);
        downBtn->setEnabled(false);
    }

    if(m_bIsSimulate){
        simulateBtn->setRedColor(GREEN);
        reLastBtn->setEnabled(false);
        optCalBtn->setEnabled(false);
        optSampleBtn->setEnabled(false);
        calBtn->setEnabled(false);
        updateBtn->setEnabled(false);
        matterSetBtn->setEnabled(false);
    }
    else
        simulateBtn->setRedColor(DEF);

    showInfoLabel();
}

/* 响应智能分析界面的上移按钮 */
void AIAnalysis::onUpBtnClicked()
{
    param[matterIndex].iVMoveTimes--;
    param[matterIndex].iIndex -= 2*param[matterIndex].iScale;
    //qDebug()<<"scale = "<<param[matterIndex].iScale;
    //qDebug()<<"up index = "<<param[matterIndex].iIndex;
    refreshBtnStatus();
    updateImage();
}

/* 响应智能分析界面的下移按钮 */
void AIAnalysis::onDownBtnClicked()
{
    param[matterIndex].iVMoveTimes++;
    param[matterIndex].iIndex += 2*param[matterIndex].iScale;
    //qDebug()<<"scale = "<<param[matterIndex].iScale;
    //qDebug()<<"down index = "<<param[matterIndex].iIndex;
    refreshBtnStatus();
    updateImage();
}

/* 响应智能分析界面的左移按钮 */
void AIAnalysis::onLeftBtnClicked()
{
    param[matterIndex].iHMoveTimes--;
    param[matterIndex].iIndex--;
    //qDebug()<<"scale = "<<param[matterIndex].iScale;
    //qDebug()<<"left index = "<<param[matterIndex].iIndex;
    refreshBtnStatus();
    updateImage();
}

/* 响应智能分析界面的右移按钮 */
void AIAnalysis::onRightBtnClicked()
{
    param[matterIndex].iHMoveTimes++;
    param[matterIndex].iIndex++;
    //qDebug()<<"scale = "<<param[matterIndex].iScale;
    //qDebug()<<"right index = "<<param[matterIndex].iIndex;
    refreshBtnStatus();
    updateImage();
}

/* 响应智能分析界面的放大按钮 */
void AIAnalysis::onEnlargeBtnClicked()
{
    param[matterIndex].iScale = param[matterIndex].iScale*2;
    param[matterIndex].iMaxMoveTimes = 2*param[matterIndex].iScale-1;
    param[matterIndex].iHMoveTimes *= 2;
    param[matterIndex].iVMoveTimes *= 2;

    switch(param[matterIndex].iScale){
    case 2:
        param[matterIndex].iIndex = oneToTwo[param[matterIndex].iIndex];
        break;
    case 4:
        param[matterIndex].iIndex = twoToFour[param[matterIndex].iIndex];
        break;
    case 8:
        param[matterIndex].iIndex = fourToEight[param[matterIndex].iIndex];
        break;
    default:
        break;
    }

    //qDebug()<<"scale = "<<param[matterIndex].iScale;
    //qDebug()<<"enlarge index = "<<param[matterIndex].iIndex;
    refreshBtnStatus();
    updateImage();
}

/* 响应智能分析界面的缩小按钮 */
void AIAnalysis::onReduceBtnClicked()
{
    param[matterIndex].iScale = param[matterIndex].iScale/2;
    param[matterIndex].iMaxMoveTimes = 2*param[matterIndex].iScale-1;
    param[matterIndex].iHMoveTimes /= 2;
    param[matterIndex].iVMoveTimes /= 2;

    switch(param[matterIndex].iScale){
    case 1:
        param[matterIndex].iIndex = twoToOne[param[matterIndex].iIndex];
        break;
    case 2:
        param[matterIndex].iIndex = fourToTwo[param[matterIndex].iIndex];
        break;
    case 4:
        param[matterIndex].iIndex = eightToFour[param[matterIndex].iIndex];
        break;
    default:
        break;
    }

    //qDebug()<<"scale = "<<param[matterIndex].iScale;
    //qDebug()<<"reduce index = "<<param[matterIndex].iIndex;
    refreshBtnStatus();
    updateImage();
}

/* 响应智能分析界面的增加板号按钮 */
void AIAnalysis::onBoardNumPlusBtnClicked()
{
    int IDTotal;
    if(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal==1)
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
    else
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
    if(struGsh.nUnit < IDTotal-2)
        struGsh.nUnit += 2;
    boardNum->display(struGsh.nUnit/2+1);
}

/* 响应智能分析界面的减小板号按钮 */
void AIAnalysis::onBoardNumMinusBtnClicked()
{
    if(struGsh.nUnit > 1)
        struGsh.nUnit -= 2;
    boardNum->display(struGsh.nUnit/2+1);
}

/* 响应智能分析界面的选择上下层按钮 */
void AIAnalysis::onLayerBtnClicked()
{
    if(struGsh.nUnit%2 == 1){
        layerBtn->setText(myLan.front);
        struGsh.nUnit -= 1;
    }
    else{
        layerBtn->setText(myLan.rear);
        struGsh.nUnit += 1;
    }
}

/* 响应智能分析设置界面的选择前后视按钮 */
void AIAnalysis::onLayerAddBtnClicked()
{
}

/* 响应智能分析设置界面的智能计算按钮 */
void AIAnalysis::onAICalBtnClicked()
{
    bool bHasEnough = true;
    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i] && matterPixelSum[i]+param[i].iDisplaySum<MIN_PIXEL_NUM){
            bHasEnough = false;
            break;
        }
    }

    if(!bHasEnough){
        infoWidget->setLabelText(myLan.ai_select_1000_pixels);
        infoWidget->showSecs();
        return;
    }

    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();

    bool status;
    if(struCnfp.nIntelMode == 1)
        status = saveDataToMultiFile(false);
    else
        status = saveDataToNormalFile();

    if(!status){
        infoWidget->setLabelText(myLan.ai_save_error);
        infoWidget->showSecs();
        return;
    }

    if(struCnfp.nIntelMode == 1)    // 多分类智能算法模式
        status = multiModeAICal();
    else                            // 普通智能算法模式
        status = normalModeAICal();

    /* 计算完毕后清除当前图片的状态 */
    if(status){
        for(int i=0; i<MAT_MAX; i++){
            matterPixelSum[i] = 0;
        }

        for(int i=0; i<param[matterIndex].iPixelSum; i++){
            int x = param[matterIndex].selectedPoint[i].x();
            int y = param[matterIndex].selectedPoint[i].y();
            changeBuf[(y*IMAGE_WIDTH+x)*3] = oriBuf[(y*IMAGE_WIDTH+x)*3];
            changeBuf[(y*IMAGE_WIDTH+x)*3+1] = oriBuf[(y*IMAGE_WIDTH+x)*3+1];
            changeBuf[(y*IMAGE_WIDTH+x)*3+2] = oriBuf[(y*IMAGE_WIDTH+x)*3+2];
        }

        clearImageStatus(false);
        updateImage();
        showMesLabel();
        refreshBtnStatus();
    }

    infoWidget->delayHide();
}

/* 响应智能分析设置界面的抓拍按钮 */
void AIAnalysis::onCaptureBtnClicked()
{
    bool bGetImageIsOk;

    /* 考虑传感器的分辨率 */
    if(struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_1024)
    {
        if(struGsh.nIntelSampType == 0) //通过串口获取数据
            bGetImageIsOk = getImageFromUART_1024();
        else                            //通过USB获取数据
            bGetImageIsOk = getImageFromUSB_1024();
    }
    else if(struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_2048)
    {
        if(struGsh.nIntelSampType == 0) //通过串口获取数据
            bGetImageIsOk = getImageFromUART_2048();
        else                            //通过USB获取数据
            bGetImageIsOk = getImageFromUSB_2048();
    }

    qDebug()<<bGetImageIsOk;
    if(!bGetImageIsOk)
        return;

    qDebug()<<bGetImageIsOk;
    qDebug()<<imgListNum;

    /*自动更新图片*/
    QString strImg;
    QImage  image;
    bool    bIsImg;
    if(imgListNum/6 == imgPageCur) {
        strImg.sprintf("/tmp/%d.png",imgListNum);
        bIsImg = image.load(strImg);
        if (bIsImg) {
            if (LCD_WIDTH == 640)
                imageBtn[imgListNum%6]->setIconSize(0.13*image.size());
            else
                imageBtn[imgListNum%6]->setIconSize(0.25*image.size());
            imageBtn[imgListNum%6]->setIcon(QPixmap::fromImage(image));
            imageBtn[imgListNum%6]->setEnabled(true);
            imageNameBtn[imgListNum%6]->setEnabled(true);
        }
        else {
            imageBtn[imgListNum%6]->setIcon(QPixmap());
            imageBtn[imgListNum%6]->setEnabled(false);
            imageNameBtn[imgListNum%6]->setEnabled(false);
        }
        imageNameBtn[imgListNum%6]->setText(imageListName[imgListNum]);
    }

    if(imgListNum < 17){
        imgListNum++;
    }

    updateImage();
    refreshBtnStatus();
}

/* 响应智能分析设置界面的导出按钮 */
void AIAnalysis::onDeriveBtnClicked()
{
    QDir dir("/proc/scsi/usb-storage");

    if(!dir.exists()){
        infoWidget->setLabelText(myLan.msg_insert_udisk);
        infoWidget->showSecs();
        return;
    }

    infoWidget->setLabelText(myLan.msg_copy_image);
    infoWidget->delayShow();
    char cmd[64];
    if (!myFlow.mountUdisk()) {
        infoWidget->setLabelText(myLan.msg_insert_udisk);
        infoWidget->delayShow();
        myFlow.sleep(1);
        infoWidget->hide();
        return;
    }

    /* copy png files to usb storage */
    QDateTime time = QDateTime::currentDateTime();
    QString pngDir = QString("/udisk/%1/%2").arg(myString.sVendor).arg(time.toString("yyyyMMddhhmmss"));

    QString shell;
    shell.sprintf("mkdir -p %s", qPrintable(pngDir));
    qDebug("%s", qPrintable(shell));
    system(qPrintable(shell));
    myFlow.sleep(1);

    shell.sprintf("cp -f /tmp/*.png %s", qPrintable(pngDir));
    qDebug("%s", qPrintable(shell));
    system(qPrintable(shell));
    myFlow.sleep(1);

    system("sync");

    sprintf(cmd,"umount /udisk");
    system(cmd);
    myFlow.sleep(1);

    infoWidget->hide();
}

/* 响应智能分析设置界面的仿真按钮 */
void AIAnalysis::onSimulateBtnClicked()
{
    m_bIsSimulate = !m_bIsSimulate;
    if(m_bIsSimulate){
        simulateBtn->setRedColor(GREEN);
        displaySimulate();
    }
    else{
        simulateBtn->setRedColor(DEF);
        for(int i=0; i<IMAGE_HEIGHT; i++){
            for(int j=0; j<IMAGE_WIDTH; j++){
                changeBuf[(i*IMAGE_WIDTH+j)*3] = oriBuf[(i*IMAGE_WIDTH+j)*3];
                changeBuf[(i*IMAGE_WIDTH+j)*3+1] = oriBuf[(i*IMAGE_WIDTH+j)*3+1];
                changeBuf[(i*IMAGE_WIDTH+j)*3+2] = oriBuf[(i*IMAGE_WIDTH+j)*3+2];
            }
        }
        refreshDisplay();
    }
}

/* 响应智能分析设置界面的样本更新按钮 */
void AIAnalysis::onUpdateBtnClicked()
{
    bool bGetImageIsOk;
    bGetImageIsOk = getImageData(QString("%1.png").arg(param[matterIndex].imgIndex));
    if(!bGetImageIsOk)
        return;

    // 将上幅图片中选择的样本点保存到样本文件中
    bool status;
    if(struCnfp.nIntelMode == 1)// 多分类模式
        status = saveDataToMultiFile(true);
    else
        status = saveDataToNormalFile(true);

    matterPixelSum[matterIndex] += param[matterIndex].iDisplaySum;

    clearSingleImageStatus(matterIndex);
    updateImage();
    refreshBtnStatus();
}

/*进入图像采集页面*/
void AIAnalysis::onImgCapBtnClicked()
{
    stackedWidget->setCurrentIndex(AI_CAP_IMAGE_PAGE);
    imgListIndex = param[matterIndex].imgIndex;
    imgListCur   = imgListIndex;
    updateImgList();
}

/* 响应智能分析页面中的清空样本按钮 */
void AIAnalysis::onClearSamBtnClicked()
{
    myMessageBox *mesBox = new myMessageBox(MSG_QUES, myLan.cfm_reselect_sample);
    int ret = mesBox->exec();
    if(ret == QDialog::Accepted){
        // 删除之前保存的训练样本
        system("rm /tmp/train*");

        // 清空所有物料的智能参数
        clearImageStatus(false);
        for(int i=0; i<MAT_MAX; i++){
            matterPixelSum[i] = 0;
        }

        // 刷新界面的显示
        reLastBtn->setEnabled(false);
        memcpy(changeBuf, oriBuf, IMAGE_WIDTH*IMAGE_HEIGHT*3);
        updateImage();
        showMesLabel();
    }
}

/* 响应智能分析设置界面的选择正常模式按钮 */
void AIAnalysis::onNormalModeBtnClicked()
{
    if(m_nNormalMode == 0){
        m_nNormalMode = 1;
        normalModeBtn->setIcon(myIcon.Action_Apply);
    }
    else if(m_nNormalMode == 1){
        m_nNormalMode = 0;
        normalModeBtn->setIcon(QIcon());
    }
}

/* 响应智能分析设置界面的选择红外模式按钮 */
void AIAnalysis::onInfModeBtnClicked()
{
    if(m_nInfMode == 0){
        m_nInfMode = 1;
        infModeBtn->setIcon(myIcon.Action_Apply);
    }
    else if(m_nInfMode == 1){
        m_nInfMode = 0;
        infModeBtn->setIcon(QIcon());
    }
}

/* 响应智能分析设置界面的选择多分类模式按钮 */
void AIAnalysis::onMultiModeBtnClicked()
{
    m_nMultiMode = !m_nMultiMode;
    if(m_nMultiMode)
        multiModeBtn->setIcon(myIcon.Action_Apply);
    else
        multiModeBtn->setIcon(QIcon());
}

/* 响应智能分析设置界面的返回按钮 */
void AIAnalysis::onAISetBackBtnClicked()
{
    stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

/* 响应智能分析设置界面的确认按钮 */
void AIAnalysis::onAISetSureBtnClicked()
{
    if(matterIndex != NO_MATTER){   // 存在使能物料
        struGsh.nIntelSampAi = m_nNormalMode;
        struGsh.nIntelSampBurst = m_nTriThreshold;
        if(m_nSamColor == 0)
            struGsh.nIntelSampColor[matterIndex] = 1;
        else
            struGsh.nIntelSampColor[matterIndex] = m_nSamColor*2;
        struGsh.nIntelSampMode[matterIndex] = m_nSamMode;
        struCnfp.nIntelMode = m_nMultiMode;
        if(struCnfp.nIntelMode){    // 多分类模式
            struCnfp.struIntelMat[0].use = 1;
            struCnfp.struIntelMat[0].good_bad = 1;
            struCnfp.struIntelMat[1].use = 1;
            struCnfp.struIntelMat[1].good_bad = 0;
            struCnfp.struIntelMat[2].use = 0;
            struCnfp.struIntelMat[3].use = 0;
            enableAIArithmetic();
        }

        struGsh.nIntelSampSens[matterIndex] = m_nSamSens;
        setDisplayColor(matterIndex);

        if(!getImageData(QString("%1.png").arg(param[matterIndex].imgIndex))){
            imageOpenError();
            goto ChangeIndex;
        }
        refreshDisplay();
    }


ChangeIndex:
    stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

/* 响应智能分析设置界面灵敏度变化的事件 */
void AIAnalysis::onSensVlaueChangedSlt(int value)
{
    m_nSamSens = value;
}

/* 响应智能分析设置界面触发阈值变化的事件 */
void AIAnalysis::onTriValueChangedSlt(int value)
{
    m_nTriThreshold = value;
}

/* 响应智能分析设置界面样本颜色变化的事件 */
void AIAnalysis::onSamColorIndexChangedSlt(int index)
{
    m_nSamColor = index;
}

/* 响应智能分析设置界面传输模式变化的事件 */
void AIAnalysis::onTransModeChangedSlt(int index)
{
    m_nColorSpace = index;
}

/* 响应智能分析设置界面样本模式变化的事件 */
void AIAnalysis::onSamModeChangedSlt(int index)
{
    m_nSamMode = index;
}

/* 响应物料设置界面的确认按钮 */
void AIAnalysis::onSetMatterSureBtnClicked()
{
    int matterNum = 0;
    QString warningStr("");
    bool isSameStatus = true;
    bool firstStatus;
    bool hasNoSel = false;
    bool isFirst = true;
    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i]){
            if(!isSelected[i])
                hasNoSel = true;
            matterNum++;
        }
    }

    if(hasNoSel){       //存在没有选择好坏的物料
        infoWidget->setLabelText(myLan.ai_set_good_bad);
        infoWidget->showSecs();
        return;
    }

    if(matterNum == 1){ //物料数目为1
        infoWidget->setLabelText(myLan.ai_set_more_material);
        infoWidget->showSecs();
        return;
    }
    if(matterNum > 1){
        for(int i=0; i<MAT_MAX; i++){
            if(isEnable[i]){
                if(isFirst){
                    firstStatus = isGood[i];
                    isFirst = false;
                }
                if(isGood[i] != firstStatus)
                    isSameStatus = false;
            }
        }
        if(isSameStatus){
            if(firstStatus)
                warningStr = myLan.ai_need_bad_material;
            else
                warningStr = myLan.ai_need_good_material;
            infoWidget->setLabelText(warningStr);
            infoWidget->showSecs();
            return;
        }
    }

    setGlobalMatterParams();
    enableAIArithmetic();

    /* 若物料未使能，则清空该物料的所有参数 */
    for(int i=0; i<MAT_MAX; i++){
        if(!isEnable[i]){
            param[i].imgIndex = -1;
            clearSingleImageStatus(i);
        }
    }

    /* 若物料图片存在，则打开该图片 */
    bool hasEnable = false;
    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i]){
            hasEnable = true;
            matterIndex = i;
            arrayIndex = 0;
            nextBtn->setEnabled(true);
            lastBtn->setEnabled(false);
            if(!getImageData(QString("%1.png").arg(param[matterIndex].imgIndex))){
                imageOpenError();
                goto ChangeIndex;
            }
            refreshDisplay();
            break;
        }
    }

    if(!hasEnable){
        matterIndex = NO_MATTER;
        clearImageStatus();
        imageLabel->setText(myLan.ai_no_image);
        mesLabel->setText(myLan.msg_set_material);
        AIWarningLabel->setText("");
    }
    refreshBtnStatus();

ChangeIndex:
    stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

/* 响应物料设置界面的返回按钮 */
void AIAnalysis::onSetMatterBackBtnClicked()
{
    stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

/* 响应物料设置界面的使能1按钮 */
void AIAnalysis::onEnableBtn1Clicked()
{
    isEnable[0] = !isEnable[0];
    if(isEnable[0]){
        enableBtns[0]->setIcon(myIcon.Action_Apply);
        nameBtns[0]->setEnabled(true);
        isGoodBtns[0]->setEnabled(true);
        isBadBtns[0]->setEnabled(true);
    }
    else{
        enableBtns[0]->setIcon(QIcon());
        nameBtns[0]->setEnabled(false);
        isGoodBtns[0]->setEnabled(false);
        isBadBtns[0]->setEnabled(false);
        isGood[0] = false;
        isGoodBtns[0]->setIcon(QIcon());
        isBadBtns[0]->setIcon(QIcon());
    }
}

/* 响应物料设置界面的使能2按钮 */
void AIAnalysis::onEnableBtn2Clicked()
{
    isEnable[1] = !isEnable[1];
    if(isEnable[1]){
        enableBtns[1]->setIcon(myIcon.Action_Apply);
        nameBtns[1]->setEnabled(true);
        isGoodBtns[1]->setEnabled(true);
        isBadBtns[1]->setEnabled(true);
    }
    else{
        enableBtns[1]->setIcon(QIcon());
        nameBtns[1]->setEnabled(false);
        isGoodBtns[1]->setEnabled(false);
        isBadBtns[1]->setEnabled(false);
        isGood[1] = false;
        isGoodBtns[1]->setIcon(QIcon());
        isBadBtns[1]->setIcon(QIcon());
    }
}

/* 响应物料设置界面的使能3按钮 */
void AIAnalysis::onEnableBtn3Clicked()
{
    isEnable[2] = !isEnable[2];
    if(isEnable[2]){
        enableBtns[2]->setIcon(myIcon.Action_Apply);
        nameBtns[2]->setEnabled(true);
        isGoodBtns[2]->setEnabled(true);
        isBadBtns[2]->setEnabled(true);
    }
    else{
        enableBtns[2]->setIcon(QIcon());
        nameBtns[2]->setEnabled(false);
        isGoodBtns[2]->setEnabled(false);
        isBadBtns[2]->setEnabled(false);
        isGood[2] = false;
        isGoodBtns[2]->setIcon(QIcon());
        isBadBtns[2]->setIcon(QIcon());
    }
}

/* 响应物料设置界面的使能4按钮 */
void AIAnalysis::onEnableBtn4Clicked()
{
    isEnable[3] = !isEnable[3];
    if(isEnable[3]){
        enableBtns[3]->setIcon(myIcon.Action_Apply);
        nameBtns[3]->setEnabled(true);
        isGoodBtns[3]->setEnabled(true);
        isBadBtns[3]->setEnabled(true);
    }
    else{
        enableBtns[3]->setIcon(QIcon());
        nameBtns[3]->setEnabled(false);
        isGoodBtns[3]->setEnabled(false);
        isBadBtns[3]->setEnabled(false);
        isGood[3] = false;
        isGoodBtns[3]->setIcon(QIcon());
        isBadBtns[3]->setIcon(QIcon());
    }
}

/* 响应物料设置界面的名称1按钮 */
void AIAnalysis::onNameBtn1Clicked()
{
    myInputMethod nameMethod(myLan.msg_set_material,nameBtns[0]->text());
    int ret = nameMethod.exec();
    if(ret == QDialog::Accepted){
        matterName[0] = nameMethod.getText();
        nameBtns[0]->setText(matterName[0]);
    }
}

/* 响应物料设置界面的名称2按钮 */
void AIAnalysis::onNameBtn2Clicked()
{
    myInputMethod nameMethod(myLan.msg_set_material,nameBtns[1]->text());
    int ret = nameMethod.exec();
    if(ret == QDialog::Accepted){
        matterName[1] = nameMethod.getText();
        nameBtns[1]->setText(matterName[1]);
    }
}

/* 响应物料设置界面的名称3按钮 */
void AIAnalysis::onNameBtn3Clicked()
{
    myInputMethod nameMethod(myLan.msg_set_material,nameBtns[2]->text());
    int ret = nameMethod.exec();
    if(ret == QDialog::Accepted){
        matterName[2] = nameMethod.getText();
        nameBtns[2]->setText(matterName[2]);
    }
}

/* 响应物料设置界面的名称4按钮 */
void AIAnalysis::onNameBtn4Clicked()
{
    myInputMethod nameMethod(myLan.msg_set_material,nameBtns[3]->text());
    int ret = nameMethod.exec();
    if(ret == QDialog::Accepted){
        matterName[3] = nameMethod.getText();
        nameBtns[3]->setText(matterName[3]);
    }
}

/* 响应物料设置界面的设置1为好按钮 */
void AIAnalysis::onIsGoodBtn1Clicked()
{
    isGood[0] = true;
    isSelected[0] = true;
    isGoodBtns[0]->setIcon(myIcon.Action_Apply);
    isBadBtns[0]->setIcon(QIcon());
}

/* 响应物料设置界面的设置2为好按钮 */
void AIAnalysis::onIsGoodBtn2Clicked()
{
    isGood[1] = true;
    isSelected[1] = true;
    isGoodBtns[1]->setIcon(myIcon.Action_Apply);
    isBadBtns[1]->setIcon(QIcon());
}

/* 响应物料设置界面的设置3为好按钮 */
void AIAnalysis::onIsGoodBtn3Clicked()
{
    isGood[2] = true;
    isSelected[2] = true;
    isGoodBtns[2]->setIcon(myIcon.Action_Apply);
    isBadBtns[2]->setIcon(QIcon());
}

/* 响应物料设置界面的设置4为好按钮 */
void AIAnalysis::onIsGoodBtn4Clicked()
{
    isGood[3] = true;
    isSelected[3] = true;
    isGoodBtns[3]->setIcon(myIcon.Action_Apply);
    isBadBtns[3]->setIcon(QIcon());
}

/* 响应物料设置界面的设置1为坏按钮 */
void AIAnalysis::onIsBadBtn1Clicked()
{
    isGood[0] = false;
    isSelected[0] = true;
    isGoodBtns[0]->setIcon(QIcon());
    isBadBtns[0]->setIcon(myIcon.Action_Apply);
}

/* 响应物料设置界面的设置2为坏按钮 */
void AIAnalysis::onIsBadBtn2Clicked()
{
    isGood[1] = false;
    isSelected[1] = true;
    isGoodBtns[1]->setIcon(QIcon());
    isBadBtns[1]->setIcon(myIcon.Action_Apply);
}

/* 响应物料设置界面的设置3为坏按钮 */
void AIAnalysis::onIsBadBtn3Clicked()
{
    isGood[2] = false;
    isSelected[2] = true;
    isGoodBtns[2]->setIcon(QIcon());
    isBadBtns[2]->setIcon(myIcon.Action_Apply);
}

/* 响应物料设置界面的设置4为坏按钮 */
void AIAnalysis::onIsBadBtn4Clicked()
{
    isGood[3] = false;
    isSelected[3] = true;
    isGoodBtns[3]->setIcon(QIcon());
    isBadBtns[3]->setIcon(myIcon.Action_Apply);
}

/* 页面切至第一页 */
void AIAnalysis::gotoFirstPageSlt()
{
    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 0, 3);
    stackedWidget->setCurrentIndex(0);
}

/* 获取画框的起始坐标 */
void AIAnalysis::mousePressEvent(QMouseEvent *event)
{
    if(stackedWidget->currentIndex()!=AI_MAIN_PAGE || matterIndex==NO_MATTER || !bRespondMouse)
        return;

    int tmpX = event->pos().x()-offsetX;
    int tmpY = event->pos().y()-offsetY;

    if(tmpX<0 || tmpX>=imageLabel->width() || tmpY<0 || tmpY>=imageLabel->height())
        return;

    labelXPre = tmpX;
    labelYPre = tmpY;
    QString str;
    uchar redValue;
    uchar greenValue;
    uchar blueValue;

    int x = labelXPre/param[matterIndex].iScale+param[matterIndex].iIndex%(param[matterIndex].iScale*2)*param[matterIndex].iSpaceX;
    int y = labelYPre/param[matterIndex].iScale+param[matterIndex].iIndex/(param[matterIndex].iScale*2)*param[matterIndex].iSpaceY;

    redValue = oriBuf[(y*IMAGE_WIDTH+x)*3];
    greenValue = oriBuf[(y*IMAGE_WIDTH+x)*3+1];
    blueValue = oriBuf[(y*IMAGE_WIDTH+x)*3+2];
    str = QString("x=%1,y=%2  R:%3,G:%4,B:%5").arg(labelXPre).arg(labelYPre).arg(redValue).arg(greenValue).arg(blueValue);
    AIWarningLabel->setText(str);
}

/* 获取画框的终止坐标 */
void AIAnalysis::mouseReleaseEvent(QMouseEvent *)
{
    if(stackedWidget->currentIndex()!=AI_MAIN_PAGE || matterIndex==NO_MATTER || m_bIsSimulate)
        return;
    if(param[matterIndex].iSelTimes>=MAX_SELECT_TIMES || !bRespondMouse)
        return;

    if(param[matterIndex].bHasTar){
        param[matterIndex].iSelTimes++;
        reLastBtn->setEnabled(true);
    }

    /*----- 刷新界面上的显示 -----*/
    QImage img(displayBuf,DISPLAY_WID,DISPLAY_HEI,QImage::Format_RGB888);
    imageLabel->setPixmap(QPixmap::fromImage(img));

    param[matterIndex].bHasTar = false;
    changeImage();
}

/* 获取画框的移动坐标 */
void AIAnalysis::mouseMoveEvent(QMouseEvent *event)
{
    if(stackedWidget->currentIndex()!=AI_MAIN_PAGE || matterIndex==NO_MATTER
            || m_bIsSimulate || !bRespondMouse)
        return;

    labelXNext = event->pos().x()-offsetX;
    labelYNext = event->pos().y()-offsetY;

    if(labelXNext<0 || labelXNext>=imageLabel->width() || labelYNext<0 || labelYNext>=imageLabel->height())
        return;

    if(labelXPre>labelXNext || labelYPre>labelYNext)
        return;

    if(param[matterIndex].iSelTimes >= MAX_SELECT_TIMES){
        infoWidget->setLabelText(myLan.ai_exceed_max_times);
        infoWidget->showSecs();
        return;
    }

    // 实现选中后取消选中
    int disbufIndex;
    int changebufIndex;
    if(param[matterIndex].bHasTar){
        for(int i=0; i<displayNum; i++){
            int x = displayPoints[i].x();
            int y = displayPoints[i].y();
            disbufIndex = ((labelYPre-labelYPre%param[matterIndex].iScale+y*param[matterIndex].iScale)*DISPLAY_WID+labelXPre-labelXPre%param[matterIndex].iScale+x*param[matterIndex].iScale)*3;
            changebufIndex = ((startY+y)*IMAGE_WIDTH+startX+x)*3;
            displayBuf[disbufIndex] = changeBuf[changebufIndex];
            displayBuf[disbufIndex+1] = changeBuf[changebufIndex+1];
            displayBuf[disbufIndex+2] = changeBuf[changebufIndex+2];
        }

        if(param[matterIndex].iScale > 1){
            for(int i=0; i<displayNum; i++){
                int x = displayPoints[i].x();
                int y = displayPoints[i].y();
                for(int k=0; k<param[matterIndex].iScale; k++){
                    for(int l=0; l<param[matterIndex].iScale; l++){
                        if(k!=0 || l!=0){
                            disbufIndex = ((labelYPre-labelYPre%param[matterIndex].iScale+y*param[matterIndex].iScale+k)*DISPLAY_WID+labelXPre-labelXPre%param[matterIndex].iScale+x*param[matterIndex].iScale+l)*3;
                            changebufIndex = ((startY+y)*IMAGE_WIDTH+startX+x)*3;
                            displayBuf[disbufIndex] = changeBuf[changebufIndex];
                            displayBuf[disbufIndex+1] = changeBuf[changebufIndex+1];
                            displayBuf[disbufIndex+2] = changeBuf[changebufIndex+2];
                        }//end if
                    }//end for(l)
                }//end for(k)
            }
        }

        QImage img(displayBuf,DISPLAY_WID,DISPLAY_HEI,QImage::Format_RGB888);
        imageLabel->setPixmap(QPixmap::fromImage(img));
    }

    param[matterIndex].bHasTar = false;
    int pixelNum = 0;
    displayNum = 0;

    // 将鼠标移动形成的方框坐标信息映射到原始图片上
    startX = labelXPre/param[matterIndex].iScale+param[matterIndex].iIndex%(param[matterIndex].iScale*2)*param[matterIndex].iSpaceX;
    startY = labelYPre/param[matterIndex].iScale+param[matterIndex].iIndex/(param[matterIndex].iScale*2)*param[matterIndex].iSpaceY;
    endX = labelXNext/param[matterIndex].iScale+param[matterIndex].iIndex%(param[matterIndex].iScale*2)*param[matterIndex].iSpaceX;
    endY = labelYNext/param[matterIndex].iScale+param[matterIndex].iIndex/(param[matterIndex].iScale*2)*param[matterIndex].iSpaceY;
    param[matterIndex].oriRect = QRect(QPoint(startX,startY),QPoint(endX,endY));

    param[matterIndex].iPixelSum = 0;
    for(int i=0; i<param[matterIndex].iSelTimes; i++){
        param[matterIndex].iPixelSum += param[matterIndex].iPixelNum[i];
    }
    param[matterIndex].iDisplaySum = 0;
    for(int i=0; i<param[matterIndex].iSelTimes; i++){
        param[matterIndex].iDisplaySum += param[matterIndex].iDisplayNum[i];
    }

    // 利用预先设置好的阈值筛选出需要的物料像素点
    int tmpWid = param[matterIndex].oriRect.width();
    int tmpHei = param[matterIndex].oriRect.height();
    for(int i=0; i<tmpHei; i++){
        for(int j=0; j<tmpWid; j++){
            int x = startX+j;
            int y = startY+i;
            uchar r = changeBuf[(y*IMAGE_WIDTH+x)*3];
            uchar g = changeBuf[(y*IMAGE_WIDTH+x)*3+1];
            uchar b = changeBuf[(y*IMAGE_WIDTH+x)*3+2];

            if(!pixelIsWanted(r,g,b) || pixelIsSelected(r,g,b))
                continue;
            if(param[matterIndex].iPixelSum >= MAX_PIXEL_NUM)
                continue;
            param[matterIndex].selectedPoint[param[matterIndex].iPixelSum] = QPoint(x, y);
            pixelNum++;
            param[matterIndex].iPixelSum++;
            if(param[matterIndex].bIsOptCal){
                if(pixelIsReSelected(r,g,b))
                    continue;
            }
            if(param[matterIndex].bIsOptSam){
                if(pixelIsEdge(x, y))
                    continue;
            }

            param[matterIndex].bHasTar = true;
            param[matterIndex].selBuf[3*param[matterIndex].iDisplaySum] = r;
            param[matterIndex].selBuf[3*param[matterIndex].iDisplaySum+1] = g;
            param[matterIndex].selBuf[3*param[matterIndex].iDisplaySum+2] = b;

            displayPoints[displayNum] = QPoint(j,i);
            displayNum++;
            param[matterIndex].iDisplaySum++;
        }
    }

    param[matterIndex].iPixelNum[param[matterIndex].iSelTimes] = pixelNum;
    param[matterIndex].iDisplayNum[param[matterIndex].iSelTimes] = displayNum;
    showMesLabel();

    // 将数据根据一定的缩放比列显示出来
    if(param[matterIndex].bHasTar){
        for(int i=0; i<displayNum; i++){
            int x = displayPoints[i].x();
            int y = displayPoints[i].y();
            disbufIndex = ((labelYPre-labelYPre%param[matterIndex].iScale+y*param[matterIndex].iScale)*DISPLAY_WID+labelXPre-labelXPre%param[matterIndex].iScale+x*param[matterIndex].iScale)*3;
            displayBuf[disbufIndex] = param[matterIndex].displayColor[0];
            displayBuf[disbufIndex+1] = param[matterIndex].displayColor[1];
            displayBuf[disbufIndex+2] = param[matterIndex].displayColor[2];
        }

        if(param[matterIndex].iScale > 1){
            for(int i=0; i<displayNum; i++){
                int x = displayPoints[i].x();
                int y = displayPoints[i].y();
                for(int k=0; k<param[matterIndex].iScale; k++){
                    for(int l=0; l<param[matterIndex].iScale; l++){
                        if(k!=0 || l!=0){
                            disbufIndex = ((labelYPre-labelYPre%param[matterIndex].iScale+y*param[matterIndex].iScale+k)*DISPLAY_WID+labelXPre-labelXPre%param[matterIndex].iScale+x*param[matterIndex].iScale+l)*3;
                            displayBuf[disbufIndex] = param[matterIndex].displayColor[0];
                            displayBuf[disbufIndex+1] = param[matterIndex].displayColor[1];
                            displayBuf[disbufIndex+2] = param[matterIndex].displayColor[2];
                        }//end if
                    }//end for(l)
                }//end for(k)
            }
        }
    }

    uchar upLineBuf[IMAGE_WIDTH*3];
    uchar DownLineBuf[IMAGE_WIDTH*3];
    uchar leftLineBuf[IMAGE_HEIGHT*3];
    uchar rightLineBuf[IMAGE_HEIGHT*3];

    // 将框选的数据拷贝到缓冲区中
    memcpy(upLineBuf, displayBuf+(labelYPre*DISPLAY_WID+labelXPre)*3, (labelXNext-labelXPre+1)*3);
    memcpy(DownLineBuf, displayBuf+(labelYNext*DISPLAY_WID+labelXPre)*3, (labelXNext-labelXPre+1)*3);
    int lineIndex;
    for(int i=labelYPre; i<labelYNext+1; i++){
        lineIndex = (i-labelYPre)*3;
        disbufIndex = (i*DISPLAY_WID+labelXPre)*3;
        leftLineBuf[lineIndex] = displayBuf[disbufIndex];
        leftLineBuf[lineIndex+1] = displayBuf[disbufIndex+1];
        leftLineBuf[lineIndex+2] = displayBuf[disbufIndex+2];
        disbufIndex = (i*DISPLAY_WID+labelXNext)*3;
        rightLineBuf[lineIndex] = displayBuf[disbufIndex];
        rightLineBuf[lineIndex+1] = displayBuf[disbufIndex+1];
        rightLineBuf[lineIndex+2] = displayBuf[disbufIndex+2];
    }

    // 实现框选后显示外部框
    for(int i=labelXPre; i<labelXNext+1; i++){
        disbufIndex = (labelYPre*DISPLAY_WID+i)*3;
        displayBuf[disbufIndex] = 255;
        displayBuf[disbufIndex+1] = 255;
        displayBuf[disbufIndex+2] = 255;
        disbufIndex = (labelYNext*DISPLAY_WID+i)*3;
        displayBuf[disbufIndex] = 255;
        displayBuf[disbufIndex+1] = 255;
        displayBuf[disbufIndex+2] = 255;
    }    

    for(int i=labelYPre; i<labelYNext+1; i++){
        disbufIndex = (i*DISPLAY_WID+labelXPre)*3;
        displayBuf[disbufIndex] = 255;
        displayBuf[disbufIndex+1] = 255;
        displayBuf[disbufIndex+2] = 255;
        disbufIndex = (i*DISPLAY_WID+labelXNext)*3;
        displayBuf[disbufIndex] = 255;
        displayBuf[disbufIndex+1] = 255;
        displayBuf[disbufIndex+2] = 255;
    }

    // 刷新界面上的显示
    QImage img(displayBuf,DISPLAY_WID,DISPLAY_HEI,QImage::Format_RGB888);
    imageLabel->setPixmap(QPixmap::fromImage(img));

    // 取消框选后显示的外部框
    memcpy(displayBuf+(labelYPre*DISPLAY_WID+labelXPre)*3, upLineBuf, (labelXNext-labelXPre+1)*3);
    memcpy(displayBuf+(labelYNext*DISPLAY_WID+labelXPre)*3, DownLineBuf, (labelXNext-labelXPre+1)*3);
    for(int i=labelYPre; i<labelYNext+1; i++){
        disbufIndex = (i*DISPLAY_WID+labelXPre)*3;
        lineIndex = (i-labelYPre)*3;
        displayBuf[disbufIndex] = leftLineBuf[lineIndex];
        displayBuf[disbufIndex+1] = leftLineBuf[lineIndex+1];
        displayBuf[disbufIndex+2] = leftLineBuf[lineIndex+2];

        disbufIndex = (i*DISPLAY_WID+labelXNext)*3;
        lineIndex = (i-labelYPre)*3;
        displayBuf[disbufIndex] = rightLineBuf[lineIndex];
        displayBuf[disbufIndex+1] = rightLineBuf[lineIndex+1];
        displayBuf[disbufIndex+2] = rightLineBuf[lineIndex+2];
    }
}

/* 根据需要更新显示的照片 */
void AIAnalysis::updateImage()
{
    param[matterIndex].iSpaceX = (IMAGE_WIDTH-DISPLAY_WID/param[matterIndex].iScale)/param[matterIndex].iMaxMoveTimes;
        param[matterIndex].iSpaceY = (IMAGE_HEIGHT-DISPLAY_HEI/param[matterIndex].iScale)/param[matterIndex].iMaxMoveTimes;

        int tmpWid = DISPLAY_WID/param[matterIndex].iScale;
        int tmpHei = DISPLAY_HEI/param[matterIndex].iScale;
        int disbufIndex;
        int changebufIndex;
        for(int i=0; i<tmpHei; i++){
            for(int j=0; j<tmpWid; j++){
                disbufIndex = (i*param[matterIndex].iScale*DISPLAY_WID+j*param[matterIndex].iScale)*3;
                changebufIndex = ((i+param[matterIndex].iIndex/(param[matterIndex].iScale*2)*param[matterIndex].iSpaceY)*IMAGE_WIDTH+j+param[matterIndex].iIndex%(param[matterIndex].iScale*2)*param[matterIndex].iSpaceX)*3;
                displayBuf[disbufIndex] = changeBuf[changebufIndex];
                displayBuf[disbufIndex+1] = changeBuf[changebufIndex+1];
                displayBuf[disbufIndex+2] = changeBuf[changebufIndex+2];
            }
        }

        /* 若尺寸比原始尺寸大，则需要填充显示buf中未赋值的像素点 */
        int changeIndex;
        int baseIndex;
        if(param[matterIndex].iScale > 1){
            tmpWid = DISPLAY_WID/param[matterIndex].iScale;
            tmpHei = DISPLAY_HEI/param[matterIndex].iScale;
            for(int i=0; i<tmpHei; i++){
                for(int j=0; j<tmpWid; j++){
                    for(int k=0; k<param[matterIndex].iScale; k++){
                        for(int l=0; l<param[matterIndex].iScale; l++){
                            if(k!=0 || l!=0){
                                changeIndex = ((i*param[matterIndex].iScale+k)*DISPLAY_WID+j*param[matterIndex].iScale+l)*3;
                                baseIndex = ((i*param[matterIndex].iScale)*DISPLAY_WID+j*param[matterIndex].iScale)*3;
                                displayBuf[changeIndex] = displayBuf[baseIndex];
                                displayBuf[changeIndex+1] = displayBuf[baseIndex+1];
                                displayBuf[changeIndex+2] = displayBuf[baseIndex+2];
                            }//end if
                        }//end for(l)
                    }//end for(k)
                }//end for(j)
            }//end for(i)

            /* 解决图片放大八倍后最后四行无数据的问题 */
            tmpHei = DISPLAY_HEI-1-DISPLAY_HEI%param[matterIndex].iScale;
            for(int i=DISPLAY_HEI-1; i>tmpHei; i--){
                for(int j=0; j<DISPLAY_WID; j++){
                    baseIndex = ((DISPLAY_HEI-1-DISPLAY_HEI%param[matterIndex].iScale)*DISPLAY_WID+j)*3;
                    changeIndex = (i*DISPLAY_WID+j)*3;
                    displayBuf[changeIndex] = displayBuf[baseIndex];
                    displayBuf[changeIndex+1] = displayBuf[baseIndex+1];
                    displayBuf[changeIndex+2] = displayBuf[baseIndex+2];
                }
            }
        }//end if

        QImage img(displayBuf,DISPLAY_WID,DISPLAY_HEI,QImage::Format_RGB888);
        imageLabel->setPixmap(QPixmap::fromImage(img));
}

/* 根据需要改变中间层的数据changeBuf */
void AIAnalysis::changeImage()
{
    int pointIndex = 0;
    param[matterIndex].iDisplaySum = 0;
    for(int i=0; i<param[matterIndex].iSelTimes-1; i++){
        pointIndex += param[matterIndex].iPixelNum[i];
        param[matterIndex].iDisplaySum += param[matterIndex].iDisplayNum[i];
    }

    for(int i=pointIndex; i<param[matterIndex].iPixelSum; i++){
        int x = param[matterIndex].selectedPoint[i].x();
        int y = param[matterIndex].selectedPoint[i].y();
        uchar r = oriBuf[(y*IMAGE_WIDTH+x)*3];
        uchar g = oriBuf[(y*IMAGE_WIDTH+x)*3+1];
        uchar b = oriBuf[(y*IMAGE_WIDTH+x)*3+2];

        if(param[matterIndex].bIsOptCal){
            if(pixelIsReSelected(r,g,b))
                continue;
        }
        if(param[matterIndex].bIsOptSam){
            if(pixelIsEdge(x, y))
                continue;
        }

        changeBuf[(y*IMAGE_WIDTH+x)*3] = param[matterIndex].displayColor[0];
        changeBuf[(y*IMAGE_WIDTH+x)*3+1] = param[matterIndex].displayColor[1];
        changeBuf[(y*IMAGE_WIDTH+x)*3+2] = param[matterIndex].displayColor[2];

        param[matterIndex].selBuf[3*param[matterIndex].iDisplaySum] = r;
        param[matterIndex].selBuf[3*param[matterIndex].iDisplaySum+1] = g;
        param[matterIndex].selBuf[3*param[matterIndex].iDisplaySum+2] = b;
        param[matterIndex].iDisplaySum++;
    }
}

/* 根据优化计算和优化取样的标志位刷新显示 */
void AIAnalysis::refreshDisplay()
{
    int displayNum;
    int beginIndex = 0;
    uchar rgb[3];

    param[matterIndex].iDisplaySum = 0;
    for(int i=0; i<param[matterIndex].iSelTimes; i++){
        displayNum = 0;
        for(int j=0; j<param[matterIndex].iPixelNum[i]; j++){
            int x = param[matterIndex].selectedPoint[beginIndex+j].x();
            int y = param[matterIndex].selectedPoint[beginIndex+j].y();
            int index = (y*IMAGE_WIDTH+x)*3;

            memcpy(rgb, oriBuf+index, 3);
            memcpy(changeBuf+index, rgb, 3);
            uchar r = rgb[0];
            uchar g = rgb[1];
            uchar b = rgb[2];

            if(param[matterIndex].bIsOptCal){
                if(pixelIsReSelected(r,g,b))
                    continue;
            }
            if(param[matterIndex].bIsOptSam){
                if(pixelIsEdge(x, y))
                    continue;
            }

            memcpy(changeBuf+index, param[matterIndex].displayColor, 3);
            int bufIndex = 3*param[matterIndex].iDisplaySum;
            memcpy(param[matterIndex].selBuf+bufIndex, rgb, 3);

            displayNum++;
            param[matterIndex].iDisplaySum++;
        }
        param[matterIndex].iDisplayNum[i] = displayNum;
        beginIndex += param[matterIndex].iPixelNum[i];
    }

    updateImage();
    refreshBtnStatus();
    showMesLabel();
}

/* 显示图片仿真后的效果 */
void AIAnalysis::displaySimulate()
{
    int goodSum = 0;
    int badSum = 0;
    bool bIsBad = false;

    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i]){
            if(isGood[i])
                goodSum++;
            else
                badSum++;
        }
    }

    for(int i=0; i<IMAGE_HEIGHT; i++){
        for(int j=0; j<IMAGE_WIDTH; j++){
            bIsBad = false;
            uchar r = oriBuf[(i*IMAGE_WIDTH+j)*3];
            uchar g = oriBuf[(i*IMAGE_WIDTH+j)*3+1];
            uchar b = oriBuf[(i*IMAGE_WIDTH+j)*3+2];

            if(pixelIsBackground(r, g, b)){
                changeBuf[(i*IMAGE_WIDTH+j)*3] = r;
                changeBuf[(i*IMAGE_WIDTH+j)*3+1] = g;
                changeBuf[(i*IMAGE_WIDTH+j)*3+2] = b;
                continue;
            }

            if(struCnfp.nIntelMode == 1){   // 多分类智能算法
                if(multiPixelIsBad(r, g, b)){
                    changeBuf[(i*IMAGE_WIDTH+j)*3] = param[matterIndex].displayColor[0];
                    changeBuf[(i*IMAGE_WIDTH+j)*3+1] = param[matterIndex].displayColor[1];
                    changeBuf[(i*IMAGE_WIDTH+j)*3+2] = param[matterIndex].displayColor[2];
                    continue;
                }
            } else {                        // 普通智能算法
                switch(goodSum){
                case 1:
                    if(badSum == 1){
                        if(pixelIsBad(0,r,g,b))
                            bIsBad = true;
                    }
                    else if(badSum == 2){
                        if(pixelIsBad(0,r,g,b) || pixelIsBad(1,r,g,b))
                            bIsBad = true;
                    }
                    else if(badSum == 3){
                        if(pixelIsBad(0,r,g,b) || pixelIsBad(1,r,g,b) || pixelIsBad(2,r,g,b))
                        bIsBad = true;
                    }
                    break;
                case 2:
                    if(badSum == 1){
                        if(pixelIsBad(0,r,g,b) && pixelIsBad(1,r,g,b))
                            bIsBad = true;
                    }
                    else if(badSum == 2){
                        if((pixelIsBad(0,r,g,b) && pixelIsBad(2,r,g,b))
                                || (pixelIsBad(1,r,g,b) && pixelIsBad(3,r,g,b)))
                            bIsBad = true;
                    }
                    break;
                case 3:
                    if(badSum == 1){
                        if(pixelIsBad(0,r,g,b) && pixelIsBad(1,r,g,b) && pixelIsBad(2,r,g,b))
                            bIsBad = true;
                    }
                    break;
                default:
                    break;
                }
            }//end if

            if(bIsBad){
                changeBuf[(i*IMAGE_WIDTH+j)*3] = param[matterIndex].displayColor[0];
                changeBuf[(i*IMAGE_WIDTH+j)*3+1] = param[matterIndex].displayColor[1];
                changeBuf[(i*IMAGE_WIDTH+j)*3+2] = param[matterIndex].displayColor[2];
            }
            else{
                changeBuf[(i*IMAGE_WIDTH+j)*3] = r;
                changeBuf[(i*IMAGE_WIDTH+j)*3+1] = g;
                changeBuf[(i*IMAGE_WIDTH+j)*3+2] = b;
            }
        }//end j
    }//end i

    updateImage();
    refreshBtnStatus();
    showMesLabel();
}

/* 判断原始图像像素点是否为背景点 */
bool AIAnalysis::pixelIsBackground(uchar r, uchar g, uchar b)
{
    int backgroudID = struGsh.nUnit % 2;
    switch(struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID]){
    case 0: //黑
        if(r+g+b < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3)
            return true;
        break;
    case 1: //红
        if(r>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0*r/(r+g+b)>struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 2: //绿
        if(g>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0*g/(r+g+b)>struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 4: //蓝
        if(b>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0*b/(r+g+b)>struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 7: //白
        if(r+g+b>struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3 &&
                r+g+b<struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID]*3+(int)struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID]*2.55)
            return true;
        break;
    default:
        break;
    }
    return false;
}

/* 判断像素点是否为想要的物料点 */
bool AIAnalysis::pixelIsWanted(uchar r, uchar g, uchar b)
{
    if(pixelIsBackground(r,g,b))
        return false;

    switch(struGsh.nIntelSampColor[matterIndex]){
    case 1: //红
        if(struGsh.nIntelSampMode[matterIndex] == 0){  //选暗
            if(r > struGsh.nIntelSampSens[matterIndex])
                return false;
        }
        else{   //选亮
            if(r < struGsh.nIntelSampSens[matterIndex])
                return false;
        }
        break;
    case 2: //绿
        if(struGsh.nIntelSampMode[matterIndex] == 0){  //选暗
            if(g > struGsh.nIntelSampSens[matterIndex])
                return false;
        }
        else{   //选亮
            if(g < struGsh.nIntelSampSens[matterIndex])
                return false;
        }
        break;
    case 4: //蓝
        if(struGsh.nIntelSampMode[matterIndex] == 0){  //选暗
            if(b > struGsh.nIntelSampSens[matterIndex])
                return false;
        }
        else{   //选亮
            if(b < struGsh.nIntelSampSens[matterIndex])
                return false;
        }
        break;
    case 6: // 红绿色差
        if (struGsh.nIntelSampMode[matterIndex] == 0) { // 选暗
            if ((100.0*r/g) > struGsh.nIntelSampSens[matterIndex])
            return false;
        }
        else { // 选亮
            if ((100.0*r/g) < struGsh.nIntelSampSens[matterIndex])
            return false;
        }
        break;
    case 8: // 红蓝色差
        if (struGsh.nIntelSampMode[matterIndex] == 0) { // 选暗
            if ((100.0*r/b) > struGsh.nIntelSampSens[matterIndex])
            return false;
        }
        else { // 选亮
            if ((100.0*r/b) < struGsh.nIntelSampSens[matterIndex])
            return false;
        }
        break;
    case 10: // 绿蓝色差
        if (struGsh.nIntelSampMode[matterIndex] == 0) { // 选暗
            if ((100.0*g/b) > struGsh.nIntelSampSens[matterIndex])
            return false;
        }
        else { // 选亮
            if ((100.0*g/b) < struGsh.nIntelSampSens[matterIndex])
            return false;
        }
        break;
    default:
        break;
    }

    return true;
}

/* 判断图像中像素点为原始数据点还是被标记的点 */
bool AIAnalysis::pixelIsSelected(uchar r, uchar g, uchar b)
{
    if(r==param[matterIndex].displayColor[0] && g==param[matterIndex].displayColor[1]
            && b==param[matterIndex].displayColor[2])
        return true;
    return false;
}

/* 优化计算时判断像素点是否为冗余点 */
bool AIAnalysis::pixelIsReSelected(uchar r, uchar g, uchar b)
{
    for(int i=0; i<param[matterIndex].iDisplaySum; i++){
        if(r==param[matterIndex].selBuf[3*i] && g==param[matterIndex].selBuf[3*i+1]
                && b==param[matterIndex].selBuf[3*i+2])
            return true;
    }
    return false;
}

/* 优化取样时判断像素点是否为边缘点 */
bool AIAnalysis::pixelIsEdge(int x, int y)
{
    if(x==0 || y==0 || x==IMAGE_WIDTH-1 || y==IMAGE_HEIGHT-1)
        return false;

    uchar r, g, b;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
        //    if(i!=0 || j!=0){
        //    if(abs(i) != abs(j)){
	      if(i==0 && j==-1){
                r = oriBuf[((y+j)*IMAGE_WIDTH+(x+i))*3];
                g = oriBuf[((y+j)*IMAGE_WIDTH+(x+i))*3+1];
                b = oriBuf[((y+j)*IMAGE_WIDTH+(x+i))*3+2];
                if(pixelIsBackground(r, g, b))
                    return true;
            }
        }
    }
    return false;
}

/* 优化取样时判断像素点是否为边缘点 */
bool AIAnalysis::pixelIsEdgeImage(QImage *image,int x, int y)
{
    if(x==0 || y==0 || x==IMAGE_WIDTH-1 || y==IMAGE_HEIGHT-1)
        return false;

    QColor rgb;
    for(int m=-1; m<=1; m++){
        for(int n=-1; n<=1; n++){
            if(m!=0 || n!=0){
                rgb = image->pixel(x+n,y+m);
                if(pixelIsBackground(rgb.red(), rgb.green(), rgb.blue()))
                    return true;
            }
        }
    }
    return false;
}
/* 根据选定的方程判断像素点是否为坏点 */
bool AIAnalysis::pixelIsBad(int index, uchar r, uchar g, uchar b)
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

/* 提示框中显示不同的信息 */
void AIAnalysis::showMesLabel()
{
    QString statusStr = (isGood[matterIndex]==true)?myLan.ai_good:myLan.ai_bad;
    QString str;
    QString name = QString::fromLocal8Bit(struCnfp.struIntelMat[matterIndex].name);
    str = QString("%1/%2/%3").arg(name).arg(statusStr)
            .arg(matterPixelSum[matterIndex]+param[matterIndex].iDisplaySum);

    mesLabel->setText(str);
}

/* 显示当前层和前后视信息 */
void AIAnalysis::showInfoLabel()
{
   infoLabel->setText("");
}

/* 设置高亮显示的颜色 */
void AIAnalysis::setDisplayColor(int index)
{
    switch(struGsh.nIntelSampColor[index]){
    case 1:// red
        param[index].displayColor[0] = struGsh.nIntelSampSens[index];
        param[index].displayColor[1] = 0;
        param[index].displayColor[2] = 255;
        break;
    case 2:// green
        param[index].displayColor[0] = 255;
        param[index].displayColor[1] = struGsh.nIntelSampSens[index];
        param[index].displayColor[2] = 0;
        break;
    case 4:// blue
        param[index].displayColor[0] = 255;
        param[index].displayColor[1] = 0;
        param[index].displayColor[2] = struGsh.nIntelSampSens[index];
        break;
    case 6://red-green
        param[index].displayColor[0] = struGsh.nIntelSampSens[index];
        param[index].displayColor[1] = 0;
        param[index].displayColor[2] = 255;
        break;
    case 8://red-blue
        param[index].displayColor[0] = 255;
        param[index].displayColor[1] = struGsh.nIntelSampSens[index];
        param[index].displayColor[2] = 0;
        break;
    case 10://green-blue
        param[index].displayColor[0] = 255;
        param[index].displayColor[1] = 0;
        param[index].displayColor[2] = struGsh.nIntelSampSens[index];
        break;
    default:
        break;
    }
}

/* 判断采集到的图像是否为去背景图像 */
bool AIAnalysis::imageIsSubBackground()
{
    char buf[24];
    int ret;
    MySerial.com1Write(CMD_UNIT_CAMERA_VERSION, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, 1, 0, 3);
    myFlow.sleep(1);
    memset(buf, 0, sizeof(buf));
    ret = MySerial.com1Read(buf, 24);
    qDebug("subbackground ret = %d", ret);

    if(ret == 24) {
        if(buf[14] == 0x02)
            return true;
    }

    return false;
}

/* 获取去背景图像中的背景 */
QColor AIAnalysis::getBackgroundColor(char *buf, QSize tmpSize)
{
    QColor backColor(0, 0, 0);
    for(int i=0; i<tmpSize.height(); i++) {
        for(int j=0; j<tmpSize.width(); j++) {
            uchar r = buf[(i*IMAGE_WIDTH+j)*3];
            uchar g = buf[(i*IMAGE_WIDTH+j)*3+1];
            uchar b = buf[(i*IMAGE_WIDTH+j)*3+2];

            if( pixelIsBackground(r, g, b) ) {
                backColor.setRed(r);
                backColor.setGreen(g);
                backColor.setBlue(b);
                return backColor;
            }
        }
        break;
    }

    return backColor;
}

/* 通过串口获取1024分辨率图片 */
bool AIAnalysis::getImageFromUART_1024()
{
    /* 定义每一帧的数据长度为3600(1024*3+512+16) */
    int lenOfFrame = 3600;
    char *p = struGsh.sRgbRow;
    char tmpColorBuf[IMAGE_WIDTH*IMAGE_HEIGHT*3];
    char tmpInfBuf[INF_WIDTH*INF_HEIGHT*3];
    int ret;
    int maxTimes = 10;
    int times = 0;
    int count = 0;
    int per;

    /* 针对采集图像去背景设置的变量 */
    int nImgWid = 1024;
    int nImgHei = 500;
    bool bIsSubBackground = false;

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 0, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();
    bIsSubBackground = imageIsSubBackground();
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0,(IMAGE_HEIGHT-1)/256,(IMAGE_HEIGHT-1)%256, 1);
    
    /* 若采集图像为去背景后图像,则重新设置采集图像的宽高及每行数据的大小 */
    if( bIsSubBackground ) {
        count = 0;
        times = 0;

        /* 由于下位机回包为3次,同时上位机不清空缓冲区,因此包的长度定义为正常包长度的3倍 */
        int tmpLen = 72;
        char buf[72] = {0};

        while(count < tmpLen) {
            ret = MySerial.com1Read(buf+count, tmpLen-count);
            times++;
            count += ret;
            if(times >= maxTimes) {
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->showSecs();
                return false;
            }
            myFlow.sleep(1);
        }

        if (ret == tmpLen) {
            nImgHei = buf[16]*256+buf[17];
            lenOfFrame = buf[14]*256+buf[15]+16;

            /* 判断一帧图像数据去除包头包尾及红外数据后是否合法 */
            if( (lenOfFrame-512-16)%3 ) {
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->showSecs();
                return false;
            }
            nImgWid = (lenOfFrame-512-16)/3;
            qDebug("MSG : width=%d, height=%d, len=%d", nImgWid, nImgHei, lenOfFrame);
        }
    }

    count = 0;
    times = 0;
    while(count < lenOfFrame){
        ret = MySerial.com1Read(p+count,lenOfFrame-count);
        count += ret;
        times++;
        if(times >= maxTimes){
            infoWidget->setLabelText(myLan.ai_image_capture_error);
            infoWidget->showSecs();
            return false;
        }
        myFlow.sleep(1);
    }

    if(p[0]==0xAA && p[1]==0xAA && p[2]==0xAA && p[3]==0xAA){
        memcpy(tmpColorBuf, p+10, nImgWid*3);

        for(int j=0; j<INF_WIDTH; j++){
            tmpInfBuf[j*3] = p[j+IMAGE_WIDTH*3+10];
            tmpInfBuf[j*3+1] = p[j+IMAGE_WIDTH*3+10];
            tmpInfBuf[j*3+2] = p[j+IMAGE_WIDTH*3+10];
        }
    }
    else{
        infoWidget->setLabelText(myLan.ai_image_capture_error);
        infoWidget->showSecs();
        return false;
    }

    /* 将串口获取到的数据填充到原始图片的缓冲区中 */
    for(int i=1; i<nImgHei; i++){
        times = 0;
        count = 0;
        per = (i*100)/nImgHei;
        memset(p, 0, lenOfFrame);
        while(count < lenOfFrame){
            ret = MySerial.com1Read(p+count,lenOfFrame-count);
            count += ret;
            times++;
            if(times >= maxTimes){
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->showSecs();
                return false;
            }

            myFlow.msleep(100);
        }

        /* 判断包头是否正确 */
        if(p[0]==0xAA && p[1]==0xAA && p[2]==0xAA && p[3]==0xAA){
            memcpy(tmpColorBuf+i*IMAGE_WIDTH*3, p+10, nImgWid*3);
            if(i%2 == 0){
                for(int j=0; j<INF_WIDTH; j++){
                    /* 除2是因为红外图像的高度为彩色图像高度的一半 */
                    tmpInfBuf[((i/2)*INF_WIDTH+j)*3] = p[j+IMAGE_WIDTH*3+10];
                    tmpInfBuf[((i/2)*INF_WIDTH+j)*3+1] = p[j+IMAGE_WIDTH*3+10];
                    tmpInfBuf[((i/2)*INF_WIDTH+j)*3+2] = p[j+IMAGE_WIDTH*3+10];
                }
            }
            QString str = QString("%1%2%").arg(myLan.ai_image_capturing).arg(per);
            infoWidget->setLabelText(str);
        }
        else{
            infoWidget->setLabelText(myLan.ai_image_capture_error);
            infoWidget->showSecs();
            return false;
        }
    }

    /* 若采集图像为去背景图像,则以背景颜色填充空白位置 */
    if( bIsSubBackground ) {
        QColor backColor = getBackgroundColor(tmpColorBuf, QSize(nImgWid, nImgHei));
        for(int i=0; i<nImgHei; i++) {
            for(int j=nImgWid; j<IMAGE_WIDTH; j++) {
                tmpColorBuf[(i*IMAGE_WIDTH+j)*3] = backColor.red();
                tmpColorBuf[(i*IMAGE_WIDTH+j)*3+1] = backColor.green();
                tmpColorBuf[(i*IMAGE_WIDTH+j)*3+2] = backColor.blue();
            }
        }

        for(int i=nImgHei; i<IMAGE_HEIGHT; i++) {
            for(int j=0; j<IMAGE_WIDTH; j++) {
                tmpColorBuf[(i*IMAGE_WIDTH+j)*3] = backColor.red();
                tmpColorBuf[(i*IMAGE_WIDTH+j)*3+1] = backColor.green();
                tmpColorBuf[(i*IMAGE_WIDTH+j)*3+2] = backColor.blue();
            }
        }
    }

    /* 将图像数据保存成位图文件 */
    QString colorImageName = QString("%1.png").arg(imgListNum);
    saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar *)tmpColorBuf);
    infoWidget->hide();
    return true;
}

/* 通过串口获取2048分辨率图片,暂时不考虑红外图片 */
bool AIAnalysis::getImageFromUART_2048()
{
    /* 定义每一帧数据的长度为6160(2048*3+16) */
    int lenOfFrame = 6160;
    char *p = struGsh.sRgbRow;
    char tmpColorBuf[IMAGE_WIDTH*IMAGE_HEIGHT*3];
    int ret;
    int maxTimes = 10;
    int times = 0;
    int count = 0;
    int per;
    bool bIsLeft = false;

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 0, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0,(IMAGE_HEIGHT-1)/256,(IMAGE_HEIGHT-1)%256, 1);

    while(count < lenOfFrame){
        ret = MySerial.com1Read(p+count, lenOfFrame-count);
        count += ret;
        times++;
        if(times >= maxTimes){
            infoWidget->setLabelText(myLan.ai_image_capture_error);
            infoWidget->showSecs();
            return false;
        }
        myFlow.sleep(1);
    }

    if(p[0]==0xAA && p[1]==0xAA && p[2]==0xAA && p[3]==0xAA){
        for(int i=0; i<1024; i++)
        {
            uchar r = p[i*3+10];
            uchar g = p[i*3+10+1];
            uchar b = p[i*3+10+2];

            if(pixelIsWanted(r, g, b))
            {
                bIsLeft = true;
                break;
            }
        }

        if(bIsLeft)
            memcpy(tmpColorBuf+(IMAGE_HEIGHT-1)*IMAGE_WIDTH*3, p+10, IMAGE_WIDTH*3);
        else
            memcpy(tmpColorBuf+(IMAGE_HEIGHT-1)*IMAGE_WIDTH*3, p+IMAGE_WIDTH*3+10, IMAGE_WIDTH*3);
    }
    else{
        infoWidget->setLabelText(myLan.ai_image_capture_error);
        infoWidget->showSecs();
        return false;
    }

    //将串口获取到的数据填充到原始图片的缓冲区中
    for(int i=1; i<IMAGE_HEIGHT; i++){
        times = 0;
        count = 0;
        per = (i*100)/IMAGE_HEIGHT;
        memset(p, 0, lenOfFrame);
        while(count < lenOfFrame){
            ret = MySerial.com1Read(p+count, lenOfFrame-count);
            count += ret;
            times++;
            if(times >= maxTimes){
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->showSecs();
                return false;
            }

            myFlow.msleep(200);
        }

        /* 判断包头是否正确 */
        if(p[0]==0xAA && p[1]==0xAA && p[2]==0xAA && p[3]==0xAA){
            memcpy(tmpColorBuf+IMAGE_WIDTH*(IMAGE_HEIGHT-1-i)*3, p+10, IMAGE_WIDTH*3);
            QString str = QString("%1%2%").arg(myLan.ai_image_capturing).arg(per);
            infoWidget->setLabelText(str);
        }
        else{
            infoWidget->setLabelText(myLan.ai_image_capture_error);
            infoWidget->showSecs();
            return false;
        }
    }

    //将图像数据保存成位图文件
    QString colorImageName = QString("%1.png").arg(imgListNum);
    saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar *)tmpColorBuf);

    infoWidget->hide();
    return true;
}

/* 获取一帧数据的头指针 */
char *AIAnalysis::getHead(const char *mem, int size, const char *str)
{
    long lmem=size;
    char *cp = NULL;
    char *s1 = NULL;
    char *s2 = NULL;

    cp=(char*)mem;
    if((mem==NULL) || (str==NULL) || (size<=0)){
        return NULL;
    }

    if(!str){
        return ((char *)mem);
    }
    while(lmem>0){
        s1=cp;
        s2=(char *)str;
        while(*s1&&s2&&!(*s1-*s2)){
            s1++;
            s2++;
        }
        if(!*s2){
            return cp+10;
        }
        cp++;
        lmem--;
    }
    return NULL;

}

/* 通过USB获取1024分辨率图片 */
bool AIAnalysis::getImageFromUSB_1024()
{
#ifdef Q_OS_UNIX
    int usbReturn;
    uchar tmpColorBuf[IMAGE_WIDTH*3*IMAGE_HEIGHT];
    uchar tmpBuf[(IMAGE_WIDTH*3+INF_WIDTH+16)*IMAGE_HEIGHT+2240];
    uchar tmpInfBuf[INF_WIDTH*INF_HEIGHT*3];
    char *pImageHead;
    uchar imageHead[5]={0xAA,0xAA,0xAA,0xAA,0};

    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();

    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        infoWidget->setLabelText(myLan.msg_usb_init_error);
        infoWidget->showSecs();
        return false;
    }

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 1, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, struGsh.nUnit, 0 ,0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    MySerial.com1Write(2, 2, 0, 0x3f,  0, 0, 0, 0, 0, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 1, (IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1);

    bzero(tmpBuf,(IMAGE_WIDTH*3+512+16)*IMAGE_HEIGHT+2240);
    usbReturn = usb_bulk_read(myUsbDev,USB_EP_IN_AI,(char *)tmpBuf,(IMAGE_WIDTH*3+512+16)*IMAGE_HEIGHT+2240,20*1000);
    pImageHead = (char *)tmpBuf;
    qDebug("usbReturn = %d", usbReturn);
    if(usbReturn == (IMAGE_WIDTH*3+512+16)*IMAGE_HEIGHT+2240){
        for(int i=0; i<IMAGE_HEIGHT; i++){
            //判断包头数据是否正确
            if((pImageHead=getHead((char *)pImageHead,(IMAGE_WIDTH*3+512+16)*3,(char *)imageHead)) != NULL){
                memcpy(tmpColorBuf+IMAGE_WIDTH*3*(IMAGE_HEIGHT-1-i),pImageHead,IMAGE_WIDTH*3);
                if(i%2 == 0){
                    for(int j=0; j<INF_WIDTH; j++){
                        tmpInfBuf[(INF_WIDTH*(INF_HEIGHT-1-i/2)+j)*3] = *(pImageHead+IMAGE_WIDTH*3+j);
                    }
                }
            }
            else{
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->showSecs();
                return false;
            }
        }

        MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);

        for(int i=0; i<INF_HEIGHT; i++){
            for(int j=0; j<INF_WIDTH; j++){
                tmpInfBuf[(i*INF_WIDTH+j)*3+1] = tmpInfBuf[(i*INF_WIDTH+j)*3];
                tmpInfBuf[(i*INF_WIDTH+j)*3+2] = tmpInfBuf[(i*INF_WIDTH+j)*3];
            }
        }
    }
    else {
	    infoWidget->hide();
        return false;
    }

    //将图像数据保存成位图文件
    QString colorImageName = QString("%1.png").arg(imgListNum);
    qDebug()<<imgListNum;
    saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar *)tmpColorBuf);

    infoWidget->hide();

    return true;
#endif
}

/* 通过USB获取2048分辨率图片,暂时不考虑红外图片 */
bool AIAnalysis::getImageFromUSB_2048()
{
#ifdef Q_OS_UNIX
    int usbReturn;
    int bufSize = 6160*IMAGE_HEIGHT+2240;
    uchar tmpColorBuf[IMAGE_WIDTH*3*IMAGE_HEIGHT];
    uchar tmpBuf[bufSize];
    uchar tmpInfBuf[INF_WIDTH*INF_HEIGHT*3];
    char *pImageHead;
    uchar imageHead[5]={0xAA,0xAA,0xAA,0xAA,0};
    bool bIsLeft = false;

    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();

    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        infoWidget->setLabelText(myLan.msg_usb_init_error);
        infoWidget->showSecs();
        return false;
    }

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, 0, 0, 0, 0, 0, 1, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, struGsh.nUnit, 0 ,0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    MySerial.com1Write(2, 2, 0, 0x3f,  0, 0, 0, 0, 0, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 1, (IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1);

    bzero(tmpBuf, bufSize);
    usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, (char *)tmpBuf, bufSize, 20*1000);
    pImageHead = (char *)tmpBuf;
    qDebug("usbReturn = %d", usbReturn);
    if(usbReturn == bufSize){
        for(int i=0; i<IMAGE_HEIGHT; i++){
            /* 判断包头数据是否正确 */
            if((pImageHead=getHead((char *)pImageHead, 6160*3, (char *)imageHead)) != NULL){
                /* 在第一帧判断物料在整幅图片的左侧还是右侧 */
                if(i == 0){
                    for(int j=0; j<1024; j++)
                    {
                        uchar r = *(pImageHead+j*3);
                        uchar g = *(pImageHead+j*3+1);
                        uchar b = *(pImageHead+j*3+2);

                        if(pixelIsWanted(r, g, b)){
                            bIsLeft = true;
                            break;
                        }
                    }
                }
                if(bIsLeft)
                    memcpy(tmpColorBuf+IMAGE_WIDTH*3*(IMAGE_HEIGHT-1-i), pImageHead, IMAGE_WIDTH*3);
                else
                    memcpy(tmpColorBuf+IMAGE_WIDTH*3*(IMAGE_HEIGHT-1-i), pImageHead+IMAGE_WIDTH*3, IMAGE_WIDTH*3);
            }
            else{
                qDebug("Wrong frame : %d", i);
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->showSecs();
                return false;
            }
        }

        MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    }
    else {
        infoWidget->hide();
        return false;
    }

    //将图像数据保存成位图文件
    QString colorImageName = QString("%1.png").arg(imgListNum);
    qDebug()<<imgListNum;
    saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar *)tmpColorBuf);

    infoWidget->hide();

    return true;
#endif
}

/* 根据需要使能智能算法 */
void AIAnalysis::enableAIArithmetic()
{
    int goodSum = 0;    //好物料的数目
    int badSum = 0;     //坏物料的数目
    int ariSum = 0;     //开启智能算法的数量

    // 根据选择的物料使能全局参数中的智能算法
    for(int i=0; i<MAT_MAX; i++){
        if(struCnfp.struIntelMat[i].use){
            if(struCnfp.struIntelMat[i].good_bad == 1)
                goodSum++;
            else
                badSum++;
        }
    }

	if((goodSum == 0) && (badSum == 0)) {
		struCnfp.nMatIntelSeq = 0;
	}
	if((goodSum == 1) && (badSum == 1)) {
		struCnfp.nMatIntelSeq = 1;
	}
	if((goodSum == 1) &&( badSum == 2)) {
		struCnfp.nMatIntelSeq = 2;
	}
	if((goodSum == 1) && (badSum == 3)) {
		struCnfp.nMatIntelSeq = 3;
	}
	if((goodSum == 2) && (badSum == 1)) {
		struCnfp.nMatIntelSeq = 4;
	}
	if((goodSum == 2) && (badSum == 2)) {
		struCnfp.nMatIntelSeq = 5;
	}
	if((goodSum == 3) && (badSum == 1)) {
		struCnfp.nMatIntelSeq = 6;
	}

	myFlow.resetIntelSeq();

    switch(goodSum+badSum){
    case 2:
        ariSum = 1;
        break;
    case 3:
        ariSum = 2;
        break;
    case 4:
        if(goodSum==1 || goodSum==3)
            ariSum = 3;
        if(goodSum == 2)
            ariSum = 4;
        break;
    default:
        break;
    }

    switch(ariSum){
    case 0:
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_B] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_C] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_D] = 0;
        break;
    case 1:
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_B] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_C] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_D] = 0;
        break;
    case 2:
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_B] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_C] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_D] = 0;
        break;
    case 3:
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_B] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_C] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_D] = 0;
        break;
    case 4:
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_B] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_C] = 1;
        struCnfp.nArithmeticEnable[ARITH_INTEL_D] = 1;
        break;
    default:
        break;
    }
    /* only for tea */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < 4; j++) {
            struCnfp.nArithmeticEnableLevel[i][ARITH_INTEL_A+j] = struCnfp.nArithmeticEnable[ARITH_INTEL_A+j];
        }
    }
    emit updateArithmeticSig();
}

/* 设置全局参数中的智能部分参数 */
void AIAnalysis::setGlobalMatterParams()
{
    for(int i=0; i<MAT_MAX; i++){
        struCnfp.struIntelMat[i].use = (isEnable[i]==true) ? 1:0;
        if(struCnfp.struIntelMat[i].use){
            const char *name = matterName[i].toLatin1();
            strcpy(struCnfp.struIntelMat[i].name, name);
            struCnfp.struIntelMat[i].good_bad = (isGood[i]==true) ? 1:0;
        }
        else{
            memcpy(struCnfp.struIntelMat[i].name, "Name", sizeof("Name"));
            struCnfp.struIntelMat[i].good_bad = 2;
        }
    }
}

/* 将图片数据保存为BMP文件 */
void AIAnalysis::saveBMPFile(QString name, int width, int height, uchar *data)
{
    QImage img(data, width, height, QImage::Format_RGB888);
    img.save(QString("/tmp/%1").arg(name),"PNG",50);
}

/* 将选择的像素点数据保存到文件中 */
bool AIAnalysis::saveDataToNormalFile(bool bIsAddCur)
{
    QFile file;
    QTextStream io(&file);
    int num = 0;
    int fileIndex[3];   // 记录需写入文件的编号
    int fileSum = 0;    // 记录需写入文件的数量

    if(bIsAddCur){
        for(int i=0; i<MAT_MAX; i++){
            if(isEnable[i] && isGood[i]){
                for(int j=0; j<MAT_MAX; j++){
                    if(isEnable[j] && !isGood[j]){
                        if(matterIndex==i || matterIndex==j)
                            fileIndex[fileSum++] = num;
                        num++;
                    } // end if
                } //end for
            } // end if
        } // end for

        for(int i=0; i<fileSum; i++){
            file.setFileName(QString("/tmp/train%1").arg(fileIndex[i]));
            if(!file.open(QIODevice::Append | QIODevice::WriteOnly)){
                qDebug("File open error");
                return false;
            }

            if(isGood[matterIndex]){
                for(int m=0; m<param[matterIndex].iDisplaySum; m++){
                    QString str = QString("1\t1:%1\t2:%2\t3:%3\t\n").arg(param[matterIndex].selBuf[3*m])
                            .arg(param[matterIndex].selBuf[3*m+1])
                            .arg(param[matterIndex].selBuf[3*m+2]);
                    io<<str;
                }
            }
            else{
                for(int m=0; m<param[matterIndex].iDisplaySum; m++){
                    QString str = QString("-1\t1:%1\t2:%2\t3:%3\t\n").arg(param[matterIndex].selBuf[3*m])
                            .arg(param[matterIndex].selBuf[3*m+1])
                            .arg(param[matterIndex].selBuf[3*m+2]);
                    io<<str;
                }
            }
            file.close();
        }
    }
    else{
        for(int i=0; i<MAT_MAX; i++){
            if(isEnable[i] && isGood[i]){
                for(int j=0; j<MAT_MAX; j++){
                    if(isEnable[j] && !isGood[j]){
                        file.setFileName(QString("/tmp/train%1").arg(num++));
                        if(!file.open(QIODevice::Append | QIODevice::WriteOnly)){
                            qDebug("File open error");
                            return false;
                        }
                        for(int m=0; m<param[i].iDisplaySum; m++){
                            QString str = QString("1\t1:%1\t2:%2\t3:%3\t\n").arg(param[i].selBuf[3*m])
                                    .arg(param[i].selBuf[3*m+1])
                                    .arg(param[i].selBuf[3*m+2]);
                            io<<str;
                        }
                        for(int m=0; m<param[j].iDisplaySum; m++){
                            QString str = QString("-1\t1:%1\t2:%2\t3:%3\t\n").arg(param[j].selBuf[3*m])
                                    .arg(param[j].selBuf[3*m+1])
                                    .arg(param[j].selBuf[3*m+2]);
                            io<<str;
                        }
                        file.close();
                    } // end if
                } //end for
            } // end if
        } // end for
    }

    return true;
}

/* 多分类模式下将选择的像素点数据保存到文件中 */
bool AIAnalysis::saveDataToMultiFile(bool bIsAddCur)
{
    QFile file("/tmp/train.learn");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append)){
        qDebug("Save data : File open error");
        return false;
    }
    QTextStream io(&file);
    int red, green, blue;

    if(bIsAddCur){  //将当前图片中的数据增加到训练样本中
        if(isGood[matterIndex]){
            for(int i=0; i<param[matterIndex].iDisplaySum; i++){
                red = param[matterIndex].selBuf[3*i];
                green = param[matterIndex].selBuf[3*i+1];
                blue = param[matterIndex].selBuf[3*i+2];
				/*
                QString str = QString("1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t7:%7\t8:%8\t9:%9\t\n")
                        .arg(red).arg(green).arg(blue)
                        .arg(red*green).arg(red*blue).arg(green*blue)
                        .arg(red*red).arg(green*green).arg(blue*blue);
				*/
                QString str = QString("1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t\n")
                        .arg(red).arg(green).arg(blue)
                        .arg(red*green).arg(red*blue).arg(green*blue);
                io<<str;
            }
        }
        else{
            for(int i=0; i<param[matterIndex].iDisplaySum; i++){
                red = param[matterIndex].selBuf[3*i];
                green = param[matterIndex].selBuf[3*i+1];
                blue = param[matterIndex].selBuf[3*i+2];
				/*
                QString str = QString("-1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t7:%7\t8:%8\t9:%9\t\n")
                        .arg(red).arg(green).arg(blue)
                        .arg(red*green).arg(red*blue).arg(green*blue)
                        .arg(red*red).arg(green*green).arg(blue*blue);
				*/
                QString str = QString("-1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t\n")
                        .arg(red).arg(green).arg(blue)
                        .arg(red*green).arg(red*blue).arg(green*blue);
                io<<str;
            }
        }
    }
    else{           //将好坏样本图片中的数据都增加到训练样本中
        int goodIndex;
        int badIndex;
        if(isGood[0]){
            goodIndex = 0;
            badIndex = 1;
        }
        else{
            goodIndex = 1;
            badIndex = 0;
        }

        for(int i=0; i<param[goodIndex].iDisplaySum; i++){
            red = param[goodIndex].selBuf[3*i];
            green = param[goodIndex].selBuf[3*i+1];
            blue = param[goodIndex].selBuf[3*i+2];
			/*
            QString str = QString("1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t7:%7\t8:%8\t9:%9\t\n")
                    .arg(red).arg(green).arg(blue)
                    .arg(red*green).arg(red*blue).arg(green*blue)
                    .arg(red*red).arg(green*green).arg(blue*blue);
			*/
            QString str = QString("1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t\n")
                    .arg(red).arg(green).arg(blue)
                    .arg(red*green).arg(red*blue).arg(green*blue);
            io<<str;
        }
        for(int i=0; i<param[badIndex].iDisplaySum; i++){
            red = param[badIndex].selBuf[3*i];
            green = param[badIndex].selBuf[3*i+1];
            blue = param[badIndex].selBuf[3*i+2];
			/*
            QString str = QString("-1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t7:%7\t8:%8\t9:%9\t\n")
                    .arg(red).arg(green).arg(blue)
                    .arg(red*green).arg(red*blue).arg(green*blue)
                    .arg(red*red).arg(green*green).arg(blue*blue);
			*/
            QString str = QString("-1\t1:%1\t2:%2\t3:%3\t4:%4\t5:%5\t6:%6\t\n")
                    .arg(red).arg(green).arg(blue)
                    .arg(red*green).arg(red*blue).arg(green*blue);
            io<<str;
        }
    }
    file.close();

    return true;
}

/* 普通智能算法计算 */
bool AIAnalysis::normalModeAICal()
{
    QFile file;
    QTextStream io(&file);
    int num = 0;

    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i] && isGood[i]){
            for(int j=0; j<MAT_MAX; j++){
                if(isEnable[j] && !isGood[j])
                    num++;
            }
        }
    }

    for(int i=0; i<num; i++){
        //! 重置训练样本
        if (!resetTrainFile(QString("/tmp/train%1").arg(i))) {
            return false;
        }

        char cmd[64];
//        sprintf(cmd,"./normalization_learn /tmp/train%d",i);
        system(cmd);
        file.setFileName(QString("/tmp/train%1.result").arg(i));
        if(!file.open(QIODevice::ReadOnly)){
            qDebug("result file open error");
            return false;
        }
        io>>myMatterQuo[i].nCons;
        io>>myMatterQuo[i].nQuo1;
        io>>myMatterQuo[i].nQuo2;
        io>>myMatterQuo[i].nQuo3;
        file.close();

        file.setFileName(QString("/tmp/train%1.maxmin").arg(i));
        if(!file.open(QIODevice::ReadOnly)){
            qDebug("maxmin file open error");
            return false;
        }
        for(int j=0; j<3; j++){
            io>>myMatterQuo[i].nMin[j];
            io>>myMatterQuo[i].nMax[j];
        }
        file.close();

        double W = myMatterQuo[i].nCons;
        double X = myMatterQuo[i].nQuo1;
        double Y = myMatterQuo[i].nQuo2;
        double Z = myMatterQuo[i].nQuo3;
        // 计算W的值：W=w-x-y-z
        W = W-X-Y-Z;

        if(myMatterQuo[i].nMax[0] == myMatterQuo[i].nMin[0]
                || myMatterQuo[i].nMax[1] == myMatterQuo[i].nMin[1]
                || myMatterQuo[i].nMax[2] == myMatterQuo[i].nMin[2]){
            infoWidget->setLabelText(myLan.msg_compute_error);
            infoWidget->showSecs();
            return false;
        }

        // W=W-2*min_r*x/(max_r-min_r)
        W = W-myMatterQuo[i].nMin[0]*2*X/(myMatterQuo[i].nMax[0]-myMatterQuo[i].nMin[0]);
        // W=W-2*min_g*y/(max_g-min_g)
        W = W-myMatterQuo[i].nMin[1]*2*Y/(myMatterQuo[i].nMax[1]-myMatterQuo[i].nMin[1]);
        // W=(W-2*min_b*z/(max_r-min_b))*1000
        W = (W-myMatterQuo[i].nMin[2]*2*Z/(myMatterQuo[i].nMax[2]-myMatterQuo[i].nMin[2]))*1000;
        // 计算X的值：X=2*x/(max_r-min_r)
        X = 2*X/(myMatterQuo[i].nMax[0]-myMatterQuo[i].nMin[0])*1000;
        // 计算Y的值：Y=2*Y/(max_g-min_g)
        Y = 2*Y/(myMatterQuo[i].nMax[1]-myMatterQuo[i].nMin[1])*1000;
        // 计算Y的值：Z=2*Z/(max_b-min_b)
        Z = 2*Z/(myMatterQuo[i].nMax[2]-myMatterQuo[i].nMin[2])*1000;

        int groupNum, addr;
        switch(struCnfp.nMatAssembleMode) {
        case PARAMS_ALL_SEPARATE:     //全部单独设置
            groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nCons[0] = (W>=0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nCons[1] = abs(W);
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nQuo1[0] = (X>=0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nQuo1[1] = abs(X);
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nQuo2[0] = (Y>=0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nQuo2[1] = abs(Y);
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nQuo3[0] = (Z>=0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[i].nQuo3[1] = abs(Z);
            break;
        case PARAMS_FRONT_REAR_SAME: //前后视相同设置
            groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
            for (int j = groupNum; j < groupNum+2; j++) {
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nCons[0] = (W>=0)?0:1;
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nCons[1] = abs(W);
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo1[0] = (X>=0)?0:1;
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo1[1] = abs(X);
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo2[0] = (Y>=0)?0:1;
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo2[1] = abs(Y);
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo3[0] = (Z>=0)?0:1;
                struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo3[1] = abs(Z);
            }
            break;
        case PARAMS_FIRST_SECOND_SAME: //一二三次相同设置
            groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
            addr = groupNum%2;
            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].nIdentifyGroupTotal; j++){
                if (j%2 == addr) {
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nCons[0] = (W>=0)?0:1;
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nCons[1] = abs(W);
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo1[0] = (X>=0)?0:1;
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo1[1] = abs(X);
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo2[0] = (Y>=0)?0:1;
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo2[1] = abs(Y);
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo3[0] = (Z>=0)?0:1;
                    struCnfp.struGroupIdentify[struGsh.nLevel][j].struIntel[i].nQuo3[1] = abs(Z);
                }
            }
            break;
        case PARAMS_ALL_SAME: //全部相同设置
            for(int j=0; j<struCnfg.nLevelTotal; j++){
                for(int k=0; k<struCnfg.struLevelInfo[j].nIdentifyGroupTotal; k++){
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nCons[0] = (W>=0)?0:1;
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nCons[1] = abs(W);
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nQuo1[0] = (X>=0)?0:1;
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nQuo1[1] = abs(X);
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nQuo2[0] = (Y>=0)?0:1;
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nQuo2[1] = abs(Y);
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nQuo3[0] = (Z>=0)?0:1;
                    struCnfp.struGroupIdentify[j][k].struIntel[i].nQuo3[1] = abs(Z);
                }
            }
            break;
        default:
            break;
        }

        myMatterQuo[i].nCons = W;
        myMatterQuo[i].nQuo1 = X;
        myMatterQuo[i].nQuo2 = Y;
        myMatterQuo[i].nQuo3 = Z;

        qDebug("W = %f",W);
        qDebug("X = %f",X);
        qDebug("Y = %f",Y);
        qDebug("Z = %f",Z);
    }

    //智能计算后删除训练样本文件
    system("rm /tmp/train*");

    return true;
}

/* 多分类智能算法计算 */
bool AIAnalysis::multiModeAICal()
{
    int groupNum = -1;
    int addr = -1;

    //! 确保训练文本的第一个样本为正样本
    if (!resetTrainFile("/tmp/train.learn")) {
        return false;
    }

    //智能计算并读取计算结果
    char cmd[64];
//    sprintf(cmd, "./normalization_learn /tmp/train.learn");
    system(cmd);

    QFile file;
    QTextStream io(&file);
    file.setFileName("/tmp/train.learn.result");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug("result file open error!");
        return false;
    }
    memset(&myMultiQuo, 0, sizeof(myMultiQuo));
    io>>myMultiQuo.nCons;     //cons
    for(int i=0; i<6; i++){
        io>>myMultiQuo.nQuo[i];
    }
    file.close();

    file.setFileName("/tmp/train.learn.maxmin");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug("maxmin file open error");
        return false;
    }
    for(int i=0; i<6; i++){
        io>>myMultiQuo.nMin[i];
        io>>myMultiQuo.nMax[i];

        //若某一系数最大值与最小值相等，则提示用户重新选取像素点
        if(myMultiQuo.nMax[i] == myMultiQuo.nMin[i]){
            infoWidget->setLabelText(myLan.msg_compute_error);
            infoWidget->showSecs();
            return false;
        }
    }
    file.close();

    for (int i=0; i<6; i++) {
	myMultiQuo.nCons -= myMultiQuo.nQuo[i];
    }

    for(int i=0; i<6; i++){
        myMultiQuo.nCons = myMultiQuo.nCons
                -2*myMultiQuo.nMin[i]*myMultiQuo.nQuo[i]/(myMultiQuo.nMax[i]-myMultiQuo.nMin[i]);
        myMultiQuo.nQuo[i] = 2*myMultiQuo.nQuo[i]/(myMultiQuo.nMax[i]-myMultiQuo.nMin[i])*10000000;
        qDebug("nQuo[%d] = %f\n", i, myMultiQuo.nQuo[i]);
    }
    myMultiQuo.nCons = myMultiQuo.nCons*10000000;
    qDebug("nCons = %f\n", myMultiQuo.nCons);

    //将计算出的结果保存到全局参数中
    switch(struCnfp.nMatAssembleMode) {
    case PARAMS_ALL_SEPARATE:     //全部单独设置
        groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);

        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
        struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
        break;
    case PARAMS_FRONT_REAR_SAME: //前后视相同设置
        groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
        int nStart;
        if (groupNum%2){
            nStart = groupNum-1;
        }
        else {
            nStart = groupNum;
        }
        for (int i = nStart; i < nStart+2; i++) {
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
            struCnfp.struGroupIdentify[struGsh.nLevel][i].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
        }
        break;
    case PARAMS_FIRST_SECOND_SAME: //一二三次相同设置
        groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
        addr = groupNum%2;
        for(int i=0; i<struCnfg.nLevelTotal; i++){
            for(int j=0; j<MAX_GROUP_IDTNTIFY; j++){
                if (j%2 == addr) {
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
                }
            }
        }
        break;
    case PARAMS_ALL_SAME: //全部相同设置
        for(int i=0; i<struCnfg.nLevelTotal; i++){
            for(int j=0; j<MAX_GROUP_IDTNTIFY; j++){
                struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[0] = (myMultiQuo.nCons >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nConsDfl[1] = abs(myMultiQuo.nCons);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[0] = (myMultiQuo.nQuo[0] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoR[1] = abs(myMultiQuo.nQuo[0]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[0] = (myMultiQuo.nQuo[1] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoG[1] = abs(myMultiQuo.nQuo[1]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[0] = (myMultiQuo.nQuo[2] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoB[1] = abs(myMultiQuo.nQuo[2]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[0] = (myMultiQuo.nQuo[3] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRG[1] = abs(myMultiQuo.nQuo[3]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[0] = (myMultiQuo.nQuo[4] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRB[1] = abs(myMultiQuo.nQuo[4]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[0] = (myMultiQuo.nQuo[5] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGB[1] = abs(myMultiQuo.nQuo[5]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[0] = (myMultiQuo.nQuo[6] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoRR[1] = abs(myMultiQuo.nQuo[6]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[0] = (myMultiQuo.nQuo[7] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoGG[1] = abs(myMultiQuo.nQuo[7]);
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[0] = (myMultiQuo.nQuo[8] >= 0)?0:1;
                struCnfp.struGroupIdentify[i][j].struIntel[0].nQuoBB[1] = abs(myMultiQuo.nQuo[8]);
            }
        }
        break;
    default:
        break;
    }

    //智能计算后删除训练样本文件
    system("rm /tmp/train*");

    return true;
}

int AIAnalysis::getUnitGroup(int level, int unit)
{
    int group = -1;

    for(int i=0; i<MAX_GROUP_IDTNTIFY; i++){
        for(int j=0; j<struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitCount; j++){
            if(unit == struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitId[j]) {
                group = i;
            }
        }
    }

    return group;
}

/*!
 * \brief 重置训练样本
 * \param sFileName 文件名称
 * \return 重置结果
 */
bool AIAnalysis::resetTrainFile(QString sFileName)
{
    //! 确保第一个样本点为正样本点
    QFile file(sFileName);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug("train file open error!");
        return false;
    }
    QTextStream io(&file);
    char flag;
    io>>flag;
    io.seek(0);
    if (flag == '-') {
        QFile bakFile(QString("%1.bak").arg(sFileName));
        if(!bakFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            qDebug("bak file open error");
            return false;
        }
        QTextStream bakIo(&bakFile);
        QString lineStr;
        int lineNum = 1;
        while(!io.atEnd()){
            lineStr = io.readLine();
            if(lineStr.at(0) == '1'){
                bakIo<<lineStr;
                bakIo<<'\n';
                break;
            }
            lineNum++;
        }
        io.seek(0);
        int num = 1;
        while(!io.atEnd()){
            if(num == lineNum){
                num++;
                lineStr = io.readLine();
                continue;
            }
            lineStr = io.readLine();
            bakIo<<lineStr;
            bakIo<<'\n';
            num++;
        }
        file.close();
        file.remove();
        bakFile.rename(sFileName);
    } else {
        file.close();
    }

    return true;
}

/* 多分类模式下某像素点是否为坏物料点 */
bool AIAnalysis::multiPixelIsBad(uchar r, uchar g, uchar b)
{
    uint rg = r*g;
    uint rb = r*b;
    uint gb = g*b;
    uint rr = r*r;
    uint gg = g*g;
    uint bb = b*b;

    int nCons, nQuo0, nQuo1, nQuo2, nQuo3;
    int nQuo4, nQuo5, nQuo6, nQuo7, nQuo8;
    int groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
    int nSens = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nSens;

    // 常数项
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nModeDfl == 0){    // 正选
        if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[0] == 0){
            if(nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1-(nSens-100)/100.0);
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1+(100-nSens)/100.0);
        }
        else{
            if(nSens > 100)
                nCons = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1+(nSens-100)/100.0);
            else
                nCons = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1-(100-nSens)/100.0);
        }
    }
    else{       // 反选
        if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[0] == 0){
            if(nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1+(nSens-100)/100.0);
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1-(100-nSens)/100.0);
        }
        else{
            if(nSens > 100)
                nCons = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1-(nSens-100)/100.0);
            else
                nCons = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nConsDfl[1]*(1+(100-nSens)/100.0);
        }
    }

    // 系数1
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[0] == 0)
        nQuo0 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[1];
    else
        nQuo0 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoR[1];

    // 系数2
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[0] == 0)
        nQuo1 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[1];
    else
        nQuo1 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoG[1];

    // 系数3
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[0] == 0)
        nQuo2 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[1];
    else
        nQuo2 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoB[1];

    // 系数4
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[0] == 0)
        nQuo3 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[1];
    else
        nQuo3 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRG[1];

    // 系数5
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[0] == 0)
        nQuo4 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[1];
    else
        nQuo4 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRB[1];

    // 系数6
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[0] == 0)
        nQuo5 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[1];
    else
        nQuo5 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGB[1];

    // 系数7
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[0] == 0)
        nQuo6 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[1];
    else
        nQuo6 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoRR[1];

    // 系数8
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[0] == 0)
        nQuo7 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[1];
    else
        nQuo7 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoGG[1];

    // 系数9
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[0] == 0)
        nQuo8 = struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[1];
    else
        nQuo8 = -struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nQuoBB[1];

    int value = nCons+nQuo0*r+nQuo1*g+nQuo2*b+nQuo3*rg+nQuo4*rb+nQuo5*gb+nQuo6*rr+nQuo7*gg+nQuo8*bb; 
    if(struCnfp.struGroupIdentify[struGsh.nLevel][groupNum].struIntel[0].nModeDfl == 0){     // 正选
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

/* 响应智能界面的全部重选按钮 */
void AIAnalysis::onReWholeBtnClicked()
{
    if(struCnfp.nIntelMode == 1){
        myMessageBox *mesBox = new myMessageBox(MSG_QUES, "Lose the pixels you select before,Reselect whole?");
        int ret = mesBox->exec();
        if(ret == QDialog::Accepted){
            for(int i=0; i<2; i++){
                param[1].iSelTimes = 0;
                param[1].iDisplaySum = 0;
                param[i].iPixelSum = 0;
            }

            char cmd[64];
            sprintf(cmd, "rm /tmp/train.learn");
            system(cmd);
        }
        else
            return;
    }

    clearSingleImageStatus(matterIndex);
    reLastBtn->setEnabled(false);

    memcpy(changeBuf, oriBuf, IMAGE_WIDTH*IMAGE_HEIGHT*3);
    updateImage();
    showMesLabel();
}

/* 响应智能界面的重选上次按钮 */
void AIAnalysis::onReLastBtnClicked()
{
    param[matterIndex].iSelTimes--;
    if(param[matterIndex].iSelTimes < 1)
        reLastBtn->setEnabled(false);

    int tmpSum = param[matterIndex].iPixelSum;
    int index;
    param[matterIndex].iPixelSum -= param[matterIndex].iPixelNum[param[matterIndex].iSelTimes];
    param[matterIndex].iDisplaySum -= param[matterIndex].iDisplayNum[param[matterIndex].iSelTimes];
    for(int i=param[matterIndex].iPixelSum; i<tmpSum; i++){
        int x = param[matterIndex].selectedPoint[i].x();
        int y = param[matterIndex].selectedPoint[i].y();
        index = (y*IMAGE_WIDTH+x)*3;
        memcpy(changeBuf+index, oriBuf+index, 3);
    }

    updateImage();
    showMesLabel();
}

/* 响应智能界面的优化计算按钮 */
void AIAnalysis::onOptCalBtnClicked()
{
    param[matterIndex].bIsOptCal = !param[matterIndex].bIsOptCal;
    if(param[matterIndex].bIsOptCal && param[matterIndex].iDisplaySum>0){
        infoWidget->setLabelText(myLan.msg_optimizing);
        infoWidget->delayShow();
    }

    refreshDisplay();
    infoWidget->hide();
}

/* 响应智能界面的优化取样按钮 */
void AIAnalysis::onOptSamBtnClicked()
{
    param[matterIndex].bIsOptSam = !param[matterIndex].bIsOptSam;
    if(param[matterIndex].bIsOptCal && param[matterIndex].iDisplaySum>0){
        infoWidget->setLabelText(myLan.msg_optimizing);
        infoWidget->delayShow();
    }

    refreshDisplay();
    infoWidget->hide();
}

/* 响应智能界面的物料设置按钮 */
void AIAnalysis::onSetMatterBtnClicked()
{
    updateMatterParams();
    initMatterWidgetStatus();
    stackedWidget->setCurrentIndex(AI_SET_MATTER_PAGE);
}

/* 响应智能界面的上一张按钮 */
void AIAnalysis::onLastBtnClicked()
{
    uint tmp = 0;
    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i]){
            imgsIndex[tmp++] = i;
        }
    }

    nextBtn->setEnabled(true);
    if(arrayIndex > 0){
        arrayIndex--;
        if(arrayIndex == 0)
            lastBtn->setEnabled(false);
        matterIndex = imgsIndex[arrayIndex];
        if(!getImageData(QString("%1.png").arg(param[matterIndex].imgIndex))){
            imageOpenError();
            return;
        }
        if(m_bIsSimulate)
            displaySimulate();
        else
            refreshDisplay();
    }
}

/* 响应智能界面的下一张按钮 */
void AIAnalysis::onNextBtnClicked()
{
    uint tmp = 0;
    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i]){
            imgsIndex[tmp++] = i;
        }
    }

    lastBtn->setEnabled(true);
    if(arrayIndex < tmp-1){
        arrayIndex++;
        if(arrayIndex == tmp-1)
            nextBtn->setEnabled(false);
        matterIndex = imgsIndex[arrayIndex];
        if(!getImageData(QString("%1.png").arg(param[matterIndex].imgIndex))){
            imageOpenError();
            return;
        }
        if(m_bIsSimulate)
            displaySimulate();
        else
            refreshDisplay();
    }
}

/* 响应智能界面的返回按钮 */
void AIAnalysis::onAIBackBtnClicked()
{
    /*
    for(int i=0; i<struCnfg.nLevelTotal; i++) {
        MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, 0, 3);
    }
    */
    myFlow.initInterfaceTransMode(0);
    emit goToHomePage();
}

/* 响应智能界面的智能分析设置按钮 */
void AIAnalysis::onAISetBtnClicked()
{
    //进入智能设置界面前设置相关参数并同步到显示界面中
    setAIParams();
    colorGroup->setCurrentIndex(m_nSamColor);
    colorSpaceGroup->setCurrentIndex(m_nColorSpace);
    samModeGroup->setCurrentIndex(m_nSamMode);
    if(m_nNormalMode)
        normalModeBtn->setIcon(myIcon.Action_Apply);
    else
        normalModeBtn->setIcon(QIcon());
    if(m_nInfMode)
        infModeBtn->setIcon(myIcon.Action_Apply);
    else
        infModeBtn->setIcon(QIcon());
    if(m_nMultiMode)
        multiModeBtn->setIcon(myIcon.Action_Apply);
    else
        multiModeBtn->setIcon(QIcon());
    sensValueBar->setValue(m_nSamSens);

    stackedWidget->setCurrentIndex(AI_SET_PAGE);
}

/* 刷新智能分析界面的显示 */
void AIAnalysis::refreshDisplaySlt()
{
    if(struGsh.nUnit%2 == 0)
        layerBtn->setText(myLan.front);
    else
        layerBtn->setText(myLan.rear);

    boardNum->display(struGsh.nUnit/2+1);

    if(m_nProfile == struCnfg.nProfile)
        return;
    m_nProfile = struCnfg.nProfile;
    m_bIsSimulate = false;
    bRespondMouse = false;

    initMatterParams();
    clearImageStatus();
    bool hasEnable = false;
    for(int i=0; i<MAT_MAX; i++){
        if(isEnable[i]){
            hasEnable = true;
            matterIndex = i;
            arrayIndex = 0;
            nextBtn->setEnabled(true);
            lastBtn->setEnabled(false);
            imageOpenError();
            break;
        }
    }
    if(!hasEnable){
        matterIndex = NO_MATTER;
        imageLabel->setText(myLan.ai_no_image);
        mesLabel->setText(myLan.msg_set_material);
        refreshBtnStatus();
    }
}

/* 响应当前页面索引号发生变化的情况 */
void AIAnalysis::onStackedWidgetIndexChangedSlt(int index)
{
    switch(index){
    case AI_MAIN_PAGE:
        titleLabel->setText(QString("%1").arg(myLan.scheme_parameter));
        break;
    case AI_SET_PAGE:
        titleLabel->setText(QString("%1 > %2").arg(myLan.scheme_parameter)
                            .arg(myLan.ai_analysis_set));
        break;
    case AI_SET_MATTER_PAGE:
        titleLabel->setText(QString("%1 > %2").arg(myLan.scheme_parameter)
                            .arg(myLan.ai_analysis_material));
        break;
    default:
        break;
    }
}
