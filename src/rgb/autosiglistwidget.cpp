#include "autosiglistwidget.h"

autoSigListWidget::autoSigListWidget(QWidget *parent) :
    QWidget(parent)
{
    m_bView = false;
    m_autoSigListSigMapper = new QSignalMapper(this);

    QGridLayout *tmpMainGridLayout = new QGridLayout(this);
    tmpMainGridLayout->setContentsMargins(10,10,10,10);
    tmpMainGridLayout->setHorizontalSpacing(50);
    QHBoxLayout *tmpAutoSigHBoxLayout[MAX_UNIT/2];
    QHBoxLayout *tmpAutoSigBackHBoxLayout = new QHBoxLayout;
    for (int i = 0; i < MAX_UNIT/2; i++) {
        tmpAutoSigHBoxLayout[i] = new QHBoxLayout;
        m_unitLabel[i]      = new myLabel(QString("%1").arg(i+1));
        m_autoSigRValLbe[i] = new myLineEdit;
        m_autoSigGValLbe[i] = new myLineEdit;
        m_autoSigBValLbe[i] = new myLineEdit;
        tmpAutoSigHBoxLayout[i]->addWidget(m_unitLabel[i]);
        tmpAutoSigHBoxLayout[i]->addStretch();
        tmpAutoSigHBoxLayout[i]->addWidget(m_autoSigRValLbe[i]);
        tmpAutoSigHBoxLayout[i]->addWidget(m_autoSigGValLbe[i]);
        tmpAutoSigHBoxLayout[i]->addWidget(m_autoSigBValLbe[i]);

        tmpMainGridLayout->addLayout(tmpAutoSigHBoxLayout[i], i/2, i%2);

        m_autoSigListSigMapper->setMapping(m_autoSigRValLbe[i],i*3);
        m_autoSigListSigMapper->setMapping(m_autoSigGValLbe[i],i*3+1);
        m_autoSigListSigMapper->setMapping(m_autoSigBValLbe[i],i*3+2);

        connect(m_autoSigRValLbe[i], SIGNAL(pressed()), m_autoSigListSigMapper, SLOT(map()));
        connect(m_autoSigGValLbe[i], SIGNAL(pressed()), m_autoSigListSigMapper, SLOT(map()));
        connect(m_autoSigBValLbe[i], SIGNAL(pressed()), m_autoSigListSigMapper, SLOT(map()));
    }

    m_viewChaneBtn         = new myPushButton(myLan.front_view, QIcon());
    m_autoSigInitEnableCbx = new myCustomCheckBox(myLan.init_autoSig+myLan.enable,true);
    m_autoSigListBackBtn   = new myPushButton(myLan.back,myIcon.Action_Back);

    m_viewChaneBtn->setMaximumSize(BTN_WIDTH,BTN_HEIGHT);
    m_autoSigInitEnableCbx->setMaximumHeight(BTN_HEIGHT);
    m_autoSigListBackBtn->setMaximumSize(BTN_WIDTH,BTN_HEIGHT);

    tmpAutoSigBackHBoxLayout->addWidget(m_viewChaneBtn);
    tmpAutoSigBackHBoxLayout->addStretch();
    tmpAutoSigBackHBoxLayout->addWidget(m_autoSigInitEnableCbx);
    tmpAutoSigBackHBoxLayout->addStretch();
    tmpAutoSigBackHBoxLayout->addWidget(m_autoSigListBackBtn);

    tmpMainGridLayout->addLayout(tmpAutoSigBackHBoxLayout,MAX_UNIT/2,0,1,2);

    updateAutoSigListWidget();

    connect(m_viewChaneBtn        , SIGNAL(pressed()) , this, SLOT(onViewChangePressedBtnSlt()));
    connect(m_autoSigInitEnableCbx,SIGNAL(pressed())  , this, SLOT(onSigInitEnbalePressedCbxSlt()));
    connect(m_autoSigListBackBtn  ,SIGNAL(pressed())  , this, SLOT(onAutoSigListBackPressedBtnSlt()));
    connect(m_autoSigListSigMapper,SIGNAL(mapped(int)), this, SLOT(autoSigListValueChange(int)));
}

/*!
 * \brief autoSigListWidget::updateAutoSigListWidget
 */
void autoSigListWidget::updateAutoSigListWidget()
{
    int tmpAddr;
    for (int i = 0; i < MAX_UNIT/2; i++) {
        if (i >= struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2) {
            m_unitLabel[i]->setHidden(true);
            m_autoSigRValLbe[i]->setHidden(true);
            m_autoSigGValLbe[i]->setHidden(true);
            m_autoSigBValLbe[i]->setHidden(true);
        }
        if (m_bView){
            tmpAddr = i*2+1;
        }
        else {
            tmpAddr = i*2;
        }
        m_autoSigRValLbe[i]->setText(QString("%1").arg(struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][0]));
        m_autoSigGValLbe[i]->setText(QString("%1").arg(struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][1]));
        m_autoSigBValLbe[i]->setText(QString("%1").arg(struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][2]));
    }

    if (m_bView){
        m_viewChaneBtn->setText(myLan.rear_view);
    }
    else {
        m_viewChaneBtn->setText(myLan.front_view);
    }

    if (struCnfp.nAutoSigInit) {
        m_autoSigInitEnableCbx->setChecked(true);
    }
    else {
        m_autoSigInitEnableCbx->setChecked(false);
    }
}

/*!
 * \brief autoSigListWidget::autoSigListValueChange
 * \param index
 */
void autoSigListWidget::autoSigListValueChange(int index)
{
    int ret;
    int tmpAddr;
    double tmpVal;

    if (m_bView) {
        tmpAddr = (index/3)*2+1;
    }
    else {
        tmpAddr = (index/3)*2;
    }
    switch(index%3){
    case 0:
        tmpVal =struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][0];
        break;
    case 1:
        tmpVal =struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][1];
        break;
    case 2:
        tmpVal =struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][2];
        break;
    default:
        tmpVal =struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][0];
        break;
    }

    myInputPanel inputDlg1(intType,1,255,tmpVal);
    ret  = inputDlg1.exec();
    if (ret == QDialog::Accepted) {
        tmpVal = inputDlg1.getValue();

        switch(index%3){
        case 0:
            struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][0] = tmpVal;
            break;
        case 1:
            struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][1] = tmpVal;
            break;
        case 2:
            struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][2] = tmpVal;
            break;
        default:
            struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][0] = tmpVal;
            break;
        }
        updateAutoSigListWidget();
    }
    qDebug("tmpView=%d,tmpAddr=%d,r=%d,g=%d,b=%d",m_bView,tmpAddr,struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][0],struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][1],struCnfp.nAutoSigVal[struGsh.nLevel][tmpAddr][2]);
}

/*!
 * \brief autoSigListWidget::onViewChangePressedBtnSlt
 */
void autoSigListWidget::onViewChangePressedBtnSlt()
{
    m_bView = !m_bView;
    updateAutoSigListWidget();
}

/*!
 * \brief autoSigListWidget::onSigInitEnbalePressedCbxSlt
 */
void autoSigListWidget::onSigInitEnbalePressedCbxSlt()
{
    if (struCnfp.nAutoSigInit == 1) {
        struCnfp.nAutoSigInit = 0;
    }
    else {
        struCnfp.nAutoSigInit = 1;
    }
    qDebug("enable:%d",struCnfp.nAutoSigInit);
    updateAutoSigListWidget();
}

/*!
 * \brief autoSigListWidget::onAutoSigListBackPressedBtnSlt
 */
void autoSigListWidget::onAutoSigListBackPressedBtnSlt()
{
    emit sBackToFactorySetPage();
}
