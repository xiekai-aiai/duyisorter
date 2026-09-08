#!/bin/bash
# =============================================================================
#  camera_uart_capture — 独立编译脚本
# =============================================================================
#
#  功能: 交叉编译出能在 RK3568 (ARM aarch64) 目标机上运行的独立测试程序
#        或者本机 x86_64 调试编译 (不带 --host 参数)
#
#  使用:
#    ./build.sh arm        # 交叉编译 → 目标机运行 (默认)
#    ./build.sh native     # 本机 x86_64 编译 (只验证语法, 无法跑在板卡上)
#    ./build.sh clean      # 清理产物
#
#  产物:
#    arm64/camera_uart_capture_test   ← 拷到板卡 /opt/app/ 或 /home/mcgs/ 直接运行
#
#  在 RK3568 目标机上运行:
#    chmod +x camera_uart_capture_test
#    ./camera_uart_capture_test
#    ./camera_uart_capture_test /dev/ttyUSB0 115200 0 5 0
#
# =============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# ── 颜色输出 ────────────────────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

info()    { echo -e "${GREEN}[INFO]${NC}  $*"; }
warn()    { echo -e "${YELLOW}[WARN]${NC}  $*"; }
error()   { echo -e "${RED}[ERROR]${NC} $*"; }
section() { echo -e "\n${CYAN}====================================================${NC}"; echo -e "${CYAN}  $*${NC}"; echo -e "${CYAN}====================================================${NC}"; }

# ── 工具链路径 (和 qtmakeApp.sh 保持一致) ─────────────────────────────
SDK_ROOT="/home/mcgs/sdk/rk3568-arm64-Qt5.15.15"
CROSS_GPP="${SDK_ROOT}/bin/aarch64-linux-gnu-g++"
CROSS_GCC="${SDK_ROOT}/bin/aarch64-linux-gnu-gcc"
TARGET_ARCH="aarch64-linux-gnu"

# ── 源码文件 ────────────────────────────────────────────────────────
SRCS=(
    camera_uart_reader.cpp
    camera_uart_capture_test.cpp
)

OBJS="${SRCS[*]}"
OBJS="${OBJS//.cpp/.o}"

# =============================================================================
# 子命令
# =============================================================================

do_clean() {
    section "清理产物"
    rm -rf arm64 native
    info "完成"
}

# =============================================================================
# ARM aarch64 交叉编译 (目标机 RK3568)
# =============================================================================

do_build_arm() {
    section "交叉编译 → ARM aarch64 (RK3568)"

    local CC="${CROSS_GPP}"
    local OUT_DIR="arm64"
    local OUT_BIN="${OUT_DIR}/camera_uart_capture_test_arm64"

    # 检查交叉编译器是否存在
    if [ ! -x "${CC}" ]; then
        error "交叉编译器不存在或不可执行:"
        error "  期望路径: ${CC}"
        error ""
        error "如果在本地 Mac 上运行本脚本:"
        error "  1. 在远程编译服务器 (192.168.172.136) 上执行 ./build.sh arm"
        error "  2. 或者手动指定 CROSS_GPP 环境变量"
        error "     CROSS_GPP=/path/to/aarch64-linux-gnu-g++ ./build.sh arm"
        return 1
    fi

    info "编译器: ${CC}"
    info "SDK 根: ${SDK_ROOT}"
    info "目标架构: ${TARGET_ARCH}"

    # 创建输出目录
    mkdir -p "${OUT_DIR}"

    # ── CPU 指令集 (RK3568 = Cortex-A55, armv8-a 基础版) ────────────
    # 必须显式指定, 否则交叉编译器可能默认加 +crypto/+fp16/+dotprod,
    # 这些扩展指令 RK3568 不支持 → 运行时 Illegal instruction
    # armv8-a 64-bit 默认就是 NEON + hard-float, 不需要额外指定 -mfpu
    CXXFLAGS="-march=armv8-a -mtune=cortex-a55"

    # ── 语言/警告/优化 ──────────────────────────────────────────────
    CXXFLAGS="${CXXFLAGS} -std=c++11 -O2 -Wall -Wextra -fno-exceptions"
    CXXFLAGS="${CXXFLAGS} -D_GNU_SOURCE -DLINUX"
    CXXFLAGS="${CXXFLAGS} -Wno-unused-parameter"

    LDFLAGS="-static-libstdc++ -static-libgcc"
    # 注意: 不 -static, 因为我们只用了 libstdc++/libgcc,
    #       其他系统库 (libc, libpthread, libm) RK3568 板卡上自带

    # ── 编译所有 .cpp → .o ────────────────────────────────────────
    local ok=1
    for src in "${SRCS[@]}"; do
        local obj="${OUT_DIR}/${src%.cpp}.o"
        info "  编译 ${src} → ${obj}"
        if ! ${CC} ${CXXFLAGS} -c "${src}" -o "${obj}"; then
            error "编译失败: ${src}"
            ok=0; break
        fi
    done

    [ "$ok" = "0" ] && return 1

    # ── 链接 ─────────────────────────────────────────────────────
    info "  链接 → ${OUT_BIN}"
    local OBJ_FILES=""
    for src in "${SRCS[@]}"; do
        OBJ_FILES="${OBJ_FILES} ${OUT_DIR}/${src%.cpp}.o"
    done

    if ! ${CC} ${OBJ_FILES} -o "${OUT_BIN}" ${LDFLAGS}; then
        error "链接失败"
        return 1
    fi

    # ── strip (可选, 减小体积) ──────────────────────────────────────
    local STRIP="${SDK_ROOT}/bin/aarch64-linux-gnu-strip"
    if [ -x "${STRIP}" ]; then
        info "  strip: $(du -h "${OUT_BIN}" | cut -f1) → "
        "${STRIP}" "${OUT_BIN}"
        info "  strip 后: $(du -h "${OUT_BIN}" | cut -f1)"
    else
        warn "strip 工具不存在, 跳过"
    fi

    # ── 验证架构 ──────────────────────────────────────────────────
    section "产物验证"
    file "${OUT_BIN}"
    echo ""

    # 检查是否真的是 ARM
    if file "${OUT_BIN}" | grep -qi "aarch64\|arm.*64"; then
        info "✅ 架构正确: ARM aarch64 (可在 RK3568 上运行)"
    else
        error "❌ 架构不对! 预期 aarch64, 实际:"
        file "${OUT_BIN}"
        return 1
    fi

    echo ""
    info "────────────────────────────────────────"
    info "✅ 编译成功!"
    info "产物路径: ${OUT_BIN}"
    info "文件大小: $(du -h "${OUT_BIN}" | cut -f1)"
    echo ""
    info "拷到目标机运行:"
    info "  scp ${OUT_BIN} mcgs@<RK3568-IP>:/home/mcgs/"
    info "  ssh mcgs@<RK3568-IP>"
    info "  chmod +x camera_uart_capture_test"
    info "  ./camera_uart_capture_test"
    info "────────────────────────────────────────"

    return 0
}

# =============================================================================
# x86_64 本机编译 (只验证语法, 无法在 RK3568 上跑)
# =============================================================================

do_build_native() {
    section "本机 x86_64 编译 (语法验证, 非目标机)"

    local CC="g++"
    local OUT_DIR="native"
    local OUT_BIN="${OUT_DIR}/camera_uart_capture_test_x86_64"

    # 检查本机有没有 g++
    if ! command -v g++ &>/dev/null; then
        error "本机没有 g++, 请先安装 (sudo apt install g++)"
        return 1
    fi

    info "编译器: $(which g++)"
    info "本机架构: $(uname -m)"

    mkdir -p "${OUT_DIR}"

    local CXXFLAGS="-std=c++11 -O2 -Wall -Wextra -D_GNU_SOURCE -DLINUX -Wno-unused-parameter"

    local OBJ_FILES=""
    for src in "${SRCS[@]}"; do
        local obj="${OUT_DIR}/${src%.cpp}.o"
        info "  编译 ${src} → ${obj}"
        if ! ${CC} ${CXXFLAGS} -c "${src}" -o "${obj}"; then
            error "编译失败: ${src}"
            return 1
        fi
        OBJ_FILES="${OBJ_FILES} ${obj}"
    done

    info "  链接 → ${OUT_BIN}"
    if ! ${CC} ${OBJ_FILES} -o "${OUT_BIN}"; then
        error "链接失败"
        return 1
    fi

    section "产物验证"
    file "${OUT_BIN}"

    info "✅ 本机编译成功 (语法验证通过, 但只能在本机 x86_64 上运行)"
    info "   产物: ${OUT_BIN}"
    info ""
    warn "⚠️  注意: 这个二进制是 x86_64 的, 不能拷到 RK3568 (ARM aarch64) 上跑!"
    warn "   要用 ./build.sh arm 交叉编译才能在板卡上运行"

    return 0
}


# =============================================================================
# 主入口
# =============================================================================

case "${1:-arm}" in
    arm|arm64|rk3568)
        do_build_arm
        ;;
    native|local|host)
        do_build_native
        ;;
    clean)
        do_clean
        ;;
    *)
        echo "用法: $0 [arm|native|clean]"
        echo "  arm      交叉编译 RK3568 ARM64 (默认)"
        echo "  native   本机 x86_64 编译 (语法验证)"
        echo "  clean    清理产物"
        exit 1
        ;;
esac
