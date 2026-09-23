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

PageAi::~PageAi()
{
    for (auto* worker : sftp_worker_map.values())
    {
        worker->deleteLater();
    }
    sftp_worker_map.clear();
}

void PageAi::onUploadModel()
{
    // 点击上传时，处理模型应用 
    LOG_INFO_STM("upload page ai params! count:" << modelListWidget->count());

    if (modelListWidget->currentItem() == NULL)
    {
        LOG_INFO_STM("modelListWidget current item is null");
        return;
    }

    QString model_name = modelListWidget->currentItem()->text();
    QString model_id = modelListWidget->currentItem()->data(Qt::UserRole).toString();
    LOG_INFO_STM("modelListWidget current item text : " << model_name.toStdString()
        << ", model id:" << model_id.toStdString());

    ModelInfo info;
    info.is_apply_ = false;
    info.is_upload_ = false;
    info.model_id_ = model_id;
    info.model_name_ = model_name;
    if (!modelUpload(info))
    {
        QMessageBox::warning(this, "应用警告", "模型上传失败！");
        return;
    }

    SQLiteMgr::Instance().UpdateModelInfoUploadFlag(model_id, true);
}

void PageAi::onDeleteModel()
{
    // 点击上传时，处理模型应用 
    LOG_INFO_STM("delete page ai params! count:" << modelListWidget->count());

    if (modelListWidget->currentItem() == NULL)
    {
        LOG_INFO_STM("modelListWidget current item is null");
        return;
    }

    QString model_name = modelListWidget->currentItem()->text();
    QString model_id = modelListWidget->currentItem()->data(Qt::UserRole).toString();
    LOG_INFO_STM("modelListWidget current item text : " << model_name.toStdString()
        << ", model id:" << model_id.toStdString());

    ModelInfo info;
    if (!SQLiteMgr::Instance().LoadModelInfoById(model_id, info))
    {
        QMessageBox::warning(this, "应用警告", "模型删除失败！");
        return;
    }

    if (info.is_apply_)
    {
        QMessageBox::warning(this, "应用警告", "模型应用中，删除失败！");
        return;
    }

    if(!modelDelete(info))
    {
        QMessageBox::warning(this, "应用警告", "模型删除失败！");
        return;
    }

    SQLiteMgr::Instance().DelModelInfo(model_id);
    SQLiteMgr::Instance().DelModelClsParam(model_id);

    emit pageUpdated();
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

    ModelInfo info;
    if (!SQLiteMgr::Instance().LoadModelInfoById(model_id, info))
    {
        QMessageBox::warning(this, "应用警告", "模型加载失败！");
        return;
    }

    if (!modelUpload(info))
    {
        QMessageBox::warning(this, "应用警告", "模型上传失败！");
        return;
    }

    if (!modelApply(info))
    {
        QMessageBox::warning(this, "应用警告", "模型应用失败！");
        return;
    }

    if (!SQLiteMgr::Instance().UpdateModelInfoUploadFlag(model_id, true)
        || !SQLiteMgr::Instance().UpdateModelInfoApplyFlag(model_id, true))
    {
        QMessageBox::warning(this, "应用警告", "模型应用持久化失败！");
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

    int lastDot = info.model_name_.lastIndexOf('.');
    QString name = (lastDot != -1) ? info.model_name_.left(lastDot) : info.model_name_;

    QStringList upload_lists;
    upload_lists.append(ai_helper::GetModelRootPath() + "/" + info.model_name_);
    upload_lists.append(ai_helper::GetModelRootPath() + "/" + name + ".json");

    for (int idx = 0; idx < struCnfg.struLevelInfo[0].nUnitLevelTotal; idx++)
    {
        QString ip = ai_helper::GetAiIpByIndex(idx);
        auto it = sftp_worker_map.find(ip);
        if (sftp_worker_map.end() == it)
        {
            SftpWorker* worker = new SftpWorker(ip, AI_DEV_USER, AI_DEV_PWD);
            it = sftp_worker_map.insert(ip, worker);
        }

        bool ret = it.value()->onUploadFilesSync(upload_lists, AI_DEV_MODEL_PATH);
        if (!ret)
        {
            return ret;
        }
    }

    return true;
}

bool PageAi::modelDelete(const ModelInfo& info)
{
    if (info.is_apply_)
    {
        LOG_ERROR_STM("model:" << info.model_id_.toStdString() << ", model name:" << info.model_name_.toStdString()
            << " applyed!");
        return false;
    }

    int lastDot = info.model_name_.lastIndexOf('.');
    QString name = (lastDot != -1) ? info.model_name_.left(lastDot) : info.model_name_;

    QStringList del_lists;
    del_lists.append(AI_DEV_MODEL_PATH + info.model_name_);
    del_lists.append(AI_DEV_MODEL_PATH + name + ".json");

    for (int idx = 0; idx < struCnfg.struLevelInfo[0].nUnitLevelTotal; idx++)
    {
        QString ip = ai_helper::GetAiIpByIndex(idx);
        auto it = sftp_worker_map.find(ip);
        if (sftp_worker_map.end() == it)
        {
            SftpWorker* worker = new SftpWorker(ip, AI_DEV_USER, AI_DEV_PWD);
            it = sftp_worker_map.insert(ip, worker);
        }

        bool ret = it.value()->onDeleteFilesSync(del_lists);
        if (!ret)
        {
            return ret;
        }
    }

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
