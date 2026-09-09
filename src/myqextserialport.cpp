/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myqextserialport.cpp
 * \brief       串口操作源文件
 * \date        2015.01.14
 */
#include "myqextserialport.h"
#include <exception>
#include <QMutex>
#include "unilog.h"

#if DEBUG_OUTPUT == 1
#define PRINT_FOR_DEBUG_1 1
#define PRINT_FOR_DEBUG_2 0
#define PRINT_FOR_DEBUG_3 0
#else
#define PRINT_FOR_DEBUG_1 0
#define PRINT_FOR_DEBUG_2 0
#define PRINT_FOR_DEBUG_3 0
#endif

MyQextSerialPort MySerial;
QMutex mutex;

MyQextSerialPort::MyQextSerialPort()
{
}

unsigned int MyQextSerialPort::crcFpga(char* sData, int nLen)
{
    unsigned char j;
    unsigned int nCrc = 0;

    while (nLen--)
    {
        for (j = 0x80; j != 0; j >>= 1)
        {
            if ((nCrc & 0x8000) != 0)
            {
                nCrc <<= 1;
                nCrc ^= 0x1021;
            }
            else
            {
                nCrc <<= 1;
            }
            if ((*sData & j) != 0)
            {
                nCrc ^= 0x1021;
            }
        }
        sData++;
    }

    return(nCrc);
}

/* 发送短信息 */
int MyQextSerialPort::smsComWrite(int nCmd, QString str)
{
    int nPacketSize = 8192;
    char sBuf[nPacketSize];
    char sCrcData[nPacketSize];
    memset(sBuf, 0, nPacketSize);

    const char* data = str.toStdString().c_str();
    int nLen = strlen(data);

    /* 目前仅考虑接口板1,模块类型及模块地址固定 */
    sBuf[0] = 0xA5;
    sBuf[1] = 0x5A;
    sBuf[2] = 0x01;
    sBuf[3] = 0x00;
    sBuf[4] = nCmd / 256;
    sBuf[5] = nCmd % 256;
    sBuf[6] = struGsh.nSerialCount1++;
    sBuf[7] = nLen / 256;
    sBuf[8] = nLen % 256;
    for (int i = 0; i < nLen; i++)
    {
        sBuf[9 + i] = data[i];
    }

    /* 判断序列号是否越界 */
    if (struGsh.nSerialCount1 > 250)
    {
        struGsh.nSerialCount1 = 0;
    }

    for (int i = 0; i < nLen + 7; i++)
    {
        sCrcData[i] = sBuf[i + 2];
    }
    unsigned int nCrcResult = crcFpga(sCrcData, nLen + 7);

    sBuf[9 + nLen] = nCrcResult / 256;
    sBuf[10 + nLen] = nCrcResult % 256;
    sBuf[11 + nLen] = 0xFF;
    sBuf[12 + nLen] = 0xFF;

#if PRINT_FOR_DEBUG_1 == 1
    printf("sms write >> ");
    for (int i = 0; i < nLen + 13; i++)
    {
        printf("%02x ", sBuf[i]);
    }
    printf("\n");
    fflush(stdout);
#endif

    myPort1->readAll();
    return myPort1->write(sBuf, 13 + nLen);
}

/* 接收短信息 */
QString MyQextSerialPort::smsComRead()
{
    int nSize = 256;
    char sBuf[nSize];
    int nRet;
    QString str = "";
    nRet = myPort1->read(sBuf, nSize);

    if (sBuf[0] == 0xAA && sBuf[1] == 0xAA
        && sBuf[2] == 0xAA && sBuf[3] == 0xAA
        && sBuf[6] == 0x30 && sBuf[7] == 0x02)
    {
        str = str.fromLatin1(sBuf + 10, nRet - 16);
    }
    else
    {
        return str;
    }

#if PRINT_FOR_DEBUG_1 == 1
    printf("sms read: %d\n", nRet);
    for (int i = 0; i < nRet; i++)
    {
        printf("%02x ", sBuf[i]);
    }
    printf("\n");
    fflush(stdout);
#endif

    return str;
}

// linux 下使用
int MyQextSerialPort::comOpen(int nPortno)
{
    char sComName[32];
    int ret = -1;

    struct PortSettings myCom1Setting = { BAUD115200, DATA_8, PAR_NONE,STOP_1, FLOW_OFF, TIMEOUT };
    struct PortSettings myCom2Setting = { BAUD115200, DATA_8, PAR_NONE,STOP_1, FLOW_OFF, TIMEOUT };
    struct PortSettings myCom3Setting = { BAUD115200, DATA_8, PAR_NONE,STOP_1, FLOW_OFF, TIMEOUT };

    memset(sComName, 0, sizeof(sComName));
    sprintf(sComName, "%s%d", "/dev/ttyS", nPortno + 3);
#ifdef Q_OS_UNIX
    switch (nPortno)
    {
    case 1:
        myPort1 = new QextSerialPort(sComName, myCom1Setting, QextSerialPort::Polling);
        ret = myPort1->open(QIODevice::ReadWrite);
        break;
    case 2:
        myPort2 = new QextSerialPort(sComName, myCom2Setting, QextSerialPort::Polling);
        ret = myPort2->open(QIODevice::ReadWrite);
        break;
    case 3:
        myPort3 = new QextSerialPort("/dev/ttyS3", myCom3Setting, QextSerialPort::Polling);
        ret = myPort3->open(QIODevice::ReadWrite);
        break;
    }
#endif
    if (ret != 1)
    {
        qDebug("open port%d is err!", nPortno);
    }
    return 1;
}

int MyQextSerialPort::comClose(int portno)
{
    switch (portno)
    {
    case 1:
        myPort1->close();
        delete myPort1;
        myPort1 = NULL;
        break;
    case 2:
        myPort2->close();
        delete myPort2;
        myPort2 = NULL;
        break;
    case 3:
        myPort3->close();
        delete myPort3;
        myPort3 = NULL;
        break;
    }

    return 0;
}

int MyQextSerialPort::getComBaud(int nPortno)     // 获取串口波特率
{
#ifdef Q_OS_UNIX
    switch (nPortno)
    {
    case 1:
        return myPort1->baudRate();
        break;
    case 2:
        return myPort2->baudRate();
        break;
    case 3:
        return myPort3->baudRate();
        break;
    }
#endif
}

void MyQextSerialPort::setComBaud(int nPortno, int baud)
{
#ifdef Q_OS_UNIX
    switch (nPortno)
    {
    case 1:
        switch (baud)
        {
        case MY_UARTBAUD_115200:
            myPort1->setBaudRate(BAUD115200);
            break;
        case MY_UARTBAUD_921600:
            myPort1->setBaudRate(BAUD921600);
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (baud)
        {
        case MY_UARTBAUD_115200:
            myPort2->setBaudRate(BAUD115200);
            break;
        case MY_UARTBAUD_921600:
            myPort2->setBaudRate(BAUD921600);
            break;
        default:
            myPort2->setBaudRate(BAUD115200);
            break;
        }
        break;
    case 3:
        switch (baud)
        {
        case MY_UARTBAUD_115200:
            myPort3->setBaudRate(BAUD115200);
            break;
        case MY_UARTBAUD_921600:
            myPort3->setBaudRate(BAUD921600);
            break;
        default:
            myPort3->setBaudRate(BAUD115200);
            break;
        }
    }
#endif
}

int MyQextSerialPort::com1Write(int nCmd, char SModuleType, char sIntAddr, char sUnitAddr,
    char arg1, char arg2, char arg3, char arg4, char arg5, int nCount)
{
    char sPacket[16];
    char sCrcData[10];
    unsigned int nCrcItt = 0;
    int i, ret = 0;
    int nUnit = (uchar)(sUnitAddr & 0x3F);

    memset(sPacket, 0, 16);

    sPacket[0] = 0xA5;
    sPacket[1] = 0x5A;

    if (nUnit < MAX_UNIT_PER_INT)
    {
        sPacket[2] = (sIntAddr * 16) | SModuleType;
        sPacket[3] = sUnitAddr;
    }
    else
    {
        sPacket[2] = ((sIntAddr + 1) * 16) | SModuleType;
        sPacket[3] = (uchar)sUnitAddr - MAX_UNIT_PER_INT;
    }

    sPacket[4] = nCmd / 256;
    sPacket[5] = nCmd % 256;
    sPacket[6] = struGsh.nSerialCount1++;
    sPacket[7] = arg1;
    sPacket[8] = arg2;
    sPacket[9] = arg3;
    sPacket[10] = arg4;
    sPacket[11] = arg5;
    sPacket[14] = 0xff;
    sPacket[15] = 0xff;

    if (struGsh.nSerialCount1 > 250)
    {
        struGsh.nSerialCount1 = 0;
    }

    for (i = 0; i < 10; i++)
    {
        sCrcData[i] = sPacket[i + 2];
    }

    if (sUnitAddr == 0x3f)
    {
        for (i = 0; i < 10; i++)
        {
            nCrcItt += sPacket[i + 2];
        }
    }
    else
    {
        nCrcItt = crcFpga(sCrcData, 10);
    }

    sPacket[12] = nCrcItt / 256;
    sPacket[13] = nCrcItt % 256;
#ifdef Q_OS_UNIX
    /* 针对需要回包的命令，需先清除缓冲区 */
    if (nCmd == CMD_UNIT_CAMERA_VERSION
        || nCmd == CMD_UNIT_IMAGE
        || nCmd == CMD_INT_VERSION
        || nCmd == CMD_UNIT_GET_LIGHT
        || nCmd == CMD_UNIT_DISPLAY_ADJUST
        || nCmd == CMD_UNIT_OBTAIN_RESULT
        || nCmd == CMD_UNIT_MATERIAL_STATISTIC_UPLOAD
        || nCmd == CMD_UNIT_POSITION_ACCEPT)
    {
        myPort1->readAll();
    }


    //! 杂粮机型及RS机型吹气次数统计命令时清空串口3缓冲区
    if ((nCmd == CMD_INT_EJECTOR_COUNT)
        || (nCmd == CMD_UNIT_STATISTIC_MATERIAL_CAPTURE)
        || (nCmd == CMD_INT_STATISTIC_EJTIMES_CAPTURE)
        || (nCmd == CMD_UNIT_STATISTIC_MATERIAL_OBTAIN)
        || (nCmd == CMD_UNIT_STATISTIC_HISTOGRAM_OBTAIN)
        || (nCmd == CMD_UNIT_STATISTIC_BACKGROUND_OBTAIN))
    {
        myPort3->readAll();
    }

    for (int m = 0; m < nCount; m++)
    {
        ret = myPort1->write(sPacket, 16);
    }
#endif

#if PRINT_FOR_DEBUG_1 == 1
#ifdef Q_OS_WIN
    QString tmpFormat = "W1 >>";

    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[2] & 0xFF);//地址
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[3] & 0xFF);//地址

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[4] & 0xFF);//命令编码
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[5] & 0xFF);//命令编码

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[6] & 0xFF);//命令序号

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[7] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[8] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[9] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[10] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[11] & 0xFF);//命令参数

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[12] & 0xFF);//命令校验
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[13] & 0xFF);//命令校验

    LOG_INFO_STM("com1 write >> " << tmpFormat.toStdString());
#else
    printf("com1(0x%x) write >> ", nCmd);
    for (int i = 0; i < 16; i++)
    {
        printf("%02x ", sPacket[i]);
    }
    printf("\n");
    fflush(0);
#endif
#endif

    return ret;
}

int MyQextSerialPort::com1Read(char* sBuf, int nLen, int timeoutMsec)
{
    mutex.lock();
    int ret = 0;
    int nDispLen = 0;

    int count = timeoutMsec / 50;
    count = (count <= 1) ? 1 : count;

    memset(sBuf, 0, nLen);
#ifdef Q_OS_UNIX
    while (1)
    {
        ret += myPort1->read(sBuf + ret, nLen - ret);
        if ((--count > 0) && (ret < nLen))
        {
            myFlow.msleep(50);
        }
        else
        {
            break;
        }
    }
#endif
    nDispLen = (ret > nLen) ? nLen : ret;
    nDispLen = (nDispLen > 24) ? 24 : nDispLen;

#if PRINT_FOR_DEBUG_1 == 1
#ifdef Q_OS_WIN
    if (nDispLen > 0)
    {
        QString tmpFormat = QString("R1(%1) <<").arg(ret);
        for (int i = 0; i < nDispLen; i++)
        {
            tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sBuf[i] & 0xFF);
        }
        LOG_INFO_STM("com1 read:" << tmpFormat.toStdString());
    }
#else
    printf("com1 read  << %d\n", ret);
    for (int i = 0; i < nDispLen; i++)
    {
        printf("%02x ", sBuf[i]);
    }
    printf("\n");
    fflush(0);
#endif
#endif

    mutex.unlock();
    return ret;
}

/* 通用串口2 write函数 */
int MyQextSerialPort::com2Write(int nCmd, char sModuleType, char sModuleAddr, char arg1, char arg2, char arg3, char arg4, char arg5, int nCount)
{
    char sPacket[16];
    char sCrcData[10];
    unsigned int nCrcItt = 0;
    int i, ret = 0;

    /* pack & add checksum */
    memset(sPacket, 0, 16);

    sPacket[0] = 0xA5;
    sPacket[1] = 0x5A;
    sPacket[2] = sModuleType;
    sPacket[3] = sModuleAddr;
    sPacket[4] = nCmd / 256;
    sPacket[5] = nCmd % 256;
    sPacket[6] = struGsh.nSerialCount2++;
    sPacket[7] = arg1;
    sPacket[8] = arg2;
    sPacket[9] = arg3;
    sPacket[10] = arg4;
    sPacket[11] = arg5;

    for (i = 0; i < 10; i++)
    {
        sCrcData[i] = sPacket[i + 2];
    }

    nCrcItt = crcFpga(sCrcData, 10);
    sPacket[12] = nCrcItt / 256;
    sPacket[13] = nCrcItt % 256;
    sPacket[14] = 0xff;
    sPacket[15] = 0xff;

    if (struGsh.nSerialCount2 > 250)
    {
        struGsh.nSerialCount2 = 0;
    }
#ifdef Q_OS_UNIX
    /* 有回包命令发送前需清空缓冲区 */
    if ((nCmd == CMD_CTRL_ALARM)
        || (nCmd == CMD_CTRL_VERSION)
        || (nCmd == CMD_CTRL_FEED_LEVEL)
        || (nCmd == CMD_CTRL_TEMPERATURE)
        || (nCmd == CMD_CTRL_LIGHT_SRC_VERSION))
    {
        myPort2->readAll();
    }

    for (i = 0; i < nCount; i++)
    {
        ret = myPort2->write(sPacket, 16);
    }
#endif

#if PRINT_FOR_DEBUG_2 == 1
#ifdef Q_OS_WIN
    QString tmpFormat = "W2 -->>";
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[2] & 0xFF);//地址
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[3] & 0xFF);//地址

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[4] & 0xFF);//命令编码
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[5] & 0xFF);//命令编码

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[6] & 0xFF);//命令序号

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[7] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[8] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[9] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[10] & 0xFF);//命令参数
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[11] & 0xFF);//命令参数

    tmpFormat.sprintf("%s / %02x", tmpFormat.toLocal8Bit().constData(), sPacket[12] & 0xFF);//命令校验
    tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sPacket[13] & 0xFF);//命令校验

    if ((nCmd == CMD_CTRL_ALARM))
    { //不打印定时报警
        return ret;
    }

    LOG_INFO_STM("com2 write:" << tmpFormat.toStdString());
#else
    printf("com2(0x%x) write >> ", nCmd);
    for (int i = 0; i < 16; i++)
    {
        printf("%02x ", sPacket[i]);
    }
    printf("\n");
    fflush(0);
#endif
#endif

    return ret;
}

/* 串口2读函数的重构函数 */
int MyQextSerialPort::com2Read(char* sBuf, int nLen)
{
    mutex.lock();

    int ret = 0;
    int i;
    int nDispLen = 0;

    memset(sBuf, 0, nLen);
#ifdef Q_OS_UNIX
    ret = myPort2->read(sBuf, nLen);
#endif
    nDispLen = (ret > nLen) ? nLen : ret;
    nDispLen = (nDispLen > 24) ? 24 : nDispLen;

#if PRINT_FOR_DEBUG_2 == 1
    if (nDispLen > 0)
    {
        QString tmpFormat = "R2: ";
        for (int i = 0; i < nDispLen; i++)
        {
            tmpFormat.sprintf("%s %02x", tmpFormat.toLocal8Bit().constData(), sBuf[i] & 0xFF);
        }
        LOG_INFO_STM("com2 read:" << tmpFormat.toStdString());
    }
#endif
    mutex.unlock();
    return ret;
}

int MyQextSerialPort::com3Write(int nCmd, char SModuleType, char sIntAddr, char sUnitAddr,
    char arg1, char arg2, char arg3, char arg4, char arg5, int nCount)
{
    char sPacket[16];
    char sCrcData[10];
    unsigned int nCrcItt = 0;
    int i, ret = 0;

    memset(sPacket, 0, 16);

    sPacket[0] = 0xA5;
    sPacket[1] = 0x5A;
    sPacket[2] = (sIntAddr * 16) | SModuleType;
    sPacket[3] = sUnitAddr;
    sPacket[4] = nCmd / 256;
    sPacket[5] = nCmd % 256;
    sPacket[6] = struGsh.nSerialCount3++;
    sPacket[7] = arg1;
    sPacket[8] = arg2;
    sPacket[9] = arg3;
    sPacket[10] = arg4;
    sPacket[11] = arg5;

    for (i = 0; i < 10; i++)
    {
        sCrcData[i] = sPacket[i + 2];
    }
    nCrcItt = crcFpga(sCrcData, 10);

    sPacket[12] = nCrcItt / 256;
    sPacket[13] = nCrcItt % 256;

    sPacket[14] = 0xff;
    sPacket[15] = 0xff;

    if (struGsh.nSerialCount3 > 250)
    {
        struGsh.nSerialCount3 = 0;
    }
#ifdef Q_OS_UNIX

    myPort3->flush();

    for (i = 0; i < nCount; i++)
    {
        ret = myPort3->write(sPacket, 16);
    }
#endif
#if PRINT_FOR_DEBUG_3 == 1
    printf("com3(0x%x) write >> ", nCmd);
    for (i = 0; i < 16; i++)
    {
        printf("%02x ", sPacket[i] & 0xFF);
    }
    printf("\n");
#endif

    return ret;
}

int MyQextSerialPort::com3Read(char* sBuf, int nLen, int timeoutMsec)
{
    int ret = 0;

    int count = timeoutMsec / 50;
    count = (count <= 1) ? 1 : count;

    if (sBuf == NULL)
    {
        LOG_ERROR_STM("sBuf is null");
        return 0;
    }

    memset(sBuf, 0, nLen);
    while (1)
    {
        ret += myPort3->read(sBuf + ret, nLen - ret);
        if ((--count > 0) && (ret < nLen))
        {
            myFlow.msleep(50);
        }
        else
        {
            break;
        }
    }
#if PRINT_FOR_DEBUG_3 == 1
    printf("com3 read >> %d\n", ret);
    int nPrintLen = (ret > nLen) ? nLen : ret;
    int nPrintLen = (nDispLen > 24) ? 24 : nDispLen;
    for (int i = 0; i < nPrintLen; i++)
    {
        printf("%02x ", sBuf[i] & 0xFF);
    }
    printf("\n");
    if (ret > 34)
    {
        printf("...  ");
        for (int i = ret - 11; i < ret; i++)
        {
            printf("%02x ", sBuf[i] & 0xFF);
        }
    }
    printf("\n");
    fflush(0);
#endif

    return ret;
}

int MyQextSerialPort::remoteUpdateRead(char* sBuf, int nLen)
{
    int nCountRead = 0;
    unsigned int nCrcItt = 0x00000001;
    char sCrcData[14];

    memset(sBuf, 0, nLen);
    memset(sCrcData, 0, 14);

    myPort1->read(sBuf, nLen);

    if (nLen == 24)
    {
        if ((sBuf[15] & 8) == 8)
        {
            return 0;
        }
        if (sBuf[13] != sBuf[14])
        {
            return 0;
        }

        if ((nCrcItt / 256 == sBuf[18]) && (nCrcItt % 256 == sBuf[19]))
        {
            return nCountRead;
        }
        else
        {
            memset(sBuf, 0, nLen);
            return 0;
        }
    }

    memset(sBuf, 0, nLen);
    return nCountRead;
}

/* 远程升级写操作 */
int MyQextSerialPort::fpgaUpdateWrite(int nCmd, uchar sModule1, uchar sModule2, uchar arg1, uchar arg2, uchar arg3, uchar arg4, uchar arg5, int nCount, uchar buf[][FRAME_LEN])
{
    char packet[272];
    char crcdata[266];
    uint crcitt;
    int i;

    /* pack & add checksum */
    memset(packet, 0, 16);

    if (nCmd == COM_SEND_FILE || (sModule1 == 0x00 && nCmd == COM_CTRL_UPDATE_SEND_FILE))
    {
        packet[0] = 0xF0;
        packet[1] = 0xA5;
    }
    else
    {
        packet[0] = 0xA5;
        packet[1] = 0x5A;
    }

    if (sModule2 < MAX_UNIT_PER_INT)
    {
        packet[2] = sModule1;
        packet[3] = sModule2;
    }
    else
    {
        packet[2] = sModule1 | 0x10;
        packet[3] = sModule2 - MAX_UNIT_PER_INT;
    }

    packet[4] = nCmd / 256;
    packet[5] = nCmd % 256;
    packet[6] = struGsh.nSerialCount1++;
    packet[7] = arg1;
    packet[8] = arg2;
    packet[9] = arg3;
    packet[10] = arg4;
    packet[11] = arg5;

    if (nCmd == COM_SEND_FILE || (sModule1 == 0x00 && nCmd == COM_CTRL_UPDATE_SEND_FILE))
    {
        for (i = 0; i < FRAME_LEN; i++)
        {
            packet[12 + i] = buf[arg4 * FRAME_LEN + arg5][FRAME_LEN - 1 - i];
        }
        for (i = 0; i < 266; i++)
        {
            crcdata[i] = packet[i + 2];
        }

        crcitt = crcFpga(crcdata, 266);
        packet[268] = crcitt / 256;
        packet[269] = crcitt % 256;
        packet[270] = 0xff;
        packet[271] = 0xff;
    }
    else
    {
        for (i = 0; i < 10; i++)
        {
            crcdata[i] = packet[i + 2];
        }

        crcitt = crcFpga(crcdata, 10);
        packet[12] = crcitt / 256;
        packet[13] = crcitt % 256;
        packet[14] = 0xff;
        packet[15] = 0xff;
    }

#if PRINT_FOR_DEBUG_1 == 1
    if (sModule1 == 0x00)
    {
        printf("com2(0x%x) >> ", nCmd);
        for (i = 0; i < 16; i++)
        {
            printf("%02x ", packet[i]);
        }
        printf("\n");
    }
    else
    {
        printf("com1(0x%x) >> ", nCmd);
        for (i = 0; i < 16; i++)
        {
            printf("%02x ", packet[i]);
        }
        printf("\n");
    }
    fflush(0);
#endif

    if (struGsh.nSerialCount1 > 250)
    {
        struGsh.nSerialCount1 = 0;
    }

    if (sModule1 == 0x00)
    { // 控制板使用COM2
        for (i = 0; i < nCount; i++)
        {
            myPort2->readAll();
            if (nCmd == COM_CTRL_UPDATE_SEND_FILE)
            {
                myPort2->write(packet, 272);
            }
            else
            {
                myPort2->write(packet, 16);
            }
        }
    }
    else
    {    // 接口板、前置板使用COM1
        for (i = 0; i < nCount; i++)
        {
            myPort1->readAll();
            if (nCmd == COM_SEND_FILE)
            {
                myPort1->write(packet, 272);
            }
            else
            {
                myPort1->write(packet, 16);
            }
        }
    }
    return 0;
}


/* 远程升级读操作 */
int MyQextSerialPort::fpgaUpdateRead(char* sBuf, int len, bool bIsCtrl)
{
    int countRead = 0, countRead2 = 0, i;
    uint crcitt = 0x00000000;
    char crcData[14];

    memset(sBuf, 0, len);
    memset(crcData, 0, 14);

    if (bIsCtrl)
        countRead = myPort2->read(sBuf, len);
    else
        countRead = myPort1->read(sBuf, len);

    if ((countRead > 0) && (countRead < len))
    {
        myFlow.msleep(20);
        if (bIsCtrl)
        {
            countRead2 = myPort2->read(sBuf + countRead, len - countRead);
        }
        else
        {
            countRead2 = myPort1->read(sBuf + countRead, len - countRead);
        }
    }

#if PRINT_FOR_DEBUG_1 == 1       //打印开关
    printf("com1 << %d, %d \n", countRead, countRead2);
    for (i = 0; i < countRead + countRead2; i++)
    {
        printf("%02x ", sBuf[i]);
    }
    printf("\n");
    fflush(0);
#endif

    if (len == PACKET_SIZE)
    {
        //! CRC校验判断
        if ((sBuf[15] & 0x08) == 0x08)
        {
            return 0;
        }

        for (i = 0; i < 14; i++)
        {
            crcData[i] = sBuf[i + 4];
        }
        crcitt = crcFpga(crcData, 14);
        crcitt = crcitt & 0x0000ffff;

        if ((crcitt / 256 == sBuf[18]) && (crcitt % 256 == sBuf[19]))
        {
            return countRead + countRead2;
        }
        else
        {
            memset(sBuf, 0, len);
            return 0;
        }
    }

    memset(sBuf, 0, len);
    return countRead + countRead2;
}
