#!/bin/bash
# =============================================================================
#  杀死所有 camera_uart_ 相关进程 (测试采集程序)
# =============================================================================
#  用法:
#    chmod +x kill_camera_uart.sh
#    ./kill_camera_uart.sh
# =============================================================================

PATTERN="camera_uart_"
SELF=$$

echo "=== 查找进程 (匹配: ${PATTERN}) ==="

# 列出匹配的 PID, 排除本脚本自身
PIDS=$(pgrep -f "$PATTERN" 2>/dev/null | grep -v "^${SELF}$")

if [ -z "$PIDS" ]; then
    echo "未找到匹配进程"
    exit 0
fi

echo "找到以下进程:"
ps -o pid,cmd -p $PIDS 2>/dev/null

# 先 SIGTERM 优雅退出
echo "发送 SIGTERM ..."
kill $PIDS 2>/dev/null
sleep 1

# 若仍存活, SIGKILL 强杀
LEFT=$(pgrep -f "$PATTERN" 2>/dev/null | grep -v "^${SELF}$")
if [ -n "$LEFT" ]; then
    echo "以下进程未退出, 发送 SIGKILL ..."
    ps -o pid,cmd -p $LEFT 2>/dev/null
    kill -9 $LEFT 2>/dev/null
fi

echo "完成"
