#include "udpworker.h"

#include <QUdpSocket>
#include <QElapsedTimer>
#include <QThread>
#include "unilog.h"
#include "sortertypes.h"

UdpWorker::UdpWorker(QObject *parent)
    : QObject{parent}, socket_(nullptr)
{}

UdpWorker::~UdpWorker()
{
    delete socket_;
    socket_ = nullptr;
}


void UdpWorker::init()
{
    if (socket_)
    {
        return;
    }

    socket_ = new QUdpSocket(this);

    if (!socket_->bind(
            QHostAddress::AnyIPv4,
            UPD_CMD_PORT,
            QUdpSocket::ShareAddress |
                QUdpSocket::ReuseAddressHint))
    {
        LOG_ERROR_STM("upd bind port[" << UPD_CMD_PORT << " failed! " << socket_->errorString().toStdString());

        delete socket_;
        socket_ = nullptr;
        return;
    }

    LOG_INFO_STM("upd bind success, local port:" << socket_->localPort());
}

void UdpWorker::onSendCommand(const QHostAddress& address, quint16 port, const QByteArray& request)
{
    if (!socket_)
    {
        LOG_ERROR_STM("UDP socket is not initialized, host address:" << address.toString().toStdString() << ", port:" << port);
        emit commandFinished(false,QByteArray(), QStringLiteral("UDP socket is not initialized"));
        return;
    }

    if (request.isEmpty())
    {
        LOG_ERROR_STM("Request data is empty, host address:" << address.toString().toStdString() << ", port:" << port);
        emit commandFinished(false,QByteArray(), QStringLiteral("Request data is empty"));
        return;
    }

    // ------------------------------------------------------------
    // 1. 发送 UDP
    // ------------------------------------------------------------

    const qint64 written = socket_->writeDatagram(request, address, port);
    if (written != request.size())
    {
        LOG_ERROR_STM("writeDatagram failed, host address:" << address.toString().toStdString() << ", port:" << port
                      << ", need write:" << request.size() << " real write:" << written);
        emit commandFinished(false,QByteArray(),QStringLiteral("writeDatagram failed"));
        return;
    }

    emit commandFinished(true,QByteArray(),QString());
}


void UdpWorker::onSendCommand(const QHostAddress& address,
                            quint16 port,
                            const QByteArray& request,
                            int timeoutMs)
{
    if (!socket_)
    {
        LOG_ERROR_STM("UDP socket is not initialized, host address:" << address.toString().toStdString() << ", port:" << port);
        emit commandFinished(false,QByteArray(), QStringLiteral("UDP socket is not initialized"));
        return;
    }

    if (request.isEmpty())
    {
        LOG_ERROR_STM("Request data is empty, host address:" << address.toString().toStdString() << ", port:" << port);
        emit commandFinished(false,QByteArray(), QStringLiteral("Request data is empty"));
        return;
    }

    // ------------------------------------------------------------
    // 1. 清理之前残留的数据
    // ------------------------------------------------------------

    while (socket_->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(static_cast<int>(socket_->pendingDatagramSize()));

        QHostAddress senderAddress;
        quint16 senderPort = 0;
        socket_->readDatagram(datagram.data(),datagram.size(),&senderAddress,&senderPort);
    }

    // ------------------------------------------------------------
    // 2. 发送 UDP
    // ------------------------------------------------------------

    const qint64 written = socket_->writeDatagram(request, address, port);
    if (written != request.size())
    {
        LOG_ERROR_STM("writeDatagram failed, host address:" << address.toString().toStdString() << ", port:" << port
                      << ", need write:" << request.size() << " real write:" << written);

        emit commandFinished(false,QByteArray(),QStringLiteral("writeDatagram failed"));
        return;
    }

    // ------------------------------------------------------------
    // 3. 等待响应
    // ------------------------------------------------------------

    QElapsedTimer timer;
    timer.start();

    while (true)
    {
        const qint64 elapsed = timer.elapsed();
        if (elapsed >= timeoutMs)
        {
            LOG_ERROR_STM("UDP response timeout, host address:" << address.toString().toStdString() << ", port:" << port
                          << ", timeoutMs:" << timeoutMs);
            emit commandFinished(false,QByteArray(),QStringLiteral("UDP response timeout"));
            return;
        }

        const int remaining = timeoutMs - static_cast<int>(elapsed);

        if (!socket_->waitForReadyRead(remaining))
        {
            LOG_ERROR_STM("UDP waitForReadyRead timeout, host address:" << address.toString().toStdString() << ", port:" << port
                          << ", timeoutMs:" << timeoutMs);
            emit commandFinished(false, QByteArray(),QStringLiteral("UDP response timeout"));
            return;
        }

        // --------------------------------------------------------
        // 4. 读取所有已经到达的数据
        // --------------------------------------------------------

        while (socket_->hasPendingDatagrams())
        {
            const qint64 size = socket_->pendingDatagramSize();
            if (size <= 0)
            {
                break;
            }

            QByteArray response;
            response.resize(static_cast<int>(size));

            QHostAddress senderAddress;
            quint16 senderPort = 0;
            const qint64 readSize = socket_->readDatagram(response.data(),response.size(),&senderAddress, &senderPort);
            if (readSize < 0)
            {
                continue;
            }

            response.resize(static_cast<int>(readSize));

            // ----------------------------------------------------
            // 5. 检查响应是不是目标设备返回的
            // ----------------------------------------------------
            if (senderAddress != address)
            {
                continue;
            }

            // ----------------------------------------------------
            // 6. 收到目标设备响应
            // ----------------------------------------------------

            emit commandFinished(true,response,QString());
            return;
        }
    }
}

void UdpWorker::stop()
{
    if (socket_)
    {
        socket_->close();
    }
}
