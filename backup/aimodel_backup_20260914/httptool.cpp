#include "httptool.h"
#include <QUrlQuery>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>

HttpTool::HttpTool(QObject *parent)
    : QObject(parent)
    , m_timeoutMs(30000)  // 默认 30 秒超时
{
    m_manager = new QNetworkAccessManager(this);
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);  // 单次触发定时器
    m_reply = nullptr;
    m_downloadFile = nullptr;

    // 连接超时信号
    connect(m_timeoutTimer, &QTimer::timeout, this, &HttpTool::onTimeout);
}

HttpTool::~HttpTool()
{
    cancelRequest();  // 析构时取消当前请求
    delete m_manager;
    delete m_timeoutTimer;
    if (m_downloadFile) {
        m_downloadFile->close();
        delete m_downloadFile;
    }
}

// 设置超时时间
void HttpTool::setTimeout(int timeoutMs)
{
    m_timeoutMs = timeoutMs;
}

// 设置请求头
void HttpTool::setHeader(const QMap<QString, QString>& headers)
{
    m_headers = headers;
}

// 发起 HTTP 请求
void HttpTool::sendRequest(const QString& url, 
                          HttpRequestType type, 
                          const QMap<QString, QString>& params,
                          const QString& jsonData,
                          const QString& fileParamName,
                          const QString& filePath)
{
    // 取消之前的请求
    cancelRequest();

    // 构建请求 URL
    QUrl requestUrl(url);
    if (!requestUrl.isValid()) {
        emit requestFailed("无效的 URL", -1);
        return;
    }

    // 构建请求对象
    QNetworkRequest request(requestUrl);

    // 设置全局请求头
    for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    // 根据请求类型设置请求体和 Content-Type
    QByteArray requestData;
    switch (type) {
    case HttpRequestType::GET:
        // GET 请求：参数拼接在 URL 后
        if (!params.isEmpty()) {
            QUrlQuery query;
            for (auto it = params.begin(); it != params.end(); ++it) {
                query.addQueryItem(it.key(), it.value());
            }
            requestUrl.setQuery(query);
            request.setUrl(requestUrl);
        }
        m_reply = m_manager->get(request);
        break;

    case HttpRequestType::POST_FORM:
        // POST 表单：application/x-www-form-urlencoded
        request.setHeader(QNetworkRequest::ContentTypeHeader, 
                         "application/x-www-form-urlencoded; charset=utf-8");
        requestData = buildFormParams(params);
        m_reply = m_manager->post(request, requestData);
        break;

    case HttpRequestType::POST_JSON:
        // POST JSON：application/json
        request.setHeader(QNetworkRequest::ContentTypeHeader, 
                         "application/json; charset=utf-8");
        requestData = jsonData.toUtf8();
        m_reply = m_manager->post(request, requestData);
        break;

    case HttpRequestType::POST_MULTIPART:
        // POST 多表单（文件上传）：multipart/form-data
        if (filePath.isEmpty() || fileParamName.isEmpty()) {
            emit requestFailed("文件路径或参数名不能为空", -2);
            return;
        }
        QString boundary = QString("----HttpToolBoundary%1").arg(QDateTime::currentMSecsSinceEpoch());
        request.setHeader(QNetworkRequest::ContentTypeHeader, 
                         QString("multipart/form-data; boundary=%1").arg(boundary).toUtf8());
        requestData = buildMultipartData(params, fileParamName, filePath, boundary);
        m_reply = m_manager->post(request, requestData);
        break;
    }

    // 连接响应信号
    connect(m_reply, &QNetworkReply::finished, this, &HttpTool::onReplyFinished);
    connect(m_reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTool::onReplyError);
    // 上传/下载进度信号
    connect(m_reply, &QNetworkReply::downloadProgress, this, &HttpTool::onDownloadProgress);
    connect(m_reply, &QNetworkReply::uploadProgress, this, &HttpTool::onUploadProgress);

    // 启动超时定时器
    m_timeoutTimer->start(m_timeoutMs);
}

// 下载文件
void HttpTool::downloadFile(const QString& url, const QString& savePath)
{
    // 取消之前的请求
    cancelRequest();

    // 检查保存路径
    QFileInfo fileInfo(savePath);
    QDir saveDir = fileInfo.dir();
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(saveDir.path())) {
            emit requestFailed("保存目录创建失败", -3);
            return;
        }
    }

    // 构建请求
    QUrl requestUrl(url);
    if (!requestUrl.isValid()) {
        emit requestFailed("无效的下载 URL", -1);
        return;
    }
    QNetworkRequest request(requestUrl);
    for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    // 打开文件（覆盖写入）
    m_downloadFile = new QFile(savePath);
    if (!m_downloadFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit requestFailed("文件打开失败：" + m_downloadFile->errorString(), -4);
        delete m_downloadFile;
        m_downloadFile = nullptr;
        return;
    }

    // 发起 GET 请求下载
    m_reply = m_manager->get(request);
    connect(m_reply, &QNetworkReply::finished, this, &HttpTool::onReplyFinished);
    connect(m_reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTool::onReplyError);
    connect(m_reply, &QNetworkReply::downloadProgress, this, &HttpTool::onDownloadProgress);
    // 响应数据写入文件
    connect(m_reply, &QNetworkReply::readyRead, this, [this]() {
        if (m_downloadFile && m_downloadFile->isOpen()) {
            m_downloadFile->write(m_reply->readAll());
        }
    });

    // 启动超时定时器
    m_timeoutTimer->start(m_timeoutMs);
}

// 取消当前请求
void HttpTool::cancelRequest()
{
    m_timeoutTimer->stop();
    if (m_reply) {
        m_reply->abort();
        m_reply->disconnect();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    if (m_downloadFile) {
        m_downloadFile->close();
        delete m_downloadFile;
        m_downloadFile = nullptr;
    }
}

// 请求完成回调
void HttpTool::onReplyFinished()
{
    m_timeoutTimer->stop();
    if (!m_reply) return;

    // 获取响应状态码
    int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray responseData = m_reply->readAll();

    // 下载文件场景：确保数据写入完成
    if (m_downloadFile && m_downloadFile->isOpen()) {
        m_downloadFile->write(responseData);
        m_downloadFile->close();
        delete m_downloadFile;
        m_downloadFile = nullptr;
    }

    // 状态码 2xx 表示成功
    if (statusCode >= 200 && statusCode < 300) {
        emit requestSuccess(responseData, statusCode);
    } else {
        emit requestFailed(QString("请求失败，状态码：%1，响应：%2").arg(statusCode).arg(QString(responseData)), statusCode);
    }

    // 释放资源
    m_reply->deleteLater();
    m_reply = nullptr;
}

// 请求错误回调
void HttpTool::onReplyError(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::OperationCanceledError) {
        // 主动取消请求，不触发失败信号
        return;
    }

    m_timeoutTimer->stop();
    QString errorMsg = QString("网络错误：%1").arg(m_reply->errorString());
    emit requestFailed(errorMsg, static_cast<int>(error));

    // 释放资源
    m_reply->deleteLater();
    m_reply = nullptr;
    if (m_downloadFile) {
        m_downloadFile->close();
        delete m_downloadFile;
        m_downloadFile = nullptr;
    }
}

// 超时回调
void HttpTool::onTimeout()
{
    emit requestFailed("请求超时", -5);
    cancelRequest();  // 超时后取消请求
}

// 下载进度回调
void HttpTool::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

// 上传进度回调
void HttpTool::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

// 构建表单参数（application/x-www-form-urlencoded）
QByteArray HttpTool::buildFormParams(const QMap<QString, QString>& params)
{
    QUrlQuery query;
    for (auto it = params.begin(); it != params.end(); ++it) {
        query.addQueryItem(it.key(), it.value());
    }
    return query.toString(QUrl::FullyEncoded).toUtf8();
}

// 构建多表单数据（multipart/form-data）
QByteArray HttpTool::buildMultipartData(const QMap<QString, QString>& params, 
                                       const QString& fileParamName, 
                                       const QString& filePath, 
                                       QString& boundary)
{
    QByteArray data;
    QString boundaryPrefix = "--" + boundary + "\r\n";

    // 添加普通参数
    for (auto it = params.begin(); it != params.end(); ++it) {
        data += boundaryPrefix;
        data += QString("Content-Disposition: form-data; name=\"%1\"\r\n\r\n").arg(it.key()).toUtf8();
        data += it.value().toUtf8() + "\r\n";
    }

    // 添加文件参数
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        return QByteArray();
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    data += boundaryPrefix;
    // Content-Disposition：包含参数名和文件名
    data += QString("Content-Disposition: form-data; name=\"%1\"; filename=\"%2\"\r\n").arg(fileParamName).arg(fileInfo.fileName()).toUtf8();
    // Content-Type：自动识别文件类型（默认 application/octet-stream）
    data += QString("Content-Type: application/octet-stream\r\n\r\n").toUtf8();
    // 文件数据
    data += file.readAll();
    data += "\r\n";
    file.close();

    // 结束边界
    data += "--" + boundary + "--\r\n";

    return data;
}
