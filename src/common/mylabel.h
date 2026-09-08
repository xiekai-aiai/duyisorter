/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylabel.h
 * \brief       标签控件头文件
 * \date        2015.01.14
 */
#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <mymutex.h>

/*!
 * \brief 标签控件类
 */
class myLabel : public QLabel
{
public:
    explicit myLabel();
    explicit myLabel(QString text, QWidget *parent = 0);

private:
    g_Config *config;
};

#endif // MYLABEL_H
