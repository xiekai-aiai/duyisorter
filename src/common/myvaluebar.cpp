/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myvaluebar.cpp
 * \brief       数字显示控制控件源文件
 * \date        2015.01.14
 */
#include "myvaluebar.h"

myValueBar::myValueBar(int min, int max, double slowChgValue, double fastChgValue, double initValue, QWidget *parent)
    :QWidget(parent)
{
    config = g_Config::getInstance();
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);

    m_bIsPop = true;
    m_bEnableChanged = true;
    minValue = min;
    maxValue = max;
    chgValue = slowChgValue;
    chgFastValue = fastChgValue;
    style = 0;

    minusFastBtn = new myPushButton(myIcon.Arrow_LeftFast,btnSize,this);
    minusBtn = new myPushButton(myIcon.Arrow_Left,btnSize,this);
    lcdNum = new myLCDNumber;
    lcdNum->setFixedSize(80, btnSize.height());

    /* 根据最大值设置lcdNum显示数字的位数 */
    setDigitalCountByMax();
    if(maxValue > 999) {
        lcdNum->setFixedWidth(100);
    }

    lcdNum->display(initValue);
    plusBtn = new myPushButton(myIcon.Arrow_Right,btnSize,this);
    plusFastBtn = new myPushButton(myIcon.Arrow_RightFast,btnSize,this);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(minusFastBtn);
    lay->addWidget(minusBtn);
    lay->addWidget(lcdNum);
    lay->addWidget(plusBtn);
    lay->addWidget(plusFastBtn);

    connect(minusFastBtn, SIGNAL(pressed()), this, SLOT(onMinusFastBtnClicked()));
    connect(minusBtn, SIGNAL(pressed()), this, SLOT(onMinusBtnClicked()));
    connect(plusBtn, SIGNAL(pressed()), this, SLOT(onPlusBtnClicked()));
    connect(plusFastBtn, SIGNAL(pressed()), this, SLOT(onPlusFastBtnClicked()));
    connect(lcdNum, SIGNAL(pressed()), this, SLOT(onLcdNumberPressed()));
}

myValueBar::myValueBar(double *arr, int arrElementTotal, double initValue, QWidget *parent)
    :QWidget(parent)
{
    config = g_Config::getInstance();
    QSize btnSize = config->getBtnSize(SMALL_BTN_SIZE);

    m_bIsPop = true;    //禁用Lcd编辑
    m_bEnableChanged = true;
    style = 1;
    currentArrSuffix = 0;
    arrEleTotal = arrElementTotal;

    maxValue = arr[0];
    minValue = arr[0];

    for (int i = 0; i < arrEleTotal; i++) {
        tmpArr[i] = arr[i];
        if ((initValue - tmpArr[i]) < 0.000001 && (initValue - tmpArr[i]) > -0.000001)  {
            currentArrSuffix = i;
        }
        minValue = (minValue < tmpArr[i]) ? minValue  : tmpArr[i];
        maxValue = (maxValue < tmpArr[i]) ? tmpArr[i] : maxValue;
    }

    minusFastBtn = new myPushButton(myIcon.Arrow_LeftFast,btnSize,this);
    minusBtn = new myPushButton(myIcon.Arrow_Left,btnSize,this);
    lcdNum = new myLCDNumber;
    lcdNum->setFixedSize(55, btnSize.height());

    // 根据最大值设置lcdNum显示数字的位数
    setDigitalCountByMax();
    if(maxValue > 999) {
        lcdNum->setFixedWidth(85);
    }

    lcdNum->display(initValue);
    plusBtn = new myPushButton(myIcon.Arrow_Right,btnSize,this);
    plusFastBtn = new myPushButton(myIcon.Arrow_RightFast,btnSize,this);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(minusFastBtn);
    lay->addWidget(minusBtn);
    lay->addWidget(lcdNum);
    lay->addWidget(plusBtn);
    lay->addWidget(plusFastBtn);
    minusFastBtn->hide();
    plusFastBtn->hide();

    connect(minusFastBtn, SIGNAL(pressed()), this, SLOT(onMinusFastBtnClicked()));
    connect(minusBtn, SIGNAL(pressed()), this, SLOT(onMinusBtnClicked()));
    connect(plusBtn, SIGNAL(pressed()), this, SLOT(onPlusBtnClicked()));
    connect(plusFastBtn, SIGNAL(pressed()), this, SLOT(onPlusFastBtnClicked()));
    connect(lcdNum, SIGNAL(pressed()), this, SLOT(onLcdNumberPressed()));
}

myValueBar::~myValueBar()
{
}

/* 设置当前的值(浮点型) */
void myValueBar::setValue(double value)
{
    lcdNum->display(value);
}

/* 获取当前的值(浮点型) */
double myValueBar::getValue()
{
    return lcdNum->value();
}

/* 隐藏快速变化按钮 */
void myValueBar::hideFastChangeBtns()
{
    minusFastBtn->hide();
    plusFastBtn->hide();
}

/* 设置最大值 */
void myValueBar::setMaxValue(int value)
{
    maxValue = value;
    setDigitalCountByMax();
}

/* 设置最小值 */
void myValueBar::setMinValue(int value)
{
    minValue = value;
}

/* 设置加速增加/减少的幅度 */
void myValueBar::setChgFastValue(int value)
{
    chgFastValue = value;
}

/* 设置增加/减少的幅度 */
void myValueBar::setChgValue(int value)
{
    chgValue = value;
}

/* 隐藏调整按钮 */
void myValueBar::hideBtns()
{
    minusBtn->hide();
    plusBtn->hide();
    minusFastBtn->hide();
    plusFastBtn->hide();
}

/* 显示隐藏按钮 */
void myValueBar::showBtns()
{
    minusBtn->show();
    plusBtn->show();
    minusFastBtn->show();
    plusFastBtn->show();
}

/* 设置是否弹出对话框 */
void myValueBar::setIsPop(bool flag)
{
    m_bIsPop = flag;
}

/* 设置数字显示框显示的位数 */
void myValueBar::setDigitalCount(int num)
{
    lcdNum->setDigitCount(num);
}

/* 设置显示数字的尺寸 */
void myValueBar::setLcdNumSize(QSize size)
{
    lcdNum->setFixedSize(size);
}

void myValueBar::setLcdNumPressEnable(bool flag)
{
    if (!flag) {
        disconnect(lcdNum, SIGNAL(pressed()), this, SLOT(onLcdNumberPressed()));
    } else {
        connect(lcdNum, SIGNAL(pressed()), this, SLOT(onLcdNumberPressed()));
    }

}

/* 设置显示的值可否改变 */
void myValueBar::setEnableChanged(bool flag)
{
    if(flag) {
        m_bEnableChanged = true;
        plusBtn->setEnabled(true);
        minusBtn->setEnabled(true);
        plusFastBtn->setEnabled(true);
        minusFastBtn->setEnabled(true);
    } else {
        m_bEnableChanged = false;
        plusBtn->setEnabled(false);
        minusBtn->setEnabled(false);
        plusFastBtn->setEnabled(false);
        minusFastBtn->setEnabled(false);
    }
}

/* 根据最大值设置lcdNum显示数字的位数 */
void myValueBar::setDigitalCountByMax()
{
    if(maxValue < 10)
        lcdNum->setDigitCount(1);
    else if(maxValue < 100)
        lcdNum->setDigitCount(2);
    else if(maxValue < 1000)
        lcdNum->setDigitCount(3);
    else {
        lcdNum->setDigitCount(4);
    }
}

/* 响应加速减少按钮 */
void myValueBar::onMinusFastBtnClicked()
{
    double tmp = lcdNum->value();
    if(tmp >= minValue+chgFastValue)
        lcdNum->display(tmp-chgFastValue);
    else
        lcdNum->display(minValue);

    emit valueChanged(lcdNum->value());
}

/* 响应减少按钮 */
void myValueBar::onMinusBtnClicked()
{
    double tmp = lcdNum->value();
    if (style == 0) {
        if(tmp >= minValue+chgValue){
            tmp -= chgValue;
            if(tmp > -0.000001 && tmp < 0.000001) {
                lcdNum->display(0);
            } else {
                lcdNum->display(tmp);
            }
        }
    } else if (style == 1){
        for (int i = 0; i < arrEleTotal; i++) {
            if ((tmp - tmpArr[i]) < 0.000001 && (tmp - tmpArr[i]) > -0.000001)  {
                currentArrSuffix = i;
                break;
            }
        }
        if (currentArrSuffix > 0) {
            lcdNum->display(tmpArr[--currentArrSuffix]);
        }
    }
    emit valueChanged(lcdNum->value());
}

/* 响应增加按钮 */
void myValueBar::onPlusBtnClicked()
{
    double tmp = lcdNum->value();
    if (style == 0) {
        if(tmp <= maxValue-chgValue){
            tmp += chgValue;
            if(tmp > -0.000001 && tmp < 0.000001) {
                lcdNum->display(0);
            } else {
                lcdNum->display(tmp);
            }
        }
    } else if (style == 1){
        for (int i = 0; i < arrEleTotal; i++) {
            if ((tmp - tmpArr[i]) < 0.000001 && (tmp - tmpArr[i]) > -0.000001)  {
                currentArrSuffix = i;
                break;
            }
        }
        if (currentArrSuffix < arrEleTotal - 1) {
            lcdNum->display(tmpArr[++currentArrSuffix]);
        }
    }
    emit valueChanged(lcdNum->value());
}

/* 响应加速增加按钮 */
void myValueBar::onPlusFastBtnClicked()
{
    double tmp = lcdNum->value();
    if(tmp <= maxValue-chgFastValue)
        lcdNum->display(tmp+chgFastValue);
    else
        lcdNum->display(maxValue);

    emit valueChanged(lcdNum->value());
}

/* 响应LcdNumber被按下的事件 */
void myValueBar::onLcdNumberPressed()
{
    if(!m_bEnableChanged)
        return;

    if(m_bIsPop) {
        int tmp = lcdNum->value();
        myInputPanel inputDlg(intType, minValue, maxValue, tmp);
        int ret = inputDlg.exec();
        if(ret == QDialog::Accepted){
            lcdNum->display(inputDlg.getValue());
            emit valueChanged(lcdNum->value());
        }
    } else {
        emit lcdNumPressed();
    }
}
