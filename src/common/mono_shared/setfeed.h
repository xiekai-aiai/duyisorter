/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setfeed.h
 * \brief       黑白机型设置供料量界面头文件。
 * \date        2015.01.14
 */
#ifndef SETFEED_H
#define SETFEED_H
#include <QApplication>
#include <QWidget>
#include <QSignalMapper>
#include <QStackedWidget>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "globalparams.h"
#include "globalflow.h"
#include "feedctrl.h"
#include "common/mono_shared/sortparams/paramsetalone.h"

/*!
 * \brief 黑白机型设置供料量界面类
 */
class setFeedPage : public QWidget
{
    Q_OBJECT
public:
    explicit setFeedPage(QWidget *parent = 0);
    void createMainPage(void);                          // 创建主页面
    void CreateFeedSeparateWidget(void);                // 创建各供料器供料量独立设置页面
    void CreatFeedMainWidget(void);                     // 创建供料设置主页面
    void CreateFeedEnableWidget(void);                  // 创建供料使能设置页面
    void getFeedGroupIndex(void);                       // 获取供料组起始结束信息
    void getFeedValue(int indexOfFeeder,int feedValue); // 获取当前组各供料器值
    void updatePage();                                  // 更新当前显示页面
    void updateSeparatePageValue();                     // 更新各供料器供料量值
    void updateMainPageValue();                         // 更新一次、二次、三次供料量值
    void updateFeedEnableState();                       // 更新使能页面显示状态
    void updateEnableAllBtnState();                     // 更新全部开关按钮状态

signals:
    void goToHomePageSig();             // 返回主界面
    void hideOneKeyFeedSig();
    void showOneKeyFeedSig();

private slots:
    void onfeedEnableAllBtnClickedSlt(void);
    void onfeedDisableAllBtnClickedSlt(void);
    void onFeedEnableBtnClickedSlt(void);
    void feederRSValueChange(int indexOfFeeder);
    void feederEnableChange(int indexOfFeeder);
    void feederSeparateRSValueChange(int indexOfFeeder);
    void onFeedCtrlBtnPressed(void);    // 进入料位监控页面
    void onFeedSetBtnPressed(void);     // 进入供料组合选择页面
    void onBackBtnPressed(void);        // 点击返回按钮
    void onStakedWidgetIndexChangedSlt(int nIndex);     // 切换页索引变化
    void onWaitTimePressed(void);       // 检测等待时间变化

private:
    enum{
        FEED_SEPARATE_PAGE,     // 供料量独立设置页面
        FEED_MAIN_PAGE,         // 供料设置主页面
        FEED_ENABLE_PAGE,       // 供料使能设置页面
        FEED_CTRL_PAGE,         // 料位监控页面
        FEED_SET_PAGE           // 供料器组合页面
    };

    QStackedWidget  *stackedWidget;
    QWidget         *feedSeparatePage;
    QWidget         *feedMainPage;
    QWidget         *feedEnablePage;
    FeedCtrl        *m_feedCtrlPage;
    paramSetAlone   *m_feedSetPage;

    // 设置供料控件
    myPushButton    *feedEnableBtn;
    myPushButton    *feedEnableAllBtn;
    myPushButton    *feedDisableAllBtn;
    QSignalMapper   *feedEnableMapper;
    QSignalMapper   *feederSignalMapper;
    QSignalMapper   *feederSeparateSignalMapper;
    myLabel         *feedNameRS[4];
    myLineEdit      *feedLbeRS[4];
    myLabel         *feedNameSeparateRS[12];
    myLineEdit      *feedLbeSeparateRS[12];
    myPushButton    *m_feedCtrlBtn;
    myPushButton    *m_feedSetBtn;
    myPushButton    *m_backBtn;
    myLabel         *m_waitTimeLbl;
    myLCDNumber     *m_waitTimeNum;

    QSignalMapper           *sigBoxMapper;
    QVector<MyCheckBox *>   feedEnableBox1;

    int  feedStart[4];          // 供料组起始编号
    int  feedEnd[4];            // 供料组结束编号
    int m_nPreIndex;            // 记录切换至料位监控页面前的索引号
};

#endif // SETFEED_H
