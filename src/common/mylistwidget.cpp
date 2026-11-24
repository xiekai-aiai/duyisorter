/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylistwidget.cpp
 * \brief       列表控件源文件
 * \date        2026.11.21
 */
#include "mylistwidget.h"

myListWidget::myListWidget()
{
    config = g_Config::getInstance();
    setFont(config->getFont());
}

myListWidget::myListWidget(QSize size, QWidget *parent):
    QListWidget(parent)
{
    config = g_Config::getInstance();
    setFont(config->getFont());
    setIconSize(size);
}
