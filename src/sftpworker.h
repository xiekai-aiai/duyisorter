#ifndef SFTPWORKER_H
#define SFTPWORKER_H

#include <QObject>
#include "sftp_client.h"

/**
 * @brief SFTP 上传/下载 Worker（基于 libssh2 原生，持久 session）
 *
 * 设计：
 *   - SftpWorker 持有一个 SftpClient 成员（后者是 libssh2 的薄封装）
 *   - 一次 connect() 建立的 TCP+SSH+SFTP 会话在整个 Worker 生命周期内复用
 *   - 析构时自动 disconnect()
 *
 * 线程模型（必须遵守）：
 *   1. 主线程先调 connect() + mkdir_p() 建 session 和目录
 *   2. 再 moveToThread(childThread) —— socket fd 是 OS 级资源，跟线程无关，可以带过去
 *   3. 子线程通过 QueuedConnection 调 slots 执行实际 upload/download
 *   4. 完成后子线程 invokeMethod("disconnect") 断连，主线程接收 finished 信号收尾
 *
 *   ❌ 禁止在主线程调 connect() 后又在子线程调 connect()（会重复握手）
 *   ❌ 禁止跨线程共享同一个 SftpWorker（libssh2 session 非线程安全）
 *
 * 典型用法（训练上传 3 阶段串行）：
 *   SftpWorker w(host, user, pwd, port=50011);
 *   w.connect();                                         // 主线程握手一次
 *   w.mkdir_p("/ftp/task/raw/image");
 *   w.mkdir_p("/ftp/task/raw/label");
 *   w.moveToThread(thread);
 *   connect(&thread, &QThread::started, [&]{
 *       invokeMethod(&w, "onUploadLocalDir", ...);       // image
 *   });
 *   connect(&w, &allUploadCompleted, [&](bool){
 *       invokeMethod(&w, "onUploadLocalDir", ...);       // label
 *       // ...
 *   });
 *   thread.start();
 */
class SftpWorker : public QObject
{
    Q_OBJECT
public:
    /**
     * @param host SFTP 服务器域名或 IP，如 "hf.intellindust.net"
     * @param user 用户名
     * @param pwd  密码
     * @param port SSH 端口（默认 22，训练服务器用 50011）
     * @param parent Qt 父对象（通常是 AiModelSet，用它的线程作为 moveToThread 的起点）
     */
    explicit SftpWorker(const QString& host, const QString& user, const QString& pwd,
                        int port = 22, QObject *parent = nullptr);
    ~SftpWorker();

    /**
     * @brief 建立持久连接（阻塞调用）
     *        内部做一次 getaddrinfo + TCP connect + SSH handshake + userauth + SFTP init
     *        成功后所有后续 upload/download 都复用同一个 session
     *        如果已经 connected，直接返回 true（幂等）
     *        ⚠️ 必须在 moveToThread 之前调，或者在当前所在线程调
     */
    bool connect();

    /**
     * @brief 断开持久连接（阻塞调用）
     *        内部依次 libssh2_sftp_shutdown → libssh2_session_disconnect → close socket
     *        必须在 session 所在的线程调（如果已 moveToThread，用 invokeMethod 到子线程）
     */
    void disconnect();

    /**
     * @brief 递归创建远程目录（类似 mkdir -p，阻塞）
     *        前提：connect() 已成功
     *        可在主线程或子线程调（但前后要在同一线程）
     * @param remoteDir 完整路径，如 "/ftp/yaoguo128/raw/image"
     * @return true 成功或已存在
     */
    bool mkdir_p(const QString& remoteDir);

signals:
    // ── 目录遍历 ──────────────────────────────────────────────────
    /** onRemoteList 完成后发射，files 是纯文件名（不含路径） */
    void remoteListCompleted(const QStringList& files);

    // ── 上传 ──────────────────────────────────────────────────────
    /** 每个文件上传完成后发射（成功或失败都会发） */
    void fileUploadCompleted(const QString& localFile, const QString& remoteFile, bool success);
    /** 一批文件全部处理完后发射（allSuccess=false 表示至少有一个失败，但已全部尝试过） */
    void allUploadCompleted(bool success);

    // ── 下载 ──────────────────────────────────────────────────────
    /** 每个文件下载完成后发射 */
    void fileDownloadCompleted(const QString& remoteFile, const QString& localFile, bool success);
    /** 一批下载全部处理完后发射 */
    void allDownloadCompleted(bool success);

    // ── 通用 ──────────────────────────────────────────────────────
    /** 进度通知，current/total 从 1/N 到 N/N；单次操作内保证递增 */
    void progressChanged(int current, int total);
    /** 任何错误都会发这条（同时不影响 allUploadCompleted / allDownloadCompleted 的发射） */
    void error(const QString& message);

public slots:
    // ──────────────────────────────────────────────────────────────
    // 以下 slots 应通过 QueuedConnection 调用（通常是 moveToThread 后由其他线程 invokeMethod）
    // 内部调 client_.upload() / download()，这些方法本身是阻塞的，所以要放在子线程
    // 依赖：connect() 已在同线程成功调过
    // ──────────────────────────────────────────────────────────────

    /** 列出远程目录下的文件名（纯文件名，不含路径，不含 "." ".."）*/
    void onRemoteList(const QString& remoteDir);

    /** 上传指定的文件列表（保持原文件名，放到 remoteDir 下）*/
    void onUploadFiles(const QStringList& files, const QString& remoteDir);

    /** 递归遍历 localDir 下所有文件，全部上传到 remoteDir 下（不保留子目录层级，只取文件名）
     *  训练场景专用：image/ 和 label/ 目录各自独立上传 */
    void onUploadLocalDir(const QString& localDir, const QString& remoteDir);

    /** 下载单个远程文件到本地 */
    void onDownloadFile(const QString& remoteFile, const QString& localFile);

    /** 遍历远程目录下所有文件，下载到 localDir（扁平，不保留远程子目录层级）
     *  自动 mkdir localDir */
    void onDownloadRemoteDir(const QString& remoteDir, const QString& localDir);

private:
    SftpClient client_;   ///< libssh2 客户端，持有持久 session
};

#endif // SFTPWORKER_H
