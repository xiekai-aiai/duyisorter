/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myqextserialport.h
 * \brief       串口操作头文件
 * \date        2015.01.14
 */
#ifndef MYQEXTSERIALPORT_H
#define MYQEXTSERIALPORT_H

#include "3rdparty/qextserialport/qextserialport.h"          // 串口类头文件

#include "globalparams.h"
#include "globalconfig.h"
#include "globalflow.h"

#define MY_UARTBAUD_115200  115200
#define MY_UARTBAUD_921600  921600

#define TIMEOUT         100
#define FRAME_LEN       256
#define PACKET_SIZE     24
#define FRAME_LEN_MAIZE 8192

/*  模块类型 */
#define INT     0x01
#define UNIT	0x02

#define CAMERA_COLOR    1
#define CAMERA_INFRA    2
#define CAMERA_ASSIST   3

#define CTRL_GENERAL    0
#define CTRL_LED        1

/* 接口板参数协议 */
#define CMD_INT_ONOFF                  0x01             // 运行开始/停止
#define CMD_INT_EJTIME                 0x02             // 喷阀时间
#define CMD_INT_EJTEST                 0x03             // 喷阀自检
#define CMD_INT_EJECT_WIDTH            0x04             // 吹气模式
#define CMD_INT_EJECTOR_SET            0x05             // 喷嘴数量
#define CMD_INT_EJECTOR_COUNT          0x06             // 吹气频率
#define CMD_INT_FRONT_REAR_RELATION    0x07             // 前后视算法关系
#define CMD_INT_IMAGE_CAPTURE_MODE     0x08             // 图像获取方式
#define CMD_INT_VIDEO_IMAGE            0x11             // 视频图像获取
#define CMD_INT_IMAGE_ERROR_REVISE     0x13             // 图像获取出错，FPGA纠错
#define CMD_INT_VALVE_COMBIN_TYPE      0x14             // 增加LD机型接口板协议
#define CMD_INT_EJECT_TRUNCATION       0x15             // 消磁时间和截断时间
#define CMD_INT_LED_MODE               0x16             // LED指示灯模式
#define CMD_INT_VERSION                0x2001           // 版本查询
#define CMD_INT_SMS_WRITE              0x3001           // 短信模块发送数据
#define CMD_INT_SMS_READ               0x3002           // 短信模块接收数据
#define CMD_INT_POSITION_BIAS          0x3200           // 对位偏置数据下发
#define CMD_INT_MASTER_SLAVE_RELATION  0x3201           // 三次反选中主配辅配逻辑关系
#define CMD_INT_TICK_MODE              0x3210           // 剔除模式下发
									
/* 彩色相机板参数协议 */
#define CMD_UNIT_ONOFF                  0x01            // 运行开始/停止
#define CMD_UNIT_UPDATE                 0x02            // 校正开始/停止
#define CMD_UNIT_IMAGE                  0x03            // 图像获取
#define CMD_UNIT_CAMERA                 0x04            // 光源标定
#define CMD_UNIT_CHANNEL                0x05            // 通道信息
#define CMD_UNIT_GREY_A                 0x06            // 灰度杂质 A
#define CMD_UNIT_BURNT_ENABLE           0x48            // 玉米焦糊使能
#define CMD_UNIT_GREY_B                 0x07            // 灰度杂质 B
#define CMD_UNIT_DISCOLOR_A_1           0x08            // 色差杂质 A-1
#define CMD_UNIT_DISCOLOR_A_2           0x09            // 色差杂质 A-2
#define CMD_UNIT_DISCOLOR_B_1           0x0A            // 色差杂质 B-1
#define CMD_UNIT_DISCOLOR_B_2           0x0B            // 色差杂质 B-2
#define CMD_UNIT_ROW_FREQUENCY          0x0C            // 行频设置
#define CMD_UNIT_BIASING                0x0D            // 彩色相机偏置
#define CMD_UNIT_ANA_GAIN               0x0E            // 彩色相机模拟增益
#define CMD_UNIT_DIGI_GAIN_QUICK        0x0F            // 彩色相机数字增益粗调
#define CMD_UNIT_DIGI_GAIN_INCH         0x10            // 彩色相机数字增益细调
#define CMD_UNIT_PIXEL_COMBINE          0x11            // 像素合并（已停用）
#define CMD_UNIT_BG_COLOR               0x12            // 背景设置
#define CMD_UNIT_BG_ADJUST              0x86            // 背景设置自适应
#define CMD_UNIT_SPACE_UPDATE           0x13            // 彩色相机空间校正(已停用)
#define CMD_UNIT_RESERVED_1             0x14            // 算法关联保留参数 1
#define CMD_UNIT_RESERVED_2             0x15            // 算法关联保留参数 2
#define CMD_UNIT_SCALE_PARAMS           0x16            // 大小参数
#define CMD_UNIT_SCALE_B_PARAMS         0x4010          // 大小参数B
#define CMD_UNIT_BAD_NUM                0x17            // 坏点个数
#define CMD_UNIT_EJECT_ONOFF            0x18            // 喷阀开关
#define CMD_UNIT_EDGE_CUT               0x19            // 边缘切除

#define CMD_UNIT_CROSS_PARAMS           0x1B            // 差分参数
#define CMD_UNIT_INFECT                 0x1C            // 传染算法

#define CMD_UNIT_EARTHPEA_PARAMS1_MISC	0x1E            // 花生选芽1 参数
#define CMD_UNIT_EARTHPEA_PARAMS1_AREA	0x1F            // 花生选芽1 面积
#define CMD_UNIT_EARTHPEA_PARAMS2_MISC	0x20            // 花生选芽2 参数
#define CMD_UNIT_EARTHPEA_PARAMS2_AREA	0x21            // 花生选芽2 面积
#define CMD_UNIT_SHAPE                  0x22            // 形状算法
#define CMD_UNIT_INTEL_SET              0x23            // 智能模式物料设置
#define CMD_UNIT_INTEL_A_1              0x24            // 智能算法A-1
#define CMD_UNIT_INTEL_A_2              0x25            // 智能算法A-2
#define CMD_UNIT_INTEL_A_3              0x26            // 智能算法A-3
#define CMD_UNIT_INTEL_B_1              0x27            // 智能算法B-1
#define CMD_UNIT_INTEL_B_2              0x28            // 智能算法B-2
#define CMD_UNIT_INTEL_B_3              0x29            // 智能算法B-3
#define CMD_UNIT_INTEL_C_1              0x2A            // 智能算法C-1
#define CMD_UNIT_INTEL_C_2              0x2B            // 智能算法C-2
#define CMD_UNIT_INTEL_C_3              0x2C            // 智能算法C-3
#define CMD_UNIT_INTEL_D_1              0x2D            // 智能算法D-1
#define CMD_UNIT_INTEL_D_2              0x2E            // 智能算法D-2
#define CMD_UNIT_INTEL_D_3              0x2F            // 智能算法D-3
#define CMD_UNIT_INTEL_RESERVED_1       0x30            // 智能算法保留参数1
#define CMD_UNIT_INTEL_RESERVED_2       0x31            // 智能算法保留参数2

//智能算法A、B、C、D的R、G、B和常数项的值
#define CMD_UNIT_INTEL_R              0x4023          // 智能算法-R值
#define CMD_UNIT_INTEL_G              0x4024          // 智能算法-G值
#define CMD_UNIT_INTEL_B              0x4025          // 智能算法-B值
#define CMD_UNIT_INTEL_C              0x4026          // 智能算法-常数项值

#define CMD_UNIT_EJECTOR_SET            0x1D            // 喷嘴数量
#define CMD_UNIT_CHANNEL_EJECTOR        0x34            // 喷嘴像素

#define CMD_UNIT_SENS_T_MODE            0x35            // 东芝传感器模式
#define CMD_UNIT_IMG_BURST              0x36            // 智能算法触发阈值
#define CMD_UNIT_REVERSE_SORT           0x37            // 物料算法正反选方式
#define CMD_UNIT_MATERIAL_SIZE_MAX      0x38            // 物料算法最大行列尺寸

#define CMD_UNIT_MELON_WHITE_SENS       0x39            // 西瓜子算法白色灵敏度
#define CMD_UNIT_MELON_EDGE_RED         0x3A            // 西瓜子算法边缘发红粒
#define CMD_UNIT_MELON_EDGE_DAMAGED     0x3B            // 西瓜子算法边缘破损粒
#define CMD_UNIT_MELON_RUGGED_1         0x3C            // 西瓜子算法麻粒1
#define CMD_UNIT_MELON_RUGGED_2         0x3D            // 西瓜子算法麻粒2
#define CMD_UNIT_MELON_AI_AREA          0x3E            // 西瓜子算法时多分类考虑边缘和整体， 0：整体， 1：边缘
#define CMD_UNIT_SHAPE_LENGTH           0x3f            // 长度算法
#define CMD_UNIT_MAIZE_PARAMS_1         0x42            // 玉米算法参数1
#define CMD_UNIT_MAIZE_PARAMS_2         0x43            // 玉米算法参数2
#define CMD_UNIT_MAIZE_PARAMS_3         0x44            // 玉米算法参数3
#define CMD_UNIT_MAIZE_IMAGE_UPLOAD     0x45            // 玉米机型图像上传开始

#define CMD_UNIT_VIDEO_IMAGE            0x46            // 视频图像获取

#define CMD_UINT_EJTIME_BIAS            0x47            // 相机板针对延迟时间做微调

#define CMD_UNIT_CANDY                  0x49            // 糖果算法
#define CMD_UNIT_PISTACHIO              0x4A            // 开心果算法
#define CMD_UNIT_ARITH_EDGE_CUT         0x4B            // 算法边缘切除

#define CMD_UNIT_MOTOR_CTRL             0x53            // 点击控制
#define CMD_UNIT_SET_FRAME              0x54            // 帧起点设置
#define CMD_UNIT_PMT_COLOR              0x55            // PMT颜色设置

#define CMD_UNIT_HUFF_WIDTH             0x66            // 吹气宽度

#define CMD_UNIT_INTEL_DFL_SIZE         0x70            // 多分类-尺寸、纯度
#define CMD_UNIT_INTEL_DFL_CONST        0x71            // 多分类-常数项
#define CMD_UNIT_INTEL_DFL_RED          0x72            // 多分类-红
#define CMD_UNIT_INTEL_DFL_GREEN        0x73            // 多分类-绿
#define CMD_UNIT_INTEL_DFL_BLUE         0x74            // 多分类-蓝
#define CMD_UNIT_INTEL_DFL_RG           0x75            // 多分类-红绿
#define CMD_UNIT_INTEL_DFL_RB           0x76            // 多分类-红蓝
#define CMD_UNIT_INTEL_DFL_GB           0x77            // 多分类-绿蓝
#define CMD_UNIT_INTEL_DFL_RR           0x78            // 多分类-红红
#define CMD_UNIT_INTEL_DFL_GG           0x79            // 多分类-绿绿
#define CMD_UNIT_INTEL_DFL_BB           0x7A            // 多分类-蓝蓝
#define CMD_UNIT_LED_MODE               0x7B            // LED指示灯模式

#define CMD_UNIT_SHAPE_POLE             0x90            // 选杆模式（仅限于茶叶机型）
#define CMD_UNIT_SHAPE_POLE_A           0x90            // 选细杆模式（仅限于茶叶机型）
#define CMD_UNIT_SHAPE_POLE_A_RESERVED  0x91            // 选细杆保留（仅限于茶叶机型）
#define CMD_UNIT_SHAPE_POLE_B           0x94            // 选粗杆模式（仅限于茶叶机型）
#define CMD_UNIT_SHAPE_SLICE            0x95            // 选片模式（仅限于茶叶机型）
#define CMD_UNIT_SHAPE_LEAF             0x96            // 选芽模式（仅限于茶叶机型）
#define CMD_UNIT_INTEL_RESERVED         0x97            // 智能算法保留（仅限于茶叶机型）

#define CMD_UNIT_PEANUT_A               0xAAAA          // 花生机专用：A 类杂质
#define CMD_UNIT_PEANUT_B               0xBBBB          // 花生机专用：B 类杂质
#define CMD_UNIT_PEANUT_C               0xCCCC          // 花生机专用：C 类杂质
#define CMD_UNIT_PEANUT_D               0xDDDD          // 花生机专用：D 类杂质
#define CMD_UNIT_CAMERA_VERSION         0x2001          // 相机板版本查询
#define CMD_UNIT_OBTAIN_RESULT          0x2003          //! 获取前置板识别结果

#define CMD_UNIT_GET_LIGHT              0x0083          // 获取物料亮度值
#define CMD_UNIT_ADJ_COEF               0x008A          // 发送校正系数
#define CMD_UNIT_ADJ_ON_OFF             0x008B          // 发送校正开关
#define CMD_UNIT_IMAGE_ERROR_REVISE     0x0065          // 图像获取出错，FPGA纠错

/* 区分传感器和帧频，新增识别算法命令编号 */
#define CMD_UNIT_NEW_GREY_A_1                 0x4001            // 灰度杂质 A
#define CMD_UNIT_NEW_GREY_A_2                 0x4002            // 灰度杂质 A
#define CMD_UNIT_NEW_GREY_B_1                 0x4003            // 灰度杂质 B
#define CMD_UNIT_NEW_GREY_B_2                 0x4004            // 灰度杂质 B
#define CMD_UNIT_NEW_DISCOLOR_A_1             0x4005            // 色差杂质 A-1
#define CMD_UNIT_NEW_DISCOLOR_A_2             0x4006            // 色差杂质 A-2
#define CMD_UNIT_NEW_DISCOLOR_B_1             0x4007            // 色差杂质 B-1
#define CMD_UNIT_NEW_DISCOLOR_B_2             0x4008            // 色差杂质 B-2
#define CMD_UNIT_NEW_SCALE_PARAMS             0x4009            // 大小参数
#define CMD_UNIT_NEW_CROSS_PARAMS_1           0x400A            // 差分参数
#define CMD_UNIT_NEW_CROSS_PARAMS_2           0x400B            // 差分参数

#define CMD_UNIT_NEW_EARTHPEA_PARAMS1         0x400C            // 花生选芽1 参数
#define CMD_UNIT_NEW_EARTHPEA_PARAMS2         0x400D            // 花生选芽2 参数

#define CMD_UNIT_NEW_INTEL_DFL_SIZE           0x400E            // 多分类-尺寸、纯度、颜色空间,大蒜机型针对智能A
#define CMD_UNIT_NEW_INTEL_DFL_SIZE_B         0x400F            // 多分类-尺寸、纯度、颜色空间,大蒜机型针对智能B
#define CMD_UNIT_NEW_INTEL_RATIO_SENS_1       0x4011            // 多分类比例智能1灵敏度
#define CMD_UNIT_NEW_INTEL_RATIO_SENS_2       0x4012            // 多分类比例智能2灵敏度
#define CMD_UNIT_NEW_INTEL_DFL_SIZE_GARLIC_1  0x4020            // 多分类-大尺寸,大蒜机型针对智能A
#define CMD_UNIT_NEW_INTEL_DFL_SIZE_GARLIC_2  0x4021            // 多分类-小尺寸,大蒜机型针对智能A
#define CMD_UNIT_RESERVED_PERCENT             0x4022            // 保留比例,对应协议中的平衡病斑


/* 红外相机板参数协议 */
#define CMD_INF_BAD                     0x1A            // 红外坏点 1(已停用)
#define CMD_INF_GAIN                    0x100           // 红外增益
#define CMD_INF_PARAMS1                 0x101           // 红外参数 1
#define CMD_INF_PARAMS2                 0x102           // 红外参数 2
#define CMD_INF_ADV                     0x32            // 红外保留（已停用）
#define CMD_INF_BIASING                 0x103           // 红外偏置
#define CMD_INF_UPDATE                  0x104           // 红外校正
#define CMD_INF_CH_BEGIN                0x105           // 红外起始象元
#define CMD_INF_CH_END                  0x106           // 红外终止象元
#define CMD_INF_AUTO_ADJUST             0x110           // 自动对齐
#define CMD_UNIT_ARITHMETIC_ENABLE      0x2002          // 算法使能

#define CMD_UNIT_AI_CAPTURE_ENABLE      0x110           //ai采集图像

/* FPGA 升级协议 */
#define COM_INT_UPDATE_STATUS           0x1001          // FPGA升级：接口板状态查询
#define COM_INT_UPDATE_BURN             0x1002          // FPGA升级：接口板烧写文件
#define COM_INT_UPDATE_USER             0x1003          // FPGA升级：接口板切换到用户模式
#define COM_INT_UPDATE_FACTORY          0x1004          // FPGA升级：接口板切换到工厂模式
#define COM_INT_FILE_LEN                0x1005          // FPGA升级：接口板文件长度
#define COM_UNIT_FILE_LEN               0x2007          // FPGA升级：相机板文件长度
#define COM_UNIT_UPDATE_STATUS          0x1005          // FPGA升级：相机板状态查询
#define COM_UNIT_UPDATE_BURN            0x1006          // FPGA升级：相机板烧写文件
#define COM_UNIT_UPDATE_USER            0x1007          // FPGA升级：相机板切换到用户模式
#define COM_UNIT_UPDATE_FACTORY         0x1008          // FPGA升级：相机板切换到工厂模式
#define COM_SEND_FILE                   0x1011          // FPGA升级：向接口板发送文件
#define COM_TRANSFER                    0X1009          // FPGA升级：转发升级文件

/* 控制板协议 */
#define CMD_CTRL_FEEDER                 0x0001          // 振动器控制
#define CMD_CTRL_LAMP                   0x0002          // 灯控
#define CMD_CTRL_WIPE                   0x0003          // 清灰刷
#define CMD_CTRL_BACKGROUND             0x0004          // 背景角度
#define CMD_CTRL_ALARM                  0x0005          // 报警设置
#define CMD_CTRL_VERSION                0x0006          // 版本查询
#define CMD_CTRL_TEMPERATURE            0x0007          // 温度获取(已停用)
#define CMD_CTRL_WARM                   0x0008          // 加热控制(已停用)
#define CMD_CTRL_MOTOR_ONOFF            0x0009          // 电机控制
#define CMD_CTRL_TEMPERATURE_ENABLE     0x000B          // 加热控制使能(已停用)
#define CMD_CTRL_WIPE_WIND              0x000C          // 清灰气帘
#define CMD_CTRL_WIPE_WATER             0x000D          // 清灰喷水时间
#define CMD_CTRL_WIPE_WATER_DELAY       0x0011          // 清灰喷水延时
#define CMD_CTRL_FEEDER_CTRL_MODE       0x0020          // 振动器外接控制模式
#define CMD_CTRL_LIFT_SWITCH            0x0021          // 提升机控制

#define CMD_CTRL_FEED_VOLTAGE           0x000F          // 振动器高低压切换， 1：高压(默认)， 0： 低压

#define COM_CTRL_UPDATE_STATUS          0x1001          // FPGA升级：查询控制板状态
#define COM_CTRL_UPDATE_FACTORY         0x1004          // FPGA升级：控制板切换至工厂模式
#define COM_CTRL_UPDATE_ERASE           0x1002          // FPGA升级：控制板擦除用户空间
#define COM_CTRL_UPDATE_SEND_FILE       0x1006          // FPGA升级：向控制板发送升级文件
#define COM_CTRL_UPDATE_USER            0x1003          // FPGA升级：控制板切换至用户模式
#define COM_CTRL_UPDATE_IS_OK           0x1007          // FPGA升级：控制板切换至用户模式是否成功


/*----------------------- 针对RS机型增加的命令编码 -------------------------*/
/* 前置板 */
#define CMD_UNIT_MATTER_NUM             0x35            // 物料代号
#define CMD_UNIT_SYSTEM_TEST            0x80            // 整机老化测试
#define CMD_UNIT_DISPLAY_ADJUST         0x81            // 校正波形显示
#define CMD_UNIT_CROSS_CHANNEL          0x82            // 交叉象元
#define CMD_UNIT_LENS_TYPE              0x84            // 镜头畸变
#define CMD_UNIT_FRAME_FRE              0x3001          // 帧频设置
#define CMD_ARITH_RICE                  0x1000          // 大米算法的命令标志

/* 控制板 */
#define CMD_CTRL_FEED_LEVEL             0x0E            // 料位数据请求
#define CMD_CTRL_WIPE_WIND_RS           0x10            // RS机型清灰气帘
#define CMD_CTRL_DETECT_WAIT            0x11            // 料位检测等待时间

/*恒流源调光板协议*/
#define CMD_CTRL_SET_LIGHT              0x0501          // 光源亮度值设定
#define CMD_CTRL_LIGHT_SRC_VERSION      0x0503          // 光源板版本查询
/*-----------------------------------------------------------------------*/

/*-------------RSC机型通过量测试物料点信息统计和上传命令编码-------------------------*/
#define CMD_UNIT_MATERIAL_STATISTIC_CTRL          0x4200    // 物料点信息统计控制
#define CMD_UNIT_MATERIAL_STATISTIC_UPLOAD        0x4201    // 物料点信息统计上传

#define CMD_UNIT_MATERIAL_STATISTIC_CTRL_ALL          0x4202    // 物料点信息统计控制(总)
#define CMD_UNIT_MATERIAL_STATISTIC_UPLOAD_ALL        0x4203    // 物料点信息统计上传(总)

/*-------------RSC机型精准剔除参数下发命令编码-------------------------*/
#define CMD_UNIT_TICK_MODE                        0x4210    // 精准剔除参数下发

/*-------------RSC机型前置板对位信息相关命令编码-------------------------*/
#define CMD_UNIT_POSITION_START                   0x4300    // 对位信息统计开始
#define CMD_UNIT_POSITION_ACCEPT                  0x4301    // 对位信息获取
#define CMD_UNIT_REVERSE                          0x4302    // 三次反选参数

/*----------------------- 整机性能参数统计命令编码 -------------------------*/
/* 前置板 */
#define CMD_UNIT_STATISTIC_HISTOGRAM_CAPTURE      0x8B      // 信号直方图统计开始、停止命令
#define CMD_UNIT_STATISTIC_BACKGROUND_CAPTURE     0x8C      // 清灰前后背景信息统计命令
#define CMD_UNIT_STATISTIC_HISTOGRAM_OBTAIN       0x8D	    // 信号直方图信息获取命令
#define CMD_UNIT_CAPTURE_RANGE                    0x8E      // 图像拍摄触发象元范围
#define CMD_UNIT_STATISTIC_MATERIAL_OBTAIN        0x2004    // 物料点像素统计信息获取
#define CMD_UNIT_STATISTIC_BACKGROUND_OBTAIN      0x2005    // 清灰前后背景信息获取
#define CMD_UNIT_STATISTIC_MATERIAL_CAPTURE       0x2101    // 物料点信息统计
#define CMD_INT_STATISTIC_EJTIMES_CAPTURE         0x2102    // 吹气次数统计
/*-----------------------------------------------------------------------*/

/* 定义单个接口板外接前置板的最大数量 */
const int MAX_UNIT_PER_INT = 24;

/*!
 * \brief 串口操作类
 */
class MyQextSerialPort
{
public:
    MyQextSerialPort();

    int comOpen(int nPortno);
    int comClose(int nPortno);

    void setComBaud(int nPortno, int baud);     // 设置串口波特率
    int getComBaud(int nPortno);     // 获取串口波特率

    int com1Write(int nCmd, char sModuleType, char sIntAddr, char sUnitAddr, char arg1, char arg2,
                        char arg3, char arg4, char arg5, int nCount);
    int com1Read(char *sBuf, int nLen, int timeoutMsec = 0);
    int com1Write_update(int nCmd, char sModule, char sModule2, char *sBuf, int nCount);

    int fpgaUpdateWrite(int nCmd, uchar sModule1, uchar sModule2, uchar arg1, uchar arg2,
                        uchar arg3, uchar arg4, uchar arg5, int nCount, uchar buf[][FRAME_LEN]=NULL);
    int fpgaUpdateRead(char *sBuf, int len, bool bIsCtrl=false);

    // sModuleType: 0-通用控制板， 1-恒流源控制板
    int com2Write(int nCmd, char sModuleType, char sModuleAddr, char arg1, char arg2,
                  char arg3, char arg4, char arg5, int nCount);
    int com2Read(char *sBuf, int nLen);

    int com3Write(int nCmd, char sModuleType, char sIntAddr, char sUnitAddr, char arg1, char arg2,
                        char arg3, char arg4, char arg5, int nCount);
    int com3Read(char *sbuf, int nLen, int timeoutMsec = 0);

    int remoteUpdateRead(char *sbuf, int nLen);
    unsigned int crcFpga(char *sData, int nLen);    // CRC check

    int smsComWrite(int nCmd, QString str);         // 发送短信息
    QString smsComRead(void);       // 接收短信息

private:
    QextSerialPort *myPort1, *myPort2, *myPort3;
};

#endif // MYQEXTSERIALPORT_H
