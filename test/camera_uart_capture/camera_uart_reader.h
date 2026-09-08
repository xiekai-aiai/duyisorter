/*!
 * \file        camera_uart_reader.h
 * \brief       基于 FPGA 协议的 UART 串口相机图像采集类（独立可复用模块）
 * \details
 *
 *  ╔══════════════════════════════════════════════════════════════════════════╗
 *  ║  硬件架构                                                                  ║
 *  ║                                                                            ║
 *  ║  RK3566 ──── UART (ttyUSB0) ──── FPGA 下位机 ──── 并行总线 ──── CCD 相机板  ║
 *  ║  (ARM64)    115200bps          (地址 Unit 0~7)                  (SONY 等)  ║
 *  ║                                                                            ║
 *  ╚══════════════════════════════════════════════════════════════════════════╝
 *
 *  ╔══════════════════════════════════════════════════════════════════════════╗
 *  ║  协议帧格式 (16 字节固定长度, CRC-CCITT 校验)                               ║
 *  ║                                                                            ║
 *  ║  [0]  0xA5  帧头1                                                           ║
 *  ║  [1]  0x5A  帧头2                                                           ║
 *  ║  [2]  模块地址高 (板号<<4 | 模块类型)                                       ║
 *  ║  [3]  模块地址低 (Unit 0~7, 0x3F=广播全部)                                   ║
 *  ║  [4]  命令码高                                                               ║
 *  ║  [5]  命令码低                                                               ║
 *  ║  [6]  序列号 (每次 +1, 溢出归零)                                             ║
 *  ║  [7]~[11]  5 字节参数 (arg1~arg5)                                           ║
 *  ║  [12] CRC-CCITT 高                                                          ║
 *  ║  [13] CRC-CCITT 低                                                          ║
 *  ║  [14] 0xFF  帧尾1                                                           ║
 *  ║  [15] 0xFF  帧尾2                                                           ║
 *  ║                                                                            ║
 *  ║  CRC-CCITT 多项式: x^16 + x^12 + x^5 + 1  (0x1021)                        ║
 *  ║  CRC 覆盖范围: sPacket[2] ~ sPacket[11] 共 10 字节                          ║
 *  ║                                                                            ║
 *  ╚══════════════════════════════════════════════════════════════════════════╝
 *
 *  ╔══════════════════════════════════════════════════════════════════════════╗
 *  ║  图像数据帧格式 (FPGA 推送到 UART)                                           ║
 *  ║                                                                            ║
 *  ║  [0] 0xAA  帧头1  ← checkFrame() 用这 4 字节验帧                           ║
 *  ║  [1] 0xAA  帧头2                                                            ║
 *  ║  [2] 0xAA  帧头3                                                            ║
 *  ║  [3] 0xAA  帧头4                                                            ║
 *  ║  [4] ~ [N-1]  纯像素数据 (8bit CCD 灰度 / 16bit RAW)                        ║
 *  ║                                                                            ║
 *  ╚══════════════════════════════════════════════════════════════════════════╝
 *
 *  使用示例:
 *  @code
 *      CameraUartReader reader;
 *      reader.setSerialPort("/dev/ttyUSB0", BAUD115200);
 *      reader.open();
 *
 *      CaptureConfig cfg;
 *      cfg.nUnitAddr   = 0;      // 第 0 块相机板
 *      cfg.dataDepth   = 500;    // 协议数据包行数 (= CAM_IMAGE_HEIGHT)
 *      cfg.dataLen     = 500*(1024*2+16);  // 原始数据 = 行数*(WIDTH*2+FRAME_LEN_NOT_DATA)
 *      cfg.nTransType  = 0;      // UART 传输
 *
 *      unsigned char *buf = nullptr;
 *      int result = reader.capture(&cfg, buf);
 *      if (result == ERR_SUCCESS) {
 *          // buf 里已经是剥掉帧头的纯像素数据
 *          cv::Mat img(500, 1024, CV_8UC1, buf);
 *          cv::imwrite("capture.bmp", img);
 *          delete[] buf;
 *      }
 *      reader.close();
 *  @endcode
 *
 *  \author  DuySorter Project
 *  \date    2026.09.03
 */

#ifndef CAMERA_UART_READER_H
#define CAMERA_UART_READER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

// ============================================================================
// 常量定义 (原项目里这些散落在 myqextserialport.h / globalparams.h / aishare.h)
// ============================================================================

/** 中控板最大 Unit 数 */
#define MAX_UNIT_PER_INT    8

/** 最大层数 (供料分组) */
#define MAX_LEVEL           3

/** 模块类型 (与原项目 myqextserialport.h 一致) */
#define INT_MODULE  0x01  /**< 中控板 (interface board) */
#define UNIT_MODULE 0x02  /**< 相机板 (unit board)       */

/** 图像尺寸 */
#define CAM_IMAGE_WIDTH     1024
#define CAM_IMAGE_HEIGHT    500
#define CAM_IMAGE_WIDTH_MAX 2048

/** UART 图像帧头: 4 字节连续 0xAA */
#define FRAME_HEADER_AA     0xAA
#define FRAME_HEADER_LEN    4

/** UART 图像帧结构 (PROTOCAL_3 前视/辅配灰度相机) */
#define FRAME_PACKET_HEAD   10   /**< 每行包头字节数 (前 4 字节 0xAA AA AA AA) */
#define FRAME_PACKET_TAIL   6    /**< 每行包尾字节数 */
#define FRAME_LEN_NOT_DATA  16   /**< 每行非数据字节 = 包头10 + 包尾6 */

/** FPGA 命令码 (通用机型协议, 对应 doc/笃一光电通用色选机平台机型上位机通信协议1.docx 相机板命令) */
#define CMD_INT_IMAGE_CAPTURE_MODE   0x08   /**< 接口板: 图像获取方式 (Byte10/11=行数, Byte11=传输类型) */
#define CMD_UNIT_ONOFF               0x01   /**< 运行开始/停止 (Byte11: 0停止/1开始/2自检)      */
#define CMD_UNIT_IMG_BURST           0x36   /**< 智能算法触发阈值 (触发像元个数高/低)            */
#define CMD_UNIT_IMAGE               0x03   /**< 图像获取 (Byte9=0:RS485/1:USB, Byte10/11=行数) */
#define CMD_UNIT_MAIZE_IMAGE_UPLOAD  0x45   /**< 玉米机型图像上传开始 (无参数)                  */
#define CMD_UNIT_VERSION             0x2001 /**< 读取版本信息 (回 8 字节, 用于通信握手诊断)      */

/** 相机类型 (版本查询等命令用) */
#define CAMERA_COLOR                 1      /**< 彩色相机 */

/** 传输类型 */
#define TRANS_UART   0   /**< 串口  */
#define TRANS_USB    1   /**< USB   */
#define TRANS_UDP    2   /**< UDP   */

/** 采集结果错误码 */
#define ERR_SUCCESS       0
#define ERR_INPUT_ARG    -1
#define ERR_CONFIG       -2
#define ERR_MALLOC_FAILED -3
#define ERR_TIMEOUT      -4
#define ERR_DATA         -5
#define ERR_SERIAL_OPEN  -6
#define ERR_SERIAL_WRITE -7
#define ERR_SERIAL_READ  -8

/** 模块类型字段 (SModuleType, 与原项目 myqextserialport.h 一致) */
struct ModuleType {
    enum {
        INT  = 0x01,   /**< 中控板 */
        UNIT = 0x02    /**< 相机板 */
    };
};


// ============================================================================
// 数据结构
// ============================================================================

/**
 * \brief 单次采集配置
 */
struct CaptureConfig {
    int  nSeq;           /**< 采集序号 (用于多板顺序采集) */
    int  nUnitAddr;      /**< 相机板地址 0~7, 或 0x3F=广播全部 */
    int  dataDepth;      /**< 协议数据包行数 (= CAM_IMAGE_HEIGHT = 500) */
    int  dataLen;        /**< 原始数据总字节 = dataDepth * (WIDTH*2 + FRAME_LEN_NOT_DATA) */
    int  nTransType;     /**< 传输类型: 0=UART 1=USB 2=UDP */
    int  result;         /**< 采集结果错误码 (函数返回后读此字段) */
};


// ============================================================================
// CameraUartReader 类
// ============================================================================

/*!
 * \brief UART 串口相机采集类
 * \details 封装了命令帧构建、CRC 校验、串口 write/read、图像帧拆包、帧头验证的完整流程
 */
class CameraUartReader {
public:
    CameraUartReader();
    ~CameraUartReader();

    // ------------------------------------------------------------------
    // 串口配置与生命周期
    // ------------------------------------------------------------------

    /**
     * \brief 指定串口设备路径和波特率 (打开前调用)
     * \param device  设备路径, 如 "/dev/ttyUSB0"
     * \param baud    波特率, 如 B115200 / B230400 / B460800 / B921600
     */
    void setSerialPort(const char *device, speed_t baud = B115200);

    /** 设置当前层数 (供料分组) */
    void setLevel(int level) { m_level = level; }

    /** 设置 UDP socket fd (可选, 用于前置 CMD_AI_IMG_VID_UPLOAD 通知) */
    void setUdpFd(int fd) { m_udpFd = fd; }

    /** 开启/关闭详细日志 */
    void setVerbose(bool v) { m_verbose = v; }
    bool isVerbose() const  { return m_verbose; }

    /** 开启/关闭原始数据 dump (触发上传后把串口收到的字节写入文件) */
    void setDump(bool d) { m_dump = d; }
    bool isDump() const  { return m_dump; }

    /** 指定 dump 输出文件路径 (空则自动命名 uart_dump_unitX_seqY.hex) */
    void setDumpFile(const char *path) {
        if (path == nullptr) { m_dumpFile[0] = '\0'; return; }
        strncpy(m_dumpFile, path, sizeof(m_dumpFile) - 1);
        m_dumpFile[sizeof(m_dumpFile) - 1] = '\0';
    }

    /** 打开串口 */
    int open();

    /** 关闭串口 */
    void close();

    /** 是否已经打开 */
    bool isOpen() const { return m_fd >= 0; }


    // ------------------------------------------------------------------
    // 核心采集 API
    // ------------------------------------------------------------------

    /**
     * \brief 完整的单次采集流程 (配模式 → 触发 → 上传 → 读帧 → 验帧 → 剥帧头)
     *
     * \param cfg          [in] 采集配置 (nUnitAddr / dataDepth / dataLen 必填)
     * \param outBuf       [out] 成功后存放纯像素数据 (caller 负责 delete[] / free)
     * \param timeoutMs    [in] 单帧读取超时 (ms), 默认 5000
     *
     * \return  ERR_SUCCESS (0) 成功, 负值见 ERR_* 错误码
     *
     * \note
     * 内部流程 (UART 模式, 通用机型协议):
     *   1. 发送 CMD_UNIT_IMG_BURST           配图像拍摄触发条件 (触发像元个数)
     *   2. 发送 CMD_UNIT_IMAGE               图像获取 (RS485 上传 + 行数)
     *   3. 发送 CMD_UNIT_MAIZE_IMAGE_UPLOAD  图像上传开始
     *   4. 循环 readData() 读 N 帧, 每帧 checkFrame()
     *   5. stripFrameData() 剥掉帧头, 提取纯像素
     */
    int capture(CaptureConfig *cfg, unsigned char *&outBuf, int timeoutMs = 5000);


    // ------------------------------------------------------------------
    // 低层 API (可单独调用, 用于分步调试)
    // ------------------------------------------------------------------

    /** 构造并发送一个 16 字节 FPGA 命令帧 */
    int sendCmd(int nCmd, char sModuleType, char sIntAddr, char sUnitAddr,
                char arg1 = 0, char arg2 = 0, char arg3 = 0, char arg4 = 0, char arg5 = 0,
                int nCount = 1);

    /** 从串口读数据, 返回实际读到的字节数 (阻塞 + 内部定时轮询) */
    int readData(unsigned char *buf, int nLen, int timeoutMs = 5000);

    /**
     * \brief 把串口当前能读到的原始字节持续写入文件 (诊断用)
     * \param filename   输出文件路径
     * \param timeoutMs  持续抓取时长 (ms), 默认 3000
     * \return 抓取到的总字节数 (0 表示这段时间内串口完全无数据)
     */
    int dumpRaw(const char *filename, int timeoutMs = 3000);

    /**
     * \brief 发送版本查询命令 (0x2001) 并抓取回包 (通信握手诊断)
     * \param unitAddr   相机板地址 0~7
     * \param timeoutMs  抓取时长 (ms)
     * \return 抓取到的字节数 (0 表示 FPGA 无回包 → 物理层/波特率/地址问题)
     */
    int probeVersion(int unitAddr, int timeoutMs = 2000);


    // ------------------------------------------------------------------
    // 静态工具函数 (协议相关, 可复用)
    // ------------------------------------------------------------------

    /** CRC-CCITT 校验 (多项式 0x1021) */
    static uint16_t crcFpga(const char *data, int len);

    /** 帧头验证: 必须连续 4 字节 0xAA */
    static int checkFrame(const unsigned char *data, int dataLen);

    /**
     * \brief 剥掉所有帧头, 提取纯像素数据
     * \param data    [in/out] 原始数据 (帧头+像素 交错)
     * \param cfg     [in]     采集配置
     * \return 0=成功, -1=strip 出错
     */
    static int stripFrameData(unsigned char *data, const CaptureConfig &cfg);


private:
    // ------------------------------------------------------------------
    // 成员变量
    // ------------------------------------------------------------------

    int         m_fd;          /**< 串口文件描述符, -1 表示未打开 */
    char        m_device[64];  /**< 设备路径 "/dev/ttyUSB0" */
    speed_t     m_baud;        /**< 波特率 */
    int         m_level;       /**< 当前层数 (供料分组), 默认 0 */
    int         m_udpFd;      /**< UDP socket fd (可选, 默认 -1) */
    int         m_seq;         /**< 命令序号计数器, 每次 sendCmd +1, 超 250 归零 */
    bool        m_verbose;     /**< 详细日志开关 */
    bool        m_dump;        /**< 原始数据 dump 开关 */
    char        m_dumpFile[256]; /**< dump 输出文件路径 (空 = 自动命名) */


    // ------------------------------------------------------------------
    // 内部辅助
    // ------------------------------------------------------------------

    /** 构造 16 字节命令帧 */
    static void buildPacket(char packet[16],
                            int nCmd, char sModuleType, char sIntAddr, char sUnitAddr,
                            char arg1, char arg2, char arg3, char arg4, char arg5,
                            int seq);

    /** 设置 termios (8N1, cbreak, 无流控) */
    int setTermios();

    /** ms 级 sleep */
    static void msleep(int ms);

    /** [DEBUG] 把一段 hex 数据打印出来 (每 16 字节一行) */
    static void hexDump(const char *label, const void *data, int len);

    /** [DEBUG] 把一个命令值转换成可读字符串 */
    static const char *cmdName(int nCmd);
};


#endif // CAMERA_UART_READER_H
