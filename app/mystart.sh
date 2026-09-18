#!/bin/bash

#export QTEDIR=/opt/qt5.15.15
#export LD_LIBRARY_PATH=/opt/qt5.15.15/lib:$LD_LIBRARY_PATH
#export QT_QPA_PLATFORM_PLUGIN_PATH=$QTEDIR/plugins
#export QT_QPA_FONTDIR=/usr/lib/fonts
#export QT_QPA_PLATFORM=xcb
#source ~/.bashrc

source  /opt/app/env.sh

# 查找 DuySorter 进程（排除 grep 自身）
PID=$(ps -ef | grep DuySorter | grep -v grep | awk '{print $2}')

# 如果 PID 不为空，说明进程在运行
if [ -n "$PID" ]; then
    echo "发现 DuySorter 进程，正在停止..."
    kill $PID
    
    # 等待进程退出（可选，更安全）
    sleep 1
    echo "DuySorter 进程已停止"
else
    echo "DuySorter 进程未运行"
fi

# if have network config file, do it
#NETCONF=/userdata/cnf/cnf.network
#if [ -s $NETCONF ] ; then
#	source $NETCONF
#
#	DEV_ADDR=`cat /sys/class/net/eth0/address`
#	if [ "$DEV_ADDR" != "$MAC" ]; then
#		ifconfig eth0 down
#		ifconfig eth0 hw ether $MAC
#	fi
#
#	ifconfig eth0 $IP netmask $Mask up
#	route add default gw $Gateway
#
#	if ! grep -q "$DNS" /etc/resolv.conf; then
#		echo nameserver $DNS > /etc/resolv.conf
#	fi
#else
#	/sbin/ifconfig eth0 192.168.0.10 netmask 255.255.255.0 up
#	route add default gw 192.168.0.1
#	echo nameserver 192.168.0.1 > /etc/resolv.conf
#fi

if lsmod | grep -q tun; then
	echo "tun模块已加载"
else
	if [ -s /opt/app/lib/tun.ko ] ; then
        	insmod /opt/app/lib/tun.ko
	fi
	echo "tun.ko加载成功"
fi



#if [ -s /opt/app/lib/tun.ko ] ; then
#	insmod /opt/app/lib/tun.ko
#fi
#echo "tun.ko加载成功"

#sh check_and_mount_usb.sh /dev/sda1 /udisk
#echo "mount U盘结束"

# 检查 U 盘是否已挂载
mount /dev/sda1 /udisk
if [ $? -eq 0 ]; then
        echo "U盘成功挂载到 /udisk。"
else
        echo "挂载 U 盘时出错，请检查设备和权限。"
fi


if  [ -e /udisk/upgrade/logo.bmp ]; then
        echo "U盘logo文件存在"
        cp /udisk/upgrade/logo.bmp  /dev/mmcblk0p8
else
        echo "U盘log文件不存在"
fi

if  [ -e /udisk/upgrade/duyiSorter_v1.00.tar.xz ]; then
        echo "U盘升级包存在"
        cp /udisk/upgrade/duyiSorter_v1.00.tar.xz  /opt/app
else
	echo "U盘升级包不存在"
fi


cd /opt/app

if [ -e duyiSorter_v1.00.tar.xz ]; then
        echo "软件升级开始"
        tar -xvf duyiSorter_v1.00.tar.xz
        mv -f tar_app/* .
        rm -rf tar_app
        rm duyiSorter_v1.00.tar.xz
        echo "软件升级结束"
else
        echo "软件无需升级"
fi

cp lib/libt* /usr/lib/aarch64-linux-gnu/

mkdir -p /opt/app/userdata/i18n
cp -f i18n/* /opt/app/userdata/i18n/ 2>/dev/null

chmod u+x DuySorter
 
# start app
./DuySorter &
