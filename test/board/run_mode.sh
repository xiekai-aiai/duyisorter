#!/bin/bash
cd "$(dirname "$0")"
export LD_LIBRARY_PATH=/opt/app/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

if [ $# -lt 1 ]; then
    echo "用法: $0 <mode> [timeout_sec]"
    echo "  mode: 1=AI测阀  2=单色1开料  3=AI+单色1  4=诊断  5=Direct串口测阀"
    exit 1
fi
MODE=$1
DURATION=${2:-25}

echo "===== test_arm mode=$MODE (timeout=${DURATION}s) ====="
echo "二进制: $(ls -lh test_arm | awk '{print $NF,$5,$6,$7,$8}')"

( ./test_arm "$MODE" > run_mode${MODE}.log 2>&1 ) &
PID=$!

for ((i=1; i<=DURATION; i++)); do
    sleep 1
    if ! kill -0 $PID 2>/dev/null; then
        break
    fi
done

if kill -0 $PID 2>/dev/null; then
    echo "[wrapper] 超时, kill -9 $PID"
    kill -9 $PID 2>/dev/null
    wait $PID 2>/dev/null
    RC=124
else
    wait $PID 2>/dev/null
    RC=$?
fi

echo ""
echo "===== 退出码: $RC ====="
echo "日志: $(pwd)/run_mode${MODE}.log"
echo "----- 最后10行 -----"
tail -10 run_mode${MODE}.log
