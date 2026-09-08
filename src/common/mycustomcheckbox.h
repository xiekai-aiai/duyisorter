/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycustomcheckbox.h
 * \brief       通用选择框控件头文件
 * \date        2015.01.14
 */
#ifndef MYCUSTOMCHECKBOX_H
#define MYCUSTOMCHECKBOX_H

#include <QWidget>
#include <QHBoxLayout>
#include "mypushbutton.h"
#include "mylabel.h"
#include "myicon.h"

/*!
 * \enum 枚举选择框样式
 */
enum {
    CB_STYLE_APPLY = 0,
    CB_STYLE_CIRCLE
};

/*!
 * \brief 通用选择框控件类
 */
class myCustomCheckBox : public QWidget
{
    Q_OBJECT

public:
    explicit myCustomCheckBox(QWidget *parent = 0);
    explicit myCustomCheckBox(QString text, bool isChecked, int nStyle = CB_STYLE_APPLY, QWidget *parent = 0);
    bool getChecked(void);
    void setChecked(bool isChecked);
    void setCheckBoxName(QString text);
    void setCheckBoxStyle(void);
    
signals:
    void pressed();

public slots:
    void onToolBtnClickedSlt(void);

private:
    myPushButton *toolBtn;
    myLabel      *textLabel;
    bool         bFlagCheck;
    int m_nStyle;
};

#endif // MYCUSTOMCHECKBOX_H
