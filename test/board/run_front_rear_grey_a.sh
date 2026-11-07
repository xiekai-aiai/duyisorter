#!/bin/bash
# 前后视不同 GREY_A (单色1) 配置 + 测阀 1~32 + 供料速度 40% + 10秒自动关料
# 对应 test_arm 的 mode=6
#
# 前视 (group 0): 蓝低 mode=0 精度 100% (nSensMin=1000) 行数4 斑点2
# 后视 (group 1): 蓝低 mode=0 精度   0% (nSensMin=0)     行数4 斑点2
# 测阀从 1 到 32 停止 (Direct CMD_INT_EJTEST 串口)
# 然后开料 10 秒 → 关料 → 退出
#
APP_DIR=/opt/app/Test
export LD_LIBRARY_PATH=/opt/app/lib:$LD_LIBRARY_PATH

cd $APP_DIR
echo "=================================================="
echo "  mode=6: 前后视不同 GREY_A 测试"
echo "  前视 G0: 蓝低 100%  后视 G1: 蓝低 0%"
echo "  测阀 1~32 + 供料 40% + 10秒"
echo "=================================================="

./test_arm 6 2>&1 | tee front_rear_grey_a.log

echo ""
echo "===== 测试结束, 日志: $APP_DIR/front_rear_grey_a.log ====="
