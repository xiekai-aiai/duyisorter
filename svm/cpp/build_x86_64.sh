#!/bin/bash
set -e

# ===== 本机 x86_64 编译 SVM 训练程序（调试用）=====
# 参考 build/build_opencv_x86_64.sh
# 与目标机 aarch64 版本保持一致，仅编译器不同，便于本机调试
#
# 用法:
#   ./build_x86_64.sh                    # 默认输出 svm_train_x86_64
#   OUTPUT=svm_train ./build_x86_64.sh   # 自定义输出文件名

CXX="${CXX:-g++}"
OUTPUT="${OUTPUT:-svm_train_x86_64}"

cd "$(dirname "$0")"

echo "编译器: $($CXX --version | head -1)"
echo "命令:   $CXX -O2 -o $OUTPUT train.cpp svm.cpp"
$CXX -O2 -o "$OUTPUT" train.cpp svm.cpp

echo
echo "产出: $OUTPUT"
if command -v file >/dev/null 2>&1; then
    file "$OUTPUT"
fi
