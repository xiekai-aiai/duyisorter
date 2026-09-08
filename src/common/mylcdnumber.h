/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylcdnumber.h
 * \brief       数字显示控件头文件
 * \date        2015.01.14
 */
#ifndef MYLCDNUMBER_H
#define MYLCDNUMBER_H

#include <QLCDNumber>

/*!
 * \brief 数字显示控件类
 */
class myLCDNumber : public QLCDNumber
{
    Q_OBJECT

public:
   explicit myLCDNumber();
   explicit myLCDNumber(int count, float disp, QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *);

signals:
    void pressed();
};

#endif // MYLCDNUMBER_H
