/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        manageengineer.h
 * \brief       工程模式管理界面头文件
 * \date        2015.01.14
 */
#ifndef MANAGEENGINEER_H
#define MANAGEENGINEER_H

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
#include "mydelaycode.h"

#include "globalparams.h"
#include "globalflow.h"
#include "mydelaycode.h"

/*!
 * \brief 工程模式管理界面类
 */
class manageEngineer : public QWidget
{
    Q_OBJECT
public:
    explicit manageEngineer(QWidget *parent = 0);

    void createManageEngineerPage(void);
    void createSetProductPage(void);    // 创建机型设置界面
    void createSetLanguagePage(void);   // 创建设置语言界面
    void createDelayPaymentPage(void);  // 创建延迟付款界面
    void createSetBeanPage(void);       // 创建豆类定制机型设置界面

    void onTeaProductListChanged(void); // 响应茶叶机型列表变化

signals:
    void  goToHomePageSig();            // 返回主界面
    void  sUpdateSchemeList();          // 更新方案列表
    void  sUpdateArithmeticList();      // 更新算法列表

public slots:
    void onStackedWidgetIndexChangedSlt(int index);
    void onProductBtnClickedSlt();
    void onLanguageBtnClickedSlt();
    void onLimitBtnClickedSlt();
    void onSetBeanBtnClickedSlt();
    void onCustomLanguageBtnClickedSlt();
    void onBackBtnClckedSlt();

    void gotoFirstPageSlt();    // 返回主界面

    /* 机型设置槽函数 */
    void onProductCancelBtnClicked();
    void onPoductSureBtnClicked();
    void onProductListChanged(int index);

    /* 语言设置槽函数 */
    void onLangCancelBtnClicked();
    void onLangSureBtnClicked();
    void onLangAddBtnClicked();
    void onLangDelBtnClicked();

    /* 豆类定制设置槽函数 */
    void onsetBeanCancelBtnClicked();
    void onsetBeanSureBtnClicked();
    void onsetBeanSelectChange(int indexBean);

private:
    /* 标志工程模式界面不同页面的索引号 */
    enum {
        Page_ManageEngineer,//! 工程模式页面
        Page_Product,       //! 机型设置界面
        Page_Language,      //! 设置语言界面
        Page_DelayPayment,  //! 延迟付款界面
        Page_SetBean        //! 设置豆类定制界面
    };

    enum {
        machineTotal = 10,   //! 机型设置界面
        beanTotal = 6       //! 豆类类型
    };

    void updateProduct();
    void updateTeaProduct(void);
    void updateLangList();
    void updateSetBean();

    int margin;
    QStackedWidget *stackedWidget;
    QWidget *manageEngineerWidget;
    g_Config *config;

    /* 工程模式设置界面的控件 */
    myPushButton *productBtn;
    myPushButton *languageBtn;
    myPushButton *limitBtn;
    myPushButton *setBeanBtn;
    myPushButton *customLanguageBtn;
    myPushButton *backBtn;

    QVBoxLayout  *mainVbLayout;
    QGridLayout  *upGrid;
    QHBoxLayout  *upHbLayout;
    QHBoxLayout  *downHbLayout;

    /* 机型设置界面的控件 */
    QWidget *productWidget;
    QListWidget *productList;
    QListWidgetItem *machineItem[machineTotal];
    myPushButton *productSureBtn, *productCancelBtn;

    int nProduct;
    int isBeanEnable;

    /* 语言设置界面的控件 */
    QWidget *langWidget;
    QListWidget *allLangListWidget;
    QListWidget *selLangListWidget;
    QList<int> allLangList;
    QList<int> selLangList;
    /*
    QList<QListWidgetItem *> allLangItem;
    QList<QListWidgetItem *> selLangItem;
    */
    myPushButton *langAddBtn, *langDelBtn;
    myPushButton *langSureBtn, *langCancelBtn;

    /* 延迟付款设置的控件 */
    QWidget *delayPaymentWidget;
    myPushButton *delayPaymentSureBtn, *delayPaymentCancelBtn;

    /* 设置豆类定制机型界面的控件 */
    QWidget *setBeanWidget;
    QGridLayout   *setBeanGrid;
    myCustomCheckBox *setBeanCheckBoxList[beanTotal];
    QSignalMapper *setBeanSignalMapper;

    myPushButton *setBeanSureBtn, *setBeanCancelBtn;
    int beanList[beanTotal+1];

};

#endif // MANAGEENGINEER_H
