/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycurve.cpp
 * \brief       波形显示界面源文件
 * \date        2015.01.14
 */
#include "mycurve.h"

MyCurveThread::MyCurveThread()
{
    nCurveType = 0;
}

MyCurveThread::~MyCurveThread()
{
}

/*!
 * \brief 获取波形包长及等待时间
 */
void MyCurveThread::getLenAndWait()
{
    switch(struCnfc.nSensorPixelLevel[struGsh.nLevel]) {
    case SENSOR_PIXELS_1024:
        nDataLength = 1024*3+512+16;
        nWaittime = 400000;
        break;
    case SENSOR_PIXELS_2048:
        nDataLength = 2048*3+16;
        nWaittime = 800000;
        break;
    case SENSOR_PIXELS_2592:
        nDataLength = SENSOR_PIXELS_2592*3+16;
        nWaittime = 800000;
        break;
    default:
        nDataLength = 1024*3+512+16;
        nWaittime = 400000;
        break;
    }
}

void MyCurveThread::run(void)
{
    char *p = struGsh.sRgbRow;
    bIsRunning = true;
    int nUnit = 0;

    while(bIsRunning) {
        //! 判断当前是否为辅配
		struGsh.bCurAssist = struGsh.bIsAssist;
        if (nCurveType == 1) {
            nUnit = struGsh.nUnit + struGsh.nDevTypeAddrBias;//I机型红外背景波形或者红外机型设置
        } else {
            nUnit = struGsh.nUnit;
        }

        MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, nUnit, 0, 0, 0, 0, 0, 1);

        //! 获取波形包长及等待时间
        getLenAndWait();

        usleep(nWaittime);
		memset(struGsh.sRgbRow,0,sizeof(struGsh.sRgbRow));
        MySerial.com1Read(p, nDataLength);
        if (p[0] != 0xAA || p[1] != 0xAA || p[2] != 0xAA || p[3] != 0xAA) {
            memset(p, 0, sizeof(struGsh.sRgbRow));
        }

        emit sReceiveData();
        msleep(100);
    }
}

/* 终止线程 */
void MyCurveThread::stop()
{
    bIsRunning = false;
}

void MyCurveThread::onCurveTypeChangedSlt(int type)
{
    nCurveType = type;
}


/*********
 *
 * 绘制波形类
 *
 ***************************************************************/
MyCurve::MyCurve(int width, int height, QWidget *parent) :
        QWidget(parent)
{
    config = g_Config::getInstance();
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);

    bIsWorking = true;
    m_bMultiWave = false;
    m_nCurveType = 0;

    /*   画坐标轴   */
    axisWidget = new MyAxis;
    axisWidget->setFixedSize(width, height);

    /*   通道信息   */
    unitGroup = new myGroupBox(myLan.chute);
//    unitGroup = new myGroupBox("通道");

    unitGroup->setFont(config->getFont());

    m_modeBtn = new myPushButton(myLan.config_major, QIcon(), true, true);
    m_modeBtn->hide();
    m_multiWaveBtn = new myPushButton("", myIcon.Chart_Bar);
    m_multiWaveBtn->hide();
    unitViewBtn = new myPushButton(myLan.front_view, QIcon(), true, true);
    unitViewAdditionBtn = new myPushButton(myLan.lower, QIcon(), true, true);
    unitViewAdditionBtn->hide();
    unitPlusBtn = new myPushButton("",myIcon.Action_Plus);
    unitLcdNum = new QLCDNumber;
    unitLcdNum->setDigitCount(2);
    unitLcdNum->setFixedSize(btnSize);
    unitLcdNum->display(struGsh.nUnit/2+1);
    unitMinusBtn = new myPushButton("",myIcon.Action_Minus);
    rowPauseBtn = new myPushButton("",myIcon.Media_Pause);

    unitGridLayout = new QGridLayout(unitGroup);
    unitGridLayout->addWidget(m_modeBtn, 0, 0, Qt::AlignHCenter);
    unitGridLayout->addWidget(unitViewBtn, 1, 0, Qt::AlignHCenter );
    unitGridLayout->addWidget(unitViewAdditionBtn, 2, 0, Qt::AlignHCenter);
    unitGridLayout->addWidget(unitPlusBtn, 3, 0, Qt::AlignHCenter);
    unitGridLayout->addWidget(unitLcdNum, 4, 0, Qt::AlignHCenter);
    unitGridLayout->addWidget(unitMinusBtn, 5, 0, Qt::AlignHCenter);
    unitGridLayout->addWidget(m_multiWaveBtn, 6, 0, Qt::AlignHCenter);
    unitGridLayout->addWidget(rowPauseBtn, 7, 0, Qt::AlignHCenter);

    m_modeBtn->setFixedSize(SMALL_BTN_WIDTH+10, BTN_HEIGHT);
    m_multiWaveBtn->setFixedSize(btnSize);
    unitViewAdditionBtn->setFixedSize(btnSize);
    unitViewAdditionBtn->setFixedWidth(SMALL_BTN_WIDTH+10);
    unitViewBtn->setFixedSize(btnSize);
    unitViewBtn->setFixedWidth(SMALL_BTN_WIDTH+10);
    unitPlusBtn->setFixedSize(btnSize);
    unitMinusBtn->setFixedSize(btnSize);
    rowPauseBtn->setFixedSize(btnSize);

    axisPageHBLayout = new QHBoxLayout;
    axisPageHBLayout->addWidget(axisWidget);
    axisPageHBLayout->addWidget(unitGroup);
    axisPageHBLayout->setContentsMargins(0, 0, 5, 5);

    setLayout(axisPageHBLayout);

     /*  画波形线程  */
    curveThread = new MyCurveThread;

    connect(curveThread, SIGNAL(sReceiveData()), axisWidget, SLOT(update()));
    connect(curveThread, SIGNAL(sReceiveData()), this, SLOT(onUpdatePixmapSlt()));
    connect(this, SIGNAL(backgroudCurveTypeChanged(int)), curveThread, SLOT(onCurveTypeChangedSlt(int)));
    connect(unitPlusBtn, SIGNAL(pressed()), this, SLOT(onUnitPlusBtnClicked()));
    connect(unitMinusBtn, SIGNAL(pressed()), this, SLOT(onUnitMinusBtnClicked()));
    connect(rowPauseBtn, SIGNAL(pressed()), this, SLOT(onRowPauseBtnClicked()));
    connect(unitViewBtn, SIGNAL(pressed()), this, SLOT(onUnitViewClicked()));
    connect(m_modeBtn, SIGNAL(pressed()), this, SLOT(onModeBtnClicked()));
    connect(m_multiWaveBtn, SIGNAL(pressed()), this, SLOT(onMultiWaveBtnPressed()));
    connect(unitViewAdditionBtn, SIGNAL(pressed()), this, SLOT(onUnitViewAddtionClicked()));
}

MyCurve::~MyCurve()
{
}

/* 新增相机位置切换按钮按下*/
void MyCurve::onUnitViewAddtionClicked()
{
    emit boardNumChanged();
    clearPixmap();
}

/* 上中下层或者前后视切换按钮 */
void MyCurve::onUnitViewClicked()
{
    struGsh.nUnit += (struGsh.nUnit%2 == 0) ? 1 : -1;
    if (struGsh.nUnit%2 == 0) {
        unitViewBtn->setText(myLan.front_view);
    } else {
        unitViewBtn->setText(myLan.rear_view);
    }

    emit boardNumChanged();
    clearPixmap();
}

/* 响应相机模式按钮按下事件 */
void MyCurve::onModeBtnClicked()
{
	struGsh.bIsAssist = !struGsh.bIsAssist;
    if (struGsh.bIsAssist) {
        m_modeBtn->setText(myLan.config_assist);
    } else {
        m_modeBtn->setText(myLan.config_major);
    }
    emit boardNumChanged();
    clearPixmap();
}

/* 板号增加按钮 */
void MyCurve::onUnitPlusBtnClicked()
{
    int idTotal = 0;
    switch(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal) {
    case 1:     // 单视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
        break;
    case 2:     // 双视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
    }
    if(struGsh.nUnit < idTotal-2) {
        struGsh.nUnit += 2;
        emit boardNumChanged();
        clearPixmap();
    }
    unitLcdNum->display(struGsh.nUnit/2+1);
}
/* 多视机型板号增加按钮 */
void MyCurve::onUnitPlusBtnMultiViewClicked()
{
}
/* 板号减少按钮 */
void MyCurve::onUnitMinusBtnClicked()
{
    if(struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
        emit boardNumChanged();
        clearPixmap();
    }
    unitLcdNum->display(struGsh.nUnit/2+1);
}
/* 多视机型板号减少按钮 */
void MyCurve::onUnitMinusBtnMultiViewClicked()
{
}
/* 信号开始/暂停按钮 */
void MyCurve::onRowPauseBtnClicked()
{
    bIsWorking = !bIsWorking;

    if (bIsWorking) {
        curveThread->start();
        rowPauseBtn->setIcon(myIcon.Media_Pause);
    } else{
        curveThread->stop();
        rowPauseBtn->setIcon(myIcon.Media_Start);
    }
}

void MyCurve::curveStartSlt()
{
    if (bIsWorking) {
        curveThread->start();
    }
}

void MyCurve::curveStopSlt()
{
    if (bIsWorking) {
        curveThread->stop();
    }
}

/* 响应绘制重叠波形按钮按下事件 */
void MyCurve::onMultiWaveBtnPressed()
{
    m_bMultiWave = !m_bMultiWave;
    if(m_bMultiWave) {
        m_multiWaveBtn->setRedColor(GREEN);
    } else {
        m_multiWaveBtn->setRedColor(DEF);
    }

    axisWidget->setMultiWaveFlag(m_bMultiWave);
}

/* 响应刷新图像的信号 */
void MyCurve::onUpdatePixmapSlt()
{
    if(!m_bMultiWave) {
        return;
    }

    int nLeft = axisWidget->getLeftBorder();
    int nRight = axisWidget->getRightBorder();
    int nUp = axisWidget->getUpBorder();
    int nDown = axisWidget->getDownBorder();

    m_pixmap = QPixmap::grabWidget(axisWidget, nLeft, nUp, axisWidget->width()-nLeft-nRight,
                                   axisWidget->height()-nUp-nDown);
    axisWidget->setPixmap(m_pixmap);

}

/*!
 * \brief 获取线程状态
 * \return  当前线程状态
 */
bool MyCurve::isRunning()
{
    return bIsWorking;
}

/* 刷新当前通道信息的界面显示 */
void MyCurve::refreshCurveDisplay()
{
    unitLcdNum->display(struGsh.nUnit / 2 + 1); //LCD

    unitViewBtn->show();
    if (m_nCurveType == 1) {
        struGsh.nUnit = struGsh.nUnit/2*2;
        unitViewBtn->hide();
    }
    if (struGsh.nUnit%2 == 1) {
        unitViewBtn->setText(myLan.rear_view);
    } else {
        unitViewBtn->setText(myLan.front_view);
    }

    if (bIsWorking) {
        rowPauseBtn->setIcon(myIcon.Media_Pause);
    } else {
        rowPauseBtn->setIcon(myIcon.Media_Start);
    }

    unitGroup->show();
}

/* 根据辅助相机使能情况隐藏或显示模式按钮 */
void MyCurve::setModeBtnStatus()
{
    bool bIsShow = false;

    int nCount = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
    for (int i = 0; i < nCount; i++) {
        if(struCnfg.nAssistCamEn[i] == 1) {
            bIsShow = true;
            break;
        }
    }

    if (bIsShow) {
        m_modeBtn->show();
        if(!struGsh.bIsAssist) {
            m_modeBtn->setText(myLan.config_major);
        } else{
            m_modeBtn->setText(myLan.config_assist);
        }
    } else {
        m_modeBtn->hide();
    }
}

/* 隐藏波形开始/暂停按钮 */
void MyCurve::hidePauseBtn()
{
    rowPauseBtn->hide();
}

/* 显示波形开始/暂停按钮 */
void MyCurve::showPauseBtn()
{
    rowPauseBtn->show();
}

/* 隐藏相机模式按钮 */
void MyCurve::hideModeBtn()
{
    m_modeBtn->hide();
}

/* 显示相机模式按钮 */
void MyCurve::showModeBtn()
{
    m_modeBtn->show();
}

/* 设置是否显示边界线 */
void MyCurve::setBorderShow(bool bFlag)
{
    axisWidget->setBorderShow(bFlag);
}

/* 根据标志位设定绘制重叠波形按钮是否使能 */
void MyCurve::setMultiWaveBtnEnabled(bool bFlag)
{
    m_multiWaveBtn->setVisible(bFlag);
}

/* 设置是否显示重叠波形 */
void MyCurve::setMultiWaveEnabled(bool bFlag)
{
    axisWidget->setMultiWaveFlag(bFlag);
    m_bMultiWave = bFlag;

    if(bFlag) {
        m_multiWaveBtn->setRedColor(GREEN);
    } else {
        m_multiWaveBtn->setRedColor(DEF);
    }

}

/* 清空绘制重叠波形时保存的图像数据 */
void MyCurve::clearPixmap()
{
    axisWidget->setPixmap(QPixmap());
    axisWidget->update();
}

/*!
 * \brief 设置波形的类型
 * \param type 波形类型
 */
void MyCurve::setCurveType(int type)
{
    m_nCurveType = type;
    emit backgroudCurveTypeChanged(m_nCurveType);
    axisWidget->setSignalType(type);
}

/***********************
 *
 * 绘制坐标轴类
 *
 ****************************************************/
MyAxis::MyAxis(QWidget *parent) :
        QWidget(parent)
{
    config = g_Config::getInstance();
    m_nDrawType = TYPE_WAVE;
    m_bDrawBoarder = false;
    m_bDrawMultiWave = false;
    m_nSignalType = 0;

    setMinValueX(0);
    setMinValueY(0);
    setMaxValueY(255);
    if(struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_1024) {
        setMaxValueX(1024);
    } else if(struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_2048) {
        setMaxValueX(2048);
    }
    setTickAttr();
    font.setPixelSize(16);

    if(LCD_WIDTH == 1024) {
        setBorder(40,10,20,50); // 设置坐标轴的左右上下边界
    } else {
        setBorder(40,10,5,35);  // 设置坐标轴的左右上下边界
    }
}

MyAxis::~MyAxis()
{
}

/* 根据分辨率来设置X坐标 */
void MyAxis::setXByData()
{
    int maxValue = struCnfc.nSensorPixelLevel[struGsh.nLevel];

    setMaxValueX(maxValue);     // 设置x轴上的最大值
}

/* 设置x轴上的最小值 */
void MyAxis::setMinValueX(int value)
{
    minValueX = value;
}

/* 设置x轴上的最大值 */
void MyAxis::setMaxValueX(int value)
{
    maxValueX = value;
}

/* 设置y轴上的最小值 */
void MyAxis::setMinValueY(int value)
{
    minValueY = value;
}

/* 设置y轴上的最大值 */
void MyAxis::setMaxValueY(int value)
{
    maxValueY = value;
}

/* 设置坐标轴的边界 */
void MyAxis::setBorder(int left, int right, int up, int down)
{
    leftBorder = left;
    rightBorder = right;
    upBorder = up;
    downBorder = down;
}

/* 获取左边界 */
int MyAxis::getLeftBorder()
{
    return leftBorder;
}

/* 获取右边界 */
int MyAxis::getRightBorder()
{
    return rightBorder;
}

/* 获取上边界 */
int MyAxis::getUpBorder()
{
    return upBorder;
}

/* 获取下边界 */
int MyAxis::getDownBorder()
{
    return downBorder;
}

/* 设置标记属性 */
void MyAxis::setTickAttr()
{
    setTickValue();
    setTickNum();
}

/* 设置标记的数目 */
void MyAxis::setTickNum()
{
    majorXNum = (maxValueX-minValueX)/majorTickValueX;  // 长刻度数量
    minorXNum = (maxValueX-minValueX)/minorTickValueX;  // 短刻度数量
    majorYNum = (maxValueY-minValueY)/majorTickValueY;
    minorYNum = (maxValueY-minValueY)/minorTickValueY;
}

/* 计算记号上的值 */
void MyAxis::setTickValue()
{
    switch(maxValueX-minValueX) {   
    case 60:
        majorTickValueX = 5;    // 长刻度单位
        minorTickValueX = 1;    // 短刻度单位
        break;
    case 512:
        majorTickValueX = 50;   // 长刻度单位
        minorTickValueX = 10;   // 短刻度单位
        break;
    case 1024:
        majorTickValueX = 100;  // 长刻度单位
        minorTickValueX = 20;   // 短刻度单位
        break;
    case 2048:
        majorTickValueX = 200;  // 长刻度单位
        minorTickValueX = 40;   // 短刻度单位
        break;
    case 2592:
        majorTickValueX = 200;  // 长刻度单位
        minorTickValueX = 40;   // 短刻度单位
        break;
    default:
        majorTickValueX = 5;   // 长刻度单位
        minorTickValueX = 1;   // 短刻度单位
        break;
    }

    switch(maxValueY) {
    case 50:
        majorTickValueY = 10;
        minorTickValueY = 2;
        break;
    case 100:
        majorTickValueY = 20;
        minorTickValueY = 4;
        break;
    case 200:
        majorTickValueY = 40;
        minorTickValueY = 8;
        break;
    case 255:
        majorTickValueY = 50;
        minorTickValueY = 10;
        break;
    case 400:
        majorTickValueY = 80;
        minorTickValueY = 16;
        break;
    case 600:
        majorTickValueY = 120;
        minorTickValueY = 24;
        break;
    case 1000:
        majorTickValueY = 200;
        minorTickValueY = 40;
        break;
    default:
        majorTickValueY = 50;
        minorTickValueY = 10;
        break;
    }
}

/* 从串口处更新波形信号数据 */
void MyAxis::getNewdata()
{
    for(int i = 0; i < struCnfc.nSensorPixelLevel[struGsh.nLevel]; i++){
        struGsh.sRowRed[i] = struGsh.sRgbRow[10+i*3];
        struGsh.sRowGreen[i] = struGsh.sRgbRow[10+i*3+1];
        struGsh.sRowBlue[i] = struGsh.sRgbRow[10+i*3+2];
    }
}

/* 设置绘制的类型 */
void MyAxis::setDrawType(int type)
{
    m_nDrawType = type;
}

/* 绘制吹气次数 */
void MyAxis::drawEjectTimes(QPainter &painter)
{
	if (struGsh.bIsAssist) {
    	drawSingleTimes(painter, ASSIST_FRONT_VIEW);
    	drawSingleTimes(painter, ASSIST_REAR_VIEW);
	} else {
    	drawSingleTimes(painter, FRONT_VIEW);
    	drawSingleTimes(painter, REAR_VIEW);
	}
    memset(struGsh.pEjectTimes, 0, sizeof(struGsh.pEjectTimes));
}

/* 绘制单视的吹气次数波形 */
void MyAxis::drawSingleTimes(QPainter &painter, int flag)
{
    int *pData;
    QPen pen;
    pen.setWidthF(2);
    switch(flag){
    case FRONT_VIEW:
        pData = struGsh.pEjectTimes;
        pen.setColor(Qt::red);
        break;
    case REAR_VIEW:
        pData = struGsh.pEjectTimes+MAX_EJECTOR_SIXTY;
        pen.setColor(Qt::blue);
        break;
	case ASSIST_FRONT_VIEW:
        pData = struGsh.pEjectTimes+MAX_EJECTOR_SIXTY*2;
        pen.setColor(Qt::red);
        break;
    case ASSIST_REAR_VIEW:
        pData = struGsh.pEjectTimes+MAX_EJECTOR_SIXTY*3;
        pen.setColor(Qt::blue);
        break;
    default:
        break;
    }

    bool isFirstpoint = true;
    QRect rect = this->rect();
    rect.adjust(leftBorder,upBorder,-rightBorder,-downBorder);
    painter.setClipRect(rect);
    painter.setPen(pen);

    QPointF currentPoint, lastPoint;
    int tmp = maxValueX-minValueX+1;
    for (int i = 1; i < tmp; i++) {
        if (isFirstpoint) {
            lastPoint.setX(leftBorder+i/valueXPerPixel);
            lastPoint.setY(rect.height()+upBorder-pData[i-1]/valueYPerPixel);
            isFirstpoint = false;
        } else {
            currentPoint.setX(leftBorder+i/valueXPerPixel);
            currentPoint.setY(rect.height()+upBorder-pData[i-1]/valueYPerPixel);
            painter.drawLine(lastPoint,currentPoint);
            lastPoint = currentPoint;
        }
    }
}

/*!
 * \brief 绘制颜色算法识别结果
 * \param painter
 */
void MyAxis::drawIdeResult(QPainter &painter)
{
    //! 根据算法使能绘制识别结果
    //! 普通颜色算法:灰度A 灰度B 色差A 色差B
    for (int i = 0; i < 4; i++) {
        if (struCnfp.nArithmeticEnable[i] == 1
                && struGsh.struResult[struGsh.nUnit].nEn[i]) {
            drawSingleResult(painter, i);
        }
    }

    //! 智能A
    if (struCnfp.nArithmeticEnable[ARITH_INTEL_A] == 1
            && struGsh.struResult[struGsh.nUnit].nEn[IDE_RESULT_INTEL_A]) {
        drawSingleResult(painter, IDE_RESULT_INTEL_A);
    }

    //! 智能B
    if (struCnfp.nArithmeticEnable[ARITH_INTEL_B] == 1
            && struGsh.struResult[struGsh.nUnit].nEn[IDE_RESULT_INTEL_B]) {
        drawSingleResult(painter, IDE_RESULT_INTEL_B);
    }
}

/*!
 * \brief 绘制单个算法识别结果
 * \param painter
 * \param flag
 */
void MyAxis::drawSingleResult(QPainter &painter, int flag)
{
    int pData[MAX_UNIT];
    memset(pData, 0, sizeof(pData));

    int nCount = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
    for (int i = 0; i < nCount; i++) {
        float fBadNum = struGsh.struResult[i].nBadNum[flag];
        float fPixelSum = struGsh.struResult[i].nPixelSum;

        if (struGsh.struResult[i].nPixelSum <= 0) {
            continue;
        }
        pData[i] = (fBadNum/fPixelSum)*100;
    }

    QPen pen;
    pen.setWidthF(2);
    QVector <QColor> vColors;
    vColors << Qt::red << Qt::green
               << Qt::blue << Qt::gray
                  << Qt::yellow << Qt::black;
    pen.setColor(vColors.at(flag));

    bool isFirstpoint = true;
    QRect rect = this->rect();
    rect.adjust(leftBorder, upBorder, -rightBorder, -downBorder);
    painter.setClipRect(rect);
    painter.setPen(pen);

    QPointF currentPoint, lastPoint;
    int tmp = maxValueX-minValueX+1;
    for (int i = 1; i < tmp; i++) {
        if(isFirstpoint){
            lastPoint.setX(leftBorder+i/valueXPerPixel);
            lastPoint.setY(rect.height()+upBorder-pData[i-1]/valueYPerPixel);
            isFirstpoint = false;
        } else {
            currentPoint.setX(leftBorder+i/valueXPerPixel);
            currentPoint.setY(rect.height()+upBorder-pData[i-1]/valueYPerPixel);
            painter.drawLine(lastPoint, currentPoint);
            lastPoint = currentPoint;
        }
    }
}

/* 设置边界是否显示 */
void MyAxis::setBorderShow(bool bFlag)
{
    m_bDrawBoarder = bFlag;
}

/* 获取边界是否显示 */
bool MyAxis::getBorderShow()
{
    return m_bDrawBoarder;
}

/* 设置是否绘制重叠波形标志位 */
void MyAxis::setMultiWaveFlag(bool bFlag)
{
    m_bDrawMultiWave = bFlag;
    m_pixmap = QPixmap();
}

/* 设置需要绘制的图像 */
void MyAxis::setPixmap(QPixmap pixmap)
{
    m_pixmap = pixmap;
}

void MyAxis::setSignalType(int type)
{
    m_nSignalType = type;
}

/* 覆写绘制事件　*/
void MyAxis::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(font);

    /* 若绘制重叠波形则无需重绘坐标轴 */
    paintAxis(painter);
    if (m_bDrawMultiWave) {
        painter.drawPixmap(leftBorder,
                           upBorder,
                           width()-leftBorder-rightBorder,
                           height()-upBorder-downBorder,
                           m_pixmap);
    }

    drawWanted(painter);
}

/* 绘制整体的坐标轴 */
void MyAxis::paintAxis(QPainter &painter)
{
    if(width()<leftBorder+rightBorder || height()<upBorder+downBorder)
        return;

    /* 根据绘制类型刷新坐标轴参数 */
    if(m_nDrawType == TYPE_WAVE) {
        setXByData();
    } else if (m_nDrawType == TYPE_EJECT_TIMES) {
        setYByData();
    } else if (m_nDrawType == TYPE_IDE_RESULT) {
        int nNum = struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal;
        setMaxValueX(nNum);
        setMaxValueY(100);
    }
    setTickAttr();

    paintAxisX(painter);
    paintAxisY(painter);
}

/* 绘制坐标轴x */
void MyAxis::paintAxisX(QPainter &painter)
{
    qreal majorTickX, minorTickX;
    QPen pen_axis(Qt::black, 2);
    QPen pen_gridline(Qt::black, 1, Qt::DotLine);
    QPen pen_majortick(Qt::black, 2);
    QPen pen_minortick(Qt::black, 1);
    QPen pen_border(QColor(180, 0, 180));
    pen_border.setWidth(1);

    valueXPerPixel = ((qreal)(maxValueX-minValueX)/(qreal)(width()-leftBorder-rightBorder));
    QPointF leftDownPoint(leftBorder, height()-downBorder);
    QPointF rightDownPoint(width()-rightBorder, height()-downBorder);
    /* 记录下tick点的坐标 */
    QPointF tmp(leftDownPoint);
    int pixelSize = painter.fontInfo().pixelSize();
    qreal halfTextLength = ((((qreal)width()-leftBorder-rightBorder))/(majorXNum-1))/3.0;
    QRect textRect;

    painter.setPen(pen_axis);
    painter.drawLine(leftDownPoint,rightDownPoint);

    /* 1.绘制主标记及纵轴虚线 */
    for (int i = 1; i <= majorXNum; i++) {
        painter.setPen(pen_majortick);
        majorTickX = (qreal)(i*majorTickValueX/valueXPerPixel);
        tmp.setX(leftBorder+majorTickX);
        /* 绘制tick */
        painter.drawLine(tmp,tmp+QPointF(0,8));
        textRect.setTopLeft(QPoint(tmp.x()-halfTextLength,tmp.y()+10));
        textRect.setSize(QSize(2*halfTextLength,pixelSize));
        /* 绘制X轴tick上的值 */
        painter.drawText(textRect, QString("%1")
                         .arg(minValueX+i*majorTickValueX),QTextOption(Qt::AlignHCenter));
        painter.setPen(pen_gridline);
        /* 绘制坐标轴中的网格 */
        painter.drawLine(tmp,QPointF(tmp.x(),upBorder));
    }

    /* 2.绘制辅标记 */
    painter.setPen(pen_minortick);
    for (int i = 1; i <= minorXNum; i++) {
        minorTickX = (qreal)(i*minorTickValueX/valueXPerPixel);
        tmp.setX(leftBorder+minorTickX);
        painter.drawLine(tmp,tmp+QPointF(0,5));
    }

    /* 3.根据需要绘制左右边界线 */
    if(m_bDrawBoarder) {
        painter.setPen(pen_border);
        /* 左边界线 */
        qreal fLeft;
		fLeft = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]/valueXPerPixel);
        tmp.setX(leftBorder+fLeft);
        painter.drawLine(tmp, QPointF(tmp.x(), upBorder));

        /* 右边界线 */
        qreal fRight;
		fRight = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]/valueXPerPixel);
		tmp.setX(leftBorder+fRight);
        painter.drawLine(tmp, QPointF(tmp.x(), upBorder));
    }
}

/* 绘制坐标轴y */
void MyAxis::paintAxisY(QPainter &painter)
{
    qreal majorTickY, minorTickY;
    QPen pen_axis(Qt::black, 2);
    QPen pen_gridline(Qt::black, 1, Qt::DotLine);
    QPen pen_majortick(Qt::black, 2);
    QPen pen_minortick(Qt::black, 1);

    valueYPerPixel = ((qreal)(maxValueY-minValueY)/(qreal)(height()-upBorder-downBorder));
    QPointF leftDownPoint(leftBorder,height()-downBorder);
    QPointF leftUpPoint(leftBorder,upBorder);
    QPointF tmp(leftDownPoint);
    QRect textRect;
    qreal halfTextHeight = ((((qreal)height()-upBorder-downBorder))/(majorYNum-1))/3.0;

    painter.setPen(pen_axis);
    painter.drawLine(leftDownPoint,leftUpPoint);

    /* 绘制主标记及横轴虚线 */
    for (int i = 1; i <= majorYNum; i++) {
        painter.setPen(pen_majortick);
        majorTickY = (qreal)(i*majorTickValueY/valueYPerPixel);
        tmp.setY(height()-downBorder-majorTickY);
        painter.drawLine(tmp, tmp+QPointF(-8,0));

        textRect.setTopLeft(QPoint(tmp.x()-40,tmp.y()-halfTextHeight));
        textRect.setSize(QSize(35,2*halfTextHeight));

        painter.drawText(textRect,QString("%1")
                         .arg(minValueY+i*majorTickValueY),QTextOption(Qt::AlignCenter));
        painter.setPen(pen_gridline);
        painter.drawLine(tmp, QPointF(width()-rightBorder,tmp.y()));
    }

    /* 绘制辅标记 */
    painter.setPen(pen_minortick);
    for (int i = 1; i <= minorYNum; i++) {
        minorTickY = (qreal)(i*minorTickValueY/valueYPerPixel);
        tmp.setY(height()-downBorder-minorTickY);
        painter.drawLine(tmp,tmp+QPointF(-5,0));
    }
}

/* 根据数据设置纵轴坐标 */
void MyAxis::setYByData()
{
    int maxValue = 0;
    for (int i = 0; i < MAX_EJECTOR_SIXTY*4; i++) {
        if(struGsh.pEjectTimes[i] > maxValue) {
            maxValue = struGsh.pEjectTimes[i];
        }
    }

    if(maxValue < 50) {
        setMaxValueY(50);
    } else if (maxValue >= 50 && maxValue < 100) {
        setMaxValueY(100);
    } else if (maxValue >= 100 && maxValue < 200) {
        setMaxValueY(200);
    } else if (maxValue >= 200 && maxValue < 400) {
        setMaxValueY(400);
    } else if (maxValue >= 400 && maxValue < 600) {
        setMaxValueY(600);
    } else {
        setMaxValueY(1000);
    }
}

/* 绘制想要的波形 */
void MyAxis::drawWanted(QPainter &painter)
{
    switch (m_nDrawType) {
    case TYPE_WAVE:
        if(struGsh.bCurAssist) {
            getNewdata();
            drawGrayWave(painter);
        } else {
            getNewdata();
            drawColorWave(painter);
        }
        break;

    case TYPE_EJECT_TIMES:
        drawEjectTimes(painter);
        break;

    case TYPE_IDE_RESULT:
        drawIdeResult(painter);
        break;

    default:
        break;
    }
}

/* 绘制彩色波形 */
void MyAxis::drawColorWave(QPainter &painter)
{
    drawSingleWave(painter, WAVE_RED);
    drawSingleWave(painter, WAVE_GREEN);
    drawSingleWave(painter, WAVE_BLUE);
}

/* 绘制灰度波形 */
void MyAxis::drawGrayWave(QPainter &painter)
{
    if(!struGsh.bCurAssist) {
        drawSingleWave(painter, WAVE_GRAY);
    } else {
        drawSingleWave(painter, WAVE_INF_RS);
    }
}

/* 根据标志位显示单一的波形 */
void MyAxis::drawSingleWave(QPainter &painter, int flag)
{
    char *pData;
    int quo = 1;

    QPen pen;
    pen.setWidthF(1);
    switch(flag){
    case WAVE_RED:
        pData = struGsh.sRowRed;
        pen.setColor(Qt::red);
        break;
    case WAVE_GREEN:
        pData = struGsh.sRowGreen;
        pen.setColor(Qt::green);
        break;
    case WAVE_BLUE:
        pData = struGsh.sRowBlue;
        pen.setColor(Qt::blue);
        break;
    case WAVE_GRAY:
        pData = struGsh.sRgbRow+10;
        if(struGsh.nUnit%2 == 0) {
            pen.setColor(Qt::red);
        } else {
            pen.setColor(Qt::blue);
        }
        break;
    case WAVE_INF_RS:
        pData = struGsh.sRgbRow+10;
        pen.setColor(Qt::red);
        break;
    case WAVE_ASSIST_RED:
        pData = struGsh.sRgbRow+10+struCnfc.nSensorPixelLevel[struGsh.nLevel]*3;
        pen.setColor(QColor(150, 0, 0));
        break;
    case WAVE_ASSIST_GREEN:
        pData = struGsh.sRgbRow+10+struCnfc.nSensorPixelLevel[struGsh.nLevel]*4;
        pen.setColor(QColor(0, 150, 0));
        break;
    case WAVE_ASSIST_BLUE:
        pData = struGsh.sRgbRow+10+struCnfc.nSensorPixelLevel[struGsh.nLevel]*5;
        pen.setColor(QColor(0, 0, 150));
        break;
    default:
        break;
    }

    bool isFirstpoint = true;
    QRect rect = this->rect();
    rect.adjust(leftBorder,upBorder,-rightBorder,-downBorder);
    painter.setClipRect(rect);
    painter.setPen(pen);

    QPointF currentPoint, lastPoint;
    for (int i = 0; i < maxValueX; i++) {
        if (isFirstpoint) {
            lastPoint.setX(leftBorder+i/valueXPerPixel);
            lastPoint.setY(rect.height()+upBorder-pData[i/quo]/valueYPerPixel);
            isFirstpoint = false;
        } else {
            currentPoint.setX(leftBorder+i/valueXPerPixel);
            currentPoint.setY(rect.height()+upBorder-pData[i/quo]/valueYPerPixel);
            painter.drawLine(lastPoint,currentPoint);
            lastPoint = currentPoint;
        }
    }
}
