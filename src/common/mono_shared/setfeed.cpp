/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setfeed.cpp
 * \brief       黑白机型设置供料量界面源文件。
 * \date        2015.01.14
 */
#include "setfeed.h"
#include "unilog.h"

setFeedPage::setFeedPage(QWidget* parent) :
    QWidget(parent)
{
    setFixedSize(LCD_WIDTH - BTN_WIDTH + 80, LCD_HEIGHT - 140);
    m_nPreIndex = FEED_MAIN_PAGE;
    createMainPage();

    feedSeparatePage = new QWidget();
    stackedWidget->addWidget(feedSeparatePage);
    CreateFeedSeparateWidget();

    feedMainPage = new QWidget();
    stackedWidget->addWidget(feedMainPage);
    CreatFeedMainWidget();

    feedEnablePage = new QWidget();
    stackedWidget->addWidget(feedEnablePage);
    CreateFeedEnableWidget();

    m_feedCtrlPage = new FeedCtrl();
    stackedWidget->addWidget(m_feedCtrlPage);

    m_feedSetPage = new paramSetAlone();
    stackedWidget->addWidget(m_feedSetPage);

    connect(this, SIGNAL(hideOneKeyFeedSig()), m_feedCtrlPage, SLOT(hideOneKeyFeedSlt()));
    connect(this, SIGNAL(showOneKeyFeedSig()), m_feedCtrlPage, SLOT(showOneKeyFeedSlt()));
}

/* 创建主页面 */
void setFeedPage::createMainPage()
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);
    QHBoxLayout* downLay = new QHBoxLayout;
    stackedWidget = new QStackedWidget();
    stackedWidget->setFixedSize(width(), height() - BTN_HEIGHT - 45);

    m_feedCtrlBtn = new myPushButton(myLan.feeder_control, QIcon());
    m_feedCtrlBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    m_feedSetBtn = new myPushButton("", myIcon.Action_Configure, true, true, this);
    QString timeStr = QString("%1(s)").arg(myLan.detect_wait);
    m_waitTimeLbl = new myLabel(timeStr);
    m_waitTimeLbl->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_waitTimeNum = new myLCDNumber(3, struCnfg.nFeederCtrlWaitTime);
    m_waitTimeNum->setFixedSize(100, BTN_HEIGHT);
    m_waitTimeLbl->hide();
    m_waitTimeNum->hide();

    m_backBtn = new myPushButton(myLan.back, myIcon.Action_Back);
    m_backBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    connect(m_feedCtrlBtn, SIGNAL(clicked()), this, SLOT(onFeedCtrlBtnPressed()));
    connect(m_feedSetBtn, SIGNAL(clicked()), this, SLOT(onFeedSetBtnPressed()));
    connect(m_backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnPressed()));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStakedWidgetIndexChangedSlt(int)));
    connect(m_waitTimeNum, SIGNAL(pressed()), this, SLOT(onWaitTimePressed()));

    downLay->addWidget(m_waitTimeLbl);
    downLay->addWidget(m_waitTimeNum);
    downLay->addWidget(m_feedSetBtn, 1, Qt::AlignLeft);
    downLay->addWidget(m_feedCtrlBtn, 1, Qt::AlignLeft);
    downLay->addStretch(0);
    downLay->addWidget(m_backBtn);
    mainLay->addWidget(stackedWidget);
    mainLay->addLayout(downLay);
}

/* 创建独立供料量页面 */
void setFeedPage::CreateFeedSeparateWidget(void)
{
    QString strFeed;
    feederSeparateSignalMapper = new QSignalMapper(this);
    QHBoxLayout* inputLay[MAX_FEED];
    QGridLayout* mainLay = new QGridLayout(feedSeparatePage);
    mainLay->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < MAX_FEED; i++)
    {
        strFeed = QString("%1 %2").arg(myLan.feeder).arg(i + 1);
        feedNameSeparateRS[i] = new myLabel(strFeed, feedSeparatePage);
        feedNameSeparateRS[i]->setFixedSize(BTN_WIDTH - 40, BTN_HEIGHT);
        strFeed = QString("%1").arg(struCnfp.struGroupCtrl[0].nFeeder[i] * 0.1);
        feedLbeSeparateRS[i] = new myLineEdit(strFeed, feedSeparatePage);
        feedLbeSeparateRS[i]->setReadOnly(true);
        feedLbeSeparateRS[i]->setFixedSize(BTN_WIDTH - 70, BTN_HEIGHT);

        feederSeparateSignalMapper->setMapping(feedLbeSeparateRS[i], i);
        connect(feedLbeSeparateRS[i], SIGNAL(pressed()), feederSeparateSignalMapper, SLOT(map()));

        //! 布局
        inputLay[i] = new QHBoxLayout();
        //        inputLay[i]->setContentsMargins(10, 0, 10, 0);
        inputLay[i]->addWidget(feedNameSeparateRS[i]);
        inputLay[i]->addWidget(feedLbeSeparateRS[i]);
        if (i % 4 == 0)
        {
            inputLay[i]->addSpacing(20);
        }

        mainLay->addLayout(inputLay[i], i / 4, i % 4);
    }

    /* 槽函数 */
    connect(feederSeparateSignalMapper, SIGNAL(mapped(int)), this, SLOT(feederSeparateRSValueChange(int)));
}

/* 创建供料主页面 */
void setFeedPage::CreatFeedMainWidget(void)
{
    QHBoxLayout* feederHBLayout[4];
    QVBoxLayout* mainLay = new QVBoxLayout(feedMainPage);
    feederSignalMapper = new QSignalMapper(this);

    feedNameRS[0] = new myLabel("");
    feedNameRS[1] = new myLabel("");
    feedNameRS[2] = new myLabel("");
    feedNameRS[3] = new myLabel("");
    for (int i = 0; i < 4; i++)
    {
        feedNameRS[i]->setFixedSize(BTN_WIDTH + 20, BTN_HEIGHT);
        QString strFeed = "";
        feedLbeRS[i] = new myLineEdit(strFeed);
        feedLbeRS[i]->setReadOnly(true);
        feedLbeRS[i]->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

        /* 供料按钮布局 */
        feederHBLayout[i] = new QHBoxLayout();
        if (LCD_WIDTH == 1024)
        {
            feederHBLayout[i]->setContentsMargins(10, 5, 10, 5);
        }
        else if (LCD_WIDTH == 640)
        {
            feederHBLayout[i]->setContentsMargins(10, 15, 10, 15);
        }
        feederHBLayout[i]->addWidget(feedNameRS[i]);
        feederHBLayout[i]->addWidget(feedLbeRS[i]);
        mainLay->addLayout(feederHBLayout[i]);

        /* 使用信号栈方式实现各供料设置 */
        feederSignalMapper->setMapping(feedLbeRS[i], i);
        connect(feedLbeRS[i], SIGNAL(pressed()), feederSignalMapper, SLOT(map()));
    }

    /* 使能及返回按钮 */
    feedEnableBtn = new myPushButton(myLan.feedEnable, QIcon());
    feedEnableBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

    /* 槽函数 */
    connect(feederSignalMapper, SIGNAL(mapped(int)), this, SLOT(feederRSValueChange(int)));
    connect(feedEnableBtn, SIGNAL(pressed()), this, SLOT(onFeedEnableBtnClickedSlt()));

    /* 整个页面布局 */
    feederHBLayout[3] = new QHBoxLayout();
    if (LCD_WIDTH == 1024)
    {
        feederHBLayout[3]->setContentsMargins(10, 5, 10, 5);
    }
    else if (LCD_WIDTH == 640)
    {
        feederHBLayout[3]->setContentsMargins(10, 15, 10, 15);
    }
    feederHBLayout[3]->addWidget(feedEnableBtn);
    mainLay->addLayout(feederHBLayout[3]);
}

/* 创建供料使能页面 */
void setFeedPage::CreateFeedEnableWidget(void)
{
    feedEnableMapper = new QSignalMapper(feedEnablePage);
    feedEnableBox1.clear();
    for (int i = 0; i < MAX_FEED; i++)
    {
        QString strFeed = QString("%1 %2").arg(myLan.feeder).arg(i + 1);
        feedEnableBox1.append(new MyCheckBox(strFeed, this));
    }

    for (int i = 0; i < feedEnableBox1.size();i++)
    {
        /* size policy */
        feedEnableBox1[i]->setFixedSize(BTN_WIDTH, BTN_HEIGHT);

        feedEnableMapper->setMapping(feedEnableBox1[i], i);
        connect(feedEnableBox1[i], SIGNAL(pressed()), feedEnableMapper, SLOT(map()));
    }

    QHBoxLayout* feedEnablePageHBLayout[5];
    QVBoxLayout* mainLay = new QVBoxLayout(feedEnablePage);

    feedEnableAllBtn = new myPushButton(myLan.enable_all, QIcon());
    feedEnableAllBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    feedDisableAllBtn = new myPushButton(myLan.disable_all, QIcon());
    feedDisableAllBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    feedDisableAllBtn->setEnabled(false);

    for (int i = 0; i < 4; i++)
    {
        feedEnablePageHBLayout[i] = new QHBoxLayout();
        for (int j = 0; j < 3; j++)
        {
            if (LCD_WIDTH == 1024)
            {
                feedEnablePageHBLayout[i]->setContentsMargins(0, 5, 0, 5);
            }
            else if (LCD_WIDTH == 640)
            {
                feedEnablePageHBLayout[i]->setContentsMargins(0, 15, 0, 15);
            }
            feedEnablePageHBLayout[i]->addWidget(feedEnableBox1[i * 3 + j]);
        }
    }

    feedEnablePageHBLayout[4] = new QHBoxLayout();
    if (LCD_WIDTH == 1024)
    {
        feedEnablePageHBLayout[4]->setContentsMargins(0, 5, 0, 5);
    }
    else if (LCD_WIDTH == 640)
    {
        feedEnablePageHBLayout[4]->setContentsMargins(0, 15, 0, 15);
    }
    feedEnablePageHBLayout[4]->addWidget(feedEnableAllBtn);
    feedEnablePageHBLayout[4]->addWidget(feedEnableBox1[11]);
    feedEnablePageHBLayout[4]->addWidget(feedDisableAllBtn);
    /* 槽函数 */
    connect(feedEnableMapper, SIGNAL(mapped(int)), this, SLOT(feederEnableChange(int)));
    connect(feedEnableAllBtn, SIGNAL(pressed()), this, SLOT(onfeedEnableAllBtnClickedSlt()));
    connect(feedDisableAllBtn, SIGNAL(pressed()), this, SLOT(onfeedDisableAllBtnClickedSlt()));

    for (int j = 0; j < 5; j++)
    {
        mainLay->addLayout(feedEnablePageHBLayout[j]);
    }
}

void setFeedPage::feederRSValueChange(int indexOfFeeder)
{
    double nFeeder;
    nFeeder = struCnfp.struGroupCtrl[0].nFeeder[feedStart[indexOfFeeder]] * 0.1;
    myInputPanel inputDlg(intType, 0, 100, nFeeder);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nFeeder = inputDlg.getValue();
        QString str = QString("%1%").arg(nFeeder);
        feedLbeRS[indexOfFeeder]->setText(str);
        getFeedValue(indexOfFeeder, nFeeder);
        myFlow.resetFeeder(0);
        LOG_INFO_STM("idx:" << indexOfFeeder << " Feeder:" << struCnfp.struGroupCtrl[0].nFeeder[indexOfFeeder]);
    }
}

/* 供料使能 */
void setFeedPage::onFeedEnableBtnClickedSlt(void)
{
    stackedWidget->setCurrentIndex(FEED_ENABLE_PAGE);
    updateFeedEnableState();
}

void setFeedPage::feederEnableChange(int indexOfFeeder)
{
    struCnfp.struGroupCtrl[0].nFeederEnable[indexOfFeeder] = 1 - struCnfp.struGroupCtrl[0].nFeederEnable[indexOfFeeder];
    if (struCnfp.struGroupCtrl[0].nFeederEnable[indexOfFeeder])
    {
        feedEnableBox1[indexOfFeeder]->setChecked(true);
    }
    else
    {
        feedEnableBox1[indexOfFeeder]->setChecked(false);
    }

    updateEnableAllBtnState();
    myFlow.resetFeeder(0);
    LOG_INFO_STM("idx:" << indexOfFeeder << " FeederStatus:" << struCnfp.struGroupCtrl[0].nFeederEnable[indexOfFeeder]);
}

void setFeedPage::onfeedEnableAllBtnClickedSlt(void)
{
    feedEnableAllBtn->setEnabled(false);
    feedDisableAllBtn->setEnabled(true);

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        feedEnableBox1[i]->setChecked(true);
        struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
    }

    myFlow.resetFeeder(0);
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        LOG_INFO_STM("idx:" << i << " FeederStatus:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
    }
}

void setFeedPage::onfeedDisableAllBtnClickedSlt(void)
{
    feedEnableAllBtn->setEnabled(true);
    feedDisableAllBtn->setEnabled(false);
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        feedEnableBox1[i]->setChecked(false);
        struCnfp.struGroupCtrl[0].nFeederEnable[i] = 0;
    }

    myFlow.resetFeeder(0);
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        LOG_INFO_STM("idx:" << i << " FeederStatus:" << struCnfp.struGroupCtrl[0].nFeederEnable[i]);
    }
}

void setFeedPage::getFeedGroupIndex(void)
{
    for (int i = 0; i < 4; i++)
    {
        if (struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitSplice[0] == 2
            && struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitSplice[1] == 2)
        {
            feedStart[i] = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitId[2] / 2;
            feedEnd[i] = feedStart[i] + (struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitCount - 2) / 2 - 1;
        }
        else
        {
            feedStart[i] = struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitId[0] / 2;
            feedEnd[i] = feedStart[i] + struCnfg.struLevelInfo[ONE_LEVEL].struTickGroupInfo[i].nUnitCount / 2 - 1;
        }
    }
}

void setFeedPage::getFeedValue(int indexOfFeeder, int feedValue)
{
    for (int i = feedStart[indexOfFeeder]; i <= feedEnd[indexOfFeeder]; i++)
    {
        struCnfp.struGroupCtrl[0].nFeeder[i] = feedValue * 10;
    }
}

void setFeedPage::feederSeparateRSValueChange(int indexOfFeeder)
{
    double nFeeder;
    nFeeder = struCnfp.struGroupCtrl[0].nFeeder[indexOfFeeder] * 0.1;
    myInputPanel inputDlg(intType, 0, 100, nFeeder);
    int ret = inputDlg.exec();
    if (ret == QDialog::Accepted)
    {
        nFeeder = inputDlg.getValue();
        struCnfp.struGroupCtrl[0].nFeeder[indexOfFeeder] = (int)nFeeder * 10;
        QString str = QString("%1%").arg(nFeeder);
        feedLbeSeparateRS[indexOfFeeder]->setText(str);
        myFlow.resetFeeder(0);
        LOG_INFO_STM("idx:" << indexOfFeeder << " Feeder:" << struCnfp.struGroupCtrl[0].nFeeder[indexOfFeeder]);
    }
}

/* 进入料位监控页面 */
void setFeedPage::onFeedCtrlBtnPressed()
{
    m_nPreIndex = stackedWidget->currentIndex();
    m_feedCtrlPage->refreshDisplay();
    stackedWidget->setCurrentIndex(FEED_CTRL_PAGE);
}

/* 进入供料组合选择页面 */
void setFeedPage::onFeedSetBtnPressed()
{
    m_nPreIndex = stackedWidget->currentIndex();
    m_feedSetPage->refreshDisplay();
    stackedWidget->setCurrentIndex(FEED_SET_PAGE);
}

/* 点击返回按钮 */
void setFeedPage::onBackBtnPressed()
{
    int nIndex = stackedWidget->currentIndex();

    switch (nIndex)
    {
    case FEED_MAIN_PAGE:
    case FEED_SEPARATE_PAGE:
        emit goToHomePageSig();
        break;
    case FEED_ENABLE_PAGE:
        stackedWidget->setCurrentIndex(FEED_MAIN_PAGE);
        break;
    case FEED_CTRL_PAGE:
        m_feedCtrlPage->stopRefresh();
        stackedWidget->setCurrentIndex(m_nPreIndex);
        break;
    case FEED_SET_PAGE:
        updatePage();
        break;
    default:
        break;
    }
}

/* 切换页索引变化 */
void setFeedPage::onStakedWidgetIndexChangedSlt(int nIndex)
{
    switch (nIndex)
    {
    case FEED_MAIN_PAGE:
    case FEED_SEPARATE_PAGE:
        m_feedCtrlBtn->show();
        m_feedSetBtn->show();
        m_waitTimeLbl->hide();
        m_waitTimeNum->hide();
        break;
    case FEED_CTRL_PAGE:
        m_feedCtrlBtn->hide();
        m_feedSetBtn->hide();
        m_waitTimeLbl->show();
        m_waitTimeNum->show();
        break;
    case FEED_ENABLE_PAGE:
        m_feedCtrlBtn->hide();
        m_feedSetBtn->hide();
        m_waitTimeLbl->hide();
        m_waitTimeNum->hide();
        break;
    case FEED_SET_PAGE:
        m_feedCtrlBtn->hide();
        m_feedSetBtn->hide();
        m_waitTimeLbl->hide();
        m_waitTimeNum->hide();
        break;
    default:
        break;
    }

    switch (nIndex)
    {
    case FEED_MAIN_PAGE:
    case FEED_SEPARATE_PAGE:
        m_backBtn->hide();
        break;
    case FEED_CTRL_PAGE:
    case FEED_ENABLE_PAGE:
    case FEED_SET_PAGE:
        m_backBtn->show();
        break;
    default:
        break;
    }


}

/* 检测等待时间变化 */
void setFeedPage::onWaitTimePressed()
{
    int nVal = struCnfg.nFeederCtrlWaitTime;
    myInputPanel* inputPanel = new myInputPanel(intType, 0, 120, nVal);
    int nRet = inputPanel->exec();

    if (nRet == QDialog::Accepted)
    {
        nVal = inputPanel->getValue();
        if (nVal < 5)
        {
            nVal = 5;
        }
        struCnfg.nFeederCtrlWaitTime = nVal;
        m_waitTimeNum->display(nVal);
    }
}

/** 更新页面状态 */
void setFeedPage::updatePage()
{
    if (struCnfg.nParamSetAlone == 1)
    {
        stackedWidget->setCurrentIndex(FEED_SEPARATE_PAGE);
        updateSeparatePageValue();
    }
    else
    {
        stackedWidget->setCurrentIndex(FEED_MAIN_PAGE);
        getFeedGroupIndex();
        updateMainPageValue();
    }
}

/** 更新独立灵敏度页面状态 */
void setFeedPage::updateSeparatePageValue()
{
    QString str;
    for (int i = 0; i < MAX_FEED; i++)
    {
        feedNameSeparateRS[i]->hide();
        feedLbeSeparateRS[i]->hide();
    }

    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        str = QString("%1%").arg(struCnfp.struGroupCtrl[0].nFeeder[i] * 0.1);
        feedNameSeparateRS[i]->show();
        feedLbeSeparateRS[i]->show();
        feedLbeSeparateRS[i]->setText(str);
    }

    if (1 == struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2)
    {
        str = QString("%1%").arg(struCnfp.struGroupCtrl[0].nFeeder[1] * 0.1);
        feedNameSeparateRS[1]->show();
        feedLbeSeparateRS[1]->show();
        feedLbeSeparateRS[1]->setText(str);
    }
    myFlow.resetFeeder(0);
}

/** 更新分次设置供料量页面状态 */
void setFeedPage::updateMainPageValue()
{
    QString str;
    int i;
    feedNameRS[0]->setText(struGsh.first + myLan.feeder_value);
    feedNameRS[1]->setText(struGsh.second + myLan.feeder_value);
    feedNameRS[2]->setText(struGsh.third + myLan.feeder_value);
    feedNameRS[3]->setText(myLan.fourth + myLan.feeder_value);

    for (i = 0; i < 4; i++)
    {
        str = QString("%1%").arg(struCnfp.struGroupCtrl[0].nFeeder[feedStart[i]] * 0.1);
        feedLbeRS[i]->setEnabled(true);
        feedLbeRS[i]->setText(str);
        feedNameRS[i]->setEnabled(true);
        if (feedEnd[i] >= (struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2 - 1))
        {
            break;
        }
    }
    if (i == 2)
    {
        feedNameRS[3]->setEnabled(false);
        feedLbeRS[3]->setText("0");
        feedLbeRS[3]->setEnabled(false);
    }

    if (i == 1)
    {
        feedNameRS[2]->setEnabled(false);
        feedLbeRS[2]->setText("0");
        feedLbeRS[2]->setEnabled(false);
        feedNameRS[3]->setEnabled(false);
        feedLbeRS[3]->setText("0");
        feedLbeRS[3]->setEnabled(false);
    }
    if (i == 0)
    {
        feedNameRS[1]->setEnabled(false);
        feedLbeRS[1]->setText("0");
        feedLbeRS[1]->setEnabled(false);
        feedNameRS[2]->setEnabled(false);
        feedLbeRS[2]->setText("0");
        feedLbeRS[2]->setEnabled(false);
        feedNameRS[3]->setEnabled(false);
        feedLbeRS[3]->setText("0");
        feedLbeRS[3]->setEnabled(false);
    }
}

/** 更新供料使能状态 */
void setFeedPage::updateFeedEnableState()
{
    for (int i = 0; i < MAX_FEED; i++)
    {
        feedEnableBox1[i]->hide();
    }
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        feedEnableBox1[i]->show();
        if (struCnfp.struGroupCtrl[0].nFeederEnable[i])
        {
            feedEnableBox1[i]->setChecked(true);
        }
        else
        {
            feedEnableBox1[i]->setChecked(false);
        }
    }

    updateEnableAllBtnState();
    myFlow.resetFeeder(0);
}

void setFeedPage::updateEnableAllBtnState()
{
    int index = 0;
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2; i++)
    {
        if (struCnfp.struGroupCtrl[0].nFeederEnable[i])
        {
            index++;
        }
    }

    if (index == struCnfg.struLevelInfo[ONE_LEVEL].nUnitLevelTotal / 2)
    {
        feedEnableAllBtn->setEnabled(false);
        feedDisableAllBtn->setEnabled(true);
    }
    else if (index == 0)
    {
        feedEnableAllBtn->setEnabled(true);
        feedDisableAllBtn->setEnabled(false);
    }
    else
    {
        feedEnableAllBtn->setEnabled(true);
        feedDisableAllBtn->setEnabled(true);
    }
}
