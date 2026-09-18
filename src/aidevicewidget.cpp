#include "aidevicewidget.h"
#include <QGridLayout>
#include <QDir>
#include <QDateTime>
#include <QMutexLocker>
#include "unilog.h"
#include "aihelper.h"
#include "sortertypes.h"
#include "mylanguage.h"
#include "globalparams.h"
#include "mypushbutton.h"
#include "mylistwidgetitem.h"
#include "aidiskinfowidget.h"
#include "aiimageacquisitionwidget.h"

AiDeviceWidget::AiDeviceWidget(QWidget* parent) : QWidget(parent)
{
    // 标题栏默认不显示
    setWindowFlags(Qt::FramelessWindowHint);

    setGeometry(0, 0, LCD_WIDTH - 20, LCD_HEIGHT - 60);
    setFixedSize(LCD_WIDTH - 20, LCD_HEIGHT - 60);
    this->setAutoFillBackground(true);

    QPalette palette;
    QPixmap pixmap(":/res/png/back.png");
    pixmap = pixmap.scaled(this->size());
    palette.setBrush(backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);


    // 左侧ListWidet控件
    listWidget = new myListWidget();
    listWidget->setViewMode(QListView::ListMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setFixedWidth(BTN_WIDTH);
    listWidget->setStyleSheet("background-color:transparent");
    listWidget->setFixedSize(LCD_WIDTH / 5, LCD_HEIGHT - 200);


    // 右侧StackedWidet控件
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedSize(700, LCD_HEIGHT - 150);

    // 第一个widget是默认参数Widget,即图像采集配置页面
    AiImageAcquisitionWidget* imgAcqWidget = new AiImageAcquisitionWidget();
    stackedWidget->addWidget(imgAcqWidget);

    myListWidgetItem* item1 = new myListWidgetItem(QString(""), QIcon(), QSize());
    item1->setText(myLan.default_params);
    item1->setSizeHint(QSize(100, 70));
    listWidget->addItem(item1);

    // 第二个widget是磁盘信息Widget
    AiDiskInfoWidget* diskWidget = new AiDiskInfoWidget();
    stackedWidget->addWidget(diskWidget);

    myListWidgetItem* item2 = new myListWidgetItem(QString(""), QIcon(), QSize());
    item2->setText(myLan.disk_space);
    item2->setSizeHint(QSize(100, 70));
    listWidget->addItem(item2);

    // 应用按钮
    applyBtn = new myPushButton(myLan.apply, QIcon(""), true, true, this);
    applyBtn->setFont(g_Config::getInstance()->getFont(DEFAULT_FONT_SIZE));
    applyBtn->setIconSize(QSize(ICON_WID, ICON_WID));
    applyBtn->setFixedSize(BTN_WIDTH + 30, BTN_HEIGHT);

    // 图像下载按钮
    downBtn = new myPushButton(myLan.download_image, QIcon(""), true, true, this);
    downBtn->setFont(g_Config::getInstance()->getFont(DEFAULT_FONT_SIZE));
    downBtn->setIconSize(QSize(ICON_WID, ICON_WID));
    downBtn->setFixedSize(BTN_WIDTH + 70, BTN_HEIGHT);

    // 返回按钮
    backBtn = new myPushButton(myLan.back, QIcon(""), true, true, this);
    backBtn->setFont(g_Config::getInstance()->getFont(DEFAULT_FONT_SIZE));
    backBtn->setIconSize(QSize(ICON_WID, ICON_WID));
    backBtn->setFixedSize(BTN_WIDTH + 30, BTN_HEIGHT);


    // 主布局
    QGridLayout* mainGridLayout = new QGridLayout(this);
    mainGridLayout->setContentsMargins(20, 0, 20, 20);
    mainGridLayout->setHorizontalSpacing(20);
    mainGridLayout->setRowMinimumHeight(0, LCD_TITLE_HEIGHT - 10);
    mainGridLayout->setVerticalSpacing(0);

    mainGridLayout->addWidget(listWidget, 0, 0, 1, 1, Qt::AlignCenter);
    mainGridLayout->addWidget(stackedWidget, 0, 1, 1, 4, Qt::AlignCenter);
    mainGridLayout->addWidget(applyBtn, 1, 0, 1, 1, Qt::AlignCenter);
    mainGridLayout->addWidget(downBtn, 1, 2, 1, 1, Qt::AlignCenter);
    mainGridLayout->addWidget(backBtn, 1, 4, 1, 1, Qt::AlignCenter);

    // note: 图像采集下载，目前只下载通道1的前后视
    for (int idx = 0; idx < AI_SFTP_DEV_NUM; idx++)
    {
        QThread* th = new QThread(this);
        SftpWorker* worker = new SftpWorker(ai_helper::GetAiIpByIndex(idx), AI_DEV_USER, AI_DEV_PWD);
        worker->moveToThread(th);

        // 下载完成
        connect(worker, &SftpWorker::allDownloadCompleted,
            this,
            [this, idx](bool success) {
                onDownloadFinished(idx, success);
            }
        );

        // 下载进度
        connect(worker, &SftpWorker::progressChanged,
            this,
            [this, idx](int current, int total) {
                onProgressChanged(idx, current, total);
            }
        );

        // 线程结束
        connect(th, &QThread::finished,
            worker,
            &QObject::deleteLater);

        th->start();

        sftpThreads.append(th);
        sftpWorkers.append(worker);
    }

    progressDlg = new QProgressDialog(this);
    progressDlg->setWindowTitle("Down Image");
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setAutoClose(false);
    progressDlg->setAutoReset(false);
    progressDlg->setRange(0, 100);
    progressDlg->setMinimumDuration(INT_MAX);
    progressDlg->setValue(0);
    progressDlg->setFixedSize(600, 200);
    // 不允许取消，直接隐藏取消按钮
    progressDlg->setCancelButton(nullptr);
    progressDlg->hide();

    LOG_INFO_STM("list size:" << listWidget->count() << ",statcked:" << stackedWidget->count());

    connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onListWidgetRowChangedSlt(int)));
    connect(applyBtn, SIGNAL(pressed()), this, SLOT(onApplyBtnPressed()));
    connect(downBtn, SIGNAL(pressed()), this, SLOT(onDownBtnPressed()));
    connect(backBtn, SIGNAL(pressed()), this, SLOT(onBackBtnPressed()));

    // 默认显示第一个空间
    stackedWidget->setCurrentIndex(0);
    listWidget->setCurrentRow(0);
}


AiDeviceWidget::~AiDeviceWidget()
{
    destroySftp();
}

void AiDeviceWidget::destroySftp()
{
    for (int idx = 0; idx < sftpThreads.size(); idx++)
    {
        if (sftpThreads[idx])
        {
            continue;
        }

        if (sftpThreads[idx]->isRunning())
        {
            sftpThreads[idx]->quit();
            sftpThreads[idx]->wait();
        }

        sftpThreads[idx] = nullptr;
        sftpWorkers[idx] = nullptr;
    }
    sftpThreads.clear();
    sftpWorkers.clear();
}

void AiDeviceWidget::onListWidgetRowChangedSlt(int idx)
{
    stackedWidget->setCurrentIndex(idx);
    switch (idx)
    {
    case 0:
    {
        // 对应图像下载配置页面
        auto* page = stackedWidget->currentWidget();
        auto* acqWidget = dynamic_cast<AiImageAcquisitionWidget*>(page);
        if (!acqWidget)
        {
            LOG_ERROR_STM("idx:" << idx << " not AiImageAcquisitionWidget!");
            return;
        }
        acqWidget->onRefresh();
    }

    break;
    case 1:
    {
        // 对应磁盘信息页面
        auto* page = stackedWidget->currentWidget();
        auto* diskWidget = dynamic_cast<AiDiskInfoWidget*>(page);
        if (!diskWidget)
        {
            LOG_ERROR_STM("idx:" << idx << " not AiDiskInfoWidget!");
            return;
        }
        diskWidget->onRefresh();
    }
    break;
    default:
        LOG_ERROR_STM("unknown list idx:" << idx);
        break;
    }
}

void AiDeviceWidget::onApplyBtnPressed()
{
    int idx = listWidget->currentIndex().row();

    switch (idx)
    {
    case 0:
    {
        // 对应图像下载配置页面
        auto* page = stackedWidget->currentWidget();
        auto* acqWidget = dynamic_cast<AiImageAcquisitionWidget*>(page);
        if (!acqWidget)
        {
            LOG_ERROR_STM("idx:" << idx << " not AiImageAcquisitionWidget!");
            return;
        }
        acqWidget->onSave();
    }
    break;
    case 1:
    {
        // 对应磁盘信息页面
        auto* page = stackedWidget->currentWidget();
        auto* diskWidget = dynamic_cast<AiDiskInfoWidget*>(page);
        if (!diskWidget)
        {
            LOG_ERROR_STM("idx:" << idx << " not AiDiskInfoWidget!");
            return;
        }
        diskWidget->onSave();
    }
    break;
    default:
        LOG_ERROR_STM("unknown list idx:" << idx);
    }
}

void AiDeviceWidget::onBackBtnPressed()
{
    emit backToHomePageSig();
}

void AiDeviceWidget::onDownloadFinished(int idx, bool success)
{
    if (idx < 0 || idx >= AI_SFTP_DEV_NUM)
    {
        LOG_ERROR_STM("progress idx error:" << idx);
        return;
    }

    QMutexLocker locker(&mutex);
    downProgressArr[idx].finished_ = true;

    bool all_finished = true;
    for (int idx = 0; idx < AI_SFTP_DEV_NUM; idx++)
    {
        if (!downProgressArr[idx].finished_)
        {
            all_finished = false;
            break;
        }
    }

    LOG_INFO_STM("idx :" << idx << ", success:" << success << ", all finished:" << all_finished);

    if (all_finished)
    {
        progressDlg->setValue(100);

        progressDlg->setLabelText("Download Finished!");
        progressDlg->hide();;

        // 下载按钮可用
        downBtn->setEnabled(true);
    }

}

void AiDeviceWidget::onProgressChanged(int idx, int current, int total)
{
    if (idx < 0 || idx >= AI_SFTP_DEV_NUM)
    {
        LOG_ERROR_STM("progress idx error:" << idx << ", current:" << current << ", total:" << total);
        return;
    }

    LOG_TRACE_STM("idx:" << idx << ", current:" << current << ", total:" << total);

    int all_total = 0;
    int all_current = 0;

    QMutexLocker locker(&mutex);
    downProgressArr[idx].current_num_ = current;
    downProgressArr[idx].total_num_ = total;
    for (int i = 0; i < AI_SFTP_DEV_NUM; i++)
    {
        all_total += downProgressArr[i].total_num_;
        all_current += downProgressArr[i].current_num_;
    }

    int value = (all_total == 0) ? (100) : (all_current * 100 / all_total);
    progressDlg->setValue(value);
    progressDlg->setLabelText(tr("Image Downing... %1%").arg(value));
}

void AiDeviceWidget::onDownBtnPressed()
{
    // 下载操作
    for (int idx = 0; idx < AI_SFTP_DEV_NUM; idx++)
    {
        downProgressArr[idx].idx_ = idx;
        downProgressArr[idx].current_num_ = 0;
        downProgressArr[idx].total_num_ = 0;
        downProgressArr[idx].finished_ = false;
    }

    LOG_INFO_STM("onDownBtnPressed start download image...");

    progressDlg->setValue(0);
    // progressDlg->show();   // 调试时屏蔽下载进度弹窗，后台下载仍进行
    QString downDir = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString downPath = ai_helper::GetAcqImgRootPath() + "/" + downDir;
    if (!QDir().mkpath(downPath))
    {
        LOG_ERROR_STM("mkdir downPath:" << downPath.toStdString() << " failed!");
    }

    for (int idx = 0; idx < sftpWorkers.size(); idx++)
    {
        QMetaObject::invokeMethod(
            sftpWorkers[idx],
            "onDownloadRemoteDir",
            Qt::QueuedConnection,
            Q_ARG(QString, AI_DEV_IMG_ACQ_PATH),
            Q_ARG(QString, downPath));
    }

    // 下载按钮先不可用
    downBtn->setEnabled(false);
}
