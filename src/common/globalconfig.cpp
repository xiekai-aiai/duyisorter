/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        globalconfig.cpp
 * \brief       全局配置类源文件
 * \date        2015.01.14
 */
#include "globalconfig.h"

g_Config::g_Config()
{
    setBtnSize(BIG_BTN_SIZE, BIG_BTN_WIDTH, BTN_HEIGHT);
    setBtnSize(DEFAULT_BTN_SIZE, DEFAULT_BTN_WIDTH, BTN_HEIGHT);

    if(LCD_WIDTH == 1024) {
        setBtnSize(SMALL_BTN_SIZE, SMALL_BTN_WIDTH, BTN_HEIGHT);
    } else {
        setBtnSize(SMALL_BTN_SIZE, SMALL_BTN_WIDTH, BTN_HEIGHT);
    }

    if(LCD_WIDTH == 1024) {
        setFont(DEFAULT_FONT_SIZE, 22);
    } else {
        setFont(DEFAULT_FONT_SIZE, 16);
    }

    setFont(FONT_SIZE_12, 12);
    setFont(FONT_SIZE_16, 16);
}

/*!
 * \brief 设置按钮尺寸
 * \param nFlagBtn
 * \param w
 * \param h
 */
void g_Config::setBtnSize(int nFlagBtn, int w, int h)
{
    switch(nFlagBtn) {
    case BIG_BTN_SIZE:
        m_sizeBigBtn.setWidth(w);
        m_sizeBigBtn.setHeight(h);
        break;
    case DEFAULT_BTN_SIZE:
        m_sizeNormalBtn.setWidth(w);
        m_sizeNormalBtn.setHeight(h);
        break;
    case SMALL_BTN_SIZE:
        m_sizeSmallBtn.setWidth(w);
        m_sizeSmallBtn.setHeight(h);
        break;
    default:
        break;
    }
}

/*!
 * \brief 设置字体尺寸
 * \param nFlagFont
 * \param size
 */
void g_Config::setFont(int nFlagFont, int size)
{
    switch(nFlagFont) {
    case DEFAULT_FONT_SIZE:
        m_sizeNormalFont.setPixelSize(size);
        break;
    case FONT_SIZE_12:
        m_font12.setPixelSize(12);
        break;
    case FONT_SIZE_16:
        m_font16.setPixelSize(16);
        break;
    default:
        break;
    }
}

/*!
 * \brief 获取尺寸尺寸
 * \param nFlagFont
 * \return
 */
QFont g_Config::getFont(int nFlagFont)
{
    switch(nFlagFont) {
    case DEFAULT_FONT_SIZE:
        return m_sizeNormalFont;
    case FONT_SIZE_12:
        return m_font12;
    case FONT_SIZE_16:
        return m_font16;
    default:
        return QFont();
    }
}

/*!
 * \brief 获取按钮尺寸
 * \param nFlagBtn
 * \return
 */
QSize g_Config::getBtnSize(int nFlagBtn)
{
    switch(nFlagBtn){
    case BIG_BTN_SIZE:
        return m_sizeBigBtn;
    case DEFAULT_BTN_SIZE:
        return m_sizeNormalBtn;
    case SMALL_BTN_SIZE:
        return m_sizeSmallBtn;
    default:
        return QSize(0,0);
    }
}
