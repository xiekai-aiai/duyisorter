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
    m_readyReadCount = nullptr;

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
    
    // ═══ 关键修复：用 lambda 捕获 reply 指针，不连成员函数 slot ═══
    // 原因：旧 reply 的 finished/error 信号可能排队延迟触发，
    // 如果连到成员函数 slot（读 m_reply/m_downloadFile），会被新请求的成员变量拦截，
    // 导致错误地 emit requestSuccess/requestFailed。
    QNetworkReply* reply = m_reply;
    connect(reply, &QNetworkReply::finished, this,
        [this, reply]() { handleReplyFinished(reply, nullptr); });
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, [this, reply](QNetworkReply::NetworkError e) { handleReplyError(reply, e, nullptr); });
    connect(reply, &QNetworkReply::downloadProgress, this,
        [this, reply](qint64 r, qint64 t) { handleDownloadProgress(reply, r, t); });
    connect(reply, &QNetworkReply::uploadProgress, this,
        [this, reply](qint64 r, qint64 t) { handleUploadProgress(reply, r, t); });

    // 启动超时定时器
    m_timeoutTimer->start(m_timeoutMs);
}

// 下载文件
void HttpTool::downloadFile(const QString& url, const QString& savePath)
{
    qDebug() << "[DOWNLOAD] downloadFile 入口：url=" << url << "savePath=" << savePath;

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

    // ═══ 关键修复：下载用独立的 QNetworkAccessManager ═══
    // 原因：训练服务器返回 Connection: close，训练查询的 POST 轮询
    // 复用同一个 manager 会导致连接池/内部状态被污染，后续 GET 下载
    // 可能卡在 TCP 建连阶段，等到 30s 超时。
    // 每次 downloadFile 都 new 一个干净的 manager，下载完 deleteLater。
    QNetworkAccessManager* dlManager = new QNetworkAccessManager(this);
    m_reply = dlManager->get(request);
    qDebug() << "[DOWNLOAD] 发起网络请求（独立 manager），reply="
             << (quintptr)m_reply;

    // 下载完成后销毁临时 manager
    connect(m_reply, &QNetworkReply::finished, dlManager,
            [dlManager]() { dlManager->deleteLater(); });

    // ═══ 关键修复：用 lambda 捕获 reply + file，不读成员变量 ═══
    QNetworkReply* reply = m_reply;
    QFile* outFile = m_downloadFile;
    // readyReadCount 用堆分配（不能用栈变量被 lambda 引用——downloadFile 返回后栈变量就失效）
    int* readyReadCount = new int(0);
    m_readyReadCount = readyReadCount;  // 记录到成员变量，cancelRequest 时释放

    // readyRead：增量写入文件
    connect(reply, &QNetworkReply::readyRead, this,
        [reply, outFile, readyReadCount]() {
        (*readyReadCount)++;
        if (!outFile || !outFile->isOpen()) {
            qDebug() << "[DOWNLOAD] readyRead #" << *readyReadCount
                     << "被跳过：downloadFile 不可用";
            return;
        }
        qint64 n = reply->bytesAvailable();
        qint64 written = outFile->write(reply->readAll());
        if (written > 0) {
            qDebug() << "[DOWNLOAD] readyRead #" << *readyReadCount
                     << "写入" << written << "字节（总可用" << n << "）";
        } else {
            qDebug() << "[DOWNLOAD] readyRead #" << *readyReadCount
                     << "写入 0 字节";
        }
    });

    // finished：捕获 reply + outFile + readyReadCount
    // 小响应场景 Qt 可能跳过 readyRead 直接在 finished 时返回全部数据，
    // 所以这里必须 flush reply->readAll() 到文件
    connect(reply, &QNetworkReply::finished, this,
        [this, reply, outFile, readyReadCount]() {
        handleReplyFinished(reply, outFile, readyReadCount);
    });

    // error：捕获 reply + outFile（readyReadCount 在 finished 路径释放，因为 Qt 保证 error 之后 finished 还会触发）
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, [this, reply, outFile](QNetworkReply::NetworkError e) {
        handleReplyError(reply, e, outFile);
    });

    connect(reply, &QNetworkReply::downloadProgress, this,
        [this, reply](qint64 r, qint64 t) { handleDownloadProgress(reply, r, t); });

    // 启动超时定时器
    m_timeoutTimer->start(m_timeoutMs);
}

// 取消当前请求
void HttpTool::cancelRequest()
{
    m_timeoutTimer->stop();
    qDebug() << "[DOWNLOAD] cancelRequest 进入，当前 m_reply=" << (quintptr)m_reply
             << "m_downloadFile=" << (quintptr)m_downloadFile;
    // 关键：先保存并置空成员指针，避免 abort() 同步触发 finished → 重入 onReplyFinished
    // 时，把本次请求的成员指针再次修改（或 deleteLater 重复调用）。
    QNetworkReply* reply = m_reply;
    QFile* dlFile = m_downloadFile;
    int* readyCount = m_readyReadCount;
    m_reply = nullptr;
    m_downloadFile = nullptr;
    m_readyReadCount = nullptr;
    if (reply) {
        // 不调 abort() —— abort() 对进行中的 reply 会同步 emit finished → 重入 onReplyFinished
        // 直接 disconnect + abort + deleteLater：把 abort 的 OperationCanceledError 拦截（onReplyError 里已跳过）
        reply->disconnect();
        reply->abort();      // 发 OperationCanceledError，onReplyError 里跳过；finished 此时 reply 已断开，不会进来
        reply->deleteLater();
        qDebug() << "[DOWNLOAD] cancelRequest reply 已清理";
    }
    if (dlFile) {
        dlFile->close();
        delete dlFile;
        qDebug() << "[DOWNLOAD] cancelRequest downloadFile 已清理";
    }
    if (readyCount) {
        delete readyCount;  // cancel 时 readyReadCount 不会再被 handleReplyFinished 释放（因为已 disconnect）
        qDebug() << "[DOWNLOAD] cancelRequest readyReadCount 已释放";
    }
    qDebug() << "[DOWNLOAD] cancelRequest 完成";
}

// ═══ 新实现：全部用 reply 参数，不读成员变量 ═══
// 这样旧 reply 的延迟信号不会被新请求的成员变量拦截

void HttpTool::handleReplyFinished(QNetworkReply* reply, QFile* outFile, int* readyReadCountPtr)
{
    m_timeoutTimer->stop();
    qDebug() << "[DOWNLOAD] handleReplyFinished 进入，reply=" << (quintptr)reply
             << "outFile=" << (quintptr)outFile;
    if (!reply) {
        // 同步清 m_readyReadCount，避免 cancelRequest 后面 double-free
        if (m_readyReadCount == readyReadCountPtr) m_readyReadCount = nullptr;
        delete readyReadCountPtr;
        return;
    }

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // 下载场景（outFile != nullptr）
    if (outFile) {
        // ═══ Bug 1 修复：flush reply->readAll() 处理小响应跳过 readyRead ═══
        // Qt 对小响应可能不发 readyRead，直接在 finished 时返回全部数据
        if (outFile->isOpen()) {
            qint64 remaining = reply->bytesAvailable();
            if (remaining > 0) {
                qint64 flushed = outFile->write(reply->readAll());
                qDebug() << "[DOWNLOAD] finished flush：" << remaining
                         << "剩余字节（readyRead 触发数="
                         << (readyReadCountPtr ? *readyReadCountPtr : 0)
                         << "），实际写入" << flushed;
            }
        }

        qint64 fsize = outFile->size();
        bool wasOpen = outFile->isOpen();
        if (wasOpen) {
            outFile->close();
        }
        qDebug() << "[DOWNLOAD] 下载场景：文件"
                 << (wasOpen ? "正常关闭" : "之前已关闭")
                 << "，最终大小=" << fsize << "，状态码=" << statusCode;

        // 清理 outFile 指针（只有它是当前 m_downloadFile 时才 delete）
        if (m_downloadFile == outFile) {
            delete outFile;
            m_downloadFile = nullptr;
        }

        // 同步 reply 指针
        if (m_reply == reply) m_reply = nullptr;

        // 释放 readyReadCount 堆变量，并同步清成员指针避免 cancelRequest 后面 double-free
        if (m_readyReadCount == readyReadCountPtr) m_readyReadCount = nullptr;
        delete readyReadCountPtr;
        readyReadCountPtr = nullptr;

        // 状态码 + 文件大小双重校验
        if (statusCode >= 200 && statusCode < 300) {
            if (fsize <= 0) {
                qDebug() << "[DOWNLOAD] ✗ 文件为空但状态码" << statusCode
                         << "，强制判失败";
                emit requestFailed(QString("下载完成但文件为空（状态码 %1）").arg(statusCode), statusCode);
            } else {
                qDebug() << "[DOWNLOAD] ✓ 下载成功，emit requestSuccess";
                emit requestSuccess(QByteArray(), statusCode);
            }
        } else {
            qDebug() << "[DOWNLOAD] ✗ 下载状态码异常，emit requestFailed";
            emit requestFailed(QString("请求失败，状态码：%1").arg(statusCode), statusCode);
        }
        reply->deleteLater();
        return;
    }

    // 非下载场景：直接读响应数据
    QByteArray responseData = reply->readAll();
    qDebug() << "[DOWNLOAD] 非下载场景：读取" << responseData.size() << "字节";

    // 释放 readyReadCount 堆变量（非下载场景也传了 nullptr，不会出错）
    // 并同步清成员指针避免 cancelRequest 后面 double-free
    if (m_readyReadCount == readyReadCountPtr) m_readyReadCount = nullptr;
    delete readyReadCountPtr;
    readyReadCountPtr = nullptr;

    // 置空成员指针（只有匹配时才置空，避免误伤新请求）
    if (m_reply == reply) m_reply = nullptr;

    if (statusCode >= 200 && statusCode < 300) {
        emit requestSuccess(responseData, statusCode);
    } else {
        emit requestFailed(QString("请求失败，状态码：%1，响应：%2").arg(statusCode).arg(QString(responseData)), statusCode);
    }

    reply->deleteLater();
}

void HttpTool::handleReplyError(QNetworkReply* reply, QNetworkReply::NetworkError error, QFile* outFile)
{
    qDebug() << "[DOWNLOAD] handleReplyError 进入，error=" << error
             << "reply=" << (quintptr)reply;
    if (error == QNetworkReply::OperationCanceledError) {
        qDebug() << "[DOWNLOAD] 主动取消，忽略";
        return;
    }

    m_timeoutTimer->stop();

    QString errorMsg = QString("网络错误：%1").arg(reply->errorString());
    qDebug() << "[DOWNLOAD] 网络错误详情：" << errorMsg;

    // 置空成员指针
    if (m_reply == reply) m_reply = nullptr;
    if (m_downloadFile == outFile) {
        if (outFile && outFile->isOpen()) outFile->close();
        delete outFile;
        m_downloadFile = nullptr;
    }

    emit requestFailed(errorMsg, static_cast<int>(error));
    reply->deleteLater();
}

// 超时回调（保持不变）
void HttpTool::onTimeout()
{
    qDebug() << "[DOWNLOAD] onTimeout 触发！";
    emit requestFailed("请求超时", -5);
    cancelRequest();
}

void HttpTool::handleDownloadProgress(QNetworkReply* reply, qint64 bytesReceived, qint64 bytesTotal)
{
    // 只处理当前活跃的 reply，忽略旧 reply 的延迟信号
    if (reply != m_reply) return;
    static qint64 lastLogReceived = 0;
    if (bytesReceived - lastLogReceived >= 1024 * 1024 || bytesReceived == bytesTotal) {
        qDebug() << "[DOWNLOAD] progress:" << bytesReceived << "/" << bytesTotal;
        lastLogReceived = bytesReceived;
    }
    emit downloadProgress(bytesReceived, bytesTotal);
}

void HttpTool::handleUploadProgress(QNetworkReply* reply, qint64 bytesSent, qint64 bytesTotal)
{
    if (reply != m_reply) return;
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
        data += boundaryPrefix.toUtf8();
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

    data += boundaryPrefix.toUtf8();
    // Content-Disposition：包含参数名和文件名
    data += QString("Content-Disposition: form-data; name=\"%1\"; filename=\"%2\"\r\n").arg(fileParamName).arg(fileInfo.fileName()).toUtf8();
    // Content-Type：自动识别文件类型（默认 application/octet-stream）
    data += QString("Content-Type: application/octet-stream\r\n\r\n").toUtf8();
    // 文件数据
    data += file.readAll();
    data += "\r\n";
    file.close();

    // 结束边界
    data += ("--" + boundary + "--\r\n").toUtf8();

    return data;
}
