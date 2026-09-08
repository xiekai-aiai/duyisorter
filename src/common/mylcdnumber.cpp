/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylcdnumber.cpp
 * \brief       数字显示控件源文件
 * \date        2015.01.14
 */
#include "mylcdnumber.h"

myLCDNumber::myLCDNumber()
{
}

myLCDNumber::myLCDNumber(int count, float disp, QWidget *parent):
    QLCDNumber(parent)
{
    setDigitCount(count);
    display(disp);
}

void myLCDNumber::mousePressEvent(QMouseEvent *)
{
    emit pressed();
}
