#include "aidevicewidget.h"
#include <QGridLayout>
#include <QDir>
#include <QDateTime>
#include <QMutexLocker>
#include <QStorageInfo>
#include <QTimer>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "unilog.h"
#include "aihelper.h"
#include "sortertypes.h"
#include "mylanguage.h"
#include "globalparams.h"
#include "mypushbutton.h"
#include "mylistwidgetitem.h"
#include "aidiskinfowidget.h"
#include "aiimageacquisitionwidget.h"

/**
 * 图像下载进度对话框：总进度 + 逐块 AI 板卡进度
 * 下载仍是多板并行，这里只做展示；逐板进度来自 SftpWorker 的文件数进度。
 */
class ImageDownloadDialog : public QDialog
{
public:
    explicit ImageDownloadDialog(QWidget* parent = nullptr) : QDialog(parent)
    {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setWindowModality(Qt::WindowModal);
        setFixedSize(720, 348);

        setStyleSheet(
            "QDialog { background:#0c161c; }"
            "QLabel { color:#cfe3ea; font-family:'Microsoft YaHei'; }"
            "QProgressBar { background:#0a141a; border:1px solid #16303b; border-radius:5px; }"
            "QProgressBar::chunk { background:#22d3ee; border-radius:4px; }"
            "QPushButton { background:#0d1a21; border:1px solid #1e3a4a; border-radius:4px;"
            "              color:#cfe3ea; font-size:13px; font-family:'Microsoft YaHei'; }"
            "QPushButton:hover { border-color:#155e6b; color:#22d3ee; }");

        QVBoxLayout* root = new QVBoxLayout(this);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);

        // ── 标题栏 ──
        QWidget* head = new QWidget(this);
        head->setAttribute(Qt::WA_StyledBackground, true);
        head->setFixedHeight(44);
        head->setStyleSheet("background:#0d1a21;");
        QHBoxLayout* headLay = new QHBoxLayout(head);
        headLay->setContentsMargins(20, 0, 12, 0);
        QLabel* title = new QLabel(tr("图像下载"), head);
        title->setStyleSheet("font-size:15px; font-weight:600; color:#e6f6fa;");
        QPushButton* closeBtn = new QPushButton(QStringLiteral("×"), head);
        closeBtn->setFixedSize(24, 24);
        closeBtn->setStyleSheet(
            "QPushButton { border:none; background:transparent; color:#6b8794; font-size:16px; }"
            "QPushButton:hover { background:#16222c; color:#cfe3ea; }");
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::hide);
        headLay->addWidget(title);
        headLay->addStretch();
        headLay->addWidget(closeBtn);
        root->addWidget(head);

        QFrame* headLine = new QFrame(this);
        headLine->setFrameShape(QFrame::HLine);
        headLine->setFixedHeight(1);
        headLine->setStyleSheet("background:#16303b; border:none;");
        root->addWidget(headLine);

        // ── 主体 ──
        QWidget* body = new QWidget(this);
        QVBoxLayout* bl = new QVBoxLayout(body);
        bl->setContentsMargins(24, 18, 24, 14);
        bl->setSpacing(0);

        QHBoxLayout* totalRow = new QHBoxLayout;
        QLabel* totalTitle = new QLabel(tr("总进度"), body);
        totalTitle->setStyleSheet("font-size:12px; color:#6b8794;");
        m_totalPct = new QLabel(QStringLiteral("0%"), body);
        m_totalPct->setStyleSheet("font-size:16px; font-weight:600; color:#22d3ee; font-family:Consolas;");
        totalRow->addWidget(totalTitle);
        totalRow->addStretch();
        totalRow->addWidget(m_totalPct);
        bl->addLayout(totalRow);

        m_totalBar = new QProgressBar(body);
        m_totalBar->setRange(0, 100);
        m_totalBar->setValue(0);
        m_totalBar->setTextVisible(false);
        m_totalBar->setFixedHeight(10);
        bl->addSpacing(8);
        bl->addWidget(m_totalBar);
        bl->addSpacing(16);

        for (int idx = 0; idx < AI_SFTP_DEV_NUM; ++idx)
        {
            BoardRow br;
            QWidget* row = new QWidget(body);
            QHBoxLayout* rl = new QHBoxLayout(row);
            rl->setContentsMargins(0, 0, 0, 0);
            rl->setSpacing(10);

            br.dot = new QLabel(row);
            br.dot->setFixedSize(10, 10);
            br.dot->setStyleSheet("background:#2c424e; border-radius:5px;");

            br.name = new QLabel(QString("AI 板卡 %1 · %2")
                                 .arg(idx + 1)
                                 .arg(ai_helper::GetAiIpByIndex(idx)), row);
            br.name->setFixedWidth(190);
            br.name->setStyleSheet("font-size:12px; color:#cfe3ea;");

            br.bar = new QProgressBar(row);
            br.bar->setRange(0, 100);
            br.bar->setValue(0);
            br.bar->setTextVisible(false);
            br.bar->setFixedHeight(6);

            br.right = new QLabel(tr("等待中"), row);
            br.right->setFixedWidth(90);
            br.right->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            br.right->setStyleSheet("font-size:11px; color:#6b8794;");

            rl->addWidget(br.dot);
            rl->addWidget(br.name);
            rl->addWidget(br.bar, 1);
            rl->addWidget(br.right);
            bl->addWidget(row);
            bl->addSpacing(10);
            m_rows.append(br);
        }

        m_result = new QLabel(body);
        m_result->setVisible(false);
        m_result->setStyleSheet("font-size:12px; font-weight:600;");
        bl->addWidget(m_result);
        root->addWidget(body);

        QFrame* footLine = new QFrame(this);
        footLine->setFrameShape(QFrame::HLine);
        footLine->setFixedHeight(1);
        footLine->setStyleSheet("background:#16303b; border:none;");
        root->addWidget(footLine);

        // ── 底部 ──
        QWidget* foot = new QWidget(this);
        foot->setAttribute(Qt::WA_StyledBackground, true);
        QHBoxLayout* fl = new QHBoxLayout(foot);
        fl->setContentsMargins(24, 12, 24, 14);
        m_hint = new QLabel(tr("正在从 AI 板卡下载图像…"), foot);
        m_hint->setStyleSheet("font-size:11px; color:#3f5a66;");
        QPushButton* okBtn = new QPushButton(tr("关闭"), foot);
        okBtn->setFixedSize(100, 34);
        connect(okBtn, &QPushButton::clicked, this, &QDialog::hide);
        fl->addWidget(m_hint);
        fl->addStretch();
        fl->addWidget(okBtn);
        root->addWidget(foot);
    }

    void reset()
    {
        m_totalBar->setValue(0);
        m_totalPct->setText(QStringLiteral("0%"));
        for (int i = 0; i < m_rows.size(); ++i)
        {
            BoardRow& br = m_rows[i];
            br.bar->setValue(0);
            br.bar->setStyleSheet(QString());
            br.dot->setStyleSheet("background:#2c424e; border-radius:5px;");
            br.right->setText(tr("等待中"));
            br.right->setStyleSheet("font-size:11px; color:#6b8794;");
        }
        m_result->hide();
        m_hint->setText(tr("正在从 AI 板卡下载图像…"));
    }

    void setOverall(int value)
    {
        value = qBound(0, value, 100);
        m_totalBar->setValue(value);
        m_totalPct->setText(QString::number(value) + "%");
    }

    void setBoardProgress(int idx, int current, int total)
    {
        if (idx < 0 || idx >= m_rows.size())
        {
            return;
        }
        BoardRow& br = m_rows[idx];
        const int pct = (total > 0) ? (current * 100 / total) : 0;
        br.bar->setValue(pct);
        br.dot->setStyleSheet("background:#22d3ee; border-radius:5px;");
        br.right->setText((total > 0)
            ? tr("%1/%2 张").arg(current).arg(total)
            : tr("等待中"));
        br.right->setStyleSheet("font-size:11px; color:#6b8794;");
    }

    void setBoardFinished(int idx, bool success)
    {
        if (idx < 0 || idx >= m_rows.size())
        {
            return;
        }
        BoardRow& br = m_rows[idx];
        br.bar->setValue(100);
        br.bar->setStyleSheet(success
            ? "QProgressBar::chunk { background:#34d399; border-radius:4px; }"
            : "QProgressBar::chunk { background:#f87171; border-radius:4px; }");
        br.dot->setStyleSheet(success
            ? "background:#34d399; border-radius:5px;"
            : "background:#f87171; border-radius:5px;");
        br.right->setText(success ? tr("完成") : tr("失败"));
        br.right->setStyleSheet(success
            ? "font-size:11px; color:#34d399;"
            : "font-size:11px; color:#f87171;");
    }

    void showResult(bool allSuccess)
    {
        setOverall(100);
        m_result->setText(allSuccess ? tr("Download Finished!") : tr("Download Failed!"));
        m_result->setStyleSheet(allSuccess
            ? "font-size:12px; font-weight:600; color:#34d399;"
            : "font-size:12px; font-weight:600; color:#f87171;");
        m_result->setVisible(true);
        m_hint->setText(tr("对话框将在 3 秒后自动关闭"));
    }

private:
    struct BoardRow
    {
        QLabel* dot{ nullptr };
        QLabel* name{ nullptr };
        QProgressBar* bar{ nullptr };
        QLabel* right{ nullptr };
    };

    QProgressBar* m_totalBar{ nullptr };
    QLabel* m_totalPct{ nullptr };
    QLabel* m_result{ nullptr };
    QLabel* m_hint{ nullptr };
    QVector<BoardRow> m_rows;
};


AiDeviceWidget::AiDeviceWidget(QWidget* parent) : QWidget(parent)
{
    // 标题栏默认不显示
    setWindowFlags(Qt::FramelessWindowHint);

    setGeometry(0, 0, LCD_WIDTH - 20, LCD_HEIGHT - 60);
    setFixedSize(LCD_WIDTH - 20, LCD_HEIGHT - 60);
    this->setAutoFillBackground(true);

    QPalette palette;
    QPixmap pixmap(":/res/png/back_deep_clear.png");
    pixmap = pixmap.scaled(this->size());
    palette.setBrush(backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);


    // 左侧ListWidet控件
    listWidget = new myListWidget();
    listWidget->setViewMode(QListView::ListMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setFixedWidth(BTN_WIDTH);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setUniformItemSizes(true);   // 所有行高度一致
    listWidget->setSpacing(10);
    /* 与算法设置页左侧列表统一的线框卡片风 */
    listWidget->setStyleSheet(
        "QListWidget {"
        "  background: transparent;"
        "  border: none;"
        "  border-image: url(:/res/png2/sidelist/list_panel.png) 12 12 12 12 stretch stretch;"
        "  padding: 10px 10px;"
        "  outline: none;"
        "}"
        "QListWidget::item {"
        "  background-color: rgba(13, 34, 50, 0.55);"
        "  border: 1px solid #2A5B6E;"
        "  border-radius: 8px;"
        "  color: #DCEEF5;"
        "  font-size: 12px;"
        "  padding: 0px 10px;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: rgba(24, 54, 74, 0.72);"
        "  border: 1px solid #3FA9BF;"
        "  color: #FFFFFF;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: rgba(20, 58, 76, 0.75);"
        "  border: 1px solid #45D2E4;"
        "  color: #FFFFFF;"
        "}"
        "QScrollBar:vertical { width: 6px; background: transparent; margin: 6px 0; }"
        "QScrollBar::handle:vertical { background: #2A6076; border-radius: 3px; min-height: 40px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }");
    listWidget->setFixedSize(LCD_WIDTH / 5, LCD_HEIGHT - 200);


    // 右侧StackedWidet控件
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedSize(700, LCD_HEIGHT - 150);

    // 第一个widget是默认参数Widget,即图像采集配置页面
    AiImageAcquisitionWidget* imgAcqWidget = new AiImageAcquisitionWidget();
    stackedWidget->addWidget(imgAcqWidget);

    QFont listItemFont = font();
    listItemFont.setPixelSize(12);

    myListWidgetItem* item1 = new myListWidgetItem(QString(""), QIcon(), QSize());
    item1->setText(myLan.default_params);
    item1->setSizeHint(QSize(170, 56));
    item1->setFont(listItemFont);
    listWidget->addItem(item1);

    // 第二个widget是磁盘信息Widget
    AiDiskInfoWidget* diskWidget = new AiDiskInfoWidget();
    stackedWidget->addWidget(diskWidget);

    myListWidgetItem* item2 = new myListWidgetItem(QString(""), QIcon(), QSize());
    item2->setText(myLan.disk_space);
    item2->setSizeHint(QSize(170, 56));
    item2->setFont(listItemFont);
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

    progressDlg = new ImageDownloadDialog(this);
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

    /* 选中/切页立即生效；页面数据刷新推迟到下一次事件循环，
       避免磁盘查询(同步 UDP 等待)阻塞在点击事件中，导致选中态不刷新、看起来需要点两次 */
    QWidget* page = stackedWidget->currentWidget();
    switch (idx)
    {
    case 0:
    {
        // 对应图像下载配置页面
        auto* acqWidget = dynamic_cast<AiImageAcquisitionWidget*>(page);
        if (!acqWidget)
        {
            LOG_ERROR_STM("idx:" << idx << " not AiImageAcquisitionWidget!");
            return;
        }
        QTimer::singleShot(0, acqWidget, [acqWidget]() { acqWidget->onRefresh(); });
    }

    break;
    case 1:
    {
        // 对应磁盘信息页面
        auto* diskWidget = dynamic_cast<AiDiskInfoWidget*>(page);
        if (!diskWidget)
        {
            LOG_ERROR_STM("idx:" << idx << " not AiDiskInfoWidget!");
            return;
        }
        QTimer::singleShot(0, diskWidget, [diskWidget]() { diskWidget->onRefresh(); });
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
    downProgressArr[idx].success_ = success;

    progressDlg->setBoardFinished(idx, success);

    bool all_finished = true;
    bool all_success = true;
    for (int idx = 0; idx < AI_SFTP_DEV_NUM; idx++)
    {
        if (!downProgressArr[idx].finished_)
        {
            all_finished = false;
            break;
        }

        all_success = all_success && downProgressArr[idx].success_;
    }

    LOG_INFO_STM("idx :" << idx << ", success:" << success << ", all finished:" << all_finished << ", all success:" << all_success);

    if (all_finished)
    {
        progressDlg->showResult(all_success);

        QTimer::singleShot(3000, progressDlg, [this]() {
            progressDlg->hide();
            });

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
    progressDlg->setBoardProgress(idx, current, total);
    progressDlg->setOverall(value);
}

void AiDeviceWidget::onDownBtnPressed()
{
    QString img_dir = ai_helper::GetAcqImgRootPath();
    QStorageInfo storage(img_dir);
    if (storage.isValid() && storage.isReady())
    {
        qint64 total = storage.bytesTotal() / SPACEMB;
        qint64 available = storage.bytesAvailable() / SPACEMB;
        qint64 free = storage.bytesFree() / SPACEMB;
        LOG_INFO_STM("path:" << img_dir.toStdString() << " total:" << total
            << " MB, availabel:" << available << " MB, free:" << free << " MB");
        if (free < MIN_DOWN_SPACITY)
        {
            LOG_ERROR_STM("path:" << img_dir.toStdString() << " spacity[" << free << "] MB < " << MIN_DOWN_SPACITY << " MB");
            QMessageBox::warning(this, myLan.app_warn, myLan.disk_space_insufficent);
            return;
        }
    }

    // 下载操作
    for (int idx = 0; idx < AI_SFTP_DEV_NUM; idx++)
    {
        downProgressArr[idx].idx_ = idx;
        downProgressArr[idx].current_num_ = 0;
        downProgressArr[idx].total_num_ = 0;
        downProgressArr[idx].finished_ = false;
    }

    LOG_INFO_STM("onDownBtnPressed start download image...");

    progressDlg->reset();
    progressDlg->setOverall(0);
    progressDlg->show();   // 调试时屏蔽下载进度弹窗，后台下载仍进行
    progressDlg->raise();
    QString downDir = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString downPath = img_dir + "/" + downDir;
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
