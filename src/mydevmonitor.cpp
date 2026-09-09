#include "mydevmonitor.h"
#include "globalparams.h"
#include <QtNetwork>
#include "unilog.h"

/*!
 * \brief MyDevMonitor::MyDevMonitor
 * 设备状态网络监听服务构造函数
 */
MyDevMonitor::MyDevMonitor(QObject* parent)
    : QObject(parent)
{
    m_udpSocket = new QUdpSocket();

    //! 绑定监听端口 (默认10000）
    if (!m_udpSocket->bind(QHostAddress::AnyIPv4, strNetInfo.selfNetControlPort, QUdpSocket::ShareAddress))
    {
        LOG_ERROR_STM("Bind to port:" << strNetInfo.selfNetControlPort << " failed");
        return;
    }
    LOG_INFO_STM("Bind to port:" << strNetInfo.selfNetControlPort << " successful!");

    //! 选择组播类型（多播或广播）
    m_udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
#ifdef CONFIG_MULTICAST
    m_udpSocket->joinMulticastGroup(strNetInfo.groupAddress);
#endif

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

/*!
 * \brief MyDevMonitor::~MyDevMonitor
 * 设备状态网络监听服务析构函数
 */
MyDevMonitor::~MyDevMonitor()
{
    m_udpSocket->deleteLater();
}

/*!
 * \brief MyDevMonitor::readPendingDatagrams
 * 接收服务请求数据包
 */
void MyDevMonitor::readPendingDatagrams()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        datagram.fill('\0');
        QHostAddress peerAddress;
        quint16 peerPort;

        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &peerAddress, &peerPort);
        LOG_TRACE_STM("recv from:" << peerAddress.toString().toStdString() << " port:" << peerPort);

        processDatagram(datagram, peerAddress, peerPort);
    }
}

/*!
 * \brief MyDevMonitor::processDatagram
 * 解析处理服务请求数据包
 * \param datagram
 * \param peerAddress
 * \param peerPort
 */
void MyDevMonitor::processDatagram(QByteArray datagram, QHostAddress peerAddress, quint16 peerPort)
{
    Data_Protocol_D data;
    peerPort = 10001;

#if DEBUG_OUTPUT == 1
    printDatagram(datagram);
#endif

    //! 过滤自发自收的广播数据包
#if 1
    if (isLocalIP(peerAddress))
        return;
#endif
    //! 基于屏运行模式过滤此数据包

    //! 提取服务请求数据包协议内容
    if (getProtocolData(datagram, &data) < 0)
    {
        return;
    }

    //! 根据命令类型进入相应处理流程
    switch (data.nCommandAddress)
    {
    case CMD_SCREEN_SYSTEM_STATE: // 状态
        processSystemState(data, peerAddress, peerPort);
        break;
    case CMD_SCREEN_FEED_CONTROL: // 供料
        processFeedControl(data, peerAddress, peerPort);
        break;
    case CMD_SCREEN_WIPE_CONTROL: // 清灰
        processWipeControl(data, peerAddress, peerPort);
        break;
    case CMD_SCREEN_BELT_CONTROL: // 皮带
        processBeltControl(data, peerAddress, peerPort);
        break;
    default:
        LOG_ERROR_STM("recv from:" << peerAddress.toString().toStdString() << " port:" << peerPort
            << " invalid request type!");
        break;
    }
}

/*!
 * \brief MyDevMonitor::getProtocolData
 * 获取数据包协议内容
 * \param datagram
 * \param data
 * \return
 */
int MyDevMonitor::getProtocolData(QByteArray datagram, Data_Protocol_D* data)
{
    //! 协议长度校验
    if (datagram.size() < 6)
        return -1;

    //! 协议包头校验
    if ((quint8)datagram[0] != cmd_data_head_1 || (quint8)datagram[1] != cmd_data_head_2)
        return -1;

    //! 协议数据长度
    quint16 size = (quint8)datagram[2] * 256 + (quint8)datagram[3];
    //! 协议命令地址
    quint16 cmd = (quint8)datagram[4] * 256 + (quint8)datagram[5];

    //! 提取协议内容
    if (data == NULL)
        return -1;

    data->nCommandAddress = cmd;
    data->nCommandData = datagram.mid(6, size);
    /*
    data->nCommandData.resize(size);
    for (int i = 0; i < size; i++) {
        data->nCommandData[i] = datagram.at(6+i);
    }
    */

    return 0;
}

/*!
 * \brief MyDevMonitor::processSystemState
 * 处理系统状态查询请求
 * \param datagram
 * \param peerAddress
 * \param peerPort
 */
void MyDevMonitor::processSystemState(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort)
{
    int index = 0;

    //! 系统状态组包
    QByteArray resp;
    resp[index++] = struGsh.bStatFeed;
    resp[index++] = struGsh.bStatEjectFpga;
    resp[index++] = struCnfg.nAlarmEnable[ALARM_ID_PRESSURE];
    resp[index++] = struGsh.bAlarmPressure;
    resp[index++] = struCnfg.nAlarmEnable[ALARM_ID_BLOWER];
    resp[index++] = struGsh.bAlarmBlower;
    resp[index++] = struCnfg.nAlarmEnable[ALARM_ID_LAMP];
    resp[index++] = struGsh.bAlarmLamp;
    resp[index++] = struCnfg.nAlarmEnable[ALARM_ID_BELT];
    resp[index++] = struGsh.bStatBelt[0];
    resp[index++] = struCnfg.nAlarmEnable[ALARM_ID_BELT];
    resp[index++] = struGsh.bStatBelt[1];

    Data_Protocol_D datagram;
    datagram.nCommandAddress = data.nCommandAddress;
    datagram.nCommandData = resp;
    writeDatagram(datagram, peerAddress, peerPort);
}

/*!
 * \brief MyDevMonitor::processFeedControl
 * 处理供料控制请求
 * \param data
 * \param peerAddress
 * \param peerPort
 */
void MyDevMonitor::processFeedControl(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort)
{
    //! 解析协议指令内容
    if (data.nCommandData.size() != 1)
        return;

    quint8 mode = (quint8)data.nCommandData.at(0);
    switch (mode)
    {
    case MODE_CLOSE:
        if (struGsh.bStatFeed != MODE_CLOSE)
        {
            emit closeFeederSig(TRIGGER_SYNC);
        }
        break;
    case MODE_OPEN:
        if (struGsh.bStatFeed != MODE_OPEN)
        {
            emit openFeederSig(TRIGGER_SYNC);
        }
        break;
    default:
        break;
    }

    //! 可考虑反馈请求执行结果
}

/*!
 * \brief MyDevMonitor::processWipeControl
 * 处理清灰控制请求
 * \param data
 * \param peerAddress
 * \param peerPort
 */
void MyDevMonitor::processWipeControl(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort)
{
    //! 解析协议指令内容
    if (data.nCommandData.size() != 1)
        return;

    //! 同步清灰时间设定
    quint8 wipeDelay = (quint8)data.nCommandData[0];
    /*
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        struCnfp.struGroupCtrl[i].nWipeDelay = wipeDelay;
    }
    */
    //! 触发同步清灰操作
//    struGsh.bFlagManuwipe = 1;
    emit startWipeSig(TRIGGER_SYNC);

    //! 可考虑反馈请求执行结果
}

/*!
 * \brief MyDevMonitor::processBeltControl
 * 处理皮带控制请求
 * \param data
 * \param peerAddress
 * \param peerPort
 */
void MyDevMonitor::processBeltControl(Data_Protocol_D data, QHostAddress peerAddress, quint16 peerPort)
{
    //! 可考虑反馈请求执行结果
}

/*!
 * \brief MyDevMonitor::printDatagram
 * 打印数据包内容
 * \param datagram
 */
void MyDevMonitor::printDatagram(const QByteArray datagram)
{
    QString tmp;

    if (datagram.data() == NULL || datagram.size() == 0)
    {
        return;
    }

    for (int i = 0; i < datagram.size(); i++)
    {
        tmp.sprintf("%s %02x", qPrintable(tmp), datagram.at(i) & 0xff);
    }

    LOG_TRACE_STM("data length:" << datagram.size() << ", data:" << tmp.toStdString());
}

/*!
 * \brief MyDevMonitor::writeDatagram
 * 发送数据包接口
 * \param data
 * \param address
 * \param port
 * \return
 */
int MyDevMonitor::writeDatagram(Data_Protocol_D data, QHostAddress address, quint16 port)
{
    int index = 0;

    //! 按照协议约定组包
    QByteArray datagram;
    datagram[index++] = cmd_data_head_1;
    datagram[index++] = cmd_data_head_2;
    datagram[index++] = data.nCommandData.size() / 256;
    datagram[index++] = data.nCommandData.size() % 256;
    datagram[index++] = data.nCommandAddress / 256;
    datagram[index++] = data.nCommandAddress % 256;
    for (int i = 0; i < data.nCommandData.size(); i++)
    {
        datagram[index++] = data.nCommandData.at(i);
    }
    datagram[index++] = cmd_data_tail_1;
    datagram[index++] = cmd_data_tail_2;

    //! 发送UDP数据包
#if DEBUG_OUTPUT == 1
    printDatagram(datagram);
    LOG_TRACE_STM("send to:" << address.toString().toStdString() << ", port:" << port);
#endif
    return m_udpSocket->writeDatagram(datagram, address, port);
}

/*!
 * \brief MyDevMonitor::writeDatagram
 * 发送数据包接口
 * \param cmd
 * \param args
 * \param address
 * \param port
 * \return
 */
int MyDevMonitor::writeDatagram(quint16 cmd, QByteArray args, QHostAddress address, quint16 port)
{
    Data_Protocol_D data;
    data.nCommandAddress = cmd;
    data.nCommandData = args;

    return writeDatagram(data, address, port);
}

/*!
 * \brief MyDevMonitor::getLocalIP
 * 获取IPv4/v6协议对应IP地址
 * \return
 */
QString MyDevMonitor::getLocalIP(QAbstractSocket::NetworkLayerProtocol protocol)
{
    QString ip;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface interface, interfaceList)
    {
        if (interface.flags() & QNetworkInterface::IsLoopBack)
            continue;
        QList<QNetworkAddressEntry> hostAddressList = interface.addressEntries();
        foreach(QNetworkAddressEntry address, hostAddressList)
        {
            if (address.ip().protocol() != protocol)
                continue;
            /*
            if (address.ip().toString() == QHostAddress(QHostAddress::LocalHost).toString())
                continue;
            */
            ip = address.ip().toString();
            break;
        }
    }
    return ip;
}

/*!
 * \brief MyDevMonitor::isLocalIP
 * \param address
 * \return
 */
bool MyDevMonitor::isLocalIP(QHostAddress address, QAbstractSocket::NetworkLayerProtocol protocol)
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress hostAddress, list)
    {
        if (address.protocol() != protocol)
        {
            continue;
        }
        if (hostAddress.toString() == address.toString())
        {
            return true;
        }
    }

    return false;
}

