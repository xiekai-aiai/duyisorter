/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        aimainwidget.cpp
 * \brief       彩色机型智能分析主界面头文件
 * \date        2015.01.14
 */
#include "aimainwidget.h"
#include <iostream>
#include <cstring>
#include <vector>
#include "unilog.h"
#ifdef Q_OS_UNIX
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
 // #include <opencv2/opencv.hpp>
#endif


/*!
 * \brief AIMainWidget::AIMainWidget
 * \param parent
 */
AIMainWidget::AIMainWidget(QWidget* parent) :
    QWidget(parent)
{
    delTimer = new QTimer();
    connect(delTimer, SIGNAL(timeout()), this, SLOT(onDeleteTimerOutSlt()));

    // set geometry
    setGeometry(0, 0, LCD_WIDTH, LCD_HEIGHT - LCD_TITLE_HEIGHT - LCD_TITLE_HEIGHT - 10);

    m_nIndexOfImg = 0;
    m_nIndexOfPage = 0;
    m_nNumOfImg = 0;
    m_bStatDelImg = false;
    m_algorithmType = 0;
    m_currentChan = 0;
    m_bCalSet = struCnfp.nMatAssembleModeIntel;

    for (int i = 0; i < MAX_AI; i++)
    {
        for (int j = 0; j < MAX_AI; j++)
        {
            m_bIsAutoCopy[i][j] = false;          //初始化不使能自动复制功能
        }
    }

    m_bIsCaptureAllView = false;
    m_bIsCuteOffBg = false;
    m_nLayer = 0;
    m_nPicType = 0;
    m_bIsAutoGetData = false;
    m_nLevelToTal = 0;
    m_nRGBNum = 0;
    m_nIndexOfArithIntel = 0;
    for (int j = 0; j < MAX_IMG_NUM;j++)
    {
        m_nViewNo[j] = -1;
    }

    for (int i = 0; i < MAX_VIEW_LOCATION; i++)
    {
        for (int j = 0; j < MAX_IMG_NUM;j++)
        {
            setImgName(j, NULL);
            m_nStatOfImg[i][j] = CLASS_NULL;
            m_bSampedOfImg[j] = SAMPLED_NO;
        }
        m_nClassBadNum[i] = 0;
        m_nClassGoodNum[i] = 0;
        m_nClassNum[i] = 0;
    }

    // create stackedwidget
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setGeometry(0, 0, width(), height());

    // add child widget to stackedwidget

    m_AIArithListWidget = new QWidget();
    m_stackedWidget->addWidget(m_AIArithListWidget);
    createAIArithListWidget();

    // create AI main widget
    m_AIMainWidget = new QWidget();
    m_stackedWidget->addWidget(m_AIMainWidget);
    createAIMainWidget();

    m_AIPreviewWidget = new AIPreviewWidget();
    m_stackedWidget->addWidget(m_AIPreviewWidget);

    m_AIImageWidget = new QWidget();
    m_stackedWidget->addWidget(m_AIImageWidget);
    createAIImageWidget();

    m_AIInputParamsWidget = new AIInputParams();
    m_stackedWidget->addWidget(m_AIInputParamsWidget);

    //智能参数设置
    m_AIParamsSetWidget = new QWidget();
    m_stackedWidget->addWidget(m_AIParamsSetWidget);
    createAIModeParamsWidget();

    connect(m_stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetIndexChangedSlt(int)));
    connect(m_AIPreviewWidget, SIGNAL(backToAIMainPageSig()), this, SLOT(goBackToMainWidget()));
    connect(this, SIGNAL(deleteCurImgSig()), m_AIPreviewWidget, SLOT(onDelCurImgSlt()));
    connect(this, SIGNAL(deleteAllImgSig()), m_AIPreviewWidget, SLOT(onDelAllImgSlt()));
    connect(this, SIGNAL(updatePreviewPageSig()), m_AIPreviewWidget, SLOT(refreshDisplaySlt()));
    connect(m_AIInputParamsWidget, SIGNAL(backToHomePageSig()), this, SLOT(goBackToAIImageWidget()));

    m_stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

/*!
 * \brief AIMainWidget::createAIMainWidget \n
 */
void AIMainWidget::createAIMainWidget()
{
    // init layout
    m_mainVBLayout = new QVBoxLayout(m_AIMainWidget);

    m_upHBLayout = new QHBoxLayout();
    m_downHBLayout = new QHBoxLayout();

    m_setVBLayout = new QVBoxLayout();

    m_imgListGridLayout = new QGridLayout();
    m_channelGridLayout = new QGridLayout();
    m_commVBLayout = new QVBoxLayout();

    // init groupbox
    m_imgListGBox = new myGroupBox("", m_AIMainWidget);
    m_channelGBox = new myGroupBox("", m_AIMainWidget);
    m_commGBox = new myGroupBox("", m_AIMainWidget);

    m_imgListGBox->setLayout(m_imgListGridLayout);
    m_channelGBox->setLayout(m_channelGridLayout);
    m_commGBox->setLayout(m_commVBLayout);


    // init buttons of img list
    m_imgListSMapper = new QSignalMapper(m_AIMainWidget);

    for (int i = 0; i < MAX_PER_LIST;i++)
    {
        m_imgListBtn[i] = new myPushButton("", QIcon(), m_AIMainWidget);

        m_imgPropertyBtn[i] = new myPushButton("", QIcon(), m_AIMainWidget);
        m_imgPropertyBtn[i]->setMaximumHeight(BTN_HEIGHT);

        if (i / 3 == 0)
        {
            m_imgListGridLayout->addWidget(m_imgListBtn[i], 0, i % 3, 1, 1);
            m_imgListGridLayout->addWidget(m_imgPropertyBtn[i], 1, i % 3, 1, 1);
        }
        else
        {
            m_imgListGridLayout->addWidget(m_imgListBtn[i], 2, i % 3, 1, 1);
            m_imgListGridLayout->addWidget(m_imgPropertyBtn[i], 3, i % 3, 1, 1);
        }

        /*使用信号栈方式实现各供料设置*/
        m_imgListSMapper->setMapping(m_imgListBtn[i], i * 2);
        m_imgListSMapper->setMapping(m_imgPropertyBtn[i], i * 2 + 1);
        connect(m_imgListBtn[i], SIGNAL(clicked()), m_imgListSMapper, SLOT(map()));
        connect(m_imgPropertyBtn[i], SIGNAL(pressed()), m_imgListSMapper, SLOT(map()));
    }

    // init btns of channel groupbox
    m_layerBtn = new myPushButton("", QIcon(), m_AIMainWidget);
    m_layerBtnAdd = new myPushButton("", QIcon(), m_AIMainWidget);
    m_chanNum = new QLCDNumber(2);
    m_chanNum->display(1);
    m_plusChanNumBtn = new myPushButton("", myIcon.Action_Plus, m_AIMainWidget);
    m_minusChanNUmBtn = new myPushButton("", myIcon.Action_Minus, m_AIMainWidget);
    m_impurityNameLe = new myLineEdit;

    m_layerBtn->setMaximumSize(QSize(ICON_WID + 35, ICON_HEI + 16));
    m_layerBtnAdd->setMaximumSize(QSize(ICON_WID + 35, ICON_HEI + 16));
    m_plusChanNumBtn->setMaximumSize(QSize(ICON_WID + 16, ICON_HEI + 16));
    m_minusChanNUmBtn->setMaximumSize(QSize(ICON_WID + 16, ICON_HEI + 16));

    m_chanNum->setMaximumSize(QSize(ICON_WID + 16, ICON_HEI + 16));
    m_channelGridLayout->addWidget(m_layerBtn, 0, 0);
    m_channelGridLayout->addWidget(m_chanNum, 0, 1);
    m_channelGridLayout->addWidget(m_layerBtnAdd, 0, 2);
    m_channelGridLayout->addWidget(m_minusChanNUmBtn, 1, 0);
    m_channelGridLayout->addWidget(m_plusChanNumBtn, 1, 1);
    m_channelGridLayout->addWidget(m_impurityNameLe, 2, 0, 1, 2);

    // init btns of communication groupbox
    m_AICaptureAllView = new myCustomCheckBox(myLan.co_capture, m_bIsCaptureAllView, 0, m_AIMainWidget);
    m_exportImgBtn = new myPushButton("", myIcon.Action_Configure, m_AIMainWidget);

    m_AICaptureAllView->setFixedSize(BTN_WIDTH, BTN_HEIGHT + 10);
    //    m_AICaptureAllView->setMaximumSize(QSize(BTN_WIDTH+50,BTN_HEIGHT));
    //    m_exportImgBtn->setMaximumSize(QSize(ICON_WID+16, ICON_HEI+16));
    m_exportImgBtn->setFixedSize(QSize(ICON_WID + 10, ICON_HEI + 10));

    m_commGBox->setFixedHeight(160);

    m_commVBLayout->addWidget(m_AICaptureAllView, 1, Qt::AlignVCenter | Qt::AlignHCenter);
    m_commVBLayout->addStretch(0);
    m_commVBLayout->addWidget(m_exportImgBtn, 1, Qt::AlignBottom | Qt::AlignLeft);


    m_calSetBtn = new myPushButton(myLan.ai_cal_set_same, QIcon(), m_AIMainWidget);
    m_classModeBtn = new myPushButton(myLan.ai_class_mode_null, QIcon(), m_AIMainWidget);
    m_AIParamsSetBtn = new myPushButton(myLan.ai_params_set, QIcon(), m_AIMainWidget);
    m_AIParamsSetBtn->hide();

    m_AIStudyBtn = new myPushButton(myLan.ai_auto_cal, QIcon(), false, m_AIMainWidget);

    m_calSetBtn->setFixedHeight(BTN_HEIGHT);
    m_classModeBtn->setFixedHeight(BTN_HEIGHT);
    m_AIStudyBtn->setFixedHeight(BTN_HEIGHT * 2);
    m_AIParamsSetBtn->setFixedHeight(BTN_HEIGHT);

    if (LCD_WIDTH == 640)
    {
        m_channelGBox->setFixedWidth(ICON_WID * 2 + 50);
        m_commGBox->setFixedWidth(ICON_WID * 2 + 50);
        m_calSetBtn->setFixedWidth(ICON_WID * 2 + 50);
        m_classModeBtn->setFixedWidth(ICON_WID * 2 + 50);
        m_AIParamsSetBtn->setFixedWidth(ICON_WID * 2 + 50);
        m_AIStudyBtn->setFixedWidth(ICON_WID * 2 + 50);
    }
    else
    {
        m_channelGBox->setFixedSize(170, 300);
        m_commGBox->setFixedWidth(170);
        m_calSetBtn->setFixedWidth(170);
        m_AIParamsSetBtn->setFixedWidth(170);
        m_classModeBtn->setFixedWidth(170);
        m_AIStudyBtn->setFixedWidth(170);
    }

    m_setVBLayout->addWidget(m_channelGBox);
    m_setVBLayout->addStretch(0);
    m_setVBLayout->addWidget(m_calSetBtn);
    m_setVBLayout->addStretch(0);
    m_setVBLayout->addWidget(m_commGBox);
    m_setVBLayout->addStretch(0);
    m_setVBLayout->addWidget(m_classModeBtn);
    m_setVBLayout->addStretch(0);
    m_setVBLayout->addWidget(m_AIParamsSetBtn);
    m_setVBLayout->addStretch(0);
    m_setVBLayout->addWidget(m_AIStudyBtn);

    // init buttons of down groupbox
    m_startCapBtn = new myPushButton(myLan.ai_capture, QIcon(), m_AIMainWidget);
    m_delImgBtn = new myPushButton(myLan.ai_delete, QIcon(), m_AIMainWidget);
    m_preListBtn = new myPushButton(myLan.last_page, QIcon(), false, m_AIMainWidget);
    m_nextListBtn = new myPushButton(myLan.next_page, QIcon(), true, m_AIMainWidget);
    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back, m_AIMainWidget);

    m_startCapBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_delImgBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_preListBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_nextListBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_backBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    m_downHBLayout->setSpacing((int)(this->width() - 5 * BTN_WIDTH) / 4);
    m_downHBLayout->addWidget(m_startCapBtn);
    m_downHBLayout->addWidget(m_delImgBtn);
    m_downHBLayout->addWidget(m_preListBtn);
    m_downHBLayout->addWidget(m_nextListBtn);
    m_downHBLayout->addWidget(m_backBtn);

    // the combination of layout
    m_upHBLayout->addWidget(m_imgListGBox);
    m_upHBLayout->addLayout(m_setVBLayout);

    m_mainVBLayout->setContentsMargins(1, 5, 10, 10);
    m_mainVBLayout->addLayout(m_upHBLayout);
    m_mainVBLayout->addLayout(m_downHBLayout);

    // update image list
    initAIMainWidget();
    updateImgList();
    updateBtnStatus();

    // slots
    connect(m_layerBtn, SIGNAL(pressed()), this, SLOT(onLayerBtnPressedSlt()));
    connect(m_layerBtnAdd, SIGNAL(pressed()), this, SLOT(onLayerBtnPressedSltAdd()));
    connect(m_plusChanNumBtn, SIGNAL(pressed()), this, SLOT(onPlusChanNumPressedSlt()));
    connect(m_minusChanNUmBtn, SIGNAL(pressed()), this, SLOT(onMinusChanNumPressedSlt()));

    connect(m_AICaptureAllView, SIGNAL(pressed()), this, SLOT(onAICaptureAllViewPressedSlt()));
    connect(m_exportImgBtn, SIGNAL(pressed()), this, SLOT(onExportImgBtnPressedSlt()));
    connect(m_calSetBtn, SIGNAL(pressed()), this, SLOT(onCalSetBtnSlt()));
    connect(m_classModeBtn, SIGNAL(pressed()), this, SLOT(onClassModeBtnPressedSlt()));
    connect(m_AIParamsSetBtn, SIGNAL(pressed()), this, SLOT(onAIParamsSetBtnPressedSlt()));
    connect(m_AIStudyBtn, SIGNAL(pressed()), this, SLOT(onAIStudyBtnPressedSlt()));

    connect(m_startCapBtn, SIGNAL(pressed()), this, SLOT(onStartCapBtnPressedSlt()));
    connect(m_delImgBtn, SIGNAL(pressed()), this, SLOT(onDeleteBtnPressedSlt()));
    connect(m_delImgBtn, SIGNAL(released()), this, SLOT(onDeleteBtnReleasedSlt()));
    connect(m_preListBtn, SIGNAL(pressed()), this, SLOT(onPreBtnPressedSlt()));
    connect(m_nextListBtn, SIGNAL(pressed()), this, SLOT(onNextBtnPressedSlt()));
    connect(m_backBtn, SIGNAL(clicked()), this, SLOT(onBackBtnPressedSlt()));

    connect(m_imgListSMapper, SIGNAL(mapped(int)), this, SLOT(imageListChangeSlt(int)));
    connect(m_impurityNameLe, SIGNAL(pressed()), this, SLOT(onImpurityNameLePressed()));
}

void AIMainWidget::createAIArithListWidget()
{
    arithAIGroup = new myGroupBox("", m_AIArithListWidget);
    sigBoxMapper = new QSignalMapper(m_AIArithListWidget);
    sigBtnMapper = new QSignalMapper(m_AIArithListWidget);

    vBoxBtnLayout = new QVBoxLayout(arithAIGroup);
    m_preVboxlayout = new QVBoxLayout(m_AIArithListWidget);

    //size policy
    aiBtn.append(new myPushButton(myString.sArithmeticName[ARITH_INTEL_A], QIcon(), m_AIArithListWidget));
    aiBtn.append(new myPushButton(myString.sArithmeticName[ARITH_INTEL_B], QIcon(), m_AIArithListWidget));
    for (int i = 0; i < 2; i++)
    {   //大蒜物料支持智能A/B两种算法
        aiBox.append(new MyCheckBox("", struCnfp.nArithmeticEnable[ARITH_INTEL_A + i]));
        aiBox[i]->setMinimumSize(BTN_HEIGHT, BTN_HEIGHT);

        aiBtn[i]->setFocusPolicy(Qt::NoFocus);
        aiBtn[i]->setMinimumSize(BTN_WIDTH, BTN_HEIGHT);

        if (struCnfp.nArithmeticEnable[ARITH_INTEL_A + i] == 1)
        {
            aiBtn[i]->setEnabled(true);
        }
        else
        {
            aiBtn[i]->setEnabled(false);
        }
    }

    for (int i = 0; i < aiBox.size(); i++)
    {
        boxBtnLayout.append(new QFormLayout);
        boxBtnLayout[i]->addRow(aiBox[i], aiBtn[i]);
    }

    for (int i = 0; i < aiBox.size(); i++)
    {
        vBoxBtnLayout->addStretch();
        vBoxBtnLayout->addLayout(boxBtnLayout[i], 0);
    }
    vBoxBtnLayout->addStretch();

    for (int i = 0; i < aiBox.size(); i++)
    {
        //signal mapped
        sigBoxMapper->setMapping(aiBox[i], i);
        sigBtnMapper->setMapping(aiBtn[i], i);

        //signal & slot
        connect(aiBox[i], SIGNAL(toggled(bool)), sigBoxMapper, SLOT(map()));
        connect(aiBtn[i], SIGNAL(pressed()), sigBtnMapper, SLOT(map()));
    }

    connect(sigBoxMapper, SIGNAL(mapped(int)), this, SLOT(onBoxMapClicked(int)));
    connect(sigBtnMapper, SIGNAL(mapped(int)), this, SLOT(onBtnMapClicked(int)));
    arithAIGroup->setTitle(myLan.arithmetic_list);
    arithAIGroup->setFixedSize(this->width() / 2, this->height() / 2);

    /* page buttons */
    okButton = new myPushButton(myLan.apply, myIcon.Action_Apply, true, true, this);
    okButton->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    cancelButton = new myPushButton(myLan.back, myIcon.Action_Back, true, true, this);
    cancelButton->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(10, 0, 15, 20);
    btnLayout->addStretch(1);
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);

    connect(okButton, SIGNAL(pressed()), this, SLOT(onOkBtnClicked()));
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(onCancelBtnClicked()));

    m_preVboxlayout->addWidget(arithAIGroup, 0, Qt::AlignCenter);
    m_preVboxlayout->addLayout(btnLayout);
}

/*!
 * \brief AIMainWidget::initAIMainWidget
 * \see updateLayerStat()
 */
void AIMainWidget::initAIMainWidget()
{
    m_nLevelToTal = struCnfg.nLevelTotal;
    updateLayerStat();
}

/*终止图像采集过程*/
void AIMainWidget::dropRecieveImageData()
{
    m_bIsImageRecvTerminate = true;
}

/*!
 * \brief AIMainWidget::onOkBtnClicked
 */
void AIMainWidget::onOkBtnClicked()
{
    /* 重置算法使能 */
    myFlow.resetArithmeticEnable();

    /* 重置延迟时间 */
    myFlow.resetEjectTime();
}

/*!
 * \brief AIMainWidget::onCancelBtnClicked
 */
void AIMainWidget::onCancelBtnClicked()
{
    emit goToHomePage();
}

/*!
 * \brief AIMainWidget::onBoxMapClicked
 */
void AIMainWidget::onBoxMapClicked(int index)
{
    if (aiBox[index]->isChecked())
    {
        struCnfp.nArithmeticEnable[ARITH_INTEL_A + index] = 1;
        aiBtn[index]->setEnabled(true);
    }
    else
    {
        struCnfp.nArithmeticEnable[ARITH_INTEL_A + index] = 0;
        aiBtn[index]->setEnabled(false);
    }

    myFlow.resetArithmeticEnable();
}

/*!
 * \brief AIMainWidget::onBtnMapClicked
 */
void AIMainWidget::onBtnMapClicked(int index)
{
    if (m_nIndexOfArithIntel != index)
    {
        m_nIndexOfArithIntel = index;
        for (int j = 0; j < m_nLevelToTal; j++)
        {
            for (int i = 0;i < IMG_MAX_NUM;i++)
            {
                m_nStatOfImg[j][i] = CLASS_NULL;
            }
        }
    }
    struGsh.curIntelArithNo = m_nIndexOfArithIntel;

    struCnfp.nIntelMode = MODE_MULTIPLE;
    m_classModeBtn->hide();
    m_AIStudyBtn->setEnabled(true);
}

/*!
 * \brief AIMainWidget::updateImgList
 * \see updateImgName()
 */
void AIMainWidget::updateImgList()
{
    int i = 0;

    while (i < MAX_PER_LIST)
    {
        displayCurImg(i);
        i++;
    }
}

/*!
 * \brief AIMainWidget::updateLayerStatSlt
 */
void AIMainWidget::updateLayerStatMaize()
{
    /*相机位置变化规则
     *   前下-->后下
     *    |      |
     *    |      |
     *    ↑      ↓
     *   后上<--前上
     */
     /*拍摄图片暂定同时获得上下两排相机的图片故将通道显示范围只定在前、后*/
    if (struGsh.nUnit % 2 == 0)
    {//奇数,当前为前
        m_layerBtn->setText(myLan.front);
    }
    else
    {//偶数,当前为后
        m_layerBtn->setText(myLan.rear);
    }
}

/*!
 * \brief AIMainWidget::updateLayerStatSlt
 */
void AIMainWidget::updateLayerStat()
{

    m_channelGridLayout->removeWidget(m_layerBtn);
    m_channelGridLayout->removeWidget(m_layerBtnAdd);
    m_channelGridLayout->removeWidget(m_plusChanNumBtn);
    m_channelGridLayout->removeWidget(m_chanNum);
    m_channelGridLayout->removeWidget(m_minusChanNUmBtn);

    if (struGsh.nUnit % 2 == 0)
    {
        m_layerBtn->setText(myLan.front);
    }
    else
    {
        m_layerBtn->setText(myLan.rear);
    }
    switch (getUnitGroup(struGsh.nLevel, struGsh.nUnit))
    {
    case 0:
        m_channelGBox->setTitle(myLan.first);
        break;
    case 1:
        m_channelGBox->setTitle(myLan.first);
        break;
    case 2:
        m_channelGBox->setTitle(myLan.second);
        break;
    case 3:
        m_channelGBox->setTitle(myLan.second);
        break;
    case 4:
        m_channelGBox->setTitle(myLan.third);
        break;
    case 5:
        m_channelGBox->setTitle(myLan.third);
        break;
    case 6:
        m_channelGBox->setTitle(myLan.fourth);
        break;
    case 7:
        m_channelGBox->setTitle(myLan.fourth);
        break;
    default:
        break;
    }
}
/*!
 * \brief AIMainWidget::onStackedWidgetIndexChangedSlt
 * \param index ：当前页面索引号
 */
void AIMainWidget::onStackedWidgetIndexChangedSlt(int index)
{
    bool isToCopy = false;
    switch (index)
    {
    case AI_PREPARE_PAGE:
    case AI_MAIN_PAGE:
        titleLabel->setText(QString("%1").arg(myLan.scheme_parameter));
        refreshDisplaySlt();
        break;
    case AI_PREVIEW_PAGE:
        titleLabel->setText(QString("%1 > %2").arg(myLan.scheme_parameter)
            .arg(myLan.ai_analysis_set));
        break;
    case AI_IMAGE_PAGE:
        for (int i = 0; i < MAX_AI; i++)
        {
            if (m_bIsAutoCopy[m_nIndexOfArithIntel][i])
            {
                isToCopy = true;
            }
        }
        m_AIParaAutoCopy->setChecked(isToCopy);

        if (m_nIndexOfArithIntel == 0)
        {
            m_AIParaAutoCopy->setCheckBoxName(myLan.auto_copy + " A->B");
        }
        else
        {
            m_AIParaAutoCopy->setCheckBoxName(myLan.auto_copy + " B->A");
        }
        break;
    case AI_INPUT_PARAMS_PAGE:
        m_AIInputParamsWidget->refreshDisplay();
        break;
    default:
        break;
    }

    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < (m_nIndexOfPage + 1) * MAX_PER_LIST; i++)
    {
        updateImgName(i, false);
    }
}

/*!
 * \brief AIMainWidget::onLayerBtnPressedSltMaize
 * \see updateLayerStat()
 * \see updateImgName(int indexOfImg, bool bFlagSet)
 */
void AIMainWidget::onLayerBtnPressedSltMaize()
{
    //由后下变为前下，玉米界面调整
    if (struGsh.nUnit % 2 == 0)
    {//偶数,当前为前-->后
        struGsh.nUnit += 1;
    }
    else
    {//奇数,当前为后-->前
        struGsh.nUnit -= 1;
    }
    updateChuteLCDStatMaize();
    updateLayerStatMaize();

    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < (m_nIndexOfPage + 1) * MAX_PER_LIST; i++)
    {
        updateImgName(i, false);
    }
}

/*!
 * \brief AIMainWidget::onLayerBtnPressedSlt
 * \see updateLayerStat()
 * \see updateImgName(int indexOfImg, bool bFlagSet)
 */
void AIMainWidget::onLayerBtnPressedSlt()
{
    if (struGsh.nUnit % 2 == 1)
    {
        struGsh.nUnit -= 1;
        m_nLayer = 1;

    }
    else
    {
        struGsh.nUnit += 1;
        m_nLayer = 0;
    }
    updateLayerStat();
    LOG_TRACE_STM("current unit :" << struGsh.nUnit << ",m_nlayer:" << m_nLayer);

    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < (m_nIndexOfPage + 1) * MAX_PER_LIST; i++)
    {
        updateImgName(i, false);
    }
    updateBtnStatus();
}

void AIMainWidget::onLayerBtnPressedSltAdd()
{
    updateLayerStat();
    LOG_TRACE_STM("current unit :" << struGsh.nUnit << ",m_nlayer:" << m_nLayer);
    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < (m_nIndexOfPage + 1) * MAX_PER_LIST; i++)
    {
        updateImgName(i, false);
    }
}
/*!
 * \brief AIMainWidget::onPlusChanNumPressedSltMaize
 */
void AIMainWidget::updateChuteLCDStatMaize()
{
    m_chanNum->display(struGsh.nUnit / 2 + 1);
}
/*!
 * \brief AIMainWidget::onPlusChanNumPressedSltMaize
 */
void AIMainWidget::onPlusChanNumPressedSltMaize()
{
    int plusSpecialNum[4] = { 0 };//4排相机的结尾序号
    int isSpecialNum = 0;
    int tmp = 0;
    tmp = (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal + 2) / 4;//后视单排相机数
    plusSpecialNum[0] = (tmp - 1) * 2;                            //前下
    plusSpecialNum[1] = (tmp - 1) * 2 + 1;                         //后下

    for (int i = 0; i < 2; i++)
    {
        if (struGsh.nUnit == plusSpecialNum[i])
        {
            isSpecialNum = 1;
            break;
        }
    }

    if (isSpecialNum == 0)
    {
        struGsh.nUnit += 2;
        updateChuteLCDStatMaize();
    }
    updateBtnStatus();
}

/*!
 * \brief AIMainWidget::onPlusChanNumPressedSlt
 */
void AIMainWidget::onPlusChanNumPressedSlt()
{
    int IDTotal;

    if (struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal == 1)
    {
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal * 2;
    }
    else
    {
        IDTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
    }
    if (struGsh.nUnit < IDTotal - 2)
    {
        struGsh.nUnit += 2;
    }
    m_chanNum->display(struGsh.nUnit / 2 + 1);

    updateBtnStatus();
}

/*!
 * \brief AIMainWidget::onMinusChanNumPressedSltMaize
 */
void AIMainWidget::onMinusChanNumPressedSltMaize()
{
    int isSpecialNum = 0;
    int minuSpecialNum[4] = { 0 };//4排相机的起始序号
    minuSpecialNum[0] = 0; //前下
    minuSpecialNum[1] = 1; //后下

    for (int i = 0; i < 2; i++)
    {
        if (struGsh.nUnit == minuSpecialNum[i])
        {
            isSpecialNum = 1;
            break;
        }
    }

    if (isSpecialNum == 0)
    {
        struGsh.nUnit -= 2;
        updateChuteLCDStatMaize();
    }
}

/*!
 * \brief AIMainWidget::onMinusChanNumPressedSlt
 */
void AIMainWidget::onMinusChanNumPressedSlt()
{
    if (struGsh.nUnit > 1)
    {
        struGsh.nUnit -= 2;
    }
    m_chanNum->display(struGsh.nUnit / 2 + 1);

    updateBtnStatus();
}

/*!
 * \brief AIMainWidget::onExportImgBtnPressedSlt
 */
void AIMainWidget::onExportImgBtnPressedSlt()
{
    m_stackedWidget->setCurrentIndex(AI_IMAGE_PAGE);
}

/*!
 * \brief AIMainWidget::onCalSetBtnSlt
 * \see updateImgName(int indexOfImg, bool bFlagSet)
 * \see updateBtnStatus()
 */
void AIMainWidget::onCalSetBtnSlt()
{
    m_bCalSet = !m_bCalSet;


    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < (m_nIndexOfPage + 1) * MAX_PER_LIST; i++)
    {
        updateImgName(i, false);
    }
    updateBtnStatus();
}
/*!
 * \brief AIMainWidget::onClassModeBtnPressedSlt
 * \see updateBtnStatus()
 */
void AIMainWidget::onClassModeBtnPressedSlt()
{
    switch (struCnfp.nIntelMode)
    {
    case MODE_MULTIPLE:
        struCnfp.nIntelMode = MODE_NULL;
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 0;
        break;
    case MODE_NULL:
        struCnfp.nIntelMode = MODE_MULTIPLE;
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 1;
        break;
    default:
        struCnfp.nIntelMode = MODE_NULL;
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 0;
        break;
    }
    for (int j = 0; j < m_nLevelToTal; j++)
    {
        for (int i = 0;i < IMG_MAX_NUM;i++)
        {
            m_nStatOfImg[j][i] = CLASS_NULL;
        }
    }
    for (int i = 0; i < MAX_PER_LIST;i++)
    {
        updateImgName(i + m_nIndexOfPage * MAX_PER_LIST, false);
    }

    updateBtnStatus();
}


void AIMainWidget::onAIParamsSetBtnPressedSlt()
{
    updateAIParamsPage();
    m_stackedWidget->setCurrentIndex(AI_PARAMS_SET_PAGE);
}

/*!
 * \brief AIMainWidget::multipleClassAICalForSoleView
 * \see usualClassAICal();
 * \see multipleClassAICal()
 */
int AIMainWidget::multipleClassAICalForSoleView(int indexOfTrain)
{
    int retCode = enableAICal();
    if (retCode == SUCCEED)
    {
        QStringList viewNameString;
        viewNameString << myLan.upper_master << myLan.lower_master << myLan.upper_slave << myLan.lower_slave;
        if (m_bIsAutoGetData)
        {
            myMessageBox msgBox(MSG_QUES, viewNameString.at(m_nLayer) + ":" + myLan.ai_cal_warn);
            int ret = msgBox.exec();
            if (ret == QDialog::Accepted)
            {
                multipleClassAICal(indexOfTrain);
            }
            else
            {
                retCode = ERR_NO_CAL;
            }
        }
        else
        {
            multipleClassAICal(indexOfTrain);
        }
    }
    return retCode;
}

/*!
 * \brief AIMainWidget::displayCalResult
 * \see displayCalResult();
 */
void AIMainWidget::displayCalResult(QList<int>& list)
{
    /* list链表中依次递增存放各背景组的智能计算结果 */
    QString infoText;
    QStringList viewNameString;
    viewNameString << myLan.upper_master << myLan.lower_master << myLan.upper_slave << myLan.lower_slave;

    for (int i = 0; i < list.count(); i++)
    {
        infoText.append(viewNameString.at(i) + ": ");
        switch (list.at(i))
        {
        case ERR_NO_BAD:
            infoText.append(myLan.ai_err_no_bad + "\n");
            break;
        case ERR_NO_GOOD:
            infoText.append(myLan.ai_err_no_good + "\n");
            break;
        case ERR_OVERRUN:
            infoText.append(myLan.ai_err_overflow + "\n");
            break;
        case ERR_NO_SET:
            infoText.append(myLan.ai_err_no_set + "\n");
            break;
        case SUCCEED:
            infoText.append(myLan.upgrade_ok + "\n");
            break;
        case ERR_NO_CAL:
            infoText.append(myLan.upgrade_untreated + "\n");
            break;
        default:
            infoText.append(myLan.unknown + "\n");
            break;
        }
    }

    myMessageBox msgBox(MSG_TIPS, infoText);
    msgBox.exec();
}

/*!
 * \brief AIMainWidget::onAIStudyBtnPressedSlt
 * \see usualClassAICal();
 * \see multipleClassAICal()
 */
void AIMainWidget::onAIStudyBtnPressedSlt()
{

    int indexOfTrain = m_nIndexOfArithIntel;
    m_nRGBNum = 0;
    if (enableAICal() == SUCCEED)
    {
        if (m_bIsAutoGetData)
        {
            myMessageBox msgBox(MSG_QUES, myLan.ai_cal_warn);
            int ret = msgBox.exec();
            if (ret == QDialog::Rejected)
            {
                return;
            }
        }

        if (struCnfp.nIntelMode == MODE_USUAL)
        {
            usualClassAICal();
        }
        else
        {
            multipleClassAICal(indexOfTrain);
        }
    }
}

/*!
 * \brief AIMainWidget::getAllUnitsForOneChuteForII
 * \see getAllUnitsAddrForOneChute();
 */
void AIMainWidget::getAllUnitsAddrForOneChuteForII(QList <int>& qlnUnit, int nUnitElem)
{
    if (nUnitElem < struGsh.nDevTypeAddrBias)
    {
        if ((nUnitElem & 0x01) == 0x01)
        {   //奇数地址
            if ((struCnfe.nDerivedDevType & 0x01) != 0)
            { //有前视可见相机
                qlnUnit << nUnitElem - 1;
            }
            if ((struCnfe.nDerivedDevType & 0x02) != 0)
            { //有后视可见相机
                qlnUnit << nUnitElem;
            }
            if ((struCnfe.nDerivedDevType & 0x0100) != 0)
            { //有前视红外相机
                qlnUnit << nUnitElem - 1 + struGsh.nDevTypeAddrBias;
            }
            if ((struCnfe.nDerivedDevType & 0x0200) != 0)
            { //有后视红外相机
                qlnUnit << nUnitElem + struGsh.nDevTypeAddrBias;
            }
        }
        else
        {                            //偶数地址
            if ((struCnfe.nDerivedDevType & 0x01) != 0)
            { //有前视可见相机
                qlnUnit << nUnitElem;
            }
            if ((struCnfe.nDerivedDevType & 0x02) != 0)
            { //有后视可见相机
                qlnUnit << nUnitElem + 1;
            }
            if ((struCnfe.nDerivedDevType & 0x0100) != 0)
            { //有前视红外相机
                qlnUnit << nUnitElem + struGsh.nDevTypeAddrBias;
            }
            if ((struCnfe.nDerivedDevType & 0x0200) != 0)
            { //有后视红外相机
                qlnUnit << nUnitElem + 1 + struGsh.nDevTypeAddrBias;
            }
        }
    }
    else
    {
        if ((nUnitElem & 0x01) == 0x01)
        {   //奇数地址
            if ((struCnfe.nDerivedDevType & 0x01) != 0)
            { //有前视可见相机
                qlnUnit << nUnitElem - 1 - struGsh.nDevTypeAddrBias;
            }
            if ((struCnfe.nDerivedDevType & 0x02) != 0)
            { //有后视可见相机
                qlnUnit << nUnitElem - struGsh.nDevTypeAddrBias;
            }
            if ((struCnfe.nDerivedDevType & 0x0100) != 0)
            { //有前视红外相机
                qlnUnit << nUnitElem - 1;
            }
            if ((struCnfe.nDerivedDevType & 0x0200) != 0)
            { //有后视红外相机
                qlnUnit << nUnitElem;
            }
        }
        else
        {                            //偶数地址
            if ((struCnfe.nDerivedDevType & 0x01) != 0)
            { //有前视可见相机
                qlnUnit << nUnitElem - struGsh.nDevTypeAddrBias;
            }
            if ((struCnfe.nDerivedDevType & 0x02) != 0)
            { //有后视可见相机
                qlnUnit << nUnitElem + 1 - struGsh.nDevTypeAddrBias;
            }
            if ((struCnfe.nDerivedDevType & 0x0100) != 0)
            { //有前视红外相机
                qlnUnit << nUnitElem;
            }
            if ((struCnfe.nDerivedDevType & 0x0200) != 0)
            { //有后视红外相机
                qlnUnit << nUnitElem + 1;
            }
        }
    }
}

/*!
 * \brief AIMainWidget::getAllUnitsAddrForOneChuteForOneViewPerLayer
 * \see getAllUnitsAddrForOneChute();
 */
void AIMainWidget::getAllUnitsAddrForOneChuteForOneViewPerLayer(QList <int>& qlnUnit, int nUnitElem)
{
    qlnUnit << nUnitElem;
}

/*!
 * \brief AIMainWidget::getAllUnitsAddrForOneChuteForTwoViewPerLayer
 * \see getAllUnitsAddrForOneChute();
 */
void AIMainWidget::getAllUnitsAddrForOneChuteForTwoViewPerLayer(QList <int>& qlnUnit, int nUnitElem)
{
    if ((nUnitElem & 0x01) == 0x01)
    {   //奇数地址
        qlnUnit << nUnitElem - 1;
        qlnUnit << nUnitElem;
    }
    else
    {                            //偶数地址
        qlnUnit << nUnitElem;
        qlnUnit << nUnitElem + 1;
    }
}

/*!
 * \brief AIMainWidget::getAllUnitsAddrForOneChuteForFourViewPerLayer
 * \see getAllUnitsAddrForOneChute();
 */
void AIMainWidget::getAllUnitsAddrForOneChuteForFourViewPerLayer(QList <int>& qlnUnit, int nUnitElem)
{
    if ((nUnitElem & 0x01) == 0x01)
    {   //奇数地址
        qlnUnit << nUnitElem - 1;
        qlnUnit << nUnitElem;
    }
    else
    {                            //偶数地址
        qlnUnit << nUnitElem;
        qlnUnit << nUnitElem + 1;
    }
}

/*!
 * \brief AIMainWidget::getAllUnitsForOneChute
 * \see setCaptureAttr();
 */
void AIMainWidget::getAllUnitsAddrForOneChuteForFourViewForRSC(QList<int>& qlnUnit, int nUnitElem)
{
    if ((nUnitElem & 0x01) == 0x01)
    {   //奇数地址
        qlnUnit << nUnitElem - 1;//前视主配
        qlnUnit << nUnitElem;//后视主配
    }
    else
    {                            //偶数地址
        qlnUnit << nUnitElem;
        qlnUnit << nUnitElem + 1;
    }
}

/*!
 * \brief AIMainWidget::getAllUnitsForOneChute
 * \see setCaptureAttr();
 */
void AIMainWidget::getAllUnitsAddrForOneChute(QList <int>& qlnUnit, int nUnitElem)
{
    getAllUnitsAddrForOneChuteForTwoViewPerLayer(qlnUnit, nUnitElem);
}

/*!
 * \brief AIMainWidget::getUnitViewNum
 * \see setCaptureAttr();
 */
int AIMainWidget::getUnitViewNum(int nUnit)
{
    int viewNum = VIEW_ZERO;
    if ((nUnit & 0x01) == 0x01)
    {
        viewNum = VIEW_ONE;
    }
    else
    {
        viewNum = VIEW_ZERO;
    }
    return viewNum;
}

void AIMainWidget::autoCopyIntellArgs()
{
    int levelNum = 0;
    int groupNum = 0;

    for (int i = 0; i < MAX_AI; i++)
    {
        if (i != m_nIndexOfArithIntel)
        {
            if (m_bIsAutoCopy[m_nIndexOfArithIntel][i])
            {
                myFlow.printArgsForTest(m_nIndexOfArithIntel);
                for (levelNum = 0; levelNum < struCnfg.nLevelTotal; levelNum++)
                {
                    for (groupNum = 0; groupNum < struCnfg.struLevelInfo[ONE_LEVEL].nIdentifyGroupTotal; groupNum++)
                    {
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nConsDfl[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nConsDfl[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nConsDfl[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nConsDfl[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoR[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoR[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoR[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoR[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoG[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoG[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoG[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoG[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoB[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoB[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoB[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoB[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoRG[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoRG[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoRG[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoRG[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoRB[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoRB[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoRB[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoRB[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoGB[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoGB[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoGB[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoGB[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoRR[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoRR[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoRR[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoRR[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoGG[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoGG[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoGG[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoGG[1];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoBB[0] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoBB[0];
                        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[i].nQuoBB[1] = struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[m_nIndexOfArithIntel].nQuoBB[1];
                    }
                }
                myFlow.printArgsForTest(i);
            }
        }
    }
}

/*!
 * \brief AIMainWidget::getUnitCameraType
 * \see getUnitProtocal();
 */
int AIMainWidget::getUnitCameraType(int nUnit)
{
    int cameraType = CAMERA_COLOR;

    return cameraType;
}

/*!
 * \brief AIMainWidget::getUnitProtocal
 * \see setCaptureAttr();
 */
int AIMainWidget::getUnitProtocal(int nUnit)
{
    int proto = PROTOCAL_0;

    int cameraType = getUnitCameraType(nUnit);

    if (cameraType == CAMERA_COLOR)
    {
        if (struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_1024)
        {
            proto = PROTOCAL_0;
        }
        else if (struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_2048
            || struCnfc.nSensorPixelLevel[struGsh.nLevel] == SENSOR_PIXELS_2592)
        {
            proto = PROTOCAL_1;
        }
    }
    else if (cameraType == CAMERA_INFRA)
    {
        proto = PROTOCAL_2;
    }
    else if (cameraType == CAMERA_ASSIST)
    {
        proto = PROTOCAL_3;
    }
    if (struCnfg.aiEnable == 1)
    {
        proto = PROTOCAL_4;
    }

    return proto;
}

/*!
 * \brief AIMainWidget::getDataLenByProtocal
 * \see setCaptureAttr();
 */
void AIMainWidget::setDataLenByProtocal(int proto, int& dataLenTotal, int& dataDepthth)
{
    /* 该函数获得的是原始数据尺寸 */
    switch (proto)
    {
    case PROTOCAL_0:
        dataDepthth = IMAGE_HEIGHT;
        dataLenTotal = dataDepthth * (IMAGE_WIDTH * 3 + INF_WIDTH + FRAME_LEN_NOT_DATA);//1,800,000,1.7M
        break;
    case PROTOCAL_1:
        dataDepthth = IMAGE_HEIGHT;
        dataLenTotal = dataDepthth * (IMAGE_WIDTH * 2 * 3 + FRAME_LEN_NOT_DATA);//3,080,000,2.9M
        break;
    case PROTOCAL_2:
        dataDepthth = INF_HEIGHT;
        dataLenTotal = dataDepthth * (INF_WIDTH * 2 + FRAME_LEN_NOT_DATA);//260,000,0.24M
        break;
    case PROTOCAL_3:
        dataDepthth = IMAGE_HEIGHT;
        dataLenTotal = dataDepthth * (IMAGE_WIDTH * 2 + FRAME_LEN_NOT_DATA);//1032000,0.98M
        break;
    case PROTOCAL_4:
        dataDepthth = IMAGE_HEIGHT;
        dataLenTotal = dataDepthth * ((IMAGE_WIDTH - 4) * 3);//1536000,1.46M 仅是图像的长度
        break;
    default:
        dataDepthth = 0;
        dataLenTotal = 0;
        break;
    }
}

/*!
 * \brief AIMainWidget::setCaptureAttr
 * \see getImage();
 */
void AIMainWidget::setCaptureAttr(QList <CaptureConfig>& qlCaptureConfig, int nUnitElem)
{
    CaptureConfig config;
    QList<int> qlnUnit;

    qlnUnit.clear();
    if (m_bIsCaptureAllView)
    {
        getAllUnitsAddrForOneChute(qlnUnit, nUnitElem);
    }
    else
    {
        qlnUnit << nUnitElem;
    }

    config.nSeq = 0;
    foreach(int nUnit, qlnUnit)
    {
        config.nUnitAddr = nUnit;
        config.nSeq++;
        config.nTransType = struGsh.nIntelSampType;
        config.nViewNum = getUnitViewNum(nUnit);                     // 获取相机相应背景组组号
        config.nProtoType = getUnitProtocal(nUnit);                    // 获取相机的数据传输协议
        setDataLenByProtocal(config.nProtoType, config.dataLen, config.dataDepth);    // 根据协议类型设置数据包的长度
        config.result = ERR_SUCCESS;                                                // 初始结果为成功
        qlCaptureConfig << config;
    }

    //debug
#if 1
    printf("qlCaptureConfig list:\n");
    foreach(CaptureConfig tmp, qlCaptureConfig)
    {
        printf("unit:%2d, seq:%d, result:%d, trans:%d, view:%d, proto:%d, dateLen:%d, dataDepth:%d.\n",
            tmp.nUnitAddr, tmp.nSeq, tmp.result, tmp.nTransType, tmp.nViewNum, tmp.nProtoType,
            tmp.dataLen, tmp.dataDepth);
    }
    fflush(0);
#endif
}

/*!
 * \brief AIMainWidget::startCapture
 * \see getImage();
 */
void AIMainWidget::startCapture(QList <CaptureConfig>& qlCaptureConfig)
{
    bool isUSBInit = false;
    int nBegin = m_beginChLe->text().toInt();
    int nEnd = m_endChLe->text().toInt();

    foreach(CaptureConfig tmp, qlCaptureConfig)
    {
        if (tmp.nTransType == 1)
        {  //USB需要初始化
//            if (isUSBInit) {
//                if (qlCaptureConfig.first().result == ERR_USB_INIT) {
//                    tmp.result = ERR_USB_INIT;
//                }
//            }

//            if(!isUSBInit) {
//#ifdef Q_OS_UNIX
//                if(myUsbDev != NULL){
//                    usb.myusbFree();
//                }
//                usb.myusbInit();
//                if(myUsbDev == NULL){
//                    tmp.result = ERR_USB_INIT;
//                }
//#endif
//                isUSBInit = true;
//            }
            qlCaptureConfig.replace(tmp.nSeq - 1, tmp);
        }
    }

    foreach(CaptureConfig tmp, qlCaptureConfig)
    {
        QByteArray args;
        args.clear();
        args[0] = 0;
        //清空读图片缓冲区
        int ret1 = MyUpd.clear_udp_buffer(MyUpd.getSockfd());
        std::cout << "已清空缓冲区字节" << ret1 << " 字节" << std::endl;

        MyUpd.writeDatagram(CMD_AI_IMG_VID_UPLOAD, tmp.nUnitAddr / 2, 1, args, struGsh.addressList.at(tmp.nUnitAddr / 2), AI_UDP_SEND_PORT);
        myFlow.msleep(50);

        if (tmp.result == ERR_USB_INIT)
        { //USB初始化失败
            return;
        }
        MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, tmp.nUnitAddr, 0, 0, (tmp.dataDepth - 1) / 256, (tmp.dataDepth - 1) % 256, tmp.nTransType, 3);
        MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, tmp.nUnitAddr, 0, m_bIsCuteOffBg, 0x01, struGsh.nIntelSampBurst / 256, struGsh.nIntelSampBurst % 256, 1);
        if (myFlow.getProductLineNo() != 0)
        {
            MySerial.com1Write(CMD_UNIT_CAPTURE_RANGE, UNIT, struGsh.nLevel, tmp.nUnitAddr, 0, nBegin / 256, nBegin % 256, nEnd / 256, nEnd % 256, 1);
            MySerial.com1Write(2, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
        }
        MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, tmp.nUnitAddr, 0, 0, tmp.nTransType, (tmp.dataDepth - 1) / 256, (tmp.dataDepth - 1) % 256, 1);
    }
}

/*!
 * \brief AIMainWidget::checkFrame
 * \see processImageDataFromCamera();
 */
int AIMainWidget::checkFrame(const char* data, int dataLen)
{
    int retCode = ERR_SUCCESS;
    if (data == NULL)
    {
        printf("Frame is NULL.\n");
        retCode = ERR_INPUT_ARG;
        goto EXIT;
    }

    if (dataLen < FRAME_LEN_NOT_DATA)
    {
        printf("Frame length is too short.\n");
        retCode = ERR_INPUT_ARG;
        goto EXIT;
    }

    if ((data[0] == 0xAA) && (data[1] == 0xAA) && (data[2] == 0xAA) && (data[3] == 0xAA))
    { //目前只需判断数据的头部
        retCode = ERR_SUCCESS;
    }
    else
    {
        printf("check head:\n");
        for (int i = 0; i < dataLen; i++)
        {
            printf("%02x ", data[i]);
        }
        printf("\n");
        retCode = ERR_DATA;
    }

EXIT:
    return retCode;
}


/*!
 * \brief AIMainWidget::stripFrameData
 * \see stripFrameData();
 */
int AIMainWidget::swapData(char* data, int blockLen, int blockCount)
{
    int  retCode = ERR_SUCCESS;
    char* tmp = NULL;
    if (data == NULL)
    {
        printf("input data error.\n");
        retCode = ERR_INPUT_ARG;
        goto EXIT;
    }

    tmp = (char*)malloc(blockLen);
    if (tmp == NULL)
    {
        printf("malloc failed for swap[swapData].\n");
        retCode = ERR_MALLOC_FAILED;
        goto EXIT;
    }

    for (int i = 0; i < blockCount; i++)
    {
        memcpy(tmp, data + blockLen * i, blockLen);
        memcpy(data + blockLen * i, data + blockLen * (blockCount - i - 1), blockLen);
        memcpy(data + blockLen * (blockCount - i - 1), tmp, blockLen);
    }

EXIT:
    if (tmp != NULL)
    {
        free(tmp);
        tmp = NULL;
    }
    return retCode;
}
/*!
 * \brief AIMainWidget::stripFrameData
 * \see processImageDataFromCamera();
 */

 /*!
  * \brief AIMainWidget::getAutoBackgroudData();
  * \see stripFrameData();
  *input:
  */
int AIMainWidget::getAutoBackgroundData(char* data, CaptureConfig& captureConfig)
{
    int retCode = 0;
    char* pTmp;
    char* backgroundMaxAll = NULL;
    char* backgroundMinAll = NULL;
    backgroundMaxAll = (char*)malloc(IMAGE_WIDTH * 3 * 2);
    if (backgroundMaxAll == NULL)
    {
        printf("malloc failed for getAutoBackgroundData[backgroundMaxAll].\n");
        captureConfig.result = ERR_MALLOC_FAILED;
        retCode = -1;
        goto EXIT;
    }
    backgroundMinAll = (char*)malloc(IMAGE_WIDTH * 3 * 2);
    if (backgroundMinAll == NULL)
    {
        printf("malloc failed for getAutoBackgroundData[backgroundMinAll].\n");
        captureConfig.result = ERR_MALLOC_FAILED;
        retCode = -1;
        goto EXIT;
    }

    printf("get background data from USB image");
    //取自动背景最大值(2048像素)
    pTmp = data + 10;
    memcpy(backgroundMaxAll, data + 10 + (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA), IMAGE_WIDTH * 3 * 2);
    for (int n = 0; n < IMAGE_WIDTH * 3 * 2; n++)
    {
        if (pTmp[n] != backgroundMaxAll[n])
        {
            printf("AutoBackground Max data check failed.\n");
            captureConfig.result = ERR_AUTO_BACKGROUND_CHECKERROR;
            retCode = 1;
            goto EXIT;
        }
    }

    //取自动背景最小值(2048像素)
    pTmp = data + 10 + 2 * (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA);
    memcpy(backgroundMinAll, data + 10 + 3 * (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA),
        IMAGE_WIDTH * 3 * 2);
    for (int n = 0; n < IMAGE_WIDTH * 3 * 2; n++)
    {
        if (pTmp[n] != backgroundMinAll[n])
        {
            printf("AutoBackground Max data check failed.\n");
            captureConfig.result = ERR_AUTO_BACKGROUND_CHECKERROR;
            retCode = 1;
            goto EXIT;
        }
    }

    memcpy(myAIShare.m_backMax, backgroundMaxAll, IMAGE_WIDTH * 3);
    memcpy(myAIShare.m_backMin, backgroundMinAll, IMAGE_WIDTH * 3);


EXIT:
    fflush(0);
    if (backgroundMaxAll != NULL)
    {
        free(backgroundMaxAll);
        backgroundMaxAll = NULL;
    }
    if (backgroundMinAll != NULL)
    {
        free(backgroundMinAll);
        backgroundMinAll = NULL;
    }
    return retCode;
}

int AIMainWidget::stripFrameData(char* data, CaptureConfig& captureConfig)
{
    /*
    * PROTOCAL_0,strip之后数据格式为RGB(1024*3*500)+INFRA(512*250)
    * PROTOCAL_1,strip之后数据格式为RGB(1024*3*500)
    * PROTOCAL_2,strip之后数据格式为RG(512*2*250)
    */
    char* dataOther = NULL;
    char* dataTmp = NULL;
    int   pBias = 0;
    int  retCode = 0;
    int beginCh = 0;

    if (data == NULL)
    {
        printf("input data error.\n");
        captureConfig.result = ERR_INPUT_ARG;
        retCode = -1;
        goto EXIT;
    }

    if (captureConfig.nProtoType == PROTOCAL_0)
    {   //1024(R+G+B+Infra),500帧
        dataTmp = (char*)malloc(IMAGE_WIDTH * 3 + INF_WIDTH);
        if (dataTmp == NULL)
        {
            printf("malloc failed for strip[stripFrameData].\n");
            captureConfig.result = ERR_MALLOC_FAILED;
            retCode = -1;
            goto EXIT;
        }

        for (int i = 0; i < captureConfig.dataDepth; i++)
        {
            memcpy(dataTmp, data + 10 + i * (IMAGE_WIDTH * 3 + INF_WIDTH + FRAME_LEN_NOT_DATA), IMAGE_WIDTH * 3 + INF_WIDTH);
            memcpy(data + i * (IMAGE_WIDTH * 3 + INF_WIDTH), dataTmp, IMAGE_WIDTH * 3 + INF_WIDTH);
        }
        for (int i = 0; i < captureConfig.dataDepth; i++)
        {
            memcpy(dataTmp, data + i * (IMAGE_WIDTH * 3 + INF_WIDTH), IMAGE_WIDTH * 3);
            memcpy(data + i * IMAGE_WIDTH * 3, dataTmp, IMAGE_WIDTH * 3);
        }

        if ((captureConfig.result = swapData(data, IMAGE_WIDTH * 3, captureConfig.dataDepth)) != ERR_SUCCESS)
        {
            retCode = -1;
            goto EXIT;
        }

        if (dataOther != NULL)
        {
            memcpy(data + IMAGE_WIDTH * captureConfig.dataDepth * 3, dataOther, captureConfig.dataDepth / 2 * INF_WIDTH);
        }
    }
    else if (captureConfig.nProtoType == PROTOCAL_1)
    {
        dataTmp = (char*)malloc(IMAGE_WIDTH * 3);
        if (dataTmp == NULL)
        {
            printf("malloc failed for strip[stripFrameData].\n");
            captureConfig.result = ERR_MALLOC_FAILED;
            retCode = -1;
            goto EXIT;
        }


        pBias = IMAGE_WIDTH * 3;
        beginCh = struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[captureConfig.nUnitAddr];
        int firstRowNum = 0;
        for (int k = beginCh; k < IMAGE_WIDTH; k++)
        {
            uchar r = data[k * 3 + 10 + firstRowNum * (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA)];
            uchar g = data[k * 3 + 10 + 1 + firstRowNum * (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA)];
            uchar b = data[k * 3 + 10 + 2 + firstRowNum * (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA)];

            bool flag = true;
            flag = pixelIsBackground(r, g, b, captureConfig.nUnitAddr);

            if (!flag)
            {  //物料点数据在前半部分
                pBias = 0;
                break;
            }
        }
        for (int i = 0; i < captureConfig.dataDepth; i++)
        {
            memcpy(dataTmp, data + 10 + pBias + i * (IMAGE_WIDTH * 3 * 2 + FRAME_LEN_NOT_DATA), IMAGE_WIDTH * 3);
            memcpy(data + i * IMAGE_WIDTH * 3, dataTmp, IMAGE_WIDTH * 3);
        }

        if ((captureConfig.result = swapData(data, IMAGE_WIDTH * 3, captureConfig.dataDepth)) != ERR_SUCCESS)
        {
            retCode = -1;
            goto EXIT;
        }

    }
    else if (captureConfig.nProtoType == PROTOCAL_2)
    {
        dataTmp = (char*)malloc(INF_WIDTH * 2);
        if (dataTmp == NULL)
        {
            printf("malloc failed for strip[stripFrameData].\n");
            captureConfig.result = ERR_MALLOC_FAILED;
            retCode = -1;
            goto EXIT;
        }

        for (int i = 0; i < captureConfig.dataDepth; i++)
        {
            memcpy(dataTmp, data + i * (INF_WIDTH * 2 + FRAME_LEN_NOT_DATA) + 10, INF_WIDTH * 2);
            memcpy(data + i * INF_WIDTH * 2, dataTmp, INF_WIDTH * 2);
        }

        if ((captureConfig.result = swapData(data, INF_WIDTH * 2, captureConfig.dataDepth)) != ERR_SUCCESS)
        {
            retCode = -1;
            goto EXIT;
        }
    }
    else if (captureConfig.nProtoType == PROTOCAL_3)
    {
        for (int i = 0; i < captureConfig.dataDepth; i++)
        {//红外数据一帧2048,有效数据1024，奇偶是重复的。
            for (int j = 0;j < IMAGE_WIDTH; j++)
            {
                data[i * IMAGE_WIDTH + j] = data[10 + i * (IMAGE_WIDTH * 2 + FRAME_LEN_NOT_DATA) + j * 2];//取偶数部分1024个有效数据
            }
        }
        if ((captureConfig.result = swapData(data, IMAGE_WIDTH, captureConfig.dataDepth)) != ERR_SUCCESS)
        {
            retCode = -1;
            goto EXIT;
        }
    }
EXIT:
    if (dataOther != NULL)
    {
        free(dataOther);
        dataOther = NULL;
    }
    if (dataTmp != NULL)
    {
        free(dataTmp);
        dataTmp = NULL;
    }
    return retCode;
}

/*!
 * \brief AIMainWidget::displayImgInfo
 * \see processImageDataFromCamera();
 */
void AIMainWidget::displayImgInfo(char* data, CaptureConfig& captureConfig)
{
    QString imageName;
    char* pDataTmp = NULL;

    /*
    * PROTOCAL_0,strip之后数据格式为RGB(1024*3*500)+INFRA(512*250)
    * PROTOCAL_1,strip之后数据格式为RGB(1024*3*500)
    * PROTOCAL_2,strip之后数据格式为RG(512*2*250)
    */

    imageName = QString("%1.png").arg(m_nNumOfImg);
    if (captureConfig.nProtoType == PROTOCAL_0)
    {
        saveBMPFile(imageName, IMAGE_WIDTH, captureConfig.dataDepth, (uchar*)data);
    }
    else if (captureConfig.nProtoType == PROTOCAL_1)
    {
        saveBMPFile(imageName, IMAGE_WIDTH, captureConfig.dataDepth, (uchar*)data);
    }
    else if (captureConfig.nProtoType == PROTOCAL_2)
    {
        pDataTmp = (char*)malloc(IMAGE_WIDTH * captureConfig.dataDepth * 2 * 3);   //新开辟的内存用于图像文件的保存和显示
        if (pDataTmp == NULL)
        {
            printf("malloc for protocal_2 failed[displayImgInfo].\n");
            captureConfig.result = ERR_MALLOC_FAILED;
            return;
        }
        for (int i = 0; i < captureConfig.dataDepth; i++)
        {
            for (int j = 0; j < INF_WIDTH; j++)
            {
                for (int m = 0; m < 2; m++)
                {
                    for (int n = 0; n < 2; n++)
                    {
                        int nRow = 2 * i + m;
                        int nCol = 2 * j + n;
                        pDataTmp[(nRow * IMAGE_WIDTH + nCol) * 3] = data[(i * INF_WIDTH + j) * 2];
                        pDataTmp[(nRow * IMAGE_WIDTH + nCol) * 3 + 1] = data[(i * INF_WIDTH + j) * 2 + 1];
                        pDataTmp[(nRow * IMAGE_WIDTH + nCol) * 3 + 2] = 0;
                    }
                }
            }
        }
        saveBMPFile(imageName, IMAGE_WIDTH, captureConfig.dataDepth * 2, (uchar*)pDataTmp);
        if (pDataTmp != NULL)
        {
            free(pDataTmp);
            pDataTmp = NULL;
        }
    }
    else if (captureConfig.nProtoType == PROTOCAL_3)
    {
        saveBMPFileForMono(imageName, IMAGE_WIDTH, captureConfig.dataDepth, (uchar*)data);
    }
    else if (captureConfig.nProtoType == PROTOCAL_4)
    {
        saveBMPFile(imageName, IMAGE_WIDTH - 4, captureConfig.dataDepth, (uchar*)data);
    }

    if (m_nNumOfImg / MAX_PER_LIST == m_nIndexOfPage)
    {    //本页有显示格则显示
        displayCurImg(m_nNumOfImg % MAX_PER_LIST);
    }

    // m_nLayer = captureConfig.nViewNum;
    m_nViewNo[m_nNumOfImg] = captureConfig.nViewNum;

    if (m_nNumOfImg < MAX_IMG_NUM)
    {                      //更新显示照片个数的全局变量
        m_nNumOfImg++;
    }
    updateBtnStatus();
}

/*!
 * \brief AIMainWidget::processImageDataFromCameraByUART
 * \see getImage();
 */
void AIMainWidget::processImageDataFromCameraByUART(CaptureConfig& captureConfig)
{
    char* data = NULL;
    char* pDataTmp = NULL;
    int dataLenPerFrame = 0;
    int count = 0;
    int retLen = 0;
    int maxTimes = 200;             //获取一帧数据的最大尝试获取次数
    int tryTimes = 0;               //一帧数据的尝试获取次数
    int stripResult = 0;
    if (captureConfig.dataLen == 0)
    {
        printf("protocal data length error [0]\n");
        captureConfig.result = ERR_CONFIG;
        goto EXIT;
    }

    dataLenPerFrame = captureConfig.dataLen / captureConfig.dataDepth;
    data = (char*)malloc(captureConfig.dataLen);
    if (data == NULL)
    {
        printf("malloc failed[processImageDataFromCameraByUART]\n");
        captureConfig.result = ERR_MALLOC_FAILED;
        goto EXIT;
    }
    memset(data, 0, captureConfig.dataLen);

    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, captureConfig.nUnitAddr, 0, 0, 0, 0, 0, 1); // 主相机板传输图像

    // 先处理第0帧数据
    tryTimes = 0;
    count = 0;
    infoWidget->setLabelText(myLan.ai_image_capturing + QString("%1(%2%)").arg(captureConfig.nSeq).arg(100 / captureConfig.dataDepth));
    pDataTmp = data;
    while (count < dataLenPerFrame)
    {
        if (tryTimes++ < maxTimes)
        {
            retLen = MySerial.com1Read(pDataTmp, dataLenPerFrame - count);
            if (retLen > 0)
            {
                count += retLen;
                pDataTmp += count;
            }
            myFlow.msleep(100);
        }
        else
        {
            break;
        }
    }

    if (count == dataLenPerFrame)
    {
        if ((captureConfig.result = checkFrame(data, dataLenPerFrame)) != ERR_SUCCESS)
        {
            if (captureConfig.result == ERR_DATA)
            {
                printf("frame[0] style error.\n");
            }
            goto EXIT;
        }
    }
    else
    {
        printf("timeout for frame[0].\n");
        captureConfig.result = ERR_TIMEOUT;
        goto EXIT;
    }

    // 处理后续第(1.2.3..)帧数据
    for (int i = 1; i < captureConfig.dataDepth; i++)
    {
        tryTimes = 0;
        count = 0;
        pDataTmp = data + i * dataLenPerFrame;
        while (count < dataLenPerFrame)
        {
            if (tryTimes++ < maxTimes)
            {    //后续每帧数据的传输时间最长为4s.
                retLen = MySerial.com1Read(pDataTmp, dataLenPerFrame - count);
                if (retLen > 0)
                {
                    count += retLen;
                    pDataTmp += retLen;
                }
                myFlow.msleep(20);
            }
            else
            {
                break;
            }
        }
        if (count == dataLenPerFrame)
        {
            if ((captureConfig.result = checkFrame(data + i * dataLenPerFrame, dataLenPerFrame)) != ERR_SUCCESS)
            {
                if (captureConfig.result == ERR_DATA)
                {
                    printf("frame[%d] style error.\n", i);
                }
                goto EXIT;
            }
        }
        else
        {
            printf("timeout for frame[%d].\n", i);
            captureConfig.result = ERR_TIMEOUT;
            goto EXIT;
        }
        infoWidget->setLabelText(myLan.ai_image_capturing + QString("%1(%2%)").arg(captureConfig.nSeq).arg(i * 100 / captureConfig.dataDepth));
    }

    //将data内数据整理成纯粹的像素数据
    stripResult = stripFrameData(data, captureConfig);
    if (stripResult == 1)
    {
        infoWidget->setLabelText(myLan.msg_auto_bgk_ckeck_error);
        infoWidget->showSecs();
        goto EXIT;
    }
    else if (stripResult == -1)
    {
        infoWidget->setLabelText(myLan.msg_strip_frame_data_error);
        infoWidget->showSecs();
        goto EXIT;
    }
    //图片图标显示处理
    displayImgInfo(data, captureConfig);
EXIT:
    fflush(0);
    if (data != NULL)
    {
        free(data);
        data = NULL;
    }
}

void AIMainWidget::processImageDataFromCameraByAIEN(CaptureConfig& captureConfig)
{

    char* data = NULL;

#ifdef Q_OS_WIN32
    captureConfig.result = ERR_CONFIG;
    goto EXIT;
#endif

#ifdef Q_OS_UNIX
    //    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    //    int buf_size = BUFFER_SIZE;
    //    struct sockaddr_in serv_addr;
    //    struct sockaddr_in client_addr;
    //    socklen_t addr_len;
    //    uint32_t image_len;
        // 用vector作为动态缓冲区（便于处理剩余数据）
    //    std::vector<unsigned char> buffer;
    //    buffer.reserve(BUFFER_SIZE);  // 预分配内存提升效率
    //    std::vector<unsigned char> img_vec;

    //    // 设置超时时间：3秒
    //    struct timeval timeout;
    //    timeout.tv_sec = 3;   // 秒
    //    timeout.tv_usec = 0;  // 微秒（0表示不使用）

    //    // 1. 创建UDP套接字并检查错误
    //    if (sockfd < 0) {
    ////        std::cerr << "创建套接字失败: " << strerror(errno) << std::endl;
    //        printf("创建套接字失败.\n");
    //        captureConfig.result = ERR_UNKOWN;
    //        goto EXIT;
    //    }

    //    // 应用超时设置到套接字
    //    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    //        printf("设置超时失败.\n");
    //        ::close(sockfd);
    //        captureConfig.result = ERR_UNKOWN;
    //        goto EXIT;
    //    }

    //    // 2. 设置接收缓冲区大小并检查
    //    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) < 0) {
    ////        std::cerr << "设置缓冲区失败: " << strerror(errno) << std::endl;
    //        printf("设置缓冲区失败.\n");
    //        ::close(sockfd);
    //        captureConfig.result = ERR_MALLOC_FAILED;
    //        goto EXIT;
    //    }

    //    memset(&serv_addr, 0, sizeof(serv_addr));
    //    serv_addr.sin_family = AF_INET;
    //    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //    serv_addr.sin_port = htons(AI_UDP_IMG_PORT);

    //    // 3. 绑定地址和端口并检查
    //    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    ////        std::cerr << "绑定失败: " << strerror(errno) << std::endl;
    //        printf("绑定失败.\n");
    //        ::close(sockfd);
    //        captureConfig.result = ERR_UNKOWN;
    //        goto EXIT;
    //    }

    //    std::cout << "UDP服务器启动，监听端口 9196...缓冲区大小:" << BUFFER_SIZE << "字节" << std::endl;

    //    std::vector<unsigned char> buffer;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    addr_len = sizeof(client_addr);
    // 用vector作为动态缓冲区（便于处理剩余数据）
    std::vector<unsigned char> buffer;
    buffer.reserve(BUFFER_SIZE);  // 预分配内存提升效率
    std::vector<unsigned char> img_vec;
    uint32_t image_len;
    QByteArray jpegData;
    QImage image;
    // 开始计时
    QElapsedTimer timer;
    timer.start();

    while (true)
    {
        // 检查是否超时（4秒）
        if (timer.elapsed() > 4000)
        {
            std::cout << "耗时timer.elapsed(): " << timer.elapsed() << " ms" << std::endl;
            printf("总处理时间超过4秒，超时退出\n");
            //            ::close(sockfd);
            captureConfig.result = ERR_TIMEOUT;
            goto EXIT;
        }

        // 临时缓冲区用于接收单次数据
        unsigned char temp_buf[40960];  // 单次接收4KB，避免大数组
        ssize_t recv_len = recvfrom(MyUpd.getSockfd(), temp_buf, sizeof(temp_buf), 0,
            (struct sockaddr*)&client_addr, &addr_len);

        // 处理接收结果（关键错误检查）
        if (recv_len < 0)
        {
            //            std::cerr << "接收失败: " << strerror(errno) << std::endl;
            printf("接收失败.\n");
            continue;  // 跳过错误，继续接收
        }
        else if (recv_len == 0)
        {
            std::cout << "客户端关闭连接" << std::endl;
            continue;
        }

        // 将新接收的数据追加到全局缓冲区（保持数据连续性）
        buffer.insert(buffer.end(), temp_buf, temp_buf + recv_len);
        std::cout << "已接收 " << recv_len << " 字节，累计: " << buffer.size() << " 字节" << std::endl;

        // 数据处理：检查是否有完整的头部（至少12字节）
        if (buffer.size() >= 12)
        {
            // 验证魔术字（0xA6A66A6A）
            if (buffer[0] == 0xA6 && buffer[1] == 0xA6 &&
                buffer[2] == 0x6A && buffer[3] == 0x6A)
            {

                // 解析图像长度（注意网络字节序转主机字节序）
                image_len = ntohl(*(reinterpret_cast<int*>(buffer.data() + 4)));
                std::cout << "检测到图像头部，图像长度: " << image_len << " 字节" << std::endl;


                // 检查是否接收完整（头部8字节 + 图像数据）
                if (buffer.size() >= 8 + image_len)
                {
                    try
                    {
                        // 提取图像数据
                        img_vec.assign(buffer.begin() + 8, buffer.begin() + 8 + image_len);

                        // 解码并保存图像
//                        cv::Mat img_mat = cv::imdecode(img_vec, cv::IMREAD_COLOR);
//                        if (!img_mat.empty()) {
//                            std::string filename = "received_image.jpg";
//                            if (cv::imwrite(filename, img_mat)) {
//                                std::cout << "图像保存成功: " << filename << std::endl;
//                            } else {
//                                std::cerr << "图像保存失败" << std::endl;
//                            }
//                        } else {
//                            std::cerr << "图像解码失败（数据可能损坏）" << std::endl;
//                        }

                        // 移除已处理的数据，保留剩余字节（可能是下一个图像）
                        buffer.erase(buffer.begin(), buffer.begin() + 8 + 4 + image_len);
                        goto OK;
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "图像处理出错: " << e.what() << std::endl;
                        buffer.clear();  // 出错时清空缓冲区，避免后续异常
                    }
                }
            }
        }
    }

OK:
    //图片图标显示处理
//    displayImgInfo(reinterpret_cast<char*>(img_vec.data()),captureConfig);
    jpegData = QByteArray(reinterpret_cast<const char*>(img_vec.data()), img_vec.size());
    if (!image.loadFromData(jpegData, "JPEG"))
    {
        qWarning("Failed to load image from JPEG data");
        return;
    }
    else
    {
        QImage scaledImg = image.scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QString  imageName = QString("%1.png").arg(m_nNumOfImg);
        scaledImg.save(QString("/tmp/%1").arg(imageName), "PNG", 50);

        if (m_nNumOfImg / MAX_PER_LIST == m_nIndexOfPage)
        {    //本页有显示格则显示
            displayCurImg(m_nNumOfImg % MAX_PER_LIST);
        }

        m_nViewNo[m_nNumOfImg] = captureConfig.nViewNum;

        if (m_nNumOfImg < MAX_IMG_NUM)
        {                      //更新显示照片个数的全局变量
            m_nNumOfImg++;
        }
        updateBtnStatus();
    }
#endif

EXIT:
    fflush(0);
    if (data != NULL)
    {
        free(data);
        data = NULL;
    }

}

/*!
 * \brief AIMainWidget::processImageDataFromCameraByUSB
 * \see getImage();
 */
void AIMainWidget::processImageDataFromCameraByUSB(CaptureConfig& captureConfig)
{
    char* data = NULL;
    char* pDataTmp = NULL;
    char* ptmp = NULL;
    int   dataLenPerFrame = 0;
    int   usbReturn = 0;
    int   strangeNumHead = 2240;	//USB数据包开头的2240个字节，实际图片的帧可能在这2240之中
    int   strangeNumTail = 0;		//USB数据包结尾的不用字节数，PROTOCAL_2最后有32个字节不用，其他协议为0
    uchar imageHead[5] = { 0xAA,0xAA,0xAA,0xAA,0 };
    int   headBias = 0;
    int   bias = 0;

    int   stripResult = 0;
    if (captureConfig.result == ERR_USB_INIT)
    {
        printf("USB init failed.\n");
        goto EXIT;
    }

    if (captureConfig.dataLen == 0)
    {
        printf("protocal data length error [0].\n");
        captureConfig.result = ERR_CONFIG;
        goto EXIT;
    }

    if (captureConfig.nProtoType == PROTOCAL_2)
    {
#if PLATFORM == PLATFORM_335X
        strangeNumTail = 928;
#else
        strangeNumTail = 32;
#endif
    }

    dataLenPerFrame = captureConfig.dataLen / captureConfig.dataDepth;
    data = (char*)malloc(captureConfig.dataLen + strangeNumHead + strangeNumTail + dataLenPerFrame);//多开辟一帧数据的空间，容纳数据偏移

    if (data == NULL)
    {
        printf("malloc failed[processImageDataFromCameraByUSB].\n");
        captureConfig.result = ERR_MALLOC_FAILED;
        goto EXIT;
    }
    memset(data, 0, captureConfig.dataLen + strangeNumHead + strangeNumTail);

    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, captureConfig.nUnitAddr, 0, 0, 0, 0, 0, 1);
#ifdef Q_OS_UNIX
    if (myUsbDev != NULL)
    {
        usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, data, captureConfig.dataLen + strangeNumHead + strangeNumTail, 20 * 1000);
    }
#endif
    if (usbReturn == captureConfig.dataLen + strangeNumHead + strangeNumTail)
    {
        printf("USB(ret = %d).\n", usbReturn);

        //从固定长度的数据中找到数据头，找不到此包丢掉
        if ((pDataTmp = getHead(data, dataLenPerFrame + strangeNumHead + strangeNumTail, (char*)imageHead)) != NULL)
        {
            pDataTmp -= 10;
            headBias = pDataTmp - data;
            bias = headBias;
            for (int i = 1; i < captureConfig.dataDepth; i++)
            {
                if ((ptmp = getHead(pDataTmp, dataLenPerFrame, (char*)imageHead)) != NULL)
                {
                    ptmp -= 10;
                    if (ptmp - pDataTmp > 0)
                    {
                        bias += (ptmp - pDataTmp);
                        printf("USB data frame[%d] bias: %d\n", i, ptmp - pDataTmp);
                    }

                    if (bias > dataLenPerFrame + strangeNumHead + strangeNumTail)
                    {
                        captureConfig.result = ERR_DATA;
                        printf("frame[%d] style error.\n", i);
                        goto EXIT;
                    }

                    pDataTmp = ptmp + dataLenPerFrame;
                }
                else
                {
                    captureConfig.result = ERR_DATA;
                    printf("frame[%d] style error.\n", i);
                    goto EXIT;
                }
            }
        }
        else
        {
            captureConfig.result = ERR_DATA;
            printf("frame style error.\n");
            goto EXIT;
        }
    }
    else if (usbReturn == 0)
    {
        printf("USB(ret = %d).\n", usbReturn);
        captureConfig.result = ERR_UNKOWN;
        goto EXIT;
    }
    else
    {
        printf("timeout for USB(ret = %d).\n", usbReturn);
        captureConfig.result = ERR_TIMEOUT;
        goto EXIT;
    }
    printf("USB data head bias: %d, total bias: %d\n", headBias, bias);

    //将data内数据整理成纯粹的像素数据
    stripResult = stripFrameData(data + headBias, captureConfig);
    if (stripResult == 1)
    {
        infoWidget->setLabelText(myLan.msg_auto_bgk_ckeck_error);
        infoWidget->showSecs();
        goto EXIT;
    }
    else if (stripResult == -1)
    {
        infoWidget->setLabelText(myLan.msg_strip_frame_data_error);
        infoWidget->showSecs();
        goto EXIT;
    }

    //图片图标显示处理
    displayImgInfo(data + headBias, captureConfig);

EXIT:
    fflush(0);
    if (data != NULL)
    {
        free(data);
        data = NULL;
    }
}

/*!
 * \brief AIMainWidget::processImageDataFromCamera
 * \see getImage();
 */
void AIMainWidget::processImageDataFromCamera(CaptureConfig& captureConfig)
{
    if (captureConfig.nTransType == 0)
    {
        processImageDataFromCameraByUART(captureConfig);
    }
    else if (captureConfig.nTransType == 1)
    {
        if (struCnfg.aiEnable == 1)
        {
            processImageDataFromCameraByAIEN(captureConfig);
        }
        //        processImageDataFromCameraByUSB(captureConfig);
    }
}

/*!
 * \brief AIMainWidget::processCaptureResult
 * \see getImage();
 */
void AIMainWidget::processCaptureResult(QList <CaptureConfig>& qlCaptureConfig)
{
    QString text;
    QString enterStr;
    bool hasFailed = false;

#ifdef Q_OS_UNIX
    enterStr = "\n";
#else
    enterStr = "\r\n";
#endif
    foreach(CaptureConfig tmp, qlCaptureConfig)
    {
        if (tmp.result != ERR_SUCCESS)
        {
            hasFailed = true;
        }
        switch (tmp.result)
        {
        case ERR_SUCCESS:
            text += QString("%1/%2: %3").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.upgrade_ok);
            break;
        case ERR_TIMEOUT:
            text += QString("%1/%2: %3").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.timeout);
            break;
        case ERR_USB_INIT:
            text += QString("%1/%2: %3").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.msg_usb_init_error);
            break;
        case ERR_DATA:
            text += QString("%1/%2: %3(%4)").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.upgrade_fail).arg(tmp.result);
            break;
        case ERR_MALLOC_FAILED:
            text += QString("%1/%2: %3(%4)").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.upgrade_fail).arg(tmp.result);
            break;
        case ERR_CONFIG:
            text += QString("%1/%2: %3(%4)").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.upgrade_fail).arg(tmp.result);
            break;
        case ERR_INPUT_ARG:
            text += QString("%1/%2: %3(%4)").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.upgrade_fail).arg(tmp.result);
            break;
        default:
            text += QString("%1/%2: %3(%4)").arg(tmp.nSeq).arg(qlCaptureConfig.count()).arg(myLan.upgrade_fail).arg(tmp.result);
            break;
        }
        text += enterStr;

        if (tmp.nTransType == 1)
        {  //USB传输模式，数据传输结果失败时需复位下位机
            if (tmp.result != ERR_SUCCESS)
            {
                MySerial.com1Write(CMD_UNIT_IMAGE_ERROR_REVISE, UNIT, struGsh.nLevel, tmp.nUnitAddr, 0, 0, 0, struGsh.nIntelSampBurst / 256, struGsh.nIntelSampBurst % 256, 1);
                myFlow.sleep(1);
                MySerial.com1Write(CMD_INT_IMAGE_ERROR_REVISE, INT, struGsh.nLevel, tmp.nUnitAddr, 0, 0, 0, 0, 1, 3);
            }
        }
    }
    if (hasFailed)
    {
        infoWidget->hide();
        myMessageBox msgBox(MSG_TIPS, text);
        msgBox.exec();
    }
}

/*!
 * \brief 更新杂质名称
 */
void AIMainWidget::refreshImpurityName()
{
    int nIdeGp = myFlow.getIdentifyGroupID(struGsh.nUnit, 0);
    QString str = QString("%1").arg(struCnfp.struGroupIdentify[ONE_LEVEL][nIdeGp].struIntel[m_nIndexOfArithIntel].sName);
    m_impurityNameLe->setText(str);
}
/*!
 * \brief AIMainWidget::getImage
 * \see onStartCapBtnPressedSlt();
 */
void AIMainWidget::getImage()
{
    QList <CaptureConfig> qlCaptureConfig;

    myMessageBox msgBox(MSG_QUES, myLan.ai_capture);
    int ret = msgBox.exec();
    if (ret != QDialog::Accepted)
    {
        return;
    }

    bool bIsRunning = false;
    if (g_ejectTimesThread->isRunning() && struGsh.bStatFeed == 1)
    {//! 在开下料过程中拍照，要停止吹气次数统计线程（红外设置中玻璃专选使能后，启用红外自动跟踪,会发红外相机灵敏度调节命令）
        bIsRunning = true;					   //! 由于下位机通信机制原因，会导致拍照命令回包和灵敏度调节命令的回包冲突。导致拍照异常。
        g_ejectTimesThread->stopStatRun();
        myFlow.sleep(1);
    }
    setCaptureAttr(qlCaptureConfig, struGsh.nUnit);              //! 设置所有执行拍照的相机的信息链表
    startCapture(qlCaptureConfig);                              //! 相机开始拍照,所有相机一起准备拍照

    foreach(CaptureConfig tmp, qlCaptureConfig)
    {               //! 从每个相机逐个获取
        infoWidget->setLabelText(myLan.ai_image_capturing + QString("%1/%2").arg(tmp.nSeq).arg(qlCaptureConfig.count()));
        infoWidget->delayShow();
        processImageDataFromCamera(tmp);                        //! 图像数据传输获取及页面显示
        qlCaptureConfig.replace(tmp.nSeq - 1, tmp);
    }
    processCaptureResult(qlCaptureConfig);                      //! 拍照结束后的一些处理，如拍照结果提示、复位下位机等

    if (bIsRunning && !g_ejectTimesThread->isRunning())
    {	//! 拍照时关过吹气次数线程，拍照后要重新运行线程
        g_ejectTimesThread->start();
    }

    infoWidget->hide();
}

/*!
 * \brief AIMainWidget::onStartCapBtnPressedSlt
 * \see getImageFromUSB()
 * \see getImageFromUART()
 * \see displayCurImg(int indexOfImg)
 */
void AIMainWidget::onStartCapBtnPressedSlt()
{
    getImage();
}
/*!
 * \brief AIMainWidget::onDeleteBtnPressedSlt
 */
void AIMainWidget::onDeleteBtnPressedSlt()
{
    m_nPressTimeStart = struGsh.nCounter; //记下按压开始时间
    delTimer->start(3000);

}
/*!
 * \brief AIMainWidget::onDeleteBtnReleasedSlt
 */
void AIMainWidget::onDeleteBtnReleasedSlt()
{
    delTimer->stop();
    if ((struGsh.nCounter - m_nPressTimeStart) < 3)
    { //按压时间少于3s，单个删除图片
        if (m_bStatDelImg)
        {
            m_bStatDelImg = false;
            m_delImgBtn->setRedColor(DEF);
        }
        else
        {
            m_bStatDelImg = true;
            m_delImgBtn->setRedColor(GREEN);
        }
    }
}

void AIMainWidget::onDeleteTimerOutSlt()
{
    delTimer->stop();
    myMessageBox msgBox(MSG_QUES, myLan.all + myLan.ai_delete + "?");

    int ret = msgBox.exec();
    if (ret == QDialog::Accepted)
    {

        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->showSecs();
#ifdef Q_OS_UNIX
        system("rm /tmp/*");
#endif
        m_nNumOfImg = 0;
        m_nIndexOfImg = 0;
        for (int i = 0;i < IMG_MAX_NUM;i++)
        {
            myAIShare.m_nDisPixelNum[i] = 0;
            myAIShare.m_nSelPixelNum[i] = 0;
            for (int j = 0; j < m_nLevelToTal; j++)
            {
                m_nStatOfImg[j][i] = CLASS_NULL;
            }
        }
        emit deleteAllImgSig();//删除成功后发信号
        updateImgList();
        infoWidget->hide();
    }
    m_bStatDelImg = false;
    m_delImgBtn->setRedColor(DEF);
}
/*!
 * \brief AIMainWidget::onPreBtnPressedSlt
 * \see updateImgList()
 * \see updateImgListStat()
 */
void AIMainWidget::onPreBtnPressedSlt()
{
    if (m_nIndexOfPage > FIRST_PAGE)
    {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        m_nIndexOfPage--;
        updateImgList();
        updateImgListStat();
        infoWidget->hide();
    }
    updateBtnStatus();
}
/*!
 * \brief AIMainWidget::onNextBtnPressedSlt
 * \see updateImgList()
 * \see updateImgListStat()
 */
void AIMainWidget::onNextBtnPressedSlt()
{
    if (m_nIndexOfPage < THIRD_PAGE)
    {
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        m_nIndexOfPage++;
        updateImgList();
        updateImgListStat();
        infoWidget->hide();
    }
    updateBtnStatus();
}
/*!
 * \brief AIMainWidget::onBackBtnPressedSlt
 * \see enableAIArithmetic(int classMode)
 */
void AIMainWidget::onBackBtnPressedSlt()
{
    if (enableAIArithmetic(struCnfp.nIntelMode))
    {
        for (int i = 0; i < struCnfg.nInterfaceBoardTotal; i++)
        {
            MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, 0, 3);
        }

        emit goToIdentifyPagesig();
    }
}
/*!
 * \brief AIMainWidget::imageListChangeSlt
 * \param index
 */
void AIMainWidget::imageListChangeSlt(int index)
{
    myMessageBox msgBox(MSG_QUES, myLan.ai_cal_open);

    if (m_bStatDelImg)
    {
        //删除图片
        m_nIndexOfImg = index / 2 + m_nIndexOfPage * MAX_PER_LIST;
        myAIShare.m_nImgIndex = m_nIndexOfImg;
        updateImgListStat();
        deleteCurImg(m_nIndexOfImg);
    }
    else
    {
        if (index % 2)
        {
            //设置物料属性
            if (struCnfp.nIntelMode == MODE_NULL)
            {
                msgBox.exec();
            }
            else
            {
                updateImgName(index / 2 + m_nIndexOfPage * MAX_PER_LIST, true);
            }
        }
        else
        {
            //进入图片预览界面
            m_nIndexOfImg = index / 2 + m_nIndexOfPage * MAX_PER_LIST;
            updateImgListStat();
            myAIShare.m_nImgIndex = m_nIndexOfImg;
            for (int i = 0; i < MAX_IMG_NUM; i++)
            {
                myAIShare.m_nMatStatus[i] = m_nStatOfImg[m_nLayer][i];
            }

            emit updatePreviewPageSig();
            m_stackedWidget->setCurrentIndex(AI_PREVIEW_PAGE);
        }
    }
}
/*!
 * \brief AIMainWidget::goBackToMainWidget
 */
void AIMainWidget::goBackToMainWidget()
{
    m_bSampedOfImg[m_nIndexOfImg] = (myAIShare.m_nDisPixelNum[m_nIndexOfImg] > 100) ? SAMPLED_YES : SAMPLED_NO;
    m_stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
    updateImgName(m_nIndexOfImg, false);
}
/*!
 * \brief AIMainWidget::refreshDisplaySlt
 */
void AIMainWidget::refreshDisplaySlt()
{
    for (int i = 0; i < 2; i++)
    {
        aiBox[i]->setChecked(struCnfp.nArithmeticEnable[ARITH_INTEL_A + i]);
        if (aiBox[i]->isChecked())
        {
            aiBtn[i]->setEnabled(true);
        }
        else
        {
            aiBtn[i]->setEnabled(false);
        }
    }
    updateImgList();
    updateBtnStatus();
}
/*!
 * \brief AIMainWidget::gotoFirstPageSlt
 */
void AIMainWidget::gotoFirstPageSlt()
{
}

/*!
 * \brief AIMainWidget::getImgName
 */
QString AIMainWidget::getImgName(int imgIndex, bool isNeedSuffix)
{
    QString tmpOrig;
    QString tmp;
    int viewNo = m_nViewNo[imgIndex];

    tmpOrig = m_sNameOfImg[viewNo][imgIndex];
    if (isNeedSuffix)
    {
        tmp = tmpOrig;
    }
    else
    {
        setImgName(imgIndex, NULL);
        tmp = m_sNameOfImg[viewNo][imgIndex];
    }

    m_sNameOfImg[viewNo][imgIndex] = tmpOrig;

    return tmp;
}

/*!
 * \brief AIMainWidget::getImgName
 */
void AIMainWidget::setImgName(int imgIndex, QString suffix)
{
    m_sNameOfImg[m_nLayer][imgIndex] = QString("%1%2%3").arg(myLan.ai_material).arg(imgIndex + 1).arg(suffix);
}

/*!
 * \brief AIMainWidget::updateImgName
 * \param indexOfImg
 * \param bFlagSet
 */
void AIMainWidget::updateImgName(int indexOfImg, bool bFlagSet)
{
    if (bFlagSet)
    { //bFlagSet为true时，设置物料属性，更改物料名称
        switch (m_nStatOfImg[m_nLayer][indexOfImg])
        {
        case CLASS_GOOD:
            if (m_bCalSet)
            {
                m_nStatOfImg[m_nLayer][indexOfImg] = CLASS_BAD;
            }
            else
            {
                for (int i = 0; i < m_nLevelToTal; i++)
                {
                    m_nStatOfImg[i][indexOfImg] = CLASS_BAD;
                }
            }
            break;
        case CLASS_BAD:
            if (m_bCalSet)
            {
                m_nStatOfImg[m_nLayer][indexOfImg] = CLASS_NULL;
            }
            else
            {
                for (int i = 0; i < m_nLevelToTal; i++)
                {
                    m_nStatOfImg[i][indexOfImg] = CLASS_NULL;
                }
            }
            break;
        case CLASS_NULL:
            if (m_bCalSet)
            {
                m_nStatOfImg[m_nLayer][indexOfImg] = CLASS_GOOD;
            }
            else
            {
                for (int i = 0; i < m_nLevelToTal; i++)
                {
                    m_nStatOfImg[i][indexOfImg] = CLASS_GOOD;
                }
            }
            break;
        default:
            break;
        }
    }

    switch (m_nStatOfImg[m_nLayer][indexOfImg])
    {
    case CLASS_GOOD:
        if (m_bCalSet)
        {
            setImgName(indexOfImg, QString("/%1/%2").arg(myLan.ai_good).arg(myAIShare.m_nDisPixelNum[indexOfImg]));
        }
        else
        {
            for (int i = 0; i < m_nLevelToTal; i++)
            {
                m_nStatOfImg[i][indexOfImg] = CLASS_GOOD;
                setImgName(indexOfImg, QString("/%1/%2").arg(myLan.ai_good).arg(myAIShare.m_nDisPixelNum[indexOfImg]));
            }
        }
        break;
    case CLASS_BAD:
        if (m_bCalSet)
        {
            setImgName(indexOfImg, QString("/%1/%2").arg(myLan.ai_bad).arg(myAIShare.m_nDisPixelNum[indexOfImg]));
        }
        else
        {
            for (int i = 0; i < m_nLevelToTal; i++)
            {
                m_nStatOfImg[i][indexOfImg] = CLASS_BAD;
                setImgName(indexOfImg, QString("/%1/%2").arg(myLan.ai_bad).arg(myAIShare.m_nDisPixelNum[indexOfImg]));
            }
        }
        break;
    default:
        if (m_bCalSet)
        {
            setImgName(indexOfImg, NULL);
        }
        else
        {
            for (int i = 0; i < m_nLevelToTal; i++)
            {
                m_nStatOfImg[i][indexOfImg] = CLASS_NULL;
                setImgName(indexOfImg, NULL);
            }
        }
        break;
    }

    m_imgPropertyBtn[indexOfImg % MAX_PER_LIST]->setText(m_sNameOfImg[m_nLayer][indexOfImg]);
}
/*!
 * \brief AIMainWidget::updateBtnStatus
 */
void AIMainWidget::updateBtnStatus()
{
    switch (struCnfp.nIntelMode)
    {
    case MODE_MULTIPLE:
        m_classModeBtn->setText(myLan.ai_class_mode_multiple);
        m_AIStudyBtn->setEnabled(true);
        break;
    case MODE_USUAL:
        m_classModeBtn->setText(myLan.ai_class_mode_usual);
        m_AIStudyBtn->setEnabled(true);
        break;
    case MODE_NULL:
        m_classModeBtn->setText(myLan.ai_class_mode_null);
        m_AIStudyBtn->setEnabled(false);
        break;
    default:
        break;
    }

    if (m_bCalSet)
    {
        m_calSetBtn->setText(myLan.ai_cal_set_self);
    }
    else
    {
        m_calSetBtn->setText(myLan.ai_cal_set_same);
    }

    switch (m_nIndexOfPage)
    {
    case FIRST_PAGE:
        m_preListBtn->setEnabled(false);
        m_nextListBtn->setEnabled(true);
        break;
    case SECOND_PAGE:
        m_preListBtn->setEnabled(true);
        m_nextListBtn->setEnabled(true);
        break;
    case THIRD_PAGE:
        m_preListBtn->setEnabled(true);
        m_nextListBtn->setEnabled(false);
        break;
    default:
        break;
    }

    updateLayerStat();

    m_chanNum->display(struGsh.nUnit / 2 + 1);

    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < m_nIndexOfPage * MAX_PER_LIST + MAX_PER_LIST; i++)
    {
        updateImgName(i, false);
    }
    refreshImpurityName();
}

/*!
 * \brief AIMainWidget::displayCurImg
 * \param indexOfImg
 */
void AIMainWidget::displayCurImg(int indexOfImg)
{
    bool bIsImg = true;
    QString strImg;
    QImage  image;

    strImg.sprintf("/tmp/%d.png", indexOfImg + m_nIndexOfPage * MAX_PER_LIST);
    bIsImg = image.load(strImg);
    if (bIsImg)
    {
        if (LCD_WIDTH == 640)
            m_imgListBtn[indexOfImg]->setIconSize(0.13 * image.size());
        else
            m_imgListBtn[indexOfImg]->setIconSize(0.25 * image.size());
        m_imgListBtn[indexOfImg]->setIcon(QPixmap::fromImage(image));
        m_imgListBtn[indexOfImg]->setEnabled(true);
        m_imgPropertyBtn[indexOfImg]->setEnabled(true);
    }
    else
    {
        m_imgListBtn[indexOfImg]->setIcon(QPixmap());
        m_imgListBtn[indexOfImg]->setEnabled(false);
        m_imgPropertyBtn[indexOfImg]->setEnabled(false);
    }

    updateImgName(indexOfImg + m_nIndexOfPage * MAX_PER_LIST, false);
}
/*!
 * \brief AIMainWidget::deleteCurImg
 * \param indexOfImg
 */
void AIMainWidget::deleteCurImg(int indexOfImg)
{
    myMessageBox dlg(MSG_QUES, myLan.ai_del_img);
    int ret = dlg.exec();
    if (ret == QDialog::Accepted)
    {
        //emit deleteCurImgSig();
        infoWidget->setLabelText(myLan.msg_applying);
        infoWidget->delayShow();
        for (int i = indexOfImg;i < MAX_IMG_NUM;i++)
        {
            if (i == MAX_IMG_NUM - 1)
            {
                myAIShare.m_nDisPixelNum[i] = 0;
                myAIShare.m_nSelPixelNum[i] = 0;
                for (int j = 0; j < m_nLevelToTal; j++)
                {
                    m_nStatOfImg[j][i] = CLASS_NULL;
                    setImgName(i, NULL);
                }
#ifdef Q_OS_UNIX
                bool retFile = true;
                QFile pngFile1(QString("/tmp/%1.png").arg(i));

                if (pngFile1.exists())
                {
                    pngFile1.close();
                    retFile = pngFile1.remove();
                }
                if (!retFile)
                {
                    infoWidget->setLabelText("Failed!");
                    myFlow.sleep(1);
                    infoWidget->hide();
                    return;
                }
#endif
            }
            else
            {
                m_nViewNo[i] = m_nViewNo[i + 1];
                myAIShare.m_nDisPixelNum[i] = myAIShare.m_nDisPixelNum[i + 1];
                myAIShare.m_nSelPixelNum[i] = myAIShare.m_nSelPixelNum[i + 1];
                for (int j = 0; j < m_nLevelToTal; j++)
                {
                    m_nStatOfImg[j][i] = m_nStatOfImg[j][i + 1];
                    m_sNameOfImg[j][i] = m_sNameOfImg[j][i + 1];
                }

#ifdef Q_OS_UNIX
                bool retFile = true;
                QFile pngFile1(QString("/tmp/%1.png").arg(i));
                QFile pngFile2(QString("/tmp/%1.png").arg(i + 1));


                if (pngFile1.exists())
                {
                    pngFile1.close();
                    retFile = pngFile1.remove();
                }
                if (!retFile)
                {
                    infoWidget->setLabelText("Failed!");
                    myFlow.sleep(1);
                    infoWidget->hide();
                    return;
                }

                if (pngFile2.exists())
                {
                    pngFile2.close();
                    retFile = pngFile2.rename(QString("/tmp/%1.png").arg(i));
                    pngFile2.close();
                }
                if (!retFile)
                {
                    infoWidget->setLabelText("Failed!!");
                    myFlow.sleep(1);
                    infoWidget->hide();
                    return;
                }
#endif
            }
            updateImgName(i, false);
        }
        emit deleteCurImgSig();	//删除操作成功后再发信号

        if (m_nNumOfImg > 0)
            m_nNumOfImg--;
        for (int j = 0; j < m_nLevelToTal; j++)
        {
            for (int i = 0;i < IMG_MAX_NUM;i++)
            {
                m_nStatOfImg[j][i] = CLASS_NULL;
            }
        }
        updateImgList();
        infoWidget->hide();
    }
}
/*!
 * \brief AIMainWidget::updateImgListStat
 */
void AIMainWidget::updateImgListStat()
{
    for (int i = m_nIndexOfPage * MAX_PER_LIST; i < (1 + m_nIndexOfPage) * MAX_PER_LIST; i++)
    {
        if (i == m_nIndexOfImg
            && m_nIndexOfPage == m_nIndexOfImg / MAX_PER_LIST
            && m_nIndexOfImg != 0)
        {
            m_imgListBtn[i % MAX_PER_LIST]->setRedColor(GREEN);
        }
        else
        {
            m_imgListBtn[i % MAX_PER_LIST]->setRedColor(DEF);
        }
    }
}

/*!
 * \brief AIMainWidget::getHead
 * \param mem
 * \param size
 * \param str
 * \return
 */
char* AIMainWidget::getHead(const char* mem, int size, const char* str)
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
 * \brief AIMainWidget::getImageFromUSB_2048_All
 * \return
 */
bool AIMainWidget::getImageFromUSB_2048_All()
{
    bool retCode = true;
    int nFeederEn[10];
#ifdef Q_OS_UNIX
    int usbReturn;
    int bufSize = 6160 * IMAGE_HEIGHT + 2240;
    uchar tmpColorBuf[IMAGE_WIDTH * 3 * IMAGE_HEIGHT];
    uchar tmpColorBuf1[IMAGE_WIDTH * 3 * IMAGE_HEIGHT];
    uchar tmpBuf[bufSize];
    char* pImageHead;
    uchar imageHead[5] = { 0xAA,0xAA,0xAA,0xAA,0 };
    bool bIsLeft = false;
    int nBegin = m_beginChLe->text().toInt();
    int nEnd = m_endChLe->text().toInt();
    int tmpFrontUnit, tmpRearUnit;

    //	if (struGsh.nUnit%2) {
    //			tmpFrontUnit = struGsh.nUnit-1;
    //			tmpRearUnit  = struGsh.nUnit;
    //	}
    //	else {
    tmpFrontUnit = (struCnfg.nAutoReferChute - 1) * 2;
    tmpRearUnit = (struCnfg.nAutoReferChute - 1) * 2 + 1;
    //	}


    if (myUsbDev != NULL)
    {
        usb.myusbFree();
    }
    usb.myusbInit();
    if (myUsbDev == NULL)
    {
        return false;
    }

    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, tmpRearUnit, 0, 0, (IMAGE_HEIGHT - 1) / 256, (IMAGE_HEIGHT - 1) % 256, 1, 3);
    // MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, tmpFrontUnit, 0 ,0, 0, struGsh.nIntelSampBurst/256, struGsh.nIntelSampBurst%256, 1);
    MySerial.com1Write(CMD_UNIT_IMG_BURST, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 0, struGsh.nIntelSampBurst / 256, struGsh.nIntelSampBurst % 256, 1);
    // MySerial.com1Write(CMD_UNIT_CAPTURE_RANGE, UNIT, struGsh.nLevel, tmpFrontUnit, 0 ,nBegin/256, nBegin%256, nEnd/256, nEnd%256, 1);
    MySerial.com1Write(CMD_UNIT_CAPTURE_RANGE, UNIT, struGsh.nLevel, tmpRearUnit, 0, nBegin / 256, nBegin % 256, nEnd / 256, nEnd % 256, 1);
    MySerial.com1Write(2, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    // MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, tmpFrontUnit, 0, 0, 1, (IMAGE_HEIGHT-1)/256, (IMAGE_HEIGHT-1)%256, 1);
    MySerial.com1Write(CMD_UNIT_IMAGE, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 1, (IMAGE_HEIGHT - 1) / 256, (IMAGE_HEIGHT - 1) % 256, 1);

    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 0, 0, 0, 1); // 主相机板传输图像
    // for(int i=0;i<10;i++) {
    //     nFeederEn[i] = struCnfp.struGroupCtrl[0].nFeederEnable[i];
    // }
   //  for(int i=0;i<10;i++) {
   //      struCnfp.struGroupCtrl[0].nFeederEnable[i] = 0;
   //  }
   //  myFlow.resetFeeder(0);
     //for (int loop = 0; loop < 2; loop++) {
    bzero(tmpBuf, bufSize);
    usbReturn = usb_bulk_read(myUsbDev, USB_EP_IN_AI, (char*)tmpBuf, bufSize, 20 * 1000);
    pImageHead = (char*)tmpBuf;
    qDebug("usbReturn = %d", usbReturn);
    if (usbReturn == bufSize)
    {
        for (int i = 0; i < IMAGE_HEIGHT; i++)
        {
            /* 判断包头数据是否正确 */
            if ((pImageHead = getHead((char*)pImageHead, 6160 * 3, (char*)imageHead)) != NULL)
            {
                memcpy(tmpColorBuf + IMAGE_WIDTH * 3 * (IMAGE_HEIGHT - 1 - i), pImageHead, IMAGE_WIDTH * 3);

                memcpy(tmpColorBuf1 + IMAGE_WIDTH * 3 * (IMAGE_HEIGHT - 1 - i), pImageHead + IMAGE_WIDTH * 3, IMAGE_WIDTH * 3);
            }
            else
            {
                retCode = false;
                return retCode;
            }
        }
        MySerial.com1Write(3, 2, 0, 0x3f, 0, 0, 0, 0, 0, 1);
    }
    else
    {
        retCode = false;
        return retCode;
    }
    QString colorImageName;
    //    if(loop == 0) {
            /* 将图像数据保存成位图文件 */
    colorImageName = QString("%1.png").arg(2 * m_nNumOfImg);
    qDebug() << colorImageName;
    saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar*)tmpColorBuf);
    /* 将图像数据保存成位图文件 */
    colorImageName = QString("%1.png").arg(2 * m_nNumOfImg + 1);
    qDebug() << colorImageName;
    saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar*)tmpColorBuf1);

    MySerial.com1Write(CMD_UNIT_MAIZE_IMAGE_UPLOAD, UNIT, struGsh.nLevel, tmpRearUnit, 0, 0, 0, 0, 0, 1); // 上层相机板传输图像
    //    }
    //    else {
    //        /* 将图像数据保存成位图文件 */
    //        colorImageName = QString("%1.png").arg(4*m_nNumOfImg+1);
    //        qDebug()<<colorImageName;
    //        saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar *)tmpColorBuf);
    //        colorImageName = QString("%1.png").arg(4*m_nNumOfImg+3);
    //        qDebug()<<colorImageName;
    //        saveBMPFile(colorImageName, IMAGE_WIDTH, IMAGE_HEIGHT, (uchar *)tmpColorBuf1);
    //    }
        //}
      //  for(int i=0; i<10; i++) {
      //      struCnfp.struGroupCtrl[0].nFeederEnable[i] = nFeederEn[i];
      //  }

      //  myFlow.resetFeeder(0);
      //  myFlow.sleep(5);
#endif
    return retCode;
}

/*!
 * \brief AIMainWidget::saveBMPFile
 * \param name
 * \param width
 * \param height
 * \param data
 */
void AIMainWidget::saveBMPFile(QString name, int width, int height, uchar* data)
{
    QImage img(data, width, height, QImage::Format_RGB888);
    QImage scaledImg = img.scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    scaledImg.save(QString("/tmp/%1").arg(name), "PNG", 50);

}

void AIMainWidget::saveBMPFileForMono(QString name, int width, int height, uchar* data)
{
    QImage img = QImage(data, width, height, QImage::Format_Indexed8);
    QVector<QRgb> grayTable;
    for (int i = 0;i < 256; i++)
        grayTable.push_back(qRgb(i, i, i));
    img.setColorTable(grayTable);
    img.save(QString("/tmp/%1").arg(name), "PNG", 50);
}
/*!
 * \brief AIMainWidget::pixelIsEdgeImage
 * \param image
 * \param x
 * \param y
 * \return
 */
bool AIMainWidget::pixelIsEdgeImage(QImage* image, int x, int y)
{
    QColor rgb;
    for (int m = -1; m <= 1; m++)
    {
        for (int n = -1; n <= 1; n++)
        {
            if (m != 0 || n != 0)
            {
                if (x + m >= 0 && x + m < IMAGE_WIDTH && y + n >= 0 && y + n < IMAGE_HEIGHT)
                {
                    rgb = image->pixel(x + m, y + n);
                    if (myAIShare.pixelIsBackgroundcomm((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue(), (x + m)))
                        return true;
                }
            }
        }
    }
    return false;
}
/*!
 * \brief AIMainWidget::multipleClassAICal
 * \return
 */
bool AIMainWidget::multipleClassAICal(int indexOfTrain)
{
    QFile file;
    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();

    file.remove(QString("/tmp/train%1").arg(indexOfTrain));
    resetDataMaxMin(indexOfTrain);

    //get the data of good material
    for (int k = 0; k < MAX_IMG_NUM; k++)
    {
        if (m_nStatOfImg[m_nLayer][k] == CLASS_NULL)
        {
            continue;
        }
        if (m_nStatOfImg[m_nLayer][k] == CLASS_GOOD)
        {
            qDebug() << m_bSampedOfImg[k];
            if (m_bSampedOfImg[k])
            {
                saveDataFromImg(DATA_DISK, MODE_MULTIPLE, CLASS_GOOD, k, indexOfTrain);
            }
            else
            {
                saveDataFromImg(DATA_IMG, MODE_MULTIPLE, CLASS_GOOD, k, indexOfTrain);
            }
        }
    }

    //get data of bad material
    for (int k = 0; k < MAX_IMG_NUM; k++)
    {
        if (m_nStatOfImg[m_nLayer][k] == CLASS_NULL)
        {
            continue;
        }
        if (m_nStatOfImg[m_nLayer][k] == CLASS_BAD)
        {
            if (m_bSampedOfImg[k])
            {
                saveDataFromImg(DATA_DISK, MODE_MULTIPLE, CLASS_BAD, k, indexOfTrain);
            }
            else
            {
                saveDataFromImg(DATA_IMG, MODE_MULTIPLE, CLASS_BAD, k, indexOfTrain);
            }
        }
    }

    qDebug("data\n");
    saveDataMaxMin(MODE_MULTIPLE, indexOfTrain);
    qDebug("max_min\n");
    AIClassComputer(MODE_MULTIPLE, indexOfTrain);
    qDebug("computer\n");

    setGlobalAIParam(MODE_MULTIPLE, indexOfTrain);
    autoCopyIntellArgs();

    enableAIArithmetic(MODE_MULTIPLE);

    infoWidget->hide();

    return true;
}
/*!
 * \brief AIMainWidget::usualClassAICal
 * \return
 */
void AIMainWidget::usualClassAICal()
{
    QFile file;

    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();

    for (int i = 0; i < MAX_IMG_NUM; i++)
    {
        file.remove(QString("/tmp/train%1").arg(i));
    }
    m_nClassNum[m_nLayer] = 0;

    //get the data of good material
    for (int k = 0; k < MAX_IMG_NUM; k++)
    {
        if (m_nStatOfImg[m_nLayer][k] == CLASS_NULL)
        {
            continue;
        }
        if (m_nStatOfImg[m_nLayer][k] == CLASS_GOOD)
        {
            for (int m = 0; m < MAX_IMG_NUM; m++)
            {
                if (m_nStatOfImg[m_nLayer][m] == CLASS_NULL)
                {
                    continue;
                }
                if (m_nStatOfImg[m_nLayer][m] == CLASS_BAD)
                {
                    resetDataMaxMin(m_nClassNum[m_nLayer]);
                    if (m_bSampedOfImg[m])
                    {
                        saveDataFromImg(DATA_DISK, MODE_USUAL, CLASS_GOOD, k, m_nClassNum[m_nLayer]);
                        saveDataFromImg(DATA_DISK, MODE_USUAL, CLASS_BAD, m, m_nClassNum[m_nLayer]);
                        m_nClassNum[m_nLayer]++;
                    }
                    else
                    {
                        saveDataFromImg(DATA_IMG, MODE_USUAL, CLASS_GOOD, k, m_nClassNum[m_nLayer]);
                        saveDataFromImg(DATA_IMG, MODE_USUAL, CLASS_BAD, m, m_nClassNum[m_nLayer]);
                        m_nClassNum[m_nLayer]++;
                    }
                }
            }
        }
    }
    qDebug() << m_nLayer;
    qDebug() << m_nClassNum[m_nLayer];
    for (int i = 0; i < m_nClassNum[m_nLayer]; i++)
    {
        saveDataMaxMin(MODE_USUAL, i);
        AIClassComputer(MODE_USUAL, i);
        setGlobalAIParam(MODE_USUAL, i);
    }
    enableAIArithmetic(MODE_USUAL);
    infoWidget->hide();
}
/*!
 * \brief AIMainWidget::AIClassComputer
 * \param classMode
 * \param indexOfTrain
 * \return
 */
bool AIMainWidget::AIClassComputer(int classMode, int indexOfTrain)
{
    QFile file;
    char cmd[64];
    int maxQuo;

    memset(m_struClassQuo, 0, MAX_VIEW_LOCATION * sizeof(struct classQuo));
    //    sprintf(cmd, "./normalization_scale -r /tmp/train%d.range /tmp/train%d",indexOfTrain, indexOfTrain);
    //    system(cmd);
    //    file.remove(QString("/tmp/train%1").arg(indexOfTrain));
    //    file.remove(QString("/tmp/train%1.range").arg(indexOfTrain));
    //    sprintf(cmd, "./normalization_linear_learn /tmp/train%d.scale",indexOfTrain);
    //    system(cmd);
    //    file.remove(QString("/tmp/train%1.scale").arg(indexOfTrain));

    sprintf(cmd, "./norm_train /tmp/train%d", indexOfTrain);
    system(cmd);
    file.remove(QString("/tmp/train%1").arg(indexOfTrain));


    m_struClassQuo[m_nLayer].nMin[0] = m_nMinR[m_nLayer][indexOfTrain];
    m_struClassQuo[m_nLayer].nMax[0] = m_nMaxR[m_nLayer][indexOfTrain];
    m_struClassQuo[m_nLayer].nMin[1] = m_nMinG[m_nLayer][indexOfTrain];
    m_struClassQuo[m_nLayer].nMax[1] = m_nMaxG[m_nLayer][indexOfTrain];
    m_struClassQuo[m_nLayer].nMin[2] = m_nMinB[m_nLayer][indexOfTrain];
    m_struClassQuo[m_nLayer].nMax[2] = m_nMaxB[m_nLayer][indexOfTrain];
    if (classMode == MODE_MULTIPLE)
    {
        m_struClassQuo[m_nLayer].nMin[3] = m_nMinRG[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMax[3] = m_nMaxRG[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMin[4] = m_nMinRB[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMax[4] = m_nMaxRB[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMin[5] = m_nMinGB[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMax[5] = m_nMaxGB[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMin[6] = m_nMinRR[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMax[6] = m_nMaxRR[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMin[7] = m_nMinGG[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMax[7] = m_nMaxGG[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMin[8] = m_nMinBB[m_nLayer][indexOfTrain];
        m_struClassQuo[m_nLayer].nMax[8] = m_nMaxBB[m_nLayer][indexOfTrain];
    }

    file.setFileName(QString("/tmp/train%1.model").arg(indexOfTrain));
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("result file open error!");
        return false;
    }

    QTextStream io(&file);

    if (classMode == MODE_MULTIPLE)
    {
        if (struCnfp.nAIMode)
        {
            maxQuo = MAX_MUL_QUO1;
        }
        else
        {
            maxQuo = MAX_MUL_QUO;
        }
    }
    else
    {
        maxQuo = MAX_USUAL_QUO;
    }
    for (int i = 0; i < maxQuo; i++)
    {
        io >> m_struClassQuo[m_nLayer].nQuo[i];
    }
    io >> m_struClassQuo[m_nLayer].nCons;     //cons*/
    file.close();


    for (int i = 0; i < maxQuo; i++)
    {
        qDebug("nQuo[%d] = %f\n", i, m_struClassQuo[m_nLayer].nQuo[i]);
    }
    qDebug("nCons = %f\n", m_struClassQuo[m_nLayer].nCons);


    //    for (int i = 0; i < maxQuo;i++) {
    //        m_struClassQuo[m_nLayer].nCons = m_struClassQuo[m_nLayer].nCons-m_struClassQuo[m_nLayer].nQuo[i];
    //    }

    //    for(int i = 0; i < maxQuo; i++){
    //        m_struClassQuo[m_nLayer].nCons = m_struClassQuo[m_nLayer].nCons
    //                -2*m_struClassQuo[m_nLayer].nMin[i]*m_struClassQuo[m_nLayer].nQuo[i]/(m_struClassQuo[m_nLayer].nMax[i]-m_struClassQuo[m_nLayer].nMin[i]);
    //        if (classMode == MODE_MULTIPLE) {
    //            m_struClassQuo[m_nLayer].nQuo[i] = 2*m_struClassQuo[m_nLayer].nQuo[i]/(m_struClassQuo[m_nLayer].nMax[i]-m_struClassQuo[m_nLayer].nMin[i])*100000;
    //        }
    //        else {
    //           m_struClassQuo[m_nLayer].nQuo[i] = 2*m_struClassQuo[m_nLayer].nQuo[i]/(m_struClassQuo[m_nLayer].nMax[i]-m_struClassQuo[m_nLayer].nMin[i])*1000;
    //        }
    //        qDebug("nQuo[%d] = %f\n", i, m_struClassQuo[m_nLayer].nQuo[i]);
    //    }
    //    if (classMode == MODE_MULTIPLE) {
    //        m_struClassQuo[m_nLayer].nCons = m_struClassQuo[m_nLayer].nCons*100000;
    //    }
    //    else {
    //        m_struClassQuo[m_nLayer].nCons = m_struClassQuo[m_nLayer].nCons*1000;
    //    }
    //    qDebug("nCons = %f\n", m_struClassQuo[m_nLayer].nCons);

    //    file.remove(QString("/tmp/train%1.scale.model").arg(indexOfTrain));

    return true;
}
/*!
 * \brief AIMainWidget::setGlobalAIParam
 * \param classMode
 * \param indexOfTrain
 * \return
 */
bool AIMainWidget::setGlobalAIParam(int classMode, int indexOfTrain)
{
    int groupNum = 0;
    int addr = 0;
    switch (struCnfe.nMachine)
    {
    case MACHINE_CF:
        qDebug("cal-%d", m_bCalSet);
        if (m_bCalSet)
        {//独立设置
            groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
            switch (struCnfp.nMatAssembleMode)
            {
            case PARAMS_ALL_SEPARATE:     //全部单独设置
                getGlobalAIParam(classMode, struGsh.nLevel, groupNum, indexOfTrain);
                break;
            case PARAMS_FRONT_REAR_SAME: //前后视相同设置
                groupNum = getUnitGroup(struGsh.nLevel, struGsh.nUnit);
                if (groupNum % 2)
                {
                    for (int i = groupNum - 1; i < groupNum + 1; i++)
                    {
                        getGlobalAIParam(classMode, struGsh.nLevel, i, indexOfTrain);
                    }
                }
                else
                {
                    for (int i = groupNum; i < groupNum + 2; i++)
                    {
                        getGlobalAIParam(classMode, struGsh.nLevel, i, indexOfTrain);
                    }
                }
                break;
            case PARAMS_FIRST_SECOND_SAME: //一二三次相同设置
                addr = groupNum % 2;
                for (int i = 0; i < struCnfg.nLevelTotal; i++)
                {
                    for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
                    {
                        if (j % 2 == addr)
                        {
                            getGlobalAIParam(classMode, struGsh.nLevel, j, indexOfTrain);
                        }
                    }
                }
                break;
            case PARAMS_ALL_SAME: //全部相同设置
                for (int i = 0; i < struCnfg.nLevelTotal; i++)
                {
                    for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
                    {
                        getGlobalAIParam(classMode, i, j, indexOfTrain);
                    }
                }
                break;
            default:
                for (int i = 0; i < struCnfg.nLevelTotal; i++)
                {
                    for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
                    {
                        getGlobalAIParam(classMode, i, j, indexOfTrain);
                    }
                }
                break;
            }
        }
        else
        {//全部相同
            for (int j = 0; j < struCnfg.nLevelTotal; j++)
            {
                for (int k = 0; k < struCnfg.struLevelInfo[j].nIdentifyGroupTotal; k++)
                {
                    getGlobalAIParam(classMode, j, k, indexOfTrain);
                }
            }
        }
        break;
    }

    return true;
}
/*!
 * \brief AIMainWidget::getUnitGroup
 * \param level
 * \param unit
 * \return
 */
int AIMainWidget::getUnitGroup(int level, int unit)
{
    int group = -1;

    for (int i = 0; i < MAX_GROUP_IDTNTIFY; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitCount; j++)
        {
            if (unit == struCnfg.struLevelInfo[level].struIdentifyGroupInfo[i].nUnitId[j])
            {
                group = i;
            }
        }
    }

    return group;
}
/*!
 * \brief AIMainWidget::getGlobalAIParam
 * \param classMode
 * \param indexOfTrain
 */
void AIMainWidget::getGlobalAIParam(int classMode, int levelNum, int groupNum, int indexOfTrain)
{
    if (classMode == MODE_MULTIPLE)
    {
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[0] = (m_struClassQuo[m_nLayer].nCons >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[1] = abs(m_struClassQuo[m_nLayer].nCons);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[0] = (m_struClassQuo[m_nLayer].nQuo[0] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[1] = abs(m_struClassQuo[m_nLayer].nQuo[0]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[0] = (m_struClassQuo[m_nLayer].nQuo[1] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[1] = abs(m_struClassQuo[m_nLayer].nQuo[1]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[0] = (m_struClassQuo[m_nLayer].nQuo[2] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[1] = abs(m_struClassQuo[m_nLayer].nQuo[2]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[0] = (m_struClassQuo[m_nLayer].nQuo[3] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[1] = abs(m_struClassQuo[m_nLayer].nQuo[3]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[0] = (m_struClassQuo[m_nLayer].nQuo[4] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[1] = abs(m_struClassQuo[m_nLayer].nQuo[4]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[0] = (m_struClassQuo[m_nLayer].nQuo[5] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[1] = abs(m_struClassQuo[m_nLayer].nQuo[5]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[0] = (m_struClassQuo[m_nLayer].nQuo[6] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[1] = abs(m_struClassQuo[m_nLayer].nQuo[6]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[0] = (m_struClassQuo[m_nLayer].nQuo[7] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[1] = abs(m_struClassQuo[m_nLayer].nQuo[7]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[0] = (m_struClassQuo[m_nLayer].nQuo[8] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[1] = abs(m_struClassQuo[m_nLayer].nQuo[8]);

        qDebug("%d-%d", levelNum, groupNum);
        qDebug("cons-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nConsDfl[1]);
        qDebug("R-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoR[1]);
        qDebug("G-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoG[1]);
        qDebug("B-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoB[1]);
        qDebug("RG-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRG[1]);
        qDebug("RB-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRB[1]);
        qDebug("GB-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGB[1]);
        qDebug("RR-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoRR[1]);
        qDebug("GG-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoGG[1]);
        qDebug("BB-%d-%d", struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[0], struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuoBB[1]);
    }
    else
    {
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nCons[0] = (m_struClassQuo[m_nLayer].nCons >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nCons[1] = abs(m_struClassQuo[m_nLayer].nCons);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuo1[0] = (m_struClassQuo[m_nLayer].nQuo[0] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuo1[1] = abs(m_struClassQuo[m_nLayer].nQuo[0]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuo2[0] = (m_struClassQuo[m_nLayer].nQuo[1] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuo2[1] = abs(m_struClassQuo[m_nLayer].nQuo[1]);
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuo3[0] = (m_struClassQuo[m_nLayer].nQuo[2] >= 0) ? 0 : 1;
        struCnfp.struGroupIdentify[levelNum][groupNum].struIntel[indexOfTrain].nQuo3[1] = abs(m_struClassQuo[m_nLayer].nQuo[2]);
    }
}
/*!
 * \brief AIMainWidget::getDataMaxMin
 * \param index
 * \param r
 * \param g
 * \param b
 */
void AIMainWidget::getDataMaxMin(int index, int r, int g, int b)
{
    m_nMaxR[m_nLayer][index] = (r > m_nMaxR[m_nLayer][index]) ? r : m_nMaxR[m_nLayer][index];
    m_nMinR[m_nLayer][index] = (r < m_nMinR[m_nLayer][index]) ? r : m_nMinR[m_nLayer][index];

    m_nMaxG[m_nLayer][index] = (g > m_nMaxG[m_nLayer][index]) ? g : m_nMaxG[m_nLayer][index];
    m_nMinG[m_nLayer][index] = (g < m_nMinG[m_nLayer][index]) ? g : m_nMinG[m_nLayer][index];

    m_nMaxB[m_nLayer][index] = (b > m_nMaxB[m_nLayer][index]) ? b : m_nMaxB[m_nLayer][index];
    m_nMinB[m_nLayer][index] = (b < m_nMinB[m_nLayer][index]) ? b : m_nMinB[m_nLayer][index];

    m_nMaxRG[m_nLayer][index] = (r * g > m_nMaxRG[m_nLayer][index]) ? r * g : m_nMaxRG[m_nLayer][index];
    m_nMinRG[m_nLayer][index] = (r * g < m_nMinRG[m_nLayer][index]) ? r * g : m_nMinRG[m_nLayer][index];

    m_nMaxRB[m_nLayer][index] = (r * b > m_nMaxRB[m_nLayer][index]) ? r * b : m_nMaxRB[m_nLayer][index];
    m_nMinRB[m_nLayer][index] = (r * b < m_nMinRB[m_nLayer][index]) ? r * b : m_nMinRB[m_nLayer][index];

    m_nMaxGB[m_nLayer][index] = (g * b > m_nMaxGB[m_nLayer][index]) ? g * b : m_nMaxGB[m_nLayer][index];
    m_nMinGB[m_nLayer][index] = (g * b < m_nMinGB[m_nLayer][index]) ? g * b : m_nMinGB[m_nLayer][index];

    m_nMaxRR[m_nLayer][index] = m_nMaxR[m_nLayer][index] * m_nMaxR[m_nLayer][index];
    m_nMinRR[m_nLayer][index] = m_nMinR[m_nLayer][index] * m_nMinR[m_nLayer][index];
    m_nMaxGG[m_nLayer][index] = m_nMaxG[m_nLayer][index] * m_nMaxG[m_nLayer][index];
    m_nMinGG[m_nLayer][index] = m_nMinG[m_nLayer][index] * m_nMinG[m_nLayer][index];
    m_nMaxBB[m_nLayer][index] = m_nMaxB[m_nLayer][index] * m_nMaxB[m_nLayer][index];
    m_nMinBB[m_nLayer][index] = m_nMinB[m_nLayer][index] * m_nMinB[m_nLayer][index];
}
/*!
 * \brief AIMainWidget::resetDataMaxMin
 */
void AIMainWidget::resetDataMaxMin(int index)
{
    m_nMaxR[m_nLayer][index] = 0;
    m_nMinR[m_nLayer][index] = 255;

    m_nMaxG[m_nLayer][index] = 0;
    m_nMinG[m_nLayer][index] = 255;

    m_nMaxB[m_nLayer][index] = 0;
    m_nMinB[m_nLayer][index] = 255;

    m_nMaxRG[m_nLayer][index] = 0;
    m_nMinRG[m_nLayer][index] = 65025;

    m_nMaxRB[m_nLayer][index] = 0;
    m_nMinRB[m_nLayer][index] = 65025;

    m_nMaxGB[m_nLayer][index] = 0;
    m_nMinGB[m_nLayer][index] = 65025;

    m_nMaxRR[m_nLayer][index] = 0;
    m_nMinRR[m_nLayer][index] = 65025;
    m_nMaxGG[m_nLayer][index] = 0;
    m_nMinGG[m_nLayer][index] = 65025;
    m_nMaxBB[m_nLayer][index] = 0;
    m_nMinBB[m_nLayer][index] = 65025;
}
/*!
 * \brief AIMainWidget::saveDataMaxMin
 * \param classMode
 * \return
 */
bool AIMainWidget::saveDataMaxMin(int classMode, int indexOfTrain)
{
    QFile file(QString("/tmp/train%1.range").arg(indexOfTrain));
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug("Save Max and Min data : File open error");
        return false;
    }
    QTextStream io(&file);
    if (classMode == MODE_MULTIPLE)
    {
        QString strMultiple = QString("x\n-1 1\n1 %1 %2\n2 %3 %4\n3 %5 %6\n4 %7 %8\n5 %9 %10\n6 %11 %12\n7 %13 %14\n8 %15 %16\n9 %17 %18\n")
            .arg(m_nMinR[m_nLayer][indexOfTrain]).arg(m_nMaxR[m_nLayer][indexOfTrain])
            .arg(m_nMinG[m_nLayer][indexOfTrain]).arg(m_nMaxG[m_nLayer][indexOfTrain])
            .arg(m_nMinB[m_nLayer][indexOfTrain]).arg(m_nMaxB[m_nLayer][indexOfTrain])
            .arg(m_nMinRG[m_nLayer][indexOfTrain]).arg(m_nMaxRG[m_nLayer][indexOfTrain])
            .arg(m_nMinRB[m_nLayer][indexOfTrain]).arg(m_nMaxRB[m_nLayer][indexOfTrain])
            .arg(m_nMinGB[m_nLayer][indexOfTrain]).arg(m_nMaxGB[m_nLayer][indexOfTrain])
            .arg(m_nMinR[m_nLayer][indexOfTrain] * m_nMinR[m_nLayer][indexOfTrain]).arg(m_nMaxR[m_nLayer][indexOfTrain] * m_nMaxR[m_nLayer][indexOfTrain])
            .arg(m_nMinG[m_nLayer][indexOfTrain] * m_nMinG[m_nLayer][indexOfTrain]).arg(m_nMaxG[m_nLayer][indexOfTrain] * m_nMaxG[m_nLayer][indexOfTrain])
            .arg(m_nMinB[m_nLayer][indexOfTrain] * m_nMinB[m_nLayer][indexOfTrain]).arg(m_nMaxB[m_nLayer][indexOfTrain] * m_nMaxB[m_nLayer][indexOfTrain]);
        io << strMultiple;
    }
    else
    {
        QString strUsual = QString("x\n-1 1\n1 %1 %2\n2 %3 %4\n3 %5 %6\n")
            .arg(m_nMinR[m_nLayer][indexOfTrain]).arg(m_nMaxR[m_nLayer][indexOfTrain])
            .arg(m_nMinG[m_nLayer][indexOfTrain]).arg(m_nMaxG[m_nLayer][indexOfTrain])
            .arg(m_nMinB[m_nLayer][indexOfTrain]).arg(m_nMaxB[m_nLayer][indexOfTrain]);
        io << strUsual;
    }

    file.close();
    return true;
}
/*!
 * \brief AIMainWidget::saveDataFromImg
 * \param classLevel
 * \param indexOfImg
 * \param indexOfTrain
 * \return
 */
bool AIMainWidget::saveDataFromImg(int nDataFrom, int classMode, int classLevel, int indexOfImg, int indexOfTrain)
{
    QImage image;
    QColor rgb;
    HSVType hsv;

    QFile file(QString("/tmp/train%1").arg(indexOfTrain));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug("Save data : File open error");
        return false;
    }
    qDebug() << file.size();
    if (file.size() > 12582912) //12M
    {
        qDebug("Save data : data max error");
        file.close();
        return false;
    }
    QTextStream io(&file);
    image.load(QString("/tmp/%1.png").arg(indexOfImg));

    QFile sample_ponit_file(QString("/tmp/sample%1").arg(indexOfImg));
    if (!sample_ponit_file.open(QIODevice::WriteOnly))
    {
        qDebug("Save Max and Min data : File open error");
        return false;
    }
    QTextStream io2(&sample_ponit_file);

    if (nDataFrom == DATA_IMG)
    { //From Img
        for (int i = 0; i < IMAGE_HEIGHT; i++)
        {
            for (int j = 0; j < IMAGE_WIDTH; j++)
            {
                rgb = image.pixel(j, i);
                if (myAIShare.pixelIsBackgroundcomm((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue(), j))
                {
                    continue;
                }

                if (pixelIsEdgeImage(&image, j, i) && myAIShare.m_bIsOptSam[indexOfImg])
                {
                    continue;
                }

                if (IsRGBExit((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue()))
                {
                    continue;
                }
                else
                {
                    if (m_nRGBNum < IMAGE_WIDTH * IMAGE_HEIGHT)
                    {
                        m_cRGBStat[m_nRGBNum][0] = (uchar)rgb.red();
                        m_cRGBStat[m_nRGBNum][1] = (uchar)rgb.green();
                        m_cRGBStat[m_nRGBNum][2] = (uchar)rgb.blue();
                        m_nRGBNum++;
                    }
                }

                QString strUsual = QString("%1: %2\n").arg(j).arg(i);
                io2 << strUsual;

                if (struCnfp.nColorSpace)
                {
                    hsv = myAIShare.RGBToHSV(rgb.red(), rgb.green(), rgb.blue());


                    /* 获取最大最小值*/
                    getDataMaxMin(indexOfTrain, hsv.x, hsv.y, hsv.z);

                    if (classMode == MODE_MULTIPLE)
                    {
                        if (struCnfp.nAIMode)
                        {
                            QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t\n")
                                .arg(classLevel).arg(hsv.x).arg(hsv.y).arg(hsv.z)
                                .arg(hsv.x * hsv.y).arg(hsv.x * hsv.z).arg(hsv.y * hsv.z);
                            io << strMulti;
                        }
                        else
                        {
                            QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t7:%8\t8:%9\t9:%10\t\n")
                                .arg(classLevel).arg(hsv.x).arg(hsv.y).arg(hsv.z)
                                .arg(hsv.x * hsv.y).arg(hsv.x * hsv.z).arg(hsv.y * hsv.z)
                                .arg(hsv.x * hsv.x).arg(hsv.y * hsv.y).arg(hsv.z * hsv.z);
                            io << strMulti;
                        }
                    }
                    else
                    {
                        QString strUsual = QString("%1\t1:%2\t2:%3\t3:%4\t\n")
                            .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
                        io << strUsual;
                    }
                }
                else
                {
                    /* 获取最大最小值*/
                    getDataMaxMin(indexOfTrain, rgb.red(), rgb.green(), rgb.blue());

                    if (classMode == MODE_MULTIPLE)
                    {
                        if (struCnfp.nAIMode)
                        {
                            QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t\n")
                                .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                                .arg(rgb.red() * rgb.green()).arg(rgb.red() * rgb.blue()).arg(rgb.green() * rgb.blue());
                            io << strMulti;
                        }
                        else
                        {
                            QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t7:%8\t8:%9\t9:%10\t\n")
                                .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                                .arg(rgb.red() * rgb.green()).arg(rgb.red() * rgb.blue()).arg(rgb.green() * rgb.blue())
                                .arg(rgb.red() * rgb.red()).arg(rgb.green() * rgb.green()).arg(rgb.blue() * rgb.blue());
                            io << strMulti;
                        }
                    }
                    else
                    {
                        QString strUsual = QString("%1\t1:%2\t2:%3\t3:%4\t\n")
                            .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
                        io << strUsual;
                    }
                }
            }
        }
        sample_ponit_file.close();
    }
    else
    {//From Disk
        QFile fileData(QString("/tmp/%1").arg(indexOfImg));
        if (!fileData.open(QIODevice::ReadOnly))
        {
            qDebug("read data : File open error");
            return false;
        }
        QTextStream ioData(&fileData);

        int nPosX, nPosY;
        for (int i = 0; i < myAIShare.m_nSelPixelNum[indexOfImg];i++)
        {
            ioData >> nPosX;
            ioData >> nPosY;

            if (myAIShare.m_bIsOptSam[indexOfImg])
            {
                if (pixelIsEdgeImage(&image, nPosX, nPosY))
                    continue;
            }

            rgb = image.pixel(nPosX, nPosY);

            if (IsRGBExit((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue()))
            {
                continue;
            }
            else
            {
                if (m_nRGBNum < IMAGE_WIDTH * IMAGE_HEIGHT)
                {
                    m_cRGBStat[m_nRGBNum][0] = (uchar)rgb.red();
                    m_cRGBStat[m_nRGBNum][1] = (uchar)rgb.green();
                    m_cRGBStat[m_nRGBNum][2] = (uchar)rgb.blue();
                    m_nRGBNum++;
                }
            }
            QString strUsual = QString("%1\n").arg(i);
            io2 << strUsual;

            if (struCnfp.nColorSpace)
            {
                hsv = myAIShare.RGBToHSV(rgb.red(), rgb.green(), rgb.blue());

                /* 获取最大最小值*/
                getDataMaxMin(indexOfTrain, hsv.x, hsv.y, hsv.z);

                if (classMode == MODE_MULTIPLE)
                {
                    if (struCnfp.nAIMode)
                    {
                        QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t\n")
                            .arg(classLevel).arg(hsv.x).arg(hsv.y).arg(hsv.z)
                            .arg(hsv.x * hsv.y).arg(hsv.x * hsv.z).arg(hsv.y * hsv.z);
                        io << strMulti;
                    }
                    else
                    {
                        QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t7:%8\t8:%9\t9:%10\t\n")
                            .arg(classLevel).arg(hsv.x).arg(hsv.y).arg(hsv.z)
                            .arg(hsv.x * hsv.y).arg(hsv.x * hsv.z).arg(hsv.y * hsv.z)
                            .arg(hsv.x * hsv.x).arg(hsv.y * hsv.y).arg(hsv.z * hsv.z);
                        io << strMulti;
                    }
                }
                else
                {
                    QString strUsual = QString("%1\t1:%2\t2:%3\t3:%4\t\n")
                        .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
                    io << strUsual;
                }
            }
            else
            {
                /* 获取最大最小值*/
                getDataMaxMin(indexOfTrain, rgb.red(), rgb.green(), rgb.blue());

                if (classMode == MODE_MULTIPLE)
                {
                    if (struCnfp.nAIMode)
                    {
                        QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t\n")
                            .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                            .arg(rgb.red() * rgb.green()).arg(rgb.red() * rgb.blue()).arg(rgb.green() * rgb.blue());
                        io << strMulti;
                    }
                    else
                    {
                        QString strMulti = QString("%1\t1:%2\t2:%3\t3:%4\t4:%5\t5:%6\t6:%7\t7:%8\t8:%9\t9:%10\t\n")
                            .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue())
                            .arg(rgb.red() * rgb.green()).arg(rgb.red() * rgb.blue()).arg(rgb.green() * rgb.blue())
                            .arg(rgb.red() * rgb.red()).arg(rgb.green() * rgb.green()).arg(rgb.blue() * rgb.blue());
                        io << strMulti;
                    }
                }
                else
                {
                    QString strUsual = QString("%1\t1:%2\t2:%3\t3:%4\t\n")
                        .arg(classLevel).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
                    io << strUsual;
                }
            }
        }
        fileData.close();
    }
    file.close();

    return true;
}
/*!
 * \brief AIMainWidget::enableAICal
 * \return
 */
int AIMainWidget::enableAICal()
{
    myMessageBox msgBox(MSG_INFO, "");

    m_bIsAutoGetData = false;

    m_nClassBadNum[m_nLayer] = 0;
    m_nClassGoodNum[m_nLayer] = 0;

    QString ld4Prefix;
    ld4Prefix.append("");


    for (int k = 0; k < MAX_IMG_NUM; k++)
    {
        if (m_nStatOfImg[m_nLayer][k] == CLASS_NULL)
        {
            continue;
        }
        if (m_nStatOfImg[m_nLayer][k] == CLASS_GOOD)
        {
            if (m_bSampedOfImg[k] == SAMPLED_NO)
            {
                m_bIsAutoGetData = true;
            }
            m_nClassGoodNum[m_nLayer]++;
        }
        if (m_nStatOfImg[m_nLayer][k] == CLASS_BAD)
        {
            if (m_bSampedOfImg[k] == SAMPLED_NO)
            {
                m_bIsAutoGetData = true;
            }
            m_nClassBadNum[m_nLayer]++;
        }
    }

    if (struCnfp.nIntelMode != MODE_NULL)
    {
        if (m_nClassGoodNum[m_nLayer] == 0 && m_nClassBadNum[m_nLayer] == 0)
        {
            msgBox.setText(ld4Prefix + myLan.ai_err_no_set);
            msgBox.exec();
            return ERR_NO_SET;
        }
        if (m_nClassGoodNum[m_nLayer] == 0)
        {
            msgBox.setText(ld4Prefix + myLan.ai_err_no_good);
            msgBox.exec();
            return ERR_NO_GOOD;
        }

        if (m_nClassBadNum[m_nLayer] == 0)
        {
            msgBox.setText(ld4Prefix + myLan.ai_err_no_bad);
            msgBox.exec();
            return ERR_NO_BAD;
        }

        if (struCnfp.nIntelMode == MODE_USUAL)
        {
            if (m_nClassGoodNum[m_nLayer] + m_nClassBadNum[m_nLayer] > MAX_USUAL_IMG_NUM)
            {
                msgBox.setText(ld4Prefix + myLan.ai_err_overflow);
                msgBox.exec();
                return ERR_OVERRUN;
            }
        }
    }

    return SUCCEED;
}

/*!
 * \brief AIMainWidget::enableAIArithmetic
 */
bool AIMainWidget::enableAIArithmetic(int classMode)
{
    if (classMode == MODE_USUAL)
    {
        switch (enableAICal())
        {
        case ERR_NO_BAD:
        case ERR_NO_GOOD:
        case ERR_OVERRUN:
        case ERR_NO_SET:
            return false;
        case SUCCEED:
            break;
        }
        if ((m_nClassGoodNum[m_nLayer] == 0) && (m_nClassBadNum[m_nLayer] == 0))
        {
            struCnfp.nMatIntelSeq = 0;
            m_nClassNum[m_nLayer] = 0;
        }
        if ((m_nClassGoodNum[m_nLayer] == 1) && (m_nClassBadNum[m_nLayer] == 1))
        {
            struCnfp.nMatIntelSeq = 1;
            m_nClassNum[m_nLayer] = 1;
        }
        if ((m_nClassGoodNum[m_nLayer] == 1) && (m_nClassBadNum[m_nLayer] == 2))
        {
            struCnfp.nMatIntelSeq = 2;
            m_nClassNum[m_nLayer] = 2;
        }
        if ((m_nClassGoodNum[m_nLayer] == 1) && (m_nClassBadNum[m_nLayer] == 3))
        {
            struCnfp.nMatIntelSeq = 3;
            m_nClassNum[m_nLayer] = 3;
        }
        if ((m_nClassGoodNum[m_nLayer] == 2) && (m_nClassBadNum[m_nLayer] == 1))
        {
            struCnfp.nMatIntelSeq = 4;
            m_nClassNum[m_nLayer] = 2;
        }
        if ((m_nClassGoodNum[m_nLayer] == 2) && (m_nClassBadNum[m_nLayer] == 2))
        {
            struCnfp.nMatIntelSeq = 6;
            m_nClassNum[m_nLayer] = 4;
        }
        if ((m_nClassGoodNum[m_nLayer] == 3) && (m_nClassBadNum[m_nLayer] == 1))
        {
            struCnfp.nMatIntelSeq = 5;
            m_nClassNum[m_nLayer] = 3;
        }
        switch (m_nClassNum[m_nLayer])
        {
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
    }
    else if (classMode == MODE_MULTIPLE)
    {
        struCnfp.nMatIntelSeq = 1;
    }
    else
    {
        struCnfp.nArithmeticEnable[ARITH_INTEL_A] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_B] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_C] = 0;
        struCnfp.nArithmeticEnable[ARITH_INTEL_D] = 0;
        struCnfp.nMatIntelSeq = 0;
        m_nClassNum[m_nLayer] = 0;
        updateBtnStatus();
    }

    /* only for tea */
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            struCnfp.nArithmeticEnableLevel[i][ARITH_INTEL_A + j] = struCnfp.nArithmeticEnable[ARITH_INTEL_A + j];
        }
    }

    return true;
}
/*!
 * \brief AIMainWidget::IsRGBExit
 * \param r
 * \param g
 * \param b
 * \return
 */
bool AIMainWidget::IsRGBExit(uchar r, uchar g, uchar b)
{
    for (int t = 0; t < m_nRGBNum; t++)
    {
        if (m_cRGBStat[t][0] == r && m_cRGBStat[t][1] == g && m_cRGBStat[t][2] == b)
        {
            return true;
        }
    }
    return false;
}

/*!
 * \brief AIMainWidget::doUnsupervisedLearning
 */
void AIMainWidget::doUnsupervisedLearning()
{
    int nFeederEn[10];
    int feederTemp[10];
    int nAlarmTmp[16];
    int bStatFeed = struGsh.bStatFeed;

    //! 增加清灰
    struGsh.bFlagManuwipe = 1;
    myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration + 3);

    //! 复合清灰模式
    if (struCnfg.nWipeMode)
    {
        myFlow.sleep(struCnfp.struGroupCtrl[0].nWipeDuration2);
    }

    //! 屏蔽报警
    for (int i = 0; i < 16;i++)
    {
        nAlarmTmp[i] = struCnfg.nAlarmEnable[i];
        struCnfg.nAlarmEnable[i] = 0;
    }

    //! 供料打开
    infoWidget->setLabelText(myLan.ai_image_capturing);
    infoWidget->delayShow();

    //! 设置供料量
    int feedEnd;
    feedEnd = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[0].nUnitCount / 2 - 1;
    for (int i = 0; i < 10; i++)
    {
        feederTemp[i] = struCnfp.struGroupCtrl[0].nFeeder[i];
        nFeederEn[i] = struCnfp.struGroupCtrl[0].nFeederEnable[i];
    }

    if (struCnfg.nAutoFeederSet == 1)
    {
        struCnfp.struGroupCtrl[0].nFeeder[struCnfg.nAutoReferChute - 1] = 150;
    }
    struCnfp.struGroupCtrl[0].nFeederEnable[struCnfg.nAutoReferChute - 1] = 1;
    myFlow.resetFeeder(0);

    for (int i = 0; i < 10; i++)
    {
        LOG_INFO_STM("idx:" << i << " Feeder:" << struCnfp.struGroupCtrl[0].nFeeder[i] << " FeederStatus:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
    }

    struGsh.nUnit = (struCnfg.nAutoReferChute - 1) * 2 + 1;
    if (bStatFeed != 1)
    {
        myFlow.onOff();
        myFlow.sleep(8);
    }

    //! 若吹气频率线程开启则关闭
    bool bIsRunning = false;
    if (g_ejectTimesThread->isRunning())
    {
        g_ejectTimesThread->stopStatRun();
        bIsRunning = true;
        myFlow.sleep(5);
    }

    m_nNumOfImg = 0;
    bool bGetImageIsOk = false;
    for (m_nNumOfImg = 0; m_nNumOfImg < 3; m_nNumOfImg++)
    {
        bGetImageIsOk = false;
        bGetImageIsOk = getImageFromUSB_2048_All();
        if (!bGetImageIsOk)
        {
            break;
        }
    }
    if (!IsImageOk(6))
    {
        bGetImageIsOk = false;
    }
    // update image
    m_nNumOfImg *= 2;
    updateImgList();
    updateImgListStat();
    updateBtnStatus();

    for (int i = 0; i < 10; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = feederTemp[i];
        struCnfp.struGroupCtrl[0].nFeederEnable[i] = nFeederEn[i];
    }

    //! 图片采集出错
    if (!bGetImageIsOk)
    {
        //! 供料关闭
        if (bStatFeed != 1)
        {
            myFlow.onOff();
        }
        else
        {
            //! 恢复吹气频率线程状态
            if (bIsRunning)
            {
                g_ejectTimesThread->start();
            }
        }
        infoWidget->setLabelText(myLan.ai_image_capture_error);
        infoWidget->showSecs();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    infoWidget->hide();

    //! 防止屏保
    struGsh.nBacklightCounter = 0;

    //! 主循环
    infoWidget->setLabelText(myLan.msg_computing);
    infoWidget->delayShow();
    unsupervisedLearning();

    //! 供料关闭
    if (bStatFeed != 1)
    {
        myFlow.onOff();
    }
    else
    {
        //! 恢复吹气频率线程状态
        if (bIsRunning)
        {
            g_ejectTimesThread->start();
        }
    }
    myFlow.resetSortParams();
    infoWidget->hide();
}

/*!
 * \brief AIMainWidget::unsupervisedLearning
 */
void AIMainWidget::unsupervisedLearning()
{
    int tmpImgNum = 6;
    int nFlagImg = 0;
    int tmpFrontUnit, tmpRearUnit, tmpUnit;
    tmpFrontUnit = struCnfg.nAutoReferChute * 2;
    tmpRearUnit = struCnfg.nAutoReferChute * 2 + 1;
    tmpUnit = struGsh.nUnit;

    //! 智能
    if (struCnfg.nEnableAI && (struCnfg.nProfileMode[struCnfg.nProfile] == RSC_DIFF_POSITIVE_SORT || struCnfg.nProfileMode[struCnfg.nProfile] == RSC_YELLOW_WHITE_SORT))
    {
        m_nLayer = 1;
        struGsh.nUnit = tmpRearUnit;
        saveAIConfig(struCnfg.nMaterPer1, tmpImgNum, nFlagImg, 1, struCnfg.nReservedSens);

        //! 防止屏保
        struGsh.nBacklightCounter = 0;
        //#ifdef Q_OS_UNIX
        //        char cmd[64];
        //        sprintf(cmd,"./myAISample");
        //        system(cmd);
        //#endif
        //#ifdef Q_OS_WIN32
        //        QProcess p(0);
        //        p.start("myAISample.exe");
        //        p.waitForFinished();
        //#endif
        QFile file;
        int maxQuo = 0;

        file.setFileName(QString("/tmp/train%1.result").arg(0));
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug("result file open error!");
            return;
        }

        QTextStream io(&file);
        maxQuo = 3;

        for (int i = 0; i < maxQuo; i++)
        {
            io >> m_struClassQuo[m_nLayer].nQuo[i];
            qDebug("nQuo[%d] = %f\n", i, m_struClassQuo[m_nLayer].nQuo[i]);
        }
        io >> m_struClassQuo[m_nLayer].nCons;     //cons*/
        qDebug("nCons = %f\n", m_struClassQuo[m_nLayer].nCons);
        file.close();
        //        file.remove(QString("/tmp/train%1.result").arg(0));
        //        file.remove(QString("/tmp/train%1").arg(0));
        //        file.remove(QString("/tmp/train%1.range").arg(0));

                //! 后视全部相同
        for (int i = 0; i < struCnfg.nLevelTotal; i++)
        {
            for (int j = 0; j < MAX_GROUP_IDTNTIFY; j++)
            {
                if (j % 2 == 1)
                {
                    getGlobalAIParam(MODE_MULTIPLE, struGsh.nLevel, j, 0);
                    struCnfp.struGroupIdentify[i][j].struIntel[0].nSens = 100;
                }
            }
        }

        enableAIArithmetic(MODE_MULTIPLE);
    }
    struGsh.nUnit = tmpUnit;
}
/*!
 * \brief AIMainWidget::IsImageOk
 * \param nImgNum
 * \return
 */
bool AIMainWidget::IsImageOk(int nImgNum)
{
    bool tmpLoad = true;
    int nImageData = 0;
    QImage image;
    QColor rgb;
    //! 读取图像
    for (int n = 0; n < nImgNum;n++)
    {
        tmpLoad = image.load(QString("/tmp/%1.png").arg(n));

        if (!tmpLoad)
            return false;

        for (int i = 0; i < IMAGE_HEIGHT; i++)
        {
            for (int j = 0; j < IMAGE_WIDTH; j++)
            {
                rgb = image.pixel(j, i);

                if (myAIShare.pixelIsBackgroundcomm((uchar)rgb.red(), (uchar)rgb.green(), (uchar)rgb.blue(), j))
                {
                    continue;
                }

                if (pixelIsEdgeImage(&image, j, i))
                {
                    continue;
                }
                nImageData++;
            }
        }
    }

    qDebug() << nImageData;
    if (nImageData < 2000)
    {
        return false;
    }
    return true;
}
/*!
 * \brief AIMainWidget::saveAIConfig
 * \param nMaterPer
 * \param nImgNum
 * \param nFlgView
 * \param nResMode
 * \param nResSens
 * \return
 */
bool AIMainWidget::saveAIConfig(int nMaterPer, int nImgNum, int nFlgView, int nResMode, int nResSens)
{
    QFile file("/tmp/config");
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug("Save Config : File open error");
        return false;
    }
    QTextStream io(&file);

    QString strConfig = QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8\n%9\n%10\n%11\n%12\n%13\n")
        .arg(struCnfp.nBackgroundColor[struGsh.nLevel][struGsh.nUnit % 2])
        .arg(struCnfp.nBackgroundColorValue[struGsh.nLevel][struGsh.nUnit % 2])
        .arg(struCnfp.nBackgroundColorPer[struGsh.nLevel][struGsh.nUnit % 2])
        .arg(3)
        .arg(nImgNum)
        .arg(nFlgView)
        .arg(nResMode)
        .arg(nResSens)
        .arg(nResMode)
        .arg(2)
        .arg(1)
        .arg(45)
        .arg(0);

    io << strConfig;
    file.close();

    return true;
}
/*!
 * \brief AIMainWidget::createAIImageWidget
 */
void AIMainWidget::createAIImageWidget()
{
    m_imgImportBtn = new myPushButton(myLan.ai_img_import, myIcon.Action_Restore, true);
    m_imgExportBtn = new myPushButton(myLan.ai_img_export, myIcon.Action_Backup, true);
    m_imgBackBtn = new myPushButton(myLan.back, myIcon.Action_Back, true);
    m_imgThresholdLabel = new myLabel(myLan.ai_img_threshold);
    m_imgThresholdLineEdit = new myLineEdit(QString("%1").arg(struGsh.nIntelSampBurst));
    m_highSpeedCbx = new myCustomCheckBox(myLan.ai_high_speed, true, CB_STYLE_CIRCLE);
    m_lowSpeedCbx = new myCustomCheckBox(myLan.ai_low_speed, true, CB_STYLE_CIRCLE);
    m_rangeLbl = new myLabel(myLan.channel_range);
    m_beginChLe = new myLineEdit(QString("%1").arg(0));
    myLabel* tmpLbl = new myLabel(" - ");
    m_endChLe = new myLineEdit(QString("%1").arg(struCnfc.nSensorPixel - 1));
    m_beginChLe->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_endChLe->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    m_AIParaAutoCopy = new myCustomCheckBox(myLan.auto_copy, false);
    m_AICuteOffBgCBx = new myCustomCheckBox(myLan.cuteoff_bg, m_bIsCuteOffBg);
    m_enableAIAuto1CBx = new myCustomCheckBox(myLan.enable + '1', false);
    m_enableAIAuto2CBx = new myCustomCheckBox(myLan.enable + '2', false);
    m_reservedSensLabel = new myLabel(myLan.ai_reserved_sens);
    m_reservedSensLbe = new myLineEdit(QString("%1").arg(struCnfg.nReservedSens));
    m_materPer1Label = new myLabel("异色");
    m_materPer1Lbe = new myLineEdit(QString("%1%").arg(struCnfg.nMaterPer1 / 10.0));
    m_materPer2Label = new myLabel("腹白");
    m_materPer2Lbe = new myLineEdit(QString("%1%").arg(struCnfg.nMaterPer2 / 10.0));
    m_materPer3Label = new myLabel("透明");
    m_materPer3Lbe = new myLineEdit(QString("%1%").arg(struCnfg.nMaterPer3 / 10.0));

    m_inputParamsBtn = new myPushButton(myLan.input_ai_params, myIcon.Edit_Rename, true, true);

    myGroupBox* imgDisposeGBox = new myGroupBox(myLan.ai_img_import_export);
    myGroupBox* otherGBox = new myGroupBox(myLan.other);
    myGroupBox* advGb = new myGroupBox(myLan.advanced_parameters);
    myGroupBox* captureModeGb = new myGroupBox(myLan.capture + myLan.mode);

    m_reservedSensLbe->setMaximumSize(BTN_WIDTH, BTN_HEIGHT);
    m_enableAIAuto2CBx->hide();
    m_materPer1Label->hide();
    m_materPer1Lbe->hide();
    m_materPer2Label->hide();
    m_materPer2Lbe->hide();
    m_materPer3Label->hide();
    m_materPer3Lbe->hide();

    m_inputParamsBtn->setMaximumSize(QSize(BTN_WIDTH * 1.5, BTN_HEIGHT));
    m_imgThresholdLabel->setMaximumSize(QSize(BTN_WIDTH * 1.5, BTN_HEIGHT));
    m_imgThresholdLineEdit->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_imgImportBtn->setMaximumSize(QSize(BTN_WIDTH * 1.5, BTN_HEIGHT));
    m_imgExportBtn->setMaximumSize(QSize(BTN_WIDTH * 1.5, BTN_HEIGHT));
    m_imgBackBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    //! 颜色空间与高低维
    myLabel* colorSpaceLbl = new myLabel(myLan.ai_color_space);
    QStringList strList;
    strList << myLan.ai_img_colorSpace1 << myLan.ai_img_colorSpace2;
    m_colorSpaceBtnGp = new MyButtonGroup(strList);
    myLabel* dimensionLbl = new myLabel(myLan.ai_mode);
    strList.clear();
    strList << myLan.ai_mode1 << myLan.ai_mode2;
    m_dimensionBtnGp = new MyButtonGroup(strList);
    QGridLayout* advLay = new QGridLayout(advGb);
    advLay->addWidget(colorSpaceLbl, 0, 0, 1, 1);
    advLay->addWidget(m_colorSpaceBtnGp, 0, 1, 1, 2);
    advLay->addWidget(dimensionLbl, 1, 0, 1, 1);
    advLay->addWidget(m_dimensionBtnGp, 1, 1, 1, 2);
    advLay->addWidget(m_imgThresholdLabel, 2, 0, 1, 1);
    advLay->addWidget(m_imgThresholdLineEdit, 2, 1, 1, 2);

    //! 页面布局
    QVBoxLayout* imgMainVBLayout = new QVBoxLayout(m_AIImageWidget);

    QGridLayout* imgLeftGridLayout = new QGridLayout(imgDisposeGBox);
    imgLeftGridLayout->setContentsMargins(10, 10, 10, 10);
    imgLeftGridLayout->addWidget(m_imgImportBtn, 0, 0);
    imgLeftGridLayout->addWidget(m_imgExportBtn, 1, 0);

    QGridLayout* imgRightDownGridLayout = new QGridLayout(otherGBox);
    imgRightDownGridLayout->setContentsMargins(10, 10, 10, 10);
    imgRightDownGridLayout->setHorizontalSpacing(10);

    imgRightDownGridLayout->addWidget(m_inputParamsBtn, 0, 0);
    if (myFlow.getProductLineNo() == 0)
    {
        imgRightDownGridLayout->addWidget(m_AIParaAutoCopy, 1, 0);

        m_rangeLbl->hide();
        m_beginChLe->hide();
        tmpLbl->hide();
        m_endChLe->hide();

        if (struCnfe.nEnableMaizeDoubleView == 1)
        {
            m_AIParaAutoCopy->hide();
        }
    }
    else
    {
        imgRightDownGridLayout->addWidget(m_rangeLbl, 1, 0);
        imgRightDownGridLayout->addWidget(m_beginChLe, 1, 1);
        imgRightDownGridLayout->addWidget(tmpLbl, 1, 2);
        imgRightDownGridLayout->addWidget(m_endChLe, 1, 3);
        m_AIParaAutoCopy->hide();
    }

    QGridLayout* enableAIAutoGridLayout = new QGridLayout(captureModeGb);
    enableAIAutoGridLayout->setContentsMargins(10, 10, 10, 10);
    enableAIAutoGridLayout->setVerticalSpacing(10);
    enableAIAutoGridLayout->addWidget(m_highSpeedCbx, 0, 0, 1, 1, Qt::AlignCenter);
    enableAIAutoGridLayout->addWidget(m_lowSpeedCbx, 1, 0, 1, 1, Qt::AlignCenter);

    QGridLayout* imgMainUpGridLayout = new QGridLayout();
    imgMainUpGridLayout->setContentsMargins(10, 10, 10, 10);
    imgMainUpGridLayout->setHorizontalSpacing(10);
    imgMainUpGridLayout->addWidget(imgDisposeGBox, 0, 0);
    imgMainUpGridLayout->addWidget(advGb, 0, 1);
    imgMainUpGridLayout->addWidget(captureModeGb, 1, 0);
    imgMainUpGridLayout->addWidget(otherGBox, 1, 1);

    QHBoxLayout* imgBackHBLayout = new QHBoxLayout();
    QSpacerItem* horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    imgBackHBLayout->addItem(horizontalSpacer);
    imgBackHBLayout->addWidget(m_imgBackBtn);

    imgMainVBLayout->addLayout(imgMainUpGridLayout);
    imgMainVBLayout->addLayout(imgBackHBLayout);

    //! 刷新智能参数页面显示
    m_colorSpaceBtnGp->setCurrentIndex(struCnfp.nColorSpace);
    m_dimensionBtnGp->setCurrentIndex(struCnfp.nAIMode);
    if (struGsh.nIntelSampType)
    {
        m_highSpeedCbx->setChecked(true);
        m_lowSpeedCbx->setChecked(false);
    }
    else
    {
        m_highSpeedCbx->setChecked(false);
        m_lowSpeedCbx->setChecked(true);
    }

    if (struCnfe.nEnableMaizeDoubleView == 1)
    {
        m_dimensionBtnGp->setEnabled(false);
    }


    if (struCnfg.nEnableAI)
    {
        m_enableAIAuto1CBx->setChecked(true);
    }
    else
    {
        m_enableAIAuto1CBx->setChecked(false);
    }

    if (struCnfg.nEnableTradition)
    {
        m_enableAIAuto2CBx->setChecked(true);
    }
    else
    {
        m_enableAIAuto2CBx->setChecked(false);
    }

    //! 信号与槽
    connect(m_imgImportBtn, SIGNAL(pressed()), this, SLOT(onImgImportBtnPressedSlt()));
    connect(m_imgExportBtn, SIGNAL(pressed()), this, SLOT(onImgExportBtnPressedSlt()));
    connect(m_imgBackBtn, SIGNAL(pressed()), this, SLOT(onImgBackBtnPressedSlt()));
    connect(m_imgThresholdLineEdit, SIGNAL(pressed()), this, SLOT(onImgThresholdLineEditPressedSlt()));
    connect(m_highSpeedCbx, SIGNAL(pressed()), this, SLOT(onHighSpeedPressedSlt()));
    connect(m_lowSpeedCbx, SIGNAL(pressed()), this, SLOT(onLowSpeedPressedSlt()));
    connect(m_AIParaAutoCopy, SIGNAL(pressed()), this, SLOT(onAIParaAutoCopyPressedSlt()));
    connect(m_AICuteOffBgCBx, SIGNAL(pressed()), this, SLOT(onAICuteOffBgPressedSlt()));
    connect(m_beginChLe, SIGNAL(pressed()), this, SLOT(onBeginChLePressedSlt()));
    connect(m_endChLe, SIGNAL(pressed()), this, SLOT(onEndChLePressedSlt()));
    connect(m_enableAIAuto1CBx, SIGNAL(pressed()), this, SLOT(onEnableAIAuto1CBXPressedSlt()));
    connect(m_enableAIAuto2CBx, SIGNAL(pressed()), this, SLOT(onEnableAIAuto2CBXPressedSlt()));
    connect(m_reservedSensLbe, SIGNAL(pressed()), this, SLOT(onReservedSensLbePressedSlt()));
    connect(m_materPer1Lbe, SIGNAL(pressed()), this, SLOT(onMaterPer1LbePressedSlt()));
    connect(m_materPer2Lbe, SIGNAL(pressed()), this, SLOT(onMaterPer2LbePressedSlt()));
    connect(m_materPer3Lbe, SIGNAL(pressed()), this, SLOT(onMaterPer3LbePressedSlt()));
    connect(m_inputParamsBtn, SIGNAL(pressed()), this, SLOT(onAIParamsInputSlt()));
    connect(m_colorSpaceBtnGp, SIGNAL(currentChanged(int)), this, SLOT(onColorSpaceBtnGpIndexChangedSlt(int)));
    connect(m_dimensionBtnGp, SIGNAL(currentChanged(int)), this, SLOT(onDimensionBtnGpIndexChangedSlt(int)));
}

/* 智能模式参数输入页面*/
void AIMainWidget::createAIModeParamsWidget()
{
    QFont font;
    if (LCD_WIDTH == 1024)
    {
        font.setPixelSize(24);
    }
    else
    {
        font.setPixelSize(16);
    }
    myFlow.getArithmeticName();
    tabBar = new QTabBar(m_AIParamsSetWidget);
    tabBar->setFont(font);
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nIdentifyGroupTotal; j++)
        {
            tabBar->insertTab(i * struCnfg.struLevelInfo[i].nIdentifyGroupTotal + j, myString.sIdentifyGroupName[i][j]);
        }
    }
    tabBar->setStyleSheet("QTabBar::tab{""min-height:40;min-width:100;}::scroller{""width:30;}");

    nameGroup = new myGroupBox(myLan.defect_name, m_AIParamsSetWidget);
    nameGroup->setMaximumHeight(100);

    nameValue = new myLineEdit(tr(""));
    nameValue->setAlignment(Qt::AlignCenter);
    nameValue->setMinimumHeight(ICON_HEI);
    nameValue->setMaximumHeight(BTN_HEIGHT);

    AISensListCbx = new myGroupBox(myString.sArithmeticName[ARITH_INTEL_A + m_nIndexOfArithIntel], m_AIParamsSetWidget);
    AISensListCbx->setMaximumHeight(100);

    AISensLabel = new myLabel(myLan.sensitivity);
    AISensLbe = new myLineEdit("");
    AIRowLabel = new myLabel(myLan.scale);
    AIRowLbe = new myLineEdit("");
    AIPercentLabel = new myLabel(myLan.purity);
    AIPercentLbe = new myLineEdit("");

    AIParamsBackBtn = new myPushButton(myLan.back, QIcon(":/res/png/icon_back.png"), true, true, this);
    AIParamsBackBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    AISensLbe->setMinimumHeight(ICON_HEI);
    AISensLbe->setMaximumHeight(BTN_HEIGHT);
    AISensLbe->setMaximumWidth(BTN_WIDTH);

    AIRowLbe->setMinimumHeight(ICON_HEI);
    AIRowLbe->setMaximumHeight(BTN_HEIGHT);
    AIRowLbe->setMaximumWidth(BTN_WIDTH);

    AIPercentLbe->setMinimumHeight(ICON_HEI);
    AIPercentLbe->setMaximumHeight(BTN_HEIGHT);
    AIPercentLbe->setMaximumWidth(BTN_WIDTH);

    QVBoxLayout* mainVBoxLayout = new QVBoxLayout(m_AIParamsSetWidget);

    QHBoxLayout* nameLayout = new QHBoxLayout(nameGroup);
    nameLayout->setContentsMargins(10, 0, 10, 10);
    nameLayout->addWidget(nameValue);

    QHBoxLayout* AIHBoxLayout = new QHBoxLayout(AISensListCbx);
    AIHBoxLayout->addWidget(AISensLabel);
    AIHBoxLayout->addWidget(AISensLbe);
    AIHBoxLayout->addWidget(AIRowLabel);
    AIHBoxLayout->addWidget(AIRowLbe);
    AIHBoxLayout->addWidget(AIPercentLabel);
    AIHBoxLayout->addWidget(AIPercentLbe);

    QHBoxLayout* backBtnHBoxLayout = new QHBoxLayout();
    backBtnHBoxLayout->addWidget(AIParamsBackBtn, 0, Qt::AlignBottom | Qt::AlignRight);

    mainVBoxLayout->addWidget(tabBar);
    mainVBoxLayout->addStretch(0);
    mainVBoxLayout->addWidget(nameGroup);
    mainVBoxLayout->addStretch(0);
    mainVBoxLayout->addWidget(AISensListCbx);
    mainVBoxLayout->addStretch(0);
    mainVBoxLayout->addLayout(backBtnHBoxLayout);

    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(nameValue, SIGNAL(pressed()), this, SLOT(setImpurityNameSlt()));
    connect(AISensLbe, SIGNAL(pressed()), this, SLOT(setSensValueSlt()));
    connect(AIRowLbe, SIGNAL(pressed()), this, SLOT(setRowValueSlt()));
    connect(AIPercentLbe, SIGNAL(pressed()), this, SLOT(setPercentValueSlt()));
    connect(AIParamsBackBtn, SIGNAL(pressed()), this, SLOT(onAIParamsBackBtnPressedSlt()));
}

/*!
 * \brief AIMainWidget::onImgImportBtnPressedSlt
 */
void AIMainWidget::onImgImportBtnPressedSlt()
{
    bool bIsImg = true;
    QString strImg;
    QImage  image;
    QString shell;
    QStringList strList, strListDir;
    strList << "*.png";
    QDir dir("/proc/scsi/usb-storage");

    if (!dir.exists())
    {
        QDir pngDir("/sdcard/png");
        pngDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        if (pngDir.entryList().isEmpty() == true)
        {
            infoWidget->setLabelText(myLan.sd_no_imageBak_dir + QString("(png)"));
            infoWidget->showSecs();
            return;
        }
        QString selectDirStr;
        SelectFileDialog* dialog = new SelectFileDialog();
        dialog->setDir("/sdcard/png", strListDir);
        if (dialog->exec() == QDialog::Accepted)
        {
            selectDirStr = QString("/sdcard/png/") + dialog->getFileName();
            QDir selDir(selectDirStr);
            if (selDir.entryList(strList).isEmpty())
            {
                infoWidget->setLabelText(myLan.dir_no_png_file);
                infoWidget->showSecs();
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_copy_image);
                infoWidget->delayShow();

                /*添加功能：图片从SD卡导出后清空SD卡中的图片文件。*/
                myMessageBox msgBox(MSG_QUES, myLan.msg_clear_local_pics + "?");
                int ret = msgBox.exec();

                for (int i = 0; i < MAX_IMG_NUM; i++)
                {
                    strImg = selectDirStr + QString("/%1.png").arg(i);
                    bIsImg = image.load(strImg);
                    if (bIsImg)
                    {
                        if (ret == QDialog::Accepted)
                        {
                            shell = QString("mv -f ") + selectDirStr + QString("/%1.png /tmp/").arg(i);
                        }
                        else
                        {
                            shell = QString("cp -f ") + selectDirStr + QString("/%1.png /tmp/").arg(i);
                        }
                        qDebug("%s", qPrintable(shell));
                        system(qPrintable(shell));
                        myFlow.sleep(1);
                        m_nNumOfImg = i + 1;
                    }
                    else
                    {
                        break;
                    }
                }
                QDir dirTmp(selectDirStr);
                dirTmp.setFilter(QDir::NoDotAndDotDot | QDir::Files);
                if (dirTmp.entryList().isEmpty() == true)
                {
                    shell = QString("rm -rf ") + selectDirStr;
                    qDebug("%s", qPrintable(shell));
                    system(qPrintable(shell));
                    myFlow.sleep(1);
                }

                infoWidget->hide();
                if (m_nNumOfImg == 0)
                {
                    infoWidget->setLabelText(myLan.msg_import_image + myLan.failed);
                }
                else
                {
                    infoWidget->setLabelText(myLan.msg_import_image + myLan.success + tr("( ") + QString::number(m_nNumOfImg) + tr(" )"));
                }
                infoWidget->showSecs();
                system("sync");
                updateImgList();
                updateImgListStat();
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        if (!myFlow.mountUdisk())
        {
            infoWidget->setLabelText(myLan.msg_insert_udisk);
            infoWidget->delayShow();
            myFlow.sleep(1);
            infoWidget->hide();
            return;
        }
        QString pngDirStr = QString("/udisk/") + myString.sVendor;
        QDir pngDir(pngDirStr);
        pngDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        if (pngDir.entryList().isEmpty() == true)
        {
            infoWidget->setLabelText(myLan.usb_no_imageBak_dir + QString("(") + myString.sVendor + QString(")"));
            infoWidget->showSecs();
            return;
        }
        QString selectDir;
        SelectFileDialog* dialog = new SelectFileDialog();
        dialog->setDir(pngDirStr, strListDir);
        if (dialog->exec() == QDialog::Accepted)
        {
            selectDir = pngDirStr + QString("/") + dialog->getFileName();
            QDir selDir(selectDir);
            if (selDir.entryList(strList).isEmpty())
            {
                infoWidget->setLabelText(myLan.dir_no_png_file);
                infoWidget->showSecs();
                return;
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_copy_image);
                infoWidget->delayShow();

                for (int i = 0; i < MAX_IMG_NUM; i++)
                {
                    strImg = selectDir + QString("/%1.png").arg(i);
                    bIsImg = image.load(strImg);
                    if (bIsImg)
                    {
                        shell = QString("cp -f ") + selectDir + QString("/%1.png /tmp/").arg(i);
                        qDebug("%s", qPrintable(shell));
                        system(qPrintable(shell));
                        myFlow.sleep(1);
                        m_nNumOfImg = i + 1;
                    }
                    else
                    {
                        break;
                    }
                }

                infoWidget->hide();
                if (m_nNumOfImg == 0)
                {
                    infoWidget->setLabelText(myLan.msg_import_image + myLan.failed);
                }
                else
                {
                    infoWidget->setLabelText(myLan.msg_import_image + myLan.success + tr("( ") + QString::number(m_nNumOfImg) + tr(" )"));
                }
                infoWidget->showSecs();
                system("sync");
                myFlow.sleep(2);
                system("umount /udisk");
                myFlow.sleep(2);

                updateImgList();
                updateImgListStat();
            }
        }
        else
        {
            return;
        }
        return;
    }
}
/*!
 * \brief AIMainWidget::onImgExportBtnPressedSlt
 */
void AIMainWidget::onImgExportBtnPressedSlt()
{
    QDir dir("/proc/scsi/usb-storage");
    myMessageBox msgBox(MSG_QUES, myLan.ai_img_msg_export);
    int ret = msgBox.exec();
    if (ret == QDialog::Accepted)
    {
        if (dir.exists())
        {
            infoWidget->setLabelText(myLan.msg_copy_image);
            infoWidget->delayShow();
            char cmd[64];
            if (!myFlow.mountUdisk())
            {
                infoWidget->setLabelText(myLan.msg_insert_udisk);
                infoWidget->delayShow();
                myFlow.sleep(1);
                infoWidget->hide();
                return;
            }

            /* copy png files to usb storage */
            QDateTime time = QDateTime::currentDateTime();
            QString pngDirStr = QString("/udisk/%1/%2").arg(myString.sVendor).arg(time.toString("yyyyMMddhhmmss"));

            QString shell;
            shell.sprintf("mkdir -p %s", qPrintable(pngDirStr));
            qDebug("%s", qPrintable(shell));
            system(qPrintable(shell));
            myFlow.sleep(1);

            shell.sprintf("cp -f /tmp/*.png %s", qPrintable(pngDirStr));
            qDebug("%s", qPrintable(shell));
            system(qPrintable(shell));
            myFlow.sleep(1);
            system("sync");

            QDir pngDir(pngDirStr);
            QStringList filter;
            filter << "*.png";
            if (pngDir.entryInfoList(filter).count() == 0)
            {
                infoWidget->setLabelText(myLan.msg_export_image + myLan.failed);
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_export_image + myLan.success);
            }

            sprintf(cmd, "umount /udisk");
            system(cmd);
            myFlow.sleep(2);

            infoWidget->hide();
        }
        else
        {
            infoWidget->setLabelText(myLan.msg_copy_image);
            infoWidget->delayShow();

            /* copy png files to sdcard */
            QDateTime time = QDateTime::currentDateTime();
            QString pngDirStr = QString("/sdcard/png/%1").arg(time.toString("yyyyMMddhhmmss"));

            QString shell;
            shell.sprintf("mkdir -p %s", qPrintable(pngDirStr));
            qDebug("%s", qPrintable(shell));
            system(qPrintable(shell));
            myFlow.sleep(1);

            shell.sprintf("cp -f /tmp/*.png %s", qPrintable(pngDirStr));
            qDebug("%s", qPrintable(shell));
            system(qPrintable(shell));
            myFlow.sleep(2);
            system("sync");
            myFlow.sleep(2);

            QDir pngDir(pngDirStr);
            QStringList filter;
            filter << "*.png";
            if (pngDir.entryInfoList(filter).count() == 0)
            {
                infoWidget->setLabelText(myLan.msg_export_image + myLan.failed);
            }
            else
            {
                infoWidget->setLabelText(myLan.msg_export_image + myLan.success);
            }
            myFlow.sleep(2);
            infoWidget->hide();
        }
    }
}
/*!
 * \brief AIMainWidget::onImgThresholdLineEditPressedSlt
 */
void AIMainWidget::onImgThresholdLineEditPressedSlt()
{
    double nThreshold;
    nThreshold = m_imgThresholdLineEdit->text().toDouble();

    myInputPanel myInput(intType, 0, 255, nThreshold);
    int ret = myInput.exec();
    if (ret == QDialog::Accepted)
    {
        nThreshold = myInput.getValue();
        struGsh.nIntelSampBurst = (int)nThreshold;
        m_imgThresholdLineEdit->setText(QString("%1").arg(nThreshold));
    }
}

/*!
 * \brief AIMainWidget::onImgBackBtnPressedSlt
 */
void AIMainWidget::onImgBackBtnPressedSlt()
{
    m_stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

void AIMainWidget::onHighSpeedPressedSlt()
{
    if (m_highSpeedCbx->getChecked())
    {
        struGsh.nIntelSampType = 1;
        m_lowSpeedCbx->setChecked(false);
    }
    else
    {
        struGsh.nIntelSampType = 0;
        m_lowSpeedCbx->setChecked(true);
    }
}

void AIMainWidget::onLowSpeedPressedSlt()
{
    if (m_lowSpeedCbx->getChecked())
    {
        struGsh.nIntelSampType = 0;
        m_highSpeedCbx->setChecked(false);
    }
    else
    {
        struGsh.nIntelSampType = 1;
        m_highSpeedCbx->setChecked(true);
    }
}

void AIMainWidget::onAIParaAutoCopyPressedSlt()
{
    if (m_AIParaAutoCopy->getChecked())
    {
        for (int i = 0; i < MAX_AI; i++)
        {
            if (i < 2)
            {            //当期杂粮只使用智能模式A和B，2015.12.1
                m_bIsAutoCopy[m_nIndexOfArithIntel][i] = true;  //= isSelect[i]
            }
        }
    }
    else
    {
        for (int i = 0; i < MAX_AI; i++)
        {
            m_bIsAutoCopy[m_nIndexOfArithIntel][i] = false;
        }
    }
    autoCopyIntellArgs();
}

void AIMainWidget::onAICaptureAllViewPressedSlt()
{
    if (m_AICuteOffBgCBx->getChecked())
    {
        m_AICaptureAllView->setChecked(false);
    }
    m_bIsCaptureAllView = m_AICaptureAllView->getChecked();
}

void AIMainWidget::onAICuteOffBgPressedSlt()
{
    if (m_AICaptureAllView->getChecked())
    {
        m_AICuteOffBgCBx->setChecked(false);
    }
    m_bIsCuteOffBg = m_AICuteOffBgCBx->getChecked();
}

/*!
 * \brief AIMainWidget::onBeginChLePressedSlt
 */
void AIMainWidget::onBeginChLePressedSlt()
{
    int nMax = struCnfc.nSensorPixel - 1;
    int nVal = m_beginChLe->text().toInt();

    myInputPanel* inputPanel = new myInputPanel(intType, 0, nMax, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal > m_endChLe->text().toInt())
        {
            nVal = m_endChLe->text().toInt();
        }
        m_beginChLe->setText(QString("%1").arg(nVal));
    }
}

/*!
 * \brief AIMainWidget::onEndChLePressedSlt
 */
void AIMainWidget::onEndChLePressedSlt()
{
    int nMax = struCnfc.nSensorPixel - 1;
    int nVal = m_endChLe->text().toInt();

    myInputPanel* inputPanel = new myInputPanel(intType, 0, nMax, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal < m_beginChLe->text().toInt())
        {
            nVal = m_beginChLe->text().toInt();
        }
        m_endChLe->setText(QString("%1").arg(nVal));
    }
}

/*!
 * \brief AIMainWidget::onEnableAIAuto1CBXPressedSlt
 */
void AIMainWidget::onEnableAIAuto1CBXPressedSlt()
{
    if (struCnfg.nEnableAI)
    {
        struCnfg.nEnableAI = 0;
        m_enableAIAuto1CBx->setChecked(false);
    }
    else
    {
        struCnfg.nEnableAI = 1;
        m_enableAIAuto1CBx->setChecked(true);
    }
}

/*!
 * \brief AIMainWidget::onEnableAIAuto2CBXPressedSlt
 */
void AIMainWidget::onEnableAIAuto2CBXPressedSlt()
{
    if (struCnfg.nEnableTradition)
    {
        struCnfg.nEnableTradition = 0;
        m_enableAIAuto2CBx->setChecked(false);
    }
    else
    {
        struCnfg.nEnableTradition = 1;
        m_enableAIAuto2CBx->setChecked(true);
    }
}

/*!
 * \brief AIMainWidget::onReservedSensLbePressedSlt
 */
void AIMainWidget::onReservedSensLbePressedSlt()
{
    int nVal = struCnfg.nReservedSens;
    myInputPanel* inputPanel = new myInputPanel(intType, 0, 360, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal > 360)
        {
            nVal = 360;
        }
        struCnfg.nReservedSens = nVal;
        m_reservedSensLbe->setText(QString("%1").arg(nVal));
    }
}

/*!
 * \brief AIMainWidget::onMaterPer1LbePressedSlt
 */
void AIMainWidget::onMaterPer1LbePressedSlt()
{
    float nVal = struCnfg.nMaterPer1 / 10;

    myInputPanel* inputPanel = new myInputPanel(floatType, 0, 100, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal > 100)
        {
            nVal = 100;
        }
        struCnfg.nMaterPer1 = nVal * 10;
        m_materPer1Lbe->setText(QString("%1%").arg(nVal));
    }
}

/*!
 * \brief AIMainWidget::onMaterPer2LbePressedSlt
 */
void AIMainWidget::onMaterPer2LbePressedSlt()
{
    float nVal = struCnfg.nMaterPer2 / 10;

    myInputPanel* inputPanel = new myInputPanel(floatType, 0, 100, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal > 100)
        {
            nVal = 100;
        }

        struCnfg.nMaterPer2 = nVal * 10;
        m_materPer2Lbe->setText(QString("%1%").arg(nVal));
    }
}

/*!
 * \brief AIMainWidget::onMaterPer3LbePressedSlt
 */
void AIMainWidget::onMaterPer3LbePressedSlt()
{
    float nVal = struCnfg.nMaterPer3 / 10;

    myInputPanel* inputPanel = new myInputPanel(floatType, 0, 100, nVal);
    if (inputPanel->exec() == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal > 100)
        {
            nVal = 100;
        }
        struCnfg.nMaterPer3 = nVal * 10;
        m_materPer3Lbe->setText(QString("%1%").arg(nVal));
    }
}

/* 判断原始图像像素点是否为背景点 */
bool AIMainWidget::pixelIsBackground(uchar r, uchar g, uchar b, int nUnitAddr)
{
    int backgroudID = 0;
    backgroudID = nUnitAddr % 2;

    //杂粮自动背景后上传的图像背景为黑（r:0  g:0  b:0）
    if (myFlow.getProductLineNo() == 0)
    {
        if (struCnfp.nAutoBgEnable[struGsh.nLevel][nUnitAddr] == 1)
        {
            if ((r == 0) && (g == 0) && (b == 0))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    switch (struCnfp.nBackgroundColor[struGsh.nLevel][backgroudID])
    {
    case 0: //黑
        if (r + g + b < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] * 3)
            return true;
        break;
    case 1: //红
        if (r > struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0 * r / (r + g + b) > struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 2: //绿
        if (g > struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0 * g / (r + g + b) > struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 4: //蓝
        if (b > struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] && 100.0 * b / (r + g + b) > struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID])
            return true;
        break;
    case 7: //白
        if (r + g + b > struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] * 3 &&
            r + g + b < struCnfp.nBackgroundColorValue[struGsh.nLevel][backgroudID] * 3 + (int)struCnfp.nBackgroundColorPer[struGsh.nLevel][backgroudID] * 2.55)
            return true;
        break;
    default:
        break;
    }
    return false;
}
/*!
 * \brief 槽函数：杂质名称
 */
void AIMainWidget::onImpurityNameLePressed()
{
    QString str = m_impurityNameLe->text();
    myInputMethod* input = new myInputMethod(myLan.input, str);
    if (input->exec() == QDialog::Accepted)
    {
        str = input->getText();
        m_impurityNameLe->setText(str);

        int nIdeGp = myFlow.getIdentifyGroupID(struGsh.nUnit, 0);
        strcpy(struCnfp.struGroupIdentify[ONE_LEVEL][nIdeGp].struIntel[m_nIndexOfArithIntel].sName, str.toLatin1().data());
    }
}

/*!
 * \brief 槽函数：输入智能参数
 */
void AIMainWidget::onAIParamsInputSlt()
{
    m_stackedWidget->setCurrentIndex(AI_INPUT_PARAMS_PAGE);
}

/*!
 * \brief 槽函数：输入智能参数返回上一级
 */
void AIMainWidget::goBackToAIImageWidget()
{
    m_stackedWidget->setCurrentIndex(AI_IMAGE_PAGE);
}


/* 智能参数设置页面tabBar槽函数 */
void AIMainWidget::changeCurrentSlt(int indexOfUnit)
{
    struGsh.nLevel = ONE_LEVEL;
    m_currentChan = indexOfUnit;
    updateParams();
}
void AIMainWidget::updateParams()
{
    QString strAIName;
    QString strAIRow;
    QString strAIPercent;
    QString str;

    strAIName = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].sName);
    nameValue->setText(strAIName);

    AISensListCbx->setTitle(QString("%1").arg(myString.sArithmeticName[ARITH_INTEL_A + m_nIndexOfArithIntel]));

    str = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nSens);
    AISensLbe->setText(str);

    strAIRow = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nRow);
    AIRowLbe->setText(strAIRow);

    strAIPercent = QString("%1").arg(struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nPercent);
    AIPercentLbe->setText(strAIPercent);
}

void AIMainWidget::setImpurityNameSlt()
{
    myMessageBox msgBox(MSG_QUES, myLan.cfm_rename_impurity);
    if (msgBox.exec() == QDialog::Accepted)
    {
        myInputMethod inputDlg(myLan.msg_input_name, nameValue->text());
        if (inputDlg.exec() == QDialog::Accepted)
        {
            QString text = inputDlg.getText();
            memcpy(struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].sName, text.toLatin1(), MAX_NAME);

            nameValue->setText(text);
        }
    }
}

void AIMainWidget::setSensValueSlt()
{
    double nSens = 0;
    myInputPanel inputDlg1(intType, 1, 200, AISensLbe->text().toInt());
    nSens = struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nSens;
    inputDlg1.setValue(nSens);

    int ret = inputDlg1.exec();
    if (ret == QDialog::Accepted)
    {
        nSens = inputDlg1.getValue();
        QString str;
        struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nSens = nSens;
        str = QString("%1").arg(nSens);
        AISensLbe->setText(str);

        /*参数发送*/
        myFlow.materialCopyAssemble(struGsh.nLevel, m_currentChan, 0, ARITH_INTEL_A + m_nIndexOfArithIntel, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, m_currentChan, 0, ARITH_INTEL_A + m_nIndexOfArithIntel, 0);
    }
}

void AIMainWidget::setRowValueSlt()
{
    double nRow;
    int row, col, per, bal;
    nRow = AIRowLbe->text().toDouble();
    myInputPanel inputDlg2(intType, 1, struGsh.maxRowNumber, nRow);
    int ret = inputDlg2.exec();
    if (ret == QDialog::Accepted)
    {
        nRow = inputDlg2.getValue();
        struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nRow = row = nRow;
        struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nColumn = col = nRow * struGsh.rowColumnRelation;
        per = struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nPercent;
        bal = struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nEnable;
        if (per > row * col)
        {
            per = row * col;
            struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nPercent = per;
        }

        if (bal > row * col)
        {
            bal = row * col;
            struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nEnable = bal;
        }

        QString str = QString("%1").arg(nRow);
        AIRowLbe->setText(str);

        str = QString("%1").arg(per);
        AIPercentLbe->setText(str);

        /*参数发送*/
        myFlow.materialCopyAssemble(struGsh.nLevel, m_currentChan, 0, ARITH_INTEL_A + m_nIndexOfArithIntel, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, m_currentChan, 0, ARITH_INTEL_A + m_nIndexOfArithIntel, 0);
    }
    myFlow.resetEjectTime();
}

void AIMainWidget::setPercentValueSlt()
{
    double nPercent;
    int row;
    int col;
    nPercent = AIPercentLbe->text().toDouble();
    row = struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nRow;
    col = struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nColumn;
    myInputPanel inputDlg3(intType, 1, row * col, nPercent);
    int ret = inputDlg3.exec();
    if (ret == QDialog::Accepted)
    {
        nPercent = inputDlg3.getValue();
        QString str = QString("%1").arg(nPercent);
        AIPercentLbe->setText(str);

        /*参数发送*/
        struCnfp.struGroupIdentify[struGsh.nLevel][m_currentChan].struIntel[m_nIndexOfArithIntel].nPercent = nPercent;
        myFlow.materialCopyAssemble(struGsh.nLevel, m_currentChan, 0, ARITH_INTEL_A + m_nIndexOfArithIntel, 0);
        myFlow.materialResetGroupAssemble(struGsh.nLevel, m_currentChan, 0, ARITH_INTEL_A + m_nIndexOfArithIntel, 0);
    }
    myFlow.resetEjectTime();
}


/* 智能参数设置页面返回槽函数 */
void AIMainWidget::onAIParamsBackBtnPressedSlt()
{
    m_stackedWidget->setCurrentIndex(AI_MAIN_PAGE);
}

/*!
 * \brief AIMainWidget::onColorSpaceBtnGpIndexChangedSlt
 * \param nIndex
 */
void AIMainWidget::onColorSpaceBtnGpIndexChangedSlt(int nIndex)
{
    struCnfp.nColorSpace = nIndex;
}

/*!
 * \brief AIMainWidget::onDimensionBtnGpIndexChangedSlt
 * \param nIndex
 */
void AIMainWidget::onDimensionBtnGpIndexChangedSlt(int nIndex)
{
    struCnfp.nAIMode = nIndex;
}

/**
 * @brief AIMainWidget::updateAIParamsPage
 * 更新智能参数页面
 */
void AIMainWidget::updateAIParamsPage()
{
    upTabBar();
    updateParams();
}

/**
 * @brief AIMainWidget::upTabBar
 * 更新tabBar
 */
void AIMainWidget::upTabBar()
{
    for (int i = tabBar->count() - 1; i > -1;i--)
    {
        tabBar->removeTab(i);
    }

    m_algorithmType = 0;

    for (int j = 0; j < struCnfg.struLevelInfo[0].nIdentifyGroupTotal; j++)
    {
        tabBar->insertTab(j, myString.sIdentifyGroupName[0][j]);
    }

    int caseValue = struCnfp.nMatAssembleMode;

    if (caseValue == PARAMS_ALL_SEPARATE)
    {             // 单独设置
        for (int i = 0; i < tabBar->count(); i++)
        {
            tabBar->setTabEnabled(i, true);
        }
    }
    else if (caseValue == PARAMS_FRONT_REAR_SAME)
    {    // 前后视相同
        for (int i = 0; i < tabBar->count(); i++)
        {
            if (i % 2 == 0)
            {
                tabBar->setTabEnabled(i, true);
            }
            else
            {
                tabBar->setTabEnabled(i, false);
            }
        }
    }
    else if (caseValue == PARAMS_FIRST_SECOND_SAME)
    { // 一二三次相同
        for (int j = 0; j < struCnfg.nLevelTotal; j++)
        {
            for (int i = 0;i < tabBar->count() / struCnfg.nLevelTotal;i++)
            {
                if (i < 2)
                {
                    tabBar->setTabEnabled(i + j * (tabBar->count() / struCnfg.nLevelTotal), true);
                }
                else
                {
                    tabBar->setTabEnabled(i + j * (tabBar->count() / struCnfg.nLevelTotal), false);
                }
            }
        }
    }
    else if (caseValue == PARAMS_ALL_SAME)
    {          // 所有相同
        for (int i = 0;i < tabBar->count();i++)
        {
            if (i == 0)
            {
                tabBar->setTabEnabled(i, true);
            }
            else
            {
                tabBar->setTabEnabled(i, false);
            }
        }
    }
}
