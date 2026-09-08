/*!
 * \file        camera_uart_capture_test.cpp
 * \brief       独立测试入口 —— 在 RK3568 目标机上直接运行
 *
 *  用法:
 *    camera_uart_capture_test                       # 默认 /dev/ttyUSB0 @ 115200, 板 0, 5 帧
 *    camera_uart_capture_test /dev/ttyUSB1          # 指定串口
 *    camera_uart_capture_test /dev/ttyUSB0 460800   # 指定波特率
 *    camera_uart_capture_test /dev/ttyUSB0 115200 3 # 指定板号
 *    camera_uart_capture_test /dev/ttyUSB0 115200 0 10  # 指定帧数
 *
 *  Ctrl-C 优雅退出, 每采完一次等 3 秒自动继续
 *
 *  \author  DuySorter Project
 *  \date    2026.09.03
 */

#include "camera_uart_reader.h"
#include <signal.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

static volatile bool g_running = true;

static void sig_handler(int) {
    g_running = false;
    fprintf(stdout, "\n\n[相机采集测试] Ctrl-C 收到, 退出中...\n");
}

static void print_usage(const char *prog) {
    fprintf(stdout,
        "用法: %s [device] [baud] [unitAddr] [depth] [level]\n"
        "\n"
        "参数:\n"
        "  device   串口设备路径       (默认 /dev/ttyUSB0)\n"
        "  baud     波特率             (默认 115200)\n"
        "  unitAddr 相机板地址 0~7     (默认 0)\n"
        "  depth    采集张数           (默认 1)\n"
        "  level    层数/供料分组 0~2  (默认 0)\n"
        "\n"
        "示例:\n"
        "  %s                                   # 全部默认\n"
        "  %s /dev/ttyS4 921600 1 1 0           # 板1, 采1张图\n"
        "  %s /dev/ttyS4 921600 3 10            # 高速串口, 板3, 10张\n"
        "\n"
        "运行时按 Ctrl-C 退出, 每采完一次等 3 秒自动继续\n"
        , prog, prog, prog, prog);
}


// ============================================================================
// 保存 BMP 工具函数 (纯 C, 无依赖)
// ============================================================================

/*!
 * \brief 把灰度像素数据写成 8bit 灰度 BMP (无压缩, 带颜色表)
 * \param filename   输出文件名
 * \param grayData   灰度像素数据 (WIDTH*HEIGHT 字节)
 * \param width      图像宽
 * \param height     图像高 (正=从上到下)
 * \return 0=成功, 非零=失败
 */
int save_gray_bmp(const char *filename, const unsigned char *grayData,
                  int width, int height) {
    if (filename == nullptr || grayData == nullptr || width <= 0 || height <= 0) {
        return -1;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "save_gray_bmp: 无法打开 %s\n", filename);
        return -2;
    }

    int rowSize  = width;
    int rowPad   = (4 - (rowSize % 4)) % 4;         // BMP 行必须 4 字节对齐
    int pixels   = height * width;
    int paletteSize = 256 * 4;                        // 灰度 256 级 × RGBA

    int fileSize = 14                                 // BMP 文件头
                 + 40                                // BITMAPINFOHEADER
                 + paletteSize
                 + (rowSize + rowPad) * height;

    // ── 1. BMP 文件头 (14 bytes) ────────────────────────────────────
    unsigned char header[14] = {};
    header[0] = 'B'; header[1] = 'M';
    *(int*)(header + 2)  = fileSize;
    *(short*)(header + 10) = 54;  // 偏移到像素区 = 14 + 40

    fwrite(header, 1, 14, fp);

    // ── 2. BITMAPINFOHEADER (40 bytes) ─────────────────────────────
    unsigned char info[40] = {};
    *(int*)(info + 0)  = 40;        // header size
    *(int*)(info + 4)  = width;
    *(int*)(info + 8)  = height;    // 正数 = 从上到下
    *(short*)(info + 12) = 1;       // color planes
    *(short*)(info + 14) = 8;       // bpp = 8 (灰度)
    *(int*)(info + 20)  = 0;        // compression = BI_RGB
    *(int*)(info + 24)  = pixels + rowPad * height;  // image size
    *(int*)(info + 28)  = 2835;     // X resolution (dpi)
    *(int*)(info + 32)  = 2835;     // Y resolution
    *(int*)(info + 36)  = 256;      // colors in table
    *(int*)(info + 40)  = 0;        // important colors

    fwrite(info, 1, 40, fp);

    // ── 3. 灰度颜色表 (256 × 4 bytes) ───────────────────────────────
    unsigned char palette[256 * 4];
    for (int i = 0; i < 256; i++) {
        palette[i * 4 + 0] = (unsigned char)i;  // B
        palette[i * 4 + 1] = (unsigned char)i;  // G
        palette[i * 4 + 2] = (unsigned char)i;  // R
        palette[i * 4 + 3] = 0;                 // A
    }
    fwrite(palette, 1, paletteSize, fp);

    // ── 4. 像素数据 (从上到下, 每行 4 字节对齐) ──────────────────────
    unsigned char pad[3] = {0, 0, 0};
    for (int y = 0; y < height; y++) {
        fwrite(grayData + y * width, 1, width, fp);
        fwrite(pad, 1, rowPad, fp);
    }

    fclose(fp);
    return 0;
}


// ============================================================================
// main
// ============================================================================

int main(int argc, char **argv) {
    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    // ── 参数解析 (支持 -v/--verbose / -d/--dump / -o/--dump-file <path>) ──
    const char *device  = "/dev/ttyUSB0";
    speed_t     baud    = B115200;
    int         unit    = 0;
    int         depth   = 1;
    int         level   = 0;
    bool        verbose = false;
    bool        dumpMode = false;
    bool        probeMode = false;
    const char *dumpFilePath = nullptr;

    int posArgc = 0;
    char *posArgv[10] = {};
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dump") == 0) {
            dumpMode = true;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--probe") == 0) {
            probeMode = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--dump-file") == 0) {
            if (i + 1 < argc) {
                dumpFilePath = argv[++i];
                dumpMode = true;
            } else {
                fprintf(stderr, "⚠️  缺少 dump 文件路径\n");
                return 1;
            }
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "⚠️  未知选项: %s\n", argv[i]);
        } else {
            if (posArgc < 10) posArgv[posArgc++] = argv[i];
        }
    }

    if (posArgc >= 1) device = posArgv[0];
    if (posArgc >= 2) {
        int b = atoi(posArgv[1]);
        switch (b) {
            case 115200:  baud = B115200;  break;
            case 230400:  baud = B230400;  break;
            case 460800:  baud = B460800;  break;
            case 921600:  baud = B921600;  break;
            default:      baud = B115200;  break;
        }
    }
    if (posArgc >= 3) unit  = atoi(posArgv[2]);
    if (posArgc >= 4) depth = atoi(posArgv[3]);
    if (posArgc >= 5) level = atoi(posArgv[4]);

    // ── 打印启动信息 ────────────────────────────────────────────────
    fprintf(stdout,
        "╔══════════════════════════════════════════════════════════╗\n"
        "║     DuySorter — 相机 UART 图像采集独立测试程序             ║\n"
        "╠══════════════════════════════════════════════════════════╣\n"
        "║  设备:      %-46s ║\n"
        "║  波特率:    %-46d ║\n"
        "║  相机板:    %-46d ║\n"
        "║  采集张数:  %-46d ║\n"
        "║  层数:      %-46d ║\n"
        "║  分辨率:    %dx%d                                      ║\n"
        "║  Ctrl-C 退出, 每采完一次等 3 秒自动继续                     ║\n"
        "╚══════════════════════════════════════════════════════════╝\n\n",
        device, (int)baud, unit, depth, level,
        CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT);

    // ── 打开串口 ────────────────────────────────────────────────────
    CameraUartReader reader;
    reader.setSerialPort(device, baud);
    reader.setLevel(level);
    reader.setVerbose(verbose);
    reader.setDump(dumpMode);
    if (dumpFilePath != nullptr) {
        reader.setDumpFile(dumpFilePath);
    }

    if (reader.open() != ERR_SUCCESS) {
        fprintf(stderr, "❌ 串口打开失败, 请检查:\n");
        fprintf(stderr, "     1. 设备路径是否正确 (ls %s)\n", device);
        fprintf(stderr, "     2. 权限 (sudo chmod 666 %s)\n", device);
        fprintf(stderr, "     3. 没有其他进程占用 (fuser %s)\n", device);
        return 1;
    }

    if (verbose) {
        fprintf(stderr, "\n[V] === VERBOSE 模式开 ===\n");
    }

    // ── probe 诊断模式: 发版本查询命令, 验证物理层链路 ──────────────
    if (probeMode) {
        fprintf(stdout, "\n=== PROBE 模式: 通信握手诊断 ===\n");
        reader.setVerbose(verbose);
        int n = reader.probeVersion(unit, 2000);
        reader.close();
        return (n > 0) ? 0 : 1;
    }

    // ── 循环采集 ────────────────────────────────────────────────────
    int captureCount = 0;
    int rowsPerImage     = CAM_IMAGE_HEIGHT;                           // 500 行
    int rawBytesPerLine  = CAM_IMAGE_WIDTH * 2 + FRAME_LEN_NOT_DATA;   // 2048 + 16
    int rawBytesPerImage = rowsPerImage * rawBytesPerLine;             // 1032000

    while (g_running) {
        captureCount++;
        fprintf(stdout, "\n─── 采集 #%d (共 %d 张) ───\n", captureCount, depth);

        for (int f = 0; f < depth && g_running; f++) {
            // 构造配置: 每张图 = 500 行 (PROTOCAL_3)
            CaptureConfig cfg = {};
            cfg.nSeq        = captureCount - 1;
            cfg.nUnitAddr   = unit;
            cfg.dataDepth   = rowsPerImage;      // 协议数据包行数
            cfg.dataLen     = rawBytesPerImage;  // 原始数据总字节
            cfg.nTransType  = TRANS_USB;   // transType=1, FPGA 推图到 AI 板 (192.168.4.6)
            cfg.result      = ERR_SUCCESS;

            // 调用 capture
            unsigned char *buf = nullptr;
            auto t0 = std::chrono::steady_clock::now();
            int ret = reader.capture(&cfg, buf, /*timeoutMs=*/ 5000);
            auto t1 = std::chrono::steady_clock::now();
            double sec = std::chrono::duration<double>(t1 - t0).count();

            if (dumpMode) {
                // dump 模式: capture 内部已把原始数据写入 uart_dump_*.bin, 不保存 BMP
                fprintf(stdout, "  [dump 完成, 结果见 uart_dump_unit%d_seq*.hex]\n", unit);
            } else if (ret == ERR_SUCCESS && buf != nullptr) {
                fprintf(stdout, "  耗时: %.2f 秒\n", sec);

                char bmpName[256];
                snprintf(bmpName, sizeof(bmpName),
                         "capture_%03d_unit%d_f%d.bmp",
                         captureCount, unit, f);

                int rc = save_gray_bmp(bmpName, buf,
                                       CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT);
                if (rc == 0) {
                    fprintf(stdout, "  💾  %s  ✅ (1024x500 灰度)\n", bmpName);
                } else {
                    fprintf(stderr, "  ⚠️   %s 保存失败\n", bmpName);
                }
                delete[] buf;
            } else if (ret == ERR_SUCCESS && cfg.nTransType != TRANS_UART) {
                // transType != UART 时: FPGA 推图到 AI 板, 上位机不接收 UART 数据
                fprintf(stdout, "  ✅ 命令发送成功 (transType=%d, 图像推给 AI 板), 耗时 %.2f 秒\n",
                        cfg.nTransType, sec);
            } else {
                fprintf(stderr, "  ❌ 第 %d 张采集失败, ret=%d, result=%d, 耗时 %.2f 秒\n",
                        f, ret, cfg.result, sec);
            }
        }

        // 等 3 秒继续下一轮
        if (g_running) {
            fprintf(stdout, "  (3 秒后继续下一轮, Ctrl-C 可退出)\n");
            for (int i = 0; i < 30 && g_running; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    fprintf(stdout, "\n===== 总共采集 %d 次, 程序退出 =====\n", captureCount);
    reader.close();
    return 0;
}
