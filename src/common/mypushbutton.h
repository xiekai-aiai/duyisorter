/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mypushbutton.h
 * \brief       按钮控件头文件
 * \date        2015.01.14
 */
#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include "globalconfig.h"

enum purviewType{
    userOpertor,
    userEngineer,
    userFactory
};

#define DEF         0
#define RED         1
#define GREEN       2

/*!
 * \brief 按钮控件类
 */
class myPushButton : public QPushButton
{
    //Q_OBJECT
public:
    explicit myPushButton(QString text, QIcon icon, bool enable, QWidget *parent = 0);
    explicit myPushButton(QString text, QIcon icon, QWidget *parent = 0);
    explicit myPushButton(QIcon icon, QSize size, QWidget *parent = 0);
    explicit myPushButton(QString text, QIcon icon, bool fixedSize, bool enable, QWidget *parent = 0);

    void setRedColor(int color);
    void setBasicStyle(void);
    void setButtonFont(QFont font);

signals:
    
public slots:

private:
    purviewType userType;
    g_Config *config;
    QIcon m_icon;
    QString m_sText;
    int m_nColor;
};

#endif // MYPUSHBUTTON_H
