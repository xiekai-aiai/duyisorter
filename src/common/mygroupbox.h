/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mygroupbox.h
 * \brief       组框控件头文件
 * \date        2015.01.14
 */
#ifndef MYGROUPBOX_H
#define MYGROUPBOX_H

#include <QGroupBox>
#include "globalconfig.h"

/*!
 * \brief 组框控件类
 */
class myGroupBox : public QGroupBox
{
public:
    explicit myGroupBox();
    explicit myGroupBox(QString text, QWidget *parent = 0);

private:
    g_Config *config;
};

#endif // MYGROUPBOX_H
