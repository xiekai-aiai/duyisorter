#include "throughput.h"

Throughput::Throughput(QWidget *parent) :
    QWidget(parent)
{
    resetParams();

    createPage();

//    updatePage();
}

void Throughput::createPage()
{
    throughputTimer = new QTimer;

    throughputWidget = new QStackedWidget();
    mainPage = new QWidget();
    throughputWidget->addWidget(mainPage);
    createMainPage();

    testPage = new QWidget();
    throughputWidget->addWidget(testPage);
    createTestPage();
    throughputWidget->setFixedHeight(CURVE_WIDGET_HEIGHT+80);

    showMainPage = new QWidget();
    throughputWidget->addWidget(showMainPage);
    createShowMainPage();


    backBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    unitViewBtn = new myPushButton(myLan.front_view, QIcon());
    unitPlusBtn = new myPushButton("", myIcon.Action_Plus);
    unitLcdNum = new QLCDNumber;
    unitLcdNum->setDigitCount(2);
    unitLcdNum->display(struGsh.nUnit/2+1);
    unitMinusBtn = new myPushButton("", myIcon.Action_Minus);

    QSize btnSize = g_Config::getInstance()->getBtnSize(SMALL_BTN_SIZE);
    unitViewBtn->setFixedSize(btnSize);
    unitViewBtn->setFixedWidth(SMALL_BTN_WIDTH+20);
    unitPlusBtn->setFixedSize(btnSize);
    unitMinusBtn->setFixedSize(btnSize);
    unitLcdNum->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);

    unitGroup = new myGroupBox(myLan.chute);
    unitGroup->setFont(g_Config::getInstance()->getFont());
    unitGroup->setFixedHeight(100);

    QHBoxLayout *midLay = new QHBoxLayout(unitGroup);
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *mainLay = new QVBoxLayout(this);

    midLay->addWidget(unitViewBtn, 0);
    midLay->addSpacing(20);
    midLay->addWidget(unitMinusBtn, 1);
    midLay->addWidget(unitLcdNum, 2);
    midLay->addWidget(unitPlusBtn, 3);
    downLay->addWidget(testMainButton);
    downLay->addStretch();
    downLay->addWidget(backBtn);

    mainLay->addWidget(unitGroup);
    mainLay->addStretch();
    mainLay->addWidget(throughputWidget);
    mainLay->addStretch();
    mainLay->addLayout(downLay);

    connect(backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnSlt()));
    connect(unitViewBtn, SIGNAL(pressed()), this, SLOT(onUnitViewBtnSlt()));
    connect(unitPlusBtn, SIGNAL(pressed()), this, SLOT(onUnitPlusBtnSlt()));
    connect(unitMinusBtn, SIGNAL(pressed()), this, SLOT(onUnitMinusBtnSlt()));
    connect(throughputTimer, SIGNAL(timeout()), this, SLOT(throughputTimeoutSlt()));    
}

void Throughput::updatePage()
{
    unitGroup->hide();
    testMainButton->show();
    throughputWidget->setCurrentIndex(PAGE_SHOWMAIN);
    timer->start(1000);

//    updateThroughtputPage();
//    updateMainPage();
}

void Throughput::updateThroughtputPage()
{
    unitLcdNum->display(struGsh.nUnit / 2 + 1);
    switch(struCnfe.nMachine) {
    case MACHINE_CF:
        if (struGsh.nUnit%2 == 1) {
            unitViewBtn->setText(myLan.rear_view);
        } else {
            unitViewBtn->setText(myLan.front_view);
        }
        break;
    default:
        break;
    }
}

void Throughput::resetParams()
{
    m_time = 0;
    m_throughput = 0.0;
    pixelSum = 0;
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        isNumCheck[i] = 0;
        testPixelNum[i] = 0;
        testPutNum[i] = float(0);
        testTime[i] = 5*(i+1);
    }
    isNumCheck[0] = 1;
    afactor = struCnfg.aFactor;
    bfactor = struCnfg.bFactor;
}

void Throughput::setParams()
{
    struCnfg.aFactor = afactor;
    struCnfg.bFactor = bfactor;
}

void Throughput::onBackBtnSlt()
{
    switch(throughputWidget->currentIndex()) {
    case PAGE_SHOWMAIN:
        emit backToHomePageSig();
        timer->stop();
        break;
    case PAGE_MAIN:
        unitGroup->hide();
        testMainButton->show();
        throughputWidget->setCurrentIndex(PAGE_SHOWMAIN);
        break;
    case PAGE_TEST:
        throughputWidget->setCurrentIndex(PAGE_MAIN);
        break;
    default:
        break;
    }
}

void Throughput::onTestBtnSlt()
{
    throughputWidget->setCurrentIndex(PAGE_TEST);
    updateTestPage();
}

void Throughput::onClearTestBtnSlt()
{
    int index = 0;
    bool isFind = false;
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        if (isNumCheck[i] == 1) {
            index = i;
            isFind = true;
            break;
        }
    }
    if (isFind == false) {
        return;
    }
    testPixelNum[index] = 0;
    testPixelLdt[index]->setText(QString("%1").arg(testPixelNum[index]));
    testPutNum[index] = 0;
    testPutLdt[index]->setText(QString("%1").arg(testPutNum[index]));
}

void Throughput::onUnitViewBtnSlt()
{
    switch(struCnfe.nMachine) {
    case MACHINE_CF:
        struGsh.nUnit += (struGsh.nUnit%2 == 0) ? 1 : -1;
        if (struGsh.nUnit%2 == 0) {
            unitViewBtn->setText(myLan.front_view);
        } else {
            unitViewBtn->setText(myLan.rear_view);
        }
        break;
    default:
        break;
    }
}

void Throughput::onUnitPlusBtnSlt()
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

    if (struGsh.nUnit < idTotal-2) {
        struGsh.nUnit += 2;
    }
    unitLcdNum->display(struGsh.nUnit/2+1);
}

void Throughput::onUnitMinusBtnSlt()
{
    if (struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
    }
    unitLcdNum->display(struGsh.nUnit/2+1);
}

void Throughput::createShowMainPage(){

    chuteListLayout = new QGridLayout;
    chuteListLayout->setContentsMargins(BTN_WIDTH/2, 0, BTN_WIDTH/2, 0);

    chuteLabel = new myLabel(myLan.chute);
//    chuteLabel->setAlignment(Qt::AlignHCenter);

    chuteLocalLabel= new myLabel("本次产量(kg)");
//    chuteLocalLabel->setAlignment(Qt::AlignHCenter);
    chuteAllLabel= new myLabel("总产量(kg)");
//    chuteAllLabel->setAlignment(Qt::AlignHCenter);

    chuteListLayout->addWidget(chuteLabel, 0, 0, 1, 1);
    chuteListLayout->addWidget(chuteLocalLabel, 0, 1, 1, 1);
    chuteListLayout->addWidget(chuteAllLabel, 0, 2, 1, 2);

    for (int i = 0; i < MAX_UNIT / 2; i++) {
        chuteListNumLabel[i] = new myLabel(QString("%1").arg(i+1));
        chuteListNumLabel[i]->setFont(g_Config::getInstance()->getFont(DEFAULT_FONT_SIZE));
//        chuteListNumLabel[i]->setAlignment(Qt::AlignCenter);
        chuteListLayout->addWidget(chuteListNumLabel[i], i+1, 0, 1, 1);

        chuteListLocalLdt[i] = new myLineEdit();
        chuteListLocalLdt[i]->setAlignment(Qt::AlignCenter);
//        chuteListLocalLdt[i]->setMinimumHeight(ICON_HEI);
        chuteListLocalLdt[i]->setMaximumHeight(BTN_HEIGHT-10);
        chuteListLocalLdt[i]->setMaximumWidth(BTN_WIDTH+20);
        chuteListLayout->addWidget(chuteListLocalLdt[i], i+1, 1, 1, 1);

        chuteListAllLdt[i] = new myLineEdit();
        chuteListAllLdt[i]->setAlignment(Qt::AlignCenter);
//        chuteListAllLdt[i]->setMinimumHeight(ICON_HEI);
        chuteListAllLdt[i]->setMaximumHeight(BTN_HEIGHT-10);
        chuteListAllLdt[i]->setMaximumWidth(BTN_WIDTH+50);
        chuteListLayout->addWidget(chuteListAllLdt[i], i+1, 2, 1, 2);

    }

    chuteListAllLabel= new myLabel("汇总产量(kg)");
    chuteListLoaclNumLabel = new myLabel();
    chuteListAllNumLabel = new myLabel();
    chuteListLayout->addWidget(chuteListAllLabel,MAX_UNIT / 2+1, 0, 1, 1);
    chuteListLayout->addWidget(chuteListLoaclNumLabel,MAX_UNIT / 2+1, 1, 1, 1);
    chuteListLayout->addWidget(chuteListAllNumLabel,MAX_UNIT / 2+1, 2, 1, 1);

    QSignalMapper *chuteListLocalLdtMapper = new QSignalMapper;
    QSignalMapper *chuteListAllLdtMapper = new QSignalMapper;
    for (int i = 0; i < MAX_UNIT / 2; i++) {
        chuteListLocalLdtMapper->setMapping(chuteListLocalLdt[i], i);
        connect(chuteListLocalLdt[i], SIGNAL(pressed()), chuteListLocalLdtMapper, SLOT(map()));
        chuteListAllLdtMapper->setMapping(chuteListAllLdt[i], i);
        connect(chuteListAllLdt[i], SIGNAL(pressed()), chuteListAllLdtMapper, SLOT(map()));
    }

    connect(chuteListLocalLdtMapper, SIGNAL(mapped(int)), this, SLOT(onChuteListLocalLdtSlt(int)));
    connect(chuteListAllLdtMapper, SIGNAL(mapped(int)), this, SLOT(onChuteListAllLdtSlt(int)));


    for(int i = (struCnfg.struLevelInfo[0].nUnitLevelTotal)/2; i<MAX_UNIT / 2; i++){
        chuteListNumLabel[i]->hide();
        chuteListLocalLdt[i]->hide();
        chuteListAllLdt[i]->hide();
    }

    testMainButton = new myPushButton(myLan.throughput_test, myIcon.Action_Configure);
    testMainButton->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
//    QHBoxLayout *downLay = new QHBoxLayout;
//    downLay->addWidget(testMainButton);
//    downLay->addStretch();

    QVBoxLayout *mainLay = new QVBoxLayout(showMainPage);
//    mainLay->setContentsMargins(BTN_WIDTH/2, 0, BTN_WIDTH/2, 0);
    mainLay->addLayout(chuteListLayout);
//    mainLay->addSpacing(20);
//    mainLay->addLayout(downLay,1);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeOutSlt()));
    connect(testMainButton, SIGNAL(pressed()), this, SLOT(onTestMainButtonSlt()));

}

void Throughput::onChuteListLocalLdtSlt(int index){

}

void Throughput::onChuteListAllLdtSlt(int index){

}

void Throughput::TimeOutSlt()
{
    m_localThroughtPut = 0;
    m_AllThroughtPut = 0;
    if(throughputWidget->currentIndex() == PAGE_SHOWMAIN){
        for(int i=0; i<(struCnfg.struLevelInfo[0].nUnitLevelTotal)/2; i++){
            chuteListLocalLdt[i]->setText(QString("%1").arg(struGsh.nThroughtPut[i]));
            chuteListAllLdt[i]->setText(QString("%1").arg(struCnfg.nThroughtPut[i]));
            m_localThroughtPut +=struGsh.nThroughtPut[i];
            m_AllThroughtPut +=struCnfg.nThroughtPut[i];
        }
        chuteListLoaclNumLabel->setText(QString("%1").arg(m_localThroughtPut));
        chuteListAllNumLabel->setText(QString("%1").arg(m_AllThroughtPut));
    }
}

void Throughput::onTestMainButtonSlt(){
    unitGroup->show();
    testMainButton->hide();
    throughputWidget->setCurrentIndex(PAGE_MAIN);
    updateThroughtputPage();
    updateMainPage();
}

void Throughput::createMainPage()
{
    timeLbl = new myLabel(myLan.test_time);
    timeLdt = new myLineEdit();
    timeLdt->setMinimumHeight(ICON_HEI);
    timeLdt->setMaximumHeight(BTN_HEIGHT);
    timeLdt->setMaximumWidth(BTN_WIDTH);
    timeLdt->setText(QString("%1").arg(m_time));

    throughputLbl = new myLabel(myLan.capacity+"(kg)");
    throughputLdt = new myLineEdit();
    throughputLdt->setMinimumHeight(ICON_HEI);
    throughputLdt->setMaximumHeight(BTN_HEIGHT);
    throughputLdt->setMaximumWidth(BTN_WIDTH);
    throughputLdt->setText(QString("%1").arg(m_throughput));

    staticStartBtn = new myPushButton(myLan.start, myIcon.Media_Start);
    staticStartBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    testButton = new myPushButton(myLan.set, myIcon.Action_Configure);
    testButton->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    QGridLayout *mainLay = new QGridLayout(mainPage);
    mainLay->setContentsMargins(BTN_WIDTH, 0, BTN_WIDTH, 0);
    mainLay->addWidget(timeLbl, 0, 0);
    mainLay->addWidget(timeLdt, 0, 1);
    mainLay->addWidget(throughputLbl, 1, 0);
    mainLay->addWidget(throughputLdt, 1, 1);
    mainLay->addWidget(testButton, 3, 0);
    mainLay->addWidget(staticStartBtn, 3, 1);

    connect(timeLdt, SIGNAL(pressed()), this, SLOT(onTimeLdtSlt()));
    connect(throughputLdt, SIGNAL(pressed()), this, SLOT(onThroughputLdtSlt()));
    connect(staticStartBtn, SIGNAL(pressed()), this, SLOT(onStaticStartBtn()));
    connect(testButton, SIGNAL(pressed()), this, SLOT(onTestBtnSlt()));

}
void Throughput::updateMainPage()
{
    timeLdt->setText(QString("%1").arg(m_time));
    throughputLdt->setText(QString("%1").arg(m_throughput));
}

void Throughput::onTimeLdtSlt()
{
    myInputPanel inputDlg(intType, 0, 300, m_time);
    if (inputDlg.exec() == QDialog::Accepted) {
        m_time = inputDlg.getValue();
    }
    timeLdt->setText(QString("%1").arg(m_time));
}

void Throughput::onThroughputLdtSlt()
{
    myInputPanel inputDlg(floatType, 0, 500, m_throughput);
    if (inputDlg.exec() == QDialog::Accepted) {
        m_throughput = inputDlg.getValue();
    }
    throughputLdt->setText(QString("%1").arg(m_throughput));
}

void Throughput::onStaticStartBtn()
{
    infoWidget->setLabelText(myLan.msg_testing);
    infoWidget->delayShow();
    MySerial.com1Write(CMD_UNIT_UPDATE, UNIT, 0, struGsh.nUnit,
                       struCnfg.nProfile+1, 0, 0, 0, 1, 1);
    myFlow.onOff();
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, struGsh.nUnit, 0, 0, 0, 0, 1, 1);
    throughputTimer->start(1000*m_time);
}

void Throughput::createTestPage()
{
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        testPixelLdt[i] = new myLineEdit();
        testPixelLdt[i]->setReadOnly(true);
        testPutLdt[i] = new myLineEdit();
        testTimeLdt[i] = new myLineEdit();
        numBox[i] = new myCustomCheckBox(QString("%1").arg(i+1), isNumCheck[i], CB_STYLE_CIRCLE);

        testPixelLdt[i]->setMinimumHeight(ICON_HEI);
        testPixelLdt[i]->setMaximumHeight(BTN_HEIGHT);
        testPixelLdt[i]->setMaximumWidth(BTN_WIDTH);
        testPutLdt[i]->setMinimumHeight(ICON_HEI);
        testPutLdt[i]->setMaximumHeight(BTN_HEIGHT);
        testPutLdt[i]->setMaximumWidth(BTN_WIDTH);
        testTimeLdt[i]->setMinimumHeight(ICON_HEI);
        testTimeLdt[i]->setMaximumHeight(BTN_HEIGHT);
        testTimeLdt[i]->setMaximumWidth(BTN_WIDTH);
    }
    testPixelLbl = new myLabel(myLan.pixel);
    testPutLbl = new myLabel(myLan.weight+"(kg)");
    testTimeLbl = new  myLabel(myLan.test_time);

    beginBtn = new myPushButton(myLan.start, myIcon.Media_Start);
    computeBtn = new myPushButton(myLan.compute, myIcon.Sorter_AI);
    clearTestBtn = new myPushButton(myLan.clear, myIcon.Edit_Delete);
    aFactorLbl = new myLabel("a:");
    aFactorLdt = new myLineEdit();
    bFactorLbl = new myLabel("b:");
    bFactorLdt = new myLineEdit();
    beginBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    computeBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    clearTestBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    aFactorLdt->setMinimumHeight(ICON_HEI);
    aFactorLdt->setMaximumHeight(BTN_HEIGHT);
    aFactorLdt->setMaximumWidth(BTN_WIDTH);
    bFactorLdt->setMinimumHeight(ICON_HEI);
    bFactorLdt->setMaximumHeight(BTN_HEIGHT);
    bFactorLdt->setMaximumWidth(BTN_WIDTH);

    QGridLayout *leftLay = new QGridLayout;
    QVBoxLayout *rightLay = new QVBoxLayout;
    QHBoxLayout *mainLay = new QHBoxLayout(testPage);
    mainLay->setContentsMargins(0, 20, 0, 20);
    leftLay->addWidget(testPixelLbl, 0, 1);
    leftLay->addWidget(testPutLbl, 0, 2);
    leftLay->addWidget(testTimeLbl, 0, 3);
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        leftLay->addWidget(numBox[i],i+1, 0);
        leftLay->addWidget(testPixelLdt[i], i+1, 1);
        leftLay->addWidget(testPutLdt[i], i+1, 2);
        leftLay->addWidget(testTimeLdt[i],  i+1, 3);
    }
    rightLay->addWidget(beginBtn);
    rightLay->addWidget(computeBtn);
    rightLay->addWidget(clearTestBtn);
    rightLay->addWidget(aFactorLbl);
    rightLay->addWidget(aFactorLdt);
    rightLay->addWidget(bFactorLbl);
    rightLay->addWidget(bFactorLdt);
    mainLay->addLayout(leftLay,4);
    mainLay->addSpacing(20);
    mainLay->addLayout(rightLay,1);

    /*signal & slt*/
    QSignalMapper *numBoxMapper = new QSignalMapper;
    QSignalMapper *testPixelLdtMapper = new QSignalMapper;
    QSignalMapper *testPutLdtMapper = new QSignalMapper;
    QSignalMapper *testTimeLdtMapper = new QSignalMapper;
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        numBoxMapper->setMapping(numBox[i], i);
        connect(numBox[i], SIGNAL(pressed()), numBoxMapper, SLOT(map()));
        testPixelLdtMapper->setMapping(testPixelLdt[i], i);
        connect(testPixelLdt[i], SIGNAL(pressed()), testPixelLdtMapper, SLOT(map()));
        testPutLdtMapper->setMapping(testPutLdt[i], i);
        connect(testPutLdt[i], SIGNAL(pressed()), testPutLdtMapper, SLOT(map()));
        testTimeLdtMapper->setMapping(testTimeLdt[i], i);
        connect(testTimeLdt[i], SIGNAL(pressed()), testTimeLdtMapper, SLOT(map()));
    }

    connect(numBoxMapper, SIGNAL(mapped(int)), this, SLOT(onNumBoxSlt(int)));
    connect(testPixelLdtMapper, SIGNAL(mapped(int)), this, SLOT(onTestPixelLdtSlt(int)));
    connect(testPutLdtMapper, SIGNAL(mapped(int)), this, SLOT(onTestPutLdtSlt(int)));
    connect(testTimeLdtMapper, SIGNAL(mapped(int)), this, SLOT(onTestTimeLdtSlt(int)));
    connect(aFactorLdt, SIGNAL(pressed()), this, SLOT(onaFactorLdtSlt()));
    connect(bFactorLdt, SIGNAL(pressed()), this, SLOT(onbFactorLdtSlt()));
    connect(beginBtn, SIGNAL(pressed()), this, SLOT(onBeginBtnSlt()));
    connect(computeBtn, SIGNAL(pressed()), this, SLOT(onComputeBtnSlt()));
    connect(clearTestBtn, SIGNAL(pressed()), this, SLOT(onClearTestBtnSlt()));
}

void Throughput::updateTestPage()
{
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        numBox[i]->setChecked(isNumCheck[i]);
        testPixelLdt[i]->setText(QString("%1").arg(testPixelNum[i]));
        testPutLdt[i]->setText(QString("%1").arg(testPutNum[i]));
        testTimeLdt[i]->setText(QString("%1").arg(testTime[i]));
    }
    aFactorLdt->setText(QString("%1").arg(afactor));
    bFactorLdt->setText(QString("%1").arg(bfactor));
}

void Throughput::onNumBoxSlt(int index)
{
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        if (i!=index) {
            isNumCheck[i] = 0;
            numBox[i]->setChecked(false);
        } else {
            isNumCheck[i] = 1;
            numBox[i]->setChecked(true);
        }
    }
}

void Throughput::onTestPixelLdtSlt(int index)
{

}

void Throughput::onTestPutLdtSlt(int index)
{
    myInputPanel inputDlg(floatType, 0, 500, testPutNum[index]);
    if (inputDlg.exec() == QDialog::Accepted) {
        testPutNum[index] = inputDlg.getValue();
    }
    testPutLdt[index]->setText(QString("%1").arg(testPutNum[index]));
}

void Throughput::onTestTimeLdtSlt(int index)
{
    myInputPanel inputDlg(intType, 0, 300, testTime[index]);
    if (inputDlg.exec() == QDialog::Accepted) {
        testTime[index] = inputDlg.getValue();
    }
    testTimeLdt[index]->setText(QString("%1").arg(testTime[index]));
}

void Throughput::onaFactorLdtSlt()
{
    myInputPanel inputDlg(floatType, 0, 100, afactor);
    if (inputDlg.exec() == QDialog::Accepted) {
        afactor = inputDlg.getValue();
    }
    aFactorLdt->setText(QString("%1").arg(afactor));
	setParams();
}

void Throughput::onbFactorLdtSlt()
{
    myInputPanel inputDlg(floatType, 0, 100, bfactor);
    if (inputDlg.exec() == QDialog::Accepted) {
        bfactor = inputDlg.getValue();
    }
    bFactorLdt->setText(QString("%1").arg(bfactor));
	setParams();
}

void Throughput::onBeginBtnSlt()
{
    int index = 0;
    bool isFind = false;
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        if (isNumCheck[i] == 1) {
            index = i;
            isFind = true;
            break;
        }
    }
    if (isFind == false) {
        return;
    }
    infoWidget->setLabelText(myLan.msg_testing);
    infoWidget->delayShow();
    MySerial.com1Write(CMD_UNIT_UPDATE, UNIT, 0, struGsh.nUnit,
                       struCnfg.nProfile+1, 0, 0, 0, 1, 1);
    myFlow.onOff();
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, struGsh.nUnit, 0, 0, 0, 0, 1, 1);
    throughputTimer->start(1000*testTime[index]);
}

void Throughput::throughputTimeoutSlt()
{
    quint64  pixelNum;
    throughputTimer->stop();
    int index = 0;
    bool isFind = false;
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, struGsh.nUnit, 0, 0, 0, 0, 0, 1);
    myFlow.onOff();
    infoWidget->hide();
    pixelNum = getStaticsFun();

    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        if (isNumCheck[i] == 1) {
            index = i;
            isFind = true;
            break;
        }
    }
    if (isFind == false) {
        return;
    }
    printf("%lld\n", pixelNum);
    fflush(0);
    switch(throughputWidget->currentIndex()) {
    case PAGE_MAIN:
         pixelSum = pixelNum;
         if(pixelNum == 0){
            m_throughput = 0;
         }else{
             m_throughput = afactor*pixelSum + bfactor;
         }
         throughputLdt->setText(QString("%1").arg(m_throughput));
         break;
    case PAGE_TEST:
        testPixelNum[index] = pixelNum;
        testPixelLdt[index]->setText(QString("%1").arg(testPixelNum[index]));
        break;
    }
    MySerial.com1Write(CMD_UNIT_UPDATE, UNIT, 0, struGsh.nUnit,
                       struCnfg.nProfile+1, 0, 0, 0, 0, 1);
}

/*!
 * \brief Throughput::onComputeBtnSlt
 */
void Throughput::onComputeBtnSlt()
{
    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->showSecs();

    double pixelSum = 0;
    double outputSum = 0;
    double pixsquareSum = 0;
    double pAndoSum = 0;

    /* 统计有效数据组的个数*/
    int nStaticNum = 0;
    for (int i = 0; i < MAX_STATIC_NUM; i++) {
        if (testPixelNum[i] !=0 ) {
            nStaticNum++;
        }
    }

    for (int i = 0; i < nStaticNum; i++) {
        pixelSum += (double)testPixelNum[i];
        outputSum += (double)testPutNum[i];
        pixsquareSum += (double)testPixelNum[i]*(double)testPixelNum[i];
        pAndoSum +=(double)testPixelNum[i]*(double)testPutNum[i];
    }

    if((nStaticNum*pixsquareSum - pixelSum*pixelSum) == 0){
        afactor = 0;
        bfactor = 0;
    } else {
        afactor = (nStaticNum*pAndoSum-pixelSum*outputSum)
        /(nStaticNum*pixsquareSum - pixelSum*pixelSum);
        bfactor = outputSum/nStaticNum - afactor*pixelSum/nStaticNum;
    }
    printf("afactor:%lf, bfactor:%lf\n", afactor, bfactor);
    fflush(0);

    aFactorLdt->setText(QString("%1").arg(afactor));
    bFactorLdt->setText(QString("%1").arg(bfactor));
    infoWidget->hide();
    setParams();
}

/*!
 * \brief Throughput::getStaticsFun
 * \return
 */
quint64 Throughput::getStaticsFun()
{
    quint64  pixelNum = 0;
    quint64  badPointNum = 0;
    float    realDirtPer = 0;
    QByteArray materialStatistic;
    materialStatistic.resize(32);
    materialStatistic.fill(0);
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_UPLOAD, UNIT, 0, struGsh.nUnit, 0, 0, 0, 0, 0, 1);
    MySerial.com1Read(materialStatistic.data(), materialStatistic.size(), 1000);
    materialStatistic.remove(0,10);
    materialStatistic.truncate(materialStatistic.size()-6);

    for (int j = 0; j < 8; j++) {
        printf("%02x ", materialStatistic.at(j));
        pixelNum += ((quint64)materialStatistic.at(j))<< ((7-j)*8);
    }
    for (int j = 8; j < 16; j++) {
        printf("%02x ", materialStatistic.at(j));
        badPointNum +=((quint64)materialStatistic.at(j))<< ((15-j)*8);
    }
    if (pixelNum == 0) {
        realDirtPer = 0;
    } else {
        realDirtPer = (float)badPointNum/(float)pixelNum;
    }
    printf("%lld...%lld...%f\n",pixelNum, badPointNum, realDirtPer);
    fflush(0);
    return pixelNum;
}
