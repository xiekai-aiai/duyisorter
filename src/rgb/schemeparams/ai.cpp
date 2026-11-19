#include "ai.h"
#include "unilog.h"
#include "sqlitemgr.h"
#include "aihelper.h"
#include "cmdworker.h"
#include "cmdudpmanager.h"

#include <QDialog>
#include <QProgressBar>
#include <QScrollArea>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QCloseEvent>
#include <QTimer>
#include <atomic>
#include <functional>
#include <thread>
#include "sftp_client.h"

namespace {

QString muFormatSize(long long bytes)
{
    if (bytes < 0)
    {
        return "0 B";
    }
    const double kb = 1024.0;
    const double mb = kb * 1024.0;
    if (bytes >= static_cast<long long>(mb))
    {
        return QString::number(bytes / mb, 'f', 1) + " MB";
    }
    if (bytes >= static_cast<long long>(kb))
    {
        return QString::number(bytes / kb, 'f', 0) + " KB";
    }
    return QString::number(bytes) + " B";
}

// 板卡行控件集合
struct MuRowUi
{
    QLabel* dot = nullptr;
    QLabel* ip = nullptr;
    QLabel* stage = nullptr;
    QProgressBar* bar = nullptr;
    QLabel* right = nullptr;
};

/**
 * @brief 模型上传进度对话框（无 Q_OBJECT，仅作 UI 容器）
 *        上传线程通过 QTimer::singleShot(0, dlg, ...) 把更新投递回 UI 线程
 */
class ModelUploadDialog : public QDialog
{
public:
    ModelUploadDialog(QWidget* parent, const QString& modelName, const QStringList& ips)
        : QDialog(parent)
    {
        setWindowTitle("模型上传");
        setModal(true);
        setFixedWidth(780);
        setWindowFlags((windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
                       & ~Qt::WindowContextHelpButtonHint);
        setStyleSheet(
            "QDialog { background:#0c161c; }"
            "QLabel { color:#cfe3ea; font-size:13px; }"
            "QProgressBar { background:#0a141a; border:1px solid #16303b; border-radius:5px; color:transparent; }"
            "QProgressBar::chunk { background:#22d3ee; border-radius:4px; }"
            "QCheckBox { color:#6b8794; font-size:12px; }"
            "QPushButton { background:#0d1a21; border:1px solid #1e3a4a; color:#cfe3ea;"
            "  padding:8px 22px; border-radius:4px; font-size:13px; }"
            "QPushButton:hover { border-color:#155e6b; color:#22d3ee; }"
            "QPushButton:disabled { color:#3f5a66; }"
            "QScrollArea { background:transparent; border:none; }");

        // ── 标题 ──
        m_title = new QLabel("正在上传模型", this);
        QFont tf = m_title->font();
        tf.setPixelSize(17);
        tf.setBold(true);
        m_title->setFont(tf);
        m_title->setStyleSheet("color:#e6f6fa;");

        m_sub = new QLabel("AI 板卡逐个上传中，请勿断电或退出", this);
        m_sub->setStyleSheet("color:#6b8794;font-size:12px;");

        QHBoxLayout* titleLayout = new QHBoxLayout;
        titleLayout->addWidget(m_title);
        titleLayout->addStretch();
        titleLayout->addWidget(m_sub);

        // ── 模型信息 ──
        QLabel* modelKey = new QLabel("模型", this);
        modelKey->setStyleSheet("color:#6b8794;");
        QLabel* modelVal = new QLabel(modelName, this);
        modelVal->setStyleSheet("color:#22d3ee;font-family:Consolas,monospace;font-size:14px;");
        QHBoxLayout* modelLayout = new QHBoxLayout;
        modelLayout->addWidget(modelKey);
        modelLayout->addWidget(modelVal);
        modelLayout->addStretch();

        // ── 总进度 ──
        m_overallText = new QLabel(QString("总进度 · 0 / %1 块板").arg(ips.size()), this);
        m_overallText->setStyleSheet("color:#6b8794;font-size:12px;");
        m_overallPct = new QLabel("0%", this);
        m_overallPct->setStyleSheet("color:#22d3ee;font-size:15px;font-weight:600;");

        QHBoxLayout* overallTop = new QHBoxLayout;
        overallTop->addWidget(m_overallText);
        overallTop->addStretch();
        overallTop->addWidget(m_overallPct);

        m_overallBar = new QProgressBar(this);
        m_overallBar->setRange(0, 100);
        m_overallBar->setValue(0);
        m_overallBar->setTextVisible(false);
        m_overallBar->setFixedHeight(10);

        m_overallInfo = new QLabel("", this);
        m_overallInfo->setStyleSheet("color:#3f5a66;font-size:12px;");

        // ── 统计 chips ──
        m_chipOk = new QLabel("● 成功 0", this);
        m_chipOk->setStyleSheet("color:#34d399;font-size:12px;");
        m_chipSkip = new QLabel("● 跳过 0", this);
        m_chipSkip->setStyleSheet("color:#6b8794;font-size:12px;");
        m_chipFail = new QLabel("● 失败 0", this);
        m_chipFail->setStyleSheet("color:#f87171;font-size:12px;");
        QHBoxLayout* chips = new QHBoxLayout;
        chips->addWidget(m_chipOk);
        chips->addSpacing(18);
        chips->addWidget(m_chipSkip);
        chips->addSpacing(18);
        chips->addWidget(m_chipFail);
        chips->addStretch();

        // ── 板卡列表 ──
        QWidget* rows = new QWidget;
        m_rowsLayout = new QVBoxLayout(rows);
        m_rowsLayout->setContentsMargins(0, 0, 0, 0);
        m_rowsLayout->setSpacing(0);

        for (const QString& ip : ips)
        {
            QWidget* row = new QWidget(rows);
            QHBoxLayout* hl = new QHBoxLayout(row);
            hl->setContentsMargins(16, 10, 16, 10);
            hl->setSpacing(12);

            MuRowUi ui;
            ui.dot = new QLabel(row);
            ui.dot->setFixedSize(10, 10);
            ui.dot->setStyleSheet("background:#2c424e;border-radius:5px;");
            ui.ip = new QLabel(ip, row);
            ui.ip->setFixedWidth(150);
            ui.ip->setStyleSheet("font-family:Consolas,monospace;font-size:13px;");
            ui.stage = new QLabel("等待中", row);
            ui.stage->setStyleSheet("color:#3f5a66;font-size:12px;");
            ui.bar = new QProgressBar(row);
            ui.bar->setRange(0, 100);
            ui.bar->setValue(0);
            ui.bar->setTextVisible(false);
            ui.bar->setFixedSize(150, 6);
            ui.right = new QLabel("—", row);
            ui.right->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui.right->setFixedWidth(140);
            ui.right->setStyleSheet("color:#6b8794;font-size:12px;");

            hl->addWidget(ui.dot);
            hl->addWidget(ui.ip);
            hl->addWidget(ui.stage, 1);
            hl->addWidget(ui.bar);
            hl->addWidget(ui.right);

            m_rowsLayout->addWidget(row);
            m_rows.insert(ip, ui);
        }
        m_rowsLayout->addStretch();

        QScrollArea* scroll = new QScrollArea(this);
        scroll->setWidgetResizable(true);
        scroll->setWidget(rows);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setFixedHeight(qMin(320, 46 * ips.size() + 6));

        // ── 底部按钮 ──
        m_hint = new QLabel("取消后，已上传完成的板卡不会回滚", this);
        m_hint->setStyleSheet("color:#3f5a66;font-size:11px;");

        m_force = new QCheckBox("强制重新上传（忽略检测结果）", this);
        connect(m_force, &QCheckBox::toggled, this, [this](bool checked) {
            m_forceFlag.store(checked);
        });

        m_cancelBtn = new QPushButton("取消上传", this);
        m_cancelBtn->setStyleSheet("QPushButton { border-color:#5b2a2a; color:#f87171;"
                                   " background:rgba(248,113,113,0.06); }");
        connect(m_cancelBtn, &QPushButton::clicked, this, [this] {
            m_cancelFlag.store(true);
            m_cancelBtn->setEnabled(false);
            m_hint->setText("正在取消，请稍候…");
        });

        m_closeBtn = new QPushButton("关闭", this);
        m_closeBtn->hide();
        connect(m_closeBtn, &QPushButton::clicked, this, [this] { close(); });

        m_retryBtn = new QPushButton("重试失败项", this);
        m_retryBtn->setStyleSheet("QPushButton { border-color:#155e6b; color:#22d3ee;"
                                  " background:rgba(34,211,238,0.08); }");
        m_retryBtn->hide();
        connect(m_retryBtn, &QPushButton::clicked, this, [this] {
            m_retryRequested = true;
            close();
        });

        QHBoxLayout* footer = new QHBoxLayout;
        footer->addWidget(m_hint);
        footer->addStretch();
        footer->addWidget(m_force);
        footer->addWidget(m_cancelBtn);
        footer->addWidget(m_closeBtn);
        footer->addWidget(m_retryBtn);

        QVBoxLayout* main = new QVBoxLayout(this);
        main->setContentsMargins(22, 18, 22, 18);
        main->setSpacing(12);
        main->addLayout(titleLayout);
        main->addLayout(modelLayout);
        main->addLayout(overallTop);
        main->addWidget(m_overallBar);
        main->addWidget(m_overallInfo);
        main->addLayout(chips);
        main->addWidget(scroll);
        main->addLayout(footer);
    }

    // ── 以下方法只允许在 UI 线程调用 ──
    void updateStage(const QString& ip, const QString& text, int kind)
    {
        auto it = m_rows.find(ip);
        if (it == m_rows.end())
        {
            return;
        }
        static const char* colors[] = { "#3f5a66", "#22d3ee", "#34d399", "#f87171" };
        it.value().stage->setText(text);
        it.value().stage->setStyleSheet(QString("color:%1;font-size:12px;")
                                        .arg(colors[qBound(0, kind, 3)]));
        if (kind == 1)
        {
            it.value().dot->setStyleSheet("background:#22d3ee;border-radius:5px;");
        }
    }

    void updateBoardProgress(const QString& ip, int pct, const QString& rightText)
    {
        auto it = m_rows.find(ip);
        if (it == m_rows.end())
        {
            return;
        }
        it.value().bar->setValue(qBound(0, pct, 100));
        it.value().right->setText(rightText);
    }

    void updateOverall(int pct, const QString& info)
    {
        m_overallBar->setValue(qBound(0, pct, 100));
        m_overallPct->setText(QString("%1%").arg(qBound(0, pct, 100)));
        if (!info.isEmpty())
        {
            m_overallInfo->setText(info);
        }
    }

    // result: 0=成功 1=跳过 2=失败
    void markBoardDone(const QString& ip, int result, const QString& msg)
    {
        auto it = m_rows.find(ip);
        if (it == m_rows.end())
        {
            return;
        }
        MuRowUi& ui = it.value();
        if (result == 2)
        {
            ++m_failCount;
            ui.dot->setStyleSheet("background:#f87171;border-radius:5px;");
            ui.stage->setText(msg);
            ui.stage->setStyleSheet("color:#f87171;font-size:12px;");
            ui.right->setText("失败");
            ui.right->setStyleSheet("color:#f87171;font-size:12px;");
        }
        else
        {
            if (result == 0)
            {
                ++m_okCount;
            }
            else
            {
                ++m_skipCount;
            }
            ui.dot->setStyleSheet("background:#34d399;border-radius:5px;");
            ui.bar->setValue(100);
            ui.stage->setText(result == 0 ? "完成" : "已存在，跳过");
            ui.stage->setStyleSheet("color:#34d399;font-size:12px;");
            ui.right->setText(result == 0 ? "上传完成" : "校验一致");
            ui.right->setStyleSheet("color:#34d399;font-size:12px;");
        }
        refreshCounts();
    }

    void onAllFinished(int okCount, int skipCount, int failCount,
                       const QStringList& failedIps, bool cancelled)
    {
        m_okCount = okCount;
        m_skipCount = skipCount;
        m_failCount = failCount;
        m_failedIps = failedIps;
        m_cancelled = cancelled;
        m_busy = false;
        refreshCounts();

        m_force->hide();
        m_cancelBtn->hide();
        m_closeBtn->show();
        if (failCount > 0 && !cancelled)
        {
            m_retryBtn->show();
        }

        if (cancelled)
        {
            m_title->setText("模型上传已取消");
            m_sub->setText("已完成的板卡不受影响");
            m_hint->setText(QString("已完成 %1 块，失败 %2 块")
                            .arg(okCount + skipCount).arg(failCount));
            for (auto it = m_rows.begin(); it != m_rows.end(); ++it)
            {
                if (it.value().stage->text() == "等待中")
                {
                    it.value().stage->setText("已取消");
                }
            }
        }
        else
        {
            m_title->setText("模型上传完成");
            m_sub->setText(failCount > 0 ? "存在失败项，请检查网络后重试" : "全部板卡处理完成");
            m_hint->setText(QString("成功 %1 块，跳过 %2 块，失败 %3 块%4")
                            .arg(okCount).arg(skipCount).arg(failCount)
                            .arg(failedIps.isEmpty() ? "" : "（" + failedIps.join(", ") + "）"));
            m_overallBar->setValue(100);
            m_overallPct->setText("100%");
            m_overallInfo->setText("");
        }
    }

    bool wasCancelled() const { return m_cancelled; }
    QStringList failedIps() const { return m_failedIps; }
    bool retryRequested() const { return m_retryRequested; }

    std::atomic<bool> m_cancelFlag{ false };
    std::atomic<bool> m_forceFlag{ false };

protected:
    // Esc 键会走 reject()，上传中同样转为取消请求，避免对话框被提前销毁
    void reject() override
    {
        if (m_busy)
        {
            m_cancelFlag.store(true);
            m_hint->setText("正在取消，请稍候…");
            return;
        }
        QDialog::reject();
    }

    void closeEvent(QCloseEvent* event) override
    {
        if (m_busy)
        {
            // 上传进行中不允许直接关窗，转为取消请求
            m_cancelFlag.store(true);
            m_hint->setText("正在取消，请稍候…");
            event->ignore();
            return;
        }
        QDialog::closeEvent(event);
    }

private:
    void refreshCounts()
    {
        m_chipOk->setText(QString("● 成功 %1").arg(m_okCount));
        m_chipSkip->setText(QString("● 跳过 %1").arg(m_skipCount));
        m_chipFail->setText(QString("● 失败 %1").arg(m_failCount));
        m_overallText->setText(QString("总进度 · %1 / %2 块板")
                               .arg(m_okCount + m_skipCount + m_failCount)
                               .arg(m_rows.size()));
    }

    QLabel* m_title = nullptr;
    QLabel* m_sub = nullptr;
    QLabel* m_overallText = nullptr;
    QLabel* m_overallPct = nullptr;
    QLabel* m_overallInfo = nullptr;
    QProgressBar* m_overallBar = nullptr;
    QLabel* m_chipOk = nullptr;
    QLabel* m_chipSkip = nullptr;
    QLabel* m_chipFail = nullptr;
    QVBoxLayout* m_rowsLayout = nullptr;
    QLabel* m_hint = nullptr;
    QCheckBox* m_force = nullptr;
    QPushButton* m_cancelBtn = nullptr;
    QPushButton* m_closeBtn = nullptr;
    QPushButton* m_retryBtn = nullptr;

    QMap<QString, MuRowUi> m_rows;
    int m_okCount{ 0 };
    int m_skipCount{ 0 };
    int m_failCount{ 0 };
    bool m_cancelled{ false };
    bool m_busy{ true };
    bool m_retryRequested{ false };
    QStringList m_failedIps;
};

// 线程安全的 UI 更新投递
void muPost(ModelUploadDialog* dlg, std::function<void()> fn)
{
    QTimer::singleShot(0, dlg, fn);
}

// 读取 AI 板远程文件 md5（失败返回空串）
QString muRemoteMd5(SftpClient& cli, const QString& remotePath)
{
    std::string output = cli.exec("md5sum '" + remotePath.toStdString() + "' 2>/dev/null");
    QString text = QString::fromStdString(output).trimmed();
    QRegularExpression re("[0-9a-fA-F]{32}");
    QRegularExpressionMatch match = re.match(text);
    return match.hasMatch() ? match.captured(0).toLower() : QString();
}

/**
 * @brief 上传线程：按 IP 顺序逐块板检测 + 上传
 */
void muRunUpload(ModelUploadDialog* dlg, QString binPath, QString jsonPath, QStringList ips)
{
    const QString binName = QFileInfo(binPath).fileName();
    const QString jsonName = QFileInfo(jsonPath).fileName();
    const QString remoteBin = QString(AI_DEV_MODEL_PATH) + binName;
    const QString remoteJson = QString(AI_DEV_MODEL_PATH) + jsonName;

    const long long binSize = QFileInfo(binPath).size();
    const bool jsonAvailable = QFile::exists(jsonPath);
    const long long jsonSize = jsonAvailable ? QFileInfo(jsonPath).size() : 0;

    QByteArray localBinMd5;
    {
        QFile file(binPath);
        if (file.open(QIODevice::ReadOnly))
        {
            localBinMd5 = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
            file.close();
        }
    }

    const int total = ips.size();
    int okCount = 0;
    int skipCount = 0;
    int failCount = 0;
    double finished = 0.0;
    QStringList failed;

    for (int i = 0; i < total; ++i)
    {
        if (dlg->m_cancelFlag.load())
        {
            break;
        }

        const QString ip = ips.at(i);
        muPost(dlg, [dlg, ip] { dlg->updateStage(ip, "连接中…", 1); });
        muPost(dlg, [dlg, i, total, ip] {
            dlg->updateOverall(total > 0 ? int(i * 100.0 / total) : 0, "当前：" + ip);
        });

        SftpClient cli(ip.toStdString(), 22, AI_DEV_USER, AI_DEV_PWD);
        if (!cli.connect())
        {
            ++failCount;
            ++finished;
            failed.append(ip);
            muPost(dlg, [dlg, ip] { dlg->markBoardDone(ip, 2, "连接失败"); });
            continue;
        }

        bool needBin = true;
        bool needJson = jsonAvailable;
        bool binExists = false;

        if (!dlg->m_forceFlag.load())
        {
            muPost(dlg, [dlg, ip] { dlg->updateStage(ip, "检测中…", 1); });

            LIBSSH2_SFTP_ATTRIBUTES attrs;
            binExists = cli.stat_file(remoteBin.toStdString(), attrs);
            if (binExists)
            {
                QString remoteMd5 = muRemoteMd5(cli, remoteBin);
                if (!remoteMd5.isEmpty() && !localBinMd5.isEmpty())
                {
                    needBin = (remoteMd5 != QString::fromLatin1(localBinMd5));
                }
                else
                {
                    needBin = (static_cast<long long>(attrs.filesize) != binSize);
                }
            }

            if (jsonAvailable)
            {
                bool jsonExists = cli.stat_file(remoteJson.toStdString(), attrs);
                needJson = (!jsonExists || static_cast<long long>(attrs.filesize) != jsonSize);
            }
        }

        if (!needBin && !needJson)
        {
            ++skipCount;
            ++finished;
            muPost(dlg, [dlg, ip] { dlg->markBoardDone(ip, 1, "已存在，跳过"); });
            continue;
        }

        if (!needBin)
        {
            muPost(dlg, [dlg, ip] { dlg->updateStage(ip, "模型一致，仅更新 json", 1); });
        }
        else if (binExists)
        {
            muPost(dlg, [dlg, ip] { dlg->updateStage(ip, "已存在但校验不一致，重新上传", 1); });
        }

        bool boardOk = true;
        QString boardErr = "上传失败";

        if (needBin)
        {
            muPost(dlg, [dlg, ip] { dlg->updateStage(ip, "上传 model.bin", 1); });
            int lastPct = -1;
            bool uploadOk = cli.upload(binPath.toStdString(), remoteBin.toStdString(),
                [dlg, ip, &lastPct, &finished, total](long long sent, long long totalBytes) -> bool {
                    if (dlg->m_cancelFlag.load())
                    {
                        return false;
                    }
                    int pct = totalBytes > 0 ? static_cast<int>(sent * 95 / totalBytes) : 95;
                    if (pct != lastPct)
                    {
                        lastPct = pct;
                        QString right = QString("%1 / %2")
                                        .arg(muFormatSize(sent), muFormatSize(totalBytes));
                        muPost(dlg, [dlg, ip, pct, right] {
                            dlg->updateBoardProgress(ip, pct, right);
                            dlg->updateStage(ip, QString("上传 model.bin %1%").arg(pct), 1);
                        });
                        double frac = totalBytes > 0
                                      ? static_cast<double>(sent) / static_cast<double>(totalBytes)
                                      : 1.0;
                        int overall = static_cast<int>((finished + frac * 0.98) * 100.0 / total);
                        muPost(dlg, [dlg, overall, ip] {
                            dlg->updateOverall(qBound(0, overall, 100), "当前：" + ip);
                        });
                    }
                    return true;
                });

            if (!uploadOk)
            {
                boardOk = false;
                boardErr = dlg->m_cancelFlag.load() ? "已取消" : "上传 bin 失败";
            }
        }

        if (boardOk && needJson)
        {
            muPost(dlg, [dlg, ip] { dlg->updateStage(ip, "上传 json", 1); });
            bool uploadOk = cli.upload(jsonPath.toStdString(), remoteJson.toStdString(),
                [dlg](long long, long long) -> bool {
                    return !dlg->m_cancelFlag.load();
                });
            if (!uploadOk)
            {
                boardOk = false;
                boardErr = dlg->m_cancelFlag.load() ? "已取消" : "上传 json 失败";
            }
        }

        cli.disconnect();

        ++finished;
        if (boardOk)
        {
            ++okCount;
            muPost(dlg, [dlg, ip] {
                dlg->updateBoardProgress(ip, 100, "完成");
                dlg->markBoardDone(ip, 0, "完成");
            });
        }
        else
        {
            ++failCount;
            failed.append(ip);
            muPost(dlg, [dlg, ip, boardErr] { dlg->markBoardDone(ip, 2, boardErr); });
        }
    }

    bool cancelled = dlg->m_cancelFlag.load();
    int finalPct = cancelled ? static_cast<int>(finished * 100.0 / total) : 100;
    muPost(dlg, [dlg, finalPct, cancelled] {
        dlg->updateOverall(finalPct, cancelled ? "" : "全部完成");
    });
    muPost(dlg, [dlg, okCount, skipCount, failCount, failed, cancelled] {
        dlg->onAllFinished(okCount, skipCount, failCount, failed, cancelled);
    });
}

} // namespace

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
        QMessageBox::warning(this, myLan.app_warn, myLan.mdl_upload_failed);
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

    for (int idx = 0; idx < struCnfg.nLevelTotal; idx++)
    {
        for (int idy = 0; idy < struCnfg.struLevelInfo[idx].nIdentifyGroupTotal; idy++)
        {
            if (model_id == QString::fromUtf8(struCnfp.struGroupIdentify[idx][idy].struAi.modelId))
            {
                LOG_INFO_STM("level id:" << idx << ", identify group id:" << idy << " use model id:" << model_id.toStdString() << ", model name:" << model_name.toStdString() << " cannot delete!");
                QMessageBox::warning(this, myLan.app_warn, myLan.mdl_applying + myLan.mdl_del_failed);
                return;
            }
        }
    }

    ModelInfo info;
    if (!SQLiteMgr::Instance().LoadModelInfoById(model_id, info))
    {
        return;
    }

    if (!modelDelete(info))
    {
        QMessageBox::warning(this, myLan.app_warn, myLan.mdl_del_failed);
        return;
    }

    SQLiteMgr::Instance().DelModelInfo(model_id);
    SQLiteMgr::Instance().DelModelClsParam(model_id);

    emit pageUpdated();
}

/* 应用当前智能参数固化页面设置 */
bool PageAi::setIntelParams()
{
    if (tabBar->currentIndex() < 0)
    {
        LOG_ERROR_STM("tarBar currentIndex [" << tabBar->currentIndex() << "] < 0");
        return false;
    }

    // 点击应用时，处理模型应用 
    LOG_INFO_STM("save page ai params! count:" << modelListWidget->count());
    if (modelListWidget->currentItem() == NULL)
    {
        LOG_INFO_STM("modelListWidget current item is null");
        return false;
    }

    QString model_name = modelListWidget->currentItem()->text();
    QString model_id = modelListWidget->currentItem()->data(Qt::UserRole).toString();
    LOG_INFO_STM("modelListWidget current item text : " << model_name.toStdString()
        << ", model id:" << model_id.toStdString());

    ModelInfo info;
    if (!SQLiteMgr::Instance().LoadModelInfoById(model_id, info))
    {
        QMessageBox::warning(this, myLan.app_warn, myLan.mdl_load_failed);
        return false;
    }

    if (!modelUpload(info))
    {
        QMessageBox::warning(this, myLan.app_warn, myLan.mdl_upload_failed);
        return false;
    }

    if (!modelApply(info))
    {
        QMessageBox::warning(this, myLan.app_warn, myLan.mdl_apply_failed);
        return false;
    }

    SQLiteMgr::Instance().UpdateModelInfoUploadFlag(model_id, true);

    // 更新方案里的AI模型ID
    const char* charArray = model_id.toUtf8().constData();
    strcpy(struCnfp.struGroupIdentify[struGsh.nLevel][tabBar->currentIndex()].struAi.modelId, charArray);

    myFlow.materialCopyAssemble(struGsh.nLevel, tabBar->currentIndex(), 0, ARITH_PISTACHIO, 0);
    myFlow.materialResetGroupAssemble(struGsh.nLevel, tabBar->currentIndex(), 0, ARITH_PISTACHIO, 0);

    LOG_INFO_STM("modelListWidget current item text : " << model_name.toStdString()
        << ", model id:" << model_id.toStdString() << " apply model successful! tabBar currentIndex:" << tabBar->currentIndex());
    return true;
}

/* 重置当前智能参数固化页面设置 */
void PageAi::resetIntelParams()
{
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
    LOG_INFO_STM("current model name:" << model_name.toStdString() << ", model id:" << model_id.toStdString());

}


void PageAi::updateListWidget()
{
    LOG_INFO_STM("AI updateListWidget: currentIndex:" << tabBar->currentIndex());
    if (tabBar->currentIndex() < 0)
    {
        return;
    }

    modelListWidget->clear();

    QVector<ModelInfo> model_infos;
    if (!SQLiteMgr::Instance().LoadAllModeInfo(model_infos))
    {
        return;
    }

    int tarCurIdx = tabBar->currentIndex();
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

        if (item.model_id_ == QString::fromUtf8(struCnfp.struGroupIdentify[struGsh.nLevel][tarCurIdx].struAi.modelId))
        {
            LOG_INFO_STM("current level:" << struGsh.nLevel << ", current tar idx:" << tarCurIdx << ", model id:" << item.model_id_.toStdString());
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

    QString binPath = ai_helper::GetModelRootPath() + "/" + info.model_name_;
    QString jsonPath = ai_helper::GetModelRootPath() + "/" + name + ".json";

    if (!QFile::exists(binPath))
    {
        LOG_ERROR_STM("model file not exist: " << binPath.toStdString());
        QMessageBox::warning(this, myLan.app_warn,
            QString("模型文件不存在：\n%1").arg(binPath));
        return false;
    }

    QStringList ips = getAiIpSet(tabBar).values();
    ips.sort();   // QSet 无序，排序后保证逐板上传顺序稳定
    if (ips.isEmpty())
    {
        LOG_ERROR_STM("no ai board ip found, tab index:" << tabBar->currentIndex());
        return false;
    }

    LOG_INFO_STM("model upload start, model:" << info.model_name_.toStdString()
        << ", board count:" << ips.size());

    // 逐轮上传：失败后若用户点「重试失败项」，仅重传失败的板卡
    QStringList todo = ips;
    bool anyFailed = false;
    bool cancelled = false;

    while (!todo.isEmpty())
    {
        ModelUploadDialog dialog(this, info.model_name_, todo);
        std::thread uploadThread(muRunUpload, &dialog, binPath, jsonPath, todo);
        dialog.exec();
        if (uploadThread.joinable())
        {
            uploadThread.join();
        }

        if (dialog.wasCancelled())
        {
            cancelled = true;
            anyFailed = true;
            break;
        }

        if (dialog.failedIps().isEmpty())
        {
            break;
        }

        anyFailed = true;
        if (dialog.retryRequested())
        {
            todo = dialog.failedIps();
            continue;
        }
        break;
    }

    LOG_INFO_STM("model upload finished, model:" << info.model_name_.toStdString()
        << ", cancelled:" << cancelled << ", anyFailed:" << anyFailed);

    return (!cancelled && !anyFailed);
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

    // 遍历每一层
    for (int idx = 0; idx < struCnfg.nLevelTotal; idx++)
    {
        // 遍历层的每一个相机
        for (int idy = 0; idy < struCnfg.struLevelInfo[idx].nUnitLevelTotal; idy++)
        {
            int cam_id = struCnfg.struLevelInfo[idx].nUnitId[idy];
            QString ip = ai_helper::GetAiIpByIndex(cam_id);
            auto it = sftp_worker_map.find(ip);
            if (sftp_worker_map.end() == it)
            {
                SftpWorker* worker = new SftpWorker(ip, AI_DEV_USER, AI_DEV_PWD);
                it = sftp_worker_map.insert(ip, worker);
            }

            bool ret = it.value()->onDeleteFilesSync(del_lists);
            LOG_INFO_STM("delete file from [" << ip.toStdString() << "] result:" << ret << ", cam id:" << cam_id);
            if (!ret)
            {
                return ret;
            }
        }

    }

    return true;
}

bool PageAi::modelApply(const ModelInfo& info)
{
    LOG_INFO_STM("model apply:" << info.model_name_.toStdString());

    ModelApply model_apply;
    model_apply.model_name_ = info.model_name_;
    QByteArray request = cmdworker::ModelApplyRequest(model_apply);

    QSet<QString> ipSet = getAiIpSet(tabBar);
    for (const QString& ip : ipSet)
    {
        QByteArray response;
        bool ok = CmdUdpManager::instance().onSendCommand(QHostAddress(ip), AI_UPD_CMD_PORT, request,
            response, AI_RESPONSE_TIMEOUT);

        if (!ok)
        {
            LOG_ERROR_STM("model apply ip:" << ip.toStdString() << " send command failed! requst body:" << request.toHex(' ').toUpper().toStdString());
            return false;
            continue;
        }

        CmdPackage cmd_pkg;
        ok = cmdworker::ParseCmdPkg(response, cmd_pkg);
        if (!ok)
        {
            LOG_ERROR_STM("model apply ip:" << ip.toStdString() << " parse resonpse failed! request body:" << request.toHex(' ').toUpper().toStdString()
                << ", response body:" << response.toHex(' ').toUpper().toStdString());
            return false;
        }

        int code = cmdworker::CommResponse(cmd_pkg).code_;

        if (AI_RESPONSE_SUCCESS != code)
        {
            LOG_ERROR_STM("model apply  ip:" << ip.toStdString() << " send command:" << request.toHex(' ').toUpper().toStdString() << ", response:"
                << response.toHex(' ').toUpper().toStdString() << ", code:" << code);
            return false;
        }

        LOG_INFO_STM("model apply  ip:" << ip.toStdString() << " send command:" << request.toHex(' ').toUpper().toStdString() << ", response:"
            << response.toHex(' ').toUpper().toStdString() << ", code:" << code);
    }

    return true;
}
