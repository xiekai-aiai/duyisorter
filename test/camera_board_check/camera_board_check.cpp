/*!
 * \file        camera_board_check.cpp
 * \brief       相机板在线检测独立测试程序 (RK3568 目标机)
 *
 * \details
 *   本程序通过 UART (RS-485) 向 FPGA 下发「版本查询」命令,
 *   根据是否收到 18 字节回包来判断相机板是否在线、是否正常工作。
 *
 *   相比「采图」测试, 版本查询是验证相机板是否在线的最简单、最可靠手段:
 *     - 命令简单 (16 字节固定帧, 无需复杂参数)
 *     - 回包小 (18 字节)
 *     - 不依赖分辨率/传输模式/触发时序等容易出错的配置
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │ 硬件链路                                                      │
 *  │   RK3566 ──UART(/dev/ttyS4, 115200)── FPGA ──并行总线── CCD 相机板 │
 *  └─────────────────────────────────────────────────────────────┘
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │ 命令帧 (16 字节, 发往 FPGA)                                    │
 *  │  [0] 0xA5  帧头1                                              │
 *  │  [1] 0x5A  帧头2                                              │
 *  │  [2] (层号<<4) | 模块类型   (模块类型: 0x01=中控板 0x02=相机板)   │
 *  │  [3] 相机板地址 (0~7, 0x3F=广播)                               │
 *  │  [4] 命令码高字节                                              │
 *  │  [5] 命令码低字节   (0x2001 = 版本查询)                         │
 *  │  [6] 序号 (递增)                                               │
 *  │  [7]~[11] 5 字节参数                                           │
 *  │  [12] CRC 高                                                   │
 *  │  [13] CRC 低   (CRC-CCITT 0x1021, 覆盖 [2]~[11] 共10字节)      │
 *  │  [14] 0xFF  帧尾1                                              │
 *  │  [15] 0xFF  帧尾2                                              │
 *  └─────────────────────────────────────────────────────────────┘
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │ 回包 (18 字节, FPGA 返回)                                      │
 *  │  buf[10] & 0x0F = 版本主号                                     │
 *  │  buf[11]        = 版本次号                                     │
 *  │  (完整 18 字节内容以 hex dump 打印, 便于人工核对)               │
 *  └─────────────────────────────────────────────────────────────┘
 *
 *  参考源码:
 *    - src/common/mono_shared/monoflow.cpp  checkMasterCameraBoard()
 *    - src/myqextserialport.cpp             com1Write()/com1Read()
 *    - src/myqextserialport.h               命令码定义
 *
 *  使用:
 *    ./camera_board_check                            # 检测板 0
 *    ./camera_board_check /dev/ttyS4 115200 3        # 检测板 3
 *    ./camera_board_check /dev/ttyS4 115200 -scan    # 扫描 0~7 全部板
 *    ./camera_board_check -v /dev/ttyS4 115200 0     # verbose 打印 TX/RX
 *
 *  退出码:
 *    0 = 至少一块板在线
 *    1 = 全部离线 / 串口打开失败 / 参数错误
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// ============================================================================
// 协议常量 (与 src/myqextserialport.h 保持一致)
// ============================================================================

#define MODULE_INT   0x01   // 中控板 (interface board)
#define MODULE_UNIT  0x02   // 相机板 (unit board)

#define CMD_UNIT_CAMERA_VERSION  0x2001   // 相机板版本查询
#define CMD_INT_VERSION          0x2001   // 中控板版本查询

#define CAMERA_COLOR  1         // 彩色相机标识

#define MAX_UNIT_PER_INT  8     // 每块中控板最多带 8 块相机板

#define FRAME_LEN     16        // 命令帧长度
#define REPLY_LEN     18        // 版本查询回包长度

// ============================================================================
// 工具函数
// ============================================================================

/** CRC-CCITT (多项式 0x1021, 初值 0, 与 FPGA 一致) */
static uint16_t crcFpga(const uint8_t *data, int len) {
    uint16_t crc = 0;
    for (int i = 0; i < len; i++) {
        uint8_t byte = data[i];
        for (uint8_t bit = 0x80; bit != 0; bit >>= 1) {
            if (crc & 0x8000) { crc = (uint16_t)((crc << 1) ^ 0x1021); }
            else              { crc = (uint16_t)(crc << 1); }
            if (byte & bit)   { crc ^= 0x1021; }
        }
    }
    return crc;
}

/** 构造 16 字节命令帧 (与 com1Write 完全一致) */
static void buildPacket(uint8_t packet[FRAME_LEN],
                        int nCmd, uint8_t moduleType, uint8_t level, uint8_t unitAddr,
                        uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4, uint8_t arg5,
                        uint8_t seq) {
    memset(packet, 0, FRAME_LEN);

    packet[0] = 0xA5;
    packet[1] = 0x5A;

    // 模块地址 (与 com1Write L270-276 一致)
    int nUnit = unitAddr & 0x3F;
    if (nUnit < MAX_UNIT_PER_INT) {
        packet[2] = (uint8_t)((level * 16) | moduleType);
        packet[3] = unitAddr;
    } else {
        packet[2] = (uint8_t)(((level + 1) * 16) | moduleType);
        packet[3] = (uint8_t)(unitAddr - MAX_UNIT_PER_INT);
    }

    packet[4] = (uint8_t)(nCmd / 256);
    packet[5] = (uint8_t)(nCmd % 256);
    packet[6] = seq;
    packet[7]  = arg1;
    packet[8]  = arg2;
    packet[9]  = arg3;
    packet[10] = arg4;
    packet[11] = arg5;
    packet[14] = 0xFF;
    packet[15] = 0xFF;

    // CRC 覆盖 [2]~[11] 共 10 字节
    uint8_t crcData[10];
    for (int i = 0; i < 10; i++) crcData[i] = packet[i + 2];

    uint16_t crc;
    if (unitAddr == 0x3F) {  // 广播命令用简单求和
        uint32_t sum = 0;
        for (int i = 0; i < 10; i++) sum += crcData[i];
        crc = (uint16_t)(sum & 0xFFFF);
    } else {
        crc = crcFpga(crcData, 10);
    }

    packet[12] = (uint8_t)(crc / 256);
    packet[13] = (uint8_t)(crc % 256);
}

/** hex dump 打印 */
static void hexDump(const char *label, const uint8_t *data, int len) {
    fprintf(stderr, "  [%s] %d bytes:\n", label, len);
    for (int row = 0; row * 16 < len; row++) {
        fprintf(stderr, "    %04X: ", row * 16);
        for (int i = 0; i < 16; i++) {
            int idx = row * 16 + i;
            if (idx < len) fprintf(stderr, "%02X ", data[idx]);
            else           fprintf(stderr, "   ");
        }
        fprintf(stderr, "\n");
    }
}

// ============================================================================
// 串口操作
// ============================================================================

static int g_fd = -1;

static int openSerial(const char *device, speed_t baud) {
    g_fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (g_fd < 0) {
        fprintf(stderr, "[ERROR] open %s 失败: %s\n", device, strerror(errno));
        return -1;
    }

    struct termios tty;
    if (tcgetattr(g_fd, &tty) != 0) {
        fprintf(stderr, "[ERROR] tcgetattr 失败: %s\n", strerror(errno));
        close(g_fd); g_fd = -1;
        return -1;
    }

    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    // 8N1, 无流控
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    // raw mode
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;

    // 非阻塞读: VMIN=0, VTIME=1 (0.1s)
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(g_fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "[ERROR] tcsetattr 失败: %s\n", strerror(errno));
        close(g_fd); g_fd = -1;
        return -1;
    }

    tcflush(g_fd, TCIOFLUSH);
    fprintf(stdout, "[INFO] 串口已打开 %s\n", device);
    return 0;
}

static void closeSerial() {
    if (g_fd >= 0) {
        tcflush(g_fd, TCIOFLUSH);
        close(g_fd);
        g_fd = -1;
    }
}

/** 带超时的读 (毫秒) */
static int readWithTimeout(uint8_t *buf, int len, int timeoutMs) {
    int total = 0;
    int poll = timeoutMs / 10;
    if (poll < 1) poll = 1;

    while (poll-- > 0 && total < len) {
        int r = (int)read(g_fd, buf + total, len - total);
        if (r > 0) {
            total += r;
        } else if (r < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            return -1;
        }
        if (total < len) usleep(10 * 1000);
    }
    return total;
}

// ============================================================================
// 相机板在线检测
// ============================================================================

/**
 * \brief 检测单个相机板是否在线
 * \return 1=在线  0=离线
 */
static int checkCameraBoard(uint8_t unitAddr, int verbose, int *versionMajor, int *versionMinor) {
    static uint8_t seq = 0;

    uint8_t packet[FRAME_LEN];
    buildPacket(packet, CMD_UNIT_CAMERA_VERSION, MODULE_UNIT, 0, unitAddr,
                0, 0, 0, CAMERA_COLOR, 0, seq++);
    if (seq > 250) seq = 0;

    if (verbose) {
        fprintf(stderr, "\n[V] >>> 发送版本查询 (板 %d)\n", unitAddr);
        hexDump("TX", packet, FRAME_LEN);
    }

    // 发命令前清空接收缓冲 (原 com1Write L317 对版本查询命令也这么干)
    tcflush(g_fd, TCIFLUSH);

    // 发 3 次 (原 checkMasterCameraBoard 用 nCount=3)
    for (int i = 0; i < 3; i++) {
        int w = (int)write(g_fd, packet, FRAME_LEN);
        if (w != FRAME_LEN) {
            fprintf(stderr, "[ERROR] write 失败: %s\n", strerror(errno));
            return 0;
        }
    }

    // 等 50ms 让 FPGA 回包 (原逻辑 msleep(50))
    usleep(50 * 1000);

    // 读 18 字节回包
    uint8_t reply[REPLY_LEN];
    memset(reply, 0, REPLY_LEN);
    int ret = readWithTimeout(reply, REPLY_LEN, 500);

    if (verbose) {
        fprintf(stderr, "[V] <<< 收到 %d 字节 (期望 %d)\n", ret, REPLY_LEN);
        if (ret > 0) hexDump("RX", reply, ret);
    }

    if (ret == REPLY_LEN) {
        *versionMajor = reply[10] & 0x0F;
        *versionMinor = reply[11];
        return 1;
    }

    *versionMajor = -1;
    *versionMinor = -1;
    return 0;
}

/** 检测中控板是否在线 (可选, 用 CMD_INT_VERSION) */
static int checkInterfaceBoard(uint8_t intAddr, int verbose) {
    static uint8_t seq = 0;

    uint8_t packet[FRAME_LEN];
    buildPacket(packet, CMD_INT_VERSION, MODULE_INT, intAddr, 0,
                0, 0, 0, 0, 0, seq++);
    if (seq > 250) seq = 0;

    if (verbose) {
        fprintf(stderr, "\n[V] >>> 发送中控板版本查询 (中控板 %d)\n", intAddr);
        hexDump("TX", packet, FRAME_LEN);
    }

    tcflush(g_fd, TCIFLUSH);
    for (int i = 0; i < 3; i++) {
        write(g_fd, packet, FRAME_LEN);
    }
    usleep(50 * 1000);

    uint8_t reply[REPLY_LEN];
    memset(reply, 0, REPLY_LEN);
    int ret = readWithTimeout(reply, REPLY_LEN, 500);

    if (verbose) {
        fprintf(stderr, "[V] <<< 收到 %d 字节\n", ret);
        if (ret > 0) hexDump("RX", reply, ret);
    }

    return (ret == REPLY_LEN) ? 1 : 0;
}

// ============================================================================
// main
// ============================================================================

static void usage(const char *prog) {
    fprintf(stderr,
        "用法:\n"
        "  %s [选项] [设备] [波特率] [板地址]\n\n"
        "参数:\n"
        "  设备      串口设备路径 (默认 /dev/ttyS4)\n"
        "  波特率    115200/230400/460800/921600 (默认 115200)\n"
        "  板地址    相机板地址 0~7 (默认 0)\n\n"
        "选项:\n"
        "  -scan     扫描 0~7 全部相机板 + 中控板\n"
        "  -v        详细日志 (打印 TX/RX hex)\n"
        "  -h        显示帮助\n\n"
        "示例:\n"
        "  %s                                      # 检测板 0\n"
        "  %s /dev/ttyS4 115200 3                  # 检测板 3\n"
        "  %s -scan /dev/ttyS4 115200              # 扫描全部\n"
        "  %s -v /dev/ttyS4 115200 0               # verbose 检测板 0\n",
        prog, prog, prog, prog, prog);
}

int main(int argc, char **argv) {
    const char *device = "/dev/ttyS4";
    int baudVal = 115200;
    int unitAddr = 0;
    int verbose = 0;
    int scanMode = 0;

    // ── 解析参数 ─────────────────────────────────────────────────
    const char *pos[3] = {nullptr, nullptr, nullptr};
    int posCnt = 0;

    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-v") == 0)    { verbose = 1; }
        else if (strcmp(argv[i], "-scan") == 0) { scanMode = 1; }
        else if (strcmp(argv[i], "-h") == 0)    { usage(argv[0]); return 0; }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "[ERROR] 未知选项: %s\n", argv[i]);
            usage(argv[0]); return 1;
        }
        else if (posCnt < 3) { pos[posCnt++] = argv[i]; }
    }

    if (posCnt >= 1) device  = pos[0];
    if (posCnt >= 2) baudVal = atoi(pos[1]);
    if (posCnt >= 3) unitAddr = atoi(pos[2]);

    // ── 波特率映射 ───────────────────────────────────────────────
    speed_t baud;
    switch (baudVal) {
        case 230400: baud = B230400; break;
        case 460800: baud = B460800; break;
        case 921600: baud = B921600; break;
        default:     baud = B115200; break;
    }

    // ── 打印标题 ─────────────────────────────────────────────────
    fprintf(stdout,
        "╔══════════════════════════════════════════════════════════╗\n"
        "║   DuySorter — 相机板在线检测测试程序                     ║\n"
        "╠══════════════════════════════════════════════════════════╣\n"
        "║  设备:   %-16s                       ║\n"
        "║  波特率: %-6d                                   ║\n"
        "║  模式:   %-16s                       ║\n"
        "╚══════════════════════════════════════════════════════════╝\n",
        device, baudVal, scanMode ? "扫描 0~7" : "单板检测");

    // ── 打开串口 ─────────────────────────────────────────────────
    if (openSerial(device, baud) != 0) {
        fprintf(stderr,
            "[ERROR] 串口打开失败, 请检查:\n"
            "  1. 设备路径 (ls -lah %s)\n"
            "  2. 权限 (sudo chmod 666 %s)\n"
            "  3. 是否有其他进程占用 (fuser %s)\n"
            "  4. 主程序是否在运行 (killall DuySorter)\n",
            device, device, device);
        return 1;
    }

    int onlineCount = 0;

    if (scanMode) {
        // ── 扫描模式: 先查中控板, 再扫 0~7 相机板 ────────────────
        fprintf(stdout, "\n─── 中控板检测 ───\n");
        if (checkInterfaceBoard(0, verbose)) {
            fprintf(stdout, "  ✅ 中控板 0 在线\n");
        } else {
            fprintf(stdout, "  ❌ 中控板 0 离线\n");
        }

        fprintf(stdout, "\n─── 相机板扫描 (0~7) ───\n");
        for (int a = 0; a < 8; a++) {
            int vm = -1, vn = -1;
            if (checkCameraBoard((uint8_t)a, verbose, &vm, &vn)) {
                fprintf(stdout, "  ✅ 相机板 %d 在线   版本 %d.%d\n", a, vn, vm);
                onlineCount++;
            } else {
                fprintf(stdout, "  ❌ 相机板 %d 离线\n", a);
            }
        }
    } else {
        // ── 单板检测 ─────────────────────────────────────────────
        fprintf(stdout, "\n─── 检测相机板 %d ───\n", unitAddr);
        int vm = -1, vn = -1;
        if (checkCameraBoard((uint8_t)unitAddr, verbose, &vm, &vn)) {
            fprintf(stdout, "  ✅ 相机板 %d 在线   版本 %d.%d\n", unitAddr, vn, vm);
            onlineCount++;
        } else {
            fprintf(stdout, "  ❌ 相机板 %d 离线 (未收到 %d 字节回包)\n", unitAddr, REPLY_LEN);
        }
    }

    closeSerial();

    fprintf(stdout, "\n════════════════════════════════════════\n");
    if (onlineCount > 0) {
        fprintf(stdout, "✅ 结果: %d 块板在线, 相机板工作正常\n", onlineCount);
    } else {
        fprintf(stdout, "❌ 结果: 未检测到任何在线相机板\n");
        fprintf(stdout, "\n排查建议:\n");
        fprintf(stdout, "  1. 确认主程序已退出: killall DuySorter\n");
        fprintf(stdout, "  2. 确认串口设备正确: ls -lah /dev/ttyS4\n");
        fprintf(stdout, "  3. 确认 RS-485 接线 (TX/RX/A+/B-) 正确\n");
        fprintf(stdout, "  4. 确认相机板供电正常\n");
        fprintf(stdout, "  5. 用 -v 查看 TX/RX 原始数据, 核对帧格式\n");
    }
    fprintf(stdout, "════════════════════════════════════════\n");

    return onlineCount > 0 ? 0 : 1;
}
