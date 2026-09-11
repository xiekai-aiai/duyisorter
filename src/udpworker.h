#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QObject>
#include <QByteArray>
#include <QHostAddress>

class QUdpSocket;

class UdpWorker : public QObject
{
    Q_OBJECT
public:
    explicit UdpWorker(QObject *parent = nullptr);
    ~UdpWorker();

public slots:
    /**
     * @brief 初始化 UDP Socket
     */
    void init();

    /**
     * @brief 发送数据并等待响应
     *
     * 这个函数运行在 UdpWorker 所在线程。
     */
    void onSendCommand(const QHostAddress& address,quint16 port,const QByteArray& request, int timeoutMs);

    /**
     * @brief 发送数据不等待响应
     *
     * 这个函数运行在 UdpWorker 所在线程。
     */
    void onSendCommand(const QHostAddress& address, quint16 port, const QByteArray& request);

    /**
     * @brief 停止
     */
    void stop();

signals:
    /**
     * @brief 命令处理完成
     */
    void commandFinished(bool success,
                         const QByteArray& response,
                         const QString& error);

private:
    QUdpSocket* socket_;
};

#endif // UDPWORKER_H
