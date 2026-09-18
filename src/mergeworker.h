// mergeworker.h — 合并拷贝逻辑（同步执行，不启动线程）
// 用户明确要求：不要多线程，独占阻塞
#ifndef MERGEWORKER_H
#define MERGEWORKER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QSize>
#include <functional>

// 一个"图片+可选txt标注"拷贝对
struct CopyPair {
    QString jpgPath;      // 源 jpg/png/bmp 路径
    QString txtPath;      // 源 txt 路径，空字符串=没有标注
    QString newJpgName;   // 目标文件名（含 @序号 后缀，如 img_01_0001@1.jpg）
    QString newTxtName;   // 目标 txt 文件名（如 img_01_0001@1.txt）
};

// 单个源目录的扫描结果
struct DirScan {
    QString dirPath;      // 源目录路径
    QString dirName;      // 目录名（basename）
    QVector<CopyPair> pairs;
    int imgCount = 0;
    int txtCount = 0;
};

// 全局预扫结果
struct ScanResult {
    QVector<DirScan> dirScans;        // 每个源目录的扫描结果
    QStringList sizeMismatches;       // 尺寸不一致清单
    QSize refSize = QSize(-1, -1);   // 参考尺寸（-1x-1 表示未设置）
    int totalPairs = 0;               // 所有源目录总图片数
};

class MergeWorker
{
public:
    MergeWorker(const QStringList& sourceDirs,
                const QString& targetDir,
                bool labeledOnly);

    // 预扫描（同步）：收集 pairs + 尺寸校验
    ScanResult preScan();

    // ═══ 同步执行合并 ═══
    // 回调1：每个源目录内进度  (srcIdx: 0-based, current, total, fileName)
    // 回调2：总进度           (current, total)
    // 返回值：成功拷贝数；错误时 errorMsg 填具体内容
    int executeSync(
        const ScanResult& scan,
        std::function<void(int srcIdx, int current, int total, const QString& fileName)> onDirProgress,
        std::function<void(int totalCurrent, int totalCount)> onTotalProgress,
        QString& errorMsg
    );

private:
    QStringList m_sourceDirs;
    QString     m_targetDir;
    bool        m_labeledOnly;

    // 收集单个源目录
    DirScan collectFromDir(const QString& srcDir, int srcLabel);

    // 目录格式识别
    bool isImageLabelFormat(const QString& srcDir) const;

    // 简化版 find_txt：只在当前目录按同名匹配
    QString findTxt(const QString& searchDir, const QString& jpgName) const;
};

#endif // MERGEWORKER_H
