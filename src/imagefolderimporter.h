#ifndef IMAGEFOLDERIMPORTER_H
#define IMAGEFOLDERIMPORTER_H

#include <QWidget>
#include <QStringList>
#include <QThread>
#include "aicommunicate.h"

class QListWidget;
class QPushButton;
class QLabel;
class QProgressDialog;

// 图片扫描工作线程
class ImageScanner : public QObject
{
    Q_OBJECT
public:
    explicit ImageScanner(const QString& dirPath, QObject* parent = nullptr);

public slots:
    void startScan();
    void abort();

signals:
    void progressUpdated(int count);
    void scanFinished(const QStringList& imagePaths);

private:
    QString m_dirPath;
    bool m_aborted;
};

// 主窗口类
class ImageFolderImporter : public QWidget
{
    Q_OBJECT

public:
    explicit ImageFolderImporter(QWidget *parent = nullptr);
    ~ImageFolderImporter() override;

private slots:
    void selectFolder();
    void refreshList();
    void clearList();
    void onScanFinished(const QStringList& imagePaths);

private:
    void scanImagesInFolder(const QString& dirPath);

    QListWidget* m_listWidget;      // 图片预览列表
    QPushButton* m_selectDirBtn;    // 选择文件夹按钮
    QPushButton* m_refreshBtn;      // 刷新按钮
    QPushButton* m_clearBtn;        // 清空按钮
    QLabel* m_statusLabel;          // 状态标签
    QString m_currentDir;           // 当前选中的文件夹
    QThread* m_scanThread;          // 扫描线程
    ImageScanner* m_scanner;        // 图片扫描器
};

#endif // IMAGEFOLDERIMPORTER_H
