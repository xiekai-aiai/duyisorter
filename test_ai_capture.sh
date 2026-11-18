#!/bin/bash
# ================================================================
# 手动串口测试 AI 图像采集 —— 混合帧版本
# 来源:
#   - ONOFF 帧: 用户串口助手里抓的 (20:24:08.385 段) — sPacket[3] 正确 ✓
#   - AI采集 帧: 11-02 旧版日志里的正确帧 — 用户抓的有问题(见下!)
# 用法: 在板卡 192.168.3.157 上执行 /tmp/test_ai_capture.sh
# ================================================================

COM_PORT="/dev/ttyS4"
BAUD=115200

send() {
    local hex="$1"
    local desc="$2"
    echo -ne "$hex" > "$COM_PORT"
    echo "  [TX] $desc"
    sleep 0.1
}

echo "=============================================="
echo "  AI 采集手动测试"
echo "  串口: $COM_PORT  波特率: $BAUD"
echo "=============================================="

# ---- Step 0: 停主程序释放串口 ----
echo ""
echo "--- Step 0: 停主程序 ---"
killall DuySorter 2>/dev/null
sleep 1
stty -F "$COM_PORT" $BAUD cs8 -cstopb -parenb raw min 0 time 1 2>/dev/null
echo "  串口已配置"

# ================================================================
# Step 1: 开接口板 + 相机板 (用用户串口抓的 ONOFF 帧 —— sPacket[3] 正确)
# ================================================================
echo ""
echo "--- Step 1: CMD_ONOFF arg8=1 (开接口板+相机板) ---"
send '\xA5\x5A\x01\x00\x00\x01\x54\x00\x00\x00\x00\x01\xB1\x30\xFF\xFF'  "INT[0] ONOFF arg8=1"
send '\xA5\x5A\x02\x00\x00\x01\x55\x00\x00\x00\x00\x01\x45\x5F\xFF\xFF'  "UNIT[0] ONOFF arg8=1 (idx=0)"
send '\xA5\x5A\x02\x01\x00\x01\x56\x00\x00\x00\x00\x01\x60\x9C\xFF\xFF'  "UNIT[1] ONOFF arg8=1 (idx=1)"
sleep 0.5

# ================================================================
# Step 2: 开 AI 采集 0x110 (用 11-02 旧版正确帧)
# 注意: 用户串口抓的 0x110 帧 sPacket[3]=0x20=32, 是错的!
#       正确应该是 0x00 / 0x01 (unitAddr)
# ================================================================
echo ""
echo "--- Step 2: CMD_UNIT_AI_CAPTURE_ENABLE 0x110 arg8=1 ---"
send '\xA5\x5A\x02\x00\x01\x10\xCE\x00\x00\x00\x00\x01\x80\x90\xFF\xFF'  "UNIT[0] AI_CAPTURE_ENABLE arg8=1 (idx=0) [旧版正确帧]"
send '\xA5\x5A\x02\x01\x01\x10\xCF\x00\x00\x00\x00\x01\x2E\x13\xFF\xFF'  "UNIT[1] AI_CAPTURE_ENABLE arg8=1 (idx=1) [旧版正确帧]"

# ---- Step 3: 等 AI 板收图 ----
echo ""
echo "--- Step 3: 等 10 秒让 FPGA→AI板 推图 ---"
sleep 10

# ---- Step 4: 检查 AI 板 ----
echo ""
echo "--- Step 4: 检查 AI 板图像目录 ---"
for ip in 192.168.4.126 192.168.4.127; do
    echo "  --- AI板 $ip ---"
    if ping -c 1 -W 1 "$ip" > /dev/null 2>&1; then
        sshpass -p linaro ssh -o StrictHostKeyChecking=no -o ConnectTimeout=2 root@$ip \
            'ls -ltr /ftp/image/ 2>/dev/null | tail -5 || echo "  /ftp/image/ 不存在"' 2>&1
    else
        echo "  $ip ping 不通!"
    fi
done

# ================================================================
# Step 5: 停 AI 采集 (用 11-02 旧版正确帧)
# ================================================================
echo ""
echo "--- Step 5: CMD_UNIT_AI_CAPTURE_ENABLE 0x110 arg8=0 ---"
send '\xA5\x5A\x02\x00\x01\x10\xD3\x00\x00\x00\x00\x00\xC4\x76\xFF\xFF'  "UNIT[0] AI_CAPTURE_ENABLE arg8=0 (idx=0) [旧版正确帧]"
send '\xA5\x5A\x02\x01\x01\x10\xD4\x00\x00\x00\x00\x00\xE7\x14\xFF\xFF'  "UNIT[1] AI_CAPTURE_ENABLE arg8=0 (idx=1) [旧版正确帧]"
sleep 0.3

# ================================================================
# Step 6: 关接口板 + 相机板 (用用户串口抓的关帧)
# ================================================================
echo ""
echo "--- Step 6: CMD_ONOFF arg8=0 (关) ---"
send '\xA5\x5A\x01\x00\x00\x01\x58\x00\x00\x00\x00\x00\xAA\xF2\xFF\xFF'  "INT[0] ONOFF arg8=0"
send '\xA5\x5A\x02\x00\x00\x01\x59\x00\x00\x00\x00\x00\x5E\x9D\xFF\xFF'  "UNIT[0] ONOFF arg8=0 (idx=0)"
send '\xA5\x5A\x02\x01\x00\x01\x5A\x00\x00\x00\x00\x00\x7B\x5E\xFF\xFF'  "UNIT[1] ONOFF arg8=0 (idx=1)"

echo ""
echo "=============================================="
echo "  测试完成!"
echo "  重新启动主程序: /opt/app/mystart.sh"
echo "=============================================="
