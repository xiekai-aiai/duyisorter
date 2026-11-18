#!/bin/bash
set -e

# ===== 交叉编译 SVM 训练程序（RK3566 / aarch64）=====
# 参考 build/build_opencv_rk3566.sh 的工具链配置
# 目标: aarch64-linux-gnu (ARMv8-A, glibc, RK3566 SDK)
#
# 用法:
#   ./build_rk3566.sh                 # 静态链接（默认，板端无需额外动态库）
#   STATIC=0 ./build_rk3566.sh        # 动态链接（体积小，板端需有对应 libstdc++/libc）
#   OUTPUT=svm_train ./build_rk3566.sh          # 自定义输出文件名
#   TOOLCHAIN_DIR=/path/to/sdk ./build_rk3566.sh  # 自定义 SDK 根目录

TOOLCHAIN_DIR="${TOOLCHAIN_DIR:-/home/mcgs/sdk/rk3568-arm64-Qt5.15.15}"
CXX="$TOOLCHAIN_DIR/bin/aarch64-linux-gnu-g++"
READELF="$TOOLCHAIN_DIR/bin/aarch64-linux-gnu-readelf"
OUTPUT="${OUTPUT:-svm_train}"
STATIC="${STATIC:-1}"

if [ ! -x "$CXX" ]; then
    echo "错误: 找不到交叉编译器 $CXX" >&2
    echo "请用 TOOLCHAIN_DIR=/path/to/sdk 指定 SDK 根目录" >&2
    exit 1
fi

FLAGS="-O2"
if [ "$STATIC" = "1" ]; then
    FLAGS="$FLAGS -static"
fi

cd "$(dirname "$0")"

echo "编译器: $($CXX --version | head -1)"
echo "命令:   $CXX $FLAGS -o $OUTPUT train.cpp svm.cpp"
$CXX $FLAGS -o "$OUTPUT" train.cpp svm.cpp

echo
echo "产出: $OUTPUT"
if command -v file >/dev/null 2>&1; then
    file "$OUTPUT"
elif [ -x "$READELF" ]; then
    "$READELF" -h "$OUTPUT" | head -20
fi
