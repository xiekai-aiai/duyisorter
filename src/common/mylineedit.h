/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylineedit.h
 * \brief       线状编辑框控件头文件
 * \date        2015.01.14
 */
#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include "globalconfig.h"

/*!
 * \brief 线状编辑框控件类
 */
class myLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit myLineEdit(QWidget *parent = 0);
    explicit myLineEdit(QString text, QWidget *parent = 0);
    explicit myLineEdit(EchoMode mode, QWidget *parent = 0);
    void setPressEnabled(bool bEn);
    void setBasicStyle();

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void pressed();

public slots:

private:
    bool m_bPressEn;
    g_Config *config;
};

#endif // MYLINEEDIT_H
