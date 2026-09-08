#ifndef MYDEVMONITOR_H
#define MYDEVMONITOR_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>

/**
 * D类包 - 用于设备状态网络监听服务通信， 区别于现有整机通信A类、B类、C类包协议约定
 * --------------------------------------------------------------------------
 * |2Bytes |2Bytes  |2Bytes   |nBytes  |2Bytes   | 备注
 * --------------------------------------------------------------------------
 * |数据头  |数据长度  |命令地址  |命令数据  |数据尾    | 协议数据均以16进制表示
 * --------------------------------------------------------------------------
 *
 *
 * 具体协议约定 - D类上行包
 * ------------------------------------------------------------------------------------------
 * | 序号 | 协议说明 | 数据头 | 数据长度 | 命令地址 | 命令数据            | 数据尾   | 备注
 * ------------------------------------------------------------------------------------------
 * | 1   | 状态查询 | A5 5A | 00 00  | 00 01   | 0 Byte (无数据）    | 5A A5   | 回包返回系统状态
 * | 2   | 供料控制 | A5 5A | 00 01  | 00 02   | 1 Byte (0-关 1-开) | 5A A5   | 无回包
 * | 3   | 清灰控制 | A5 5A | 00 01  | 00 03   | 1 Byte (清灰时间)   | 5A A5   | 无回包
 * | 4   | 皮带控制 | A5 5A | 00 01  | 00 04   | 1 Byte (0-关 1-开) | 5A A5   | 无回包
 * ------------------------------------------------------------------------------------------
 *
 *
 * 具体协议约定 - D类下行包
 * ------------------------------------------------------------------------------------------
 * | 序号 | 协议说明 | 数据头 | 数据长度 | 命令地址 | 命令数据            | 数据尾   | 备注
 * ------------------------------------------------------------------------------------------
 * | 1   | 状态查询 | A5 5A | 00 0C  | 00 01   | 供料状态  喷阀状态    | 5A A5  | 应答数据各占1Byte
 *                                              气压使能  气压状态              (0-关, 1-开)
 *                                              风机使能  风机状态
 *                                              灯控使能  灯控状态
 *                                              皮带1使能 皮带1状态
 *                                              皮带2使能 皮带2状态
 * ------------------------------------------------------------------------------------------
 */
#define CMD_SCREEN_SYSTEM_STATE 0x01
#define CMD_SCREEN_FEED_CONTROL 0x02
#define CMD_SCREEN_WIPE_CONTROL 0x03
#define CMD_SCREEN_BELT_CONTROL 0x04

struct Data_Protocol_D {
    int nCommandAddress;     // 命令地址
    QByteArray nCommandData; // 命令数据
};

const quint8 cmd_data_head_1 = 0xA5;
const quint8 cmd_data_head_2 = 0x5A;
const quint8 cmd_data_tail_1 = 0x5A;
const quint8 cmd_data_tail_2 = 0xA5;

/*!
 * 供料控制请求模式
 */
enum {
    MODE_CLOSE,
    MODE_OPEN
};

/*!
 * 供料控制触发类型
 */
enum {
    TRIGGER_MANUAL, //! 手动触发
    TRIGGER_TIMER,  //! 定时触发
    TRIGGER_ALARM,  //! 报警触发
    TRIGGER_PLC,    //! PLC触发
    TRIGGER_SYNC    //! 网络触发
};

#define CONFIG_MULTICAST    1
#define DEFAULT_PORT        10000
#define DEFAULT_ADDRESS     "224.255.255.255"
/*!
 * 数据组播类型
 */
enum {
    GROUP_MULTICAST,   //! 多播 - 可支持跨网段发送数据
    GROUP_BROADCAST    //! 广播 - 仅支持同网段发送数据
};

/*!
 * \brief The MyDevMonitor class
 * 设备状态网络监听服务
 */
class MyDevMonitor : public QObject
{
    Q_OBJECT
public:
    explicit MyDevMonitor(QObject *parent = 0);
    virtual ~MyDevMonitor();

    void printDatagram(const QByteArray array);

    int  getProtocolData(QByteArray array, Data_Protocol_D *data = 0);
    void processDatagram(QByteArray array, QHostAddress peerAddress, quint16 peerPort);
    void processSystemState(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort);
    void processFeedControl(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort);
    void processBeltControl(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort);
    void processWipeControl(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort);

    int writeDatagram(Data_Protocol_D data, QHostAddress address, quint16 port);
    int writeDatagram(quint16 cmd, QByteArray args, QHostAddress address, quint16 port);

signals:
    void openFeederSig(int trigger);
    void closeFeederSig(int trigger);
    void startWipeSig(int trigger);

public slots:
    void readPendingDatagrams();

private:
    QString getLocalIP(QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol);
    bool    isLocalIP(QHostAddress address, QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol);

    QUdpSocket *m_udpSocket;
};

#endif // MYDEVMONITOR_H
