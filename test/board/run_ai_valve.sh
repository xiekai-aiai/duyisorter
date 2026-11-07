#!/bin/bash
# AI 板子测阀: UDP FixCheck → 192.168.4.126
# 模仿上位机 "测阀模式 + 使能 AI" 路径
# 10 个喷阀依次 StartFixCheck / StopFixCheck (低速 interval=800ms)
#
# 用法:  cd /opt/app/Test && ./run_ai_valve.sh
# 日志:  tee 到 ai_valve.log

set -e
cd "$(dirname "$0")"

export LD_LIBRARY_PATH=/opt/app/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

echo "========== AI 板子测阀 (mode=1) =========="
echo "二进制: $(ls -lh test_arm | awk '{print $NF,$5,$6,$7,$8}')"
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
echo ""

timeout 25 ./test_arm 1 2>&1 | tee ai_valve.log

RC=${PIPESTATUS[0]}
echo ""
echo "========== 退出码: $RC =========="
if [ $RC -eq 124 ]; then
    echo "(124=timeout, 属预期, 测阀跑完程序自己会退出)"
fi
echo "日志已保存到: $(pwd)/ai_valve.log"
