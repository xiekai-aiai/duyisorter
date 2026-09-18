#ifndef HTTPTOOL_H
#define HTTPTOOL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QMap>
#include <QTimer>
#include <QFile>
#include <QDir>

// HTTP 请求类型枚举
enum class HttpRequestType {
    GET,
    POST_FORM,    // POST 表单提交（application/x-www-form-urlencoded）
    POST_JSON,    // POST JSON 数据（application/json）
    POST_MULTIPART// POST 多表单数据（文件上传，multipart/form-data）
};

class HttpTool : public QObject
{
    Q_OBJECT
public:
    explicit HttpTool(QObject *parent = nullptr);
    ~HttpTool();

    // 设置请求超时时间（默认 30 秒）
    void setTimeout(int timeoutMs);

    // 设置请求头（如 Token、Content-Type 等）
    void setHeader(const QMap<QString, QString>& headers);

    // 发起 HTTP 请求
    void sendRequest(const QString& url, 
                    HttpRequestType type, 
                    const QMap<QString, QString>& params = QMap<QString, QString>(),
                    const QString& jsonData = "",  // POST_JSON 时使用
                    const QString& fileParamName = "",  // 文件上传时的参数名（如 "file"）
                    const QString& filePath = "");  // 上传文件路径（POST_MULTIPART 时使用）

    // 下载文件（GET 请求，保存到本地路径）
    void downloadFile(const QString& url, const QString& savePath);

    // 取消当前请求
    void cancelRequest();

signals:
    // 请求成功信号（响应数据、响应状态码）
    void requestSuccess(const QByteArray& responseData, int statusCode);
    // 请求失败信号（错误信息、错误码）
    void requestFailed(const QString& errorMsg, int errorCode);
    // 下载进度信号（已下载字节数、总字节数）
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // 上传进度信号（已上传字节数、总字节数）
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private slots:
    void onReplyFinished();          // 请求完成回调
    void onReplyError(QNetworkReply::NetworkError error);  // 请求错误回调
    void onTimeout();                // 超时回调
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);  // 下载进度回调
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);        // 上传进度回调

private:
    // 构建请求参数（表单格式）
    QByteArray buildFormParams(const QMap<QString, QString>& params);
    // 构建多表单数据（文件上传）
    QByteArray buildMultipartData(const QMap<QString, QString>& params, 
                                 const QString& fileParamName, 
                                 const QString& filePath, 
                                 QString& boundary);

private:
    QNetworkAccessManager* m_manager;  // HTTP 核心管理器
    QNetworkReply* m_reply;            // 当前请求响应对象
    QTimer* m_timeoutTimer;            // 超时定时器
    int m_timeoutMs;                   // 超时时间（毫秒）
    QMap<QString, QString> m_headers;  // 全局请求头
    QFile* m_downloadFile;             // 下载文件对象
};

#endif // HTTPTOOL_H