/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycustomcheckbox.cpp
 * \brief       通用选择框控件源文件
 * \date        2026.11.21
 */
#include "mycustomcheckbox.h"

myCustomCheckBox::myCustomCheckBox(QWidget *parent) :
    QWidget(parent)
{
    m_nStyle = CB_STYLE_APPLY;
}

myCustomCheckBox::myCustomCheckBox(QString text, bool isChecked, int nStyle, QWidget *parent) :
    QWidget(parent)
{
    bFlagCheck = isChecked;
    m_nStyle = nStyle;

    toolBtn = new myPushButton("", QIcon());
    toolBtn->setFixedHeight(SMALL_BTN_WIDTH-10);
    toolBtn->setFixedWidth(BTN_HEIGHT);
    setCheckBoxStyle();

    textLabel = new myLabel(text, this);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->addWidget(toolBtn);
    lay->addWidget(textLabel);
    lay->setContentsMargins(0, 0, 0, 0);

    connect(toolBtn, SIGNAL(pressed()), this, SLOT(onToolBtnClickedSlt()));
}

void myCustomCheckBox::onToolBtnClickedSlt()
{
   bFlagCheck = !bFlagCheck;
   setCheckBoxStyle();

   emit pressed();
}

bool myCustomCheckBox::getChecked()
{
    return bFlagCheck;
}

void myCustomCheckBox::setChecked(bool isChecked)
{
    bFlagCheck = isChecked;
    setCheckBoxStyle();
}

void myCustomCheckBox::setCheckBoxName(QString text)
{
    textLabel->setText(text);
}

void myCustomCheckBox::setCheckBoxStyle()
{
    /* 统一使用科技风方形勾选贴图（常态/悬停/按下/选中/禁用） */
    toolBtn->setTechCheckBoxStyle(bFlagCheck);
}
