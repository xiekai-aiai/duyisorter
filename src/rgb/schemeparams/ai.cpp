#include "ai.h"
#include "unilog.h"
#include "sqlitemgr.h"
#include "aihelper.h"
#include "cmdworker.h"
#include "cmdudpmanager.h"

/* 构造智能参数固化页面 */
PageAi::PageAi(QWidget* parent)
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
    // 点击应用时，处理模型应用 
    LOG_INFO_STM("save page ai params! count:" << modelListWidget->count());
    if (modelListWidget->currentItem() == NULL)
    {
        LOG_INFO_STM("modelListWidget current item is null");
        return;
    }

    QString model_name = modelListWidget->currentItem()->text();
    QString model_id = modelListWidget->currentItem()->data(Qt::UserRole).toString();
    LOG_INFO_STM("modelListWidget current item text : " << model_name.toStdString()
        << ", model id:" << model_id.toStdString());

    // 根据模型id获取模型信息
    ModelInfo info;
    if (!SQLiteMgr::Instance().LoadModelInfoById(model_id, info))
    {
        return;
    }

    if (!modelUpload(info))
    {
        return;
    }

    if (!modelApply(info))
    {
        return;
    }

    if (!SQLiteMgr::Instance().UpdateModelInfoApplyFlag(model_id, true)
        || !SQLiteMgr::Instance().UpdateModelInfoUploadFlag(model_id, true))
    {
        return;
    }


    /* write params to memory */
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        for (int j = 0; j < step; j++)
        {
            struCnfp.struGroupIdentify[i][j] = m_struGroupIdentify[i * step + j];
        }
    }

    myFlow.materialCopyAssemble(0, tabBar->currentIndex(), 0, ARITH_PISTACHIO, 0);
    myFlow.materialResetGroupAssemble(struGsh.nLevel, tabBar->currentIndex(), 0, ARITH_PISTACHIO, 0);
}

/* 重置当前智能参数固化页面设置 */
void PageAi::resetIntelParams()
{
    /* reset params */
    for (int i = 0; i < struCnfg.nLevelTotal; i++)
    {
        int step = struCnfg.struLevelInfo[i].nIdentifyGroupTotal;
        for (int j = 0; j < step; j++)
        {
            m_struGroupIdentify[i * step + j] = struCnfp.struGroupIdentify[i][j];
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
    modelListWidget = new myListWidget(QSize(ICON_WID, ICON_HEI), this);
    modelListWidget->setFixedWidth(800);
    modelListWidget->setFixedHeight(440);
    modelListWidget->setStyleSheet("background-color:transparent");
    modelListWidget->setFrameStyle(QFrame::Panel);

    QHBoxLayout* upLayout = new QHBoxLayout;
    upLayout->addWidget(tabBar);

    QHBoxLayout* downLayout = new QHBoxLayout;
    downLayout->addWidget(modelListWidget);

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addLayout(upLayout, 0, 0);
    mainLayout->addLayout(downLayout, 1, 0);

    setLayout(mainLayout);

    connect(modelListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onCurrentRowChanged(int)));
    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(changeCurrentSlt(int)));
    connect(this, SIGNAL(pageUpdated()), this, SLOT(updatePage()));
}

void PageAi::onCurrentRowChanged(int index)
{
    LOG_INFO_STM("AI onCurrentRowChanged: currentIndex:" << tabBar->currentIndex()
        << " index:" << index << ", modelListWidget count:" << modelListWidget->count());

    int tabIndex = tabBar->currentIndex();
    if (!modelListWidget->count())
    {
        infoWidget->setLabelText(myLan.model_list_blank);
        infoWidget->delayShow();
        myFlow.sleep(3);
        infoWidget->hide();
        return;
    }

    if (modelListWidget->currentItem() == NULL)
    {
        LOG_INFO_STM("modelListWidget current item is null");
        return;
    }

    QString model_name = modelListWidget->currentItem()->text();
    QString model_id = modelListWidget->currentItem()->data(Qt::UserRole).toString();
    const char* charArray = model_id.toStdString().c_str();
    strcpy(m_struGroupIdentify[tabIndex].struAi.modelId, charArray);
    LOG_INFO_STM("current model name:" << model_name.toStdString() << ", model id:" << model_id.toStdString());

}


void PageAi::updateListWidget()
{
    LOG_INFO_STM("AI updateListWidget: currentIndex:" << tabBar->currentIndex());
    if (tabBar->currentIndex() < 0)
    {
        return;
    }

    int tabIndex = tabBar->currentIndex();
    modelListWidget->clear();

    QVector<ModelInfo> model_infos;
    if (!SQLiteMgr::Instance().LoadAllModeInfo(model_infos))
    {
        return;
    }

    for (int idx = 0; idx < model_infos.size(); idx++)
    {
        ModelInfo item = model_infos.at(idx);
        myListWidgetItem* modelListItem = new myListWidgetItem(item.model_name_, QIcon(), QSize(602, 50));
        modelListItem->setData(Qt::UserRole, item.model_id_);
        modelListItem->setTextAlignment(Qt::AlignCenter);
        QPixmap pixmap(":/res/png/material.png");
        pixmap = pixmap.scaled(QSize(800, 50), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        modelListItem->setBackground(QBrush(pixmap));
        modelListWidget->addItem(modelListItem);
        modelListWidget->setSpacing(1);

        if (item.is_apply_)
        {
            modelListWidget->setCurrentRow(idx);
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


bool PageAi::modelUpload(const ModelInfo& info)
{
    if (info.is_upload_)
    {
        LOG_INFO_STM("model:" << info.model_id_.toStdString() << ", model name:" << info.model_name_.toStdString()
            << " uploaded!");
        return true;
    }

    // xktodo 上传模型到AI板卡
    return true;
}

bool PageAi::modelApply(const ModelInfo& info)
{
    LOG_INFO_STM("model apply:" << info.model_name_.toStdString());

    // 给所有的AI板卡发送开始模型应用命令
    ModelApply model_apply;
    model_apply.model_name_ = info.model_name_;
    QByteArray request = cmdworker::ModelApplyRequest(model_apply);

    for (int idx = 0; idx < struCnfg.struLevelInfo[0].nUnitLevelTotal; idx++)
    {
        QByteArray response;
        QString ip = ai_helper::GetAiIpByIndex(idx);
        bool ok = CmdUdpManager::instance().onSendCommand(QHostAddress(ip), AI_UPD_CMD_PORT, request,
            response, AI_RESPONSE_TIMEOUT);

        if (!ok)
        {
            LOG_ERROR_STM("model apply index:" << idx << " ip:" << ip.toStdString() << " send command failed! requst body:" << request.toHex(' ').toUpper().toStdString());
            return false;
        }

        CmdPackage cmd_pkg;
        ok = cmdworker::ParseCmdPkg(response, cmd_pkg);
        if (!ok)
        {
            LOG_ERROR_STM("model apply index:" << idx << " ip:" << ip.toStdString() << " parse resonpse failed! request body:" << request.toHex(' ').toUpper().toStdString()
                << ", response body:" << response.toHex(' ').toUpper().toStdString());
            return false;
        }

        int code = cmdworker::CommResponse(cmd_pkg).code_;

        if (AI_RESPONSE_SUCCESS != code)
        {
            LOG_ERROR_STM("model apply index:" << idx << " ip:" << ip.toStdString() << " send command:" << request.toHex(' ').toUpper().toStdString() << ", response:"
                << response.toHex(' ').toUpper().toStdString() << ", code:" << code);
            return false;
        }

        LOG_INFO_STM("model apply index:" << idx << " ip:" << ip.toStdString() << " send command:" << request.toHex(' ').toUpper().toStdString() << ", response:"
            << response.toHex(' ').toUpper().toStdString() << ", code:" << code);
    }

    return true;
}
