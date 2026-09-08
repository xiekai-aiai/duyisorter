/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        paramsetalone.cpp
 * \brief       黑白机型独立设置界面源文件。
 * \date        2015.01.14
 */
#include "paramsetalone.h"

paramSetAlone::paramSetAlone(QWidget *parent) :
    QWidget(parent)
{
    m_config = g_Config::getInstance();
    createPage();
    connectSigAndSlt();
}

paramSetAlone::paramSetAlone(QSize size, QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(size);
    m_config = g_Config::getInstance();
    createPage();
    connectSigAndSlt();
}

/* 创建页面显示 */
void paramSetAlone::createPage()
{
    /* 定义页面布局管理器 */
    QGridLayout *mainLay = new QGridLayout(this);

    /* 实例化页面控件 */
    m_mutex = new myMutex(Qt::Vertical);

    m_mutex->setLabelText(myLan.set_by_times, myLan.set_alone);
    m_mutex->setNum(2);

    if (LCD_WIDTH == 1024) {
        m_mutex->setFixedSize(500, 300);
    } else {
        m_mutex->setFixedSize(300, 200);
    }

    mainLay->addWidget(m_mutex, 0, 0);
}

/* 连接页面中的信号与槽 */
void paramSetAlone::connectSigAndSlt()
{
    connect(m_mutex, SIGNAL(indexChanged(int)), this, SLOT(onMutexIndexChangedSlt(int)));
}

/* 刷新页面的显示 */
void paramSetAlone::refreshDisplay()
{
    m_nAloneSet = struCnfg.nParamSetAlone;
    m_mutex->setCurrentIndex(m_nAloneSet);
}

/* 将局部参数保存至全局参数 */
void paramSetAlone::saveLocalParams()
{
    struCnfg.nParamSetAlone = m_nAloneSet;
}

/* 响应互斥索引变化事件 */
void paramSetAlone::onMutexIndexChangedSlt(int index)
{
    m_nAloneSet = index;
    struCnfg.nParamSetAlone = m_nAloneSet;
}
