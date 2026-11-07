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

# ---- 从 qtmakeApp.sh 抄的工具链路径 ----
SDK_DIR=/home/mcgs/sdk/rk3568-arm64-Qt5.15.15
export PATH=$SDK_DIR/sysroot/usr/local/qt5.15.15/bin/:$PATH
export CXX="ccache $SDK_DIR/bin/aarch64-linux-gnu-g++"
export CC="ccache $SDK_DIR/bin/aarch64-linux-gnu-gcc"
export STRIP=$SDK_DIR/bin/aarch64-linux-gnu-strip
QMAKE=$SDK_DIR/sysroot/usr/local/qt5.15.15/bin/qmake

echo "=============================================="
echo " DuySorter Test Build (ARM64 / RK3568)"
echo " sdk    = $SDK_DIR"
echo " qmake  = $QMAKE"
echo " cc     = $CC"
echo "=============================================="

# ---- 清理 ----
rm -rf Makefile test_arm
rm -rf release debug buildmin

# ---- qmake ----
echo "[..] qmake test.pro"
$QMAKE test.pro CONFIG+=release

# ---- make ----
echo "[..] make -j\$(nproc)"
make -j$(nproc) 2>&1 | tail -50

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
