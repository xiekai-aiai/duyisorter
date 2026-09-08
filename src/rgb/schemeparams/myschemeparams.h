/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myschemeparams.h
 * \brief       彩色机型方案参数界面头文件
 *              子页面包括识别参数、剔除参数、吹气时间、背景设置、灯光控制及智能供料。
 * \date        2015.01.14
 */
#ifndef MY_SCHEME_PARAMETERS_H
#define MY_SCHEME_PARAMETERS_H

#include <QWidget>
#include "functions.h"
#include "arith.h"
#include "background.h"
#include "reserved.h"
#include "intel.h"
#include "ai.h"
#include "schemeparams.h"

/*!
 * \brief 彩色机型方案参数界面头文件
 */
class MySchemeParams : public QWidget
{
    Q_OBJECT

    enum {
        Page_ID_Params,
//        Page_ID_Background,
        Page_ID_Arith,
        Page_ID_Reserved,
        Page_ID_Intel,
        Page_ID_Ai
    };

public:
    MySchemeParams(QWidget *parent = 0);
    void setSchemeParams(int pageId);
    void resetSchemeParams(int pageId);

signals:
    void pageUpdated(int pageId);
    void pageChanged(int pageId);
    void pageIndexChanged(int index);
    void goToHomePage();
    void goToAIMainWidgetSig(int index);
    void shutDownBg();

public slots:
    void onPageChanged(int pageId);
    void onCurrentChanged(int index);
    void onOkBtnClicked();
    void onCancelBtnClicked();
    void refreshSchemeParams();
    void shutdownCurveBackGround();


private:
    void displayPageButton(int index);

private:
    QStackedWidget  *pagesWidget;
    QPushButton     *okButton;
    QPushButton     *cancelButton;

    PageParams      *pageParams;
//    PageBackground  *pageBackground;
    PageArith       *pageArith;
    PageReserved    *pageReserved;
    PageIntel       *pageIntel;
    PageAi          *pageAi;

    g_Config *config;
};

#endif
