/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mypushbutton.cpp
 * \brief       按钮控件源文件
 * \date        2015.01.14
 */
#include "mypushbutton.h"
#include <QResizeEvent>

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

    /* 纯图标按钮不留内边距，图标可撑满内容区 */
    QString strPadding = m_sText.isEmpty() ? QString("0px") : QString("2px");

    /* 常规尺寸按钮套用点击动效贴图；小按钮/纯图标按钮保留轻量样式，
       避免 9-slice 的上下 8px、左右 16px 边距吃掉内容区 */
    m_bSkin = (width() >= 90 && height() >= 36);

    if (m_bSkin) {
        /* GREEN 表示激活/选中，直接用按下态贴图；RED 表示告警，仅文字变色 */
        QString strNormalImage = (m_nColor == GREEN)
                ? QString(":/res/png2/button/btn_pressed.png")
                : QString(":/res/png2/button/btn_normal.png");

        switch (m_nColor) {
        case RED:
            strColor = QString("#ff7a8a");
            break;
        case GREEN:
            strColor = QString("#ffffff");
            break;
        default:
            strColor = QString("#e8f6ff");
            break;
        }
        
        QString strStyle = QString("QPushButton {"
                                   "text-align: %1;"
                                   "outline: none;"
                                   "border-width: 8px 16px 8px 16px;"
                                   "border-style: solid;"
                                   "border-color: transparent;"
                                   "border-image: url(%2) 8 16 8 16 stretch stretch;"
                                   "padding: 0px;"
                                   "color: %3;}"
                          "QPushButton:hover {"
                                   "border-image: url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch;"
                                   "color: #ffffff;}"
                          "QPushButton:pressed {"
                                   "border-image: url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch;"
                                   "color: #ffffff;}"
                          "QPushButton:checked {"
                                   "border-image: url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch;"
                                   "color: #ffffff;}"
                          "QPushButton:disabled {"
                                   "border-image: url(:/res/png2/button/btn_disabled.png) 8 16 8 16 stretch stretch;"
                                   "color: #5a7686;}").arg(strAlign).arg(strNormalImage).arg(strColor);

        m_ownStyle = strStyle;
        setStyleSheet(strStyle);
        return;
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
                               "padding: %3;"
                               "color: black;}"

                      "QPushButton:pressed {"
                               "padding-left: 5px;"
                               "padding-top: 5px;"
                               "background-color: lightgrey;"
                               "color: black;}"
                      "QPushButton:checked {"
                               "background-color: lightgrey;"
                               "color: black;}").arg(strAlign).arg(strColor).arg(strPadding);

    m_ownStyle = strStyle;
    setStyleSheet(strStyle);
}

/*!
 * \brief 尺寸变化时，纯图标按钮的图标撑满内容区
 */
void myPushButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);

    if (text().isEmpty()) {
        /* 只对轻量样式的图标按钮撑满，贴图按钮的 9-slice 边框需保留 */
        if (!m_bSkin) {
            setIconSize(QSize(width() - 4, height() - 4));
        }
    }

    /* 按钮尺寸通常在构造后才由 setFixedSize/布局确定，跨过阈值时补刷一次样式；
       若外部已用 setStyleSheet 定制过，则不再接管，避免覆盖调用方的样式 */
    const bool bSkin = (width() >= 90 && height() >= 36);
    if (bSkin != m_bSkin && styleSheet() == m_ownStyle) {
        setBasicStyle();
    }
}
