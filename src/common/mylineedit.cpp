/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylineedit.h
 * \brief       线状编辑框控件源文件
 * \date        2015.01.14
 */
#include "mylineedit.h"

myLineEdit::myLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    m_bPressEn = true;
    config = g_Config::getInstance();
    setFont(config->getFont());
    setBasicStyle();
}

myLineEdit::myLineEdit(QString text, QWidget *parent):
    QLineEdit(parent)
{
    m_bPressEn = true;
    config = g_Config::getInstance();
    setFont(config->getFont());
    setText(text);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setBasicStyle();
}

myLineEdit::myLineEdit(EchoMode mode, QWidget *parent):
    QLineEdit(parent)
{
    m_bPressEn = true;
    config = g_Config::getInstance();
    setFont(config->getFont());
    setEchoMode(mode);
    setMaxLength(10);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setBasicStyle();
}

void myLineEdit::setPressEnabled(bool bEn)
{
    m_bPressEn = bEn;
}

void myLineEdit::mousePressEvent(QMouseEvent *event)
{
    if (m_bPressEn) {
        emit pressed();
        QLineEdit::mousePressEvent(event);
    }
}

void myLineEdit::setBasicStyle(){
    QString strStyle = QString("QLineEdit  {"
                               "border: 2px solid #1E90FF; "
                               "border-radius: 15px; "
                               "padding: 2px 4px; "
                               "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 white, stop: 1 #F0F8FF);}");

    setStyleSheet(strStyle);
}
