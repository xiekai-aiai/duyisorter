#include "sftpworker.h"
#include <QFileInfo>
#include <QDirIterator>
#include "unilog.h"
#include <QDebug>

SftpWorker::SftpWorker(const QString& host, const QString& user,
                       const QString& pwd,QObject *parent) : QObject(parent)
     , client_(host.toStdString(), user.toStdString(), pwd.toStdString())
{

}

void SftpWorker::onRemoteList(const QString& remoteDir)
{
    bool ret = false;
    std::vector<std::string> files;
    ret = client_.list_files(remoteDir.toStdString(), files);
    if (!ret)
    {
        emit error(QString("遍历SFTP目录失败: %1").arg(remoteDir));
        emit remoteListCompleted(QStringList());
        return;
    }

    LOG_INFO_STM("list remote dir:" << remoteDir.toStdString() << " file size:" << files.size());

    QStringList result;
    for (const auto& file : files)
    {
        result.append(QString::fromStdString(file));
    }

    emit remoteListCompleted(result);
}


void SftpWorker::onUploadFiles(const QStringList &files, const QString &remoteDir)
{
    bool allSuccess = true;
    for(const QString& localFile : files)
    {
        QFileInfo fileInfo(localFile);
        QString remoteFile = remoteDir + "/" + fileInfo.fileName();

        bool ret = client_.upload(localFile.toStdString(), remoteFile.toStdString());
        if(!ret) {
            emit error(QString("上传文件: %1 -> %2 失败!").arg(localFile).arg(remoteFile));
            allSuccess = false;
        }

        emit fileUploadCompleted(localFile, remoteFile, ret);
    }

    emit allUploadCompleted(allSuccess);
}

void SftpWorker::onUploadLocalDir(const QString& localDir, const QString& remoteDir)
{
    QDir rootDir(localDir);
    if (!rootDir.exists())
    {
        emit error(QString("本地目录不存在: %1").arg(localDir));
        emit allUploadCompleted(false);
        return;
    }

    QStringList files;
    QDirIterator iterator(localDir,QDir::Files,QDirIterator::Subdirectories);
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
    QString localRoot = QDir::cleanPath(localDir);

    for (int i = 0; i < files.size(); ++i)
    {
        QString localFile = files.at(i);
        QString remoteFile = remoteDir + "/" + QFileInfo(localFile).fileName();

        bool success = client_.upload(localFile.toStdString(), remoteFile.toStdString());

        emit fileUploadCompleted(localFile, remoteFile, success);
        emit progressChanged(i + 1, total);

        if (!success)
        {
            allSuccess = false;
        }
    }

    emit allUploadCompleted(allSuccess);
}

void SftpWorker::onDownloadFile(const QString& remoteFile, const QString& localFile)
{

    bool success = client_.download(remoteFile.toStdString(),localFile.toStdString());

    emit fileDownloadCompleted(remoteFile,localFile,success);
    if (!success)
    {
        emit error(QString("文件下载失败: %1 -> %2").arg(remoteFile).arg(localFile));
    }
}


void SftpWorker::onDownloadRemoteDir(const QString& remoteDir,
                              const QString& localDir)
{
    bool ret = false;
    std::vector<std::string> files;
    ret = client_.list_files(remoteDir.toStdString(),files);

    LOG_INFO_STM("list remote dir:" << remoteDir.toStdString() << ", localDir:" << localDir.toStdString()
                 << ", file size:" << files.size());

    if (!ret)
    {
        emit error(QString("遍历远程目录失败: %1").arg(remoteDir));
        emit allDownloadCompleted(false);
        return;
    }

    if(files.empty()) {
        emit progressChanged(0, 0);
        emit allDownloadCompleted(true);
        return;
    }

    QDir().mkpath(localDir);
    bool allSuccess = true;
    const int total = files.size();

    for(int i=0; i < files.size(); ++i) {
        const QString remoteFile = remoteDir + "/" + QString::fromStdString(files.at(i));
        QString fileName = QFileInfo(remoteFile).fileName();
        QString localPath = QDir(localDir).filePath(fileName);
        bool success = client_.download(remoteFile.toStdString(),localPath.toStdString());

        emit fileDownloadCompleted(remoteFile,localPath,success);
        emit progressChanged(i + 1, total);

        if (!success)
        {
            allSuccess = false;
        }
    }

    emit allDownloadCompleted(allSuccess);
}


