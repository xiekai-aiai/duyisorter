#!/bin/bash
# =============================================================================
#  camera_board_check — 相机板在线检测 独立编译脚本
# =============================================================================
#
#  使用:
#    ./build.sh arm        # 交叉编译 → RK3568 (ARM aarch64) (默认)
#    ./build.sh native     # 本机 x86_64 编译 (只验证语法)
#    ./build.sh clean      # 清理产物
#
#  产物:
#    arm64/camera_board_check   ← 拷到板卡直接运行
#
#  在 RK3568 目标机上运行:
#    chmod +x camera_board_check
#    ./camera_board_check                            # 检测板 0
#    ./camera_board_check /dev/ttyS4 115200 -scan    # 扫描 0~7
#    ./camera_board_check -v /dev/ttyS4 115200 0     # verbose
# =============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; CYAN='\033[0;36m'; NC='\033[0m'
info()    { echo -e "${GREEN}[INFO]${NC}  $*"; }
warn()    { echo -e "${YELLOW}[WARN]${NC}  $*"; }
error()   { echo -e "${RED}[ERROR]${NC} $*"; }
section() { echo -e "\n${CYAN}====================================================${NC}"; echo -e "${CYAN}  $*${NC}"; echo -e "${CYAN}====================================================${NC}"; }

SDK_ROOT="/home/mcgs/sdk/rk3568-arm64-Qt5.15.15"
CROSS_GPP="${SDK_ROOT}/bin/aarch64-linux-gnu-g++"
SRC="camera_board_check.cpp"
OUT_BIN_ARM="camera_board_check_arm64"
OUT_BIN_X64="camera_board_check_x86_64"

do_clean() {
    section "清理产物"
    rm -rf arm64 native
    info "完成"
}

do_build_arm() {
    section "交叉编译 → ARM aarch64 (RK3568)"

    local CC="${CROSS_GPP}"
    local OUT_DIR="arm64"
    local OUT_BIN="${OUT_DIR}/${OUT_BIN_ARM}"

    if [ ! -x "${CC}" ]; then
        error "交叉编译器不存在: ${CC}"
        error "请在远程编译服务器上执行, 或用 CROSS_GPP 指定路径"
        return 1
    fi

    info "编译器: ${CC}"
    mkdir -p "${OUT_DIR}"

    # RK3568 = Cortex-A55 (armv8-a 基础版), 不加 crypto/fp16/dotprod 扩展
    local CXXFLAGS="-march=armv8-a -mtune=cortex-a55"
    CXXFLAGS="${CXXFLAGS} -std=c++11 -O2 -Wall -Wextra -D_GNU_SOURCE -DLINUX -Wno-unused-parameter"

    local LDFLAGS="-static-libstdc++ -static-libgcc"

    info "  编译 ${SRC} → ${OUT_BIN}"
    if ! ${CC} ${CXXFLAGS} "${SRC}" -o "${OUT_BIN}" ${LDFLAGS}; then
        error "编译失败"
        return 1
    fi

    local STRIP="${SDK_ROOT}/bin/aarch64-linux-gnu-strip"
    if [ -x "${STRIP}" ]; then
        "${STRIP}" "${OUT_BIN}"
    fi

    section "产物验证"
    file "${OUT_BIN}"
    echo ""
    if file "${OUT_BIN}" | grep -qi "aarch64\|arm.*64"; then
        info "✅ 架构正确: ARM aarch64"
    else
        error "❌ 架构不对!"
        return 1
    fi

    info "产物: ${OUT_BIN}  ($(du -h "${OUT_BIN}" | cut -f1))"
    info "运行: scp ${OUT_BIN} root@<板卡IP>:/root/ && ./${OUT_BIN_ARM}"
}

do_build_native() {
    section "本机 x86_64 编译 (语法验证)"
    local CC="g++"
    local OUT_BIN="native/${OUT_BIN_X64}"
    mkdir -p native
    ${CC} -std=c++11 -O2 -Wall -Wextra "${SRC}" -o "${OUT_BIN}"
    file "${OUT_BIN}"
    info "✅ 本机编译成功 (仅语法验证, 不能跑在 RK3568 上)"
}

case "${1:-arm}" in
    arm|arm64|rk3568) do_build_arm ;;
    native|local|host) do_build_native ;;
    clean) do_clean ;;
    *) echo "用法: $0 [arm|native|clean]"; exit 1 ;;
esac
