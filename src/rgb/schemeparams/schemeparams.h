#ifndef SCHEMEPARAMSWID_H
#define SCHEMEPARAMSWID_H

#include <QWidget>
#include "functions.h"
#include "arith.h"
#include "arithenable.h"
#include "ejecttime.h"
#include "intel.h"
#include "lamp.h"
#include "tick.h"
#include "common/mono_shared/camerasignal/ejecttimes.h"
#include "background.h"

const int LIST_ITEM_TOTAL = 14;

/*!
 * \brief 彩色机型方案参数界面类
 */
class PageParams: public QWidget
{
    Q_OBJECT

    enum {
        PageParams_Identify,        //! 识别参数
        PageParams_Tick,            //! 剔除参数
        PageParams_EjectTime,       //! 吹气时间
        PageParams_Lamp,            //! 灯光控制
        PageParams_EjectTimes,      //! 吹气统计
        PageParams_Background,      //! 背景设置
        PageParams_Feeder_Adjust,   //! 智能供料
    };

public:
    PageParams(QWidget *parent = 0);
    void setParams();
    void resetParams();
    QList<int> getSelPageParamsList();
    QList<int> getAllPageParamsList();
    QString getPageParamsListName(int index);
    QIcon   getPageParamsListIcon(int index);

signals:
    void pageUpdated(int index);
    void pageChanged(int pageId);
    void sArithmeticChanged(int total);
    void setOkBtnHiddenSig(bool bEn);

public slots:
    void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onCurrentPageChanged(int index);
    void refreshPageParams();
    void shutdownBg();

private:
    void createPageListItem();

private:
    myListWidget      *pageListWidget;    // list widget index
    myListWidgetItem  *navigationListItem[LIST_ITEM_TOTAL];
    QStackedWidget    *pagesWidget;       // stack widget container

    PageIdentify      *pageIdentify;        // PageIdentify
    PageTick          *pageTick;            // PageTick
    PageEjectTime     *pageEjectTime;       // PageEjectTime
    PageLamp          *pageLamp;            // PageLamp
    ejectTimes        *pageEjectTimes;      // 吹气次数统计页面
    PageBackground    *pageBackground;      // 背景设置界面
};

#endif // SCHEMEPARAMSWID_H
