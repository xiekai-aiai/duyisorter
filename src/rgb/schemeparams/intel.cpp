#include "intel.h"

/* 构造智能参数固化页面 */
PageIntel::PageIntel(QWidget *parent)
    : QWidget(parent)
{
/* page init */
    createPage();

/* page update */
    updatePage();

/* page layout */
    layoutPage();
}

/* 应用当前智能参数固化页面设置 */
void PageIntel::setIntelParams()
{

}

/* 重置当前智能参数固化页面设置 */
void PageIntel::resetIntelParams()
{
/* reset page */
    emit pageUpdated();
}

/* 初始化智能参数固化页面 */
void PageIntel::createPage()
{
    /* private data */
    currentChan = 0;

    /* tab */
    tabBar = new MyTabBar();

    /* list */
    listWidget = new myListWidget();
    listWidget->setFixedWidth(BTN_WIDTH);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setIconSize(QSize(ICON_WID, ICON_HEI));
    listWidget->setSpacing(10);

    for (int i = 0; i < 2; i++) {
        listItem[i] = new myListWidgetItem(myLan.ai_analysis, myIcon.Sorter_AI, QSize(BTN_WIDTH-20, ICON_HEI*2));
    }

    /* stack */
    stackWidget = new QStackedWidget();

    // page normal intel
    pageNormal = new QWidget();
    createNormalIntelPage(pageNormal);

    // page multi intel
    pageMulti = new QWidget();
    createMultiIntelPage(pageMulti);

    // signals & slots
    connect(listWidget, SIGNAL(currentRowChanged(int)), stackWidget, SLOT(setCurrentIndex(int)));
    connect(tabBar,     SIGNAL(currentChanged(int)),    this,        SLOT(changeCurrentSlt(int)));
    connect(this,       SIGNAL(pageUpdated()),          this,        SLOT(updatePage()));
}

void PageIntel::updateListWidget()
{
    /* clear list widget */
    listWidget->clear();

    /* clear stacked widget */
    for (int i = stackWidget->count()-1; i >= 0; i--) {
        stackWidget->removeWidget(stackWidget->widget(i));
    }

    /* update page intel */
    for (int i = 0; i < MAX_LEVEL; i++) {
        if (struCnfp.nArithmeticEnableLevel[i][ARITH_INTEL_A] == 1) {
            if (struCnfp.nIntelMode == 1) {
                listItem[0] = new myListWidgetItem(myLan.ai_multi_mode, myIcon.Sorter_AI, QSize(BTN_WIDTH-20, ICON_HEI*2));
                listWidget->addItem(listItem[0]);
                stackWidget->addWidget(pageMulti);
            } else {
                listItem[1] = new myListWidgetItem(myLan.ai_class_mode_usual, myIcon.Sorter_AI, QSize(BTN_WIDTH-20, ICON_HEI*2));
                listWidget->addItem(listItem[1]);
                stackWidget->addWidget(pageNormal);
            }

            listWidget->setCurrentRow(0);
            break;
        }
    }
}

/* 更新智能参数固化页面 */
void PageIntel::updatePage()
{
    /* update list widget */
    updateListWidget();

    /* update tab widget page */
    updateTabBar(tabBar, true, false);
}

/* 响应智能参数固化页面tab切换 */
void PageIntel::changeCurrentSlt(int index)
{
    /* get level info */
    switch(struCnfe.nMachine){
    case MACHINE_CF:
        struGsh.nLevel = ONE_LEVEL;
        currentChan    = index;
        break;
    default:
        break;
    }

    /* update normal intel page */
    updateNormalIntelPage();

    /* update multi intel page */
    updateMultiIntelPage();
}

/* 布局智能参数固化页面 */
void PageIntel::layoutPage()
{
/* page layout */
    QHBoxLayout *upLayout = new QHBoxLayout();
    QSpacerItem *upSpacer = new QSpacerItem(BTN_WIDTH, BTN_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
    upLayout->addItem(upSpacer);
    upLayout->addWidget(tabBar);

    QHBoxLayout *downLayout = new QHBoxLayout();
    downLayout->addWidget(listWidget);
    downLayout->addWidget(stackWidget);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addLayout(upLayout, 0, 0);
    mainLayout->addLayout(downLayout, 1, 0);

    setLayout(mainLayout);
}

/* 初始化普通智能参数固化页面 */
void PageIntel::createNormalIntelPage(QWidget *parent)
{
    QString strAIRed;
    QString strAIGreen;
    QString strAIBlue;
    QString strAICons;

/* params area */
    AISignalMapper = new QSignalMapper(parent);

    for (int i = 0; i < MAX_INTEL; i++) {
        AISensListCbx[i] = new myGroupBox(QString("%1-%2").arg(myLan.hard_code).arg(i), parent);

        AIRedLabel[i]   = new myLabel("系数一", parent);

        strAIRed.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo1[1]);
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo1[0] == 1)
            strAIRed = "-" + strAIRed;
        AIRedLbe[i]     = new myLineEdit(strAIRed, parent);
        AIRedLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (LCD_WIDTH == 640)
            AIRedLbe[i]->setMaximumWidth(80);

        AIGreenLabel[i]    = new myLabel("系数二", parent);

        strAIGreen.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo2[1]);
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo2[0] == 1)
            strAIGreen = "-" + strAIGreen;
        AIGreenLbe[i]      = new myLineEdit(strAIGreen, parent);
        AIGreenLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (LCD_WIDTH == 640)
            AIGreenLbe[i]->setMaximumWidth(80);

        AIBlueLabel[i]   = new myLabel("系数三", parent);

        strAIBlue.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo3[1]);
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo3[0] == 1)
            strAIBlue = "-" + strAIBlue;
        AIBlueLbe[i]            = new myLineEdit(strAIBlue, parent);
        AIBlueLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (LCD_WIDTH == 640)
            AIBlueLbe[i]->setMaximumWidth(80);

        AIConsLabel[i] = new myLabel("系数四", parent);

        strAICons.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nCons[1]);
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nCons[0] == 1)
            strAICons = "-" + strAICons;
        AIConsLbe[i]   = new myLineEdit(strAICons, parent);
        AIConsLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (LCD_WIDTH == 640)
            AIConsLbe[i]->setMaximumWidth(80);

    /* page layout */
        AINormalBoxLayout[i]    = new QHBoxLayout(AISensListCbx[i]);
        AINormalBoxLayout[i]->addWidget(AIRedLabel[i]);
        AINormalBoxLayout[i]->addWidget(AIRedLbe[i]);
        AINormalBoxLayout[i]->addWidget(AIGreenLabel[i]);
        AINormalBoxLayout[i]->addWidget(AIGreenLbe[i]);
        AINormalBoxLayout[i]->addWidget(AIBlueLabel[i]);
        AINormalBoxLayout[i]->addWidget(AIBlueLbe[i]);
        AINormalBoxLayout[i]->addWidget(AIConsLabel[i]);
        AINormalBoxLayout[i]->addWidget(AIConsLbe[i]);
        AINormalBoxLayout[i]->setContentsMargins(0,6,0,2);
        AINormalBoxLayout[i]->setSpacing(5);

        /* signals & slots */
        AISignalMapper->setMapping(AIRedLbe[i], i*MAX_INTEL_QUO);
        connect(AIRedLbe[i], SIGNAL(pressed()), AISignalMapper, SLOT(map()));
        AISignalMapper->setMapping(AIGreenLbe[i], i*MAX_INTEL_QUO+1);
        connect(AIGreenLbe[i], SIGNAL(pressed()), AISignalMapper, SLOT(map()));
        AISignalMapper->setMapping(AIBlueLbe[i], i*MAX_INTEL_QUO+2);
        connect(AIBlueLbe[i], SIGNAL(pressed()), AISignalMapper, SLOT(map()));
        AISignalMapper->setMapping(AIConsLbe[i], i*MAX_INTEL_QUO+3);
        connect(AIConsLbe[i], SIGNAL(pressed()), AISignalMapper, SLOT(map()));
    }
    connect(AISignalMapper, SIGNAL(mapped(int)), this, SLOT(getNormalIntelIndex(int)));

    AINormalLayout = new QGridLayout(parent);
    for (int i = 0; i < MAX_INTEL;i++) {
        AINormalLayout->addWidget(AISensListCbx[i], i, 0);
    }
}

/* 刷新普通智能参数页面 */
void PageIntel::updateNormalIntelPage()
{
    QString strAIRed;
    QString strAIGreen;
    QString strAIBlue;
    QString strAICons;

    for (int i = 0; i < MAX_INTEL; i++) {
        AISensListCbx[i]->hide();
    }

    for (int i = 0; i < MAX_INTEL; i++) {
        if (struCnfp.nArithmeticEnableLevel[struGsh.nLevel][ARITH_INTEL_A+i] == 1) {
             AISensListCbx[i]->setMaximumHeight(380/MAX_INTEL);

//             strAIRed = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo1[1]);
             strAIRed.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo1[1]);
             if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo1[0] == 1)
                 strAIRed = "-"+ strAIRed;
             AIRedLbe[i]->setText(strAIRed);

//             strAIGreen = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo2[1]);
             strAIGreen.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo2[1]);
             if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo2[0] == 1)
                 strAIGreen = "-"+ strAIGreen;
             AIGreenLbe[i]->setText(strAIGreen);

//             strAIBlue = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo3[1]);
             strAIBlue.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo3[1]);
             if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nQuo3[0] == 1)
                 strAIBlue = "-"+ strAIBlue;
             AIBlueLbe[i]->setText(strAIBlue);

//             strAICons = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nCons[1]);
             strAICons.sprintf("%d", struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nCons[1]);
             if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[i].nCons[0] == 1)
                 strAICons = "-"+ strAICons;
             AIConsLbe[i]->setText(strAICons);

             AISensListCbx[i]->show();
        }
    }
}

/* 响应普通智能参数设置 */
void PageIntel::getNormalIntelIndex(int index)
{
    bool ok;

    // 系数一设置
    if (index%MAX_INTEL_QUO == 0) {
        double nQuo1[MAX_INTEL];
        myInputMethod inputDlg1(myLan.input, AIRedLbe[index/MAX_INTEL_QUO]->text());
        if (inputDlg1.exec() == QDialog::Accepted)
        {
            nQuo1[index/MAX_INTEL_QUO] = inputDlg1.getText().toDouble(&ok);
            if (ok == true) {
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nQuo1[0] = nQuo1[index/MAX_INTEL_QUO] >= 0 ? 0 : 1;
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nQuo1[1] = abs(nQuo1[index/MAX_INTEL_QUO]);

                QString str = QString("%1").arg(nQuo1[index/MAX_INTEL_QUO]);
                AIRedLbe[index/MAX_INTEL_QUO]->setText(str);

                /* 参数发送 */
                myFlow.materialCopyAssemble(struGsh.nLevel      , currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
                myFlow.materialResetGroupAssemble(struGsh.nLevel, currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
            }
        }
    }

    // 系数二设置
    if (index%MAX_INTEL_QUO == 1) {
        double nQuo2[MAX_INTEL];
        myInputMethod inputDlg2(myLan.input, AIGreenLbe[index/MAX_INTEL_QUO]->text());
        if (inputDlg2.exec() == QDialog::Accepted)
        {
            nQuo2[index/MAX_INTEL_QUO] = inputDlg2.getText().toDouble(&ok);
            if (ok == true) {
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nQuo2[0] = nQuo2[index/MAX_INTEL_QUO] >= 0 ? 0 : 1;
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nQuo2[1] = abs(nQuo2[index/MAX_INTEL_QUO]);

                QString str = QString("%1").arg(nQuo2[index/MAX_INTEL_QUO]);
                AIGreenLbe[index/MAX_INTEL_QUO]->setText(str);

                /* 参数发送 */
                myFlow.materialCopyAssemble(struGsh.nLevel      , currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
                myFlow.materialResetGroupAssemble(struGsh.nLevel, currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
            }
        }
    }

    // 系数三设置
    if (index%MAX_INTEL_QUO == 2) {
        double nQuo3[MAX_INTEL];
        myInputMethod inputDlg3(myLan.input, AIBlueLbe[index/MAX_INTEL_QUO]->text());
        if (inputDlg3.exec() == QDialog::Accepted)
        {
            nQuo3[index/MAX_INTEL_QUO] = inputDlg3.getText().toDouble(&ok);
            if (ok == true) {
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nQuo3[0] = nQuo3[index/MAX_INTEL_QUO] >= 0 ? 0 : 1;
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nQuo3[1] = abs(nQuo3[index/MAX_INTEL_QUO]);

                QString str = QString("%1").arg(nQuo3[index/MAX_INTEL_QUO]);
                AIBlueLbe[index/MAX_INTEL_QUO]->setText(str);

                /* 参数发送 */
                myFlow.materialCopyAssemble(struGsh.nLevel      , currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
                myFlow.materialResetGroupAssemble(struGsh.nLevel, currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
            }
        }
    }

    // 系数四设置
    if (index%MAX_INTEL_QUO == 3) {
        double nCons[MAX_INTEL];
        myInputMethod inputDlg4(myLan.input, AIConsLbe[index/MAX_INTEL_QUO]->text());
        if (inputDlg4.exec() == QDialog::Accepted) {
            nCons[index/MAX_INTEL_QUO] = inputDlg4.getText().toDouble(&ok);
            if (ok == true) {
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nCons[0] = nCons[index/MAX_INTEL_QUO] >= 0 ? 0 : 1;
                struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[index/MAX_INTEL_QUO].nCons[1] = abs(nCons[index/MAX_INTEL_QUO]);

                QString str = QString("%1").arg(nCons[index/MAX_INTEL_QUO]);
                AIConsLbe[index/MAX_INTEL_QUO]->setText(str);

                /* 参数发送 */
                myFlow.materialCopyAssemble(struGsh.nLevel      , currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
                myFlow.materialResetGroupAssemble(struGsh.nLevel, currentChan, 0, ARITH_INTEL_A+index/MAX_INTEL_QUO, 0);
            }
        }
    }
}

/* 创建多分类模式参数固化页面 */
void PageIntel::createMultiIntelPage(QWidget *parent)
{
    /* signals */
    AIMultiSignalMapper = new QSignalMapper(parent);

    /* group box */
    AIMultiQuoListCbx = new myGroupBox(myLan.ai_multi_mode);
    AIMultiQuoListCbx->setStyleSheet("border:none");
    /* label & lineEdit */
    for (int i = 0; i < MAX_MULTI_INTEL; i++) {
        AIMultiQuoLabel[i] = new myLabel(QString("%1%2").arg("系数").arg(i), parent);
        AIMultiQuoLbe[i]   = new myLineEdit("255", parent);
        AIMultiQuoLbe[i]->setMaximumHeight(BTN_HEIGHT);
        if (LCD_WIDTH == 640)
            AIMultiQuoLabel[i]->setMaximumWidth(80);

        AIMultiBoxLayout[i] = new QHBoxLayout();
        AIMultiBoxLayout[i]->addWidget(AIMultiQuoLabel[i]);
        AIMultiBoxLayout[i]->addSpacing(5);
        AIMultiBoxLayout[i]->addWidget(AIMultiQuoLbe[i]);

        AIMultiSignalMapper->setMapping(AIMultiQuoLbe[i], i);
        connect(AIMultiQuoLbe[i], SIGNAL(pressed()), AIMultiSignalMapper, SLOT(map()));
    }
    connect(AIMultiSignalMapper, SIGNAL(mapped(int)), this, SLOT(getMultiIntelIndex(int)));

    /* grid layout */
    AIMultiLayout = new QGridLayout(parent);
    AIMultiLayout->addWidget(AIMultiQuoListCbx, 0, 0);
    for (int i = 0; i < MAX_MULTI_INTEL; i++) {
        AIMultiLayout->addLayout(AIMultiBoxLayout[i], i/3+1, i%3);
    }
}

/* 获取多分类模式固化参数显示文本 */
QString PageIntel::getMultiQuoText(int index)
{
    QString strMultiQuo;
    switch (index) {
    case 0:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoR[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoR[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoR[1]);
        break;
    case 1:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoG[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoG[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoG[1]);
        break;
    case 2:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoB[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoB[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoB[1]);
        break;
    case 3:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRG[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRG[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRG[1]);
        break;
    case 4:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRB[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRB[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRB[1]);
        break;
    case 5:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGB[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGB[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGB[1]);
        break;
    case 6:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRR[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRR[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRR[1]);
        break;
    case 7:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGG[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGG[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGG[1]);
        break;
    case 8:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoBB[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoBB[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoBB[1]);
        break;
    case 9:
        if (struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nConsDfl[0] == 1)
            strMultiQuo = "-" + QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nConsDfl[1]);
        else
            strMultiQuo = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nConsDfl[1]);
        break;
    default:
        strMultiQuo = "";
        break;
    }

    return strMultiQuo;
}

/* 设置多分类模式智能参数 */
void PageIntel::setMultiQuoValue(int index, double value)
{
    switch (index) {
    case 0:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoR[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoR[1] = abs(value);
        break;
    case 1:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoG[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoG[1] = abs(value);
        break;
    case 2:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoB[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoB[1] = abs(value);
        break;
    case 3:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRG[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRG[1] = abs(value);
        break;
    case 4:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRB[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRB[1] = abs(value);
        break;
    case 5:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGB[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGB[1] = abs(value);
        break;
    case 6:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRR[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoRR[1] = abs(value);
        break;
    case 7:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGG[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoGG[1] = abs(value);
        break;
    case 8:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoBB[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nQuoBB[1] = abs(value);
        break;
    case 9:
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nConsDfl[0] = value >= 0 ? 0 : 1;
        struCnfp.struGroupIdentify[struGsh.nLevel][currentChan].struIntel[0].nConsDfl[1] = abs(value);
        break;
    default:
        break;
    }
}

/* 刷新多分类模式页面 */
void PageIntel::updateMultiIntelPage()
{
    for (int i = 0; i < MAX_MULTI_INTEL; i++) {
        AIMultiQuoLbe[i]->setText(getMultiQuoText(i));
    }
}

/* 响应多分类模式参数设置 */
void PageIntel::getMultiIntelIndex(int index)
{
    double nMultiQuo;
    bool ok;

    myInputMethod inputDlg(myLan.input, AIMultiQuoLbe[index]->text());
    if (inputDlg.exec() == QDialog::Accepted) {
        nMultiQuo = inputDlg.getText().toDouble(&ok);
        qDebug("nMultiQuo[%d] = %f", index, nMultiQuo);
        if (ok) {
            setMultiQuoValue(index, nMultiQuo);
            AIMultiQuoLbe[index]->setText(QString("%1").arg(nMultiQuo));

            // 参数发送
            myFlow.materialCopyAssemble(struGsh.nLevel      , currentChan, 0, ARITH_INTEL_A, 0);
            myFlow.materialResetGroupAssemble(struGsh.nLevel, currentChan, 0, ARITH_INTEL_A, 0);
        }
    }
}
