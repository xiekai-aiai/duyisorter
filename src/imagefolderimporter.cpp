#include "imagefolderimporter.h"
#include <QFileDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QDirIterator>
#include <QProgressDialog>

// ImageScanner 实现
ImageScanner::ImageScanner(const QString& dirPath, QObject* parent)
    : QObject(parent), m_dirPath(dirPath), m_aborted(false)
{
}

void ImageScanner::startScan()
{
    QDir dir(m_dirPath);
    if (!dir.exists()) {
        emit scanFinished(QStringList());
        return;
    }

    // 支持的图片格式（不区分大小写）
    QStringList imageFormats = {"png", "jpg", "jpeg", "bmp", "gif", "tiff", "webp", "svg"};
    QStringList imagePaths;

    // 递归扫描所有子文件夹
    QDirIterator it(dir.path(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        if (m_aborted) break;  // 检查是否需要终止

        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        // 检查文件后缀是否为支持的图片格式
        if (imageFormats.contains(fileInfo.suffix().toLower())) {
            imagePaths << filePath;
            emit progressUpdated(imagePaths.size());  // 发送进度更新
        }
    }

    emit scanFinished(imagePaths);  // 发送扫描结果
}

void ImageScanner::abort()
{
    m_aborted = true;  // 标记终止扫描
}

// ImageFolderImporter 实现
ImageFolderImporter::ImageFolderImporter(QWidget *parent)
    : QWidget(parent), m_scanner(nullptr)
{
    // 初始化UI组件
    m_listWidget = new QListWidget(this);
    m_listWidget->setViewMode(QListWidget::IconMode);  // 图标模式显示
    m_listWidget->setIconSize(QSize(120, 120));        // 缩略图大小
    m_listWidget->setResizeMode(QListWidget::Adjust);  // 自动调整布局
    m_listWidget->setSpacing(10);                      // 项目间距

    m_selectDirBtn = new QPushButton("选择图片文件夹", this);
    m_refreshBtn = new QPushButton("刷新列表", this);
    m_clearBtn = new QPushButton("清空列表", this);
    m_statusLabel = new QLabel("就绪", this);
    m_currentDir  = QString("%1").arg(LOCAL_IMG_PATH);

    // 布局管理
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_selectDirBtn);
    btnLayout->addWidget(m_refreshBtn);
    btnLayout->addWidget(m_clearBtn);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_listWidget);
    mainLayout->addWidget(m_statusLabel);

    setLayout(mainLayout);
    setWindowTitle("文件夹图片导入器");
    resize(800, 600);  // 初始窗口大小

    // 初始化线程
    m_scanThread = new QThread(this);

    // 连接信号槽
    connect(m_selectDirBtn, &QPushButton::clicked, this, &ImageFolderImporter::selectFolder);
    connect(m_refreshBtn, &QPushButton::clicked, this, &ImageFolderImporter::refreshList);
    connect(m_clearBtn, &QPushButton::clicked, this, &ImageFolderImporter::clearList);
}

ImageFolderImporter::~ImageFolderImporter()
{
    // 清理线程资源
    if (m_scanThread->isRunning()) {
        if (m_scanner) {
            m_scanner->abort();  // 终止扫描
        }
        m_scanThread->quit();
        m_scanThread->wait();   // 等待线程结束
    }
    delete m_scanThread;
}

void ImageFolderImporter::selectFolder()
{
    // 打开文件夹选择对话框
    QString dirPath = QFileDialog::getExistingDirectory(
        this, 
        "选择图片文件夹", 
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dirPath.isEmpty()) {
        m_currentDir = dirPath;  // 保存当前选择的文件夹
        scanImagesInFolder(dirPath);  // 扫描该文件夹
    }
}

void ImageFolderImporter::refreshList()
{
    if (m_currentDir.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择一个文件夹");
        return;
    }
    scanImagesInFolder(m_currentDir);  // 重新扫描当前文件夹
}

void ImageFolderImporter::clearList()
{
    m_listWidget->clear();
    m_statusLabel->setText("列表已清空");
}

void ImageFolderImporter::scanImagesInFolder(const QString& dirPath)
{
    // 停止当前正在进行的扫描
    if (m_scanThread->isRunning()) {
        if (m_scanner) {
            m_scanner->abort();
        }
        m_scanThread->quit();
        m_scanThread->wait();
    }

    // 显示进度对话框
    QProgressDialog progress("正在扫描图片...", "取消", 0, 0, this);
    progress.setWindowTitle("扫描中");
    progress.setWindowModality(Qt::WindowModal);  // 模态对话框，阻止其他操作
    progress.show();

    // 创建扫描器并移动到线程
    m_scanner = new ImageScanner(dirPath);
    m_scanner->moveToThread(m_scanThread);

    // 连接信号槽
    connect(m_scanThread, &QThread::started, m_scanner, &ImageScanner::startScan);
    connect(m_scanner, &ImageScanner::progressUpdated, [&](int count) {
        progress.setLabelText(QString("已找到 %1 张图片...").arg(count));
    });
    connect(m_scanner, &ImageScanner::scanFinished, this, &ImageFolderImporter::onScanFinished);
    connect(m_scanner, &ImageScanner::scanFinished, m_scanner, &ImageScanner::deleteLater);
    connect(m_scanner, &ImageScanner::scanFinished, m_scanThread, &QThread::quit);
    connect(&progress, &QProgressDialog::canceled, m_scanner, &ImageScanner::abort);  // 取消扫描

    // 开始扫描
    m_statusLabel->setText(QString("正在扫描文件夹: %1").arg(dirPath));
    m_scanThread->start();

    // 等待线程完成
    m_scanThread->wait();
    progress.close();
}

void ImageFolderImporter::onScanFinished(const QStringList& imagePaths)
{
    if (imagePaths.isEmpty()) {
        m_statusLabel->setText("未找到任何图片文件");
        QMessageBox::information(this, "提示", "所选文件夹中未找到支持的图片文件");
        return;
    }

    // 清空现有列表
    m_listWidget->clear();

    // 添加图片到列表
    foreach (const QString& filePath, imagePaths) {
        QListWidgetItem* item = new QListWidgetItem();
        
        // 显示相对路径（相对于当前文件夹）
        QFileInfo fileInfo(filePath);
        QString relativePath = QDir(m_currentDir).relativeFilePath(filePath);
        item->setText(relativePath);
        
        // 存储完整路径（用于后续处理）
        item->setData(Qt::UserRole, filePath);
        
        // 生成并显示缩略图
        QImage image(filePath);
        if (!image.isNull()) {
            QPixmap pixmap = QPixmap::fromImage(
                image.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
            item->setIcon(QIcon(pixmap));
        }

        m_listWidget->addItem(item);
    }

    // 更新状态信息
    m_statusLabel->setText(QString("已导入 %1 张图片（来自：%2）")
        .arg(imagePaths.size())
        .arg(m_currentDir));
}
