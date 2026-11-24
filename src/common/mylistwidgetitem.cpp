/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylistwidgetitem.cpp
 * \brief       列表项控件源文件
 * \date        2026.11.21
 */
#include "mylistwidgetitem.h"

myListWidgetItem::myListWidgetItem()
{
}

myListWidgetItem::myListWidgetItem(QString text, QIcon icon, QSize size):
    QListWidgetItem()
{
    config = g_Config::getInstance();
    setFont(config->getFont());
    setText(text);
    setIcon(icon);
    setSizeHint(size);
}
