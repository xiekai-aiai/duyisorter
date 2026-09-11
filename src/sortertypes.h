#ifndef SORTERTYPES_H
#define SORTERTYPES_H

#include <QString>
#include <QVector>

#define UPD_CMD_PORT 19900

// A类包包头和包尾
#define PKGA_HEAD0 0xA5
#define PKGA_HEAD1 0x5A
#define PKGA_TAIL0 0xFF
#define PKGA_TAIL1 0xFF
// 最小A类包长度
#define MIN_PKGA_LEN 12

// A类包编码
// 命令编码
#define CMD_CODE_COLLECT 0x0001                // 采集命令
#define CMD_CODE_GET_VERSION 0x0002            // 获取版本号命令
#define CMD_CODE_PIXEL 0x0003                  // 像元信息
#define CMD_CODE_MODEL_APPLY 0x0004            // 模型应用命令
#define CMD_CODE_MODEL_PARAM 0x0005            // 模型参数命令
#define CMD_CODE_DISK_SPACE 0x0006             // 磁盘空间命令
#define CMD_CODE_INFER 0x0007                  // 推理命令
#define CMD_CODE_IMG_PARAM 0x0008              // 图像参数命令
#define CMD_CODE_JETS_PARAM 0x0009             // 喷嘴参数命令
#define CMD_CODE_START_FIXED 0x000A            // 启动定检
#define CMD_CODE_STOP_FIXED 0x000B             // 停止定检
#define CMD_CODE_START_ORDER 0x000C            // 启动顺检
#define CMD_CODE_STOP_ORDER 0x000D             // 停止顺检
#define CMD_CODE_VALVE_MODE 0x000E             // 喷阀模式命令
#define CMD_CODE_VIWE 0x000F                   // 预览命令
#define CMD_CODE_EMULATE 0x0010                // 仿真命令
#define CMD_CODE_IP_MODIFY 0x0011              // ip修改命令
#define CMD_CODE_AREA_SELECT 0x0012            // 面积选择命令
#define CMD_CODE_AI_STATUS 0x0013              // AI状态命令

// 命令长度
#define CMD_LEN_COLLECT 3                     // 采集命令
#define CMD_LEN_GET_VERSION 0                 // 获取版本号命令
#define CMD_LEN_PIXEL 6                       // 像元信息
#define CMD_LEN_DISK_SPACE 0                  // 磁盘空间命令
#define CMD_LEN_INFER 1                       // 推理命令
#define CMD_LEN_MODEL_PARAM 2                 // 模型参数
#define CMD_LEN_IMG_PARAM 10                  // 图像参数命令
#define CMD_LEN_JETS_PARAM 6                  // 喷阀参数命令
#define CMD_LEN_START_FIXED 4                 // 启动定检
#define CMD_LEN_STOP_FIXED 2                  // 停止定检
#define CMD_LEN_START_ORDER 4                 // 启动顺检
#define CMD_LEN_STOP_ORDER 2                  // 停止顺检
#define CMD_LEN_VALVE_MODE 1                  // 喷阀模式命令
#define CMD_LEN_VIWE 1                        // 预览命令
#define CMD_LEN_IP_MODIFY 13                  // ip修改命令
#define CMD_LEN_AREA_SELECT 4                 // 面积选择命令

// 错误编码
#define CMD_CODE_FAIL 0                       // 错误编码
#define CMD_CODE_SUCCESS 1                    // 成功编码

// 命令包信息
typedef struct __cmd_package_ {
    quint16 addr_;                             // 模块地址
    quint16 cmd_;                              // 命令编码
    quint16 cmd_len_;                          // 命令长度
    QByteArray cmd_data_;                      // 命令数据
    quint16 crc_;                              // crc校验数据
} CmdPackage;

// AI服务的版本号
typedef struct __version_info_{
    QString version_;                          // 版本信息
} VersionInfo;

// AI服务器的磁盘空间
typedef struct __ai_space_{
    quint32 space_;                            // 磁盘空间，单位M
} SpaceInfo;

// AI状态信息
typedef struct __ai_status_info_ {
    quint32 discard_num_;                      // 丢包数量
    quint16 ai_cost_;                          // AI耗时，单位微秒
    quint32 timeout_num_;                      // 超时数量
} AiStatusInfo;

// 图像采集参数
typedef struct __img_collect_param_ {
    quint8 start_;                             // 启动标识 0：停止 1：启动
    quint16 num_;                              // 采集张数
} ImgCollectParam;

// AI通用响应信息
typedef struct __ai_comm_response_ {
    quint8 code_;                              // 0： 失败 1： 成功
} AiCommResponse;

// 像元信息
typedef struct __ai_pixel_info_ {
    quint8 type_;                              // 像元类型
    quint8 cam_no_;                            // 相机编号
    quint16 begin_pixel_;                      // 起始像元
    quint16 end_pixel_;                        // 结束像元
} AiPixelInfo;

// 模型参数
typedef struct __model_param_
{
    quint8 cls_id_;                            // 模型分类ID
    quint8 threshold_;                         // 阈值, 取值范围0~100
} ModelParam;

// 模型应用
typedef struct __model_apply_{
    QString model_name_;                       // 模型名称
} ModelApply;

// 图像推理
typedef struct __img_infer_param_ {
    quint8 start_;                             // 启动标识 0：停止 1：启动
} ImgInferParam;


// 图像高度参数
typedef struct __img_height_param_
{
    quint16 infer_height_;                     // 推理图像高度
    quint16 collect_height_;                   // 采集图像高度
    quint16 img_view_height_;                  // 图像显示高度
    quint16 video_view_height_;                // 视频显示高度
    quint16 sliding_step_;                     // 滑动步长高度, 0表示不采用滑动步长
} ImgHeightParam;

// 喷阀参数
typedef struct __jets_param_
{
    quint8 jets_num_;                          // 喷嘴数
    quint8 blow_time_;                         // 吹气时间(0.1 ms)
    quint16 fixed_delay_;                      // 固定延时时间(0.1 ms)
    quint16 dynamics_delay_;                   // 动态延时时间(0.1 ms)
} JetsParam;

// 定检参数
typedef struct __fixed_check_param_
{
    quint8 valve_no_;                          // 定检喷阀编号
    quint8 injector_no_;                       // 喷嘴编号
    quint16 interval_;                         // 定检间隔时间(ms)
} FixedCheckParam;

// 顺检参数
typedef struct __order_check_param_
{
    quint8 valve_no_;                         // 顺检喷阀编号
    quint8 injector_no_;                      // 起始喷嘴编号
    quint16 interval_;                        // 定检间隔时间(ms)
} OrderCheckParam;

// 仿真参数
typedef struct __emulate_param_
{
    QString model_name_;                      // 模型名称
    QString img_name_;                        // 图像路径
} EmulateParam;

// 目标信息
typedef struct __obj_info_{
    quint8 cls_id_;                           // 类别
    quint8 score_;                            // 得分
    quint16 x_;
    quint16 y_;
    quint16 w_;
    quint16 h_;
} ObjInfo;

// 仿真结果
typedef struct __emulate_res_info_ {
    quint8 pic_len_;                          // 图片名称长度
    QString pic_name_;                        // 图片名称
    quint8 code_;                             // 0： 失败 1： 成功
    QVector<ObjInfo> obj_infos_;              // 目标信息
} EmulateResInfo;

// 图片视频预览参数
typedef struct __view_param_ {
    quint8 flag_;                             // 0: 图片预览 1: 启动视频预览 2: 停止视频预览
} ViewParam;

// 面积参数
typedef struct __area_param_ {
    quint16 min_area_;                        // 最小面积
    quint16 bg_distance_;                     // 背景距离阈值
} AreaParam;


#endif // SORTERTYPES_H
