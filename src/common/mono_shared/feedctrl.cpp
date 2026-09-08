/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        feedctrl.cpp
 * \brief       黑白机型料位监控界面源文件。
 * \date        2015.01.14
 */
#include "feedctrl.h"

FeedCtrl::FeedCtrl(QWidget *parent) :
    QWidget(parent)
{
    /* 料位监控线程 */
    m_thread = new FeederControlThread();
	if (struCnfg.nFeederCtrlEn == 1) {
    	m_thread->start();
	}
    
	createPage();
    connectSigAndSlt();
}

/* 创建料位监控页面 */
void FeedCtrl::createPage()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *upHlay = new QHBoxLayout();

    /* 实例化互斥框 */
    m_mutex = new myMutex(Qt::Horizontal);
    m_mutex->setNum(4);
    QStringList strList;
    QString str1 = QString("%1 2").arg(myLan.single);
    strList << myLan.disable << myLan.single << myLan.complex << str1;
    m_mutex->setLabelText(strList);
    m_mutex->setFixedHeight(BTN_HEIGHT*2);

    // 一键放料
    m_oneKeyFeedBtn = new myPushButton(myLan.one_key_feed, QIcon(), true, true, this);
    m_oneKeyFeedBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_bIsFeed = false;

    upHlay->addWidget(m_mutex);
    upHlay->addWidget(m_oneKeyFeedBtn);

    /* 实例化料位信息显示部分控件 */
    QGroupBox *group = new QGroupBox();
    QGridLayout *groupLay = new QGridLayout(group);
    groupLay->setVerticalSpacing(0);

    QHBoxLayout *btnLay[FEED_CTRL_COL_NUM];
    for (int i = 0; i < FEED_CTRL_COL_NUM; i++) {
        m_selCbx[i] = new myCustomCheckBox("", false);
        m_selCbx[i]->setFixedSize(BTN_WIDTH, BTN_HEIGHT+10);
        btnLay[i] = new QHBoxLayout;
        btnLay[i]->addWidget(m_selCbx[i]);
        btnLay[i]->setContentsMargins(20, 5, 20, 5);
        groupLay->addLayout(btnLay[i], 0, i+1);
    }

    QStringList strlevel;
    strlevel << myLan.upper << myLan.mid << myLan.lower;
    for (int i = 0; i < FEED_CTRL_ROW_NUM; i++) {
        m_rowLabel[i] = new myLabel(myLan.feeder_level+QString(" %1").arg(strlevel.at(i)));
        m_rowLabel[i]->setAlignment(Qt::AlignCenter);
        groupLay->addWidget(m_rowLabel[i], i+1, 0);
    }

    QFont font;
    font.setPixelSize(40);
    for (int i = 0; i < FEED_CTRL_COL_NUM; i++) {
        for (int j = 0; j < FEED_CTRL_ROW_NUM; j++) {
            m_lineEdit[i][j] = new myLineEdit("");
            m_lineEdit[i][j]->setEnabled(false);
            m_lineEdit[i][j]->setFixedWidth(BTN_WIDTH);
            if (LCD_WIDTH == 1024) {
                m_lineEdit[i][j]->setFixedHeight(BTN_HEIGHT+20);
            } else {
                m_lineEdit[i][j]->setFixedHeight(BTN_HEIGHT);
            }
            m_lineEdit[i][j]->setAlignment(Qt::AlignCenter);
            m_lineEdit[i][j]->setFont(font);
            groupLay->addWidget(m_lineEdit[i][j], j+1, i+1, 1, 1, Qt::AlignHCenter);
        }
    }
    QHBoxLayout *downLay = new QHBoxLayout();
    downLay->setContentsMargins(40,0,0,0);
    for (int i = 0; i < 3; i++) {
        m_colorLet[i] = new myLineEdit();
        m_colorLbl[i] = new myLabel();
        m_colorLet[i]->setEnabled(false);
        m_colorLet[i]->setFixedSize(ICON_WIDTH,ICON_HEIGHT);
        m_colorLbl[i]->setFixedSize(BTN_WIDTH,BTN_HEIGHT);
        downLay->addWidget(m_colorLet[i],i*2,Qt::AlignRight);
        downLay->addWidget(m_colorLbl[i],i*2+1,Qt::AlignRight);
    }
    m_colorLet[0]->setStyleSheet(g_style3);
    m_colorLet[1]->setStyleSheet(g_style1);
    m_colorLet[2]->setStyleSheet(g_styleBlue);
    m_colorLbl[0]->setText(myLan.disable);
    m_colorLbl[1]->setText(myLan.no_material);
    m_colorLbl[2]->setText(myLan.exist_material);
    groupLay->addLayout(downLay,4,1,1,3);

    mainLay->addLayout(upHlay);
    if (LCD_WIDTH == 1024) {
        mainLay->setContentsMargins(20, 5, 50, 5);
        mainLay->addSpacing(20);
    } else {
        mainLay->setContentsMargins(5, 5, 25, 5);
    }
    mainLay->addWidget(group);
}

/* 连接信号与槽 */
void FeedCtrl::connectSigAndSlt()
{
    m_sigMap = new QSignalMapper;
    for (int i = 0; i < FEED_CTRL_COL_NUM; i++) {
        m_sigMap->setMapping(m_selCbx[i], i);
        connect(m_selCbx[i], SIGNAL(pressed()), m_sigMap, SLOT(map()));
    }
    connect(m_sigMap, SIGNAL(mapped(int)), this, SLOT(onSelBtnPressed(int)));

    connect(m_mutex, SIGNAL(indexChanged(int)), this, SLOT(onMutexIndexChangedSlt(int)));
    connect(m_oneKeyFeedBtn,SIGNAL(clicked()),this,SLOT(onOneKeyFeedBtnPressedSlt()));
    connect(m_thread, SIGNAL(refreshFeederInfo()), this, SLOT(refreshFeederInfo()));
}

/* 刷新页面的显示 */
void FeedCtrl::refreshDisplay()
{
    /* 刷新单选框 */
    if (struCnfg.nFeederCtrlEn == 0) {
        m_mutex->setCurrentIndex(0);
    } else {
        if (struCnfg.nFeederCtrlMode == 0) {
            m_mutex->setCurrentIndex(1);
        } else if (struCnfg.nFeederCtrlMode == 1) {
            m_mutex->setCurrentIndex(2);
        } else if (struCnfg.nFeederCtrlMode == 2) {
            m_mutex->setCurrentIndex(3);
        }
    }

    //! 刷新分次使能状态
    refreshSelBtnStatus();

    /* 刷新料位信息的显示 */
    refreshFeedDiplay();
    refreshFeedStatus();

    /* 设置料位监控线程发送料位信息信号 */
    m_thread->setNeedEmitFlag(true);
}

/* 刷新料位信息的显示 */
void FeedCtrl::refreshFeedDiplay()
{
    for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
        m_selCbx[i]->setCheckBoxName(myString.sTickGroupName[0][i]);
        m_selCbx[i]->show();
        for (int j = 0; j < FEED_CTRL_ROW_NUM; j++) {
            m_lineEdit[i][j]->show();
        }
    }

    for (int i = struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i < MAX_GROUP_TICK; i++) {
        m_selCbx[i]->hide();
        for (int j = 0; j < FEED_CTRL_ROW_NUM; j++) {
            m_lineEdit[i][j]->hide();
        }
    }
}

/* 刷新料位状态 */
void FeedCtrl::refreshFeedStatus()
{
    if (struCnfg.nFeederCtrlEn == 0) {
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
            for (int j = 0; j < FEED_CTRL_ROW_NUM; j++) {
                m_lineEdit[i][j]->setStyleSheet(g_style3);
            }
        }
    } else if (struCnfg.nFeederCtrlMode == 0
               || struCnfg.nFeederCtrlMode == 2) {
        //! 单一料位
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
            m_lineEdit[i][0]->setStyleSheet(g_style3);
            m_lineEdit[i][1]->setStyleSheet(g_style3);
            if (struCnfg.nFeederCtrlTickEn[i] == 1) {
                m_lineEdit[i][2]->setStyleSheet("");
            } else {
                m_lineEdit[i][2]->setStyleSheet(g_style3);
            }
        }
    } else if (struCnfg.nFeederCtrlMode == 1) {
        //! 复合料位
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
            if (struCnfg.nFeederCtrlTickEn[i] == 1) {
                for (int j = 0; j < FEED_CTRL_ROW_NUM; j++) {
                    m_lineEdit[i][j]->setStyleSheet("");
                }
            } else {
                for (int j = 0; j < FEED_CTRL_ROW_NUM; j++) {
                    m_lineEdit[i][j]->setStyleSheet(g_style3);
                }
            }
        }
    }
}

/*!
 * \brief 刷新分次使能按钮状态
 */
void FeedCtrl::refreshSelBtnStatus()
{
    for (int i = 0; i < FEED_CTRL_COL_NUM; i++) {
        if (struCnfg.nFeederCtrlEn == 0) {
            m_selCbx[i]->setEnabled(false);
            m_selCbx[i]->setChecked(false);
        } else {
            m_selCbx[i]->setEnabled(true);
            if (struCnfg.nFeederCtrlTickEn[i] == 1) {
                m_selCbx[i]->setChecked(true);
            } else {
                m_selCbx[i]->setChecked(false);
            }
        }
    }

    //! 刷新一键放料按钮状态
    if(struCnfg.nFeederCtrlEn == 0) {
        m_oneKeyFeedBtn->setEnabled(false);
        m_oneKeyFeedBtn->setRedColor(DEF);
        struGsh.bStatFeedCtrlEn = 1;
        m_bIsFeed = false;
    } else if(struGsh.bStatFeed){
        m_oneKeyFeedBtn->setEnabled(true);
    }
    if(struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal == 4){
        struCnfg.nFeederCtrlTickEn[3] = 0;
        m_selCbx[3]->setEnabled(false);
        m_selCbx[3]->setChecked(false);
    }
}

/* 停止料位信息的刷新 */
void FeedCtrl::stopRefresh()
{
    m_thread->setNeedEmitFlag(false);
}

/* 单选框索引变化 */
void FeedCtrl::onMutexIndexChangedSlt(int nIndex)
{
    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    switch (nIndex) {
    case 0:
        struCnfg.nFeederCtrlEn = 0;
        if (m_thread->threadIsRunning()) {
            m_thread->stop();
            struGsh.nAlarmLevel = ALARM_LEVEL_NULL;
            for (int i = 0 ; i < MAX_GROUP_TICK; i++) {
                struGsh.nAlarmLevelGroup[i] = 0;
            }
			/* 打开所有振动器 */
			for (int i=0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal/2; i++) {
            	struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
        	}
        	myFlow.resetFeeder(0);
            for (int i=0; i<struCnfg.struLevelInfo[0].nUnitLevelTotal/2; i++) {
                myLog->info(LOG_FEED,"FeederStatus%d:%d",i,struCnfp.struGroupCtrl[0].nFeederEnable[i]);
            }

            //保证线程完全退出
            myFlow.sleep(5);
        }
        break;

    case 1:
        struCnfg.nFeederCtrlEn = 1;
        struCnfg.nFeederCtrlMode = 0;
        m_thread->setCtrlMode(FEEDER_CONTROL_MODE_1);
        if (!m_thread->threadIsRunning()) {
            m_thread->start();
        }
        break;

    case 2:
        struCnfg.nFeederCtrlEn = 1;
        struCnfg.nFeederCtrlMode = 1;
        m_thread->setCtrlMode(FEEDER_CONTROL_MODE_2);
        if (!m_thread->threadIsRunning()) {
            m_thread->start();
        }
        break;

    case 3:
        struCnfg.nFeederCtrlEn = 1;
        struCnfg.nFeederCtrlMode = 2;
        m_thread->setCtrlMode(FEEDER_CONTROL_MODE_3);
        if (!m_thread->threadIsRunning()) {
            m_thread->start();
        }
        break;

    default:
        break;
    }

    refreshSelBtnStatus();
    refreshFeedStatus();

    myFlow.sleep(2);
    infoWidget->delayHide();
}

/* 刷新料位信息的显示 */
void FeedCtrl::refreshFeederInfo()
{
    if (struCnfg.nFeederCtrlEn == 0) {
        return;
    }

    char *sFeederInfo = m_thread->getFeederInfo();
    if (struCnfg.nFeederCtrlMode == 0
            || struCnfg.nFeederCtrlMode == 2) {
        /* 单一模式 */
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
            if (struCnfg.nFeederCtrlTickEn[i] == 1) {
                if (sFeederInfo[3*i] == 1) {
                    m_lineEdit[i][2]->setStyleSheet(g_style1);
                } else {
                    m_lineEdit[i][2]->setStyleSheet(g_styleBlue);
                }
            }
        }
    } else if (struCnfg.nFeederCtrlMode == 1) {
        /* 复合模式 */
        for (int i = 0; i < struCnfg.struLevelInfo[ONE_LEVEL].nTickGroupTotal; i++) {
            if (struCnfg.nFeederCtrlTickEn[i] == 1) {
                if (sFeederInfo[3*i] == 0) {
                    m_lineEdit[i][0]->setStyleSheet(g_styleBlue);
                } else {
                    m_lineEdit[i][0]->setStyleSheet(g_style1);
                }
                if (sFeederInfo[3*i+1] == 0) {
                    m_lineEdit[i][1]->setStyleSheet(g_styleBlue);
                } else {
                    m_lineEdit[i][1]->setStyleSheet(g_style1);
                }
                if (sFeederInfo[3*i+2] == 0) {
                    m_lineEdit[i][2]->setStyleSheet(g_styleBlue);
                } else {
                    m_lineEdit[i][2]->setStyleSheet(g_style1);
                }
            }
        }
    }
}

/*!
 * \brief 分次使能状态变化
 * \param nIndex 索引号
 */
void FeedCtrl::onSelBtnPressed(int nIndex)
{
    if (struCnfg.nFeederCtrlTickEn[nIndex] == 0) {
        struCnfg.nFeederCtrlTickEn[nIndex] = 1;
        m_selCbx[nIndex]->setChecked(true);
    } else {
        struCnfg.nFeederCtrlTickEn[nIndex] = 0;
        m_selCbx[nIndex]->setChecked(false);

        //! 关闭料位监控分次使能后开启振动器使能
        m_thread->openGroupFeed(nIndex);
        myFlow.resetFeeder(0);
    }

    refreshFeedStatus();
}

/*!
 * \brief 一键放料按钮按下
 * \param
 */
void FeedCtrl::onOneKeyFeedBtnPressedSlt()
{
    m_bIsFeed = !m_bIsFeed;

    if (m_bIsFeed){
        m_oneKeyFeedBtn->setRedColor(GREEN);
        struGsh.bStatFeedCtrlEn = 0;

        for (int i = 0; i < ((struCnfg.struLevelInfo[0].nViewTotal == 1)?struCnfg.struLevelInfo[0].nUnitLevelTotal/2:struCnfg.struLevelInfo[0].nUnitLevelTotal/4)/2; i++) {
                    struCnfp.struGroupCtrl[0].nFeederEnable[i] = 1;
        }
        myFlow.resetFeeder(0);
    }else {
        m_oneKeyFeedBtn->setRedColor(DEF);
        struGsh.bStatFeedCtrlEn = 1;
    }
}

/*!
 * \brief 不使能一键放料
 */
void FeedCtrl::hideOneKeyFeedSlt()
{
    m_oneKeyFeedBtn->setEnabled(false);
    m_oneKeyFeedBtn->setRedColor(DEF);
    struGsh.bStatFeedCtrlEn = 1;
    m_bIsFeed = false;
}

/*!
 * \brief 使能一键放料
 */
void FeedCtrl::showOneKeyFeedSlt()
{
    if(struCnfg.nFeederCtrlEn == 1) {
        m_oneKeyFeedBtn->setEnabled(true);
        m_oneKeyFeedBtn->setRedColor(DEF);
        struGsh.bStatFeedCtrlEn = 1;
        m_bIsFeed = false;
    }
}
