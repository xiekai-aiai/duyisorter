/**
 * sftpworker.cpp —— SftpWorker 的实现
 *
 * 线程模型核心：SftpClient 持有一个 libssh2 session（TCP socket + SSH state + SFTP handle），
 * 这个 session 在 SftpWorker 的生命周期内只建立一次。
 * 调用方需要保证：所有对 SftpWorker 的阻塞调用（connect / mkdir_p / slots）都在同一线程内。
 *
 * 典型时序（训练上传 3 阶段）：
 *   MainThread                              SftpThread (moveToThread 后)
 *   ──────────────────────────────────────  ────────────────────────────────
 *   worker.connect()  ← 一次 TCP+SSH 握手
 *   worker.mkdir_p(image)  ← 复用 session
 *   worker.mkdir_p(label)  ← 复用 session
 *   worker.moveToThread(sftpThread)
 *   sftpThread.start()
 *   ...                                     onUploadLocalDir(image)
 *                                           → emit progressChanged 0..100%
 *                                           → emit allUploadCompleted
 *                                           onUploadLocalDir(label)
 *                                           onUploadFiles(classes.txt)
 *                                           invokeMethod("disconnect")
 *   modelApi->startTrain()                  ← 不需要等 thread 真退出
 */

#include "sftpworker.h"
#include <QFileInfo>
#include <QDirIterator>
#include "unilog.h"
#include <QDebug>

// ──────────────────────────────────────────────────────────────────
// 构造 / 析构
// ──────────────────────────────────────────────────────────────────

/**
 * 构造函数只保存参数，不发起网络连接。
 * 真正的 TCP+SSH 握手延后到 connect() 里做。
 * 端口默认 22，但训练服务器用 50011，调用方应该显式传 TRAIN_SFTP_PORT。
 */
SftpWorker::SftpWorker(const QString& host, const QString& user,
                       const QString& pwd, int port, QObject *parent)
    : QObject(parent)
    , client_(host.toStdString(), port, user.toStdString(), pwd.toStdString())
{
}

/**
 * 析构时 SftpClient 也会跟着析构，它的析构函数会自动调 disconnect()，
 * 保证 session 干净释放（libssh2_sftp_shutdown + session_disconnect + close socket）。
 */
SftpWorker::~SftpWorker()
{
}

// ──────────────────────────────────────────────────────────────────
// 连接管理
// ──────────────────────────────────────────────────────────────────

/**
 * 发起一次持久握手，成功后复用整个 session 直到 disconnect()
 * 幂等：如果已经 connected，client_.connect() 内部 is_init_ 检查会直接返回 true，不重复握手
 */
bool SftpWorker::connect()
{
    return client_.connect();
}

/**
 * 断开持久连接。
 * 如果已 moveToThread，必须通过 invokeMethod("disconnect") 在子线程里调，
 * 因为 session 绑定在子线程的 event loop 上。
 */
void SftpWorker::disconnect()
{
    client_.disconnect();
}

/**
 * 递归创建远程目录（公共方法，不要求在子线程）
 * 实现是逐段 stat + mkdir，已存在则跳过。
 */
bool SftpWorker::mkdir_p(const QString& remoteDir)
{
    return client_.mkdir_p(remoteDir.toStdString());
}

// ──────────────────────────────────────────────────────────────────
// 目录遍历
// ──────────────────────────────────────────────────────────────────

/**
 * 列出远程目录下的文件（纯文件名，不含路径，不含 "." ".."）
 * 失败时发 error + 空的 remoteListCompleted
 */
void SftpWorker::onRemoteList(const QString& remoteDir)
{
    std::vector<std::string> files;
    bool ret = client_.list_files(remoteDir.toStdString(), files);
    if (!ret)
    {
        emit error(QString("遍历SFTP目录失败: %1").arg(remoteDir));
        emit remoteListCompleted(QStringList());
        return;
    }

    LOG_INFO_STM("list remote dir:" << remoteDir.toStdString() << " file size:" << files.size());

    // std::vector<std::string> → QStringList，emit 给主线程处理
    QStringList result;
    for (const auto& file : files)
    {
        result.append(QString::fromStdString(file));
    }

    emit remoteListCompleted(result);
}

bool SftpWorker::onDeleteFilesSync(const QStringList& files)
{
    if(!connect()) {
        return false;
    }

    for (const QString& localFile : files)
    {
        bool ret = client_.deleteFiles(localFile.toStdString());
        if (!ret) {
            LOG_ERROR_STM("删除文件[" << localFile.toStdString() << "] failed!");
            return false;
        }
    }

    return true;

}

bool SftpWorker::onUploadFilesSync(const QStringList& files, const QString& remoteDir)
{
    if(!connect()) {
        return false;
    }

    for (const QString& localFile : files)
    {
        QString remoteFile = remoteDir + "/" + QFileInfo(localFile).fileName();

        bool ret = client_.upload(localFile.toStdString(), remoteFile.toStdString());
        if (!ret) {
            LOG_ERROR_STM("上传文件[" << localFile.toStdString() << "] -> [" << remoteFile.toStdString()
                          << "] failed!");
            return false;
        }
    }

    return true;
}

// ──────────────────────────────────────────────────────────────────
// 上传
// ──────────────────────────────────────────────────────────────────

/**
 * 上传指定的文件列表（扁平放到 remoteDir 下，文件名保持原样）
 * 单个文件失败不会中断后续文件，最后 allUploadCompleted(success=false) 表示至少有一个失败
 */
void SftpWorker::onUploadFiles(const QStringList &files, const QString &remoteDir)
{
    bool allSuccess = true;
    for (const QString& localFile : files)
    {
        // remoteFile = remoteDir + "/" + basename(localFile)
        QString remoteFile = remoteDir + "/" + QFileInfo(localFile).fileName();

        bool ret = client_.upload(localFile.toStdString(), remoteFile.toStdString());
        if (!ret) {
            emit error(QString("上传文件: %1 -> %2 失败!").arg(localFile).arg(remoteFile));
            allSuccess = false;
        }

        // 无论成功失败都发 fileUploadCompleted，给 UI 做单文件状态展示
        emit fileUploadCompleted(localFile, remoteFile, ret);
    }

    // 全部尝试完，发最终汇总
    emit allUploadCompleted(allSuccess);
}

/**
 * 递归遍历 localDir（包括子目录）下所有文件，扁平上传到 remoteDir 下。
 * 训练场景专用：
 *   - 192 张 jpg 全丢进 remoteDir
 *   - 不保留 image/label 子目录层级（服务器端已按 task 约定分开）
 *   - 发 progressChanged 让 UI 显示百分比
 */
void SftpWorker::onUploadLocalDir(const QString& localDir, const QString& remoteDir)
{
    // 先确保本地目录存在，否则后续 QDirIterator 拿不到文件
    if (!QDir(localDir).exists())
    {
        emit error(QString("本地目录不存在: %1").arg(localDir));
        emit allUploadCompleted(false);
        return;
    }

    // QDirIterator 递归收集所有文件（不跟随符号链接）
    QStringList files;
    QDirIterator iterator(localDir, QDir::Files, QDirIterator::Subdirectories);
    while (iterator.hasNext())
    {
        files.append(iterator.next());
    }

    const int total = files.size();
    if (total == 0)
    {
        emit progressChanged(0, 0);
        emit allUploadCompleted(true);
        return;
    }

    bool allSuccess = true;

    // 逐个上传，remote 路径只取 basename（扁平）
    for (int i = 0; i < files.size(); ++i)
    {
        QString localFile  = files.at(i);
        QString remoteFile = remoteDir + "/" + QFileInfo(localFile).fileName();

        bool success = client_.upload(localFile.toStdString(), remoteFile.toStdString());

        emit fileUploadCompleted(localFile, remoteFile, success);
        emit progressChanged(i + 1, total);    // 1..N 递增，UI 直接用

        if (!success)
        {
            allSuccess = false;
        }
    }

    emit allUploadCompleted(allSuccess);
}

// ──────────────────────────────────────────────────────────────────
// 下载
// ──────────────────────────────────────────────────────────────────

/** 下载单个远程文件到本地 */
void SftpWorker::onDownloadFile(const QString& remoteFile, const QString& localFile)
{
    bool success = client_.download(remoteFile.toStdString(), localFile.toStdString());

    emit fileDownloadCompleted(remoteFile, localFile, success);
    if (!success)
    {
        emit error(QString("文件下载失败: %1 -> %2").arg(remoteFile).arg(localFile));
    }
}

/**
 * 批量下载远程目录下所有文件到本地 localDir
 * 本地目录不存在会自动 mkdir
 * 远程目录文件列表通过 client_.list_files() 获取
 */
void SftpWorker::onDownloadRemoteDir(const QString& remoteDir, const QString& localDir)
{
    std::vector<std::string> files;
    bool ret = client_.list_files(remoteDir.toStdString(), files);

    LOG_INFO_STM("list remote dir:" << remoteDir.toStdString()
                 << ", localDir:" << localDir.toStdString()
                 << ", file size:" << files.size());

    if (!ret)
    {
        emit error(QString("遍历远程目录失败: %1").arg(remoteDir));
        emit allDownloadCompleted(false);
        return;
    }

    if (files.empty()) {
        emit progressChanged(0, 0);
        emit allDownloadCompleted(true);
        return;
    }

    // 确保本地目录存在（Qt 自带，失败不影响继续，download 时再报）
    QDir().mkpath(localDir);

    bool allSuccess = true;
    const int total = files.size();

    for (int i = 0; i < files.size(); ++i)
    {
        // 远程完整路径：remoteDir + "/" + filename
        QString remoteFile = remoteDir + "/" + QString::fromStdString(files.at(i));
        QString localPath  = QDir(localDir).filePath(QFileInfo(remoteFile).fileName());

        bool success = client_.download(remoteFile.toStdString(), localPath.toStdString());

        emit fileDownloadCompleted(remoteFile, localPath, success);
        emit progressChanged(i + 1, total);

        if (!success)
        {
            allSuccess = false;
        }
    }

    emit allDownloadCompleted(allSuccess);
}
