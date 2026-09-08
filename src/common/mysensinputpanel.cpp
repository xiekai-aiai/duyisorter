/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mysensinputpanel.cpp
 * \brief       灵敏度数字输入框控件源文件
 * \date        2015.01.14
 */
#include "mysensinputpanel.h"
#include "mylanguage.h"

mySensInputPanel::mySensInputPanel(sensInputType type, int nMin, int nMax, int index,
                                   int address, double nDisp, QWidget *parent)
    : QDialog(parent)
{
    nMinNum  = nMin;
    nMaxNum  = nMax;
    nDispNum = nDisp;
    nValueDisp = nDisp;
    nChannelIndex = index;
    nAddress = address;
    nFlagMode  = type;
    b_FlagCLear = true;
    m_nPreIndex = 0;
    config = g_Config::getInstance();

    if (LCD_WIDTH == 1024){
       font.setPixelSize(24);
    } else {
       font.setPixelSize(16);
    }

    /* 创建页面 */
    createButtons();

    /* 初始化临时变量 */
    initTmpData();

    /* 更新文本框显示 */
    updataTabBar();

    /* 连接信号与槽 */
    signalMapper = new QSignalMapper(this);
    for (int i=0; i<11; i++) {
        signalMapper->setMapping(button[i], i);
        connect(button[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sendChar(int)));
    connect(buttonC1, SIGNAL(pressed()), this, SLOT(buttonC1Function()));
    connect(buttonC2, SIGNAL(pressed()), this, SLOT(buttonC2Function()));
    connect(buttonC3, SIGNAL(pressed()), this, SLOT(buttonC3Function()));
    connect(tabBarIdentify, SIGNAL(currentChanged(int)), this, SLOT(updateTabBarStackWidget(int)));
}

mySensInputPanel::~mySensInputPanel()
{

}

/**
 * @brief mySensInputPanel::createButtons
 * 初始化页面，创建按钮
 */
void mySensInputPanel::createButtons(void)
{
    setWindowFlags(Qt::FramelessWindowHint);
    this->setMinimumSize(50,50);
    int wid = LCD_WIDTH*0.75;
    int hei = LCD_CONTENT_HEIGTH*0.9;

    setGeometry((LCD_WIDTH-wid)/2,(LCD_HEIGHT-hei)/2,wid,hei);
    setStyleSheet(g_style2);

    for(int i=0; i < 11; i++)
    {
        QString str;
        if (i == 10) {
            button.append(new myPushButton(".", QIcon(), this));
        } else if (i == 9) {
            button.append(new myPushButton("0", QIcon(), this));
        } else {
            str.sprintf("%d",i+1);
            button.append(new myPushButton(str, QIcon(), this));
        }
        button[i]->setFont(config->getFont(DEFAULT_FONT_SIZE));
        button[i]->setFocusPolicy(Qt::NoFocus);
    }

    titleLabel = new myLabel(myLan.input, this);
    tabBarIdentify = new QTabBar(this);
    tabBarIdentify->setFont(font);
    tabBarIdentify->insertTab(0,myLan.front_view);
    tabBarIdentify->insertTab(1,myLan.rear_view);
    tabBarIdentify->insertTab(2,myLan.config_assist+myLan.front_view);
    tabBarIdentify->insertTab(3,myLan.config_assist+myLan.rear_view);
    tabBarIdentify->setStyleSheet("QTabBar::tab{""min-height:50;min-width:150;}");
    tabBarIdentify->setFixedWidth(600);
    tabBarIdentify->setCurrentIndex(0);
    textLbe = new myLineEdit("", this);
    textLbe->setFont(config->getFont(DEFAULT_FONT_SIZE));
    buttonC1 = new myPushButton(myLan.del, myIcon.Input_Backspace,this);
    buttonC1->setFocusPolicy(Qt::NoFocus);
    buttonC2 = new myPushButton(myLan.ok, myIcon.Action_Apply,this);
    buttonC2->setFocusPolicy(Qt::NoFocus);
    buttonC3 = new myPushButton(myLan.cancel, myIcon.Action_Cancel,this);
    buttonC3->setFocusPolicy(Qt::NoFocus);
    buttonC1->setMaximumWidth(BTN_WIDTH);
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    mainVerticalLayout = new QVBoxLayout(this);

    horizontalLayout_0 = new QHBoxLayout();
    horizontalLayout_0->addWidget(tabBarIdentify);
    mainVerticalLayout->addLayout(horizontalLayout_0);

    horizontalLayout_1 = new QHBoxLayout();
    horizontalLayout_1->addWidget(textLbe);
    horizontalLayout_1->addWidget(buttonC1);
    mainVerticalLayout->addLayout(horizontalLayout_1);

    horizontalLayout_2 = new QHBoxLayout();
    mainVerticalLayout->addLayout(horizontalLayout_2);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->addWidget(button[0]);
    horizontalLayout_3->addWidget(button[1]);
    horizontalLayout_3->addWidget(button[2]);
    mainVerticalLayout->addLayout(horizontalLayout_3);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->addWidget(button[3]);
    horizontalLayout_4->addWidget(button[4]);
    horizontalLayout_4->addWidget(button[5]);
    mainVerticalLayout->addLayout(horizontalLayout_4);

    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->addWidget(button[6]);
    horizontalLayout_5->addWidget(button[7]);
    horizontalLayout_5->addWidget(button[8]);
    mainVerticalLayout->addLayout(horizontalLayout_5);

    gridLayout_6 = new QGridLayout();
    gridLayout_6->addWidget(button[9],0,0,1,1);
    gridLayout_6->addWidget(button[10],0,1,1,1);
    gridLayout_6->setColumnStretch(0, 2);
    gridLayout_6->setColumnStretch(1, 1);
    mainVerticalLayout->addLayout(gridLayout_6);

    gridLayout_7 = new QGridLayout();
    gridLayout_7->addWidget(buttonC2,0,0,1,1);
    gridLayout_7->addItem(horizontalSpacer, 0, 1, 1, 1);
    gridLayout_7->addWidget(buttonC3,0,2,1,1);
    mainVerticalLayout->addLayout(gridLayout_7);

    QString str = QString("%1").arg(nDispNum);
    textLbe->setText(str);

    if (nFlagMode == floatSensType) {
        if (str[str.length()-1] == '.') {
            button[10]->setEnabled(false);
        }
        else {
            button[10]->setEnabled(true);
        }
    } else if (nFlagMode == intSensType){
          button[10]->setEnabled(false);
    } else if (nFlagMode == passwdSensType){
        button[10]->setEnabled(false);
        textLbe->setText("");
        nMinNum = -1;
        nMaxNum = 10000000;
        textLbe->setEchoMode(QLineEdit::Password);
    } else if (nFlagMode == textSensType) {
        button[10]->setEnabled(true);
        textLbe->setText("");
        nMinNum = -1;
        nMaxNum = 10000000;
    }

    return;
}
/*
 * 各个字符槽函数，1，2，3...
 */
void mySensInputPanel::sendChar(int indexOfCharToSend)
{
    checkDotBtn();
    if (nFlagMode == floatSensType) {
        button[10]->setEnabled(true);
    }

    if (nFlagMode == passwdSensType || nFlagMode == textSensType) {
        if (b_FlagCLear) {
            b_FlagCLear = false;
            textLbe->setText("");
        }
        textLbe->insert(button[indexOfCharToSend]->text());
    }
    else {
        if (b_FlagCLear) {
            b_FlagCLear = false;
            nDispNum = 0;
            nValueDisp = nDispNum;
            textLbe->setText("");
        }
        textLbe->insert(button[indexOfCharToSend]->text());
        nDispNum = textLbe->text().toDouble();

        if (nDispNum >= nMaxNum){
            nDispNum = nMaxNum;
            QString str = QString("%1").arg(nDispNum);
            textLbe->setText(str);
        }
        if (nDispNum < nMinNum || textLbe->text() == "")
        {
            nDispNum = nMinNum;
            QString str = QString("%1").arg(nDispNum);
            textLbe->setText(str);
        }
        nValueDisp = nDispNum;
        checkDotBtn();
   }

    return;
}
/**
 * @brief mySensInputPanel::buttonC1Function
 * 删除上次输入
 */
void mySensInputPanel::buttonC1Function(void)
{
    // 屏蔽第一次清空参数
    b_FlagCLear = false;
    if (nFlagMode == textSensType || nFlagMode == passwdSensType) {
        QString str;
        str = textLbe->text();

        str.remove(str.length()-1,str.length());
        textLbe->setText(str);
    }
    else {
        nDispNum = 0;
        nValueDisp = nDispNum;
        textLbe->setText("");

        checkDotBtn();
        if (nFlagMode == floatSensType) {
            button[10]->setEnabled(true);
        }
    }

    return;
}

/**
 * @brief mySensInputPanel::buttonC2Function
 * 确定输入
 */
void mySensInputPanel::buttonC2Function(void)
{
    if(textLbe->text() == "") {
        QString str = QString("%1").arg(nMinNum);
        textLbe->setText(str);
    }
    m_nTmpData[tabBarIdentify->currentIndex()] = textLbe->text().toDouble()*10+0.5;
    accept();
}

/**
 * @brief mySensInputPanel::buttonC3Function
 * 取消输入
 */
void mySensInputPanel::buttonC3Function(void)
{
    reject();
}

/**
 * @brief mySensInputPanel::checkDotBtn
 * 检查点按钮输入使能
 */
void mySensInputPanel::checkDotBtn()
{
    QString str;
    str = textLbe->text();
    if (str.length()) {
        if (nFlagMode == floatSensType) {
            if (str[str.length()-1] == '.') {
                button[10]->setEnabled(false);
            }
        }
    }
}

/* 设置当前的值(浮点型) */
void mySensInputPanel::setValue(double value)
{
   QString str = QString("%1").arg(value);
   textLbe->setText(str);
   checkDotBtn();
}

/* 获取当前的值(浮点型) */
double mySensInputPanel::getValue()
{
    double valueTmp = textLbe->text().toDouble();
    return valueTmp;
}

/**
 * @brief mySensInputPanel::getText
 * @return
 */
QString mySensInputPanel::getText()
{
    return textLbe->text();
}

/**
 * @brief mySensInputPanel::setTitle
 * @param text
 */
void mySensInputPanel::setTitle(QString text)
{
    titleLabel->setText(text);
}

void mySensInputPanel::setText(QString text)
{
    textLbe->setText(text);
}

void mySensInputPanel::setInputType(sensInputType type)
{
    nFlagMode = type;
}

void mySensInputPanel::setValueMin(int nMin)
{
    nMinNum  = nMin;
}

void mySensInputPanel::setValueMax(int nMax)
{
    nMaxNum  = nMax;
}

/* 初始化临时变量 */
void mySensInputPanel::initTmpData()
{
    m_nTmpData[0] = struCnfp.nSens[2*nChannelIndex][nAddress];
    m_nTmpData[1] = struCnfp.nSens[2*nChannelIndex+1][nAddress];
}

/* 获取灵敏度 */
int mySensInputPanel::getSens(int nIndex)
{
    return m_nTmpData[nIndex];
}

void mySensInputPanel::updataTabBar()
{
    if(struCnfg.nAssistCamEn[nChannelIndex*2]) {
        tabBarIdentify->setTabEnabled(2,true);
    } else {
        tabBarIdentify->setTabEnabled(2,false);
    }

    if(struCnfg.nAssistCamEn[nChannelIndex*2+1]) {
        tabBarIdentify->setTabEnabled(3,true);
    } else {
        tabBarIdentify->setTabEnabled(3,false);
    }
}

/* 根据标签条更新文本框显示 */
void mySensInputPanel::updateTabBarStackWidget(int nTabIndex)
{
    /* 保存前一标签的数据 */
    double fTmp = textLbe->text().toDouble();
    m_nTmpData[m_nPreIndex] = fTmp*10+0.5;

    QString str = QString("%1").arg(m_nTmpData[nTabIndex]*0.1);
    textLbe->setText(str);
    m_nPreIndex = nTabIndex;
}
