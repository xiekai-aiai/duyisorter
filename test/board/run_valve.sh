#!/bin/bash
# 测阀 - 1~10号阀依次喷一下
# 对应 test_arm 的 mode=1
APP_DIR=/opt/app/Test
export LD_LIBRARY_PATH=/opt/app/lib:$LD_LIBRARY_PATH

cd $APP_DIR
echo "=================================================="
echo "  测阀脚本 (mode=1): 1~10号阀依次喷一下"
echo "  LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
echo "=================================================="

./test_arm 1 2>&1 | tee valve_test.log

echo ""
echo "===== 测阀结束, 日志: $APP_DIR/valve_test.log ====="
