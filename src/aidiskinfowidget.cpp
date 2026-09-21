#include "aidiskinfowidget.h"
#include <QHBoxLayout>
#include "unilog.h"
#include "aihelper.h"
#include "mylabel.h"
#include "mylanguage.h"
#include "cmdworker.h"
#include "cmdudpmanager.h"

AiDiskInfoWidget::AiDiskInfoWidget(QWidget* parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
}

void AiDiskInfoWidget::onRefresh()
{
    clear();

    QMap<int, quint32> disk_map = GetDiskInfo();
    for (auto it = disk_map.constBegin();
        it != disk_map.constEnd();
        ++it)
    {
        QHBoxLayout* deviceLayout = new QHBoxLayout;
        QString key = QString("aiDevice %1 ").arg(it.key()) + myLan.disk_space + " : ";
        QString value = QString::number(it.value()) + " M";
        myLabel* devLabel = new myLabel(key, this);
        myLabel* diskLabel = new myLabel(value, this);

        deviceLayout->addWidget(devLabel);
        deviceLayout->addWidget(diskLabel);
        deviceLayout->addSpacing(300);
        mainLayout->addLayout(deviceLayout);
    }
}

void AiDiskInfoWidget::onSave()
{
    // note: 版本信息不需要查询
}

QMap<int, quint32> AiDiskInfoWidget::GetDiskInfo()
{
    QMap<int, quint32> disk_map;

    LOG_INFO_STM("level:" << struGsh.nLevel << " view:" << struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal
        << ", num:" << struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal);

    int cam_num = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;

    // 获取磁盘信息
    QByteArray request = cmdworker::DiskSpaceRequest();

    // 相机和AI板卡一一对应，编译所有AI板卡
    for (int idx = 0; idx < cam_num; ++idx)
    {
        QString ip = ai_helper::GetAiIpByIndex(idx);

        QByteArray response;
        bool ok = CmdUdpManager::instance().onSendCommand(QHostAddress(ip), AI_UPD_CMD_PORT, request,
            response, AI_RESPONSE_TIMEOUT);
        if (!ok)
        {
            LOG_ERROR_STM("Get Ai device index:" << idx << " ip:" << ip.toStdString() << " disk space failed! request body:" << request.toHex(' ').toUpper().toStdString());
            disk_map.insert(idx + 1, 0);
            continue;
        }

        CmdPackage cmd_pkg;
        ok = cmdworker::ParseCmdPkg(response, cmd_pkg);
        if (!ok)
        {
            LOG_ERROR_STM("Get AI device index:" << idx << " ip:" << ip.toStdString() << " disk space parse failed! request body:" << request.toHex(' ').toUpper().toStdString()
                << ", response body:" << response.toHex(' ').toUpper().toStdString());
            disk_map.insert(idx + 1, 0);
            continue;
        }

        LOG_INFO_STM("AI device index:" << idx << " ip:" << ip.toStdString() << " disk space:" << cmdworker::DiskSpaceResponse(cmd_pkg).space_);
        disk_map.insert(idx + 1, cmdworker::DiskSpaceResponse(cmd_pkg).space_);
    }
    return disk_map;
}

void AiDiskInfoWidget::clear()
{
    while (mainLayout->count() > 0)
    {
        // 获取HBoyLayout
        QLayoutItem* item = mainLayout->takeAt(0);
        if (!item) continue;

        if (QLayout* layout = item->layout())
        {
            while (layout->count() > 0)
            {
                QLayoutItem* child = layout->takeAt(0);
                if (child->widget())
                {
                    delete child->widget();
                }
                delete child;
            }
        }
        delete item;
    }
}
