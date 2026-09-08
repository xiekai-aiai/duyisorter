#include "schemeparams.h"

/**
 * @brief PageParams::PageParams
 * 方案参数页面构造函数
 * @param parent
 */
PageParams::PageParams(QWidget *parent)
    : QWidget(parent)
{
/* page init */
    /* the stacked widget to hold pages */
    pagesWidget = new QStackedWidget(this);
    pagesWidget->setStyleSheet("background-color:transparent");

    //! 识别参数页面
    pageIdentify = new PageIdentify;
    pagesWidget->insertWidget(PageParams_Identify, pageIdentify);

    //! 剔除参数页面
    pageTick = new PageTick;
    pagesWidget->insertWidget(PageParams_Tick, pageTick);

    //! 吹气时间页面
    pageEjectTime= new PageEjectTime;
    pagesWidget->insertWidget(PageParams_EjectTime, pageEjectTime);

    //! 灯控参数页面
    pageLamp = new PageLamp;
    pagesWidget->insertWidget(PageParams_Lamp, pageLamp);

    QSize size(800, 600);
    //! 吹气次数统计页面
    pageEjectTimes = new ejectTimes(size);
    pagesWidget->insertWidget(PageParams_EjectTimes, pageEjectTimes);
    pageEjectTimes->hideEjectSet();

    //! 背景设置页面
    pageBackground = new PageBackground();
    pagesWidget->insertWidget(PageParams_Background,pageBackground);

    pagesWidget->setMinimumHeight(LCD_HEIGHT-BTN_HEIGHT-210);
    pagesWidget->setFixedWidth(LCD_WIDTH-65);

    //! 方案参数索引列表
    pageListWidget = new myListWidget();
    pageListWidget->setViewMode(QListView::IconMode);
    pageListWidget->setWrapping(false);
    pageListWidget->setFlow(QListView::LeftToRight);
//    pageListWidget->setIconSize(QSize(ICON_WID, ICON_HEI));
    pageListWidget->setMovement(QListView::Static);
    pageListWidget->setStyleSheet("background-color:transparent");

    if (LCD_WIDTH == 1024) {
        pageListWidget->setFixedWidth(LCD_WIDTH-BTN_WIDTH-180);
        pageListWidget->setFixedHeight(40);
        pageListWidget->setSpacing(0);
    } else {
        pageListWidget->setFixedWidth(LCD_WIDTH-BTN_WIDTH-30-25);
//        pageListWidget->setSpacing(2);
    }

    /* add list items to pageListWidget */
    createPageListItem();

/* page update */
    /* signals to slots */
    connect(pagesWidget,  SIGNAL(currentChanged(int)),          this, SLOT(onCurrentPageChanged(int)));
    connect(pageIdentify, SIGNAL(pageChanged(int)),             this, SIGNAL(pageChanged(int)));
    connect(pageIdentify, SIGNAL(sArithmeticChanged(int)),      this, SIGNAL(sArithmeticChanged(int)));
    connect(pageListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));

/* page layout */
    /* widget layout */
    QVBoxLayout *horizontalLayout = new QVBoxLayout;
    horizontalLayout->addWidget(pageListWidget, 1, Qt::AlignCenter);
    horizontalLayout->addWidget(pagesWidget,5, Qt::AlignLeft);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0,0,0,0);
    /* main layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->setContentsMargins(0,0,0,0);

    setLayout(mainLayout);

}

/**
 * @brief PageParams::createPageListItem
 * 方案参数页面列表索引构造函数
 */
void PageParams::createPageListItem()
{
    int width = (LCD_WIDTH == 640) ? 80 : 130;

    //! 获取全部列表索引项
    QList<int> list = getAllPageParamsList();

//    QPalette palette;
//    QPixmap pixmap(":/res/png/materialButton.png");
//    pixmap = pixmap.scaled(width,30);

//    palette.setBrush(backgroundRole(),QBrush(pixmap));

    for (int i = 0; i < list.count(); i++) {
        int pageId = list.at(i);
        pageListWidget->addItem(getPageParamsListName(pageId));
//        pageListWidget->item(i)->setIcon(getPageParamsListIcon(pageId));
        pageListWidget->item(i)->setData(Qt::UserRole, pageId);
        pageListWidget->item(i)->setSizeHint(QSize(width, 30));
        pageListWidget->item(i)->setTextAlignment(Qt::AlignCenter);
        pageListWidget->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
//        pageListWidget->setPalette(palette);
    }
    //! 默认切换到第一项
    if (!list.isEmpty()) {
        pageListWidget->setCurrentRow(list.first());
    }
}


void PageParams::shutdownBg(){
    pageBackground->setMyCurveStart(false);
}

/**
 * @brief PageParams::onCurrentItemChanged
 * 响应方案参数索引列表项切换
 * @param current
 * @param previous
 */
void PageParams::onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    //! 获取当前方案参数页面索引号
    if (!current) {
        current = previous;
    }
    int index = current->data(Qt::UserRole).toInt();

    if (previous->data(Qt::UserRole).toInt() == PageParams_Background) {
        pageBackground->setMyCurveStart(false);
    }

    pagesWidget->setFixedHeight(LCD_HEIGHT-BTN_HEIGHT-210);
    switch (index) {
    case PageParams_Identify:
    case PageParams_Tick:
    case PageParams_EjectTime:
    case PageParams_Lamp:
    case PageParams_EjectTimes:
        pagesWidget->setCurrentIndex(index);
        break;
    case PageParams_Background:
        struGsh.nBackgroudType = 0;
        pagesWidget->setCurrentIndex(index);
        pageBackground->updatePage();
        pagesWidget->setFixedHeight(LCD_HEIGHT-BTN_HEIGHT-145);
        break;
    case PageParams_Feeder_Adjust:
        emit pageChanged(Page_Feeder_Adjust);
        break;
    default:
        break;
    }
}

/**
 * @brief PageParams::refreshPageParams
 * 进入方案参数管理页面后默认切换至算法(可见光/红外)列表页面
 */
void PageParams::refreshPageParams()
{
    //! 获取有效索引列表
    QList<int> list = getSelPageParamsList();

    //! 遍历参数索引列表，若当前存在有效列表索引项，则放开显示，否则隐藏之
    for (int i = 0; i < pageListWidget->count(); i++) {
        int pageId = pageListWidget->item(i)->data(Qt::UserRole).toInt();

        if (list.indexOf(pageId) != -1) {
            pageListWidget->item(i)->setHidden(false);
        } else {
            pageListWidget->item(i)->setHidden(true);
        }
    }
    //! 默认显示有效索引第一项
    if (!list.isEmpty()) {
        pageListWidget->setCurrentRow(list.first());
        //! 重新刷新
        resetParams();
    }
}

/**
 * @brief PageParams::onCurrentPageChanged
 * 响应方案参数管理页面列表项切换
 * @param index
 */
void PageParams::onCurrentPageChanged(int index)
{
    emit setOkBtnHiddenSig(false);

    pageEjectTimes->stopRefresh();

    switch (index) {
    case PageParams_Identify:
        pageIdentify->resetIdentifyParams();
        break;
    case PageParams_Tick:
        pageTick->resetTickParams();
        break;
    case PageParams_EjectTime:
        pageEjectTime->resetEjectTimeParams();
        break;
    case PageParams_Lamp:
        pageLamp->resetLampParams();
        emit setOkBtnHiddenSig(false);
        break;
    case PageParams_EjectTimes:
        pageEjectTimes->refreshDisplay();
        emit setOkBtnHiddenSig(true);
        break;
    case PageParams_Background:
        pageBackground->resetBackgroundParams();
        pageBackground->updatePage();
        pageBackground->setMyCurveStart(true);
        emit setOkBtnHiddenSig(true);
        break;
    default:
        break;
    }
}

/**
 * @brief PageParams::setParams
 * 响应确定按钮，保存当前方案参数页面对应的参数
 */
void PageParams::setParams()
{
    switch (pagesWidget->currentIndex()) {
    case PageParams_Identify:
        pageIdentify->setIdentifyParams();
        break;
    case PageParams_Tick:
        pageTick->setTickParams();
        break;
    case PageParams_EjectTime:
        pageEjectTime->setEjectTimeParams();
        break;
    case PageParams_Lamp:
        pageLamp->setLampParams();
        break;
    case PageParams_Background:
        pageBackground->setBackgroundParams();
    default:
        break;
    }
}

/**
 * @brief PageParams::resetParams
 * 响应取消按钮，重置当前方案参数页面对应的参数
 */
void PageParams::resetParams()
{
    switch (pagesWidget->currentIndex()) {
    case PageParams_Identify:
        pageIdentify->resetIdentifyParams();
        break;
    case PageParams_Tick:
        pageTick->resetTickParams();
        break;
    case PageParams_EjectTime:
        pageEjectTime->resetEjectTimeParams();
        break;
    case PageParams_Lamp:
        pageLamp->resetLampParams();
        break;
    case PageParams_Background:
        pageBackground->setMyCurveStart(false);
    default:
        break;
    }
}

/**
 * @brief PageParams::getAllPageParamsList
 * 获取方案参数显示页面索引列表 - 全部列表
 * @return
 */
QList<int> PageParams::getAllPageParamsList()
{
    QList<int> list;

    //! 全部索引项
    list << PageParams_Identify
         << PageParams_Tick
         << PageParams_EjectTime
         << PageParams_Lamp
         << PageParams_EjectTimes
         << PageParams_Background
         << PageParams_Feeder_Adjust;

    return list;
}

/**
 * @brief PageParams::getSelPageParamsList
 * 获取方案参数显示页面索引列表 - 有效列表
 * @return
 */
QList<int> PageParams::getSelPageParamsList()
{
    QList<int> list;

    switch (struCnfe.nMachine) {
    case MACHINE_CF:
        list << PageParams_Identify
             << PageParams_Tick
             << PageParams_EjectTime
             << PageParams_Lamp
             << PageParams_Background;
        break;
    default:
        list << PageParams_Identify
             << PageParams_Tick
             << PageParams_EjectTime
             << PageParams_Lamp
             << PageParams_Background;
        break;
    }

#if 0
    // 智能供料
    if (struCnfp.autoFeedEnablePageFlag == 1) {
        list << PageParams_Feeder_Adjust;
    }
#endif

    return list;
}

/**
 * @brief PageParams::getPageParamsListName
 * 获取方案参数页面索引列表文本
 * @param index
 * @return
 */
QString PageParams::getPageParamsListName(int index)
{
    QString text = "";
    switch (index) {
    case PageParams_Identify:
//        text = myLan.identify_params;
        text = QString(myLan.algorithm);
        break;
    case PageParams_Tick:
//        text = myLan.tick_params;
        text = QString(myLan.aux_parameter);

        break;
    case PageParams_EjectTime:
//        text = myLan.eject_time;
        text = QString(myLan.set_time);

        break;
    case PageParams_Lamp:
        text = myLan.lamp_control;
//        text = QString("灯光设置");
//        text = QString(myLan.io_set);

        break;
    case PageParams_EjectTimes:
        text = myLan.eject_times;
        break;
    case PageParams_Background:
        text = myLan.background_set;
        break;
    case PageParams_Feeder_Adjust:
        text = myLan.feeder_AI;
        break;
    default:
        text = "";
        break;
    }

    return text;
}

/**
 * @brief PageParams::getPageParamsListIcon
 * 获取方案参数页面索引列表图标
 * @param index
 * @return
 */
QIcon PageParams::getPageParamsListIcon(int index)
{
    QIcon icon;
    switch (index) {
    case PageParams_Identify:
        icon = myIcon.Edit_Search;
        break;
    case PageParams_Tick:
        icon = myIcon.Sorter_Ejector;
        break;
    case PageParams_EjectTime:
        icon = myIcon.Sorter_EjectorTime;
        break;
    case PageParams_Lamp:
        icon = myIcon.Sorter_Light;
        break;
    case PageParams_EjectTimes:
        icon = myIcon.Sorter_EjectorStatistics;
        break;
    case PageParams_Background:
    case PageParams_Feeder_Adjust:
        icon = myIcon.Chart_Line;
        break;
    default:
        icon = QIcon();
        break;
    }
    icon =  QIcon();
    return icon;
}
