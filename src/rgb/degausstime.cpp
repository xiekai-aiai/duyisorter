#include "degausstime.h"

DegaussTime::DegaussTime(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0, 0, LCD_WIDTH-10, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);
    setFixedSize(LCD_WIDTH-10, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);
    m_config = g_Config::getInstance();

    createDegaussTimePage();
    updateDegaussTimePage();
}
/* 创建消磁时间页面 */
void DegaussTime::createDegaussTimePage()
{
    /* page init */
    degaussTimeWidget = new QWidget;

    degaussTimeTabBar = new MyTabBar(degaussTimeWidget);
    /*消磁时间、截断时间、辅配消磁时间、辅配截断时间*/
    for(int i = 0; i < GROUPS_NUM ;i++)
    {
        m_groups[i] = new myGroupBox("");
        m_groups[i]->setFixedSize(LCD_WIDTH-100,LCD_HEIGHT/GROUPS_NUM);
    }

    m_groups[GROUP_DEGAUSS]->setTitle(myLan.degauss_time + "(ms)");
    m_groups[GROUP_TRUNCATION]->setTitle(myLan.truncation_time + "(ms)");
    m_groups[GROUP_ASSIST_DEGAUSS]->setTitle(myLan.config_assist + myLan.degauss_time + "(ms)");
    m_groups[GROUP_ASSIST_TRUNCATION]->setTitle(myLan.config_assist + myLan.truncation_time + "(ms)");

    for(int i = 0 ; i < GROUPS_NUM; i++)
    {
        m_MinusBtn[i] = new myPushButton(myIcon.Action_Minus, QSize(BTN_HEIGHT, BTN_HEIGHT));
        m_PlusBtn[i]  = new myPushButton(myIcon.Action_Plus, QSize(BTN_HEIGHT, BTN_HEIGHT));

        m_lcdNums[i] = new myLCDNumber();
        m_lcdNums[i]->setDigitCount(4);
        m_lcdNums[i]->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    }

    degaussTimeBackBtn = new myPushButton(myLan.back, myIcon.Action_Back, true, true, degaussTimeWidget);
    degaussTimeBackBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    /* page layout */
    QVBoxLayout *layout = new QVBoxLayout(degaussTimeWidget);
    layout->setSpacing(50);
    QHBoxLayout *upLay = new QHBoxLayout();
    upLay->addWidget(degaussTimeTabBar);
    layout->addLayout(upLay);
    layout->addStretch();

    QHBoxLayout *midlayout[GROUPS_NUM];
    for(int i = 0;i < GROUPS_NUM; i++){
        midlayout[i] = new QHBoxLayout(m_groups[i]);
        midlayout[i]->addWidget(m_MinusBtn[i]);
        midlayout[i]->addWidget(m_lcdNums[i]);
        midlayout[i]->addWidget(m_PlusBtn[i]);
        layout->addWidget(m_groups[i]);
    }

    QHBoxLayout *downLay = new QHBoxLayout();
    downLay->addStretch();
    downLay->addWidget(degaussTimeBackBtn);
    layout->addStretch();
    layout->addLayout(downLay);
    setLayout(layout);

    /* signal & slot */
    m_minusBtnMapper = new QSignalMapper(this);
    m_plusBtnMapper = new QSignalMapper(this);
    m_lcdMapper = new QSignalMapper(this);
    for (int i = 0; i < GROUPS_NUM; i++) {
        m_minusBtnMapper->setMapping(m_MinusBtn[i],i);
        m_plusBtnMapper->setMapping(m_PlusBtn[i],i);
        m_lcdMapper->setMapping(m_lcdNums[i], i);

        connect(m_MinusBtn[i],SIGNAL(pressed()),m_minusBtnMapper,SLOT(map()));
        connect(m_PlusBtn[i],SIGNAL(pressed()),m_plusBtnMapper,SLOT(map()));
        connect(m_lcdNums[i],SIGNAL(pressed()),m_lcdMapper,SLOT(map()));
    }
    connect(m_minusBtnMapper,SIGNAL(mapped(int)),this ,SLOT(onDegaussTimeMinusBtnClicked(int)));
    connect(m_plusBtnMapper,SIGNAL(mapped(int)),this ,SLOT(onDegaussTimePlusBtnClicked(int)));
    connect(m_lcdMapper,SIGNAL(mapped(int)),this ,SLOT(onDegaussTimeValueChanged(int)));

    connect(degaussTimeTabBar,  SIGNAL(currentChanged(int)), this, SLOT(onDegaussTimeTabChanged(int)));
    connect(degaussTimeBackBtn, SIGNAL(pressed()),           this, SLOT(onDegaussTimeBackBtnClicked()));
}

void DegaussTime::updateDegaussTimePage()
{
    switch (struCnfe.nMachine) {
    default:
        updateTabBar(degaussTimeTabBar, false, false);
        break;
    }

    // 辅配相机使能才显示辅配消磁时间和截断时间
    if(!struCnfg.bAssitCameraEnable) {
        m_groups[2]->hide();
        m_groups[3]->hide();
    }
    else{
        m_groups[2]->show();
        m_groups[3]->show();
    }
}

/* 响应消磁时间页面Tab索引按钮 */
void DegaussTime::onDegaussTimeTabChanged(int index)
{
    if (index < 0)
        return;

    switch (struCnfe.nMachine) {
    case MACHINE_CF:
        struGsh.nLevel = 0;
        currentChan = index;
        break;
    default:
        currentChan = 0;
        break;
    }

    /* update degaussTimeValue */
    m_lcdNums[0]->display(struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectDegauss * 0.1);
    m_lcdNums[1]->display(struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectTruncation * 0.1);
    m_lcdNums[2]->display(struCnfp.struGroupTick[struGsh.nLevel][currentChan].nAssistEjectDegauss * 0.1);
    m_lcdNums[3]->display(struCnfp.struGroupTick[struGsh.nLevel][currentChan].nAssistEjectTruncation * 0.1);
}

/*参数存储与下发*/
void DegaussTime::sendDegauessTime(int index)
{
    double tmpValue  = m_lcdNums[index]->value();
    switch(index){
        case 0:
            struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectDegauss = tmpValue*10 + 0.5;
            break;
        case 1:
            struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectTruncation = tmpValue*10 + 0.5;
            break;
        case 2:
            struCnfp.struGroupTick[struGsh.nLevel][currentChan].nAssistEjectDegauss = tmpValue*10 + 0.5;
            break;
        case 3:
            struCnfp.struGroupTick[struGsh.nLevel][currentChan].nAssistEjectTruncation = tmpValue*10 + 0.5;
            break;
    }
    /* reset parameters */
    myFlow.resetEjectTime();
}

/* 响应消磁时间组valueBar按纽 */
void DegaussTime::onDegaussTimeValueChanged(int index)
{
    double tmpVal;
    int ret;
    switch(index){
    case 0:
        tmpVal = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectDegauss*0.1;
        break;
    case 1:
        tmpVal = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nEjectTruncation*0.1;
        break;
    case 2:
        tmpVal = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nAssistEjectDegauss*0.1;
        break;
    case 3:
        tmpVal = struCnfp.struGroupTick[struGsh.nLevel][currentChan].nAssistEjectTruncation*0.1;
        break;
    default:
        break;
    }
    getMaxAndMinValue(index);
    myInputPanel inputDlg1(floatType, 0, 6, tmpVal);
    ret  = inputDlg1.exec();
    if (ret == QDialog::Accepted) {
        tmpVal = inputDlg1.getValue()*10;
        if(tmpVal > m_max)
            tmpVal = m_max;
        if(tmpVal < m_min)
            tmpVal = m_min;
        /* update lcdnumber */
        m_lcdNums[index]->display(tmpVal/10.0);

        sendDegauessTime(index);
    }
}

/* 响应消磁时间组减少按钮 */
void DegaussTime::onDegaussTimeMinusBtnClicked(int index)
{
    int value = m_lcdNums[index]->value()*10+0.5;
    getMaxAndMinValue(index);
    if(value > m_min)
        value--;
    printf("%d\n",value);
    fflush(0);
    m_lcdNums[index]->display(value/10.0);

    sendDegauessTime(index);
}

/* 响应消磁时间组增加按钮 */
void DegaussTime::onDegaussTimePlusBtnClicked(int index)
{
    int value = m_lcdNums[index]->value()*10+0.5;
    getMaxAndMinValue(index);
    if(value < m_max)
        value++;
    m_lcdNums[index]->display(value/10.0);

    sendDegauessTime(index);
}

/*获取不同输入框上下界*/
void DegaussTime::getMaxAndMinValue(int index)
{
    switch (index){
    case 0: // 消磁
    case 2:
        m_min = 1;
        m_max = 12;
        break;
    case 1: // 截断
    case 3:
        m_min = 4;
        m_max = 60;
        break;
    }
}

/* 响应消磁时间界面设置返回按钮 */
void DegaussTime::onDegaussTimeBackBtnClicked()
{
    emit backToVenderset(); //返回厂家设置页面
}
