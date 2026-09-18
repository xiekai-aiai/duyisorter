// mergeworker.cpp — 同步拷贝实现（不启动线程）
#include "mergeworker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QTextStream>
#include <QDebug>

// 尺寸检查辅助（统一用 QImage 读图像文件尺寸）
static void checkSize(const QString& jpgPath, const QString& baseName,
                       int srcIdx, QSize& refSize, QStringList& mismatches)
{
    // 每次调用都立刻写文件，绕过任何缓冲
    auto dump = [&](const char* tag) {
        QFile f("/tmp/checksize_trace.log");
        f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        QTextStream ts(&f);
        ts << tag << " refSize.before=" << refSize.width() << "x" << refSize.height()
           << " isNull=" << refSize.isNull()
           << " base=" << baseName << "\n";
    };
    dump("ENTER");

    QImage img(jpgPath);
    if (img.isNull()) { dump("IMG_NULL"); return; }
    QSize sz(img.width(), img.height());
    dump("IMG_OK");

    if (refSize.width() <= 0 || refSize.height() <= 0) {
        refSize = sz;
        dump("SET_REF");
    } else if (sz != refSize) {
        mismatches.append(
            QString("%1: %2  %3x%4  ≠  参考 %5x%6")
            .arg(srcIdx).arg(baseName)
            .arg(sz.width()).arg(sz.height())
            .arg(refSize.width()).arg(refSize.height()));
        dump("MISMATCH");
    } else {
        dump("MATCH");
    }
}

// ═══════════════════════════════════════════════════════════
MergeWorker::MergeWorker(const QStringList& sourceDirs,
                         const QString& targetDir,
                         bool labeledOnly)
    : m_sourceDirs(sourceDirs)
    , m_targetDir(targetDir)
    , m_labeledOnly(labeledOnly)
{
}

// ═══════════════════════════════════════════════════════════
// 预扫描：收集 pairs + 尺寸检查（同步）
// ═══════════════════════════════════════════════════════════
ScanResult MergeWorker::preScan()
{
    ScanResult result;

    for (int i = 0; i < m_sourceDirs.size(); ++i) {
        int srcLabel = i + 1;
        DirScan ds = collectFromDir(m_sourceDirs[i], srcLabel);
        if (ds.pairs.isEmpty()) continue;

        result.totalPairs += ds.pairs.size();

        // 尺寸检查：只读图像文件（QImage 最鲁棒，Qt 自带 JPEG 解码器）
        for (const CopyPair& p : ds.pairs) {
            QFileInfo fi(p.jpgPath);
            checkSize(p.jpgPath, fi.fileName(), i + 1, result.refSize, result.sizeMismatches);
        }

        result.dirScans.append(ds);
    }
    // 调试：把 preScan 结果写到文件，方便 GUI 程序查看
    {
        QFile dbg("/tmp/aimodel_merge_debug.log");
        if (dbg.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QTextStream ts(&dbg);
            ts << "=== preScan result ===\n";
            ts << "dirScans.size = " << result.dirScans.size() << "\n";
            for (int i = 0; i < result.dirScans.size(); ++i) {
                const auto& d = result.dirScans[i];
                ts << QString("  [%1] path=%2 imgCount=%3 txtCount=%4 pairs.size=%5\n")
                      .arg(i).arg(d.dirPath).arg(d.imgCount).arg(d.txtCount).arg(d.pairs.size());
                if (!d.pairs.isEmpty()) {
                    ts << "    first pair.jpgPath = " << d.pairs[0].jpgPath
                       << " exists=" << QFile::exists(d.pairs[0].jpgPath) << "\n";
                }
            }
            ts << "totalPairs = " << result.totalPairs << "\n";
            ts << "refSize = " << result.refSize.width() << "x" << result.refSize.height() << "\n";
            ts << "sizeMismatches.size = " << result.sizeMismatches.size() << "\n";
            for (int i = 0; i < result.sizeMismatches.size() && i < 5; ++i) {
                ts << "  mismatch[" << i << "]: " << result.sizeMismatches[i] << "\n";
            }
            dbg.close();
        }
    }
    return result;
}

// ═══════════════════════════════════════════════════════════
// 同步执行合并（调用者在主线程，需要自己 processEvents 刷新 UI）
// ═══════════════════════════════════════════════════════════
int MergeWorker::executeSync(
    const ScanResult& scan,
    std::function<void(int, int, int, const QString&)> onDirProgress,
    std::function<void(int, int)> onTotalProgress,
    QString& errorMsg)
{
    QDir().mkpath(m_targetDir);

    int totalCount = scan.totalPairs;
    int totalCopied = 0;

    for (int di = 0; di < scan.dirScans.size(); ++di) {
        const DirScan& ds = scan.dirScans[di];
        int totalInDir = ds.pairs.size();

        for (int fi = 0; fi < totalInDir; ++fi) {
            const CopyPair& p = ds.pairs[fi];

            QString dstJpg = m_targetDir + "/" + p.newJpgName;
            if (!QFile::copy(p.jpgPath, dstJpg)) {
                QFile::remove(dstJpg);  // 已存在则删了再拷（覆盖）
                QFile::copy(p.jpgPath, dstJpg);
            }

            if (!p.txtPath.isEmpty()) {
                QString dstTxt = m_targetDir + "/" + p.newTxtName;
                if (!QFile::copy(p.txtPath, dstTxt)) {
                    QFile::remove(dstTxt);
                    QFile::copy(p.txtPath, dstTxt);
                }
            }

            totalCopied++;

            // 回调刷新进度
            if (onDirProgress) {
                onDirProgress(di, fi + 1, totalInDir, p.newJpgName);
            }
            if (onTotalProgress) {
                onTotalProgress(totalCopied, totalCount);
            }
        }
    }

    return totalCopied;
}

// ═══════════════════════════════════════════════════════════
// 收集单个源目录
// ═══════════════════════════════════════════════════════════
DirScan MergeWorker::collectFromDir(const QString& srcDir, int srcLabel)
{
    DirScan out;
    out.dirPath = srcDir;
    out.dirName = QFileInfo(srcDir).fileName();
    if (out.dirName.isEmpty()) out.dirName = QString("dir%1").arg(srcLabel);

    QString imgDir;
    QString lblDir;
    if (isImageLabelFormat(srcDir)) {
        imgDir = srcDir + "/image";
        lblDir = srcDir + "/label";
    } else {
        imgDir = srcDir;
        lblDir = srcDir;
    }

    QDir dir(imgDir);
    QStringList filters;
    filters << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG"
            << "*.png" << "*.PNG" << "*.bmp" << "*.BMP";
    QStringList jpgFiles = dir.entryList(filters, QDir::Files, QDir::Name);
    std::sort(jpgFiles.begin(), jpgFiles.end());

    for (const QString& jpgName : jpgFiles) {
        QString jpgPath = imgDir + "/" + jpgName;
        QFileInfo fi(jpgName);
        QString base = fi.completeBaseName();
        QString ext  = fi.suffix();
        QString suffix = QString("@%1").arg(srcLabel);
        QString newJpgName = base + suffix + "." + ext;
        QString newTxtName = base + suffix + ".txt";

        // bg_ 和 img_ 同名策略：都加 @源序号 后缀（统一处理）
        QString txtPath = findTxt(lblDir, jpgName);

        // bg_ 无论模式都拷贝；非 bg_ 按 labeledOnly 决定
        bool isBg = base.startsWith("bg_", Qt::CaseInsensitive);
        if (!isBg && m_labeledOnly && txtPath.isEmpty()) {
            continue;
        }

        CopyPair pair;
        pair.jpgPath    = jpgPath;
        pair.txtPath    = txtPath;
        pair.newJpgName = newJpgName;
        pair.newTxtName = newTxtName;
        out.pairs.append(pair);
        out.imgCount++;
        if (!txtPath.isEmpty()) out.txtCount++;
    }
    return out;
}

bool MergeWorker::isImageLabelFormat(const QString& srcDir) const
{
    QDir d(srcDir);
    return d.exists("image") && d.exists("label");
}

QString MergeWorker::findTxt(const QString& searchDir, const QString& jpgName) const
{
    QFileInfo fi(jpgName);
    QString candidate = searchDir + "/" + fi.completeBaseName() + ".txt";
    if (QFile::exists(candidate)) return candidate;
    return QString();
}
