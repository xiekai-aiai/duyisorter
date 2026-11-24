/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mypushbutton.cpp
 * \brief       按钮控件源文件
 * \date        2026.11.21
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
 * \brief 模式切换按钮（前视/后视、上视/下视等）套用与 MyButtonGroup 一致的单选高亮外观，
 *        仅改变外观，不改变点击切换逻辑
 */
void myPushButton::setModeToggleStyle()
{
    m_bSkin = false;

    QString strStyle = QString(
        "QPushButton {"
        "  border-width: 8px 10px 8px 10px;"
        "  border-style: solid;"
        "  border-color: transparent;"
        "  border-image: url(:/res/png2/radio/radio_row_checked.png) 8 10 8 10 stretch stretch;"
        "  color: #ffffff;"
        "  font-size: 14px;"
        "  padding: 0px;"
        "  outline: none;"
        "}"
        "QPushButton:hover {"
        "  border-image: url(:/res/png2/radio/radio_row_hover.png) 8 10 8 10 stretch stretch;"
        "  color: #ffffff;"
        "}"
        "QPushButton:pressed {"
        "  border-image: url(:/res/png2/radio/radio_row_hover.png) 8 10 8 10 stretch stretch;"
        "  color: #ffffff;"
        "}");

    setStyleSheet(strStyle);
    /* 标记为外部定制样式，避免尺寸变化时被 setBasicStyle 覆盖 */
    m_ownStyle = QString();
}

/*!
 * \brief 科技风复选框外观：常态/悬停/按下/选中/禁用 使用 res/png2/checkbox 下的贴图
 * \param checked 是否选中
 */
void myPushButton::setTechCheckBoxStyle(bool checked)
{
    m_bSkin = false;

    const QString path = ":/res/png2/checkbox/";
    QString imgNormal = path + (checked ? "cb_checked.png" : "cb_normal.png");
    QString imgHover = path + (checked ? "cb_checked.png" : "cb_hover.png");

    setIcon(QIcon());
    setStyleSheet(QString(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  padding: 0px;"
        "  image: url(%1);"
        "}"
        "QPushButton:hover { image: url(%2); }"
        "QPushButton:pressed { image: url(%3); }"
        "QPushButton:disabled { image: url(%4); }")
        .arg(imgNormal, imgHover, path + "cb_pressed.png", path + "cb_disabled.png"));

    /* 标记为外部定制样式，避免尺寸变化时被 setBasicStyle 覆盖 */
    m_ownStyle = QString();
}

/*!
 * \brief 科技风加/减按钮外观：使用 res/png2/adjust_btn_tech 下的常态/按下贴图
 * \param plus true=加号，false=减号
 */
void myPushButton::setTechAdjustStyle(bool plus)
{
    m_bSkin = false;

    const QString path = ":/res/png2/adjust_btn_tech/";
    QString imgNormal = path + (plus ? "btnPlus_normal.png" : "btnMinus_normal.png");
    QString imgPressed = path + (plus ? "btnPlus_pressed.png" : "btnMinus_pressed.png");
    QString imgDisabled = path + (plus ? "btnPlus_disabled.png" : "btnMinus_disabled.png");
    
    setIcon(QIcon());
    setStyleSheet(QString(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  padding: 0px;"
        "  border-image: url(%1) 0 0 0 0 stretch stretch;"
        "}"
        "QPushButton:pressed {"
        "  border-image: url(%2) 0 0 0 0 stretch stretch;"
        "}"
        "QPushButton:disabled {"
        "  border-image: url(%3) 0 0 0 0 stretch stretch;"
        "}").arg(imgNormal, imgPressed, imgDisabled));

    m_ownStyle = QString();
}

/*!
 * \brief 科技风细调箭头外观：使用 res/png2/adjust_btn_tech 下的常态/按下贴图
 * \param direction 0=左双箭头 1=左单箭头 2=右单箭头 3=右双箭头
 */
void myPushButton::setTechFineAdjustStyle(int direction)
{
    m_bSkin = false;

    const QString path = ":/res/png2/adjust_btn_tech/";
    QString name;
    switch (direction) {
    case 0:  name = "fineLeftDouble";  break;
    case 1:  name = "fineLeftSingle";  break;
    case 2:  name = "fineRightSingle"; break;
    default: name = "fineRightDouble"; break;
    }

    setIcon(QIcon());
    setStyleSheet(QString(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  padding: 0px;"
        "  border-image: url(%1) 0 0 0 0 stretch stretch;"
        "}"
        "QPushButton:pressed {"
        "  border-image: url(%2) 0 0 0 0 stretch stretch;"
        "}").arg(path + name + "_normal.png", path + name + "_pressed.png"));

    m_ownStyle = QString();
}

/*!
 * \brief 科技风暂停/播放按钮外观：使用 res/png2/pause_play_btn_tech 下的常态/按下贴图
 * \param playing true=播放图标(开始) false=暂停图标
 */
void myPushButton::setTechPausePlayStyle(bool playing)
{
    m_bSkin = false;

    const QString path = ":/res/png2/pause_play_btn_tech/";
    const QString name = playing ? "btnPlay" : "btnPause";

    setIcon(QIcon());
    setStyleSheet(QString(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  padding: 0px;"
        "  border-image: url(%1) 0 0 0 0 stretch stretch;"
        "}"
        "QPushButton:pressed {"
        "  border-image: url(%2) 0 0 0 0 stretch stretch;"
        "}").arg(path + name + "_normal.png", path + name + "_pressed.png"));

    m_ownStyle = QString();
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
