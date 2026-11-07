#!/bin/bash
# Direct 串口测阀: com1 CMD_INT_EJTEST → 接口板
# 不走 AI 板子 UDP, 直接 FPGA 串口驱动喷阀
# 10 个喷阀依次 开/关 (低速)
#
# 用法:  cd /opt/app/Test && ./run_direct_valve.sh
# 日志:  tee 到 direct_valve.log

set -e
cd "$(dirname "$0")"

export LD_LIBRARY_PATH=/opt/app/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

echo "========== Direct 串口测阀 (mode=5) =========="
echo "二进制: $(ls -lh test_arm | awk '{print $NF,$5,$6,$7,$8}')"
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
echo ""

timeout 25 ./test_arm 5 2>&1 | tee direct_valve.log

RC=${PIPESTATUS[0]}
echo ""
echo "========== 退出码: $RC =========="
if [ $RC -eq 124 ]; then
    echo "(124=timeout, 属预期, 测阀跑完程序自己会退出)"
fi
echo "日志已保存到: $(pwd)/direct_valve.log"
