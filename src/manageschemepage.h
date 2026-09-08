/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        manageschemepage.h
 * \brief       方案管理界面头文件
 *              子页面包括新建方案、更改方案模式及复制重要参数。
 * \date        2015.01.14
 */
#ifndef MANAGESCHEMEPAGE_H
#define MANAGESCHEMEPAGE_H

#include <QWidget>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputmethod.h"
#include "common/myinputpanel.h"

#include "globalparams.h"
#include "mythread.h"
#include "globalflow.h"

/*!
 * \brief 方案管理界面类
 */
class ManageSchemePage : public QWidget
{
    Q_OBJECT
public:
    explicit ManageSchemePage(QWidget *parent = 0);
    void resetProfileMode(void);            /*!< 重置方案模式 */
    void refreshBtnStauts(void);
    
signals:
    void goToHomePageSig();                 /*!< go back mainwidget */
    void goToNewSchemePageSig();            /*!< go to page that create new scheme */
    void goToChangeModePageSig(int);
    void goToCopyMajorPageSig();

public slots:
    void onNewSchemeBtnClickedSlt();
    void onCopySchemeBtnClickedSlt();
    void onDeleteSchemeBtnClickedSlt();
    void onRenameSchemeBtnClickedSlt();
    void onImportSchemeBtnClickedSlt();
    void onExportSchemeBtnClickedSlt();
    void onChangeModeBtnPressed(void);
    void onCopyMajorBtnPressed(void);
    void onCancelBtnClickedSlt();
    void onOkBtnClickedSlt();
    void updateListWidget();                   /*!< update listwidget status */

private:    
    myListWidget *m_listWidget;                /*!< scheme listWidget */

    myPushButton *m_newSchemeBtn;              /*!< create new scheme button */
    myPushButton *m_copySchemeBtn;             /*!< copy   scheme button */
    myPushButton *m_deleteSchemeBtn;           /*!< delete scheme button */
    myPushButton *m_renameSchemeBtn;           /*!< rename scheme button */
    myPushButton *m_importSchemeBtn;           /*!< import/backup scheme button */
    myPushButton *m_exportSchemeBtn;           /*!< export/restore scheme button */
    myPushButton *m_okBtn;                     /*!< confirm button */
    myPushButton *m_cancelBtn;                 /*!< cancel button */
    myPushButton *m_changeModeBtn;             /*!< 更改方案模式按钮 */
    myPushButton *m_copyMajorBtn;              /*!< 复制重要参数按钮 */

    QGridLayout *m_schemePageGridLayout;      /*!< page's main layout */
    QVBoxLayout *m_schemePageVBLayout;        /*!< page's sub layout, layout for operating scheme */
    QHBoxLayout *m_schemePageHBLayout;        /*!< page's sub layout, layout for ok and cancel button */
    QSpacerItem *m_horizontalSpacer;          /*!< horizontal spacer, used for schemePageHBLayout */
};

#endif // MANAGESCHEMEPAGE_H
