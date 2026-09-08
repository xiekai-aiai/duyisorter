/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        autoanalysiswidget.cpp
 * \brief       彩色机型自动分析(碎米率仿真)源文件
 * \date        2017.02.14
 */
#include "autoanalysiswidget.h"
//extern int nSmallMatArea;

AutoAnalysisWidget::AutoAnalysisWidget(QWidget *parent) :
    QWidget(parent)
{
    initLocalParams();

    createPage();

    connectSigAndSlt();
}

AutoAnalysisWidget::~AutoAnalysisWidget()
{
    if(m_pImgData !=NULL){
        delete m_pImgData;
        m_pImgData=NULL;
    }
    if(m_pDispData !=NULL){
        delete m_pDispData;
        m_pDispData=NULL;
    }
    if(m_labelData !=NULL){
        delete m_labelData;
        m_labelData=NULL;
    }
}

/* 初始化局部参数 */
void AutoAnalysisWidget::initLocalParams()
{
    m_nNum = 0;
    m_nPixelNum = 0;
    m_nTotal = 0;
    m_nSmall = 0;
    m_bIsSim = false;
    m_bIsLeft = true;
    m_nImgWid = IMG_WID;
    m_nImgHei = IMG_HEI;
    m_img = QImage();   // 清空图片

    m_pImgData = new uchar[m_nImgWid*m_nImgHei*3];
    m_pDispData = new uchar[m_nImgWid*m_nImgHei];
    m_labelData = new uint[m_nImgWid*m_nImgHei];
}

/* 创建主页面 */
void AutoAnalysisWidget::createPage()
{
    m_pImgLabel = new myLabel(myLan.ai_no_image);
    m_pImgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pImgLabel->setFixedSize(1000, 500);

    m_valueBar = new myValueBar(0, 999, 1, 50, nSmallMatArea);

    m_pInfoLbl = new myLabel("");
    m_pInfoLbl->setFixedHeight(BTN_HEIGHT);
    m_pInfoLbl->setStyleSheet("color:rgb(230,0,45)");

    m_chanNum = new QLCDNumber;
    m_chanNum->display(struGsh.nUnit/2+1);
    m_chanNum->setDigitCount(2);
    m_chanNum->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_plusChanNumBtn = new myPushButton("", myIcon.Action_Plus);
    m_minusChanNumBtn = new myPushButton("", myIcon.Action_Minus);
    m_pCaptureBtn = new myPushButton(myLan.image_capture,myIcon.Sorter_CameraRGB);
    m_pSimulateBtn = new myPushButton(myLan.ai_simulate,myIcon.Sorter_AI);

    m_plusChanNumBtn->setFixedSize(SMALL_BTN_WIDTH,BTN_HEIGHT);
    m_minusChanNumBtn->setFixedSize(SMALL_BTN_WIDTH,BTN_HEIGHT);
    m_pCaptureBtn->setFixedSize(BTN_WIDTH+50,BTN_HEIGHT);
    m_pSimulateBtn->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
    m_backBtn = new myPushButton(myLan.back,myIcon.Action_Back);
    m_backBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    // 碎米像素个数阈值group
    m_pPixelNumGroup = new myGroupBox(myLan.threshold);
    // 通道设置group
    m_pChartGroup = new myGroupBox(" ");
    QGroupBox *midGroupBox = new QGroupBox("");
    midGroupBox->setFixedHeight(100);
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *midGroupLay = new QHBoxLayout(midGroupBox);
    QHBoxLayout *pixelNumHBoxLay = new QHBoxLayout(m_pPixelNumGroup);
    QHBoxLayout *chuteHBoxLay = new QHBoxLayout(m_pChartGroup);
    QHBoxLayout *downLay = new QHBoxLayout();

    myLabel *chuteLabel = new myLabel(myLan.threshold);
    chuteLabel->setFixedSize(QSize(SMALL_BTN_WIDTH+70, BTN_HEIGHT));
    chuteLabel->setAlignment(Qt::AlignCenter);
    midGroupLay->addWidget(chuteLabel, 0,Qt::AlignRight);
    midGroupLay->addWidget(m_valueBar, 0,Qt::AlignLeft);
    midGroupLay->addSpacing(20);
    midGroupLay->addWidget(m_minusChanNumBtn, 0,Qt::AlignRight);
    midGroupLay->addWidget(m_chanNum, 0,Qt::AlignCenter);
    midGroupLay->addWidget(m_plusChanNumBtn, 0,Qt::AlignLeft);
    midGroupLay->addSpacing(20);

    //midGroupLay->addStretch();
    midGroupLay->addWidget(m_pCaptureBtn, 0,Qt::AlignRight);
    midGroupLay->addWidget(m_pSimulateBtn, 0,Qt::AlignCenter);

    m_minusChanNumBtn->setVisible(false);
    m_chanNum->setVisible(false);
    m_plusChanNumBtn->setVisible(false);

    QVBoxLayout *pVideoLayout = new QVBoxLayout;
    pVideoLayout->addWidget(m_pImgLabel);
    pVideoLayout->addSpacing(10);
    pVideoLayout->addStretch();
    pVideoLayout->addWidget(midGroupBox);
    pVideoLayout->setContentsMargins(0, 0, 10, 5);

    downLay->addWidget(m_pInfoLbl);
    downLay->addStretch(1);
    downLay->addWidget(m_backBtn);

    mainLay->addLayout(pVideoLayout);
    mainLay->addLayout(downLay);
    mainLay->addSpacing(10);

    m_config = g_Config::getInstance();
    QSize btnSize = m_config->getBtnSize(SMALL_BTN_SIZE);
    int leftX = 9;
    int leftY = 48+(500-btnSize.height())/2;
    int rightX = 9+1000-btnSize.width();
    int rightY = leftY;
    m_leftBtn = new myPushButton("", myIcon.Arrow_Left, this);
    m_leftBtn->setGeometry(QRect(QPoint(leftX, leftY), btnSize));
    m_rightBtn = new myPushButton("", myIcon.Arrow_Right, this);
    m_rightBtn->setGeometry(QRect(QPoint(rightX, rightY), btnSize));
}

/* 连接信号槽函数 */
void AutoAnalysisWidget::connectSigAndSlt()
{
    connect(m_backBtn,          SIGNAL(pressed()),          this,   SLOT(onBackBtnClickedSlt_test()));
    connect(m_pCaptureBtn,      SIGNAL(pressed()),          this,   SLOT(onCaptureBtnClicked()));
    connect(m_pSimulateBtn,     SIGNAL(pressed()),          this,   SLOT(onSimulateBtnClicked()));
    connect(m_valueBar,         SIGNAL(valueChanged(int)),  this,   SLOT(onMatAreaChangedSlt()));
    connect(m_plusChanNumBtn ,  SIGNAL(pressed()),          this,   SLOT(onPlusChanNumPressedSlt()));
    connect(m_minusChanNumBtn,  SIGNAL(pressed()),          this,   SLOT(onMinusChanNumPressedSlt()));
    connect(m_leftBtn,          SIGNAL(pressed()),          this,   SLOT(onLeftBtnClicked()));
    connect(m_rightBtn,         SIGNAL(pressed()),          this,   SLOT(onRightBtnClicked()));
}

/* 初始化并更新页面显示 */
void AutoAnalysisWidget::updatePage()
{
    initLocalParams();
    refreshPageDisplay();
}
/* 刷新页面显示 */
void AutoAnalysisWidget::refreshPageDisplay()
{
    if(m_img.isNull()) {
        QFont font;
        font.setPixelSize(50);
        m_pImgLabel->setAlignment(Qt::AlignCenter);
        m_pImgLabel->setScaledContents(true);
        m_pImgLabel->setText("NO PICTURE!");
        m_pImgLabel->setFont(font);
        m_pImgLabel->setStyleSheet("color:#036eb8");

        m_leftBtn->hide();
        m_rightBtn->hide();
    }

   //struGsh.nUnit = (struGsh.nUnit%2 == 0)?struGsh.nUnit:(struGsh.nUnit-1); //地址指定为该通道前视相机

    /* 通道数显示 */
    m_chanNum->display(struGsh.nUnit/2+1);

    if(m_bIsSim) {
        m_pSimulateBtn->setRedColor(GREEN);
    } else {
        m_pSimulateBtn->setRedColor(DEF);
    }

    refreshLabelDisplay();
}

/* 刷新产量和碎米率显示 */
void AutoAnalysisWidget::refreshLabelDisplay()
{
    double realThroughPut = 0;
    double dRatio = 0;
    double k = 0.0001;//一个物料点的重量g

    if (m_nPixelNum == 0) {
        realThroughPut = 0;
    } else {
        realThroughPut = (double)((double)(m_nPixelNum*k)/1000.0*(3600*1000.0/50.0));
    }

    if(m_nTotal == 0) {
        m_nSmall = 0;
        dRatio = 0;
    } else {
        dRatio = (double)m_nSmall/(double)m_nTotal*100.0;
    }
    QString infoStr;
    infoStr = QString("\t\t"+myLan.broken_rice_ratio + ": %2%").arg(dRatio,0,'f',2);
    m_pInfoLbl->setText(infoStr);
}

/* 获取一帧数据的数据头*/
char *AutoAnalysisWidget::getHead(const char *mem, int size, const char *str)
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


/* 从相机获取图像 */
bool AutoAnalysisWidget::getImageFromCamera()
{
    m_img = QImage();
    bool retCode = true;
    int usbReturn = 0;
    int bufSize = 6160*IMG_HEI+2240;
    uchar *tmpBuf = NULL;
    char *pImageHead;
    uchar imageHead[5]={0xAA,0xAA,0xAA,0xAA,0};
    int nUnitAddr;
    nUnitAddr = (struGsh.nUnit%2 == 0)?struGsh.nUnit:(struGsh.nUnit-1); //地址指定为该通道前视相机
    tmpBuf = (uchar *)malloc(bufSize);

#ifdef Q_OS_UNIX
    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        retCode = false;
        goto EXIT;
    }
#endif
    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, nUnitAddr, 0, 0,(IMG_HEI-1)/256, (IMG_HEI-1)%256, 1, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, nUnitAddr, 0 ,0, 0x01, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, nUnitAddr, 0, 0, 1, (IMG_HEI-1)/256, (IMG_HEI-1)%256, 1);
    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, nUnitAddr, 0, 0, 0, 0, 0, 1);
    memset(tmpBuf, 0, bufSize);
#ifdef Q_OS_UNIX
    usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, (char *)tmpBuf, bufSize, 20*1000);
#endif
    pImageHead = (char *)tmpBuf;
    qDebug("usbReturn = %d", usbReturn);
    memset(m_pImgData,0,IMG_WID*IMG_HEI*3);
    if(usbReturn == bufSize){
        for(int i=0; i<IMG_HEI; i++){
            /* 判断包头数据是否正确 */
            if((pImageHead=getHead((char *)pImageHead, 6160*3, (char *)imageHead)) != NULL) {
                memcpy(m_pImgData+IMG_WID*3*(IMG_HEI-1-i), pImageHead, IMG_WID*3);
            } else {
                retCode = false;
                goto EXIT;
            }
        }
        MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    } else {
        retCode = false;
        goto EXIT;
    }

    m_img = QImage((uchar *)m_pImgData, IMG_WID, IMG_HEI, QImage::Format_RGB888);

    if(m_bIsSim) {      // 仿真模式
        dealSimulateImage();
    } else {            // 非仿真模式
        displayImage();
    }

    m_leftBtn->show();
    m_rightBtn->show();

EXIT:
    fflush(0);
    if (tmpBuf != NULL) {
        free(tmpBuf);
        tmpBuf = NULL;
    }
    return retCode;
}

/* 处理图像的仿真*/
void AutoAnalysisWidget::dealSimulateImage()
{
    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();

    /* 2.二值化 */
    gray();

    /* 3.膨胀 */
    //dilation();

    /* 4.腐蚀 */
    erosion();

    /* 5.根据设置的物料面积显示图片 */
    findMaterial();

    /* 6.寻找小物料并显示 */
    findSmallAndDisplay();

    infoWidget->hide();
}

/* 原始图片的显示 */
void AutoAnalysisWidget::displayImage()
{
    QImage img1;
    m_img = m_img.scaled(QSize(IMG_WID, IMG_HEI), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if(m_bIsLeft) {
        img1 = m_img.copy(12,0,1000,500); // 图像左侧丢12列
    } else {
        img1 = m_img.copy(1012,0,1000,500); // 图像右侧丢12列
    }
    m_pImgLabel->setPixmap(QPixmap::fromImage(img1));
}

/* 仿真后图片的显示 */
void AutoAnalysisWidget::displaySimImage()
{
    QImage img1;
    m_simImg = m_simImg.scaled(QSize(IMG_WID, IMG_HEI), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if(m_bIsLeft) {
        img1 = m_simImg.copy(12,0,1000,500);
    } else {
        img1 = m_simImg.copy(1012,0,1000,500);
    }
    m_pImgLabel->setPixmap(QPixmap::fromImage(img1));
}

/* 像素阈值设置 */
void AutoAnalysisWidget::onMatAreaChangedSlt()
{
    nSmallMatArea =  m_valueBar->getValue();
    if(m_img.isNull()) {
        return;
    }
    if(m_bIsSim) {
        findSmallAndDisplay();
    }
}

/* 通道数增加按钮槽函数 */
void AutoAnalysisWidget::onPlusChanNumPressedSlt()
{
    int IDTotal;
    if(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal==1) {
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
    } else {
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
    }
    if(struGsh.nUnit < IDTotal-2) {
        struGsh.nUnit += 2;
        m_img = QImage();
    }

    refreshPageDisplay();// 刷新页面显示
}

/* 通道数按钮槽函数 */
void AutoAnalysisWidget::onMinusChanNumPressedSlt()
{
    if(struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
    }

    refreshPageDisplay();// 刷新页面显示
}

/* 响应图片向左切换 */
void AutoAnalysisWidget::onLeftBtnClicked()
{
    m_bIsLeft = true;
    m_leftBtn->setEnabled(false);
    m_rightBtn->setEnabled(true);

    if(m_bIsSim) {
        displaySimImage();
    } else {
        displayImage();
    }
}

/* 响应图片向右切换 */
void AutoAnalysisWidget::onRightBtnClicked()
{
    m_bIsLeft = false;
    m_leftBtn->setEnabled(true);
    m_rightBtn->setEnabled(false);

    if(m_bIsSim) {
        displaySimImage();
    } else {
        displayImage();
    }
}

/* 返回按钮槽函数*/
void AutoAnalysisWidget::onBackBtnClickedSlt_test()
{
    //myFlow.initInterfaceTransMode(0);// 切换接口板模式为串口模式
    emit backToPageSig();
}

/* 响应拍照按钮按下状态*/
void AutoAnalysisWidget::onCaptureBtnClicked()
{
    bool bIsRunning = false;

    myMessageBox msgBox(MSG_QUES, myLan.ai_capture);
    int ret = msgBox.exec();
    if (ret != QDialog::Accepted) {
        return;
    }

    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();

    if(g_ejectTimesThread->isRunning() && struGsh.bStatFeed == 1) {//! 在开下料过程中拍照，要停止吹气次数统计线程（红外设置中玻璃专选使能后，启用红外自动跟踪,会发红外相机灵敏度调节命令）
        bIsRunning = true;					   //! 由于下位机通信机制原因，会导致拍照命令回包和灵敏度调节命令的回包冲突。导致拍照异常。
        g_ejectTimesThread->stopStatRun();
        myFlow.sleep(1);
    }

    if (!getImageFromCamera()){
        infoWidget->setLabelText(myLan.ai_image_capture_error);
        infoWidget->showSecs();
        myFlow.sleep(2);
        infoWidget->hide();
        return;
    }

    //! 恢复吹气频率线程状态
    if (bIsRunning) {
        g_ejectTimesThread->start();
    }

    infoWidget->hide();
}

/* 响应仿真按钮按下状态*/
void AutoAnalysisWidget::onSimulateBtnClicked()
{
    /* 若图片不存在则直接退出 */
    if(m_img.isNull()) {
        infoWidget->setLabelText(myLan.ai_no_image);
        infoWidget->showSecs();
        return;
    }

    m_bIsSim = !m_bIsSim;
    if(m_bIsSim) {
        m_pSimulateBtn->setRedColor(GREEN);
        dealSimulateImage();
    } else {
        m_pSimulateBtn->setRedColor(DEF);
        displayImage();
    }
}

/* 图像二值化 */
bool AutoAnalysisWidget::gray()
{
    int nIndex;
    m_nPixelNum = 0;    //物料点总数

//  从图片取自动背景数据

    int oriUnit = struGsh.nUnit;
    struGsh.nUnit = (struGsh.nUnit%2 == 0)?struGsh.nUnit:(struGsh.nUnit-1); //地址指定为该通道前视相机
    for (int i = 0; i < m_nImgHei; i++) {
        for (int j = 0; j < m_nImgWid; j++) {
            nIndex = i*m_nImgWid+j;
            uchar r = m_pImgData[nIndex*3];
            uchar g = m_pImgData[nIndex*3+1];
            uchar b = m_pImgData[nIndex*3+2];
            //if(myAIShare.pixelIsBackground(r,g,b)) {
            if(myAIShare.pixelIsBackgroundcomm(r,g,b,j)) {
                m_pDispData[nIndex] = 255;
            } else {
                m_nPixelNum++;                      //计算物料点总数
                m_pDispData[nIndex] = 0;
            }
        }
    }
    struGsh.nUnit = oriUnit;
    return true;
}

/* 图像的腐蚀运算 */
void AutoAnalysisWidget::erosion()
{
    int flag;
    uchar *pTmpData = new uchar[IMG_WID*IMG_HEI];
    memcpy(pTmpData, m_pDispData, IMG_WID*IMG_HEI);

    for (int i = 1; i < IMG_HEI-1; i++) {
        for (int j = 1; j < IMG_WID-1; j++) {
            flag = 1;
            for (int m = i-1; m < i+2; m++) {
                for (int n = j-1; n < j+2; n++) {
                    if (pTmpData[m*IMG_WID+n] == 255) {
                        flag = 0;
                        break;
                    }
                }

                if (flag == 0) {
                    break;
                }
            }
            if (flag == 0) {
                m_pDispData[i*IMG_WID+j] = 255;
            } else {
                m_pDispData[i*IMG_WID+j] = 0;
            }
        }
    }

    delete []pTmpData;
}

/* 图像的膨胀运算 */
void AutoAnalysisWidget::dilation()
{
    int flag;
    uchar *pTmpData = new uchar[IMG_WID*IMG_HEI];
    memcpy(pTmpData, m_pDispData, IMG_WID*IMG_HEI);

    for (int i = 1; i < IMG_HEI-1; i++) {
        for (int j = 1; j < IMG_WID-1; j++) {
            flag = 1;
            for (int m = i-1; m < i+2; m++) {
                for (int n = j-1; n < j+2; n++) {
                    if (pTmpData[m*IMG_WID+n] == 0) {
                        flag = 0;
                        break;
                    }
                }
                if(flag == 0) {
                    break;
                }
            }
            if (flag == 0) {
                m_pDispData[i*IMG_WID+j] = 0;
            } else {
                m_pDispData[i*IMG_WID+j] = 255;
            }
        }
    }

    delete []pTmpData;
}

/* 是否为背景点 */
bool AutoAnalysisWidget::IsBack(int x, int y)
{
    if(x<0 || x>IMG_WID-1 || y<0 || y>IMG_HEI-1) {
        return true;
    }

    if(m_pDispData[y*IMG_WID+x] == 0) {
        return false;
    } else {
        return true;
    }
}

/* DFS深度搜索 */
bool AutoAnalysisWidget::DFS(int x, int y, uint label)
{
    if(IsBack(x, y)) {
        return false;
    }
    m_pDispData[y*IMG_WID+x] = 255;//搜索过的物料点标记为非0（背景）的点
    m_labelData[y*IMG_WID+x] = label;
    for(int i=0; i<4; i++) {
        if(!IsBack(x+pDirection[i][0], y+pDirection[i][1])) {
            m_nNum++;
            DFS(x+pDirection[i][0], y+pDirection[i][1], label);
        }
    }

    return true;
}

/* 寻找物料点 */
void AutoAnalysisWidget::findMaterial()
{
    int nMatSum = 1;
    m_nNum = 1;
    m_vMatParams.clear();

    memset(m_labelData,0,sizeof(uint)*(IMG_WID*IMG_HEI));

    for (int i = 0; i < IMG_HEI; i++) {
        for (int j = 0; j < IMG_WID; j++) {
            if (DFS(j, i, nMatSum)) {
                MaterialParams matParams;
                matParams.nLabel = nMatSum;
                matParams.nNum = m_nNum;
                m_vMatParams.push_back(matParams);
                m_nNum = 1;

                nMatSum++;
            }
        }
    }
}

/* 寻找小物料并显示 */
void AutoAnalysisWidget::findSmallAndDisplay()
{
    QImage img;
    int nMatArea = 60;
    int pSmallLabel[1024];      // 小物料的索引号
    int nSmallNum = 0;          // 小物料的数目

    /* 1.寻找小物料 */
    for (int i = 0; i < m_vMatParams.size(); i++) {
        if (m_vMatParams.at(i).nNum <= nMatArea) {
            pSmallLabel[nSmallNum++] = i;
        }
    }

    /* 2.刷新产量和碎米率标签的显示 */
    m_nTotal = m_vMatParams.size();
    m_nSmall = nSmallNum;
    refreshPageDisplay();

    /* 2.将仿真后的结果显示到界面中 */
    uchar *pImage = new uchar[IMG_WID*IMG_HEI*3];
    bool bIsSmall;
    for (int i = 0; i < IMG_HEI; i++) {
        for (int j = 0; j < IMG_WID; j++) {
            bIsSmall = false;
            if(m_labelData[i*IMG_WID+j] != 0) {//判断当前点是不是物料点
                for (int k = 0; k < nSmallNum; k++) {
                    if(m_labelData[i*IMG_WID+j] == m_vMatParams.at(pSmallLabel[k]).nLabel) {
                        bIsSmall = true;
                        break;
                    }
                }
            }
            if (bIsSmall) {      // 小物料像素点标红
                pImage[(i*IMG_WID+j)*3] = 255;
                pImage[(i*IMG_WID+j)*3+1] = 0;
                pImage[(i*IMG_WID+j)*3+2] = 0;
            } else {            // 非小物料恢复原始信息
                pImage[(i*IMG_WID+j)*3]   = m_pImgData[(i*IMG_WID+j)*3];
                pImage[(i*IMG_WID+j)*3+1] = m_pImgData[(i*IMG_WID+j)*3+1];
                pImage[(i*IMG_WID+j)*3+2] = m_pImgData[(i*IMG_WID+j)*3+2];
            }
        }
    }
    img = QImage(pImage, IMG_WID, IMG_HEI, QImage::Format_RGB888);

    m_simImg = img.copy();
    displaySimImage();

    delete []pImage;
}
