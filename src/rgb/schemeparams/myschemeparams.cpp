/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myschemeparams.cpp
 * \brief       彩色机型方案参数界面源文件
 *              子页面包括识别参数、剔除参数、吹气时间、背景设置、灯光控制及智能供料。
 * \date        2015.01.14
 */
#include <QtGui>

#include "myschemeparams.h"

/**
  * @brief MySchemeParams::MySchemeParams
  * @param parent
  * 构造方案参数管理页面
  */
MySchemeParams::MySchemeParams(QWidget *parent)
    :QWidget(parent)
{
    setFixedSize(LCD_WIDTH-30, LCD_HEIGHT-125);

    config = g_Config::getInstance();
/* page init */
    /* the stacked widget to hold pages */
    pagesWidget = new QStackedWidget(this);

    /* Page_Params */
    pageParams = new PageParams;
    pagesWidget->insertWidget(Page_ID_Params, pageParams);

    /* Page_Arith */
    pageArith = new PageArith;
    pagesWidget->insertWidget(Page_ID_Arith, pageArith);

    /* Page_Reserved */
    pageReserved = new PageReserved;
    pagesWidget->insertWidget(Page_ID_Reserved, pageReserved);

    /* Page_Intel */
    pageIntel = new PageIntel;
    pagesWidget->insertWidget(Page_ID_Intel, pageIntel);

    pageAi = new PageAi;
    pagesWidget->insertWidget(Page_ID_Ai, pageAi);

    /* page buttons */   
    okButton = new myPushButton(myLan.apply, myIcon.Action_Apply, true, true, this);
    okButton->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    cancelButton = new myPushButton(myLan.back, myIcon.Action_Back, true, true, this);
    cancelButton->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

/* page update */
    /* signals to slots */
    connect(pagesWidget,   SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));

    connect(okButton,      SIGNAL(pressed()), this, SLOT(onOkBtnClicked()));
    connect(cancelButton,  SIGNAL(pressed()), this, SLOT(onCancelBtnClicked()));

    connect(this,          SIGNAL(pageChanged(int)), this, SLOT(onPageChanged(int)));
    connect(pageParams,    SIGNAL(pageChanged(int)), this, SLOT(onPageChanged(int)));
    connect(pageArith,     SIGNAL(pageChanged(int)), this, SLOT(onPageChanged(int)));
    connect(pageParams,    SIGNAL(setOkBtnHiddenSig(bool)), okButton, SLOT(setHidden(bool)));
    connect(this,          SIGNAL(pageIndexChanged(int)),    pageParams, SLOT(refreshPageParams()));
    connect(this,          SIGNAL(shutDownBg()),    pageParams, SLOT(shutdownBg()));


/* page layout */
    /* btn layout */
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch(1);
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    cancelButton->hide();

    /* main page layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pagesWidget,Qt::AlignTop);
    mainLayout->addLayout(btnLayout,Qt::AlignBottom);
    mainLayout->setSpacing(5);
    setLayout(mainLayout);
}

/**
  * @brief MySchemeParams::displayPageButton
  * @param index
  * 基于不同的方案参数页面ID，更新确定取消按键的显示
  */
void MySchemeParams::displayPageButton(int index)
{
    switch (index) {
    case Page_ID_Intel:
        okButton->hide();
        cancelButton->setIcon(myIcon.Action_Back);
        cancelButton->setText(myLan.back);
        cancelButton->hide();
        break;
    case Page_ID_Params:
        okButton->setText(myLan.apply);
        okButton->show();
        cancelButton->setIcon(myIcon.Action_Back);
        cancelButton->setText(myLan.back);
        cancelButton->hide();
        break;
    case Page_ID_Arith:
    case Page_ID_Reserved:
        okButton->setText(myLan.ok);
        okButton->show();
//        cancelButton->setIcon(myIcon.Action_Cancel);
        cancelButton->setIcon(QIcon(""));

        cancelButton->setText(myLan.cancel);
        cancelButton->show();
        break;
    case Page_ID_Ai:
        okButton->setText(myLan.apply);
        okButton->show();
        cancelButton->setIcon(myIcon.Action_Back);
        cancelButton->setText(myLan.back);
        cancelButton->show();
    default:
        break;
    }

}

/**
  * @brief MySchemeParams::setSchemeParams
  * @param pageId 
  * 应用当前方案参数页面设置 
  */
void MySchemeParams::setSchemeParams(int pageId)
{
    /* apply scheme parameters */
    switch (pageId) {
    case Page_ID_Params:
        pageParams->setParams();
        break;
    case Page_ID_Arith:
        pageArith->setArithParams();
        break;
    case Page_ID_Reserved:
        pageReserved->setReservedParams();
        break;
    case Page_ID_Intel:
        pageIntel->setIntelParams();
        break;
    case Page_ID_Ai:
        pageAi->setIntelParams();
        break;
    default:
        break;
    }
}

/**
  * @brief MySchemeParams::resetSchemeParams
  * @param pageId 
  * 重置当前方案参数页面设置 
  */
void MySchemeParams::resetSchemeParams(int pageId)
{
 /* apply scheme parameters */
    switch (pageId) {
    case Page_ID_Params:
        pageParams->resetParams();
        break;
    case Page_ID_Arith:
        pageArith->resetArithParams();
        break;
    case Page_ID_Reserved:
        pageReserved->resetReservedParams();
        break;
    case Page_ID_Ai:
        pageAi->resetIntelParams();
        break;
    case Page_ID_Intel:
        pageIntel->resetIntelParams();
        break;
    default:
        break;
    }
}

/**
  * @brief MySchemeParams::onOkBtnClicked
  * 响应方案参数页面确定按钮 
  */
void MySchemeParams::onOkBtnClicked()
{
    int pageId = pagesWidget->currentIndex();

    /* applay scheme parameters */
    setSchemeParams(pageId);

    /* trigger signals to return the parent page */
    switch (pageId) {
    case Page_Params:
//      emit goToHomePage();
        break;
    case Page_ID_Intel:
        break;
    case Page_ID_Ai:
        emit pageChanged(Page_Params);
    case Page_ID_Arith:
        emit pageChanged(Page_Params);
        break;
    case Page_ID_Reserved:
        if (pageReserved->getArithIndex() == ARITH_RESERVED) {
            emit pageChanged(Page_Params);
        } else {
            emit pageChanged(getPageIndex(pageReserved->getArithIndex()));
        }
        break;
    default:
        break;
    }
}

/**
  * @brief MySchemeParams::onCancelBtnClicked
  * 响应方案参数页面取消按钮 
  */
void MySchemeParams::onCancelBtnClicked()
{
    int pageId = pagesWidget->currentIndex();

    /* reset scheme parameters */
    resetSchemeParams(pageId);

    /* trigger signals to return the parent page */
    switch (pageId) {
    case Page_ID_Params:
        emit goToHomePage();
        break;
    case Page_ID_Arith:
    case Page_ID_Intel:
    case Page_ID_Ai:
        emit pageChanged(Page_Params);
        break;
    case Page_ID_Reserved:
        if (pageReserved->getArithIndex() == ARITH_RESERVED) {
            emit pageChanged(Page_Params);
        } else {
            emit pageChanged(getPageIndex(pageReserved->getArithIndex()));
        }
        break;
    default:
        break;

    }
}

/**
  * @brief MySchemeParams::onCurrentChanged
  * @param index
  * 响应当前stackedWidget页面切换
  */
void MySchemeParams::onCurrentChanged(int index)
{
    QString title;
    resetSchemeParams(index);

    title = QString("%1").arg(myLan.scheme_parameter);
    titleLabel->setText(title);

    displayPageButton(index);
}

/**
  * @brief MySchemeParams::onPageChanged
  * @param pageId
  * 响应方案参数页面ID索引号切换信号
  */
void MySchemeParams::onPageChanged(int pageId)
{
    /* change to page by pageId index */
    switch (pageId) {
    case Page_Params:
        pagesWidget->setCurrentIndex(Page_ID_Params);
        break;
    case Page_Grey_A:
    case Page_Grey_B:
    case Page_Discolor_A:
    case Page_Discolor_B:
    case Page_Cross:
    case Page_Shape:
    case Page_Shape_Length:
    case Page_Shape_Pole_A:
    case Page_Shape_Pole_B:
    case Page_Shape_Slice:
    case Page_Shape_Leaf:
    case Page_Scale:
    case Page_Scale_B:
    case Page_Bud_1:
    case Page_Bud_2:
    case Page_PeanutAbcd_A:
    case Page_PeanutAbcd_B:
    case Page_PeanutAbcd_C:
    case Page_PeanutAbcd_D:
    case Page_Candy:
        pageArith->setArithIndex(getArithIndex(pageId));
        pagesWidget->setCurrentIndex(Page_ID_Arith);
        break;
    case Page_PISTACHIO:
        pageArith->setArithIndex(getArithIndex(pageId));
        pagesWidget->setCurrentIndex(Page_ID_Ai);
        break;
    case Page_Reserved_Grey_A:
    case Page_Reserved_Grey_B:
    case Page_Reserved_Discolor_A:
    case Page_Reserved_Discolor_B:
    case Page_Reserved_Intel_A:
    case Page_Reserved_Intel_B:
    case Page_Reserved_Intel_C:
    case Page_Reserved_Intel_D:
    case Page_Reserved_PeanutAbcd_D:
    case Page_Reserved_Shape_Pole_A:
        pageReserved->setArithIndex(getArithIndex(pageId));
        pagesWidget->setCurrentIndex(Page_ID_Reserved);
        break;
    case Page_Intel_A:
        case Page_Intel_B:
        case Page_Intel_C:
        case Page_Intel_D:
            pageArith->setArithIndex(getArithIndex(pageId));
            if(myFlow.getProductLineNo() == 0) {
                emit goToAIMainWidgetSig(pageId-Page_Intel_A);
                break;
            } else {
                myInputPanel inputDlg(passwdType, 0, 0, 0);
                if (inputDlg.exec() == QDialog::Accepted) {
                    QString str = inputDlg.getText();
                    if (str == "19850218") {
                        pagesWidget->setCurrentIndex(Page_ID_Intel);
                        break;
                    }
                }
                pagesWidget->setCurrentIndex(Page_ID_Params);
                break;
            }
    }

    /* trigger Page to first index */
    switch (pageId) {
    case Page_ID_Params:
        emit pageIndexChanged(FIRST_UNIT);
        break;
    }
}

/**
  * @brief MySchemeParams::refreshSchemeParams
  * 默认进入方案参数页面时，切换到识别参数页面
  */
void MySchemeParams::refreshSchemeParams()
{
    pagesWidget->setCurrentIndex(Page_ID_Params);
    emit pageIndexChanged(FIRST_UNIT);
}


void MySchemeParams::shutdownCurveBackGround(){
    emit shutDownBg();
}
