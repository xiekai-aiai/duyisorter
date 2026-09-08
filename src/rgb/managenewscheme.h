/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        managenewscheme.h
 * \brief       彩色机型新建界面头文件
 * \date        2015.01.14
 */
#ifndef MANAGENEWSCHEME_H
#define MANAGENEWSCHEME_H

#include <QWidget>
#include <QStackedWidget>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputpanel.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputmethod.h"
#include "globalparams.h"
#include "globalflow.h"

#define MAX_CATEGORY    32

/*!
 * \brief 彩色机型新建方案类
 */
class ManageNewScheme : public QWidget
{
    Q_OBJECT
public:
    explicit ManageNewScheme(QWidget *parent = 0);

    void getPrefixedProfile(QString fileName);      // 提取预制方案
    
signals:
    void goToManageSchemeSig();

public slots:
    void setCurrentIndex(int);
    void onCancelBtnClickedSlt();
    void onOkBtnClickedSlt();

    void createPrefixedProfile();
    void updatePrefixedProfile();
    void getPrefixedProfileCbxSlt(int index);

private:
    myListWidget                *listWidget;
    QStackedWidget              *stackedWidget;

    QVector<myListWidgetItem *> parentPrefixedProfile;
    QVector<int >               parentPrefixedProfileSeq;
    QVector<QWidget *>          sonPrefixedProfile;
    QVector<QGridLayout *>      sonPrefixedProfileLayout;
    QVector<QSignalMapper *>    sonPrefixedProfileSigMapper;
    myCustomCheckBox            *sonPrefixedProfileCbx[MAX_CATEGORY][MAX_SON_PREFIXED_PROFILE_COUNT];

    QGridLayout     *newSchemePageGridLayout;
    QHBoxLayout     *newSchemePageHBLayout;
    QSpacerItem     *horizontalSpacer;

    myPushButton    *okBtn;
    myPushButton    *cancelBtn;

    bool            bFlagChecked[MAX_CATEGORY][MAX_SON_PREFIXED_PROFILE_COUNT];
    int             currentIndex;
    int             currentCategory;
};

#endif // MANAGENEWSCHEME_H
