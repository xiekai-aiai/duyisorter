#include "aicommunicate.h"


//AiCommunicate MyUdp;

/***
 *   主控件
 */
AiCommunicate::AiCommunicate()
{
    ai_udpSocket = new QUdpSocket();
//    ai_udpSocket2 = new QUdpSocket();

    // ai绑定监听端口
    if (!ai_udpSocket->bind(AI_UDP_PORT, QUdpSocket::ShareAddress)) {
        qDebug("bind to  port %d failed.",  AI_UDP_PORT);
        return;
    } else {
        qDebug("bind to port %d sucess.",  AI_UDP_PORT);
    }

    // ai绑定监听端口
//    if (!ai_udpSocket2->bind(AI_UDP_IMG_PORT, QUdpSocket::ShareAddress)) {
//        qDebug("bind to  port %d failed.",  AI_UDP_IMG_PORT);
//        return;
//    } else {
//        qDebug("bind to port %d sucess.",  AI_UDP_IMG_PORT);
//    }
    canSend = true;

    // 选择广播
    ai_udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
//    ai_udpSocket2->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);

#ifdef Q_OS_UNIX

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    buffer.reserve(BUFFER_SIZE);  // 预分配内存提升效率
    timeout.tv_sec = 4;   // 秒
    timeout.tv_usec = 0;  // 微秒（0表示不使用）
    // 1. 创建UDP套接字并检查错误
    if (sockfd < 0) {
//        std::cerr << "创建套接字失败: " << strerror(errno) << std::endl;
        printf("创建套接字失败.\n");
    }
    // 应用超时设置到套接字
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        printf("设置超时失败.\n");
        ::close(sockfd);
    }
    // 2. 设置接收缓冲区大小并检查
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) < 0) {
//        std::cerr << "设置缓冲区失败: " << strerror(errno) << std::endl;
        printf("设置缓冲区失败.\n");
        ::close(sockfd);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(AI_UDP_IMG_PORT);

    // 3. 绑定地址和端口并检查
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
//        std::cerr << "绑定失败: " << strerror(errno) << std::endl;
        printf("绑定失败.\n");
        ::close(sockfd);
    }

    std::cout << "UDP服务器启动，监听端口 9196...缓冲区大小:" << BUFFER_SIZE << "字节" << std::endl;
#endif

//    connect(ai_udpSocket, SIGNAL(readyRead()), this, SLOT(readUdpDatagrams()));


}

int AiCommunicate::getSockfd(){
#ifdef Q_OS_UNIX
    return sockfd;
#endif
#ifdef Q_OS_WIN
    return 0;
#endif
}


// 函数：清空 UDP 接收缓冲区
int AiCommunicate::clear_udp_buffer(int sockfd) {
    #ifdef Q_OS_UNIX
    if (sockfd < 0) {
        std::cerr << "Invalid socket descriptor" << std::endl;
        return -1;
    }

    // 保存原始套接字状态
    int original_flags = fcntl(sockfd, F_GETFL, 0);
    if (original_flags == -1) {
        std::cerr << "Failed to get socket flags: " << strerror(errno) << std::endl;
        return -1;
    }
    // 1. 设置套接字为非阻塞模式（避免无数据时阻塞）
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL, O_NONBLOCK) failed");
        return -1;
    }

    char buffer[4096];  // 足够大的缓冲区，应对最大 UDP 包
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    int bytes_read;
    int packets_discarded = 0;

    // 2. 循环读取缓冲区数据，直到读取失败（EAGAIN 表示缓冲区已空）
    while (1) {
        bytes_read = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                    reinterpret_cast<struct sockaddr*>(&sender_addr), &sender_len);

               if (bytes_read > 0) {
                   packets_discarded++;
                   // 可以在这里添加日志，记录丢弃的数据量
                    std::cout << "Discarded " << bytes_read << " bytes" << std::endl;
               }else if (bytes_read == -1) {
                   // 检查是否因为缓冲区为空导致的错误
                   if (errno == EAGAIN || errno == EWOULDBLOCK) {
                       // 缓冲区已空，退出循环
                       break;
                   }
                   else {
                       std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
                       // 恢复套接字原始状态
                       fcntl(sockfd, F_SETFL, original_flags);
                       return -1;
                   }
               }
               else {
                   // UDP 不会返回 0，这里处理异常情况
                   break;
               }
    }
    // 恢复套接字原始状态
    if (fcntl(sockfd, F_SETFL, original_flags) == -1) {
        std::cerr << "Failed to restore socket flags: " << strerror(errno) << std::endl;
        return -1;
    }

    return packets_discarded;  // 返回丢弃的数据包数量
    #endif
#ifdef Q_OS_WIN
    return 0;
#endif
}




AiCommunicate::~AiCommunicate()
{
    ai_udpSocket->deleteLater();
}

int  AiCommunicate::readUdpImgDatagrams(AI_Data_Protocol_D *data, int len)
{

    int ret = -1;
    quint16 peerPort = AI_UDP_IMG_PORT;
    int times = 0;
    bool boolPending = false;
    int maxTimeout = 10;

#ifdef Q_OS_UNIX
    maxTimeout = 10;
#endif
#ifdef Q_OS_WIN
    canSend = true;
    return 0;
#endif
    maxTimeout = 10;


//    for(times = 1; times <= maxTimeout; times++){
//        boolPending = ai_udpSocket2->hasPendingDatagrams();
//        if(!boolPending){
//            myFlow.sleep(1);
//        }else{
//            break;
//        }
//    }
//    //超时处理
//    if(!boolPending && (times == (maxTimeout+1))){
//        canSend = true; // 允许发送下一个报文
//        ret = -2;
//        return ret;
//    }

////    while (ai_udpSocket->hasPendingDatagrams() && !canSend) {
//    while (boolPending && !canSend) {
//        QByteArray datagram;
//        datagram.resize(ai_udpSocket2->pendingDatagramSize());
//        datagram.fill('\0');
//        QHostAddress peerAddress;
//        ret = ai_udpSocket2->readDatagram(datagram.data(), datagram.size(), &peerAddress, &peerPort);

//        myLog->debug("recv from: %s, port: %d", qPrintable(peerAddress.toString()), peerPort);

//#if DEBUG_OUTPUT == 1
//        printDatagram(datagram);
//#endif
//    //过滤自发自收的广播数据包
//#if 1
//        if (isLocalIP(peerAddress))
//            continue;
//#endif
//        if(ret<12){
//            myLog->debug("datagram length error: %d",ret);
//            canSend = true; // 允许发送下一个报文
//            ret = -1;
//            return ret;
//        }

////        if(ret == nLen){
//            //协议包头校验
//            if ((quint8)datagram[0] != cmd_ai_img_data_head_1 || (quint8)datagram[1] != cmd_ai_img_data_head_2
//                    ||(quint8)datagram[2] != cmd_ai_img_data_head_3 || (quint8)datagram[3] != cmd_ai_img_data_head_4){
//                ret =  -1;
//                canSend = true; // 允许发送下一个报文
//                return ret;
//            }

//            //数据长度
//            quint64 size = qulonglong((quint8)datagram[4]*16777216+(quint8)datagram[5]*65536+(quint8)datagram[6]*256+(quint8)datagram[7]);
//            qDebug()<<"img size:"<<size<<endl;

//            data->nCommandAddress = CMD_AI_IMG_VID_UPLOAD;
//            data->nCommandLength = size;
//            data->nCommandData = datagram.mid(8, size);

//            ret = 0;
//            canSend = true; // 允许发送下一个报文
//    }

    return ret;
}



int  AiCommunicate::readUdpDatagrams(AI_Data_Protocol_D *data, int nLen)
{

    int ret = -1;
    quint16 peerPort = AI_UDP_PORT;
    int times = 0;
    bool boolPending = false;
    int maxTimeout = 10;
    QVector<int> whiteProtocol;
    whiteProtocol.clear();
    whiteProtocol.append(CMD_AI_IMAGE_HEIGHT);
    whiteProtocol.append(CMD_AI_PIXEL_SEND);
    whiteProtocol.append(CMD_AI_EJECTOR_FIXED_BGN);
    whiteProtocol.append(CMD_AI_EJECTOR_FIXED_END);
    whiteProtocol.append(CMD_AI_EJECTOR_PARA);
    whiteProtocol.append(CMD_AI_EJECTOR_MODE);
    whiteProtocol.append(CMD_AI_IMAGE_SEND);
    whiteProtocol.append(CMD_AI_MODEL_PARA);
    whiteProtocol.append(CMD_AI_IMAGE_INFER);

#ifdef Q_OS_UNIX
    maxTimeout = 10;
    if(whiteProtocol.contains(data->nCommandAddress)){
        myFlow.msleep(100);
    }
#endif
#ifdef Q_OS_WIN
    canSend = true;
    return 0;
#endif

    for(times = 1; times <= maxTimeout; times++){
        if(whiteProtocol.contains(data->nCommandAddress)){
            boolPending = true;
        }else{
            boolPending = ai_udpSocket->hasPendingDatagrams();
        }
        if(!boolPending){
            myFlow.sleep(1);
        }else{
            break;
        }
    }
    //超时处理
    if(!boolPending && (times == (maxTimeout+1))){
        canSend = true; // 允许发送下一个报文
        ret = -2;
        return ret;
    }

//    while(udpSocket.waitForReadyRead(1000)) {
//    while (ai_udpSocket->hasPendingDatagrams() && !canSend) {
    while (boolPending && !canSend) {
        QByteArray datagram;
        datagram.resize(ai_udpSocket->pendingDatagramSize());
        datagram.fill('\0');
        QHostAddress peerAddress;
        ret = ai_udpSocket->readDatagram(datagram.data(), datagram.size(), &peerAddress, &peerPort);

        myLog->debug("recv from: %s, port: %d", qPrintable(peerAddress.toString()), peerPort);

#if DEBUG_OUTPUT == 1
        printDatagram(datagram);
#endif
    //过滤自发自收的广播数据包
#if 1
        if (isLocalIP(peerAddress))
            continue;
#endif
        if(ret<8){
            myLog->debug("datagram length error: %d",ret);
            canSend = true; // 允许发送下一个报文
            ret = -1;
            return ret;
        }

//        if(ret == nLen){
            //协议包头校验
            if ((quint8)datagram[0] != cmd_ai_data_head_1 || (quint8)datagram[1] != cmd_ai_data_head_2){
                ret =  -10;
                canSend = true; // 允许发送下一个报文
                return ret;
            }
            //模块地址
            quint16 moduleAddr = (quint8)datagram[2]*256+(quint8)datagram[3];

            //命令编码
            quint16 cmd = (quint8)datagram[4]*256+(quint8)datagram[5];

            //命令长度
            quint16 size = (quint8)datagram[6]*256+(quint8)datagram[7];

            data->nCommandAddress = cmd;
            data->nCommandLength = size;
            data->nCommandData = datagram.mid(8, size);

//            qDebug()<<data->nCommandAddress ;
//            qDebug()<<data->nCommandLength ;
//            qDebug()<<data->nCommandData;
            ret = 0;
            canSend = true; // 允许发送下一个报文

//        } else{
//            myLog->debug("udp socket length error: %d", ret);
//            canSend = true; // 允许发送下一个报文
//            ret =  -1;
//        }
    }

    return ret;
}

void AiCommunicate::processDatagram(QByteArray datagram, QHostAddress peerAddress, quint16 peerPort){
    AI_Data_Protocol_D data;
    peerPort = AI_UDP_SEND_PORT;

#if DEBUG_OUTPUT == 1
    printDatagram(datagram);
#endif

    //过滤自发自收的广播数据包
#if 1
    if (isLocalIP(peerAddress))
        return;
#endif

    //提取服务请求数据包协议内容
    if (getProtocolData(datagram, &data) < 0) {
        return;
    }

}

int AiCommunicate::getProtocolData(QByteArray datagram, AI_Data_Protocol_D *data)
{
    //协议长度校验
//    if (datagram.size() < 6)
//        return -1;

    //协议包头校验
    if ((quint8)datagram[0] != cmd_ai_data_head_1 || (quint8)datagram[1] != cmd_ai_data_head_1)
        return -1;

    //模块地址
    quint16 moduleAddr = (quint8)datagram[2]*256+(quint8)datagram[3];

    //命令编码
    quint16 cmd = (quint8)datagram[4]*256+(quint8)datagram[5];

    //命令长度
    quint16 size = (quint8)datagram[6]*256+(quint8)datagram[7];

    //提取协议内容
    if (data == NULL)
        return -1;

    data->nCommandAddress = cmd;
    data->nCommandLength = size;
    data->nCommandData = datagram.mid(8, size);
    /*
    data->nCommandData.resize(size);
    for (int i = 0; i < size; i++) {
        data->nCommandData[i] = datagram.at(6+i);
    }
    */

    return 0;
}


int AiCommunicate::writeDatagram(quint16 cmd, char sAiIntAddr, quint16 arglength, QByteArray args, QHostAddress address, quint16 port)
{
    if (!canSend && cmd !=CMD_AI_IMG_VID_UPLOAD) {
         return -1;
     }
    AI_Data_Protocol_D data;
    data.nCommandAddress = cmd;
    data.nCommandLength = arglength;
    data.nCommandData = args;

    int result =  writeDatagram(data, sAiIntAddr, address, port);
    canSend = false;
    return result;
}



int AiCommunicate::writeDatagram(AI_Data_Protocol_D data, char sAiIntAddr,  QHostAddress address, quint16 port){
    int index = 0;

    int sCrcDataLength = 6+data.nCommandData.size();
    char sCrcData[sCrcDataLength];
    unsigned int nCrcItt = 0;

    //按照协议约定组包
    QByteArray datagram;
    //数据头
    datagram[index++] = cmd_ai_data_head_1;
    datagram[index++] = cmd_ai_data_head_2;

    //模块地址
    datagram[index++] = sAiIntAddr/256;
    datagram[index++] = sAiIntAddr%256;

    //命令编码
    datagram[index++] = data.nCommandAddress/256;
    datagram[index++] = data.nCommandAddress%256;

    //命令长度
    datagram[index++] = data.nCommandLength/256;
    datagram[index++] = data.nCommandLength%256;

    //命令数据
    for (int i = 0; i < data.nCommandData.size(); i++)  {
        datagram[index++] = data.nCommandData.at(i);
    }

    //crc校验
    for (int i = 0; i < sCrcDataLength; i++) {
        sCrcData[i] = datagram[i+2];
    }
    nCrcItt = crcCheck(sCrcData, sCrcDataLength);

    datagram[index++] = nCrcItt/256;
    datagram[index++] = nCrcItt%256;

    datagram[index++] = cmd_ai_data_tail_1;
    datagram[index++] = cmd_ai_data_tail_2;

    //! 发送UDP数据包
#if DEBUG_OUTPUT == 1
    printDatagram(datagram);
    myLog->debug("send to: %s, port: %d", qPrintable(address.toString()), port);
#endif
    return ai_udpSocket->writeDatagram(datagram, address, port);
}




void AiCommunicate::printDatagram(const QByteArray datagram)
{
    QString tmp;

    if (datagram.data() == NULL || datagram.size() == 0) {
        return;
    }

    for (int i = 0; i < datagram.size(); i++) {
        tmp.sprintf("%s %02x", qPrintable(tmp), datagram.at(i)&0xff);
    }
    myLog->debug("*Data begin*\ndata length: %d\n===%s  ===", datagram.size(), qPrintable(tmp));
    myLog->debug("*Data end*");
}


QString AiCommunicate::getLocalIP(QAbstractSocket::NetworkLayerProtocol protocol)
{
    QString ip;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    if(interfaceList.size() == 0){
        return ip;
    }

    for(int i = 0; i< interfaceList.size(); i++){
        if (!(interfaceList.at(i).flags() & QNetworkInterface::IsUp) || (interfaceList.at(i).flags() & QNetworkInterface::IsLoopBack)) {
            continue;
        }

#ifdef Q_OS_UNIX
        if(interfaceList.at(i).name() != "eth0" && interfaceList.at(i).name() != "eth1"){
            break;
        }
#endif
        QList<QNetworkAddressEntry> hostAddressList = interfaceList.at(i).addressEntries();
        if(hostAddressList.size() == 0){
            return ip;
        }
        for(int j=0; j< hostAddressList.size(); j++)
        {
            if (hostAddressList.at(j).ip().protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            if (hostAddressList.at(j).ip().toString() == QHostAddress(QHostAddress::LocalHost).toString())
                continue;
            ip = hostAddressList.at(j).ip().toString();
            break;
        }
        if (ip != QHostAddress(QHostAddress::LocalHost).toString()) {
            break;
        }
    }

    return ip;
}

bool AiCommunicate::isLocalIP(QHostAddress address, QAbstractSocket::NetworkLayerProtocol protocol)
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress hostAddress, list) {
        if (address.protocol() != protocol) {
            continue;
        }
        if (hostAddress.toString() == address.toString()) {
            return true;
        }
    }

    return false;
}

unsigned int AiCommunicate::crcCheck(char *sData, int nLen)
{
    unsigned char j;
    unsigned int nCrc = 0;

    while(nLen--) {
        for(j = 0x80; j != 0; j >>= 1)
        {
            if((nCrc&0x8000)!=0) {
                nCrc <<= 1;
                nCrc ^= 0x1021;
            }
            else {
                nCrc <<= 1;
            }
            if((*sData&j) != 0) {
                nCrc ^= 0x1021;
            }
        }
        sData++;
    }

    return(nCrc);
}

//QList<QString> AiCommunicate::getDefaultPara(){
//    //ai板子数量初始化
//    int idTotal = 0;
//    switch(struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal) {
//    case 1:     // 单视
//        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
//        break;
//    case 2:     // 双视
//        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
//        break;
//    }
//    aiDeviceNum = idTotal/2;
//    qDebug() << "aiDeviceNum"<<aiDeviceNum;

//    ftpIpAddressList.clear();
//    addressList.clear();

//    ftpIpAddressList.append(AI_IP_ADDRESS);
//    QString oldIpAddress = AI_IP_ADDRESS;

//    for(int i=1; i<aiDeviceNum; i++){
//        QString newIpAddress = oldIpAddress;
//        newIpAddress = incrementLastOctet(oldIpAddress);
//        oldIpAddress = newIpAddress;
//        ftpIpAddressList.append(newIpAddress);
//    }
//    //json文件解析
//    QFile file(DEFAULT_PARA_INIT);
//    file.open(QFile::ReadOnly);
//    QString json=  QString::fromUtf8(file.readAll());
//    file.close();
//    bool ok;
//    QVariantMap resultMap = QJson::parse(json.toUtf8(), ok).toMap();
//    if(!ok){
//         qDebug() << "json open failed, using default IpAddress";
//    }else{
//        for(int i=0; i<aiDeviceNum; i++){
//            QString key = QString("ftpIp%1").arg(i);
//            if(resultMap.keys().contains(key)){
//                ftpIpAddress = resultMap[key].toString();
//                ftpIpAddressList.replace(i,ftpIpAddress);
//            }
//        }
//    }
//    qDebug() << "ftpIpAddressList"<<ftpIpAddressList;
//    address = QHostAddress(ftpIpAddressList.at(0));
//    addressList.append(address);
//    for(int i=1; i<ftpIpAddressList.size(); i++){
//        addressList.append(QHostAddress(ftpIpAddressList.at(i)));
//    }
//    return ftpIpAddressList;
//}

//// IP地址最后一位+1
//QString AiCommunicate::incrementLastOctet(const QString & ip) {
//    QStringList parts = ip.split(".");
//    int lastOctet = parts[3].toInt();

//    // 处理最后一位+1的逻辑
//    if (lastOctet < 255) {
//        lastOctet++;
//    } else {
//        // 如果最后一位是255，则保持不变或根据需求处理
//        qDebug() << "警告: IP地址最后一位已经是255，无法再增加。" << endl;
//        return ip;
//    }

//    // 构建新的IP地址
//    return parts[0] + "." + parts[1] + "." + parts[2] + "." +  QString::number(lastOctet);
//}





