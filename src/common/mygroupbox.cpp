/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mygroupbox.h
 * \brief       组框控件源文件
 * \date        2015.01.14
 */
#include "mygroupbox.h"
#include <QPalette>

myGroupBox::myGroupBox()
{
    QPalette pe = palette();
    pe.setColor(QPalette::WindowText, Qt::white);
    setPalette(pe);
}

myGroupBox::myGroupBox(QString text, QWidget *parent)
   : QGroupBox(parent)
{
    config = g_Config::getInstance();
    setFont(config->getFont());
    setTitle(text);

    QPalette pe = palette();
    pe.setColor(QPalette::WindowText, Qt::white);
    setPalette(pe);
}
