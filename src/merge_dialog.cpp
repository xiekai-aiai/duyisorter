// merge_dialog.cpp — 合并标注目录对话框（同步阻塞 + 文本进度）
#include "merge_dialog.h"
#include "mergeworker.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QButtonGroup>
#include <QFrame>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <functional>

MergeDialog::MergeDialog(QWidget* parent)
    : QDialog(parent)
{
    initUi();
    setWindowModality(Qt::ApplicationModal);
    onAddSourceClicked();  // 默认加一行
}

MergeDialog::~MergeDialog() {}

void MergeDialog::initUi()
{
    setWindowTitle("合并标注目录");
    setMinimumSize(700, 480);
    resize(800, 560);

    auto* mainLayout = new QVBoxLayout(this);

    // ── 源目录组 ──────────────────────────────────────────────
    auto* srcGroup = new QGroupBox("源目录（最多 10 个）", this);
    auto* srcLayout = new QVBoxLayout(srcGroup);

    m_srcScroll = new QScrollArea(this);
    m_srcScroll->setWidgetResizable(true);
    m_srcScroll->setFrameShape(QFrame::NoFrame);
    m_srcScroll->setMinimumHeight(120);
    m_srcContainer = new QWidget(m_srcScroll);
    m_srcListLayout = new QVBoxLayout(m_srcContainer);
    m_srcListLayout->setContentsMargins(0, 0, 0, 0);
    m_srcListLayout->setSpacing(4);
    m_srcListLayout->addStretch();
    m_srcScroll->setWidget(m_srcContainer);
    srcLayout->addWidget(m_srcScroll);

    auto* btnRow = new QHBoxLayout();
    m_btnAddSource = new QPushButton("+ 添加源目录", this);
    connect(m_btnAddSource, &QPushButton::clicked, this, &MergeDialog::onAddSourceClicked);
    btnRow->addWidget(m_btnAddSource);
    btnRow->addStretch();
    srcLayout->addLayout(btnRow);
    mainLayout->addWidget(srcGroup);

    // ── 目标目录组 ────────────────────────────────────────────
    auto* tgtGroup = new QGroupBox("合并输出目录", this);
    auto* tgtLayout = new QVBoxLayout(tgtGroup);
    auto* tgtRow = new QHBoxLayout();
    tgtRow->addWidget(new QLabel("输出路径："));
    m_tgtPathEdit = new QLineEdit(this);
    m_tgtPathEdit->setPlaceholderText("请选择或等待自动生成...");
    tgtRow->addWidget(m_tgtPathEdit, 1);
    auto* btnBrowseTarget = new QPushButton("浏览...", this);
    connect(btnBrowseTarget, &QPushButton::clicked, this, &MergeDialog::onBrowseTarget);
    tgtRow->addWidget(btnBrowseTarget);
    tgtLayout->addLayout(tgtRow);
    mainLayout->addWidget(tgtGroup);

    // ── 拷贝模式组 ────────────────────────────────────────────
    auto* modeGroup = new QGroupBox("拷贝模式", this);
    auto* modeLayout = new QVBoxLayout(modeGroup);
    m_radioAll = new QRadioButton("拷贝全部图片（bg_ 无标注也拷）", this);
    m_radioLabeled = new QRadioButton("仅拷贝有标注的图片（txt 配对成功的）", this);
    m_radioAll->setChecked(true);
    auto* grp = new QButtonGroup(this);
    grp->addButton(m_radioAll, 0);
    grp->addButton(m_radioLabeled, 1);
    connect(m_radioAll, &QRadioButton::toggled, this, &MergeDialog::onModeChanged);
    modeLayout->addWidget(m_radioAll);
    modeLayout->addWidget(m_radioLabeled);
    mainLayout->addWidget(modeGroup);

    // ── 进度提示（一个 QLabel 汇总所有源目录进度） ────────────────
    m_progressLabel = new QLabel("", this);
    m_progressLabel->setStyleSheet(
        "color: #333; font-size: 12px; padding: 6px; "
        "background: #f5f5f5; border-radius: 4px;");
    m_progressLabel->setWordWrap(true);
    m_progressLabel->hide();
    mainLayout->addWidget(m_progressLabel);

    // ── 底部按钮 ──────────────────────────────────────────────
    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    m_btnMerge = new QPushButton("执行合并", this);
    m_btnMerge->setMinimumHeight(36);
    connect(m_btnMerge, &QPushButton::clicked, this, &MergeDialog::onStartMerge);
    bottomLayout->addWidget(m_btnMerge);
    m_btnClose = new QPushButton("关闭", this);
    m_btnClose->setMinimumHeight(36);
    connect(m_btnClose, &QPushButton::clicked, this, &MergeDialog::close);
    bottomLayout->addWidget(m_btnClose);
    mainLayout->addLayout(bottomLayout);
}

// ═══════════════════════════════════════════════════════════
// 源目录行增删
// ═══════════════════════════════════════════════════════════
void MergeDialog::onAddSourceClicked()
{
    if (m_sourceRows.size() >= MAX_SOURCE_DIRS) return;

    auto* rowWidget = new QWidget(m_srcContainer);
    auto* rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->setSpacing(4);

    int idx = m_sourceRows.size() + 1;
    auto* lbl = new QLabel(QString("%1:").arg(idx), rowWidget);
    lbl->setFixedWidth(24);
    rowLayout->addWidget(lbl);

    auto* pathEdit = new QLineEdit(rowWidget);
    pathEdit->setPlaceholderText("请选择源目录...");
    rowLayout->addWidget(pathEdit, 1);

    auto* btnBrowse = new QPushButton("浏览", rowWidget);
    connect(btnBrowse, &QPushButton::clicked, this, [this, pathEdit]() {
        onBrowseSource(pathEdit);
    });
    rowLayout->addWidget(btnBrowse);

    auto* btnRemove = new QPushButton("移除", rowWidget);
    connect(btnRemove, &QPushButton::clicked, this, [this, rowWidget]() {
        onRemoveSource(rowWidget);
    });
    rowLayout->addWidget(btnRemove);

    connect(pathEdit, &QLineEdit::textChanged, this, &MergeDialog::onSourcePathChanged);

    m_srcListLayout->insertWidget(m_srcListLayout->count() - 1, rowWidget);

    SourceRow row;
    row.row  = rowWidget;
    row.path = pathEdit;
    m_sourceRows.append(row);

    if (m_sourceRows.size() >= MAX_SOURCE_DIRS) {
        m_btnAddSource->setEnabled(false);
    }
}

void MergeDialog::onBrowseSource(QLineEdit* pathEdit)
{
    QString startDir = pathEdit->text().isEmpty()
        ? QString::fromLatin1(DEFAULT_IMAGE_ROOT)
        : pathEdit->text();
    QString d = QFileDialog::getExistingDirectory(this, "选择源目录", startDir);
    if (!d.isEmpty()) pathEdit->setText(d);
}

void MergeDialog::onRemoveSource(QWidget* rowWidget)
{
    for (int i = 0; i < m_sourceRows.size(); ++i) {
        if (m_sourceRows[i].row == rowWidget) {
            m_srcListLayout->removeWidget(rowWidget);
            rowWidget->deleteLater();
            m_sourceRows.removeAt(i);
            break;
        }
    }
    m_btnAddSource->setEnabled(true);
    renumberSources();
    refreshTargetName();
}

void MergeDialog::renumberSources()
{
    for (int i = 0; i < m_sourceRows.size(); ++i) {
        auto* lbl = m_sourceRows[i].row->findChild<QLabel*>();
        if (lbl) lbl->setText(QString("%1:").arg(i + 1));
    }
}

void MergeDialog::onBrowseTarget()
{
    QString startDir = m_tgtPathEdit->text().isEmpty()
        ? QString::fromLatin1(DEFAULT_IMAGE_ROOT)
        : m_tgtPathEdit->text();
    QString d = QFileDialog::getExistingDirectory(this, "选择输出目录", startDir);
    if (!d.isEmpty()) {
        QString autoName = QFileInfo(m_tgtPathEdit->text()).fileName();
        if (autoName.isEmpty()) autoName = "merged";
        m_tgtPathEdit->setText(d + "/" + autoName);
    }
}

void MergeDialog::refreshTargetName()
{
    QStringList srcDirs = collectSourceDirs();
    if (srcDirs.isEmpty()) return;
    QStringList names;
    for (const QString& d : srcDirs) {
        QString name = QFileInfo(d).fileName();
        if (name.isEmpty()) name = "dir";
        names.append(name);
    }
    QString newPath = QString::fromLatin1(DEFAULT_IMAGE_ROOT) + "/" + names.join("_");
    m_tgtPathEdit->setText(newPath);
}

void MergeDialog::onSourcePathChanged()   { refreshTargetName(); }
void MergeDialog::onModeChanged()         {}

QStringList MergeDialog::collectSourceDirs() const
{
    QStringList result;
    for (const SourceRow& r : m_sourceRows) {
        QString p = r.path->text().trimmed();
        if (!p.isEmpty() && QDir().exists(p)) result.append(p);
    }
    return result;
}

// ═══════════════════════════════════════════════════════════
// 点击执行合并（同步阻塞主线程）
// ═══════════════════════════════════════════════════════════
void MergeDialog::onStartMerge()
{
    QStringList sourceDirs = collectSourceDirs();
    if (sourceDirs.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先添加至少一个有效源目录");
        return;
    }

    QString targetDir = m_tgtPathEdit->text().trimmed();
    if (targetDir.isEmpty()) {
        QMessageBox::warning(this, "警告", "请指定合并输出目录");
        return;
    }

    // ═══ 预扫 ═══
    MergeWorker worker(sourceDirs, targetDir, m_radioLabeled->isChecked());
    ScanResult scan = worker.preScan();

    if (scan.dirScans.isEmpty()) {
        QMessageBox::information(this, "提示", "未收集到任何图片，请检查源目录");
        return;
    }

    QString summary = QString(
        "源目录：%1 个\n"
        "收集图片：%2 张\n"
        "配对成功 txt：%3 个\n"
        "目标输出：%4")
        .arg(scan.dirScans.size())
        .arg(scan.totalPairs)
        .arg([&]{ int t=0; for(const auto& d:scan.dirScans) t+=d.txtCount; return t; }())
        .arg(targetDir);

    // 尺寸检查
    if (!scan.sizeMismatches.isEmpty()) {
        summary += QString("\n\n⚠️ 发现 %1 张图片尺寸与参考 %2x%3 不一致：\n")
            .arg(scan.sizeMismatches.size())
            .arg(scan.refSize.width()).arg(scan.refSize.height());
        int showCount = qMin(scan.sizeMismatches.size(), 10);
        for (int i = 0; i < showCount; ++i) summary += "  · " + scan.sizeMismatches[i] + "\n";
        if (scan.sizeMismatches.size() > 10)
            summary += QString("  ... 还有 %1 张未列出\n").arg(scan.sizeMismatches.size() - 10);
        summary += "\n是否继续合并？";
        int ret = QMessageBox::warning(this, "尺寸不一致警告", summary,
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes) return;
    } else {
        int ret = QMessageBox::question(this, "确认合并", summary + "\n\n开始合并？",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret != QMessageBox::Yes) return;
    }

    // ═══ 显示进度 label，初始化 ═══
    m_progressLabel->show();
    QString initText = "开始合并：\n";
    for (int i = 0; i < scan.dirScans.size(); ++i) {
        initText += QString("  目录%1(%2): 0 / %3\n")
            .arg(i + 1).arg(scan.dirScans[i].dirName)
            .arg(scan.dirScans[i].pairs.size());
    }
    m_progressLabel->setText(initText);

    // ═══ 禁用所有输入控件（独占模式） ═══
    m_merging = true;
    m_btnMerge->setEnabled(false);
    m_btnClose->setEnabled(false);
    m_btnAddSource->setEnabled(false);
    m_tgtPathEdit->setEnabled(false);
    m_radioAll->setEnabled(false);
    m_radioLabeled->setEnabled(false);
    for (auto& r : m_sourceRows) {
        r.path->setEnabled(false);
        for (auto* btn : r.row->findChildren<QPushButton*>()) btn->setEnabled(false);
    }

    // 用一个共享状态跟踪每个源目录的进度
    QVector<int> dirCurrents(scan.dirScans.size(), 0);

    // ═══ 同步执行合并 ═══
    QString errorMsg;
    int copiedCount = worker.executeSync(
        scan,
        // onDirProgress: 更新对应源目录计数，重绘 label 文本
        [this, &dirCurrents, &scan](int srcIdx, int cur, int total, const QString& /*fileName*/) {
            if (srcIdx >= 0 && srcIdx < dirCurrents.size()) {
                dirCurrents[srcIdx] = cur;
            }
            QString text = "正在拷贝：\n";
            for (int i = 0; i < scan.dirScans.size(); ++i) {
                int c = dirCurrents[i];
                int t = scan.dirScans[i].pairs.size();
                text += QString("  目录%1(%2): %3 / %4\n")
                    .arg(i + 1).arg(scan.dirScans[i].dirName)
                    .arg(c).arg(t);
            }
            m_progressLabel->setText(text.trimmed());
            QApplication::processEvents(QEventLoop::AllEvents);
        },
        // onTotalProgress
        [&scan](int cur, int total) {
            Q_UNUSED(scan); Q_UNUSED(cur); Q_UNUSED(total);
        },
        errorMsg
    );

    // ═══ 恢复 UI ═══
    m_btnMerge->setEnabled(true);
    m_btnClose->setEnabled(true);
    m_btnAddSource->setEnabled(true);
    m_tgtPathEdit->setEnabled(true);
    m_radioAll->setEnabled(true);
    m_radioLabeled->setEnabled(true);
    for (auto& r : m_sourceRows) {
        r.path->setEnabled(true);
        for (auto* btn : r.row->findChildren<QPushButton*>()) btn->setEnabled(true);
    }
    m_merging = false;

    // 完成
    QString finalText = "完成！\n";
    for (int i = 0; i < scan.dirScans.size(); ++i) {
        finalText += QString("  目录%1(%2): %3 / %4\n")
            .arg(i + 1).arg(scan.dirScans[i].dirName)
            .arg(dirCurrents[i]).arg(scan.dirScans[i].pairs.size());
    }
    m_progressLabel->setText(finalText.trimmed());

    if (!errorMsg.isEmpty()) {
        QMessageBox::critical(this, "错误", QString("合并失败：%1").arg(errorMsg));
    } else {
        QMessageBox::information(this, "完成",
            QString("合并完成！\n成功拷贝 %1 张图片。\n输出目录：%2")
            .arg(copiedCount).arg(targetDir));
    }
}
