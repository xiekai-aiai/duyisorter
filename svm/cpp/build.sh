#!/bin/bash
set -e

# ===== 交叉编译 SVM 训练程序（ARM32 hard-float）=====
# 目标: arm-linux-gnueabihf (ARMv7-A, glibc 2.18, SigmaStar GCC 9.1)
#
# 用法:
#   ./build.sh                # 静态链接（默认，推荐，板端无需额外动态库）
#   STATIC=0 ./build.sh       # 动态链接（体积小，板端需有 libstdc++/libc）
#   OUTPUT=svm_train_arm ./build.sh   # 自定义输出文件名
#   TOOLCHAIN=/path/to/toolchain ./build.sh   # 自定义工具链路径

TOOLCHAIN="${TOOLCHAIN:-/usr/local/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf}"
CXX="$TOOLCHAIN/bin/arm-linux-gnueabihf-g++"
OUTPUT="${OUTPUT:-svm_train}"
STATIC="${STATIC:-1}"

if [ ! -x "$CXX" ]; then
    echo "错误: 找不到交叉编译器 $CXX" >&2
    echo "请用 TOOLCHAIN=/path/to/toolchain 指定工具链根目录" >&2
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
else
    "$TOOLCHAIN/bin/arm-linux-gnueabihf-readelf" -h "$OUTPUT" | head -20
fi
