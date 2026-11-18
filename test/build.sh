#!/bin/bash
#
# test/build.sh  - 在编译机上编译 DuySorter 无UI ARM64 测试程序
#
#  依赖: 编译机 sdk 路径 /home/mcgs/sdk/rk3568-arm64-Qt5.15.15/
#  用法: cd /mnt/hgfs/duyisorter/test && bash build.sh
#

set -e

SCRIPT_DIR=$(cd `dirname $0`; pwd)
cd $SCRIPT_DIR

# ---- 工具链: 与主工程 qtmakeApp.sh 完全一致 ----
# 老脚本指向 mcgs 的 sdk 路径, 这台编译机上不存在, 改用系统 gcc-12 + Sysroot
SYSROOT=/home/root123/project/Sysroot
ARM_QT_LIBS=$SYSROOT/usr/lib/aarch64-linux-gnu
QMAKE=/usr/bin/qmake
CROSS=aarch64-linux-gnu
GCC_VER=12

CC="$CROSS-gcc-$GCC_VER --sysroot=$SYSROOT"
CXX="$CROSS-g++-$GCC_VER --sysroot=$SYSROOT"
LINK="$CROSS-g++-$GCC_VER --sysroot=$SYSROOT"
AR="$CROSS-ar"
STRIP="$CROSS-strip"
OBJCOPY="$CROSS-objcopy"
RANLIB="$CROSS-ranlib"

CCACHE_STR=""
if command -v ccache >/dev/null 2>&1; then
    CC="ccache $CC"
    CXX="ccache $CXX"
    CCACHE_STR=" ccache=开"
fi

# VM 内存有限(3G), 并行度上限 4
JOBS=$(nproc)
[ "$JOBS" -gt 4 ] && JOBS=4

echo "=============================================="
echo " DuySorter Test Build (ARM64 / RK3568)"
echo " sysroot= $SYSROOT$CCACHE_STR"
echo " qmake  = $QMAKE"
echo " cc     = $CC"
echo "=============================================="

# ---- 清理 ----
rm -rf Makefile test_arm
rm -rf release debug buildmin

# ---- qmake ----
echo "[..] qmake test.pro"
$QMAKE test.pro CROSS_COMPILE=1 CONFIG+=release \
    "QMAKE_CC=$CC" \
    "QMAKE_CXX=$CXX" \
    "QMAKE_LINK=$LINK" \
    "QMAKE_LINK_C=$CC" \
    "QMAKE_AR=$AR cqs" \
    "QMAKE_STRIP=$STRIP" \
    "QMAKE_OBJCOPY=$OBJCOPY" \
    "QMAKE_RANLIB=$RANLIB"

# ---- 修复 host qmake 写死的 x86_64 Qt 路径 (否则链接报 file in wrong format) ----
for mk in Makefile Makefile.Release; do
    [ -f "$mk" ] && perl -i -pe "s|/usr/lib/x86_64-linux-gnu/lib(Qt5[A-Za-z]+)\.so|-L$ARM_QT_LIBS -l\$1|g; s|-L/usr/lib/x86_64-linux-gnu|-L$ARM_QT_LIBS|g" "$mk"
done
X86_LEFT=$(grep -c '/usr/lib/x86_64-linux-gnu/libQt' Makefile 2>/dev/null || true)
echo "[..] x86_64 Qt 路径残留: ${X86_LEFT:-0}"

# ---- make ----
echo "[..] make -j$JOBS"
make -j$JOBS 2>&1 | tail -50

# ---- 找产物 ----
BIN=""
for d in release debug .; do
    [ -f "$SCRIPT_DIR/$d/test_arm" ] && BIN="$SCRIPT_DIR/$d/test_arm" && break
done
if [ -z "$BIN" ]; then
    echo "[ERROR] build failed"
    exit 1
fi

echo ""
echo "=============================================="
echo " BUILD OK"
echo " binary : $BIN"
echo " size   : $(ls -lh $BIN | awk '{print $5}')"
echo "=============================================="
