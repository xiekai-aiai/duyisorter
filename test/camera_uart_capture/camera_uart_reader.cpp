/*!
 * \file        camera_uart_reader.cpp
 * \brief       CameraUartReader 类实现 (纯库, 不含 main)
 *
 *  独立测试入口见 camera_uart_capture_test.cpp
 *
 *  \author  DuySorter Project
 *  \date    2026.09.03
 */

#include "camera_uart_reader.h"

#include <cstddef>
#include <chrono>
#include <thread>

// ============================================================================
// 构造 / 析构
// ============================================================================

CameraUartReader::CameraUartReader()
    : m_fd(-1)
    , m_baud(B115200)
    , m_level(0)
    , m_udpFd(-1)
    , m_seq(0)
    , m_verbose(false)
    , m_dump(false)
{
    memset(m_device, 0, sizeof(m_device));
    m_dumpFile[0] = '\0';
}

CameraUartReader::~CameraUartReader() {
    close();
}


// ============================================================================
// 串口配置
// ============================================================================

void CameraUartReader::setSerialPort(const char *device, speed_t baud) {
    strncpy(m_device, device, sizeof(m_device) - 1);
    m_baud = baud;
}

int CameraUartReader::open() {
    if (m_fd >= 0) return 0;

    if (m_device[0] == '\0') {
        fprintf(stderr, "[CameraUartReader] setSerialPort() 未指定设备\n");
        return ERR_SERIAL_OPEN;
    }

    // O_RDWR: 读写, O_NOCTTY: 非控制终端, O_NONBLOCK: 非阻塞
    m_fd = ::open(m_device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (m_fd < 0) {
        fprintf(stderr, "[CameraUartReader] open %s 失败: %s\n", m_device, strerror(errno));
        return ERR_SERIAL_OPEN;
    }

    if (setTermios() != 0) {
        ::close(m_fd);
        m_fd = -1;
        return ERR_SERIAL_OPEN;
    }

    tcflush(m_fd, TCIFLUSH);
    fprintf(stdout, "[CameraUartReader] 串口已打开 %s @ %d bps\n", m_device, (int)m_baud);
    return ERR_SUCCESS;
}

void CameraUartReader::close() {
    if (m_fd >= 0) {
        tcflush(m_fd, TCIOFLUSH);
        ::close(m_fd);
        m_fd = -1;
        fprintf(stdout, "[CameraUartReader] 串口已关闭\n");
    }
}

int CameraUartReader::setTermios() {
    struct termios tty;
    if (tcgetattr(m_fd, &tty) != 0) {
        fprintf(stderr, "tcgetattr 失败: %s\n", strerror(errno));
        return -1;
    }

    cfsetispeed(&tty, m_baud);
    cfsetospeed(&tty, m_baud);

    // 8N1
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    // cbreak 模式: 原始数据
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ISIG;
    tty.c_lflag &= ~IEXTEN;

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                     INLCR  | IGNCR  | ICRNL  | IXON);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    // 非阻塞读
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(m_fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "tcsetattr 失败: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


// ============================================================================
// 工具函数
// ============================================================================

void CameraUartReader::msleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint16_t CameraUartReader::crcFpga(const char *data, int len) {
    uint16_t crc = 0;
    for (int i = 0; i < len; i++) {
        uint8_t byte = (uint8_t)data[i];
        for (uint8_t bit = 0x80; bit != 0; bit >>= 1) {
            if ((crc & 0x8000) != 0) {
                crc <<= 1;
                crc ^= 0x1021;
            } else {
                crc <<= 1;
            }
            if ((byte & bit) != 0) {
                crc ^= 0x1021;
            }
        }
    }
    return crc;
}

int CameraUartReader::checkFrame(const unsigned char *data, int dataLen) {
    if (data == nullptr || dataLen < FRAME_HEADER_LEN) return ERR_INPUT_ARG;
    if (data[0] == 0xAA && data[1] == 0xAA && data[2] == 0xAA && data[3] == 0xAA) {
        return ERR_SUCCESS;
    }
    return ERR_DATA;
}


// ============================================================================
// 命令帧构造 & 发送
// ============================================================================

void CameraUartReader::buildPacket(char packet[16],
                                   int nCmd, char sModuleType, char sIntAddr, char sUnitAddr,
                                   char arg1, char arg2, char arg3, char arg4, char arg5,
                                   int seq) {
    memset(packet, 0, 16);

    packet[0] = (char)0xA5;
    packet[1] = (char)0x5A;

    int nUnit = (uint8_t)sUnitAddr & 0x3F;
    if (nUnit < MAX_UNIT_PER_INT) {
        packet[2] = (char)((sIntAddr * 16) | sModuleType);
        packet[3] = sUnitAddr;
    } else {
        packet[2] = (char)(((sIntAddr + 1) * 16) | sModuleType);
        packet[3] = (char)(sUnitAddr - MAX_UNIT_PER_INT);
    }

    packet[4] = (char)(nCmd / 256);
    packet[5] = (char)(nCmd % 256);
    packet[6] = (char)(seq & 0xFF);
    packet[7]  = arg1;
    packet[8]  = arg2;
    packet[9]  = arg3;
    packet[10] = arg4;
    packet[11] = arg5;
    packet[14] = (char)0xFF;
    packet[15] = (char)0xFF;

    // CRC
    char crcData[10];
    for (int i = 0; i < 10; i++) crcData[i] = packet[i + 2];

    uint16_t crc;
    if (sUnitAddr == 0x3F) {
        uint32_t sum = 0;
        for (int i = 0; i < 10; i++) sum += (uint8_t)crcData[i];
        crc = (uint16_t)(sum & 0xFFFF);
    } else {
        crc = crcFpga(crcData, 10);
    }

    packet[12] = (char)(crc / 256);
    packet[13] = (char)(crc % 256);
}

int CameraUartReader::sendCmd(int nCmd, char sModuleType, char sIntAddr, char sUnitAddr,
                              char arg1, char arg2, char arg3, char arg4, char arg5,
                              int nCount) {
    if (m_fd < 0) {
        fprintf(stderr, "[CameraUartReader] 串口未打开\n");
        return ERR_SERIAL_OPEN;
    }

    // 注意: 这里不做 tcflush。发送命令前是否清空输入缓冲由调用方决定,
    // 否则会误清掉上一个命令(如 0x03 图像获取)已经触发的回传数据。

    char packet[16];
    int seq = m_seq;
    if (seq > 250) seq = 0;
    buildPacket(packet, nCmd, sModuleType, sIntAddr, sUnitAddr,
                arg1, arg2, arg3, arg4, arg5, seq);
    m_seq = seq + 1;

    // 发送 nCount 次
    for (int i = 0; i < nCount; i++) {
        int written = (int)::write(m_fd, packet, 16);
        if (written != 16) {
            fprintf(stderr, "[CameraUartReader] write 失败: %s\n", strerror(errno));
            return ERR_SERIAL_WRITE;
        }
    }

    if (m_verbose) {
        fprintf(stderr, "[V] >> sendCmd %s (seq=%d, count=%d)\n",
                cmdName(nCmd), m_seq - 1, nCount);
        hexDump("TX", packet, 16);
    }
    return ERR_SUCCESS;
}


// ============================================================================
// UART 读数据
// ============================================================================

int CameraUartReader::readData(unsigned char *buf, int nLen, int timeoutMs) {
    if (m_fd < 0) return ERR_SERIAL_OPEN;
    if (buf == nullptr || nLen <= 0) return ERR_INPUT_ARG;

    int total = 0;
    int pollCount = timeoutMs / 50;
    if (pollCount < 1) pollCount = 1;

    auto t0 = std::chrono::steady_clock::now();
    while (pollCount-- > 0 && total < nLen) {
        int r = (int)::read(m_fd, buf + total, nLen - total);
        if (r > 0) {
            total += r;
        } else if (r < 0 && errno != EAGAIN) {
            fprintf(stderr, "[CameraUartReader] read 错误: %s\n", strerror(errno));
            return ERR_SERIAL_READ;
        }
        if (total < nLen) msleep(50);
    }
    double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

    if (m_verbose) {
        fprintf(stderr, "[V] readData: 期望 %d, 实际 %d, 耗时 %.3fs\n", nLen, total, elapsed);
    }
    return total;
}

int CameraUartReader::dumpRaw(const char *filename, int timeoutMs) {
    if (m_fd < 0) return ERR_SERIAL_OPEN;

    FILE *fp = fopen(filename, "w");
    if (fp == nullptr) {
        fprintf(stderr, "[CameraUartReader] dump 无法打开 %s: %s\n", filename, strerror(errno));
        return -1;
    }

    unsigned char buf[4096];
    int total = 0;
    auto t0 = std::chrono::steady_clock::now();
    auto elapsedMs = [&]() {
        return (long)(std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count() * 1000);
    };

    while (elapsedMs() < timeoutMs) {
        int r = (int)::read(m_fd, buf, sizeof(buf));
        if (r > 0) {
            total += r;
            // 每次 read 收到的一批数据, 单独写一行 hex
            for (int i = 0; i < r; i++) {
                fprintf(fp, "%02X ", buf[i]);
            }
            fprintf(fp, "\n");
            if (m_verbose) {
                fprintf(stderr, "[V] dump +%d bytes (累计 %d)\n", r, total);
                if (total <= 64) hexDump("DUMP", buf, r);
            }
        } else if (r < 0 && errno != EAGAIN) {
            fprintf(stderr, "[CameraUartReader] dump read 错误: %s\n", strerror(errno));
            break;
        }
        msleep(10);
    }

    fclose(fp);
    if (m_verbose) {
        fprintf(stderr, "[V] dump 完成: %s, 共 %d bytes\n", filename, total);
    }
    return total;
}

int CameraUartReader::probeVersion(int unitAddr, int timeoutMs) {
    if (m_fd < 0) return ERR_SERIAL_OPEN;

    tcflush(m_fd, TCIFLUSH);

    fprintf(stdout, "[PROBE] 发送版本查询 0x2001 → 相机板 unit%d, 连发 3 次\n", unitAddr);
    for (int i = 0; i < 3; i++) {
        sendCmd(CMD_UNIT_VERSION, ModuleType::UNIT, m_level, unitAddr,
                0, 0, 0, CAMERA_COLOR, 0, 1);
        msleep(50);
    }

    char dumpName[160];
    snprintf(dumpName, sizeof(dumpName), "probe_version_unit%d.hex", unitAddr);
    int n = dumpRaw(dumpName, timeoutMs);
    fprintf(stdout, "[PROBE] 回包抓取完成: %s, 共 %d bytes\n", dumpName, n);

    if (n > 0) {
        fprintf(stdout, "[PROBE] ✅ 收到回包 → 物理层/波特率/地址/CRC 均正常, 问题在采图命令序列\n");
    } else {
        fprintf(stdout, "[PROBE] ❌ 无回包 → 物理层不通 (波特率/设备路径/RS485方向/地址)\n");
    }
    return n;
}


// ============================================================================
// 剥帧头
// ============================================================================

int CameraUartReader::stripFrameData(unsigned char *data, const CaptureConfig &cfg) {
    if (data == nullptr || cfg.dataDepth <= 0 || cfg.dataLen <= 0) {
        return ERR_INPUT_ARG;
    }

    // PROTOCAL_3 (前视/辅配灰度相机):
    //   每行 = 10 包头 + 2048 数据(奇偶重复, 取偶数 1024 有效) + 6 包尾
    //   strip 后前 CAM_IMAGE_WIDTH*CAM_IMAGE_HEIGHT 字节为有效灰度像素
    int dataLenPerFrame = cfg.dataLen / cfg.dataDepth;  // = WIDTH*2 + FRAME_LEN_NOT_DATA

    for (int i = 0; i < cfg.dataDepth; i++) {
        const unsigned char *row = data + i * dataLenPerFrame + FRAME_PACKET_HEAD;
        unsigned char *out = data + i * CAM_IMAGE_WIDTH;
        for (int j = 0; j < CAM_IMAGE_WIDTH; j++) {
            out[j] = row[j * 2];   // 取偶数位有效像素 (奇偶重复)
        }
    }
    return 0;
}


// ============================================================================
// DEBUG 辅助
// ============================================================================

void CameraUartReader::hexDump(const char *label, const void *data, int len) {
    const unsigned char *p = (const unsigned char *)data;
    fprintf(stderr, "  [%s] %d bytes:", label ? label : "data", len);
    // 第一行内联打印 (如果 len <= 16)
    if (len <= 16) {
        fprintf(stderr, "  ");
        for (int i = 0; i < len; i++) fprintf(stderr, "%02X ", p[i]);
        fprintf(stderr, "\n");
        return;
    }
    // 多行
    fprintf(stderr, "\n");
    for (int row = 0; row * 16 < len; row++) {
        fprintf(stderr, "    %04X: ", row * 16);
        for (int i = 0; i < 16; i++) {
            int idx = row * 16 + i;
            if (idx < len) fprintf(stderr, "%02X ", p[idx]);
            else fprintf(stderr, "   ");
        }
        fprintf(stderr, "\n");
    }
}

const char *CameraUartReader::cmdName(int nCmd) {
    switch (nCmd) {
        case CMD_INT_IMAGE_CAPTURE_MODE:  return "0x08 INT_IMAGE_CAPTURE_MODE";
        case CMD_UNIT_ONOFF:              return "0x01 RUN_START_STOP";
        case CMD_UNIT_IMG_BURST:          return "0x36 IMG_TRIGGER_COND";
        case CMD_UNIT_IMAGE:              return "0x03 IMAGE_GET";
        case CMD_UNIT_MAIZE_IMAGE_UPLOAD: return "0x45 IMAGE_UPLOAD_START";
        case CMD_UNIT_VERSION:            return "0x2001 VERSION_QUERY";
        default: {
            static char buf[64];
            snprintf(buf, sizeof(buf), "0x%02X (unknown)", nCmd);
            return buf;
        }
    }
}


// ============================================================================
// 核心 capture 流程
// ============================================================================

int CameraUartReader::capture(CaptureConfig *cfg, unsigned char *&outBuf, int timeoutMs) {
    if (cfg == nullptr) return ERR_INPUT_ARG;
    if (cfg->dataLen == 0) { cfg->result = ERR_CONFIG; return ERR_CONFIG; }
    if (m_fd < 0)         { cfg->result = ERR_SERIAL_OPEN; return ERR_SERIAL_OPEN; }

    int dataLenPerFrame = cfg->dataLen / cfg->dataDepth;
    outBuf = nullptr;

    if (m_verbose) {
        fprintf(stderr, "\n[V] ===== capture 开始 =====\n");
        fprintf(stderr, "[V] unitAddr=%d, depth=%d, dataLen=%d (每帧 %d bytes)\n",
                cfg->nUnitAddr, cfg->dataDepth, cfg->dataLen, dataLenPerFrame);
    }

    // 分配缓冲区
    unsigned char *data = new (std::nothrow) unsigned char[cfg->dataLen];
    if (data == nullptr) { cfg->result = ERR_MALLOC_FAILED; return ERR_MALLOC_FAILED; }
    memset(data, 0, cfg->dataLen);

    // 发送命令序列前清空输入缓冲 (清掉上一轮残留)
    tcflush(m_fd, TCIFLUSH);

    // ── 0. 运行开始 (CMD 0x01) ────────────────────────────────────────
    //     doc: Byte11 = 1(运行开始), 相机板上电默认停止, 必须先启动才能响应采集
    if (m_verbose) fprintf(stderr, "[V] ── 步骤0: sendCmd ONOFF (0x01) 运行开始 ──\n");
    sendCmd(CMD_UNIT_ONOFF, ModuleType::UNIT, m_level, cfg->nUnitAddr,
            0, 0, 0, 0, 1, 1);
    msleep(50);

    // ── 0.5 接口板: 图像获取方式 (CMD 0x08) ──────────────────────────
    //     原项目 UART 抓图 (aianalysis.cpp getImageFromUART_*) 固定向 INT
    //     发全 0 参数、unitAddr=0; depth/transType 由下面的 0x03 命令携带
    if (m_verbose) fprintf(stderr, "[V] ── 步骤0.5: sendCmd INT_IMAGE_CAPTURE_MODE (0x08) ──\n");
    sendCmd(CMD_INT_IMAGE_CAPTURE_MODE, ModuleType::INT, m_level, 0,
            0, 0, 0, 0, 0, 3);
    msleep(20);

    // ── 1. 图像拍摄触发条件 (CMD 0x36) ────────────────────────────────
    //     doc: Byte10/11 = 触发像元个数 (高/低), 0 = 手动命令触发 (对齐原项目默认 nIntelSampBurst=0)
    if (m_verbose) fprintf(stderr, "[V] ── 步骤1: sendCmd IMG_BURST (0x36) 触发像元个数=0 ──\n");
    sendCmd(CMD_UNIT_IMG_BURST, ModuleType::UNIT, m_level, cfg->nUnitAddr,
            0, 0, 0, 0x00, 0x00, 1);
    msleep(10);

    // ── 2. 图像获取 (CMD 0x03) ─────────────────────────────────────────
    //     doc: Byte9 = 0:RS485上传/1:USB上传, Byte10/11 = 获取行数 (高/低)
    if (m_verbose) fprintf(stderr, "[V] ── 步骤2: sendCmd TRIGGER_IMAGE (0x03) ──\n");
    sendCmd(CMD_UNIT_IMAGE, ModuleType::UNIT, m_level, cfg->nUnitAddr,
            0, 0,
            (char)cfg->nTransType,
            (char)((cfg->dataDepth - 1) / 256),
            (char)((cfg->dataDepth - 1) % 256),
            1);
    if (m_verbose) fprintf(stderr, "[V] 等待 FPGA 缓存图像... msleep(100)\n");
    msleep(100);

    // ── transType != UART 时: FPGA 推图到 AI 板/USB, 上位机串口收不到数据 ──
    //     原项目 transType=1 时由 AI 板处理 (processImageDataFromCameraByAIEN),
    //     上位机只需发命令、让 FPGA 推完即可, 不必等 UART 回数据。
    if (cfg->nTransType != TRANS_UART) {
        int waitMs = 3000;
        if (m_verbose) fprintf(stderr, "[V] transType=%d (非 UART), FPGA 推图给 AI 板, 等 %dms 完成...\n",
                                cfg->nTransType, waitMs);
        msleep(waitMs);
        delete[] data;
        outBuf = nullptr;
        cfg->result = ERR_SUCCESS;
        fprintf(stdout, "[CameraUartReader] ✅ 命令发送完成 (transType=%d, 图像推给 AI 板)\n", cfg->nTransType);
        return ERR_SUCCESS;
    }

    // 注: UART 抓图路径无需发 0x45 (玉米机型图像上传开始)。
    //     原项目 getImageFromUART_* 发完 0x03 后直接 com1Read 收数据;
    //     0x45 是 USB 路径用的 (后面接 usb_bulk_read), 发到串口反而不会推流。

    // ── 5.5 原始数据 dump (诊断模式: 只 dump, 不读帧) ──
    if (m_dump) {
        char autoName[128];
        snprintf(autoName, sizeof(autoName), "uart_dump_unit%d_seq%d.hex", cfg->nUnitAddr, cfg->nSeq);
        const char *dumpFile = (m_dumpFile[0] != '\0') ? m_dumpFile : autoName;
        fprintf(stderr, "[DUMP] 触发上传后抓取串口原始数据 3000ms -> %s\n", dumpFile);
        int n = dumpRaw(dumpFile, 3000);
        fprintf(stdout, "[DUMP] %s 共 %d bytes\n", dumpFile, n);
        delete[] data;
        outBuf = nullptr;
        cfg->result = (n > 0) ? ERR_SUCCESS : ERR_TIMEOUT;
        return (n > 0) ? ERR_SUCCESS : ERR_TIMEOUT;
    }

    // ── 6. 逐行读 + 验帧头 ────────────────────────────────────────────
    if (m_verbose) fprintf(stderr, "[V] ── 步骤6: 逐行读 %d 行, 每行 %d bytes ──\n",
                           cfg->dataDepth, dataLenPerFrame);

    for (int i = 0; i < cfg->dataDepth; i++) {
        unsigned char *pData = data + i * dataLenPerFrame;
        int count = readData(pData, dataLenPerFrame, timeoutMs);

        if (count != dataLenPerFrame) {
            fprintf(stderr, "[CameraUartReader] 行 %d 超时: 期望 %d, 实际 %d\n",
                    i, dataLenPerFrame, count);
            if (m_verbose && count > 0) hexDump("RX", pData, count > 16 ? 16 : count);
            cfg->result = ERR_TIMEOUT; delete[] data; return ERR_TIMEOUT;
        }
        if (checkFrame(pData, dataLenPerFrame) != ERR_SUCCESS) {
            fprintf(stderr, "[CameraUartReader] 行 %d 帧头错误 (期望 0xAA AA AA AA)\n", i);
            if (m_verbose) hexDump("RX 行头16B", pData, 16);
            cfg->result = ERR_DATA; delete[] data; return ERR_DATA;
        }

        if (m_verbose && (i == 0 || i == cfg->dataDepth - 1 || (i + 1) % 100 == 0)) {
            fprintf(stderr, "[V] 行 %d/%d OK (0xAA AA AA AA)\n", i + 1, cfg->dataDepth);
        }
    }

    // ── 7. 剥帧头 ────────────────────────────────────────────────────
    if (m_verbose) fprintf(stderr, "[V] ── 步骤7: stripFrameData 剥帧头 ──\n");
    if (stripFrameData(data, *cfg) != 0) {
        cfg->result = ERR_DATA; delete[] data; return ERR_DATA;
    }

    outBuf = data;
    cfg->result = ERR_SUCCESS;

    fprintf(stdout, "[CameraUartReader] ✅ 成功: 板=%d, 行=%d, 原始=%d bytes, 有效像素=%d bytes\n",
            cfg->nUnitAddr, cfg->dataDepth, cfg->dataLen,
            CAM_IMAGE_WIDTH * CAM_IMAGE_HEIGHT);
    if (m_verbose) fprintf(stderr, "[V] ===== capture 完成 =====\n");
    return ERR_SUCCESS;
}
