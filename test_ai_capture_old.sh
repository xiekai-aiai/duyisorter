#!/bin/bash
# ================================================================
# 手动串口测试 AI 图像采集 —— 完全用用户串口抓的帧
# 源: 用户串口助手 20:24:08.385 + 20:24:17.845 去重后
# 注意: 每帧在串口里重复3次(回显), 这里只发1次
# 用法: 在板卡 192.168.3.157 上执行
# ================================================================

COM_PORT="/dev/ttyS3"
BAUD=115200

send() {
    local hex="$1"
    local desc="$2"
    echo -ne "$hex" > "$COM_PORT"
    echo "  [TX] $desc"
    sleep 0.1
}

echo "=============================================="
echo "  AI 采集手动测试 (用户串口抓帧版)"
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
# Step 1: 开料序列 (ONOFF + AI采集开)
# 用户串口抓的完整开料帧, 按时间顺序
# ================================================================
echo ""
echo "--- Step 1: 开料序列 ---"

# 前置参数帧 (RGB增益等) — 省略, 直接从 ONOFF 开始
# 第一批: sPacket[3]=0x20 的 UNIT_ONOFF arg8=0 帧
send '\xA5\x5A\x02\x00\x20\x02\x50\x00\x00\x00\x00\x00\xB5\x95\xFF\xFF'  "UNIT ONOFF arg8=0 sPkt3=0x20"
send '\xA5\x5A\x02\x01\x20\x02\x51\x00\x00\x00\x00\x00\x1B\x16\xFF\xFF'  "UNIT ONOFF arg8=0 sPkt3=0x20"

# 第二批: sPacket[2]=0x42 的 UNIT_ONOFF arg8=1 帧
send '\xA5\x5A\x02\x00\x42\x02\x52\x00\x00\x00\x00\x01\x2C\xEA\xFF\xFF'  "UNIT ONOFF arg8=1 sPkt2=0x42"
send '\xA5\x5A\x02\x01\x42\x02\x53\x00\x00\x00\x00\x01\x82\x69\xFF\xFF'  "UNIT ONOFF arg8=1 sPkt2=0x42"

# 第三批: 标准 ONOFF 帧 (INT + UNIT[0] + UNIT[1])
send '\xA5\x5A\x01\x00\x00\x01\x54\x00\x00\x00\x00\x01\xB1\x30\xFF\xFF'  "INT[0] ONOFF arg8=1"
send '\xA5\x5A\x02\x00\x00\x01\x55\x00\x00\x00\x00\x01\x45\x5F\xFF\xFF'  "UNIT[0] ONOFF arg8=1"
send '\xA5\x5A\x02\x01\x00\x01\x56\x00\x00\x00\x00\x01\x60\x9C\xFF\xFF'  "UNIT[1] ONOFF arg8=1"

# 第四批: AI采集 0x110 开 (用户抓的 sPacket[3]=0x20)
send '\xA5\x5A\x02\x20\x01\x10\x57\x00\x00\x00\x00\x01\xA1\x2A\xFF\xFF'  "AI_CAPTURE_ENABLE arg8=1 sPkt3=0x20"

# ---- Step 2: 等 AI 板收图 ----
echo ""
echo "--- Step 2: 等 10 秒 ---"
sleep 10

# ---- Step 3: 检查 AI 板 ----
echo ""
echo "--- Step 3: 检查 AI 板 ---"
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
# Step 4: 停料序列 (ONOFF arg8=0 + AI采集关)
# ================================================================
echo ""
echo "--- Step 4: 停料序列 ---"
send '\xA5\x5A\x01\x00\x00\x01\x58\x00\x00\x00\x00\x00\xAA\xF2\xFF\xFF'  "INT[0] ONOFF arg8=0"
send '\xA5\x5A\x02\x00\x00\x01\x59\x00\x00\x00\x00\x00\x5E\x9D\xFF\xFF'  "UNIT[0] ONOFF arg8=0"
send '\xA5\x5A\x02\x01\x00\x01\x5A\x00\x00\x00\x00\x00\x7B\x5E\xFF\xFF'  "UNIT[1] ONOFF arg8=0"
send '\xA5\x5A\x02\x20\x01\x10\x5B\x00\x00\x00\x00\x00\xBA\xE8\xFF\xFF'  "AI_CAPTURE_ENABLE arg8=0 sPkt3=0x20"

echo ""
echo "=============================================="
echo "  测试完成!"
echo "=============================================="
