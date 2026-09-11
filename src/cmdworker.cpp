#include "cmdworker.h"
#include <QtEndian>
#include <QDebug>
#include "unilog.h"

namespace cmdworker {

quint16 XMODEM_CRC16(const QByteArray &data)
{
    quint16 crc = 0;

    for (char byte : data)
    {
        quint8 value = static_cast<quint8>(byte);

        for (quint8 mask = 0x80; mask != 0; mask >>= 1)
        {
            if (crc & 0x8000)
            {
                crc <<= 1;
                crc ^= 0x1021;
            }
            else
            {
                crc <<= 1;
            }

            if (value & mask)
            {
                crc ^= 0x1021;
            }
        }
    }

    return crc;
}

bool ParseCmdPkg(const QByteArray& cmd, CmdPackage& cmd_pkg)
{
    LOG_INFO_STM("cmd response:" << cmd.toHex(' ').toUpper().toStdString());
    if(cmd.length() < MIN_PKGA_LEN) {
        LOG_ERROR_STM("cmd length:[" << cmd.length() << "] < " << MIN_PKGA_LEN);
        return false;
    }

    // 两个字节的包头
    if((static_cast<quint8>(cmd.at(0)) != PKGA_HEAD0) || (static_cast<quint8>(cmd.at(1)) != PKGA_HEAD1)) {
        LOG_ERROR_STM("package head error, head[0]:" << static_cast<quint8>(cmd.at(0)) << ", head[1]:"
                      << static_cast<quint8>(cmd.at(1)) << " != 0xA5 0x5A");
        return false;
    }

    // 两个字节的模块地址
    int idx = 2;
    cmd_pkg.addr_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(cmd.constData() + idx));
    idx += 2;
    // 两个字节的命令编码
    cmd_pkg.cmd_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(cmd.constData() + idx));
    idx += 2;
    // 两个字节的命令长度
    cmd_pkg.cmd_len_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(cmd.constData() + idx));
    idx += 2;

    if(cmd.length() < MIN_PKGA_LEN + cmd_pkg.cmd_len_) {
        LOG_ERROR_STM("cmd length:[" << cmd.length() << "] < " << MIN_PKGA_LEN << " + " << cmd_pkg.cmd_len_);
        return false;
    }

    // 命令数据
    cmd_pkg.cmd_data_ = cmd.mid(idx, cmd_pkg.cmd_len_);
    idx += cmd_pkg.cmd_len_;
    // 两个字节的crc
    cmd_pkg.crc_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(cmd.constData() + idx));
    return true;
}

QByteArray VersionRequest()
{
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_GET_VERSION;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = 0;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

VersionInfo VersionResponse(const CmdPackage& cmd_pkg)
{
    if(cmd_pkg.cmd_data_.isEmpty()) {
        LOG_ERROR_STM("cmd data is empty");
        return VersionInfo();
    }

    VersionInfo version;
    version.version_ = QString::fromUtf8(cmd_pkg.cmd_data_);
    return version;
}

QByteArray DiskSpaceRequest()
{
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_DISK_SPACE;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = 0;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

SpaceInfo DiskSpaceResponse(const CmdPackage& pkg)
{
    if(pkg.cmd_data_.isEmpty()) {
        LOG_ERROR_STM("cmd data is empty");
        return SpaceInfo();
    }

    SpaceInfo space;
    space.space_ =  qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData()));
    return space;
}

QByteArray AiStatusRequest() {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_AI_STATUS;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = 0;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 解析AI状态响应
AiStatusInfo AiStatusResponse(const CmdPackage& pkg) {

    AiStatusInfo info;
    if(pkg.cmd_data_.isEmpty()) {
        LOG_ERROR_STM("cmd data is empty");
        return info;
    }

    int idx = 0;
    // 4个字节的丢包数量
    info.discard_num_ = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
    idx += 4;
    // 2个字节的推理耗时
    info.ai_cost_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
    idx += 2;
    // 4个字节的动态延迟数量
    info.timeout_num_ = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
    return info;
}

// 图像采集请求
QByteArray ImgCollectRequest(const ImgCollectParam& param) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_COLLECT;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_COLLECT;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 1个字节的启动标识
    request.append(param.start_);
    // 2个字节的图片张数
    quint16 netPicNum = qToBigEndian(param.num_);
    request.append(reinterpret_cast<const char*>(&netPicNum), sizeof(netPicNum));
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 图像采集响应
AiCommResponse CommResponse(const CmdPackage& pkg) {
    AiCommResponse res;
    if(pkg.cmd_data_.isEmpty()) {
        LOG_ERROR_STM("cmd data is empty");
        res.code_ = 0;
        return res;
    }

    res.code_ = static_cast<quint8>(pkg.cmd_data_.at(0));
    return res;
}

// 像元请求
QByteArray PixelInfoRequest(const AiPixelInfo& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_PIXEL;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_PIXEL;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 1个字节的类型
    request.append(info.type_);
    // 1个字节的相机编号
    request.append(info.cam_no_);
    // 2个字节的起始像素
    quint16 netBeginPix = qToBigEndian(info.begin_pixel_);
    request.append(reinterpret_cast<const char*>(&netBeginPix), sizeof(netBeginPix));
    // 2个字节的结束像素
    quint16 netEndPix = qToBigEndian(info.end_pixel_);
    request.append(reinterpret_cast<const char*>(&netEndPix), sizeof(netEndPix));
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 模型应用
QByteArray ModelApplyRequest(const ModelApply& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_MODEL_APPLY;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = info.model_name_.toUtf8().length();
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 模型名称
    request.append(info.model_name_.toUtf8());
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 模型参数
QByteArray ModelParamRequest(const QVector<ModelParam>& info)
{
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_MODEL_PARAM;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_MODEL_PARAM * info.size();
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 模型参数
    for(const ModelParam& item: info) {
        request.append(item.cls_id_);
        request.append(item.threshold_);
    }

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 图像推理
QByteArray ImgInferRequest(const ImgInferParam& info)
{
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_INFER;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_INFER;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));
    // 1个字节的推理参数
    request.append(info.start_);
    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 图像高度
QByteArray ImgHeightRequest(const ImgHeightParam& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_IMG_PARAM;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_IMG_PARAM;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 2个字节的采集高度
    quint16 netCollH = qToBigEndian(info.collect_height_);
    request.append(reinterpret_cast<const char*>(&netCollH), sizeof(netCollH));
    // 2个字节的推理高度
    quint16 netInfH = qToBigEndian(info.infer_height_);
    request.append(reinterpret_cast<const char*>(&netInfH), sizeof(netInfH));
    // 2个字节的图片高度
    quint16 netImgH = qToBigEndian(info.img_view_height_);
    request.append(reinterpret_cast<const char*>(&netImgH), sizeof(netImgH));
    // 2个字节的视频高度
    quint16 netVideoH = qToBigEndian(info.video_view_height_);
    request.append(reinterpret_cast<const char*>(&netVideoH), sizeof(netVideoH));
    // 2个字节的高精度复选
    quint16 netSlidH = qToBigEndian(info.sliding_step_);
    request.append(reinterpret_cast<const char*>(&netSlidH), sizeof(netSlidH));

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 喷阀参数
QByteArray JetsParamRequest(const JetsParam& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_JETS_PARAM;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_JETS_PARAM;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 1个字节的喷嘴个数
    request.append(info.jets_num_);
    // 2个字节的固定延迟
    quint16 netFixDelay = qToBigEndian(info.fixed_delay_);
    request.append(reinterpret_cast<const char*>(&netFixDelay), sizeof(netFixDelay));
    // 2个字节的动态延迟
    quint16 netDyDelay = qToBigEndian(info.dynamics_delay_);
    request.append(reinterpret_cast<const char*>(&netDyDelay), sizeof(netDyDelay));
    // 1个字节的吹气时间
    request.append(info.blow_time_);

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 启动定检
QByteArray StartFixCheckRequest(const FixedCheckParam& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_START_FIXED;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_START_FIXED;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 1个字节的喷嘴编号
    request.append(info.valve_no_);
    // 1个字节的喷嘴编号
    request.append(info.injector_no_);
    // 2个字节的喷阀间隔
    quint16 netInterval = qToBigEndian(info.interval_);
    request.append(reinterpret_cast<const char*>(&netInterval), sizeof(netInterval));

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 停止定检
QByteArray StopFixCheckRequest(const FixedCheckParam& info){
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_STOP_FIXED;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_STOP_FIXED;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 1个字节的喷嘴编号
    request.append(info.valve_no_);
    // 1个字节的喷嘴编号
    request.append(info.injector_no_);

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 启动顺检
QByteArray StartOrderCheckRequest(const OrderCheckParam& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_START_ORDER;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_START_ORDER;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 1个字节的喷嘴编号
    request.append(info.valve_no_);
    // 1个字节的喷嘴编号
    request.append(info.injector_no_);
    // 2个字节的喷阀间隔
    quint16 netInterval = qToBigEndian(info.interval_);
    request.append(reinterpret_cast<const char*>(&netInterval), sizeof(netInterval));

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 停止顺检
QByteArray StopOrderCheckRequest(const OrderCheckParam& info){
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_STOP_ORDER;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_STOP_ORDER;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 1个字节的喷嘴编号
    request.append(info.valve_no_);
    // 1个字节的喷嘴编号
    request.append(info.injector_no_);

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 预览请求
QByteArray ViewParamRequest(const ViewParam& info) {
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_VIWE;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_VIWE;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 1个字节的预览参数
    request.append(info.flag_);

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

// 仿真请求
QByteArray EmulateParamRequest(const EmulateParam& info){
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_EMULATE;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = info.img_name_.toUtf8().length() + info.model_name_.toUtf8().length() + 1;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 模型名称
    request.append(info.model_name_.toUtf8());
    // 分隔符
    request.append((char)',');
    // 图片名称
    request.append(info.img_name_.toUtf8());

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

EmulateResInfo EmulateParamResponse(const CmdPackage& pkg)
{
    EmulateResInfo emu_info;
    if(pkg.cmd_data_.isEmpty()) {
        LOG_ERROR_STM("cmd data is empty");
        return emu_info;
    }

    int idx = 0;
    // 1个字节的图片名称长度
    emu_info.pic_len_  = static_cast<quint8>(pkg.cmd_data_.at(idx));
    idx += 1;
    // 图片长度
    emu_info.pic_name_ = QString::fromUtf8(pkg.cmd_data_.mid(idx, emu_info.pic_len_));
    idx += emu_info.pic_len_;
    // 1个字节的结果
    emu_info.code_ = static_cast<quint8>(pkg.cmd_data_.at(idx));
    idx += 1;

    while(idx < pkg.cmd_len_) {
        ObjInfo obj_info;
        // 1个字节的类别
        obj_info.cls_id_ =  static_cast<quint8>(pkg.cmd_data_.at(idx));
        idx += 1;
        // 1个字节的得分
        obj_info.score_ =  static_cast<quint8>(pkg.cmd_data_.at(idx));
        idx += 1;
        // 2个字节的x坐标
        obj_info.x_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
        idx += 2;
        // 2个字节的y坐标
        obj_info.y_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
        idx += 2;
        // 2个字节的w
        obj_info.w_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
        idx += 2;
        // 2个字节的h
        obj_info.h_ = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(pkg.cmd_data_.constData() + idx));
        idx += 2;
        emu_info.obj_infos_.push_back(obj_info);
    }
    return emu_info;
}

// 面积选择阈值
QByteArray AreaParamReqeust(const AreaParam& info)
{
    QByteArray request;
    // 2个字节的包头
    request.append(PKGA_HEAD0);
    request.append(PKGA_HEAD1);
    // 2个字节的模块地址
    quint16 addr = 0;
    quint16 netAddr = qToBigEndian(addr);
    request.append(reinterpret_cast<const char*>(&netAddr), sizeof(netAddr));
    // 2个字节的命令编码
    quint16 cmd = CMD_CODE_AREA_SELECT;
    quint16 netCmd = qToBigEndian(cmd);
    request.append(reinterpret_cast<const char*>(&netCmd), sizeof(netCmd));
    // 2个字节的命令长度
    quint16 len = CMD_LEN_AREA_SELECT;
    quint16 netLen = qToBigEndian(len);
    request.append(reinterpret_cast<const char*>(&netLen), sizeof(netLen));

    // 2个字节的背景距离阈值
    quint16 netDis = qToBigEndian(info.bg_distance_);
    request.append(reinterpret_cast<const char*>(&netDis), sizeof(netDis));
    // 2个字节的最小面积
    quint16 netMinArea = qToBigEndian(info.min_area_);
    request.append(reinterpret_cast<const char*>(&netMinArea), sizeof(netMinArea));

    // 2个字节的crc
    quint16 crc = XMODEM_CRC16(request.mid(2, request.length() - 2));
    quint16 netCrc = qToBigEndian(crc);
    request.append(reinterpret_cast<const char*>(&netCrc), sizeof(netCrc));
    // 2个字节的包尾
    request.append(PKGA_TAIL0);
    request.append(PKGA_TAIL1);
    return request;
}

}
