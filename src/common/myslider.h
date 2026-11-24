/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myslider.h
 * \brief       滑块控件头文件
 * \date        2026.11.21
 */
#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>

/*!
 * \brief 滑块控件类
 */
class mySlider : public QSlider
{
public:
   explicit mySlider();
   explicit mySlider(Qt::Orientation orientation, int min, int max, int disp, QWidget * parent = 0);

private:
    void setSliderStyle(void);
};

#endif // MYSLIDER_H
