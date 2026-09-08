/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycustomcheckbox.cpp
 * \brief       通用选择框控件源文件
 * \date        2015.01.14
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
    if (bFlagCheck) {
        if (m_nStyle == CB_STYLE_APPLY) {
            toolBtn->setIcon(myIcon.Action_Select);
        } else if (m_nStyle == CB_STYLE_CIRCLE) {
            toolBtn->setIcon(myIcon.Action_Circle);
        }
    } else {
        toolBtn->setIcon(QIcon());
    }
}
