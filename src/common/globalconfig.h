/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        globalconfig.h
 * \brief       全局配置类头文件
 * \date        2015.01.14
 */
#ifndef G_CONFIG_H
#define G_CONFIG_H

#include <QtCore>
#include <QtGui>
#include "CSingletonBase.h"

#define BACKLIGHT_TIMEOUT   300		// 背光间隔：300 Sec
#define ICON_WIDTH          32
#define ICON_HEIGHT         32
#if(LCD_WIDTH == 1024)
    #define BTN_HEIGHT          40
    #define BTN_WIDTH           150
    #define SMALL_BTN_WIDTH     50
    #define DEFAULT_BTN_WIDTH   120
    #define BIG_BTN_WIDTH       250
    #define LCD_TITLE_HEIGHT    50
    #define LCD_CONTENT_HEIGTH  668
    #define LCD_STATUS_HEIGHT   60
    #define CURVE_WIDGET_HEIGHT 420
    #define CURVE_WIDGET_WIDTH	850
    #define TAB_HEIGHT          45
    #define PAGE_BTN_WIDTH      350
    #define PAGE_BTN_HEIGHT     60
    #define ICON_WID            40
    #define ICON_HEI            40
#else
    #define BTN_HEIGHT          40
    #define BTN_WIDTH           135
    #define SMALL_BTN_WIDTH     40
    #define DEFAULT_BTN_WIDTH   100
    #define BIG_BTN_WIDTH       200
    #define LCD_TITLE_HEIGHT    45
    #define LCD_CONTENT_HEIGTH  390
    #define LCD_STATUS_HEIGHT   45
    #define CURVE_WIDGET_HEIGHT 310
    #define CURVE_WIDGET_WIDTH	500
    #define TAB_HEIGHT          40
    #define PAGE_BTN_WIDTH      250
    #define PAGE_BTN_HEIGHT     45
    #define ICON_WID            32
    #define ICON_HEI            32
#endif

// 日志相关枚举值
enum {
    LOG_ALL,
    LOG_POWER,
    LOG_FEED_SWITCH,
//    LOG_MACHINE,
    LOG_PROFILE,
    LOG_CAMERA,
    LOG_BACKGROUD,
    LOG_ARITH,
    LOG_EJECT,
    LOG_WIPE,
    LOG_FEED,
    LOG_DEBUG,
    LOG_WARM
};

enum{
    DEFAULT_BTN_SIZE,
    BIG_BTN_SIZE,
    SMALL_BTN_SIZE
};

enum{
    DEFAULT_FONT_SIZE,
    FONT_SIZE_16,
    FONT_SIZE_12
};

const QString g_style1 = "background-color: rgb(220,220,220);";
const QString g_style2 = "background-color: rgb(190,190,190);";
const QString g_style3 = "background-color: rgb(150,150,150);";
const QString g_styleRed = "background-color: rgb(255,0,0);";
const QString g_styleGreen = "background-color: rgb(0,255,0);";
const QString g_styleBlue = "background-color: rgb(0,0,255);";
const QString g_styleYellow = "background-color: rgb(255,255,0);";

/*!
 * \brief 全局配置类
 */
class g_Config : public CSingletonBase<g_Config>
{
public:
    g_Config();

    void setBtnSize(int nFlagBtn, int w, int h);        //! 设置按钮尺寸
    void setFont(int nFlagFont, int size);              //! 设置字体尺寸
    QFont getFont(int nFlagFont = DEFAULT_FONT_SIZE);   //! 获取尺寸尺寸
    QSize getBtnSize(int nFlagBtn);                     //! 获取按钮尺寸

private:
    QSize m_sizeNormalBtn;
    QSize m_sizeBigBtn;
    QSize m_sizeSmallBtn;
    QFont m_sizeNormalFont;
    QFont m_font16;
    QFont m_font12;
    friend class CSingletonBase<g_Config>;
};

#endif // G_CONFIG_H
