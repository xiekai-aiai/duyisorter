/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mypushbutton.cpp
 * \brief       按钮控件源文件
 * \date        2015.01.14
 */
#include "mypushbutton.h"

myPushButton::myPushButton(QString text, QIcon icon, bool enable, QWidget *parent) :
    QPushButton(parent)
{
    config = g_Config::getInstance();
    setText(text);
    setFont(config->getFont(DEFAULT_FONT_SIZE));
    setIcon(icon);
    setIconSize(QSize(ICON_WID, ICON_HEI));
    setEnabled(enable);
    m_icon = icon;
    m_sText = text;
    m_nColor = DEF;

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);

    setBasicStyle();
}

myPushButton::myPushButton(QString text, QIcon icon, QWidget *parent) :
    QPushButton(parent)
{
    config = g_Config::getInstance();
    setText(text);
    setFont(config->getFont(DEFAULT_FONT_SIZE));
    setIcon(icon);
    setIconSize(QSize(ICON_WID, ICON_HEI));
    m_icon = icon;
    m_sText = text;
    m_nColor = DEF;

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);

    setBasicStyle();
}

myPushButton::myPushButton(QIcon icon, QSize size, QWidget *parent) :
    QPushButton(parent)
{
    setIcon(icon);
    setFixedSize(size);
    setIconSize(QSize(ICON_WID, ICON_HEI));
    m_icon = icon;
    m_sText = "";
    m_nColor = DEF;

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);

    setBasicStyle();
}

myPushButton::myPushButton(QString text, QIcon icon, bool fixedSize, bool enable, QWidget *parent) :
    QPushButton(parent)
{
    config = g_Config::getInstance();
    setText(text);
    setFont(config->getFont(DEFAULT_FONT_SIZE));
    setIcon(icon);
    setIconSize(QSize(ICON_WID, ICON_HEI));
    setEnabled(enable);
    m_icon = icon;
    m_sText = text;
    m_nColor = DEF;

    if(fixedSize) {
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setSizePolicy(sizePolicy);
    }

    setBasicStyle();
}

void myPushButton::setRedColor(int color)
{
    m_nColor = color;

    setBasicStyle();
}

void myPushButton::setButtonFont(QFont font){
    setFont(font);
}

/*!
 * \brief myPushButton::setBasicStyle
 */
void myPushButton::setBasicStyle()
{
    QString strAlign, strColor;

    if (m_sText == "" || m_icon.isNull()) {
        strAlign = QString("center");
    } else {
        strAlign = QString("left");
    }

    switch (m_nColor) {
    case RED:   //! 美亚红
        strColor = QString("deepskyblue");
        break;
    case GREEN: //! 美亚绿
        strColor = QString("deeppink");
        break;
    case DEF:
        strColor = QString("lightcyan");
        break;
    default:
        strColor = QString("lightcyan");
        break;
    }

    QString strStyle = QString("QPushButton {"
                               "text-align: %1;"
                               "outline: none;"
                               "background-color: %2;"
                               "border-color: #afafaf;"
                               "border-width: 2px;"
                               "border-style: solid;"
                               "border-radius: 5;"
                               "padding: 2px;}"

                      "QPushButton:pressed {"
                               "padding-left: 5px;"
                               "padding-top: 5px;"
                               "background-color: lightgrey;}").arg(strAlign).arg(strColor);

    setStyleSheet(strStyle);
}
