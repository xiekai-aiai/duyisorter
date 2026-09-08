#!/bin/bash
# =============================================================================
#  camera_uart_capture_test — 启动命令示例
# =============================================================================
#
#  拷贝到 RK3568 板卡后直接执行:
#    chmod +x test_capture.sh
#    ./test_capture.sh        # 默认方式运行
#    ./test_capture.sh 1      # 用预定义的 profile 1
#    ./test_capture.sh auto   # 自动扫描可用串口
#
#  也可以直接单独运行下面注释里的命令
# =============================================================================

BIN="$(dirname "$0")/camera_uart_capture_test"

# ── 颜色 ──────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; CYAN='\033[0;36m'; NC='\033[0m'

info()  { echo -e "${GREEN}[INFO]${NC}  $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*"; }

# =============================================================================
# 预定义 Profile
# =============================================================================

usage() {
    cat <<EOF

╔══════════════════════════════════════════════════════════════╗
║          camera_uart_capture_test — 启动命令示例               ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║  1) 最常用: 默认参数 (ttyUSB0, 115200, 板0, 5帧, 层0)        ║
║     $0                                                       ║
║     $0 default                                               ║
║                                                              ║
║  2) 高速串口: 921600, 板3, 连续10帧                          ║
║     $0 fast                                                  ║
║                                                              ║
║  3) 全部8块板循环采集                                         ║
║     $0 all                                                   ║
║                                                              ║
║  4) 单帧快照 (采一张就存 BMP 然后等 10s)                     ║
║     $0 snap                                                  ║
║                                                              ║
║  5) 指定完整参数: device baud unit depth level               ║
║     $0 custom                                                ║
║                                                              ║
║  6) 自己手动指定全部参数                                      ║
║     $0 /dev/ttyUSB0 460800 2 8 0                             ║
║                                                              ║
║  7) 列出可用串口                                              ║
║     $0 list                                                  ║
║                                                              ║
║  8) 检查是否有其他进程占用串口                                ║
║     $0 check                                                 ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝

EOF
}

# =============================================================================
# 各个 profile
# =============================================================================

run_default() {
    # /dev/ttyUSB0, 115200, unit=0, depth=5, level=0
    info "默认参数: /dev/ttyUSB0 115200 0 5 0"
    exec "$BIN" /dev/ttyUSB0 115200 0 5 0
}

run_fast() {
    # 高速串口, 板3, 10帧 (高速采集场景)
    info "高速模式: /dev/ttyUSB0 921600 3 10 0"
    info "注意: 921600bps 传输数据量更大, 确保 FPGA 和 USB-UART 都支持"
    exec "$BIN" /dev/ttyUSB0 921600 3 10 0
}

run_all() {
    # 循环采集全部 8 块板 (0~7)
    info "循环采集全部 8 块板, 每板 5 帧"
    while true; do
        for unit in 0 1 2 3 4 5 6 7; do
            echo ""
            info "────── 板 ${unit}/7 ──────"
            "$BIN" /dev/ttyUSB0 115200 $unit 5 0 || warn "板 $unit 采集失败, 跳过"
            sleep 1
        done
        info "全部 8 块板采完, 重新开始..."
    done
}

run_snap() {
    # 单帧快照: 只采 1 帧, 存成 BMP 后退出
    info "单帧快照模式: 采 1 张后退出"
    "$BIN" /dev/ttyUSB0 115200 0 1 0
    info "快照完成, 查看当前目录下 capture_*.bmp"
}

run_custom() {
    # 用户手动输入参数
    echo ""
    read -p "  串口设备 [/dev/ttyUSB0]: " DEVICE
    read -p "  波特率 [115200]:         " BAUD
    read -p "  板号 0~7 [0]:           " UNIT
    read -p "  每板帧数 [5]:            " DEPTH
    read -p "  层数 0~2 [0]:            " LEVEL
    echo ""
    DEVICE=${DEVICE:-/dev/ttyUSB0}
    BAUD=${BAUD:-115200}
    UNIT=${UNIT:-0}
    DEPTH=${DEPTH:-5}
    LEVEL=${LEVEL:-0}
    info "执行: $BIN $DEVICE $BAUD $UNIT $DEPTH $LEVEL"
    exec "$BIN" "$DEVICE" "$BAUD" "$UNIT" "$DEPTH" "$LEVEL"
}

list_ports() {
    info "扫描可用串口设备..."
    echo ""
    for d in /dev/ttyUSB* /dev/ttyACM* /dev/ttyS* /dev/ttyS[0-9]*; do
        [ -e "$d" ] || continue
        local owner=$(stat -c '%U:%G' "$d" 2>/dev/null || echo "unknown")
        local mode=$(stat -c '%a' "$d" 2>/dev/null || echo "???")
        echo "  ${CYAN}${d}${NC}  owner=${owner}  mode=${mode}"
    done
    echo ""
    info "确保当前用户对 /dev/ttyUSB* 有读写权限:"
    info "  sudo chmod 666 /dev/ttyUSB*"
    info "  或者: sudo usermod -aG dialout \$USER  (然后重新登录)"
}

check_port() {
    local port="${2:-/dev/ttyUSB0}"
    info "检查串口 $port ..."

    # 权限
    if [ -e "$port" ]; then
        info "  ✅ 设备存在"
        if [ -w "$port" ] && [ -r "$port" ]; then
            info "  ✅ 读写权限 OK"
        else
            error "  ❌ 没有读写权限!"
            info "     修复: sudo chmod 666 $port"
        fi
    else
        error "  ❌ 设备不存在: $port"
        info "     修复: 检查 USB 线 / lsusb"
        exit 1
    fi

    # 是否被占用
    local pids=$(fuser "$port" 2>/dev/null)
    if [ -n "$pids" ]; then
        warn "  ⚠️  串口被进程占用: PID=$pids"
        for pid in $pids; do
            local proc=$(ps -p $pid -o comm= 2>/dev/null)
            echo "      PID $pid: $proc"
        done
        warn "     修复: kill -9 $pids"
    else
        info "  ✅ 串口未被占用"
    fi
    echo ""
}

# =============================================================================
# 直接执行的例子 (可以直接 copy-paste 到板卡终端)
# =============================================================================
#
#  【例子 1】最常用: 默认参数
#    camera_uart_capture_test
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 5 0
#
#  【例子 2】换 USB 口 (有时相机板挂在 /dev/ttyUSB1 或 /dev/ttyACM0)
#    camera_uart_capture_test /dev/ttyUSB1 115200 0 5 0
#    camera_uart_capture_test /dev/ttyACM0 115200 0 5 0
#
#  【例子 3】换波特率 (高速场景, 传输更稳)
#    camera_uart_capture_test /dev/ttyUSB0 230400 0 5 0
#    camera_uart_capture_test /dev/ttyUSB0 460800 0 5 0
#    camera_uart_capture_test /dev/ttyUSB0 921600 0 5 0
#
#  【例子 4】指定不同的相机板
#    camera_uart_capture_test /dev/ttyUSB0 115200 1 5 0   # 板 1
#    camera_uart_capture_test /dev/ttyUSB0 115200 3 5 0   # 板 3
#    camera_uart_capture_test /dev/ttyUSB0 115200 7 5 0   # 板 7
#
#  【例子 5】不同帧数
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 1 0   # 只采 1 帧 (最快)
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 3 0   # 采 3 帧
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 10 0  # 采 10 帧
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 20 0  # 采 20 帧 (多帧堆叠)
#
#  【例子 6】不同层数 (供料分组)
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 5 0   # 层 0
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 5 1   # 层 1
#    camera_uart_capture_test /dev/ttyUSB0 115200 0 5 2   # 层 2
#
#  【例子 7】组合 (高速 + 多帧 + 指定板)
#    camera_uart_capture_test /dev/ttyUSB0 460800 2 10 0
#    camera_uart_capture_test /dev/ttyUSB0 921600 5 20 1
#
#  【例子 8】后台运行 + 日志 (不阻塞终端)
#    ./camera_uart_capture_test 2>&1 | tee capture.log &
#
#  【例子 9】连续采 10 分钟后自动停
#    timeout 600 ./camera_uart_capture_test
#
#  【例子 10】循环采某块板, 存满 100 张 BMP 后停
#    for i in $(seq 1 20); do ./camera_uart_capture_test /dev/ttyUSB0 115200 0 5 0; sleep 2; done
#
# =============================================================================


# =============================================================================
# main
# =============================================================================

# 如果二进制不存在, 报错
if [ ! -x "$BIN" ]; then
    error "找不到可执行文件: $BIN"
    info "请把 camera_uart_capture_test 和 test_capture.sh 放在同一个目录下"
    info "或者先 cp 过去: cp camera_uart_capture_test /home/mcgs/"
    info "                  cp test_capture.sh /home/mcgs/"
    exit 1
fi

case "${1:-default}" in
    default|"") run_default ;;
    fast)       run_fast ;;
    all)        run_all ;;
    snap)       run_snap ;;
    custom)     run_custom ;;
    list)       list_ports ;;
    check)      check_port "${2:-/dev/ttyUSB0}" ;;
    help|-h|--help) usage; exit 0 ;;
    *)
        # 直接把所有参数透传给二进制
        info "透传参数: $*"
        exec "$BIN" "$@"
        ;;
esac
