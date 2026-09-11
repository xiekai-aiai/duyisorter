#include "cmdudpmanager.h"

#include <QThread>
#include <QMetaObject>
#include <QEventLoop>
#include <QTimer>
#include <QMutexLocker>

#include "udpworker.h"
#include "unilog.h"

CmdUdpManager::CmdUdpManager(QObject *parent)
    : QObject{parent}
    , thread_(new QThread(this))
    , worker_(new UdpWorker())
{
    // ------------------------------------------------------------
    // Worker 移动到专用线程
    // ------------------------------------------------------------

    worker_->moveToThread(thread_);

    // 在线程启动后初始化 Socket
    connect(thread_,
            &QThread::started,
            worker_,
            &UdpWorker::init);

    // 停止线程
    connect(thread_,
            &QThread::finished,
            worker_,
            &QObject::deleteLater);

    thread_->start();
}

CmdUdpManager& CmdUdpManager::instance()
{
    static CmdUdpManager instance;
    return instance;
}

CmdUdpManager::~CmdUdpManager()
{
    stop();
}

bool CmdUdpManager::onSendCommand(const QHostAddress &address, quint16 port, const QByteArray &request)
{
    if (request.isEmpty())
    {
        LOG_ERROR_STM("request is empty!");
        return false;
    }

    // 防止多个界面同时发送
    QMutexLocker locker(&requestMutex_);

    UdpWorker* worker = worker_;
    const bool invoked =
        QMetaObject::invokeMethod
        (
            worker,
            [worker, address, port, request]()
            {
                worker->onSendCommand(address, port, request);
            },
            Qt::QueuedConnection
        );
    return invoked;
}

bool CmdUdpManager::onSendCommand(const QHostAddress& address,
                                  quint16 port,
                                  const QByteArray& request,
                                  QByteArray& response,
                                  int timeoutMs)
{
    response.clear();
    if (request.isEmpty())
    {
        LOG_ERROR_STM("request is empty!");
        return false;
    }

    // 防止多个界面同时发送
    QMutexLocker locker(&requestMutex_);

    bool success = false;
    bool finished = false;

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    // ------------------------------------------------------------
    // Worker完成
    // ------------------------------------------------------------
    QMetaObject::Connection connection;
    connection = QObject::connect(
        worker_,
        &UdpWorker::commandFinished,
        &loop,
        [&](bool ok,
            const QByteArray& data,
            const QString& err)
        {
            Q_UNUSED(err);

            success = ok;
            response = data;
            finished = true;

            loop.quit();
        },
        Qt::QueuedConnection);

    // ------------------------------------------------------------
    // 超时
    // ------------------------------------------------------------
    QObject::connect(
        &timer,
        &QTimer::timeout,
        &loop,
        [&]()
        {
            loop.quit();
        });

    // ------------------------------------------------------------
    // 投递给 Worker
    // ------------------------------------------------------------
    UdpWorker* worker = worker_;
    const bool invoked =
        QMetaObject::invokeMethod(
            worker,
            [worker, address, port, request, timeoutMs]()
            {
                worker->onSendCommand(
                    address,
                    port,
                    request,
                    timeoutMs);
            },
            Qt::QueuedConnection);

    if (!invoked)
    {
        QObject::disconnect(connection);
        return false;
    }

    // ------------------------------------------------------------
    // Manager自己的超时保护
    // ------------------------------------------------------------
    timer.start(timeoutMs + 100);

    loop.exec();

    QObject::disconnect(connection);

    if (!finished)
    {
        // Manager等待超时
        return false;
    }

    return success;
}

void CmdUdpManager::stop()
{
    if (!thread_)
    {
        return;
    }

    if (!thread_->isRunning())
    {
        return;
    }

    // 通知 Worker 关闭 socket
    QMetaObject::invokeMethod(
        worker_,
        "stop",
        Qt::BlockingQueuedConnection);

    thread_->quit();
    thread_->wait();
}

