/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylabel.cpp
 * \brief       标签控件源文件
 * \date        2015.01.14
 */
#include "mylabel.h"

myLabel::myLabel()
{
    config = g_Config::getInstance();
    setFont(config->getFont());
}

myLabel::myLabel(QString text, QWidget *parent):
    QLabel(parent)
{
    config = g_Config::getInstance();
    setFont(config->getFont());
    setText(text);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
}
