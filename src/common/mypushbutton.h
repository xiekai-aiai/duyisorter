/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mypushbutton.h
 * \brief       按钮控件头文件
 * \date        2026.11.21
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
    void setModeToggleStyle(void);   //! 模式切换按钮套用单选高亮贴图外观（不改逻辑）
    void setTechCheckBoxStyle(bool checked); //! 科技风复选框外观（常态/悬停/按下/选中/禁用）
    void setTechAdjustStyle(bool plus);      //! 科技风加/减按钮外观（常态/按下）
    void setTechFineAdjustStyle(int direction);
    void setTechPausePlayStyle(bool playing); //! 科技风细调箭头外观（0=左双 1=左单 2=右单 3=右双）
    void setButtonFont(QFont font);

signals:
    
public slots:

protected:
    /* 纯图标按钮在尺寸变化时让图标撑满内容区 */
    void resizeEvent(QResizeEvent *event) override;

private:
    purviewType userType;
    g_Config *config;
    QIcon m_icon;
    QString m_sText;
    int m_nColor;
    bool m_bSkin = false;   //! 当前是否套用点击动效贴图样式
    QString m_ownStyle;     //! 本类最近一次设置的样式串，用于识别外部覆盖
};

#endif // MYPUSHBUTTON_H
