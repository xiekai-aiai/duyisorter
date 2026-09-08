#ifndef AICOMMUNICATE_H
#define AICOMMUNICATE_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include <QtNetwork>
#include <mylogger.h>
#include <iostream>
#include <cstring>
#include <vector>
#ifdef Q_OS_UNIX
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>  // 用于 fcntl 函数（设置非阻塞）
// #include <opencv2/opencv.hpp>
#endif

//using QJson::JsonObject;
//using QJson::JsonArray;

/**
 *  A类包 - 上位机下发给AI模组的控制指令，以及AI模组回复给上位机指令回包
 * --------------------------------------------------------------------------
 * |2Bytes |2Bytes  |2Bytes  |2Bytes  |2Bytes   |2Bytes |2Bytes   | 备注
 * --------------------------------------------------------------------------
 * |数据头  |模块地址 |命令编码  |命令长度 |数据数据  |CRC校验 |数据尾    | 协议数据均以16进制表示
 * --------------------------------------------------------------------------
 *
 *  Bytes2  Bytes3: 0表示第一组AI模组板，1 表示第二组模组板
 *
 *  CRC校验说明：
 *   CRC采用ccit校验方式，对模块地址~命令数据的校验。
 *
 **/
#define BUFFER_SIZE 10*1024*1024  // 修正缓冲区大小计算（10MB）

#define CMD_AI_IMAGE_SEND       0x0001
#define CMD_AI_VERSION_FETCH    0x0002
#define CMD_AI_PIXEL_SEND       0x0003
#define CMD_AI_MODEL_UPDATE     0x0004
#define CMD_AI_MODEL_PARA       0x0005
#define CMD_AI_DISK_FETCH       0x0006
#define CMD_AI_IMAGE_INFER      0x0007
#define CMD_AI_IMAGE_HEIGHT     0x0008
#define CMD_AI_EJECTOR_PARA     0x0009
#define CMD_AI_EJECTOR_FIXED_BGN     0x000a
#define CMD_AI_EJECTOR_FIXED_END     0x000b
#define CMD_AI_EJECTOR_ORDER_BGN     0x000c
#define CMD_AI_EJECTOR_ORDER_END     0x000d
#define CMD_AI_EJECTOR_MODE          0x000e
#define CMD_AI_IMG_VID_UPLOAD        0x000f


struct AI_Data_Protocol_D {
    int nCommandAddress;     // 命令编码
    int nCommandLength;      // 命令长度
    QByteArray nCommandData; // 命令数据
};
const int AI_UDP_PORT = 9192;
const int AI_UDP_SEND_PORT = 9193;
const int AI_UDP_IMG_PORT = 9196;

const quint8 cmd_ai_data_head_1 = 0xa5;
const quint8 cmd_ai_data_head_2 = 0x5a;
const quint8 cmd_ai_data_tail_1 = 0xff;
const quint8 cmd_ai_data_tail_2 = 0xff;

const quint8 cmd_ai_img_data_head_1 = 0xa6;
const quint8 cmd_ai_img_data_head_2 = 0xa6;
const quint8 cmd_ai_img_data_head_3 = 0x6a;
const quint8 cmd_ai_img_data_head_4 = 0x6a;

const quint8 cmd_ai_img_data_tail_1 = 0xff;
const quint8 cmd_ai_img_data_tail_2 = 0xff;
const quint8 cmd_ai_img_data_tail_3 = 0xff;
const quint8 cmd_ai_img_data_tail_4 = 0xff;


//#define CONFIG_MULTICAST    1
#define AIUNIT	0x03

#define DEFAULT_PARA_INIT	APP_PATH "/userdata/cnf/init.json"
#define MAX_AI_DEVICE_NUM	12
//#ifdef Q_OS_UNIX
//#define AI_IP_ADDRESS           "192.168.4.126"

//#define SFTP_USER               "root"
//#define SFTP_PASSWORD           "linaro"
//#define REMOTE_MODEL_PATH       "/data/ftp/model/"
//#define REMOTE_IMG_PATH         "/data/ftp/image/"
//#define LOCAL_MODEL_PATH        "/sdcard/model/"
//#define LOCAL_IMG_PATH          "/udisk/"
//#endif
//#ifdef Q_OS_WIN
//#define AI_IP_ADDRESS           "192.168.0.105"
//#define SFTP_USER               "sanshixia"
//#define SFTP_PASSWORD           "888888"
//#define REMOTE_MODEL_PATH       "/home/sanshixia/Documents/ftp/model/"
//#define REMOTE_IMG_PATH         "/home/sanshixia/Documents/ftp/image/"
//#define LOCAL_MODEL_PATH        APP_PATH "/userdata/model/"
//#define LOCAL_IMG_PATH          APP_PATH "/userdata/image/"
//#endif

#define AI_IP_ADDRESS           "192.168.4.126"
#define SFTP_USER               "root"
#define SFTP_PASSWORD           "linaro"
#define REMOTE_MODEL_PATH       "/data/ftp/model/"
#define REMOTE_IMG_PATH         "/data/ftp/image/"
#define LOCAL_MODEL_PATH        APP_PATH "/userdata/model/"
#define LOCAL_IMG_PATH          APP_PATH "/userdata/image/"


#define SERVER_IP_ADDRESS       "192.168.1.114"
#define SERVERFTP_USER               "root"
#define SERVERFTP_PASSWORD           "YHNujm@123"
#define REMOTE_TRAIN_PATH       "/ftp/train_server/"

/*!
 * \brief 主界面类
 */
class AiCommunicate
{    
public:
    AiCommunicate();
    virtual ~AiCommunicate();

    int  getProtocolData(QByteArray array, AI_Data_Protocol_D *data = 0);
    void printDatagram(const QByteArray array);

    int writeDatagram(AI_Data_Protocol_D data,char sAiIntAddr, QHostAddress address, quint16 port);
    int writeDatagram(quint16 cmd, char sAiIntAddr, quint16 arglength, QByteArray args, QHostAddress address, quint16 port);

    void processDatagram(QByteArray array, QHostAddress peerAddress, quint16 peerPort);
//    QList<QString>  getDefaultPara();
//    QString incrementLastOctet(const QString & ip);
    int readUdpDatagrams(AI_Data_Protocol_D *data, int len);
    int readUdpImgDatagrams(AI_Data_Protocol_D *data, int len);
    int getSockfd();
    int clear_udp_buffer(int sockfd);
private:
    QUdpSocket *ai_udpSocket;
//    QUdpSocket *ai_udpSocket2;

    bool canSend;
    QString getLocalIP(QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol);
    bool    isLocalIP(QHostAddress address, QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol);
    unsigned int crcCheck(char *sData, int nLen);    // CRC check

//    int aiDeviceNum;
//    QList<QHostAddress>     addressList;
//    QList<QString>         ftpIpAddressList;
//    QHostAddress     address;
//    QString          ftpIpAddress;

#ifdef Q_OS_UNIX
    int sockfd;
    int buf_size;
    struct sockaddr_in serv_addr;
    socklen_t addr_len;
    uint32_t image_len;
    std::vector<unsigned char> buffer;
    std::vector<unsigned char> img_vec;
    struct timeval timeout;
#endif

};
extern AiCommunicate MyUpd;           // 全局串口类


#endif // AICOMMUNICATE_H
