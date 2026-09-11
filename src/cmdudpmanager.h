#ifndef CMDUDPMANAGER_H
#define CMDUDPMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QHostAddress>
#include <QMutex>

class UdpWorker;
class QThread;

class CmdUdpManager : public QObject
{
    Q_OBJECT
public:
    static CmdUdpManager& instance();

    /**
     * @brief 发送 UDP 数据并等待响应
     *
     * 该函数可以被多个界面调用。
     *
     * @param address       目标 IP
     * @param port          目标端口
     * @param request       发送的数据
     * @param response      接收到的响应数据
     * @param timeoutMs     超时时间，单位 ms
     *
     * @return true         收到有效响应
     * @return false        超时或通信失败
     */
    bool onSendCommand(const QHostAddress& address,
                     quint16 port,
                     const QByteArray& request,
                     QByteArray& response,
                     int timeoutMs = 1000);

    /**
     * @brief 发送 UDP 数据并等待响应
     *
     * 该函数可以被多个界面调用。
     *
     * @param address       目标 IP
     * @param port          目标端口
     * @param request       发送的数据
     * @return true         发送成功
     * @return false        发送失败
     */
    bool onSendCommand(const QHostAddress& address,quint16 port,const QByteArray& request);

    /**
     * @brief 停止 UDP 管理器
     */
    void stop();

private:
    explicit CmdUdpManager(QObject* parent = nullptr);
    ~CmdUdpManager();

    CmdUdpManager(const CmdUdpManager&) = delete;
    CmdUdpManager& operator=(const CmdUdpManager&) = delete;

private:
    QThread* thread_;
    UdpWorker* worker_;

    // 防止多个界面同时调用 sendCommand
    QMutex requestMutex_;
};

#endif // CMDUDPMANAGER_H
