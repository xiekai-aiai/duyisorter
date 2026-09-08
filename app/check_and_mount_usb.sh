#!/bin/bash

# 检查是否提供了设备和挂载点参数
if [ $# -ne 2 ]; then
    echo "用法: $0 <设备名称> <挂载点>"
    exit 1
fi

USB_DEVICE="$1"
MOUNT_POINT="$2"

# 检查 U 盘是否已挂载
if mount | grep -q "$USB_DEVICE"; then
    echo "U盘已挂载。"
else
    echo "U盘未挂载，尝试进行挂载..."
    # 创建挂载点
    mkdir -p "$MOUNT_POINT"
    # 尝试挂载 U 盘
    mount "$USB_DEVICE" "$MOUNT_POINT"
    if [ $? -eq 0 ]; then
        echo "U盘成功挂载到 $MOUNT_POINT。"
    else
        echo "挂载 U 盘时出错，请检查设备和权限。"
    fi
fi
    
