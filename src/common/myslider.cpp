/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myslider.cpp
 * \brief       滑块控件源文件
 * \date        2015.01.14
 */
#include "myslider.h"

mySlider::mySlider()
{
    setSliderStyle();
}

mySlider::mySlider(Qt::Orientation orientation, int min, int max, int disp, QWidget * parent ):
    QSlider(parent)
{
    setOrientation(orientation);
    setRange(min,max);
    setValue(disp);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setSliderStyle();
}

/*!
 * \brief mySlider
 */
void mySlider::setSliderStyle()
{
    setStyleSheet("QSlider::groove:horizontal {"
                  "border: 1px solid #999999;"
                  "height: 45px;"
                  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
                  "margin: 2px 0;}"

                  "QSlider::handle:horizontal {"
                  "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
                  "border: 1px solid #5c5c5c;"
                  "width: 45px;"
                  "margin: -2px 0;"
                  "border-radius: 3px; }");
}
