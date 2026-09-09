/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        globalparams.h
 * \brief       全局参数头文件
 * \date        2015.01.14
 */
#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include "myqextserialport.h"
#include "aicommunicate.h"
#include "mylanguage.h"
#include "mydelaycode.h"
#include "3rdparty/qjson/qjson.h"
#include "mydevmonitor.h"

class MyQextSerialPort;
class AiCommunicate;
class MyGlobalString;


/* x86_64 本地调试用 debug_opt；aarch64 板卡保持 /home/mcgs */
#if defined(Q_PROCESSOR_X86_64)
#define APP_PATH "/home/mcgs/Desktop/DuySorter_rk3566_simple/debug_opt"
#else
#define APP_PATH "/opt/app"
#endif

#define MAX_UART	3
#define MAX_NAME	128

/* ----------------------------------------------------------*/
/* 平台配置相关 */
#define PLATFORM_6410 0
#define PLATFORM_335X 1

#if PLATFORM == PLATFORM_6410
#define PLATFORM_NAME "6410"
#elif PLATFORM == PLATFORM_335X
#define PLATFORM_NAME "335X"
#else
#define PLATFORM_NAME ""
#endif

/* ----------------------------------------------------------*/
/* 语言配置相关 */

#define MAX_LANG    64
/*!
 * \brief 语言列表
 */
enum
{
    LANG_CHS = 1,   // 简体中文
    LANG_ENG,       // 英文
    LANG_RUS,       // 俄文
    LANG_VIET,      // 越南文
    LANG_THAILAND,  // 泰文
    LANG_SPAN,      // 西班牙文
    LANG_CHT,       // 繁体中文
    LANG_TURKEY,    // 土耳其文
    LANG_FARSIE,     // 波斯文
    LANG_FRENCHE,    // 法文
    LANG_UYGHUR,    // 维吾尔文
    LANG_KOREANE,    // 韩文
    LANG_ARABICE,    // 阿拉伯文
    LANG_BULGARIANE, // 保加利亚文
    LANG_SLOVAKE,    // 斯洛伐克语
    LANG_PORTUGUESEE,// 葡萄牙语
    LANG_MYANMAR,   // 缅甸语
    LANG_BENGALIE,   // 孟加拉语
    LANG_INDONESIA, // 印度尼西亚语
    LANG_POLAND     // 波兰语
};

/* ----------------------------------------------------------*/
/* 机型列表配置相关 */

#define MAX_GENERAL_MACHINE_NUM 8       // 杂粮类机型数
/*!
 * \brief 机型列表
 */
enum
{
    MACHINE_CF = 1,    // 杂粮CF系列    (通道式，单层双视) — CG 机型硬件基础
    MACHINE_CG = 15     // 杂粮CG系列    (通道式，单层双视）
};

/*!
 * \brief 定制机型列表（仅保留通用机型）
 */
enum
{
    OPT_PRODUCT_ANCOO_GENERAL = 1     // 品牌：安科，通用机型
};

/*!
  *\brief 屏运行模式设定列表
  */
enum
{
    MODE_RUN_STANDALONE,    // 独立运行模式
    MODE_RUN_MASTER,        // 主屏运行模式
    MODE_RUN_SLAVE          // 从屏运行模式
};

/* ---------------------------------------------------- */
/* 机型参数配置相关 */
#define MAX_LEVEL               3       // 最大层数
#define MAX_VIEW_LOCATION       4       // 最大相机板视角数，即相机板行数
#define MAX_INT                 3       // 最大接口板数量
#define MAX_UNIT                64      // 最大彩色相机板数量
#define MAX_CTRL                3       // 最大控制板数量
#define MAX_GROUP_IDTNTIFY      16      // 最大识别组数量
#define MAX_GROUP_TICK          4       // 最大剔除组数量
#define MAX_EJECTOR             120     // 单通道对应的最大喷嘴数量
#define MAX_EJECTOR_SIXTY       60      // 单通道对应的最大喷嘴数量
#define MAX_GROUP_SENS          3       // 最大精度组数
#define MAX_LIGHT_SOURCE        8       // 最多光源板数量
#define UNIT_NULL               -1      // 空

#define UP_LEVEL                20      // 上层
#define MIDDLE_LEVEL            21      // 中层
#define DOWN_LEVEL              22      // 下层

#define ONE_LEVEL               0       // 通道式色选机层序号，如CF、LD2、RS、RC-C等机型的层序号都为0
#define MAX_PARA                10      // 最大参数个数

/*!
 * \brief 识别组名称索引
 */
enum
{
    FIRST_FRONT,        // 一次前视
    FIRST_REAR,         // 一次后视
    SECOND_FRONT,       // 二次前视
    SECOND_REAR,        // 二次后视
    THIRD_FRONT,        // 三次前视
    THIRD_REAR,         // 三次后视
    FOURTH_FRONT,       // 四次前视
    FOURTH_REAR,        // 四次后视

    FIRST_UPPER,        // 一次上视
    FIRST_DOWN,         // 一次下视
    SECOND_UPPER,       // 二次上视
    SECOND_DOWN,        // 二次下视

    FIRST_DOWN_FRONT_ML = 20,// 一次下前，多层(Multi-Layer)，如LDS2
    FIRST_DOWN_REAR_ML,     // 一次下后
    FIRST_UP_FRONT_ML,      // 一次上前
    FIRST_UP_REAR_ML,       // 一次上后
    SECOND_DOWN_FRONT_ML,   // 二次下前
    SECOND_DOWN_REAR_ML,    // 二次下后
    SECOND_UP_FRONT_ML,     // 二次上前
    SECOND_UP_REAR_ML       // 二次上后
};

enum
{
    UP_UNIT = 10,	// 上视 (未使用，废弃)
    DOWN_UNIT = 11,	// 下视 (未使用，废弃)
    FRONT_UNIT = 30,	// 前视
    REAR_UNIT = 31	// 后视
};

/*!
 * \brief 玉米双视 T3/LD4 识别组索引（原来在 maizeflow.h，数据结构保留）
 */
enum
{
    FIRST_DOWN_FRONT = 32,  // 一次前下
    FIRST_UP_FRONT,         // 一次前上
    FIRST_DOWN_REAR,        // 一次后下
    FIRST_UP_REAR,          // 一次后上
    SECOND_DOWN_FRONT,      // 二次前下
    SECOND_UP_FRONT,        // 二次前上
    SECOND_DOWN_REAR,       // 二次后下
    SECOND_UP_REAR,         // 二次后上
    FIRST_UP_MASTER,        // 一次上多 (LD4)
    FIRST_DOWN_MASTER,      // 一次下多
    SECOND_UP_MASTER,       // 二次上多
    SECOND_DOWN_MASTER,     // 二次下多
    FIRST_UP_SLAVE,         // 一次上少
    FIRST_DOWN_SLAVE,       // 一次下少
    SECOND_UP_SLAVE,        // 二次上少
    SECOND_DOWN_SLAVE       // 二次下少
};


#define MAX_BACKGROUND_GROUP    4       // 最大支持背景设置组数(单层)
/*!
 *\brief 背景组索引号
 */
enum
{
    FRONT_DOWN_BKG = 1,	// 前下视(未使用，废弃)
    REAR_DOWN_BKG,		// 后下视(未使用，废弃)
    FRONT_UP_BKG,		// 前上视(未使用，废弃)
    REAR_UP_BKG         // 后上视(未使用，废弃)
};

/*!
 * \brief 剔除组索引号
 */
enum
{
    FIRST_UNIT,         // 一次
    SECOND_UNIT,        // 二次
    THIRD_UNIT,         // 三次
    FOURTH_UNIT,        // 四次

    FIRST_UP_UNIT,      // 上一次
    SECOND_UP_UNIT,     // 上二次
    THIRD_UP_UNIT,      // 上三次
    FOURTH_UP_UNIT,     // 上四次

    FIRST_DOWN_UNIT,    // 下一次
    SECOND_DOWN_UNIT,   // 下二次
    THIRD_DOWN_UNIT,    // 下三次
    FOURTH_DOWN_UNIT    // 下四次
};

/* 灵敏度设置相关 */
#define MAX_LIST		14
#define MAX_GENERAL		4
#define MAX_AI			4 
#define MAX_SHAPE		5
#define MAX_PEANUT		4
#define MAX_WATERMELON		9
#define MAX_TEA			7
#define MAX_MAIZE		1
#define MAX_RSV			1
#define MAX_MAIZE_EDIT		9

/*!
 *\brief 机型视角类型列表
 */
enum
{
    SINGLE_VIEW = 1,		// 单视
    DOUBLE_VIEW = 2,		// 双视
    TRIPLE_VIEW = 3,		// 三视
    QUADRUPLE_VIEW = 4		// 四视
};

#define CONTROL_BOARD1          0       // 控制板1
#define CONTROL_BOARD2          1       // 控制板2

#define INTERFACE_BOARD1        0       // 接口板1
#define INTERFACE_BOARD2        1       // 接口板2

/* 灯控设置相关 */
#define MAX_LAMP                48      // 灯最大数量
#define MAX_SRC_BOARD           8       // 最大恒流源板数量
#define MAX_LAMP_PER_BOARD      6       // 单个恒流源板最多灯控数

#define MAX_FEED		12	// 供料器数量

/* ---------------------------------------------------- */
/* 方案算法相关 */
/*!
 * \brief 方案参数文件状态索引
 */
enum
{
    ENGINEER_FILE_STATUS,	// 工程参数文件
    GLOBAL_FILE_STATUS,		// 全局参数文件
    CAMERA_FILE_STATUS,		// 相机参数文件
    PROFILE_FILE_STATUS     // 方案参数文件
};

#define MAX_PROFILE             500     // 最多方案数量
#define PROFILE_PRE_FIXED       30      // 预制方案数量
#define ARITHMETIC_TOTAL        30      // 最多算法总数
#define MAX_MAIZE_SENSNUM_TOTAL 10      // 玉米专用
#define MAT_MAX                 4       // 智能模式中，最大物料数量
#define IMAGE_MAX               18      // 智能模式中，最多的物料图片数

#define IMG_LEN                 1024*3  // 智能模式中，保存图片的长度
#define CAP_WID                 1024    // 图片宽度(像素点)
#define CAP_HEI                 500     // 图片高度(像素点)

#define MAX_PARENT_PREFIXED_PROFILE_COUNT        32   // 预置方案父类数量
#define MAX_SON_PREFIXED_PROFILE_COUNT           32   // 预置方案子类数量

/*!
 * \brief 物料算法列表
 */
enum
{
    ARITH_GREY_A,           //0 灰度杂质 A
    ARITH_GREY_B,           //1 灰度杂质 B
    ARITH_DISCOLOR_A,       //2 色差杂质 A
    ARITH_DISCOLOR_B,       //3 色差杂质 B
    ARITH_CROSS,            //4 差分算法
    ARITH_SHAPE,            //5 形状算法：选圆/选长
    ARITH_SHAPE_LENGTH,     //6 形状算法：选长/选短
    ARITH_SCALE,            //7 大小算法A
    ARITH_BUD_1,            //8 花生选芽 1
    ARITH_BUD_2,            //9 花生选芽 2
    ARITH_PEANUT_A,         //10 花生机专用：A类杂质
    ARITH_PEANUT_B,         //11 花生机专用：B类杂质
    ARITH_PEANUT_C,         //12 花生机专用：C类杂质
    ARITH_PEANUT_D,         //13 花生机专用：D类杂质
    ARITH_INTEL_A,          //14 智能模式 A
    ARITH_INTEL_B,          //16 智能模式 B
    ARITH_INTEL_C,          //17 智能模式 C
    ARITH_INTEL_D,          //18 智能模式 D
    ARITH_WATERMELON,       //19 西瓜子算法
    ARITH_RICE,             //20 大米算法
    ARITH_MAIZE,            //21 玉米算法
    ARITH_SHAPE_POLE,       //22 选杆算法（保留待用）
    ARITH_SHAPE_POLE_A,     //23 茶叶选细杆算法
    ARITH_SHAPE_POLE_B,     //24 茶叶选粗杆算法
    ARITH_SHAPE_SLICE,      //25 茶叶选片算法
    ARITH_SHAPE_LEAF,       //26 茶叶选芽算法
    ARITH_SCALE_B,          //27 茶叶选芽算法
    ARITH_RESERVED,         //28 保留算法，独立与其他算法
    ARITH_CANDY,            //29 糖果算法
    ARITH_PISTACHIO         //29 开心果算法
};

/*!
 * \brief 参数设置模式
 */
enum
{
    PARAMS_ALL_SEPARATE = 1,    // 全部独立
    PARAMS_ALL_SAME,            // 全部相同
    PARAMS_FRONT_REAR_SAME,     // 前后视相同
    PARAMS_FIRST_SECOND_SAME    // 一二（三）次相同
};

/*!
 * \brief 操作权限设定
 */
enum
{
    AUTHENTICATION_LEVEL_OPERATOR,	    // 操作人员模式
    AUTHENTICATION_LEVEL_ENGINEER,	    // 工程人员模式
    AUTHENTICATION_LEVEL_FACTORY	    // 厂家人员模式
};

#define MAX_COLOR_MODE     4           //色选模式最大数量

/* ----------------------------------------------------------*/
/* 机器报警配置相关 */

/*!
 * \brief 报警列表
 */
enum
{
    ALARM_ID_PRESSURE,      // 气压报警
    ALARM_ID_BLOWER,        // 风机报警
    ALARM_ID_LAMP,          // 灯报警
    ALARM_ID_BELT,          // 皮带报警
    ALARM_ID_LEVEL,         // 料位报警
    ALARM_ID_COMM,          // 通信报警-黑白米机专用
    ALARM_ID_TEMPERATURE,   // 温度报警
    ALARM_ID_SLAVE_COMM     // 从设备通信报警-双层LD4专用
};

#define MAX_ALARM       	16      // 最大报警数量
#define MAX_TEMPERATURE_SENSOR	12	// 最大温度传感器数量

/* ----------------------------------------------------------*/
/* 传感器配置相关 */

/*!
 * \brief 传感器类型
 */
enum
{
    SENSOR_K = 1,       // K系传感器
    SENSOR_T_2566,      // 2566传感器
    SENSOR_T_2564,      // 2564传感器
    SENSOR_D2,          // D2传感器
    SENSOR_P5           // P5传感器
};

/* 传感器分辨率 */
#define SENSOR_MAX_PIXELS    2592       // 分辨率上限
#define SENSOR_PIXELS_1024   1024       // 1024传感器
#define SENSOR_PIXELS_2048   2048       // 2048传感器
#define SENSOR_PIXELS_2592   2592       // 2048传感器

/* 传感器帧频系数 */
#define TD_SENS_COEFICENT   675         // TD系帧频计算校正系数
#define CF_SENS_COEFICENT   720         // CF系帧频计算校正系数

/* ----------------------------------------------------------*/
/* 日志配置相关 */
#define MAX_LEN_LOGFILE_SIZE	(10*1024*1024) 
#define MAX_LEN_LOG_VIEW	(20*1024*1024)

/*!
 * \enum 枚举RS机型色选模式
 */
enum
{
    RS_DARK_SORT = 0,                   // 深色专选
    RS_DIFF_POSITIVE_SORT,              // 异色粒正选
    RS_DIFF_NEGATIVE_SORT,              // 异色粒反选
    RS_WHITE_POSITIVE_SORT,             // 乳白色正选
    RS_WHITE_NEGATIVE_SORT,             // 乳白色反选
    RS_YELLOW_WHITE_SORT,               // 黄白同选
    RS_YELLOW_TRANS_SORT,               // 黄透同选
    RS_YELLOW_YELLOW_NEGATIVE_SORT,     // 黄+黄反选
    RS_WHITE_YELLOW_NEGATIVE_SORT,      // 黄+白反选
    RS_USER_DEFINED_SORT                // 自定义模式
};

/* 定义RS机型模式种类 */
const int RS_SORT_MODE_TYPE = 10;

/*!
 * \enum 枚举RSC机型色选模式
 */
enum
{
    RSC_DIFF_POSITIVE_SORT = 0,         // 异色粒正选
    RSC_DIFF_NEGATIVE_SORT,             // 异色粒反选
    RSC_WHITE_POSITIVE_SORT,            // 乳白色正选
    RSC_WHITE_NEGATIVE_SORT,            // 乳白色反选
    RSC_YELLOW_WHITE_SORT,              // 黄白同选
    RSC_YELLOW_TRANS_SORT,              // 黄透同选
    RSC_GENERAL                         // 通用
};

/* 定义RSC机型模式种类 */
const int RSC_SORT_MODE_TYPE = 7;

/* 定义料位监控报警类别 */
enum
{
    ALARM_LEVEL_NULL = 0,       // 料位无报警
    ALARM_LEVEL_EMPTY,          // 料位空报警
    ALARM_LEVEL_FULL,           // 料位满报警
    ALARM_LEVEL_SENSOR          // 料位传感器报警
};

/*!
 * \brief 识别组索引号
 */
enum
{
    SM_FIRST_FRONT = 0,         // 一次前视
    SM_FIRST_REAR,              // 一次后视
    SM_SECOND_FRONT,            // 二次前视
    SM_SECOND_REAR,             // 二次后视
    SM_THIRD_FRONT,             // 三次前视
    SM_THIRD_REAR               // 三次后视
};

/*!
 * \brief 枚举识别结果算法类型
 */
enum
{
    IDE_RESULT_GREY_A = 0,      //! 灰度A识别结果
    IDE_RESULT_GREY_B,          //! 灰度B识别结果
    IDE_RESULT_DISCOLOR_A,      //! 色差A识别结果
    IDE_RESULT_DISCOLOR_B,      //! 色差B识别结果
    IDE_RESULT_INTEL_A,         //! 智能A识别结果
    IDE_RESULT_INTEL_B,         //! 智能A识别结果
    IDE_RESULT_TYPE             //! 识别结果算法类型
};

/*!
 * \brief 枚举灯控界面前后视灯的类型
 */
enum
{
    FRONT_LAMP_1 = 0,
    FRONT_LAMP_2,
    FRONT_LAMP_3_BACKGROUD,     //! 后视背景板对应灯
    FRONT_LAMP_4,
    FRONT_LAMP_5,
    FRONT_LAMP_6,
    REAR_LAMP_1,
    REAR_LAMP_2,
    REAR_LAMP_3_BACKGROUD,      //! 前视背景板对应灯
    REAR_LAMP_4,
    REAR_LAMP_5,
    REAR_LAMP_6
};

/*!
 * \brief 参数文件保存状态
 */
enum
{
    ALL_PARAMS_FILE_OK = 0,     //! 默认所有文件都保存正确
    ENGINEER_FILE_ERROR = 1,    //! 工程参数文件保存错误
    GLOBAL_FILE_ERROR = 2,      //! 全局参数文件保存错误
    CAMER_FILE_ERROR = 3,       //! 相机参数文件保存错误
    SCHEME_FILE_ERROR = 4       //! 方案参数文件保存错误
};

/*!
 * \brief 背景颜色
 */
enum
{
    COLOR_OPT_BLACK = 0,    /*!< 黑背景 */
    COLOR_OPT_R = 1,    /*!< 红背景 */
    COLOR_OPT_G = 2,    /*!< 绿背景 */
    COLOR_OPT_B = 4,    /*!< 蓝背景 */
    COLOR_OPT_WHITE = 7,    /*!< 白背景 */
    COLOR_OPT_AUTO = 3     /*!< 彩色米机自动背景,借黄背景参数 */
};

/*************************************************************************************
 *
 *      以下是需要保存到文件中各个全局参数的结构体定义
 *
 ************************************************************************************/
 /* ---------------------------------------------------- */
 /* 参数保存文件名称 */
#define CNF_ENGINEER            APP_PATH "/userdata/cnf/cnf.engineer"         // 工程模式参数文件
#define CNF_ENGINEER_B          APP_PATH "/userdata/cnf/cnf.engineer_2"       // 工程模式参数文件备份
#define CNF_GLOBAL              APP_PATH "/userdata/cnf/cnf.global"           // 全局参数文件
#define CNF_GLOBAL_B            APP_PATH "/userdata/cnf/cnf.global_2"         // 全局参数文件备份
#define CNF_CAMERA              APP_PATH "/userdata/cnf/cnf.camera"           // 全局相机参数文件
#define CNF_CAMERA_B            APP_PATH "/userdata/cnf/cnf.camera_2"         // 全局相机参数文件备份
#define CNF_CUR_TIME            APP_PATH "/userdata/cnf/cnf.cur_time"         // 延迟付款时，系统时间存放文件

#define CNF_PARAMS_FILE_STATUS      APP_PATH "/userdata/cnf/cnf.params_status"           // 参数状态文件
#define CNF_PARAMS_FILE_STATUS_B    APP_PATH "/userdata/cnf/cnf.params_status_2"         // 参数文件状态备份

#define CNF_MY                  APP_PATH "/DuySorter.conf"                    // 全局配置文件
#define CNF_LOG_CFG             APP_PATH "/log.properties"                    // 日志配置文件

#ifdef Q_OS_WIN32
#define LANG_DIR                "../../DuySorter_rk3566/app/i18n/"   // 语言文件目录 *.qm
#else
#define LANG_DIR                APP_PATH "/i18n/"
#endif
#define LANG_DIR_SD             APP_PATH "/userdata/i18n/"

#define LOG_FILE_NAME           APP_PATH "/userdata/log/mylog"
#define LOG_BACK_FILE_NAME      APP_PATH "/userdata/log/mylog.1"
#define CNF_SLAVE_LIST          APP_PATH "/userdata/cnf/cnf.slave_list"

#define SYS_DB_INFO             APP_PATH "/userdata/db/sysdb1"

/* **********************************************************************************
 *
 *                          物料算法参数相关结构体
 *
 * *********************************************************************************/
 /*!
  * \brief 保留参数
  */
typedef struct stru_adv
{
    int nDiscolor1;         // 色差颜色 0-: r/(r+g), 1-: r/(r+b), 2-: g/(g+b)
    int nSensMin1;          // 色差灵敏度下限
    int nSensMax1;          // 色差灵敏度上现
    int nMode1;             // 色差模式1: -- 选亮, 0: -- 选暗
    int nColor2;            // 灰度颜色
    int nSensMin2;          // 灰度灵敏度下限
    int nSensMax2;          // 灰度灵敏度上限
    int nMode2;             // 灰度模式1: -- 选亮, 0: -- 选暗
    int nPercent;           // 保留比例
}stu_reserved;

/*!
 * \brief 灰度杂质A、B，色差杂质A、B
 */
typedef struct stru_discolor
{
    char sName[MAX_NAME];         // 杂质名称
    int nRow;                // 行数, 面积 = 行数*列数
    int nColumn;             // 列数, 列数 = 行数*2
    int nPercent;            // 纯度
    int nColor;              // 灰度颜色,  1-: red, 2-: green, 4-: blue
    int nDiscolor;           // 色差颜色 0-: r/(r+g), 1-: r/(r+b), 2-: g/(g+b)
    int nSensMin;            // 灵敏度下限
    int nSensMax;            // 灵敏度上限
    int nMode;               // 亮暗模式：1: -- 选亮, 0: --选暗
    int nLightLimit;         // 亮度下限
    stu_reserved struReserved;  // 保留参数
    int nBurntReservedFlag;      // 焦糊保留设置的标志
}stu_grey_color;

/*!
 * \brief 智能算法
 */
typedef struct stru_intelligent
{
    char sName[MAX_NAME];        // 杂质名称
    int nRow;               // 行数,大蒜的大尺寸
    int nColumn;            // 列数,大蒜的大尺寸
    int nPercent;           // 纯度,大蒜的大尺寸

    int nRowRes;            // 行数,大蒜的小尺寸
    int nColumnRes;         // 列数,大蒜的小尺寸
    int nPercentRes;        // 纯度,大蒜的小尺寸

    int nEnable;            // 智能算法固化模式使能状态
    int nSens;              // 微调系数
    int nSensRatio;         // 比例算法的灵敏度
    int nIntelType;         // 当前智能的算法类别，0：普通，1：比例智能
    int nRatioMode;			// 比例算法模式
    int nWholeEdge;         // 仅西瓜子算法时有用， 1：边缘， 0： 整体
    int nCons[2];           // 常数
    int nQuo1[2];           // 系数1
    int nQuo2[2];           // 系数2
    int nQuo3[2];           // 系数3
    stu_reserved struReserved;  // 保留参数

    int nModeDfl;           // 0： 正选， 1： 反选， -1: 禁用
    int nConsDfl[2];        // 常数项， cons[0]-0： 正， 1：负
    int nQuoR[2];           //r
    int nQuoG[2];           //g
    int nQuoB[2];           //b
    int nQuoRG[2];          //rg
    int nQuoRB[2];          //rb
    int nQuoGB[2];          //gb
    int nQuoRR[2];          //rr
    int nQuoGG[2];          //gg
    int nQuoBB[2];          //bb
}stu_it;

/*!
 * \brief 智能物料设置
 */
typedef struct stru_intelligent_material
{
    int use;                // 物料使能： 1 使能, 0: 不使能
    int good_bad;           // 物料好坏： 0: 坏, 1: 好 2: 不用
    char name[MAX_NAME];         // 物料名称
}stu_it_mat;

/*!
 * \brief 花生选芽参数
 */
typedef struct stru_bud
{
    char sName[MAX_NAME];   // 杂质名称
    int nRow;               // 行数
    int nColumn;            // 列数
    int nPercent;           // 纯度
    int nPercentLimit;      // 纯度, 对应界面参数下限
    int nArea;              // 面积，对应界面灵敏度
}stu_bd;

/*!
 * \brief 差分算法
 */
typedef struct stru_cross
{
    char sName[MAX_NAME];        // 杂质名称
    int nRow;               // 行数
    int nColumn;            // 列数
    int nPercent;           // 纯度
    int nSens;              // 灵敏度
    int nColor;             // 色差
}stu_cs;

/*!
 * \brief 形状算法
 */
typedef struct stru_shape
{
    char sName[MAX_NAME];        // 杂质名称
    int nAreaMin;           // 灵敏度下限
    int nAreaMax;           // 灵敏度上限
    int nMode;              // 选圆/选长模式切换， 0： 选圆，  1： 选长
    int nAreaLimit;         // 参数下限(圆长算法)
    char sLengthName[MAX_NAME];  // 长短杂质名称
    int nLengthMin;         // 长短灵敏度下阈
    int nLengthMax;         // 长短灵敏度上阈
    int nLengthMode;        // 选长/选短模式切换， 0： 选短， 1： 选长
    int nLengthLimit;       // 参数下限(长短算法)
}stu_sp;

/*!
 * \brief 大小算法
 */
typedef struct stru_scale
{
    char sName[MAX_NAME];        // 杂质名称
    int nMode;              // 大小模式切换, 0: 选小, 1: 选大
    int nValue;             // 灵敏度
    int nValueLimit;        // 参数下限
}stu_sc;


/*!
 * \brief 茶叶选杆算法
 */
typedef struct stru_pole
{
    char sName[MAX_NAME];   // 细杆杂质名称
    int nArea;              // 细杆形选保留面积
    int nPercent;           // 细杆形选病斑
    int nWidthMin;          // 细杆形选宽度下限
    int nWidthMax;          // 细杆形选宽度上限
    int nMode;              // 正反选切换: 0-选杆，1-选芽

    stu_reserved struReserved;  // 保留参数（暂时废弃不用）

    char sPoleName[MAX_NAME];   // 粗杆杂质名称
    int nPoleArea;              // 粗杆形选保留面积
    int nPolePercent;           // 粗杆形选病斑
    int nPoleWidthMin;          // 粗杆形选宽度下限
    int nPoleWidthMax;          // 粗杆形选宽度上限
}stu_pole;

/*!
 * \brief 茶叶选片算法
 */
typedef struct stru_slice
{
    char sName[MAX_NAME];   // 杂质名称
    int nArea;              // 叶片面积
}stu_slice;

/*!
 * \brief 茶叶选芽算法
 */
typedef struct stru_leaf
{
    char sName[MAX_NAME];	// 杂质名称
    int nArea;			// 保留面积
    int nPercent;		// 病斑
    int nWidthMax;		// 宽度上限
    int nWidthMin;		// 宽度下限	
    int nThreshold;		// 阈值
}stu_leaf;

/*!
 * \brief 保留算法
 */
typedef struct stru_general_reserved
{
    char sName[MAX_NAME];	// 杂质名称
    int nThreshold_s;		// s阈值
    int nRow;               // 圈数
    int nThreshold_r;		// r阈值
}stu_general_reserved;

/*!
 * \brief 玉米算法参数（纯数据结构，算法实现已移除）
 */
typedef struct stru_maize
{
    char sName[MAX_NAME];        // 杂质名称
    int nRow;                    // 行数
    int nColumn;                 // 列数
    int nPercentTotal;           // 总纯度
    int nPercentGood;            // 好粒纯度
    int nPercentBad1;            // 坏粒1纯度
    int nPercentBad2;            // 坏粒2纯度
    int nColor1;                 // 颜色1
    int nMode1;                  // 模式1
    int nSensMin1;               // 灵敏度1下限
    int nSensMax1;               // 灵敏度1上限
    int nColor2;                 // 颜色2
    int nMode2;                  // 模式2
    int nSensProport;            // 灵敏度比例
    int nSensMin2;               // 灵敏度2下限
    int nSensMax2;               // 灵敏度2上限
    int nThreshold;              // 阈值
}stu_maize;


/*!
 * \brief  花生机型A、B、C、D类杂质
 */
typedef struct stru_peanut_abcd
{
    char sName[MAX_NAME];        // 杂质名称
    int nRow;               // 行数, 面积 = 行数*列数
    int nColumn;            // 列数, 列数 = 行数*2
    int nPercent;           // 纯度
    int nSensMin;           // 灵敏度下限
    int nSensMax;           // 灵敏度上下
    int nMode;              // 亮暗模式：1: -- 选亮, 0: --选暗
    int nSensReserved;      // 保留参数
    stu_reserved struReserved;  // 保留参数
}stu_pn_abcd;

/*!
 * \brief 西瓜子算法
 */
typedef struct stru_watermelon
{
    int nEdgeRedSens;           // 边缘发红灵敏度
    int nEdgeRedRatio;          // 发红区域比例
    int nEdgeDamagedSens;       // 边缘破损灵敏度
    int nEdgeDamagedRatio;      // 边缘破损比例
    int nRugged1Sens;           // 麻粒1 灵敏度
    int nRugged1Save;           // 保留偏白灵敏度
    int nRugged2Sens;           // 麻粒2 灵敏度
    int nRugged2Save;           // 保留偏黑灵敏度
    int nWhiteSens;             // 白区灵敏度
    int nWhitePropMin;          // 白区比例下限
    int nWhitePropMax;          // 白区比例上限
    int nWhiteMode;             // 选亮/选暗
}stu_melon;

/*!
 * \brief RS机型算法参数
 */
typedef struct stru_rice
{
    int nSens;                  // 主配灵敏度
    int nDarkSens;              // 主配灵敏度(暗),仅自定义模式下使用
    int nBadDiscolor;           // 主配病点色差
    int nRow;                   // 主配浅黄面积的行数
    int nCol;                   // 主配浅黄面积的列数
    int nYellowArea;            // 主配浅黄面积
    int nBadWidth;              // 主配病点宽度

    int nReservePercent1;       // 保留比例1
    int nReservePercent2;       // 保留比例2
}stu_rice;

/*!
 * \brief 糖果机型糖果算法
 */
typedef struct stru_candy
{
    int nRation1;               // 比例1
    int nRation2;               // 比例2
    int nRow;                   // 行数
    int nCycles;                // 质心扩展圈数

}stu_candy;

/*!
 * \brief 开心果机型开心果算法
 */
typedef struct stru_pistachio
{
    int nAreaMin;              // 面积下限
    int nGrayThreshold_1;      // 灰度阈值1
    int nGrayThreshold_2;      // 灰度阈值2
    int nGrayThreshold_3;      // 灰度阈值3
}stu_pistachio;

typedef struct stru_ai
{
    char modelId[MAX_NAME];            // model id
    //    stu_aipara struAiPara[MAX_PARA]; // model para
}stu_ai;


typedef struct stru_aipara
{
    int id;                       // model para id
    char materialName[MAX_NAME];  // model para name
    int threshold;                // model para threshold
}stu_aipara;

/* **********************************************************************************
 *                          参数文件保存状态
 *
 * value = 0	所有文件都保存正确
 * value = 1	工程模式文件保存错误
 * value = 2	全局参数文件保存错误
 * value = 3	方案参数文件保存错误
 *
 * *********************************************************************************/
struct struCnfFileStatus
{
    int value;
};


/* **********************************************************************************
 *
 *                          工程模式参数相关结构体
 *
 * *********************************************************************************/

 /*!
  * \brief 子预制方案文件信息
  */
struct struSonPrefixedPrifileInfo
{
    int nIsAble;			 // 子预制方案使能状态
    char sMaterialName[MAX_NAME];        // 子预制方案物料名称
    char sProfileName[MAX_NAME];         // 子预制方案文件名称
};

/*!
 * \brief 父预制方案文件信息
 */
struct struParentPrefixedProfileInfo
{
    int nIsAble;                   // 父预制方案使能状态
    int nSonProfileCount;          // 当前父预制方案中子预制方案数量
    char sMaterialName[MAX_NAME];       // 父预制方案物料名称
    struct struSonPrefixedPrifileInfo  struSonProfile[MAX_SON_PREFIXED_PROFILE_COUNT];      // 子预制方案信息
};

/*!
 * \brief 工程配置参数
 */
struct struCnfEngineer
{
    int nProduct;                               // 机型类型通用、花生定制
    int nMachine;                               // 机型：CF系、单层履带机、双层履带机
    int nArithmeticTotal;                       // 当前机型物料算法总数
    int nArithmeticSeq[ARITHMETIC_TOTAL];       // 物料算法使能状态
    int nParentPrefixedProfileTotal;            // 根预制方案数量
    struParentPrefixedProfileInfo struParentPrefixedProfile[MAX_PARENT_PREFIXED_PROFILE_COUNT];   // 根预制方案信息
    int nLangList[MAX_LANG];                    // 语言列表，[0] - 当前设置语言总数, [other] - 语言编号
    char sTitle[64];                            // 定制主界面标题
    int nEnableBeanMachine;                     // 是否豆类定制机型， 1：是， 0：否
    int nEnableMaizeDoubleView;                 // 是否使能玉米双视， 1：是， 0：否

    int nEnableCGMachine;                       // 是否是CG机型
    int nEnableHGQMachine;                      // 是否为黑枸杞定制

    /* nDerivedDevType变量表示一般机型的衍生机型。
    *  I机型系列为CF+的衍生机型，其最大配置为2排可见光、2排红外相机。
    *  各种衍生机型对应的代码设定由以下规则设定：
    *  nDerivedDevType的低8位abcdefgh,表示的可见光相机的8个背景角度，从低位向高位为：前视1、后视1、前视2、后视2....
    *  nDerivedDevType的次低8位abcdefgh,表示的是红外相机的8个背景角度，从低位向高位为：前视1、后视1、前视2、后视2....
    *  如当前的通用CF机型代码为0x0003(无红外，可见前后视)，I典型机型0x0103(红外前视，可见前后视)
    *  其他的机型组合参考上述定义。
    */
    int nDerivedDevType;                        // I机型的衍生机型，其在CF/CI机型上做特定改动

};

/* **********************************************************************************
 *
 *                          全局参数相关结构体
 *
 * *********************************************************************************/
 /*!
  * \brief 方案文件名称和方案命名索引
  */
struct struProfileIndex
{
    char sProfileName[MAX_NAME];     // 方案保存文件名称
    char sMaterialName[MAX_NAME];    // 界面显示方案名称
};

/*!
 * \brief 识别+剔除 组通道信息
 */
struct stru_group_unit_info
{
    int nLevelIndex;                // 组所在的层索引
    int nNameIndex;                 // 组名称索引
    int nUnitCount;                 // 组内相机板数量
    int nUnitId[MAX_UNIT];          // 组内相机编号
    int nUnitSplice[MAX_UNIT];      // 相机是否分割，0-:不分割, 1-:分割，前一半, 2-:分割，后一半
};

/*!
 * \brief 层内部组信息
 */
struct struMachineLevel
{
    int nUnitLevelTotal;                                                    // 层内相机单元数量
    int nUnitId[MAX_UNIT];                                                  // 层内相机编号
    int nViewTotal;                                                         // 2： 当前层双视， 1： 单视  TD机型 2：开启双视 1：没有双视，普通模式
    int nIdentifyGroupTotal;                                                // 识别组总数，机型设置相关
    int nTickGroupTotal;                                                    // 剔除组总数，机型设置相关
    struct stru_group_unit_info struIdentifyGroupInfo[MAX_GROUP_IDTNTIFY];  // 识别组通道信息，机型设置相关
    struct stru_group_unit_info struTickGroupInfo[MAX_GROUP_TICK];          // 剔除组通道信息，机型设置相关
};


/*!
 * \brief 全局配置参数
 * \note  保存于cnf.global文件
 */
struct struCnfGlobal
{
    int nLang;                                                  // 语言
    int nCounter;                                               // 系统累计运行时间
    int nProfile;                                               // 当前应用方案编号
    int nProfileTotal;                                          // 当前方案总数
    struct struProfileIndex struProfileIndex[MAX_PROFILE];      // 方案文件名和对应显示的方案命名
    int nLevelTotal;                                            // 当前层总数
    struct struMachineLevel struLevelInfo[MAX_LEVEL];           // 各个层内部信息
    int nInterfaceBoardTotal;                                   // 接口板总数
    int nCtrlBoardTotal;                                        // 控制板总数
    int nEjectorsPerChute;                                      // 单通道内喷嘴数量，机型设置相关
    int nEjectCountProtect;                                     // 喷阀保护次数上限
    int nLampTotal;                                             // 灯控数量
    char sDelayCode[DCCRYPT_MAX_STR];                           // 延迟付款码
    int nEnableBacklightPassword;                               // 使能点亮触摸屏密码， 1： 使能， 0， 不使能，默认为0
    char sBacklightPassword[16];                                // 点亮触摸屏密码
    int nFeederVoltage;                                         // 0: 高电压， 1： 低电压

    /* RS机型参数 */
    int nAssistCamEn[MAX_UNIT];     // 辅助相机使能
    int nAutoFeederSet;             // 一键自动供料量设置：0-当前供料量,1-固定供料量
    int nAutoReferChute;            // 一键自动参考滑槽
    int nAutoFlagDark;              // 一键自动深色专用标志：0-否,1-是
    int nTickDirect;                // 剔除的方向：0-123,1-213,2-321
    int nParamSetAlone;             // 参数设置：0-分次设置, 1-独立设置
    int nProfileMode[MAX_PROFILE];  // 方案模式
    int nFeederCtrlEn;              // 料位监控使能
    int nFeederCtrlMode;            // 料位监控模式：0-单一模式1, 1-复合模式, 2-单一模式2
    int nFeederRemoteCtrlMode;      // 振动器外围控制：0-反向, 1-正向
    int nFeederCtrlWaitTime;        // 料位检测等待时间

    /* 通用机型 */
    int uartBaud[MAX_UART];         // 串口1,2,3波特率， 1： 115200， 2： 921600

    /* AFLATTION机型 */
    int nPMTRun;
    int nEnablePMTR1;
    int nEnablePMTR2;
    int nEnablePMTG;
    int nEnablePMTB;
    int nPMTNum;

    int nEnableDelayCodeInfo;       //! 加密提醒使能标志
    int nEnableLampAI;              //! 使能智能恒流源
    int nLampAINum;                 //! 智能恒流源个数
    int nCurSrcBoradLampNum[MAX_SRC_BOARD];      //! 每块智能恒流源板接灯的个数
    int nAutoSigEnable;             //! 一键自动使用

    int nStatisticEnable;           //! 使用大数据采集功能
    char MachineInfo[64];           //! 机器信息
    char EngineerInfo[64];          //! 工程师信息
    char MachineIDInfo[64];         //! 设备ID信息
    int nWipeMode;                  //! 清灰模式:0-单一 1-复合

    int nFeedCounter;               //! 系统累计加工时间
    int nPressureAlarmCount;        //! 累计气压报警次数

    int nFeederCtrlTickEn[MAX_GROUP_TICK];      //! 料位监控分次使能标志
    int nTemperatureThreshold;		//! 温度临界阈值
    int nAlarmEnable[MAX_ALARM];	//! 报警使能

    int  nEnableAI;                 /*!< 使能智能 */
    int  nEnableTradition;          /*!< 使能传统 */
    int  nMaterPer1;                /*!< 参数：物料含杂1 */
    int  nMaterPer2;                /*!< 参数：物料含杂2 */
    int  nMaterPer3;                /*!< 参数：物料含杂3 */
    int  nReservedSens;             /*!< 参数：保留灵敏度*/

    int nCurCapLevel;               /*!< 参数: 当前采集层 */
    int nCurCapUnit;                /*!< 参数：当前采集相机 */

    int nLedMode;                   /*!< LED指示灯模式（仅LD4机型使用） 0-双排 1-四排 */
    int nRunMode;                   /*!< 屏运行模式设定 */

    double aFactor;                  // 实时产量计算系数a
    double bFactor;                  // 实时产量计算系数b

    int bAssitCameraEnable;        // 标识当前是否有辅配相机使能

    int aiEnable;                   // ai使能 1： 使能， 0， 不使能，默认为0
    int imgFetchHeight;
    int imgInferHeight;
    int imgPicHeight;
    int imgVideoHeight;

    quint64 nThroughtPut[MAX_UNIT / 2];          // 总产量
};
/* **********************************************************************************
 *
 *                          相机参数相关结构体
 *
 * *********************************************************************************/
 /*!
  * \brief 需要区分各个相机板地址的相机参数结构体
  */
struct struLevelCameraInfo
{
    int nChannelBegin[MAX_UNIT];                                // 象元起始
    int nChannelEnd[MAX_UNIT];                                  // 象元终止
    int nMaizeChannelBegin[MAX_UNIT][2];                        // 象元起始,玉米增加
    int nMaizeChannelEnd[MAX_UNIT][2];                          // 象元终止,玉米增加
    int nChannelEjectorBegin[MAX_UNIT][MAX_EJECTOR];            // 喷嘴象元起始
    int nChannelEjectorEnd[MAX_UNIT][MAX_EJECTOR];              // 喷嘴象元终止
    int nBiasingRed[MAX_UNIT];                                  // 红色偏置
    int nBiasingGreen[MAX_UNIT];                                // 绿色偏置
    int nBiasingBlue[MAX_UNIT];                                 // 蓝色偏置
    int nAnaGainRed[MAX_UNIT];                                  // 红色模拟增益
    int nAnaGainGreen[MAX_UNIT];                                // 绿色模拟增益
    int nAnaGainBlue[MAX_UNIT];                                 // 蓝色模拟增益
    int nDigiGainUpperRed[MAX_UNIT];                            // 红色数字增益高4位
    int nDigiGainUpperGreen[MAX_UNIT];                          // 绿色数字增益高4位
    int nDigiGainUpperBlue[MAX_UNIT];                           // 蓝色数字增益高4位
    int nDigiGainLowRed[MAX_UNIT];                              // 红色数字增益低4位
    int nDigiGainLowGreen[MAX_UNIT];                            // 绿色数字增益低4位
    int nDigiGainLowBlue[MAX_UNIT];                             // 蓝色数字增益低4位
    int nEjectorBegin[MAX_UNIT];                                // 喷嘴起始
    int nEjectorEnd[MAX_UNIT];                                  // 喷嘴终止
    int nContraPosBias[MAX_UNIT];                               // 对位偏置
    int nContraPosCorrect[MAX_UNIT];                            // 对位偏置修正
};

/*!
 * \brief 相机配置参数
 * \note  保存在 cnf.camera
 */
struct struCnfCamera
{
    struct struLevelCameraInfo struLevelCamera[MAX_LEVEL];      // 需要区分各个相机板地址的相机参数结构体
    int nRefRed;                                                // 光源标定：红色参考
    int nRefGreen;                                              // 光源标定：绿色参考
    int nRefBlue;                                               // 光源标定：蓝色参考
    int nSensorType;                                            // 传感器型号---SENSOR_K:- Keda  SENSOR_T_2566:- Toshiba
    int nSensorRowFre;                                          // 东芝传感器行频
    int nLensType;                                              // 镜头型号：1: 美亚镜头, 需要畸变校正, 0: 其他，不需要畸变校正
    int nSensorPixel;                                           // 传感器分辨率， 1024: 1024分辨率，2048：2048分辨率
    int nSensorPixelLevel[MAX_LEVEL];                           // 传感器分辨率， 1024: 1024分辨率，2048：2048分辨率

    /* RS机型参数 */
    int nChannelRangeBegin;     // 象元范围的起始象元
    int nChannelRangeEnd;       // 象元范围的终止象元
    int nFrontExpTime;          // 前视曝光时间:1-短,2-中,3-长
    int nRearExpTime;           // 后视曝光时间:1-短,2-中,3-长

    /* 黄曲霉毒素 */
    int nPMTFrameFront;         // 前视帧起始
    int nPMTFrameRear;          // 后视帧起始

    /** RSC后视校正系数 */
    int nRearRefRed;            //! 后视光源标定：红色参考
    int nRearRefGreen;          //! 后视光源标定：绿色参考
    int nRearRefBlue;           //! 后视光源标定：蓝色参考
};

/* **********************************************************************************
 *
 *                          色选方案参数相关结构体
 *
 * *********************************************************************************/
 /*!
  * \brief 识别组参数
  */
typedef struct stu_profile_group_identify
{
    stu_grey_color struGreyColor[4];    // 0: 灰度 A,  1: 灰度 B,  2: 色差 A,  3: 色差 B
    stu_cs struCross;                   // 差分算法
    stu_sp struShape;                   // 形状算法
    stu_sc struScale[2];                // 大小参数1,2
    stu_bd struBud[2];                  // 花生选芽1, 2
    stu_pn_abcd struPeanutAbcd[4];      // A、B、C、D类杂质
    stu_it	struIntel[4];               // 智能模式A、B、C、D
    stu_melon struMatMelon;             // 西瓜子
    stu_rice struRice;                  // 大米算法
    stu_maize	struMaize;              // 玉米
    stu_pole	struPole;               // 选杆算法
    stu_slice   struSlice;              // 选片算法
    stu_leaf    struLeaf;               // 选芽算法
    stu_general_reserved struReserved;          // 保留参数,通用保留,独立算法,与其他算法无关
    stu_candy   struCandy;              // 糖果算法
    stu_pistachio   struPistachio;      // 开心果算法
    stu_ai   struAi;                    // ai算法


}stu_group_identify;

/* ---------------------------------------------------- */
/*!
 * \brief 剔除组参数
 */
typedef struct stu_profile_group_tick
{
    int nEjectDuration;             // 吹气时间
    int nEjectDurationCent;         // 吹气时间百分位
    int nEjectDelay;                // 延迟时间
    int nEjectDelayCent;            // 延迟时间百分位
    int nEjectDelayBias;            // 侧视延迟时间,LD4
    int nEjectDelayBiasCent;        // 侧视延迟时间百分位,LD4
    int nFrontRearMode;             // 前后视算法关系 0-或, 1-与
    int nImpuredFlag;               // 传染算法使能标志
    int nImpured;                   // 传染算法行数
    int nCorrupted;                 // 腐蚀行数
    int nExpanded;                  // 膨胀行数
    int nShapeImpured;              // 形选传染算法行数
    int nShapeCorrupted;            // 形选腐蚀行数
    int nShapeExpanded;             // 形选膨胀行数
    int nEjwidth;                   // 吹气模式, 1-: 固定宽度, 2-: 自适应宽度, 4-:扩展宽度
    int nEdgeCut;                   // 边缘切除圈数
    int nRemoveJoint;               //去除拼接问题的边缘切除圈数
    int nBadNum;                    // 坏点个数
    int nhuffWidth;                 // 吹气宽度
    int nReverseSort;               // 正反选 0: 正选, 1, 反选;
    int nMatArea;                   // 物料区域尺寸（茶叶）
    int nMatWidth;                  // 物料宽度（茶叶）
    int nGoodNum;                   // 好点个数（茶叶）
    int nEdgeRevise;                // 边缘补偿使能, 1-:enable, 0-:disable
    int nEnableBalance;             // 平衡病斑使能（茶叶）, 1-:enable, 0-:disable

    //! RS机型增加参数
    int nEjectTimesLimit;           // 喷阀吹气次数限制
    int nEjectDegauss;              // 消磁时间
    int nEjectTruncation;           // 截断时间
    int nAssistEjectDegauss;        // 辅配消磁时间
    int nAssistEjectTruncation;     // 辅配截断时间
    int nAssistEjectTime;           // 辅配吹气时间
    int nAssistDelayTime;           // 辅配延迟时间

    //! CG添加形选前后视与或关系与正反选
    int nShapeLogicMode;            // 形选前后视与或关系 0-或, 1-与
    int nShapeSortMode;             // 前后视正反选模式 0-正选, 1-反选

    /* 米机精准剔除参数*/
    int nTickMode;                  //剔除模式：0-普通   1-精准（正反选为反选时不能使用精准模式）
    int nEjectLenthMax;             // 最大吹气长度
    int nEjectLenthMin;             // 最小吹气长度
    int nEjectLenthDif;             // 最小与最大吹气长度差值
    int nEjectProportion1;               // 比例1
    int nEjectProportion2;               // 比例2
    int nEjectThresHold;                 // 阈值

    /* 米机三次反选参数*/
    int nRounds;                     // 分割圈数

    int aiEjectorDelay;
    int aiEjectorDynamicDelay;
    int aiEjectorBlowTime;

}stu_group_tick;

/* ---------------------------------------------------- */
/*!
 * \brief 控制板参数
 */
typedef struct stu_profile_group_control
{
    int nFeeder[MAX_FEED];          // 给料量
    int nWipeDuration;              // 清灰时间
    int nWipeInterval;              // 清灰间隔
    int nLiftTimeBeforeWipe;        // 清灰前预备时间，清灰之前关闭提升机
    int nLiftTimeAfterWipe;         // 清灰后等待时间，清灰之后打开提升机
    int nWipeWindDuration;          // 气帘清灰时间  履带机专用
    int nWipeWindInterval;          // 气帘清灰间隔,  履带机专用
    int nWipeWaterDuration;         // 喷水时间,  履带机专用
    int nWipeWaterDelay;            // 喷水延时时间,  履带机专用
    int nBackgroundFront;           // 前背景角度
    int nBackgroundRear;            // 后背景角度
    int nLamp[MAX_LAMP];            // 灯控开关, 0-:off, 1-: on;

    /* RS机型参数 */
    int nFeederEnable[MAX_FEED];    // 供料器使能:0-关,1-开
    int nLampLight[MAX_LAMP];       // 物料灯及背景灯光强

    int nWipeDuration2;             // 双气缸清灰时间
    int nWipeDelay;                 // 主从屏模式下清灰同步延迟时间
}stu_group_ctrl;

/* ---------------------------------------------------- */
/*!
 * \brief 色选方案参数
 * \note  保存在 cnf.profile
 */
struct struCnfProfile
{
    int nInterfaceBoardMode;                        // 接口板启动模式
    int nColorBoardMode;                            // 彩色相机板启动模式
    int nMatIntelSeq;                               // 智能模式物料设置: 1 good 1 bad, 2:1 good 2 bad, 3:1 good 3 bad, 4:2 good, 1 bad, 5:3 good 1 bad, 6:2 good 2 bad
    int nMatAssembleMode;                           // 普通物料参数设置方式: 1-all differenct, 2-fron&rear same, 3-1st&2nd&3rd same, 4-all same
    int nMatAssembleModeIntel;                      // 智能物料参数设置方式: 0:相同设置，1：独立设置
    int nMelonUse;                                  // 西瓜子算法使能1: use watermelon arithmetic, 0: not use
    int nSensorMode;                                // 传感器模式，(仅T系), 1: light, 2: dark, 3, white_black
    int nBackgroundColor[MAX_LEVEL][MAX_BACKGROUND_GROUP];                // 背景颜色 1:- Red, 2:- green, 4:-blue, 0:-black, 7:-white
    int nBackgroundColorValue[MAX_LEVEL][MAX_BACKGROUND_GROUP];           // 背景阈值
    int nBackgroundColorPer[MAX_LEVEL][MAX_BACKGROUND_GROUP];             // 颜色比例

    int nArithmeticEnable[ARITHMETIC_TOTAL];             // 物料算法使能状态
    int nArithmeticEnableLevel[MAX_LEVEL][ARITHMETIC_TOTAL];    // 物料算法使能状态(分层独立设置，限茶叶机型使用)
    int nIntelMode;                                 // 自学习模式， 0： 普通自学习， 1： 多分类自学习
    stu_it_mat struIntelMat[4];                     // 智能模式物料名称，好坏

    stu_group_identify struGroupIdentify[MAX_LEVEL][MAX_GROUP_IDTNTIFY];    // 识别组
    stu_group_tick struGroupTick[MAX_LEVEL][MAX_GROUP_TICK];                // 剔除组， 0： 一次， 1： 二次， 2： 三次
    stu_group_ctrl struGroupCtrl[MAX_CTRL];                                 // 控制板

    int nSensBias[MAX_UNIT][ARITHMETIC_TOTAL];              // 灵敏度偏置
    int nSensBiasMaize[MAX_UNIT][MAX_MAIZE_SENSNUM_TOTAL];  // 灵敏度偏置 未考虑层  玉米定制专用

    /* RS机型参数 */
    int nDegaussTime;                               // 消磁时间
    int nAnaGain[MAX_UNIT];                         // 相机模拟增益
    int nDigGain[MAX_UNIT];                         // 相机数字增益
    int nGainTarget[MAX_UNIT];                      // 相机增益目标
    int nSens[MAX_UNIT][2];                         // 主配独立灵敏度
    int nDarkSens[MAX_UNIT][2];                     // 主配独立灵敏度(暗),仅自定义模式使用
    int nCrossChannel[MAX_GROUP_TICK];              // 交叉象元
    int nDelayBiasing[MAX_UNIT / 2];                  // 延迟偏置
    int nFeederBias[MAX_UNIT / 2];                    // 供料偏置
    int nColorSpace;                                // 颜色空间 0：RGB 1：HSV
    int nAIMode;                                    // AI模式  0：高维 1:低维
    int isFeederAIEnable;                           // 智能供料使能标志,0:禁止，1：使能
    int autoFeedEnablePageFlag;                     // 智能供料页面的显示标志
    int nFeederCapMin;                              // 智能供料供料量范围之最小值
    int nFeederCapMax;                              // 智能供料供料量范围之最大值
    int nEjectTimesMin;                             // 智能供料吹气次数范围之最小值
    int nEjectTimesMax;                             // 智能供料吹气次数范围之最大值
    int nAutoSigVal[MAX_LEVEL][MAX_UNIT][3];        /*!< 一键自动使用(彩色)*/
    int nAutoAdVal[MAX_LEVEL][MAX_UNIT][3];         /*!< 一键自动使用（彩色）*/
    int nAutoSigInit;
    int nAutoBgVal[MAX_LEVEL][MAX_UNIT][3];         /*!< 一键自动使用*/
    int nTeaProfileCustomType[MAX_LEVEL][4];        // 茶叶机型定制方案
    int nAutoBackgroundEnanble_RSC;                     // 自动背景使能
    int nAutoBackgroundThresholdValue_RSC;                            // 自动背景阈值
    int nAutoBgEnable[MAX_LEVEL][MAX_UNIT];         // 杂粮背景自适应使能标志
    int nAutoBgRange[MAX_LEVEL][MAX_UNIT][3];       // 杂粮背景自适应背景值变化幅度，3表示的是RGB

    /* 性能参数统计设置参数 */
    int nMaterialQueryInterval;       // 物料的统计时间
    int nHistogramQueryTime;          // 直方图的统计时间
    int nCameraRef;                   // 参考相机的全局编号

    int nArithEdgeCut[MAX_UNIT][ARITHMETIC_TOTAL]; // 算法边缘切除圈数
};


/*************************************************************************************
 *
 *      以下是不需要保存的全局共享参数结构体定义
 *
 ************************************************************************************/

 /*!
  * \brief 延迟损耗
  */
struct stu_gsh_tick
{
    float nEdgeCutDelay;
    int   nMaxMatSize;
    float nMatSizeDelay;
    float nShapeEdgeCutDelay;
};

/*!
 * \brief 版本信息
 */
struct struVersion
{
    int sControl[MAX_CTRL][2];                    // 控制板版本，0： 大版本号， 1： 小版本号
    int sInterface[MAX_LEVEL][8];                 // 接口板版本， 0： 大版本号， 1： 小版本号，2：定制机型编号
    int sUnit[MAX_LEVEL][MAX_UNIT][8];            // 彩色相机板版本， 0：小版本号， 1： 大版本号，2：定制机型编号
    int nVersionRequire;                          // 保留
    int sLightSrc[MAX_LIGHT_SOURCE][2];           // 恒流源版本, 0：大版本号, 1：小版本号
    int nUsbFirmware;                             // USB固件版本, 1：老固件(单向,显示V1.00) 2：新固件(双向,显示V2.00)
};

/*!
 * \brief 识别结果
 */
struct struIdentifyResult
{
    int nPixelSum;      //! 物料像素点数
    int nBadNum[IDE_RESULT_TYPE];   //! 颜色识别算法识别坏点数
    bool nEn[IDE_RESULT_TYPE];      //! 算法显示使能
};

#define DEF_TIMEOUT_STATISTIC    (30)		/* min */
#define DEF_TIMEOUT_ALARM        (60)		/* s */
#define DEF_TIMEOUT_LOG          (60*60*1000)	/* ms */

#define DEF_VERSION_STATISTIC	 1

#define MAX_STATISTIC_MATERIAL	    368 
#define MAX_STATISTIC_EJTIMES	    480
#define MAX_STATISTIC_HISTOGRAM	    3072
#define MAX_STATISTIC_BACKGROUND    6144
#define MAX_STATISTIC_HEADINFO	    16

/*!
 * \brief 大数据信息结构体
 */
typedef struct
{
    QByteArray bgBeforeWipeStatistic;           // 清灰前的背景信息
    QByteArray bgAfterWipeStatistic;            // 清灰后的背景信息
    QByteArray materialStatistic;               // 物料点信息统计
    QByteArray ejectStatistic;                  // 吹气次数统计
    QByteArray histogramStatistic;              // 信号直方图
} StruStatisticInfo;

/*!
 * \brief 全局共享参数
 */
struct struShare
{
    int nSerialCount1;                      // 串口1 序号
    int nSerialCount2;                      // 串口2 序号
    int nSerialCount3;                      // 串口3 序号
    int nMachine;                           // 机型：LD2或者CF
    int nCounter;                           // 本次运行时间
    int nSelfExamineFre;                    // 整机自检频率
    float fPixelCompensate[MAX_EJECTOR_SIXTY];    // 象元划分校正系数
    struct struVersion struVer;             // 版本信息

    int bStatFeed;                          // 给料状态    : 1 - on, 0 - off
    int bStatBelt[MAX_LEVEL];               // 输送带状态     : 1 - on, 0 - off
    int bStatEjectFpga;                     // FPGA开关状态     : 1 - on, 0 - off
    int bStatFeedCtrlEn;                    // 料位监控使能：1-使能 0-不使能

    int bFlagFirstrun;                      // 开机运行状态 : 1 - first run, 0 - not
    int bFlagAutowipe;                      // 自动清灰使能 : 1 - auto wipe, 0 - not
    int bFlagManuwipe;                      // 手动清灰标志
    int bFlagRowing;                        // 波形请求使能   : 1 - get row data, 0 - not
    int bFlagMachinetest;                   // 整机自检状态    : 1 - 整机自检, 0 - 正常运行
    int bFlagPasswdAble;                    // 高级参数密码  1: need passwd to advanced, 0:disable

    int nFeederControlLevel;                // 料位监控的水平数值
    int nAlarmPLCReq;                       // PLC当前执行状态 0:开,1:关
    int nAlarmPLCReqFlag;                   // PLC执行状态的跳变标志 0:无跳变,1:有跳变
    int bAlarmPressure;                     // 气压报警状态 : 气压报警状态 : 00(0) - normal  01(1) - 气压不足（只提醒）  11(3) - 气压报警（提醒+关供料）
    int bAlarmEjector;                      // 喷阀报警状态  : 1 - alarm, 0 - normal
    int bAlarmLamp;                         // 灯报警状态  : 1 - alarm, 0 - normal
    int bAlarmBlower;                       // 风机警状态  : 1 - alarm, 0 - normal
    int bAlarmSlaveComm;                    // 网络通信报警
    int bAlarmStatus;                       // 整机报警状态: 1 - alarm, 0 - not

    float nSensorTemperature[MAX_TEMPERATURE_SENSOR];		// 温度传感器反馈值
    float nTemperatureMax;					// 温度反馈最大值
    float nTemperatureMin;					// 温度反馈最小值

    int nLevel;                             // 当前层编号
    int nUnit;                              // 当前处理单元编号
    int nUnitFeederAI;                      // 当前处理单元编号，智能供料使用
    int nMatGroup;                          // 当前物料参数所在组0-: first front, 1-: first rear, ... 4-: 3rd front, 5-: 3rd rear

    char sRgbRow[SENSOR_MAX_PIXELS * 3 + 16];   // 一帧波形数据信息
    char sRowRed[SENSOR_MAX_PIXELS];        // 红值信号
    char sRowGreen[SENSOR_MAX_PIXELS];      // 绿值信号
    char sRowBlue[SENSOR_MAX_PIXELS];       // 蓝值信号
    int nIamgeLine;                         // 波形请求行数
    int nSensCoefficient;                   // 传感器帧频计算系数

    int nIntelSampColor[IMAGE_MAX];         // 智能模式取样颜色：1: red, 2: green, 4: blue
    int nIntelSampMode[IMAGE_MAX];          // 取样模式: 0,选暗, 1:选亮
    int nIntelSampSens[IMAGE_MAX];          // 取样灵敏度
    int nIntelSampType;                     // 传输方式 0:UART,1:USB
    int nIntelSampBurst;                    // 触发阈值
    int nIntelSampAi;                       // 普通模式

    float nEjectImpureDelay[MAX_LEVEL][MAX_GROUP_TICK];         // 传染算法损耗时间
    float nShapeImpureDelay[MAX_LEVEL][MAX_GROUP_TICK];         // 形选算法传染损耗时间
    struct stu_gsh_tick struTick[MAX_LEVEL][MAX_GROUP_TICK];    // 剔除损耗时间
    float delayPerRow;                      // 根据不同传感器计算每行的延迟损耗
    int rowColumnRelation;                  // 根据不同传感器和帧频，获取行列数比值关系  1: 列数=行数， 2：列数=行数*2
    int maxRowNumber;                       // 根据不同传感器和帧频，可设置的最大行数不同, T2:16, T3:32, D:32

    int nEjectEdgeDelay[MAX_LEVEL][MAX_GROUP_TICK];                   // 边缘切除损耗时间
    int nMaxMaterialSize[MAX_LEVEL][MAX_GROUP_TICK];                  // 最大物料算法尺寸
    int nEjectMaterialSizeDelay[MAX_LEVEL][MAX_GROUP_TICK];           // 物料算法尺寸延迟时间s

    int nBacklightStat;                     // 当前背光的状态 0:off, 1:on
    int nBacklightCounter;                  // 背光开启的时间

    int nAuthenticationLevel;               // 访问权限 0 - 操作工, 1 - 工程师, 2 - 厂家
    int nPixelBegin;                        // 计算像元划分起始
    int nPixelEnd;                          // 计算像元划分截止

    /* RS机型增加参数 */
    bool bIsAssist;                         // 记录当前是否为辅配
    bool bCurAssist;
    int pEjectTimes[MAX_EJECTOR_SIXTY * 4];   // 记录喷嘴的吹气次数(前视及后视)
    QString first;                          // 一次,根据次数分配的不同显示不同的内容
    QString second;                         // 二次
    QString third;                          // 三次
    QString fourth;                         // 四次

    bool bAlarmCloseFeed;                   // 标志是否因报警关闭下料
    bool bAlarmCloseEject;                  // 标志是否因气压不足关闭喷阀
    int nAlarmLevel;                        // 料位报警标志位
    int nAlarmLevelGroup[MAX_GROUP_TICK];   // 分组记录料位报警标志
    int nCommStat;                          // 标志通信状态:0-Ok 1-Fail
    bool bIsMC;                             // 是否为印度定制机型(MC系列)
    bool bIsRF;                             // 标志是否为RF机型

    bool enableJudgeBeanMachine;            // 使能允许获取判断是否豆类机型
    bool isPeaMachineBeanMat;               // 豆类机型色选的是否为豆类物料 false：否,  true：是
    int nIntBaud[MAX_INT];                  // 接口板波特率
    int nCtrlBaud[MAX_INT];                 // 控制板波特率
    int nCustomReq;                         // 根据不同客户需求对通用机型做某些改变，适用于不同的物料，可用与临时定制

    struIdentifyResult struResult[MAX_UNIT];//! 颜色算法识别结果

    int curIntelArithNo;                    //! 当前使用的智能算法序号
    int nDevTypeAddrBias;                   // 当前衍生机型新增相机行的起始地址
    int nBackgroudType;                     // 当前处理的背景类型，0：可见光，1：红外

    StruStatisticInfo struStatisticInfo;    // 性能参数统计
    int bStatisticFinished;
    int nWipeTimes;
    bool isBigDataUpdated;
    bool isQueryingCom3;

    // 报警状态变化处理
    bool bAlarmChanged;			    // 报警状态变化标识
    unsigned int nAlarmStatus;      // 报警状态
    int nFeedCounter;			    // 开机加工时间
    int nTotalAlarmPreTimes;        //开机气压报警次数
    QStringList strSlaveList;       // 从设备列表

    int idIndex[MAX_LEVEL * 2];       // TD机型的TabBar对应的识别组位置

    QString tempPwd;                // 临时密码
    int aiDeviceNum;
    QList<QHostAddress>     addressList;
    QList<QString>         ftpIpAddressList;
    QHostAddress     address;
    QString          ftpIpAddress;
    bool             isImageSend;
    QString aiResult[MAX_UNIT / 2];

    QString wifiSsid;
    QString wifiPassWord;
    quint64 nThroughtPut[MAX_UNIT / 2]; // 本次总产量


};

typedef struct
{
    int selfNetControlPort;     // 端口号
    QHostAddress groupAddress;  // 组播地址（可支持多播或广播）
} StrNetConfig;


/* DFS搜索方向数组 */
static const int pDirection[4][2] = { {1, 0}, {0, -1}, {0, 1}, {-1, 0} };

extern int nSmallMatArea;
/* 定义物料参数 */
struct MaterialParams
{
    int nLabel;         // 物料标签号
    int nNum;           // 物料像素点个数
};

#define MaxDecryptTimes       	3     // 最大解密次数

typedef struct
{
    int  isEncrypt; //是否加密  0  false  1 true
    QString firstEncryDate; //首次加密日期
    //    bool isDecrypt; //是否解密
    QString  DecryptCode[MaxDecryptTimes];
    int DecryptType; //解密模式 1 延迟解密 2完全解密 0加密后未解密
    int DecryptDate; //延迟解密日期

} StrEncrypt;

/*************************************************************************************
 *
 *      MyGlobalString 是用于初始化时，完成对界面文字赋值，这部分文字不能保存到文件中，如一次前后视...
 *
 ************************************************************************************/
class MyGlobalString : public QObject
{
public:
    MyGlobalString() {};
    ~MyGlobalString() {};

    QString sIdentifyGroupName[MAX_LEVEL][MAX_GROUP_IDTNTIFY];   // 识别组界面显示名称, 一次前后视、二次前后视、三次前后视， 上下视...
    QString sTickGroupName[MAX_LEVEL][MAX_GROUP_TICK];           // 剔除组界面显示名称, 一次、二次、三次， 上下层...
    QString sArithmeticName[ARITHMETIC_TOTAL];                   // 物料识别算法名称
    QString sLamp[MAX_LAMP];                                     // 灯光设置名称
    QString inf_even;

    QString sColorSelectGroupName[MAX_LEVEL];                    // 色选模式界面组名
    QString sColorSelectModeName[MAX_COLOR_MODE];                // 色选模式名称

    QString sVendor;
    QString sMachine;
    QString sProduct;
    QString sCustomReq;
    QString sAppVersion;
    QString sAppSVN;
    QString sServer;
protected:

private:
};

/*************************************************************************************
 *
 *      全局变量外部声明
 *
 ************************************************************************************/

extern MyQextSerialPort MySerial;           // 全局串口类

extern struct struShare struGsh;            // 全局共享参数

extern struct struCnfGlobal struCnfg;       // 全局保存参数
extern struct struCnfGlobal _t_struCnfg;    // 全局保存参数备份
extern struct struCnfCamera   struCnfc;     // 全局相机参数
extern struct struCnfCamera _t_struCnfc;    // 全局保存参数备份
extern struct struCnfProfile struCnfp;      // 全局方案参数
extern struct struCnfProfile _t_struCnfp;   // 全局方案参数
extern struct struCnfEngineer struCnfe;     // 全局工程文件参数
extern struct struCnfEngineer _t_struCnfe;  // 全局工程文件参数
extern struct struCnfFileStatus	struCnfs;   // 参数状态文件
extern MyGlobalString myString;             // 全局字符串类,用于显示不能保存到文件的界面名称
extern StrNetConfig strNetInfo;             // 全局设备或服务器信息

#endif // GLOBALPARAMS_H
