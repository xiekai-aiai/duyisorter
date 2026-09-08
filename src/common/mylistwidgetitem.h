/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylistwidgetitem.h
 * \brief       列表项控件头文件
 * \date        2015.01.14
 */
#ifndef MYLISTWIDGETITEM_H
#define MYLISTWIDGETITEM_H

#include <QListWidgetItem>
#include "globalconfig.h"

/*!
 * \brief 列表项控件类
 */
class myListWidgetItem : public QListWidgetItem
{
public:
    explicit myListWidgetItem();
    explicit myListWidgetItem(QString text, QIcon icon, QSize size);

private:
    g_Config *config;
};

#endif // MYLISTWIDGETITEM_H
