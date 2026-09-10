#ifndef SFTPWORKER_H
#define SFTPWORKER_H

#include <QObject>
#include "sftp_client.h"

class SftpWorker : public QObject
{
    Q_OBJECT
public:
    explicit SftpWorker(const QString& host, const QString& user, const QString& pwd,
                        QObject *parent = nullptr);

signals:
    /**
     * @brief: 遍历远程ftp服务器目录完成信号
     * @param {QStringList&} files: 返回所有文件列表
     * @return {*}
     */
    void remoteListCompleted(const QStringList& files);

    /**
     * @brief: 单个文件上传到远程ftp服务器完成信号
     * @param {QString&} localFile  本地文件路径
     * @param {QString&} remoteFile 远程文件路径
     * @param {bool} success 是否成功 true: 成功 false: 失败
     * @return {*}
     */
    void fileUploadCompleted(const QString& localFile, const QString& remoteFile, bool success);


    /**
     * @brief: 所有文件上传到远程ftp服务器完成信号
     * @param {bool} success 是否成功 true: 成功 false: 失败
     * @return {*}
     */
    void allUploadCompleted(bool success);


    /**
     * @brief: 单个文件从远程ftp服务器下载完成信号
     * @param {QString&} remoteFile 远程文件路径
     * @param {QString&} localFile  本地文件路径
     * @param {bool} success 是否成功 true: 成功 false: 失败
     * @return {*}
     */
    void fileDownloadCompleted(const QString& remoteFile, const QString& localFile, bool success);


    /**
     * @brief: 所有文件从远程ftp服务器下载完成信号
     * @param {bool} success 是否成功 true: 成功 false: 失败
     * @return {*}
     */
    void allDownloadCompleted(bool success);

    /**
     * @brief: 进度信号
     * @param {int} current： 当前个数
     * @param {int} total: 所有个数
     * @return {*}
     */
    void progressChanged(int current, int total);


    /**
     * @brief: 错误信号
     * @param {QString&} message：错误信息
     * @return {*}
     */
    void error(const QString& message);


public slots:

    /**
     * @brief: 遍历远程ftp目录
     * @param {QString&} remoteDir: 远程目录
     * @return {*}
     */
    void onRemoteList(const QString& remoteDir);


    /**
     * @brief: 上传文件列表到ftp
     * @param {QStringList&} files： 待上传的文件列表
     * @param {QString&} remoteDir: 远程目录
     * @return {*}
     */
    void onUploadFiles(const QStringList& files, const QString& remoteDir);

    /**
     * @brief: 上传目录下所有文件到ftp
     * @param {QString&} localDir： 本地目录
     * @param {QString&} remoteDir: 远程目录
     * @return {*}
     */
    void onUploadLocalDir(const QString& localDir,const QString& remoteDir);


    /**
     * @brief: 下载文件
     * @param {QString&} remoteFile 远程文件
     * @param {QString&} localFile: 本地文件
     * @return {*}
     */
    void onDownloadFile(const QString& remoteFile, const QString& localFile);

    /**
     * @brief: 下载目录下所有文件
     * @param {QString&} remoteDir 远程目录
     * @param {QString&} localDir: 本地目录
     * @return {*}
     */
    void onDownloadRemoteDir(const QString& remoteDir,const QString& localDir);


private:
    SftpClient client_;

};

#endif // SFTPWORKER_H
