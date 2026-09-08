/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myinputpanel.h.cpp
 * \brief       数字输入框控件源文件
 * \date        2015.01.14
 */
#include "myinputpanel.h"

myInputPanel::myInputPanel(inputType type,int nMin, int nMax, double nDisp, QWidget *parent)
    : QDialog(parent)
{
    nMinNum  = nMin;
    nMaxNum  = nMax;
    nDispNum = nDisp;
    nValueDisp = nDisp;
    nFlagMode  = type;
    b_FlagCLear = true;
    m_bJudgeEn = true;
    config = g_Config::getInstance();

    createButtons();

    signalMapper = new QSignalMapper(this);
    for (int i=0; i<12; i++) {
        signalMapper->setMapping(button[i], i);
        connect(button[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sendChar(int)));

    // 信号槽
    connect(buttonC1, SIGNAL(pressed()), this, SLOT(buttonC1Function()));
    connect(buttonC2, SIGNAL(pressed()), this, SLOT(buttonC2Function()));
    connect(buttonC3, SIGNAL(pressed()), this, SLOT(buttonC3Function()));
    connect(minusBtn, SIGNAL(pressed()), this, SLOT(onMinusBtnClickedSlt()));
    connect(addBtn  , SIGNAL(pressed()), this, SLOT(onAddBtnClickedSlt()));
    connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onValueSliderChangeSlt()));
}

myInputPanel::~myInputPanel()
{
    
}
/**
 * @brief myInputPanel::createButtons
 * 初始化页面，创建按钮
 */
void myInputPanel::createButtons(void)
{
    setWindowFlags(Qt::FramelessWindowHint);
    this->setMinimumSize(50,50);
    int wid = LCD_WIDTH*0.75;
    int hei = LCD_CONTENT_HEIGTH*0.9;

    setGeometry((LCD_WIDTH-wid)/2,(LCD_HEIGHT-hei)/2,wid,hei);
//    setStyleSheet(g_style2);
    QPalette palette;
    QPixmap pixmap(":/res/png/inputPanelBack.png");
    pixmap = pixmap.scaled(wid+5,hei+8);

    palette.setBrush(backgroundRole(),QBrush(pixmap));
    setPalette(palette);
    setAutoFillBackground(true);
//    setStyleSheet("background-color:transparent");

    for(int i=0; i < 12; i++) {
        QString str;

        if (i == 11) {
            button.append(new myPushButton("-", QIcon()));
        } else if (i == 10) {
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
    textLbe = new myLineEdit("", this);
    textLbe->setFont(config->getFont(DEFAULT_FONT_SIZE));
    buttonC1 = new myPushButton("", QIcon(":/res/png/inputDel.png"),this);
    QString g_style1 = "background-color: rgb(124, 186, 224);""outline: none;""border-radius: 8;";
    buttonC1->setStyleSheet(g_style1);
    buttonC1->setFocusPolicy(Qt::NoFocus);


    buttonC2 = new myPushButton(myLan.ok, QIcon(),this);
    buttonC2->setStyleSheet(g_style1);
    buttonC2->setFocusPolicy(Qt::NoFocus);

    buttonC3 = new myPushButton(myLan.cancel, QIcon(),this);
    QString g_style2 = "background-color: rgb(96, 156, 194);""outline: none;""border-radius: 8;";
    buttonC3->setStyleSheet(g_style2);
    buttonC3->setFocusPolicy(Qt::NoFocus);


    buttonC1->setMaximumWidth(BTN_WIDTH);
    minusBtn = new myPushButton("",myIcon.Action_Minus,this);
    minusBtn->setMaximumWidth(ICON_WID+8);
    minusBtn->setMaximumHeight(ICON_WID+8);
    addBtn = new myPushButton("",myIcon.Action_Plus,this);
    addBtn->setMaximumWidth(ICON_WID+8);
    addBtn->setMaximumHeight(ICON_WID+8);

    valueSlider  = new mySlider(Qt::Horizontal, nMinNum, nMaxNum, nValueDisp, this);
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    mainVerticalLayout = new QVBoxLayout(this);

    horizontalLayout_0 = new QHBoxLayout();
    horizontalLayout_0->addWidget(titleLabel,1,Qt::AlignCenter);
    mainVerticalLayout->addLayout(horizontalLayout_0);

    horizontalLayout_1 = new QHBoxLayout();
    horizontalLayout_1->addWidget(textLbe);
    horizontalLayout_1->addWidget(buttonC1);
    mainVerticalLayout->addLayout(horizontalLayout_1);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->addWidget(minusBtn);
    horizontalLayout_2->addWidget(valueSlider);
    horizontalLayout_2->addWidget(addBtn);
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
    gridLayout_6->addWidget(button[9], 0, 0);
    gridLayout_6->addWidget(button[10], 0, 1);
    button[11]->hide();
    gridLayout_6->addWidget(button[11], 0, 2);
    gridLayout_6->setColumnStretch(0, 2);
    gridLayout_6->setColumnStretch(1, 1);
    mainVerticalLayout->addLayout(gridLayout_6);


    gridLayout_7 = new QGridLayout();
    gridLayout_7->addWidget(buttonC3,0,0,1,1);
    gridLayout_7->addItem(horizontalSpacer, 0, 1, 1, 1);
    gridLayout_7->addWidget(buttonC2,0,2,1,1);
    mainVerticalLayout->addLayout(gridLayout_7);

    QString str = QString("%1").arg(nDispNum);
    textLbe->setText(str);

    if (nFlagMode == floatType) {
        if (str[str.length()-1] == '.') {
              button[10]->setEnabled(false);
        } else {
           button[10]->setEnabled(true);
        }
    } else if (nFlagMode == intType){
          button[10]->setEnabled(false);
    } else if (nFlagMode == passwdType){
        button[10]->setEnabled(false);
        minusBtn->hide();
        addBtn->hide();
        valueSlider->hide();
        textLbe->setText("");
        nMinNum = -1;
        nMaxNum = 10000000;
        textLbe->setEchoMode(QLineEdit::Password);
    } else if (nFlagMode == textType) {
        button[10]->setEnabled(true);
        minusBtn->hide();
        addBtn->hide();
        valueSlider->hide();
        textLbe->setText("");
        nMinNum = -1;
        nMaxNum = 10000000;
    }

    return;
}
/*
 * 各个字符槽函数，1，2，3...
 */
void myInputPanel::sendChar(int indexOfCharToSend)
{
    checkDotBtn();
    if(m_bInputNegSign) {
        button[11]->setEnabled(false);
    }

    if (nFlagMode == floatType) {
        button[10]->setEnabled(true);
    }

    if (nFlagMode == passwdType || nFlagMode == textType) {
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
            valueSlider->setValue(nValueDisp);
            textLbe->setText("");
        }
        textLbe->insert(button[indexOfCharToSend]->text());
        nDispNum = textLbe->text().toDouble();

        if (nDispNum >= nMaxNum && m_bJudgeEn) {
            nDispNum = nMaxNum;
            QString str = QString("%1").arg(nDispNum);
            textLbe->setText(str);
        }
        if ((nDispNum < nMinNum || textLbe->text() == "")
                && textLbe->text() != "-") {
            nDispNum = nMinNum;
            QString str = QString("%1").arg(nDispNum);
            textLbe->setText(str);
        }
        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);

        checkDotBtn();
   }

    return;
}
/**
 * @brief myInputPanel::buttonC1Function
 * 删除上次输入
 */
void myInputPanel::buttonC1Function(void)
{
    // 屏蔽第一次清空参数
    b_FlagCLear = false;
    if(m_bInputNegSign) {
        button[11]->setEnabled(true);
    }
    if (nFlagMode == textType || nFlagMode == passwdType) {
        QString str;
        str = textLbe->text();

        str.remove(str.length()-1,str.length());
        textLbe->setText(str);
    }
    else {
        nDispNum = 0;
        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);
        textLbe->setText("");

        checkDotBtn();
        if (nFlagMode == floatType) {
            button[10]->setEnabled(true);
        }
    }

    return;
}
/**
 * @brief myInputPanel::buttonC2Function
 * 确定输入
 */
void myInputPanel::buttonC2Function(void)
{

    if (textLbe->text() == "") {
        if (nFlagMode != textType) {
            QString str = QString("%1").arg(nMinNum);
            textLbe->setText(str);
        }
    }
    accept();

}
/**
 * @brief myInputPanel::buttonC3Function
 * 取消输入
 */
void myInputPanel::buttonC3Function(void)
{
    reject();
}
/**
 * @brief myInputPanel::onValueSliderChangeSlt
 * 滑动框输入
 */
void myInputPanel::onValueSliderChangeSlt()
{

    nValueDisp = valueSlider->value();
    nDispNum   = nValueDisp+nDispNum - (int)nDispNum;
    if (nDispNum >= nMaxNum && m_bJudgeEn){
        nDispNum = nMaxNum;
    }
    if (nDispNum < nMinNum){
        nDispNum = nMinNum;
    }
    valueSlider->setValue(nValueDisp);
    QString str = QString("%1").arg(nDispNum);
    textLbe->setText(str);


    checkDotBtn();

}
/**
 * @brief myInputPanel::onAddBtnClickedSlt
 * 手动增加数据
 */
void myInputPanel::onAddBtnClickedSlt()
{
    if (nDispNum < nMaxNum) {
        if (nFlagMode == intType) {
             nDispNum++;
        } else if (nFlagMode == floatType){
            nDispNum += 0.01;

        }
        if (nDispNum >= nMaxNum && m_bJudgeEn){
            nDispNum = nMaxNum;
        }
        QString str = QString("%1").arg(nDispNum);
        textLbe->setText(str);
        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);
        checkDotBtn();
    }
}
/**
 * @brief myInputPanel::onMinusBtnClickedSlt
 * 手动减少数据
 */
void myInputPanel::onMinusBtnClickedSlt()
{
    if (nDispNum > nMinNum) {
        if (nFlagMode == intType) {
              nDispNum--;
        } else if (nFlagMode == floatType){
            nDispNum -= 0.01;
        }

        if (nDispNum < 0.1){
            nDispNum = nMinNum;
        }

        QString str = QString("%1").arg(nDispNum);
        textLbe->setText(str);

        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);
        checkDotBtn();
    }
}
/**
 * @brief myInputPanel::checkDotBtn
 * 检查点按钮输入使能
 */
void myInputPanel::checkDotBtn()
{
    QString str;
    str = textLbe->text();
    if (str.length()) {
        if (nFlagMode == floatType) {
            if (str[str.length()-1] == '.') {
                  button[10]->setEnabled(false);
            }
        }
    }
}

/* 设置当前的值(浮点型) */
void myInputPanel::setValue(double value)
{
   QString str = QString("%1").arg(value);

   textLbe->setText(str);
   checkDotBtn();
}

/* 获取当前的值(浮点型) */
double myInputPanel::getValue()
{
    double valueTmp = textLbe->text().toDouble();
    return valueTmp;
}
/**
 * @brief myInputPanel::getText
 * @return
 */
QString myInputPanel::getText()
{
    return textLbe->text();
}
/**
 * @brief myInputPanel::setTitle
 * @param text
 */
void myInputPanel::setTitle(QString text)
{
    titleLabel->setText(text);
}

void myInputPanel::setText(QString text)
{
    textLbe->setText(text);
}

void myInputPanel::setInputType(inputType type)
{
    nFlagMode = type;
}

void myInputPanel::setValueMin(int nMin)
{
    nMinNum  = nMin;
}

void myInputPanel::setValueMax(int nMax)
{
    nMaxNum  = nMax;
}

/* 设置是否可输入负数 */
void myInputPanel::setInputNegEn(bool bFlag)
{
    m_bInputNegSign = bFlag;
    if(bFlag) {     // 可输入负数
        button[11]->show();
        minusBtn->hide();
        addBtn->hide();
        valueSlider->hide();
        gridLayout_6->setColumnStretch(0, 0);
        gridLayout_6->setColumnStretch(1, 0);
    } else {        // 不可输入负数
        button[11]->hide();
        minusBtn->show();
        addBtn->show();
        valueSlider->show();
        gridLayout_6->setColumnStretch(0, 2);
        gridLayout_6->setColumnStretch(1, 1);
    }
}

/* 设置是否判断最大值及最小值 */
void myInputPanel::setJudgeEn(bool bFlag)
{
    m_bJudgeEn = bFlag;
}
