#include "setsorterconsistencewidget.h"

uchar   m_oriImgBuf[2][IMAGE_HEIGHT*IMAGE_WIDTH*3];
uchar   m_simImgBuf[IMAGE_HEIGHT*IMAGE_WIDTH*3];

setSorterConsistenceWidget::setSorterConsistenceWidget(QWidget *parent) :
    QWidget(parent)
{
    //! param
    memset(m_oriImgBuf[0],255,IMAGE_HEIGHT*IMAGE_WIDTH*3);
    memset(m_oriImgBuf[1],255,IMAGE_HEIGHT*IMAGE_WIDTH*3);
    memset(m_simImgBuf,255,IMAGE_HEIGHT*IMAGE_WIDTH*3);
    m_nCurView  = 0;
    m_nCurLevel = 0;
    m_nCurUnit  = 0;
    m_nCurAlg   = 0;
    m_nCurLayer   = 0;
    m_bFlgSim = false;
    m_bLightOrInfra = true;

    for (int i = 0; i< 6; i++){
        m_bEnableAlg[i] = false;
        m_struCurAlgTarget.nProbTarget[i] = 0;
        m_struCurAlgTarget.nCurProb[i] = 0;
        for (int j = 0; j < 10; j++){
           // qDebug("%d-%d",i,j);
            m_struCurAlgTarget.nAlgProb[i][j] = 0;
        }
        m_struCurAlgTarget.nBadPixelSum[i] = 0;
    }
    m_struCurAlgTarget.nPixelSum = 0;

    m_mainStackedWidget = new QStackedWidget(this);
    m_mainStackedWidget->setFixedSize(LCD_WIDTH, LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);

    m_pageMainWidget = new QWidget;
    createMainWidget();
    m_mainStackedWidget->addWidget(m_pageMainWidget);

    m_pageDetailWidget = new QWidget;
    createDetailWidget();
    m_mainStackedWidget->addWidget(m_pageDetailWidget);

    m_pageChuteWidget = new QWidget;
    createChuteWidget();
    m_mainStackedWidget->addWidget(m_pageChuteWidget);
}
/*!
 * \brief setSorterConsistenceWidget::createMainWidget
 */
void setSorterConsistenceWidget::createMainWidget()
{
    m_imgViewLbe = new myLabel;
    m_imgViewScrollArea = new QScrollArea;
    m_imgViewScrollArea->setStyleSheet("height: 30;"
                                       "width: 30;");
    m_imgViewScrollArea->setBackgroundRole(QPalette::Dark);
    m_imgViewScrollArea->setWidget(m_imgViewLbe);
    m_imgViewScrollArea->setWidgetResizable(true);

    m_chuteGbx = new myGroupBox;
    m_chuteGbx->setMaximumWidth(BTN_WIDTH);
    m_viewNumBtn = new myPushButton(myLan.first_front,QIcon());
    m_chuteNumBtn = new myPushButton("1",QIcon());
    m_captureImgBtn = new myPushButton(myLan.image_capture,QIcon());
    m_simImgBtn     = new myPushButton(myLan.ai_simulate,QIcon());
    m_viewNumBtn->setMaximumHeight(BTN_HEIGHT);
    m_chuteNumBtn->setMaximumHeight(BTN_HEIGHT);
    m_captureImgBtn->setMaximumHeight(BTN_HEIGHT);
    m_simImgBtn->setMaximumHeight(BTN_HEIGHT);

    QVBoxLayout *chuteVbLay = new QVBoxLayout(m_chuteGbx);
    chuteVbLay->addWidget(m_viewNumBtn);
    chuteVbLay->addWidget(m_chuteNumBtn);
    chuteVbLay->addWidget(m_captureImgBtn);
    chuteVbLay->addWidget(m_simImgBtn);

    m_simSensGbx = new myGroupBox;
    for (int i = 0; i < 6; i++){
        m_algEnableCbx[i] = new myCustomCheckBox(myLan.enable,m_bEnableAlg[i]);
        m_sensNameLbe[i]  = new myLabel(myLan.sensitivity);
        m_sensLbe[i]      = new myLabel(myLan.sensitivity);
        m_sensLne[i]      = new myLineEdit("0");
        m_detailBtn[i]    = new myPushButton("详细",QIcon());
        m_sensLne[i]->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
        m_detailBtn[i]->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    }
    m_simScrollArea = new QScrollArea;
    m_simScrollArea->setStyleSheet("height: 30");
    //m_simScrollArea->setBackgroundRole(QPalette::Dark);
    m_simScrollArea->setWidget(m_simSensGbx);
    m_simScrollArea->setWidgetResizable(true);

    QGridLayout *simSensGridLay = new QGridLayout(m_simSensGbx);

    for (int i = 0; i < 6; i++){
        simSensGridLay->addWidget(m_algEnableCbx[i],i,0);
        simSensGridLay->addWidget(m_sensNameLbe[i],i,1);
        simSensGridLay->addWidget(m_sensLbe[i],i,2,Qt::AlignRight);
        simSensGridLay->addWidget(m_sensLne[i],i,3,Qt::AlignLeft);
        simSensGridLay->addWidget(m_detailBtn[i],i,4);
    }

    m_settingGbx = new myGroupBox;
    m_settingGbx->setMaximumWidth(BTN_WIDTH);
    m_levelChgBtn = new myPushButton(myLan.color,QIcon());
    m_consistenceBtn = new myPushButton("组间优化",QIcon());
    m_simBackBtn     = new myPushButton(myLan.back,myIcon.Action_Back);
    m_levelChgBtn->setMaximumHeight(BTN_HEIGHT);
    m_consistenceBtn->setMaximumHeight(BTN_HEIGHT);
    m_simBackBtn->setMaximumHeight(BTN_HEIGHT);

    QVBoxLayout *settingVbLay = new QVBoxLayout(m_settingGbx);
    settingVbLay->addWidget(m_levelChgBtn);
    settingVbLay->addStretch();
    settingVbLay->addWidget(m_consistenceBtn);
    settingVbLay->addStretch();
    settingVbLay->addWidget(m_simBackBtn);

    QGridLayout *mainGridLay = new QGridLayout(m_pageMainWidget);
    mainGridLay->addWidget(m_imgViewScrollArea,0,0);
    mainGridLay->addWidget(m_chuteGbx,0,1);
    mainGridLay->addWidget(m_simScrollArea,1,0);
    mainGridLay->addWidget(m_settingGbx,1,1);

    connect(m_viewNumBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    connect(m_chuteNumBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    connect(m_captureImgBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    connect(m_simImgBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    connect(m_levelChgBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    connect(m_consistenceBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    connect(m_simBackBtn,SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));

    for (int i = 0; i < 6;i++){
        connect(m_algEnableCbx[i],SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
        connect(m_sensLne[i],SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
        connect(m_detailBtn[i],SIGNAL(pressed()),this,SLOT(commonProcessMainWidgetSlt()));
    }
}
/*!
 * \brief setSorterConsistenceWidget::updateMainWidget
 */
void setSorterConsistenceWidget::updateMainWidget()
{
    updateSimImg();
    updateChuteGbx();
    updateProductAlg();
}
/*!
 * \brief setSorterConsistenceWidget::updateInitImgSlt
 */
void setSorterConsistenceWidget::updateInitImgSlt()
{
    QImage image(QString("/tmp/%1.png").arg(0));

    if (image.isNull()) {
        updateImage();
        return;
    }

    int oriIndex;
    QColor rgb;

    for(int i=0; i<IMAGE_HEIGHT; i++){
        for(int j=0; j<IMAGE_WIDTH; j++){
            rgb = image.pixel(j,i);
            oriIndex = (i*IMAGE_WIDTH+j)*3;

            m_oriImgBuf[0][oriIndex] = (uchar)rgb.red();
            m_oriImgBuf[0][oriIndex+1] = (uchar)rgb.green();
            m_oriImgBuf[0][oriIndex+2] = (uchar)rgb.blue();
            m_oriImgBuf[1][oriIndex] = (uchar)rgb.red();
            m_oriImgBuf[1][oriIndex+1] = (uchar)rgb.green();
            m_oriImgBuf[1][oriIndex+2] = (uchar)rgb.blue();
        }
    }
    memcpy(m_simImgBuf, m_oriImgBuf[m_nCurLayer], IMAGE_WIDTH*IMAGE_HEIGHT*3);
    updateImage();
}
/*!
 * \brief setSorterConsistenceWidget::createDetailWidget
 */
void setSorterConsistenceWidget::createDetailWidget()
{
    m_cameraTitleLbe = new myLabel(myLan.chute);
    m_detailTitleLbe = new myLabel(myLan.sensitivity);
    m_biasTitleLbe   = new myLabel("偏置");
    m_percentTitleLbe = new myLabel("百分比");

    for (int i = 0; i < 10; i++){
        m_cameraLbe[i] = new myLabel(QString("%1(%2)").arg(myLan.front_view).arg(i));
        m_sensDetailLbe[i] = new myLabel("25%");
        m_sensBiasLne[i]   = new myLineEdit("0");
        m_percentLbe[i]    = new myLabel("0");
        m_sensBiasLne[i]->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    }

    m_detailResetBtn  = new myPushButton(tr("重置"),QIcon());
    m_detailResetBtn->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    m_detailBackBtn   = new myPushButton(myLan.back,myIcon.Action_Back);
   // m_detailBackBtn->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    m_detailBackBtn->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));

    QHBoxLayout *downHbLay  = new QHBoxLayout;
    downHbLay->addWidget(m_detailResetBtn);
    downHbLay->addStretch();
    downHbLay->addWidget(m_detailBackBtn);

    QGridLayout *mainGridLay = new QGridLayout(m_pageDetailWidget);
    mainGridLay->addWidget(m_cameraTitleLbe,0,0);
    mainGridLay->addWidget(m_detailTitleLbe,0,1);
    mainGridLay->addWidget(m_biasTitleLbe,0,2);
    mainGridLay->addWidget(m_percentTitleLbe,0,3);

    for (int i = 0; i < 10;i++){
        mainGridLay->addWidget(m_cameraLbe[i],i+1,0);
        mainGridLay->addWidget(m_sensDetailLbe[i],i+1,1);
        mainGridLay->addWidget(m_sensBiasLne[i],i+1,2);
        mainGridLay->addWidget(m_percentLbe[i],i+1,3);
    }
    //mainGridLay->addWidget(m_detailBackBtn,11,3);
    mainGridLay->addLayout(downHbLay,11,0,1,4);

    //! 槽函数
    for (int i = 0; i < 10; i++){
        connect(m_sensBiasLne[i],SIGNAL(pressed()),this,SLOT(commonProcessDetailWidgetSlt()));
    }
    connect(m_detailResetBtn,SIGNAL(pressed()),this,SLOT(commonProcessDetailWidgetSlt()));
    connect(m_detailBackBtn,SIGNAL(pressed()),this,SLOT(commonProcessDetailWidgetSlt()));
}
/*!
 * \brief setSorterConsistenceWidget::updateDetailWidget
 */
void setSorterConsistenceWidget::updateDetailWidget()
{
    int nUnit,nSens;
    for (int i = 0; i < 10; i++){
        m_cameraLbe[i]->hide();
        m_sensDetailLbe[i]->hide();
        m_sensBiasLne[i]->hide();
        m_percentLbe[i]->hide();
    }

    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2; i++){
        m_cameraLbe[i]->show();
        m_sensDetailLbe[i]->show();
        m_sensBiasLne[i]->show();
        m_percentLbe[i]->show();

        if (m_nCurView%2){
            m_cameraLbe[i]->setText(QString("%1(%2)").arg(myLan.rear_view).arg(i+1));
        }
        else {
            m_cameraLbe[i]->setText(QString("%1(%2)").arg(myLan.front_view).arg(i+1));
        }

        //! 灵敏度
        nUnit = (m_nCurView%2)?i*2+1:i*2;

        //! 灰度
        if (m_bLightOrInfra){
            if (m_nCurAlg == ARITH_GREY_A || m_nCurAlg == ARITH_GREY_B){
                if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[m_nCurAlg].nMode){
                    nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[m_nCurAlg].nSensMax+struCnfp.nSensBias[nUnit][m_nCurAlg];
                    nSens = (nSens > 0)?nSens:0;
                    m_sensDetailLbe[i]->setText(QString("%1%").arg(nSens*0.1));
                }
                else {
                    nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[m_nCurAlg].nSensMin+struCnfp.nSensBias[nUnit][m_nCurAlg];
                    nSens = (nSens > 0)?nSens:0;
                    m_sensDetailLbe[i]->setText(QString("%1%").arg(nSens*0.1));
                }
                //! 偏置
                m_sensBiasLne[i]->setText(QString("%1%").arg(struCnfp.nSensBias[nUnit][m_nCurAlg]*0.1));
            }

            //! 色差
            if (m_nCurAlg == ARITH_DISCOLOR_A || m_nCurAlg ==  ARITH_DISCOLOR_B){
                if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[m_nCurAlg].nMode){
                    nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[m_nCurAlg].nSensMax+struCnfp.nSensBias[nUnit][m_nCurAlg];
                    nSens = (nSens > 0)?nSens:0;
                    m_sensDetailLbe[i]->setText(QString("%1%").arg(nSens*0.01));
                }
                else {
                    nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[m_nCurAlg].nSensMin+struCnfp.nSensBias[nUnit][m_nCurAlg];
                    nSens = (nSens > 0)?nSens:0;
                    m_sensDetailLbe[i]->setText(QString("%1%").arg(nSens*0.01));
                }
                //! 偏置
                m_sensBiasLne[i]->setText(QString("%1%").arg(struCnfp.nSensBias[nUnit][m_nCurAlg]*0.01));
            }
        }

        //! 智能
        if ((m_nCurAlg+11) == ARITH_INTEL_A || (m_nCurAlg+11) == ARITH_INTEL_B){
            nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[m_nCurAlg-4].nSens+struCnfp.nSensBias[nUnit][m_nCurAlg+11];
            nSens = (nSens > 0)?nSens:0;
            m_sensDetailLbe[i]->setText(QString("%1").arg(nSens));
            //! 偏置
            m_sensBiasLne[i]->setText(QString("%1").arg(struCnfp.nSensBias[nUnit][m_nCurAlg+11]));
        }

        //! 百分比
        if (m_nCurAlg == ARITH_GREY_A || m_nCurAlg == ARITH_GREY_B || m_nCurAlg == ARITH_DISCOLOR_A || m_nCurAlg ==  ARITH_DISCOLOR_B){
            m_percentLbe[i]->setText(QString("%1%").arg(m_struCurAlgTarget.nAlgProb[m_nCurAlg][i]/100.0));
        }

        if ((m_nCurAlg+11) == ARITH_INTEL_A || (m_nCurAlg+11) == ARITH_INTEL_B){
            m_percentLbe[i]->setText(QString("%1%").arg(m_struCurAlgTarget.nAlgProb[m_nCurAlg][i]/100.0));

        }
    }
}
/*!
 * \brief setSorterConsistenceWidget::createChuteWidget
 */
void setSorterConsistenceWidget::createChuteWidget()
{
    //! 组设置
    m_viewSettingGbx = new myGroupBox;
    m_viewSettingGbx->setMaximumWidth(BTN_WIDTH*2);

    for (int i = 0; i < 6; i++){
        m_viewSettingCbx[i] = new myCustomCheckBox("",false,CB_STYLE_CIRCLE);
    }

    QVBoxLayout *viewSettingVbLay = new QVBoxLayout(m_viewSettingGbx);

    for (int i = 0; i < 6; i++){
        viewSettingVbLay->addWidget(m_viewSettingCbx[i]);
    }

    //! 相机设置
    m_chuteSettingGbx = new myGroupBox;

    for (int i = 0; i < 10; i++){
        m_chuteSettingBtn[i] = new myPushButton(QString("%1(%2)").arg(myLan.chute).arg(i+1),QIcon());
        m_chuteSettingBtn[i]->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));
    }

    QGridLayout *chuteSettingGridLay = new QGridLayout(m_chuteSettingGbx);

    for (int i = 0; i< 10; i++){
        chuteSettingGridLay->addWidget(m_chuteSettingBtn[i],i/3,i%3);
    }

    QHBoxLayout  *upHbLay = new QHBoxLayout;
    upHbLay->addWidget(m_viewSettingGbx);
    upHbLay->addWidget(m_chuteSettingGbx);

    //! 返回
    m_chuteBackBtn = new myPushButton(myLan.back,myIcon.Action_Back);
    m_chuteBackBtn->setMaximumSize(QSize(BTN_WIDTH,BTN_HEIGHT));

    QHBoxLayout *downHbLay = new QHBoxLayout;
    downHbLay->addStretch();
    downHbLay->addWidget(m_chuteBackBtn);

    //! 页面布局
    QVBoxLayout *mainVbLay = new QVBoxLayout(m_pageChuteWidget);
    mainVbLay->addLayout(upHbLay);
    mainVbLay->addLayout(downHbLay);

    //! 槽函数
    for (int i = 0; i< 6;i++){
        connect(m_viewSettingCbx[i],SIGNAL(pressed()),this,SLOT(commonProcessChuteWidgetSlt()));
    }

    for (int i = 0; i< 10; i++){
        connect(m_chuteSettingBtn[i],SIGNAL(pressed()),this,SLOT(commonProcessChuteWidgetSlt()));
    }

    connect(m_chuteBackBtn,SIGNAL(pressed()),this,SLOT(commonProcessChuteWidgetSlt()));
}
/*!
 * \brief setSorterConsistenceWidget::updateChuteWidget
 */
void setSorterConsistenceWidget::updateChuteWidget()
{
    m_viewSettingCbx[0]->setCheckBoxName(myLan.first_front);
    m_viewSettingCbx[1]->setCheckBoxName(myLan.first_rear);
    m_viewSettingCbx[2]->setCheckBoxName(myLan.second_front);
    m_viewSettingCbx[3]->setCheckBoxName(myLan.second_rear);
    m_viewSettingCbx[4]->setCheckBoxName(myLan.third_front);
    m_viewSettingCbx[5]->setCheckBoxName(myLan.third_rear);

    for (int i = 0; i < 6; i++){
        m_viewSettingCbx[i]->hide();
    }

    for (int i = 0; i < struCnfg.struLevelInfo[struGsh.nLevel].nIdentifyGroupTotal; i++){
        m_viewSettingCbx[i]->setChecked(false);
        m_viewSettingCbx[i]->show();
    }

    m_viewSettingCbx[m_nCurView]->setChecked(true);

    for (int i = 0; i < 10; i++){
        m_chuteSettingBtn[i]->hide();
    }

    for (int i = 0; i< struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal/2; i++){
        m_chuteSettingBtn[i]->setRedColor(DEF);
        m_chuteSettingBtn[i]->show();
    }
    m_chuteSettingBtn[m_nCurUnit]->setRedColor(GREEN);
}
/*!
 * \brief setSorterConsistenceWidget::getUnitGroup
 * \param level
 * \param unit
 * \return
 */
int setSorterConsistenceWidget::getUnitGroup(int level, int unit)
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
 * \brief setSorterConsistenceWidget::updateChuteGbx
 */
void setSorterConsistenceWidget::updateChuteGbx()
{
    m_nCurView = getUnitGroup(struGsh.nLevel,struGsh.nUnit);
    m_nCurUnit  = struGsh.nUnit/2;
	m_nCurLayer = struGsh.nUnit%2;
    switch(m_nCurView){
    case 0:
        m_viewNumBtn->setText(myLan.first_front);
        break;
    case 1:
        m_viewNumBtn->setText(myLan.first_rear);
        break;
    case 2:
        m_viewNumBtn->setText(myLan.second_front);
        break;
    case 3:
        m_viewNumBtn->setText(myLan.second_rear);
        break;
    case 4:
        m_viewNumBtn->setText(myLan.third_front);
        break;
    case 5:
        m_viewNumBtn->setText(myLan.third_rear);
        break;
    default:
        m_viewNumBtn->setText(myLan.first_front);
        break;
    }
    m_chuteNumBtn->setText(QString("%1(%2)").arg(myLan.chute).arg(m_nCurUnit+1));

    if (m_bFlgSim){
        m_simImgBtn->setRedColor(GREEN);
    }
    else {
        m_simImgBtn->setRedColor(DEF);
    }

    m_levelChgBtn->hide();
}
/*!
 * \brief setSorterConsistenceWidget::updateImage
 */
void setSorterConsistenceWidget::updateImage()
{
    QImage img(m_simImgBuf, CAP_WID, CAP_HEI, QImage::Format_RGB888);
    m_imgViewLbe->setPixmap(QPixmap::fromImage(img));
}
/*!
 * \brief setSorterConsistenceWidget::updateProductAlg
 */
void setSorterConsistenceWidget::updateProductAlg()
{
    updateAlgStat(ARITH_GREY_A,struGsh.nLevel,m_nCurView,true);
    updateAlgStat(ARITH_GREY_B,struGsh.nLevel,m_nCurView,true);
    updateAlgStat(ARITH_DISCOLOR_A,struGsh.nLevel,m_nCurView,true);
    updateAlgStat(ARITH_DISCOLOR_B,struGsh.nLevel,m_nCurView,true);
    updateAlgStat(ARITH_INTEL_A,struGsh.nLevel,m_nCurView,true);
    updateAlgStat(ARITH_INTEL_B,struGsh.nLevel,m_nCurView,true);
}
/*!
 * \brief setSorterConsistenceWidget::updateAlgStat
 * \param nAlgId
 */
void setSorterConsistenceWidget::updateAlgStat(int nAlgId, int nLevel, int nGroup, bool bFlgShow)
{
    //! 灰度
    if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B){
        if (struCnfp.nArithmeticEnable[nAlgId] == 1 && bFlgShow){
            m_sensNameLbe[nAlgId]->setText(QString().fromLocal8Bit(struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].sName));
            m_algEnableCbx[nAlgId]->show();
            m_sensNameLbe[nAlgId]->show();
            m_sensLbe[nAlgId]->show();
            m_sensLne[nAlgId]->show();
            m_detailBtn[nAlgId]->show();
            if (struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].nMode){
                m_sensLne[nAlgId]->setText(QString("%1%").arg(struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].nSensMax*0.1));
            }
            else {
                m_sensLne[nAlgId]->setText(QString("%1%").arg(struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].nSensMin*0.1));
            }
        }
        else {
            m_algEnableCbx[nAlgId]->hide();
            m_sensNameLbe[nAlgId]->hide();
            m_sensLbe[nAlgId]->hide();
            m_sensLne[nAlgId]->hide();
            m_detailBtn[nAlgId]->hide();
        }
        return;
    }
    //! 色差
    if (nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
        if (struCnfp.nArithmeticEnable[nAlgId] == 1 && bFlgShow){
            m_sensNameLbe[nAlgId]->setText(QString().fromLocal8Bit(struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].sName));
            m_algEnableCbx[nAlgId]->show();
            m_sensNameLbe[nAlgId]->show();
            m_sensLbe[nAlgId]->show();
            m_sensLne[nAlgId]->show();
            m_detailBtn[nAlgId]->show();
            if (struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].nMode){
                m_sensLne[nAlgId]->setText(QString("%1%").arg(struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].nSensMax*0.01));
            }
            else {
                m_sensLne[nAlgId]->setText(QString("%1%").arg(struCnfp.struGroupIdentify[nLevel][nGroup].struGreyColor[nAlgId].nSensMin*0.01));
            }
        }
        else {
            m_algEnableCbx[nAlgId]->hide();
            m_sensNameLbe[nAlgId]->hide();
            m_sensLbe[nAlgId]->hide();
            m_sensLne[nAlgId]->hide();
            m_detailBtn[nAlgId]->hide();
        }
        return;
    }
    //! 智能
    if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
        if (struCnfp.nArithmeticEnable[nAlgId] == 1 && bFlgShow){
            m_sensNameLbe[nAlgId-11]->setText(QString().fromLocal8Bit(struCnfp.struGroupIdentify[nLevel][nGroup].struIntel[nAlgId-15].sName));
            m_algEnableCbx[nAlgId-11]->show();
            m_sensNameLbe[nAlgId-11]->show();
            m_sensLbe[nAlgId-11]->show();
            m_sensLne[nAlgId-11]->show();
            m_detailBtn[nAlgId-11]->show();
            m_sensLne[nAlgId-11]->setText(QString("%1").arg(struCnfp.struGroupIdentify[nLevel][nGroup].struIntel[nAlgId-15].nSens));
        }
        else {
            m_algEnableCbx[nAlgId-11]->hide();
            m_sensNameLbe[nAlgId-11]->hide();
            m_sensLbe[nAlgId-11]->hide();
            m_sensLne[nAlgId-11]->hide();
            m_detailBtn[nAlgId-11]->hide();
        }
        return;
    }
}
/*!
 * \brief setSorterConsistenceWidget::updateInfraAlgStat
 * \param nAlgId
 * \param nLevel
 * \param nGroup
 * \param bFlgShow
 */
void setSorterConsistenceWidget::updateInfraAlgStat(int nAlgId, int nLevel, int nGroup, bool bFlgShow)
{
    (void)nAlgId;
    (void)nLevel;
    (void)nGroup;
    (void)bFlgShow;
}
/*!
 * \brief setSorterConsistenceWidget::setAlgSens
 * \param nAlgId
 */
void setSorterConsistenceWidget::setAlgSens(int nAlgId)
{
    int ret;
    inputType type;
    int nMin, nMax;
    double nSens;

    //! 灵敏度
    switch(nAlgId) {
    case ARITH_GREY_A://! 灰度AB
    case ARITH_GREY_B:
        nMin = 0;
        nMax = 100;
        type = floatType;

        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {//选亮
            nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax*0.1;
        }
        else {
            nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin*0.1;
        }
        break;
    case ARITH_DISCOLOR_A://! 色差AB
    case ARITH_DISCOLOR_B:
        nMin = 0;
        nMax = 100;
        type = floatType;

        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {//选亮
            nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax*0.01;
        }
        else {
            nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin*0.01;
        }
        break;
    case ARITH_INTEL_A:
    case ARITH_INTEL_B:
        nMin = 0;
        nMax = 200;
        type = intType;

        nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[nAlgId-4].nSens*1.0;
        break;
    }

    myInputPanel inputDlg(type,nMin,nMax,nSens);
    ret  = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        nSens = inputDlg.getValue();

        switch(nAlgId) {
        case ARITH_GREY_A:
        case ARITH_GREY_B://! 灰度AB
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {//! 选亮
                struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax= nSens*10.0+0.5;
            }
            else {
                struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin = nSens*10.0+0.5;
            }
            break;
        case ARITH_DISCOLOR_A://! 色差AB
        case ARITH_DISCOLOR_B:
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {//! 选亮
                struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax= nSens*100.0+0.5;
            }
            else {
                struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin = nSens*100.0+0.5;
            }
            break;
        case ARITH_INTEL_A:
        case ARITH_INTEL_B:
            struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[nAlgId-4].nSens = nSens;
            break;
        default:
            break;
        }

        //! 参数发送
        myFlow.materialCopyAssemble(struGsh.nLevel      , m_nCurView, 0, nAlgId, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, m_nCurView, 0, nAlgId, 0);
    }
}
/*!
 * \brief setSorterConsistenceWidget::setAlgSensInfra
 * \param nAlgId
 */
void setSorterConsistenceWidget::setAlgSensInfra(int nAlgId)
{
    (void)nAlgId;
}
/*!
 * \brief setSorterConsistenceWidget::setAlgSensBias
 * \param nAlgId
 * \param nUnit
 */
void setSorterConsistenceWidget::setAlgSensBias(int nUnit)
{
    int ret;
    inputType type;
    int nMin, nMax;
    double nSens;

    //qDebug()<<m_nCurAlg;
    //! 灵敏度偏置
    switch(m_nCurAlg) {
    case ARITH_GREY_A://! 灰度AB
    case ARITH_GREY_B:
        nMin = -30;
        nMax = 30;
        type = floatType;

        nSens = struCnfp.nSensBias[nUnit][m_nCurAlg]*0.1;
        break;
    case ARITH_DISCOLOR_A://! 色差AB
    case ARITH_DISCOLOR_B:
        nMin = -30;
        nMax = 30;
        type = floatType;

        nSens = struCnfp.nSensBias[nUnit][m_nCurAlg]*0.01;
        break;
    case ARITH_INTEL_A:
    case ARITH_INTEL_B:
        nMin = -100;
        nMax = 100;
        type = intType;

        nSens = struCnfp.nSensBias[nUnit][m_nCurAlg+11];
        break;
    }


    myInputPanel inputDlg(type,nMin,nMax,nSens);
    inputDlg.setInputNegEn(true);
    ret  = inputDlg.exec();
    if (ret == QDialog::Accepted) {
        nSens = inputDlg.getValue();

        switch(m_nCurAlg) {
        case ARITH_GREY_A:
        case ARITH_GREY_B://! 灰度AB
            struCnfp.nSensBias[nUnit][m_nCurAlg] = nSens*10;
            break;
        case ARITH_DISCOLOR_A://! 色差AB
        case ARITH_DISCOLOR_B:
            struCnfp.nSensBias[nUnit][m_nCurAlg] = nSens*100;
            break;
        case ARITH_INTEL_A:
        case ARITH_INTEL_B:
            struCnfp.nSensBias[nUnit][m_nCurAlg+11] = nSens;
            break;
        default:
            break;
        }

        //! 参数发送
        myFlow.materialCopyAssemble(struGsh.nLevel      , m_nCurView, 0, m_nCurAlg, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, m_nCurView, 0, m_nCurAlg, 0);
    }
}
/*!
 * \brief setSorterConsistenceWidget::setAlgSensBiasInfra
 * \param nUnit
 */
void setSorterConsistenceWidget::setAlgSensBiasInfra(int nUnit)
{
    (void)nUnit;
}
/*!
 * \brief setSorterConsistenceWidget::getImage
 * \return
 */
bool setSorterConsistenceWidget::getImage(int nUnit)
{
    bool retCode = true;
#ifdef Q_OS_UNIX
    int usbReturn;
    int bufSize = 6160*IMAGE_HEIGHT+2240;
    uchar tmpColorBuf[IMAGE_WIDTH*3*IMAGE_HEIGHT];
    uchar tmpColorBuf1[IMAGE_WIDTH*3*IMAGE_HEIGHT];
    uchar tmpBuf[bufSize];
    char *pImageHead;
    uchar imageHead[5]={0xAA,0xAA,0xAA,0xAA,0};
    bool bIsLeft = true;
    int nBegin = 0;
    int nEnd = 2047;
    int firstRowNum = 0;
    int tmpFrontUnit, tmpRearUnit;

    tmpFrontUnit = (struCnfg.nAutoReferChute-1)*2;
    tmpRearUnit  = nUnit;

    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        return false;
    }

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, tmpRearUnit, 0, 0,(IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1, 3);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, tmpRearUnit, 0 ,0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    MySerial.com1Write(CMD_UNIT_CAPTURE_RANGE, UNIT, struGsh.nLevel, tmpRearUnit, 0 ,nBegin/256, nBegin%256, nEnd/256, nEnd%256, 1);
    MySerial.com1Write(2, 2, 0, 0x3f,  0, 0, 0, 0, 0, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 1, (IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1);

    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 0, 0, 0, 1); // 主相机板传输图像
    bzero(tmpBuf, bufSize);
    usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, (char *)tmpBuf, bufSize, 20*1000);
    pImageHead = (char *)tmpBuf;
    qDebug("usbReturn = %d", usbReturn);
    if(usbReturn == bufSize){
        for(int i=0; i<IMAGE_HEIGHT; i++){
            //! 判断包头数据是否正确
            if((pImageHead=getHead((char *)pImageHead, 6160*3, (char *)imageHead)) != NULL) {
                if(i == 0) {
                    firstRowNum = 0;

                    //! 在第一帧有效数据中判断物料在整幅图片的左侧还是右侧
                    for(int j=0; j<1024; j++)
                    {
                        uchar r = *(pImageHead+firstRowNum*6160+j*3);
                        uchar g = *(pImageHead+firstRowNum*6160+j*3+1);
                        uchar b = *(pImageHead+firstRowNum*6160+j*3+2);

                        if(false == myAIShare.pixelIsBackgroundcomm(r, g, b, j)){
                            bIsLeft = true;
                            break;
                        }
                    }
                }
                if(bIsLeft)
                    memcpy(m_oriImgBuf[m_nCurLayer]+IMAGE_WIDTH*3*(IMAGE_HEIGHT-1-i), pImageHead, IMAGE_WIDTH*3);
                else
                    memcpy(m_oriImgBuf[m_nCurLayer]+IMAGE_WIDTH*3*(IMAGE_HEIGHT-1-i), pImageHead+IMAGE_WIDTH*3, IMAGE_WIDTH*3);
            } else {
                retCode = false;
                return retCode;
            }
        }
        MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    }
    else {
        retCode = false;
        return retCode;
    }
#endif
    return retCode;
}
/*!
 * \brief setSorterConsistenceWidget::getHist
 * \param nCorType
 * \param nUnit
 * \return
 */
bool setSorterConsistenceWidget::getHist(int nUnit)
{
    //! 获取信号直方图数据
//    struGsh.struStatisticInfo.histogramStatistic.clear();
//    struGsh.struStatisticInfo.histogramStatistic.resize(MAX_STATISTIC_HISTOGRAM+16);
//    MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_OBTAIN, UNIT, 0, nUnit, 0, 0, 0, 0, 0, 3);
//    MySerial.com3Read(struGsh.struStatisticInfo.histogramStatistic.data(), struGsh.struStatisticInfo.histogramStatistic.size(), 1000);
//    QString head1 = struGsh.struStatisticInfo.histogramStatistic.left(8);
//    if (!head1.compare("AAAAAAAA")){
//        return false;
//    }

//    struGsh.struStatisticInfo.histogramStatistic.remove(0, 10);
//    struGsh.struStatisticInfo.histogramStatistic.truncate(struGsh.struStatisticInfo.histogramStatistic.size()-6);

//    memset(m_hist[0],0,256);
//    memset(m_hist[1],0,256);
//    memset(m_hist[2],0,256);

//    for (int k = 0; k < 256; k++){
//        m_hist[0][k] =((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4))*256*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4+1))*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4+2))*256+(uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4+3);
//        m_hist[1][k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4))*256*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4+1))*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4+2))*256+(uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4+3);
//        m_hist[2][k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4))*256*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4+1))*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4+2))*256+(uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4+3);
//        //                qDebug()<<m_hist[0][k];
//        //                qDebug()<<m_hist[1][k];
//        //                qDebug()<<m_hist[2][k];
//    }

    struGsh.struStatisticInfo.histogramStatistic.clear();
    struGsh.struStatisticInfo.histogramStatistic.resize(MAX_STATISTIC_HISTOGRAM+16);
    MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_OBTAIN, UNIT, 0, nUnit, 0, 0, 0, 0, 0, 3);
    MySerial.com3Read(struGsh.struStatisticInfo.histogramStatistic.data(), struGsh.struStatisticInfo.histogramStatistic.size(), 1000);
    QString head2 = struGsh.struStatisticInfo.histogramStatistic.left(8);
//    if (!head2.compare("AAAAAAAA")){
//        return false;
//    }

    struGsh.struStatisticInfo.histogramStatistic.remove(0, 10);
    struGsh.struStatisticInfo.histogramStatistic.truncate(struGsh.struStatisticInfo.histogramStatistic.size()-6);

    m_struCurAlgTarget.nPixelSum = 0;
    memset(m_hist[3],0,256);
    memset(m_hist[4],0,256);
    memset(m_hist[5],0,256);

    for (int k = 0; k < 256; k++){
        m_hist[3][k] =((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4))*256*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4+1))*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4+2))*256+(uchar)struGsh.struStatisticInfo.histogramStatistic.at(k*4+3);
        m_hist[4][k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4))*256*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4+1))*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4+2))*256+(uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024+k*4+3);
        m_hist[5][k] = ((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4))*256*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4+1))*256*256+((uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4+2))*256+(uchar)struGsh.struStatisticInfo.histogramStatistic.at(1024*2+k*4+3);
        m_struCurAlgTarget.nPixelSum += m_hist[3][k];
        //                qDebug()<<m_hist[3][k];
        //                qDebug()<<m_hist[4][k];
        //                qDebug()<<m_hist[5][k];
    }

    QFile fileHist;
    QString fileName = QString("/tmp/1");
    fileHist.setFileName(fileName);
    if (!fileHist.open(QIODevice::WriteOnly | QIODevice::Text)){
        return false;
    }
    QTextStream out(&fileHist);
    for(int i = 0; i < 256; i++){
        QString outStr = QString("%1\t%2\t%3\n").arg(m_hist[3][i]).arg(m_hist[4][i]).arg(m_hist[5][i]);
        out<<outStr;
    }
    fileHist.close();

    return true;
}
/*!
 * \brief setSorterConsistenceWidget::initUSB
 * \return
 */
bool setSorterConsistenceWidget::initUSB()
{
#ifdef Q_OS_UNIX
    if(myUsbDev != NULL){
        usb.myusbFree();
    }
    usb.myusbInit();
    if(myUsbDev == NULL){
        return false;
    }
#endif
    return true;
}
/*!
 * \brief setSorterConsistenceWidget::getMaterData
 * \param nUnit
 * \return
 */
bool setSorterConsistenceWidget::getMaterData(int nUnit)
{
    bool retCode = true;
    int usbReturn;
    int bufSize = 6160*IMAGE_HEIGHT+2240;
    uchar tmpBuf[bufSize];
    char *pImageHead;
    uchar imageHead[5]={0xAA,0xAA,0xAA,0xAA,0};
#ifdef Q_OS_UNIX
    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, nUnit, 0, 0, 0, 0, 0, 1); // 主相机板传输图像
    bzero(tmpBuf, bufSize);
    usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, (char *)tmpBuf, bufSize, 5*1000);
    pImageHead = (char *)tmpBuf;
    qDebug("usbReturn = %d\n", usbReturn);
    if(usbReturn == bufSize){
    	m_struCurAlgTarget.nPixelSum = 0;
        for(int i=0; i<IMAGE_HEIGHT; i++){
            //! 判断包头数据是否正确
            if((pImageHead=getHead((char *)pImageHead, 6160*3, (char *)imageHead)) != NULL) {
					//	printf("%d\n",i);
					for(int j=0; j<IMAGE_WIDTH*2; j++) {
                        uchar r = *(pImageHead+j*3);
                        uchar g = *(pImageHead+j*3+1);
                        uchar b = *(pImageHead+j*3+2);

						if (r != 0 && g != 0 && b != 0) {
            				m_simImgBuf[m_struCurAlgTarget.nPixelSum*3]   = r;
            				m_simImgBuf[m_struCurAlgTarget.nPixelSum*3+1] = g;
            				m_simImgBuf[m_struCurAlgTarget.nPixelSum*3+2] = b;

							if (m_struCurAlgTarget.nPixelSum < IMAGE_HEIGHT*IMAGE_WIDTH) {
            					m_struCurAlgTarget.nPixelSum++;
							}
						}
                    }
            } else {
    			m_struCurAlgTarget.nPixelSum = 0;
                retCode = false;
                return retCode;
            }
        }
        MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    }
    else {
    	m_struCurAlgTarget.nPixelSum = 0;
        retCode = false;
        return retCode;
    }
    qDebug("sum = %d\n", m_struCurAlgTarget.nPixelSum);
#endif
    return retCode;
}
/*!
 * \brief setSorterConsistenceWidget::getHead
 * \param mem
 * \param size
 * \param str
 * \return
 */
char *setSorterConsistenceWidget::getHead(const char *mem, int size, const char *str)
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
/*!
 * \brief setSorterConsistenceWidget::updateSimImg
 */
void setSorterConsistenceWidget::updateSimImg()
{
//    qDebug()<<struGsh.nUnit;
//    qDebug()<<m_nCurView;
//    qDebug()<<m_bFlgSim;
    bool bRes = false;

    memcpy(m_simImgBuf, m_oriImgBuf[m_nCurLayer], IMAGE_HEIGHT*IMAGE_WIDTH*3);

    int i=0;

    if (m_bFlgSim) {
        for(    ; i<CAP_HEI; i++){
            for(int j=0; j<CAP_WID; j++){
                uchar r = m_oriImgBuf[m_nCurLayer][(i*IMAGE_WIDTH+j)*3];
                uchar g = m_oriImgBuf[m_nCurLayer][(i*IMAGE_WIDTH+j)*3+1];
                uchar b = m_oriImgBuf[m_nCurLayer][(i*IMAGE_WIDTH+j)*3+2];

                if(myAIShare.pixelIsBackgroundcomm(r, g, b, j)){
                    continue;
                }

                bRes = getAlgSim(ARITH_GREY_A,i,j) ||
                       getAlgSim(ARITH_GREY_B,i,j) ||
                       getAlgSim(ARITH_DISCOLOR_A,i,j) ||
                       getAlgSim(ARITH_DISCOLOR_B,i,j) ||
                       getAlgSim(ARITH_INTEL_A,i,j) ||
                       getAlgSim(ARITH_INTEL_B,i,j);

                if(bRes){
                    m_simImgBuf[(i*CAP_WID+j)*3] = 255;
                    m_simImgBuf[(i*CAP_WID+j)*3+1] = 0;
                    m_simImgBuf[(i*CAP_WID+j)*3+2] = 0;
                }
            }
        }
    }
    updateImage();
}
/*!
 * \brief setSorterConsistenceWidget::getAlgSim
 * \param nAlgId
 * \param hei
 * \param wid
 */
bool setSorterConsistenceWidget::getAlgSim(int nAlgId, int hei, int wid)
{
    int nRow,nCol,nPur,nBadPixelNum;
    int nRowBegin,nRowEnd,nColBegin,nColEnd;

    if (false == struCnfp.nArithmeticEnable[nAlgId]){
        return false;
    }

    if(nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B || nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B) {
        if (false == m_bEnableAlg[nAlgId]){
            return false;
        }
        nRow = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nRow;
        nCol = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nColumn;
        nPur = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nPercent;
    }

    if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
        if (false == m_bEnableAlg[nAlgId-11]){
            return false;
        }
        nRow = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[nAlgId-ARITH_INTEL_A].nRow;
        nCol = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[nAlgId-ARITH_INTEL_A].nColumn;
        nPur = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[nAlgId-ARITH_INTEL_A].nPercent;
    }

    nRowBegin = hei-(nRow-1)/2-(nRow-1)%2;
    nRowEnd = hei+(nRow-1)/2;
    nColBegin = wid-(nCol-1)/2-(nCol-1)%2;
    nColEnd = wid+(nCol-1)/2;

    if(nColBegin < 0)
        nColBegin = 0;
    if(nColEnd > IMAGE_WIDTH-1)
        nColEnd = IMAGE_HEIGHT-1;
    if(nRowBegin < 0)
        nRowBegin = 0;
    if(nRowEnd > IMAGE_HEIGHT-1)
        nRowEnd = IMAGE_HEIGHT-1;

    nBadPixelNum = 0;

    for(int m=nRowBegin; m<=nRowEnd; m++) {
        for(int n=nColBegin; n<=nColEnd; n++) {
            uchar r = m_oriImgBuf[m_nCurLayer][(m*IMAGE_WIDTH+n)*3];
            uchar g = m_oriImgBuf[m_nCurLayer][(m*IMAGE_WIDTH+n)*3+1];
            uchar b = m_oriImgBuf[m_nCurLayer][(m*IMAGE_WIDTH+n)*3+2];

            if(myAIShare.pixelIsBackgroundcomm(r, g, b, n)){
                continue;
            }

            if (pixelIsBad(nAlgId,struGsh.nUnit,(int)r,(int)g,(int)b)){
                nBadPixelNum++;
            }

        }//end n
    }//end m

    if(nBadPixelNum > nPur){
        return true;
    }
    return false;
}
/*!
 * \brief setSorterConsistenceWidget::pixelIsBad
 * \param r
 * \param g
 * \param b
 * \return
 */
bool setSorterConsistenceWidget::pixelIsBad(int nAlgId, int nUnit, int r, int g, int b)
{
    uint rg = r*g;
    uint rb = r*b;
    uint gb = g*b;
    uint rr = r*r;
    uint gg = g*g;
    uint bb = b*b;
    int	indexOfArithIntel;
    int nCons, nQuo0, nQuo1, nQuo2, nQuo3;
    int nQuo4, nQuo5, nQuo6, nQuo7, nQuo8;
    int nSens;
    int nConsAdj;

    //! 灰度
    if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B){
        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nColor == 1){
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (r*1000 > 255*(1000-nSens)){
                    return true;
                }
            }
            else {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (r*1000 < 255*nSens){
                     return true;
                }
            }
        }
        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nColor == 2){
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (g*1000 > 255*(1000-nSens)){
                     return true;
                }
            }
            else {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (g*1000 < 255*nSens){
                     return true;
                }
            }
        }
        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nColor == 4){
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (b*1000 > 255*(1000-nSens)){
                     return true;
                }
            }
            else {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (b*1000 < 255*nSens){
                     return true;
                }
            }
        }
    }
    //! 色差
    if (nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nDiscolor == 0){
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (m_bHighDiff){
                    if (r*10000 > (r+g)*(10000-nSens)){
                         return true;
                    }
                }
                else {
                    if (r*1000 > (r+g)*(1000-nSens)){
                         return true;
                    }
                }
            }
            else {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (m_bHighDiff){
                    if (r*10000 < (r+g)*nSens){
                        return true;
                    }
                }
                else {
                    if (r*1000 < (r+g)*nSens){
                        return true;
                    }
                }
            }
        }
        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nDiscolor == 1){
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (m_bHighDiff){
                    if (r*10000 > (r+b)*(10000-nSens)){
                        return true;
                    }
                }
                else {
                    if (r*1000 > (r+b)*(1000-nSens)){
                        return true;
                    }
                }
            }
            else {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (m_bHighDiff){
                    if (r*10000 < (r+b)*nSens){
                         return true;
                    }
                }
                else {
                    if (r*1000 < (r+b)*nSens){
                         return true;
                    }
                }
            }
        }
        if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nDiscolor == 2){
            if (struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nMode) {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMax+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (m_bHighDiff){
                    if (g*10000 > (g+b)*(10000-nSens)){
                         return true;
                    }
                }
                else {
                    if (g*1000 > (g+b)*(1000-nSens)){
                         return true;
                    }
                }
            }
            else {
                nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struGreyColor[nAlgId].nSensMin+struCnfp.nSensBias[nUnit][nAlgId];
                nSens = (nSens > 0)?nSens:0;
                if (m_bHighDiff){
                    if (g*10000 < (g+b)*nSens){
                        return true;
                    }
                }
                else {
                    if (g*1000 < (g+b)*nSens){
                        return true;
                    }
                }
            }
        }
    }
    //! 智能
    if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
        indexOfArithIntel = nAlgId-ARITH_INTEL_A;
        nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nSens+struCnfp.nSensBias[nUnit][nAlgId];

        if (abs(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[1]) < 200000){
            nConsAdj = 200000;
        }
        else {
            nConsAdj = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[1];
        }
        //! 常数项
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[0] == 0){
            if(nSens > 100)
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[1]-nConsAdj*(nSens-100)/100.0;
            else
                nCons = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[1]+nConsAdj*(100-nSens)/100.0;
        }
        else{
            if(nSens > 100)
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[1]+nConsAdj*(nSens-100)/100.0);
            else
                nCons = -(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nConsDfl[1]-nConsAdj*(100-nSens)/100.0);
        }

        //! 系数1
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoR[0] == 0)
            nQuo0 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoR[1];
        else
            nQuo0 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoR[1];

        //! 系数2
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoG[0] == 0)
            nQuo1 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoG[1];
        else
            nQuo1 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoG[1];

        //! 系数3
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoB[0] == 0)
            nQuo2 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoB[1];
        else
            nQuo2 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoB[1];

        //! 系数4
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRG[0] == 0)
            nQuo3 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRG[1];
        else
            nQuo3 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRG[1];

        //! 系数5
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRB[0] == 0)
            nQuo4 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRB[1];
        else
            nQuo4 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRB[1];

        //! 系数6
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoGB[0] == 0)
            nQuo5 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoGB[1];
        else
            nQuo5 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoGB[1];

        //! 系数7
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRR[0] == 0)
            nQuo6 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRR[1];
        else
            nQuo6 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoRR[1];

        //! 系数8
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoGG[0] == 0)
            nQuo7 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoGG[1];
        else
            nQuo7 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoGG[1];

        //! 系数9
        if(struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoBB[0] == 0)
            nQuo8 = struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoBB[1];
        else
            nQuo8 = -struCnfp.struGroupIdentify[struGsh.nLevel][m_nCurView].struIntel[indexOfArithIntel].nQuoBB[1];

        int nValue = nCons+nQuo0*r+nQuo1*g+nQuo2*b+nQuo3*rg+nQuo4*rb+nQuo5*gb+nQuo6*rr+nQuo7*gg+nQuo8*bb;
        if(nValue < 0){
            return true;
        }
    }
    return false;
}
/*!
 * \brief setSorterConsistenceWidget::pixelIsBadInfra
 * \param nAlgId
 * \param nUnit
 * \param r
 * \param g
 * \param b
 * \return
 */
int setSorterConsistenceWidget::pixelIsBadInfra(int nAlgId, int nUnit)
{
    (void)nAlgId;
    (void)nUnit;
    return 0;
}
/*!
 * \brief setSorterConsistenceWidget::resetSensBias
 * \param nAlgId
 * \param nGroup
 */
void setSorterConsistenceWidget::resetSensBias(int nAlgId, int nGroup)
{
    int nUnit;
    int nAlgIdAdj;

    if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B || nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
        nAlgIdAdj = nAlgId;
        if (!m_bEnableAlg[nAlgIdAdj]){
            return;
        }
    }

    if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_A){
        nAlgIdAdj = nAlgId-11;
        if (!m_bEnableAlg[nAlgIdAdj]){
            return;
        }
    }

    for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[nGroup].nUnitCount; j++){
        nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[nGroup].nUnitId[j];
        if (m_bLightOrInfra) {
            struCnfp.nSensBias[nUnit][nAlgId] = 0;
        }
    }
}
/*!
 * \brief setSorterConsistenceWidget::getTarget
 * \param nAlgId
 * \return
 */
bool setSorterConsistenceWidget::getTarget(bool bTarget, int nUnit)
{
    int nBadNum[6] = {0};
    bool bRes;

   // m_struCurAlgTarget.nPixelSum = 0;
   // for( int i = 0; i < IMAGE_HEIGHT; i++){
     //   for(int j = 0; j < IMAGE_WIDTH; j++){
           /* uchar r = m_oriImgBuf[m_nCurLayer][(i*IMAGE_WIDTH+j)*3];
            uchar g = m_oriImgBuf[m_nCurLayer][(i*IMAGE_WIDTH+j)*3+1];
            uchar b = m_oriImgBuf[m_nCurLayer][(i*IMAGE_WIDTH+j)*3+2];

//            if(myAIShare.pixelIsBackgroundcomm(r, g, b, j)){
//                continue;
//            }
            if (r == 0 && g == 0 && b == 0){
                continue;
            }
            m_simImgBuf[m_struCurAlgTarget.nPixelSum*3]   = r;
            m_simImgBuf[m_struCurAlgTarget.nPixelSum*3+1] = g;
            m_simImgBuf[m_struCurAlgTarget.nPixelSum*3+2] = b;

            m_struCurAlgTarget.nPixelSum++;*/
	 if (m_struCurAlgTarget.nPixelSum <= 20000){
    	qDebug()<<m_struCurAlgTarget.nPixelSum;
        return false;
     }

	 for (int i = 0; i < m_struCurAlgTarget.nPixelSum;i++){

            uchar r = m_simImgBuf[i*3]  ;
            uchar g = m_simImgBuf[i*3+1];
            uchar b = m_simImgBuf[i*3+2];

            if (m_bEnableAlg[ARITH_GREY_A]) {
                bRes = pixelIsBad(ARITH_GREY_A,nUnit,r,g,b);
                if (bRes){
                    nBadNum[0]++;
                }
            }

            if (m_bEnableAlg[ARITH_GREY_B]) {
                bRes = pixelIsBad(ARITH_GREY_B,nUnit,r,g,b);
                if (bRes){
                    nBadNum[1]++;
                }
            }

            if (m_bEnableAlg[ARITH_DISCOLOR_A]) {
                bRes = pixelIsBad(ARITH_DISCOLOR_A,nUnit,r,g,b);
                if (bRes){
                    nBadNum[2]++;
                }
            }

            if (m_bEnableAlg[ARITH_DISCOLOR_A]) {
                bRes = pixelIsBad(ARITH_DISCOLOR_A,nUnit,r,g,b);
                if (bRes){
                    nBadNum[3]++;
                }
            }

            if (m_bEnableAlg[ARITH_INTEL_A-11]) {
                bRes = pixelIsBad(ARITH_INTEL_A,nUnit,r,g,b);
                if (bRes){
                    nBadNum[4]++;
                }
            }

            if (m_bEnableAlg[ARITH_INTEL_B-11]) {
                bRes = pixelIsBad(ARITH_INTEL_B,nUnit,r,g,b);
                if (bRes){
                    nBadNum[5]++;
                }
            }
      //  }
    }

    for (int i = 0; i < 6; i++){
        if (!m_bEnableAlg[i]) {
				continue;
		}
        if (bTarget){
//            qDebug()<<nBadNum[i];
            m_struCurAlgTarget.nProbTarget[i] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
         //   m_struCurAlgTarget.nProbTarget[i] = 200;
            m_struCurAlgTarget.nAlgProb[i][nUnit/2] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
            qDebug("prob:%d",m_struCurAlgTarget.nProbTarget[i]);
        }
        else {
//            qDebug()<<nBadNum[i];
            m_struCurAlgTarget.nCurProb[i] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
            m_struCurAlgTarget.nAlgProb[i][nUnit/2] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
            qDebug("prob:%d",m_struCurAlgTarget.nCurProb[i]);
        }
    }

    return true;
}
/*!
 * \brief setSorterConsistenceWidget::getTargetInfra
 * \param bTarget
 * \param nUnit
 * \return
 */
bool setSorterConsistenceWidget::getTargetInfra(bool bTarget, int nUnit)
{
    int nBadNum[6] = {0};

    qDebug("sum-%d",m_struCurAlgTarget.nPixelSum);
    if (m_struCurAlgTarget.nPixelSum <= 2000){
        return false;
    }

    if (m_bEnableAlg[ARITH_GREY_A]) {
        nBadNum[0] = pixelIsBadInfra(ARITH_GREY_A,nUnit);
    }
    if (m_bEnableAlg[ARITH_DISCOLOR_A]) {
        nBadNum[2] = pixelIsBadInfra(ARITH_DISCOLOR_A,nUnit);
    }
    if (m_bEnableAlg[ARITH_DISCOLOR_B]) {
        nBadNum[3] = pixelIsBadInfra(ARITH_DISCOLOR_B,nUnit);
    }

    for (int i = 0; i < 4; i++){
        if (!m_bEnableAlg[i]) {
            continue;
        }
        if (bTarget){
//            qDebug()<<nBadNum[i];
            m_struCurAlgTarget.nProbTarget[i] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
//            m_struCurAlgTarget.nProbTarget[i] = 200;
            m_struCurAlgTarget.nAlgProb[i][nUnit/2] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
            qDebug("prob:%d",m_struCurAlgTarget.nProbTarget[i]);
        }
        else {
//            qDebug()<<nBadNum[i];
            m_struCurAlgTarget.nCurProb[i] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
            m_struCurAlgTarget.nAlgProb[i][nUnit/2] = nBadNum[i]*10000.0/m_struCurAlgTarget.nPixelSum;
            qDebug("prob:%d",m_struCurAlgTarget.nCurProb[i]);
        }
    }

    return true;
}
/*!
 * \brief setSorterConsistenceWidget::adjustSensBias
 */
void setSorterConsistenceWidget::adjustSensBias(int nUnit)
{
    if (!getTarget(false,nUnit)){
        return;
    }
    getAlgSensBias(ARITH_GREY_A, nUnit);
    getAlgSensBias(ARITH_GREY_B, nUnit);
    getAlgSensBias(ARITH_DISCOLOR_A, nUnit);
    getAlgSensBias(ARITH_DISCOLOR_B, nUnit);
    getAlgSensBias(ARITH_INTEL_A, nUnit);
    getAlgSensBias(ARITH_INTEL_B, nUnit);
}
/*!
 * \brief setSorterConsistenceWidget::adjustSensBiasInfra
 * \param nUnit
 */
void setSorterConsistenceWidget::adjustSensBiasInfra(int nUnit)
{
    if (!getTargetInfra(false,nUnit)){
        return;
    }
    getAlgSensBiasInfra(ARITH_GREY_A, nUnit);
    getAlgSensBiasInfra(ARITH_DISCOLOR_A, nUnit);
    getAlgSensBiasInfra(ARITH_DISCOLOR_B, nUnit);
}
/*!
 * \brief setSorterConsistenceWidget::getAlgSensBias
 * \param nAlgId
 * \param nUnit
 * \return
 */
bool setSorterConsistenceWidget::getAlgSensBias(int nAlgId, int nUnit)
{
    float tmpProb;
    int tmpBadNum, nAlgIdAdj,nAdjustMax = 50, nAdjustMin = 5;
    bool bRes;
    int nSensMax, nSensMin;
    int nAdjSensMax, nAdjSensMin;

    if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B || nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
         nAlgIdAdj = nAlgId;
        if (!m_bEnableAlg[nAlgIdAdj]){
            return false;
        }
    }

    if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
        nAlgIdAdj = nAlgId-11;
        if (!m_bEnableAlg[nAlgIdAdj]){
            return false;
        }
    }

    nSensMax = m_struCurAlgTarget.nProbTarget[nAlgIdAdj]+nAdjustMax;
    nSensMin = m_struCurAlgTarget.nProbTarget[nAlgIdAdj]-nAdjustMax;
    nAdjSensMax = m_struCurAlgTarget.nProbTarget[nAlgIdAdj]+nAdjustMin;
    nAdjSensMin = m_struCurAlgTarget.nProbTarget[nAlgIdAdj]-nAdjustMin;
    if (nSensMin < 0){
        nSensMin = 0;
    }

    if (m_struCurAlgTarget.nCurProb[nAlgIdAdj] > nSensMax){
        for (int n = 0;n > -300;) {
            tmpBadNum = 0;
            for (int i = 0; i < m_struCurAlgTarget.nPixelSum; i++) {
                uchar r = m_simImgBuf[i*3];
                uchar g = m_simImgBuf[i*3+1];
                uchar b = m_simImgBuf[i*3+2];

                if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B){
                    struCnfp.nSensBias[nUnit][nAlgId] = n;
                }

                if (nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
                    struCnfp.nSensBias[nUnit][nAlgId] = (m_bHighDiff)?n*10:n;
                }

                if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
                    struCnfp.nSensBias[nUnit][nAlgId] = n/3;
                }

                bRes = pixelIsBad(nAlgId,nUnit,r,g,b);

                if (bRes){
                    tmpBadNum++;
                }
            }
            tmpProb = (float)tmpBadNum*10000.0/m_struCurAlgTarget.nPixelSum;
//            qDebug("N:%d",n);
//            qDebug("badNum:%d",tmpBadNum);
//            qDebug("prob:%f",tmpProb);

            m_struCurAlgTarget.nAlgProb[nAlgIdAdj][nUnit/2] = tmpProb;
            if (tmpProb <= nAdjSensMax) {
                qDebug("N:%d",n);
                qDebug("badNum:%d",tmpBadNum);
                qDebug("prob:%f",tmpProb);
                //! 参数发送
                myFlow.materialCopyAssemble(struGsh.nLevel      , m_nCurView, 0, nAlgId, 0);
                myFlow.materialResetGroupAssemble(struGsh.nLevel, m_nCurView, 0, nAlgId, 0);
                return true;
            }
            if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B || nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
                n--;
            }

            if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
                n -= 3;
            }
        }
    }
    else if (m_struCurAlgTarget.nCurProb[nAlgIdAdj] < nSensMin){
        for (int n = 0;n < 300;) {
            tmpBadNum = 0;
            for (int i = 0; i < m_struCurAlgTarget.nPixelSum; i++) {
                uchar r = m_simImgBuf[i*3];
                uchar g = m_simImgBuf[i*3+1];
                uchar b = m_simImgBuf[i*3+2];

                if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B){
                    struCnfp.nSensBias[nUnit][nAlgId] = n;
                }

                if (nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
                                        struCnfp.nSensBias[nUnit][nAlgId] = (m_bHighDiff)?n*10:n;
                }

                if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
                    struCnfp.nSensBias[nUnit][nAlgId] = n/3;
                }
                bRes = pixelIsBad(nAlgId,nUnit,r,g,b);

                if (bRes){
                    tmpBadNum++;
                }
            }
            tmpProb = (float)tmpBadNum*10000.0/m_struCurAlgTarget.nPixelSum;
//            qDebug("N:%d",n);
//            qDebug("badNum:%d",tmpBadNum);
//            qDebug("prob:%f",tmpProb);
            m_struCurAlgTarget.nAlgProb[nAlgIdAdj][nUnit/2] = tmpProb;
            if (tmpProb >= nAdjSensMin) {
                qDebug("N:%d",n);
                qDebug("badNum:%d",tmpBadNum);
                qDebug("prob:%f",tmpProb);
                //! 参数发送
                myFlow.materialCopyAssemble(struGsh.nLevel      , m_nCurView, 0, nAlgId, 0);
                myFlow.materialResetGroupAssemble(struGsh.nLevel, m_nCurView, 0, nAlgId, 0);
                return true;
            }
            if (nAlgId == ARITH_GREY_A || nAlgId == ARITH_GREY_B || nAlgId == ARITH_DISCOLOR_A || nAlgId == ARITH_DISCOLOR_B){
                n++;
            }

            if (nAlgId == ARITH_INTEL_A || nAlgId == ARITH_INTEL_B){
                n += 3;
            }
        }
    }
    //! 参数发送
    myFlow.materialCopyAssemble(struGsh.nLevel      , m_nCurView, 0, nAlgId, 0);
    myFlow.materialResetGroupAssemble(struGsh.nLevel, m_nCurView, 0, nAlgId, 0);
    return false;
}
/*!
 * \brief setSorterConsistenceWidget::getAlgSensBiasInfra
 * \param nAlgId
 * \param nUnit
 * \return
 */
bool setSorterConsistenceWidget::getAlgSensBiasInfra(int nAlgId, int nUnit)
{
    (void)nAlgId;
    (void)nUnit;
    return false;
}
/*!
 * \brief setSorterConsistenceWidget::commonProcessMainWidgetSlt
 */
void setSorterConsistenceWidget::commonProcessMainWidgetSlt()
{
    int nUnit;
    bool bGetImgOk = false;
    if (sender() == m_viewNumBtn){
        m_bFlgSim = false;
        m_mainStackedWidget->setCurrentIndex(PAGE_SIM_CHUTE);
        updateChuteWidget();
        return;
    }

    if (sender() == m_chuteNumBtn){
        m_bFlgSim = false;
        m_mainStackedWidget->setCurrentIndex(PAGE_SIM_CHUTE);
        updateChuteWidget();
        return;
    }

    if (sender() == m_captureImgBtn){
        infoWidget->setLabelText(myLan.ai_image_capturing);
        infoWidget->delayShow();
        bGetImgOk = getImage(struGsh.nUnit);
        infoWidget->hide();
        if (!bGetImgOk){
            infoWidget->setLabelText(myLan.ai_image_capture_error);
            infoWidget->delayShow();
            myFlow.sleep(3);
            infoWidget->hide();
            return;
        }
    }

    if (sender() == m_simImgBtn){
        m_bFlgSim = !m_bFlgSim;
        //qDebug()<<m_bFlgSim;
    }

    for (int i = 0; i < 6; i++){
        if (sender() == m_algEnableCbx[i]){
            //qDebug()<<m_bEnableAlg[i];
            m_bEnableAlg[i] = m_algEnableCbx[i]->getChecked();
            //qDebug()<<m_bEnableAlg[i];
            break;
        }

        if (sender() == m_sensLne[i]){
            if (m_bLightOrInfra){
                setAlgSens(i);
            }
            else {
                setAlgSensInfra(i);
            }
            break;
        }

        if (sender() == m_detailBtn[i]){
        	m_bFlgSim = false;
            m_nCurAlg = i;
            //qDebug()<<i;
            m_mainStackedWidget->setCurrentIndex(PAGE_SIM_DETAIL);
            updateDetailWidget();
            return;
        }
    }

    if (sender() == m_levelChgBtn){
        m_bLightOrInfra = !m_bLightOrInfra;
    }

    if (sender() == m_consistenceBtn){
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        m_bHighDiff = false;
        resetSensBias(ARITH_GREY_A,m_nCurView);
        resetSensBias(ARITH_GREY_B,m_nCurView);
        resetSensBias(ARITH_DISCOLOR_A,m_nCurView);
        resetSensBias(ARITH_DISCOLOR_B,m_nCurView);
        resetSensBias(ARITH_INTEL_A,m_nCurView);
        resetSensBias(ARITH_INTEL_B,m_nCurView);
        if (m_bLightOrInfra){
            /*if (!getImage(struGsh.nUnit)){
				infoWidget->setLabelText(myLan.ai_image_capture_error);
				infoWidget->delayShow();
				myFlow.sleep(3);
				infoWidget->hide();
                return;
            }
            if(!getTarget(true,struGsh.nUnit)){
				infoWidget->setLabelText(myLan.ai_image_capture_error);
				infoWidget->delayShow();
				myFlow.sleep(3);
				infoWidget->hide();
                return;
            }
            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
                // for(int j=1; j<3; j++){
                nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j];
                qDebug()<<nUnit;
                if (nUnit == struGsh.nUnit){
                    continue;
                }
                if (!getImage(nUnit)){
                    continue;
                }
                adjustSensBias(nUnit);
            }*/

            if (!initUSB()){
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->delayShow();
                myFlow.sleep(3);
                infoWidget->hide();
                return;
            }

            MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, struGsh.nUnit, 0, 0,(IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1, 3);
            MySerial.com1Write(2, 2, 0, 0x3f,  0, 0, 0, 0, 0, 1);
            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
                // for(int j=1; j<3; j++){
                nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j];
            	MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, nUnit, 0 ,0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
            //	MySerial.com1Write(CMD_UNIT_CAPTURE_RANGE, UNIT, struGsh.nLevel, struGsh.nUnit, 0 ,800/256, 800%256, 850/256, 850%256, 1);
                MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, nUnit, 20, 1, 1, (IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1);
            }

            myFlow.sleep(2);

            if (!getMaterData(struGsh.nUnit)){
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->delayShow();
                myFlow.sleep(3);
                infoWidget->hide();
                return;
            }

            if(!getTarget(true,struGsh.nUnit)){
                infoWidget->setLabelText(myLan.ai_image_capture_error);
                infoWidget->delayShow();
                myFlow.sleep(3);
                infoWidget->hide();
                return;
            }

            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
           //  for(int j=2; j<4; j++){
                 nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j];
                 qDebug()<<nUnit;
                 if (nUnit == struGsh.nUnit){
                     continue;
                 }
                 if (!getMaterData(nUnit)){
                     continue;
                 }
                 adjustSensBias(nUnit);
             }
        }
        else {
            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
                // for(int j=1; j<3; j++){
                nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j]+struGsh.nDevTypeAddrBias;

                //! 开始统计信号直方图
                MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_CAPTURE, UNIT, struGsh.nLevel, nUnit, 0, 0, 0, 0, 1, 1);
            }

            myFlow.sleep(5);

            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
                // for(int j=1; j<3; j++){
                nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j]+struGsh.nDevTypeAddrBias;

                //! 停止统计信号直方图数据
                MySerial.com1Write(CMD_UNIT_STATISTIC_HISTOGRAM_CAPTURE, UNIT, struGsh.nLevel, nUnit, 0, 0, 0, 0, 0, 1);
            }

//			m_struCurAlgTarget.nProbTarget[2] = 0;
//			for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
//            for(int j=0; j<2; j++){
//                nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j];
//                qDebug()<<nUnit;
               // getHist(nUnit+struGsh.nDevTypeAddrBias);
            //    adjustSensBiasInfra(nUnit);
                getHist(struGsh.nUnit+struGsh.nDevTypeAddrBias);

                if(!getTargetInfra(true,struGsh.nUnit)){
                	infoWidget->setLabelText(myLan.ai_unsupervised_less_point);
                	infoWidget->showSecs(5);
                	return;
            	}

//				m_struCurAlgTarget.nProbTarget[2] += m_struCurAlgTarget.nAlgProb[2][nUnit/2];

//            }
//			m_struCurAlgTarget.nProbTarget[2] = m_struCurAlgTarget.nProbTarget[2]/2.0;
//			qDebug("target:%d",m_struCurAlgTarget.nProbTarget[2]);

            for(int j=0; j<struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount; j++){
//           for(int j=0; j<2; j++){
                nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[j];
                qDebug()<<nUnit;
                if (nUnit == struGsh.nUnit){
                    continue;
                }
                getHist(nUnit+struGsh.nDevTypeAddrBias);
                adjustSensBiasInfra(nUnit);
            }
        }
        infoWidget->hide();
    }

    if (sender() == m_simBackBtn){
        for(int i=0; i<struCnfg.nInterfaceBoardTotal; i++) {
            MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, 0, 3);
        }
        emit goToSetSensPageSig();
    }

    updateMainWidget();
}
/*!
 * \brief setSorterConsistenceWidget::commonProcessDetailWidgetSlt
 */
void setSorterConsistenceWidget::commonProcessDetailWidgetSlt()
{
    int nUnit;
    for (int i = 0; i< 10; i++){
        if (sender() == m_sensBiasLne[i]){
            nUnit = (m_nCurView%2)?i*2+1:i*2;
            if (m_bLightOrInfra){
                setAlgSensBias(nUnit);
            }
            else {
                setAlgSensBiasInfra(nUnit);
            }
            break;
        }
    }

    if (sender() == m_detailResetBtn){
        for (int i = 0; i < 10; i++) {
            nUnit = (m_nCurView%2)?i*2+1:i*2;
            if (m_bLightOrInfra){
                struCnfp.nSensBias[nUnit][m_nCurAlg] = 0;
            }
        }
    }

    if (sender() == m_detailBackBtn){
        m_mainStackedWidget->setCurrentIndex(PAGE_SIM_MAIN);
        updateMainWidget();
        return;
    }

    updateDetailWidget();
}
/*!
 * \brief setSorterConsistenceWidget::commonProcessChuteWidgetSlt
 */
void setSorterConsistenceWidget::commonProcessChuteWidgetSlt()
{
    for (int i= 0; i< 6;i++){
        if (sender() == m_viewSettingCbx[i]){
            m_nCurView = i;
           // qDebug()<<m_nCurView;
           // qDebug()<<struGsh.nUnit;
            if (struGsh.nUnit < struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[0] ||
                    m_nCurUnit > struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitCount-1]/2){
                struGsh.nUnit = struCnfg.struLevelInfo[struGsh.nLevel].struIdentifyGroupInfo[m_nCurView].nUnitId[0];
                m_nCurUnit = struGsh.nUnit/2;
            }

            struGsh.nUnit = (m_nCurView%2)?(m_nCurUnit*2+1):(m_nCurUnit*2);
            //qDebug()<<m_nCurView;
            //qDebug()<<struGsh.nUnit;
            break;
        }
    }

    for (int i = 0; i < 10; i++){
        if (sender() == m_chuteSettingBtn[i]){
            m_nCurUnit = i;
           // qDebug()<<m_nCurUnit;
            struGsh.nUnit = (m_nCurView%2)?(m_nCurUnit*2+1):(m_nCurUnit*2);
            m_nCurView   = getUnitGroup(struGsh.nLevel,struGsh.nUnit);
            break;
        }
    }

    if (sender() == m_chuteBackBtn){
        m_mainStackedWidget->setCurrentIndex(PAGE_SIM_MAIN);
        updateMainWidget();
        return;
    }

    m_nCurLayer = struGsh.nUnit%2;
    updateChuteWidget();
}
