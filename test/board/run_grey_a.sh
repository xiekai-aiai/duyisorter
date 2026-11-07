#!/bin/bash
# 方案A选料 - 单色1 蓝色 + 供料速度50% + 开料10秒自动关料
# 对应 test_arm 的 mode=2
APP_DIR=/opt/app/Test
export LD_LIBRARY_PATH=/opt/app/lib:$LD_LIBRARY_PATH

cd $APP_DIR
echo "=================================================="
echo "  方案A选料脚本 (mode=2): 单色1 蓝色 + 供料50% + 10秒"
echo "  LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
echo "=================================================="

./test_arm 2 2>&1 | tee grey_a_feed.log

echo ""
echo "===== 选料结束, 日志: $APP_DIR/grey_a_feed.log ====="
