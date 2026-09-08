/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylistwidget.h
 * \brief       列表项控件头文件
 * \date        2015.01.14
 */
#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>
#include "globalconfig.h"

/*!
 * \brief 列表项控件类
 */
class myListWidget : public QListWidget
{
public:
    explicit myListWidget();
    explicit myListWidget(QSize size, QWidget *parent = 0);
private:
    g_Config *config;
};

#endif // MYLISTWIDGET_H
