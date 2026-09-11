#ifndef CMDWORKER_H
#define CMDWORKER_H

#include <QByteArray>
#include "sortertypes.h"

namespace cmdworker {
// 解析命令包
bool ParseCmdPkg(const QByteArray& cmd, CmdPackage& cmd_pkg);

// 构建版本查询消息
QByteArray VersionRequest();

// 解析版本响应消息
VersionInfo VersionResponse(const CmdPackage& pkg);

// 构建磁盘空间查询消息
QByteArray DiskSpaceRequest();

// 解析磁盘空间响应消息
SpaceInfo DiskSpaceResponse(const CmdPackage& pkg);

// AI状态查询消息
QByteArray AiStatusRequest();

// 解析AI状态响应
AiStatusInfo AiStatusResponse(const CmdPackage& pkg);

// 图像采集请求
QByteArray ImgCollectRequest(const ImgCollectParam& param);

// 通用响应
AiCommResponse CommResponse(const CmdPackage& pkg);

// 像元请求
QByteArray PixelInfoRequest(const AiPixelInfo& info);

// 模型应用
QByteArray ModelApplyRequest(const ModelApply& info);

// 模型参数
QByteArray ModelParamRequest(const QVector<ModelParam>& info);

// 图像推理
QByteArray ImgInferRequest(const ImgInferParam& info);

// 图像高度
QByteArray ImgHeightRequest(const ImgHeightParam& info);

// 喷阀参数
QByteArray JetsParamRequest(const JetsParam& info);

// 启动定检
QByteArray StartFixCheckRequest(const FixedCheckParam& info);

// 停止定检
QByteArray StopFixCheckRequest(const FixedCheckParam& info);

// 启动顺检
QByteArray StartOrderCheckRequest(const OrderCheckParam& info);

// 停止顺检
QByteArray StopOrderCheckRequest(const OrderCheckParam& info);

// 预览请求
QByteArray ViewParamRequest(const ViewParam& info);

// 仿真请求
QByteArray EmulateParamRequest(const EmulateParam& info);

// 仿真响应
EmulateResInfo EmulateParamResponse(const CmdPackage& pkg);

// 面积选择阈值
QByteArray AreaParamReqeust(const AreaParam& info);

}

#endif // CMDWORKER_H
