#include "ai.h"

/* 构造智能参数固化页面 */
PageAi::PageAi(QWidget *parent)
    : QWidget(parent)
{
/* page init */
    createPage();

/* page update */
    updatePage();

/* page layout */
    layoutPage();
}

/* 应用当前智能参数固化页面设置 */
void PageAi::setIntelParams()
{
    /* write params to memory */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        for (int j = 0; j < step; j++) {
            struCnfp.struGroupIdentify[i][j] = m_struGroupIdentify[i*step+j];
        }
    }

    myFlow.materialCopyAssemble(0, tabBar->currentIndex(), 0, ARITH_PISTACHIO, 0);
    myFlow.materialResetGroupAssemble(struGsh.nLevel, tabBar->currentIndex(), 0, ARITH_PISTACHIO, 0);

    myFlow.initModelType();
//    myFlow.initModelPara();

}

/* 重置当前智能参数固化页面设置 */
void PageAi::resetIntelParams()
{
/* reset params */
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        for (int j = 0; j < step ; j++) {
            m_struGroupIdentify[i*step+j] = struCnfp.struGroupIdentify[i][j];
        }
    }

/* reset page */
    emit pageUpdated();
}

/* 初始化智能参数固化页面 */
void PageAi::createPage()
{

    /* tab */
    tabBar = new MyTabBar();
    modelListWidget = new myListWidget(QSize(ICON_WID, ICON_HEI),this);
    modelListWidget->setFixedWidth(800);
    modelListWidget->setFixedHeight(440);
    modelListWidget->setStyleSheet("background-color:transparent");
    modelListWidget->setFrameStyle(QFrame::Panel);
//    applyModeBtn = new myPushButton(myLan.apply, QIcon(""));
//    applyModeBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    QHBoxLayout *upLayout = new QHBoxLayout;
//    QSpacerItem *upSpacer = new QSpacerItem(BTN_WIDTH, BTN_HEIGHT, QSizePolicy::Fixed, QSizePolicy::Fixed);
//    upLayout->addItem(upSpacer);
    upLayout->addWidget(tabBar);

    QHBoxLayout *downLayout = new QHBoxLayout;
    downLayout->addWidget(modelListWidget);
//    QHBoxLayout *operatePageHBLayout  = new QHBoxLayout;
//    operatePageHBLayout->addWidget(applyModeBtn);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(upLayout, 0, 0);
    mainLayout->addLayout(downLayout, 1, 0);

    setLayout(mainLayout);

    connect(modelListWidget,     SIGNAL(currentRowChanged(int)),    this,        SLOT(onCurrentRowChanged(int)));
    connect(tabBar,     SIGNAL(currentChanged(int)),    this,        SLOT(changeCurrentSlt(int)));
    connect(this,       SIGNAL(pageUpdated()),          this,        SLOT(updatePage()));
//    connect(applyModeBtn,       SIGNAL(pressed()),          this,        SLOT(onApplyModeBtnPressed()));

}

void PageAi:: onCurrentRowChanged(int index){

    int tabIndex = tabBar->currentIndex();
    if(modelListWidget->currentItem() < 0 ){
        return;
    }
    if(!modelListWidget->count()){
        infoWidget->setLabelText(myLan.model_list_blank);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
     if(modelListWidget->currentItem() == NULL ||
            (!modelListWidget->currentItem()->text().contains("-"))){
        return;
    }
    QStringList list =  modelListWidget->currentItem()->text().split("-");
    if(list.size() < 2){
        infoWidget->setLabelText(myLan.model_para_error);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }
    QString  modelId = list.at(1);
    std::string stdStr = modelId.toStdString();
    const char* charArray = stdStr.c_str();
    strcpy(m_struGroupIdentify[tabIndex].struAi.modelId, charArray);

//    m_struGroupIdentify[tabIndex].struAi.modelId = charArray;
//    memcpy(m_struGroupIdentify[tabIndex].struAi.modelId, modelId.toUtf8().data(), sizeof(modelId.toUtf8().data()));
    qDebug() <<"123" << m_struGroupIdentify[tabIndex].struAi.modelId;

     /* write params */
//     setIntelParams();
}


void PageAi::updateListWidget()
{
    if(tabBar->currentIndex() < 0){
        return;
    }
    int tabIndex =tabBar->currentIndex();

    modelListWidget->clear();
    int defaultModelListWidget = 0;

    int count = 0;
    QSqlQuery query;
    if (!query.exec("SELECT modelId, modelName, isApply FROM modelInfo order by chgTime desc")) {
        qDebug() << "查询失败：" << query.lastError();
    } else {
        while (query.next()) {
            QString modelId = query.value(0).toString();
            QString modelName = query.value(1).toString();
            int isApply =  query.value(2).toInt();
//            qDebug() << modelId << modelName<<isApply;

            myListWidgetItem *modelListItem = new myListWidgetItem(modelName+"-"+modelId, QIcon(), QSize(602,50));
            modelListItem->setTextAlignment(Qt::AlignCenter);
            QPixmap pixmap(":/res/png/material.png");
            pixmap = pixmap.scaled(QSize(800,50),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            modelListItem->setBackground(QBrush(pixmap));
            modelListWidget->addItem(modelListItem);
            modelListWidget->setSpacing(1);
            count ++;
//            qDebug()<<QString::fromUtf8(struCnfp.struGroupIdentify[struGsh.nLevel][tabIndex].struAi.modelId);
            if(QString::fromUtf8(struCnfp.struGroupIdentify[struGsh.nLevel][tabIndex].struAi.modelId) == modelId){
                defaultModelListWidget = count-1;
                defaultModelId = modelId;
//                qDebug() << defaultModelListWidget << defaultModelId;
                modelListWidget->setCurrentRow(defaultModelListWidget);
            }
        }

    }
}

/* 更新智能参数固化页面 */
void PageAi::updatePage()
{
    /* update tab widget page */
    updateTabBar(tabBar, true, false);
    updateListWidget();
}

/* 响应智能参数固化页面tab切换 */
void PageAi::changeCurrentSlt(int index)
{
    updateListWidget();
}

/* 布局智能参数固化页面 */
void PageAi::layoutPage()
{

}
