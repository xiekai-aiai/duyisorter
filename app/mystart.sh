#!/bin/bash

#export QTEDIR=/opt/qt5.15.15
#export LD_LIBRARY_PATH=/opt/qt5.15.15/lib:$LD_LIBRARY_PATH
#export QT_QPA_PLATFORM_PLUGIN_PATH=$QTEDIR/plugins
#export QT_QPA_FONTDIR=/usr/lib/fonts
#export QT_QPA_PLATFORM=xcb
#source ~/.bashrc

source  /opt/app/env.sh

# ====================== 文件锁防并发 ======================
# 多个启动源（autostart / session-setup / .xprofile ...）可能同时触发
# mystart.sh，没有锁会导致多个 DuySorter 进程并发启动，窗口装饰叠加
# 注意: 锁文件若曾被 root/sudo 创建, 普通用户打不开, 而 exec 重定向失败
#       会让 shell 直接退出整个脚本 → 必须用当前用户专属文件 + 失败兜底
LOCKFILE="/tmp/duysorter_start_$(id -u).lock"
if ! ( : > "$LOCKFILE" ) 2>/dev/null; then
    LOCKFILE="/opt/app/.mystart.lock"
fi
if ! exec 9>"$LOCKFILE" 2>/dev/null; then
    echo "[mystart.sh] 无法创建锁文件 $LOCKFILE, 跳过加锁继续启动"
    LOCK_HELD=0
else
    LOCK_HELD=1
fi
if [ "$LOCK_HELD" = "1" ] && ! flock -n 9; then
    echo "[mystart.sh] 已有另一份实例在跑，退出"
    exit 0
fi

# ====================== 彻底清理旧进程和残留窗口 ======================
# 先温和终止（SIGTERM），等最多 3 秒
killall -q DuySorter 2>/dev/null || true
for i in 1 2 3; do
    pgrep -x DuySorter >/dev/null 2>&1 || break
    sleep 1
done
# 还没死就强杀
if pgrep -x DuySorter >/dev/null 2>&1; then
    echo "[mystart.sh] 强杀残留 DuySorter"
    killall -9 DuySorter 2>/dev/null || true
fi

# ====================== 等待 X11 窗口管理器回收装饰 ======================
# kill 进程后 xfwm4 的窗口装饰（标题栏/边框）可能还挂着
# 给它 2 秒彻底清理干净，再启动新进程
sleep 2
# 用 wmctrl 确保没有残留窗口（如果 wmctrl 装了）
if command -v wmctrl >/dev/null 2>&1; then
    RESIDUAL=$(wmctrl -l 2>/dev/null | grep -i duysorter | wc -l | tr -d ' ')
    if [ "$RESIDUAL" -gt 0 ]; then
        echo "[mystart.sh] 还有 $RESIDUAL 个残留窗口，再等 2 秒"
        sleep 2
    fi
fi

echo "[mystart.sh] 旧进程和残留窗口清理完毕"

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
        	sudo insmod /opt/app/lib/tun.ko
	fi
	echo "tun.ko加载成功"
fi

# 拉起 WiFi 网卡
if ip link show wlan0 &>/dev/null; then
	sudo ifconfig wlan0 up 2>/dev/null
	echo "wlan0 已拉起"
else
	echo "wlan0 不存在，跳过"
fi



#if [ -s /opt/app/lib/tun.ko ] ; then
#	insmod /opt/app/lib/tun.ko
#fi
#echo "tun.ko加载成功"

#sh check_and_mount_usb.sh /dev/sda1 /udisk
#echo "mount U盘结束"

# 检查 U 盘是否已挂载
sudo mount /dev/sda1 /udisk
if [ $? -eq 0 ]; then
        echo "U盘成功挂载到 /udisk。"
else
        echo "挂载 U 盘时出错，请检查设备和权限。"
fi


if  [ -e /udisk/upgrade/logo.bmp ]; then
        echo "U盘logo文件存在"
        sudo cp /udisk/upgrade/logo.bmp  /dev/mmcblk0p8
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
        # 先备份生产数据库，防止 tar 包里的空库覆盖现场配置
        if [ -e userdata/db/sorter.db ]; then
                cp -f userdata/db/sorter.db /tmp/sorter.db.bak
        fi
        tar -xvf duyiSorter_v1.00.tar.xz
        mv -f tar_app/* .
        rm -rf tar_app
        # 如果之前有备份，恢复（tar 包里不应带 sorter.db；即使带了也覆盖回现场库）
        if [ -e /tmp/sorter.db.bak ]; then
                mkdir -p userdata/db
                cp -f /tmp/sorter.db.bak userdata/db/sorter.db
                rm -f /tmp/sorter.db.bak
        fi
        rm duyiSorter_v1.00.tar.xz
        echo "软件升级结束"
else
        echo "软件无需升级"
fi

sudo cp lib/libt* /usr/lib/aarch64-linux-gnu/

# ====================== 运行时目录初始化 ======================
# 代码中 APP_PATH "/userdata/cnf/init.json" 等路径依赖这些目录存在
# 模板在 app/cnf/，首次启动或升级后需要拷到 userdata/
mkdir -p /opt/app/userdata/i18n
mkdir -p /opt/app/userdata/cnf
mkdir -p /opt/app/userdata/model
mkdir -p /opt/app/userdata/image
mkdir -p /opt/app/userdata/db

cp -f i18n/* /opt/app/userdata/i18n/ 2>/dev/null

# AI 配置: 只在 userdata/cnf 里没有时才从模板拷贝（保留用户现有配置）
# init.json 在 cnf/，trainserver.json 在 cnf.board/
[ -f "cnf/init.json" ] && [ ! -f "userdata/cnf/init.json" ] \
    && cp -f cnf/init.json userdata/cnf/init.json \
    && echo "[mystart.sh] AI 配置初始化: init.json"
[ -f "cnf.board/trainserver.json" ] && [ ! -f "userdata/cnf/trainserver.json" ] \
    && cp -f cnf.board/trainserver.json userdata/cnf/trainserver.json \
    && echo "[mystart.sh] AI 配置初始化: trainserver.json"

chmod u+x DuySorter

# 关闭 xfce 屏保与 DPMS，避免空闲锁屏遮挡界面
pkill -f xfce4-screensaver 2>/dev/null
xset s off s noblank -dpms 2>/dev/null

# 退出窗口管理器 xfwm4：主窗口是固定尺寸(setFixedSize 1004x708)，
# showFullScreen() 实际不生效，所以 xfwm4 一定会画标题栏/边框装饰。
# kiosk 场景直接不要 WM。注意: xfce4-session 会在 xfwm4 退出后自动重启它，
# 因此必须用后台守护持续压制，只杀 xfwm4，绝不能动 eGTouchD。
if pgrep -x xfwm4 >/dev/null 2>&1; then
        xfwm4 --quit >/dev/null 2>&1 || pkill -x xfwm4 2>/dev/null
        sleep 1
fi
# 后台守护: 每 3 秒检查一次，发现 xfwm4 被拉起来就杀掉
nohup sh -c 'while true; do
        if pgrep -x xfwm4 >/dev/null 2>&1; then
                pkill -9 -x xfwm4 2>/dev/null
        fi
        sleep 3
done' >/dev/null 2>&1 &
echo "[mystart.sh] 已启动 xfwm4 压制守护 (PID=$!)，窗口将无标题栏"

# 关闭内核 fbcon 文本控制台，避免终端文字（状态栏/命令行）叠加在 Qt 画面上
for vc in /sys/class/vtconsole/vtcon*; do
        [ -w "$vc/bind" ] && echo 0 > "$vc/bind" 2>/dev/null
done
# 降低内核打印级别，避免 dmesg 信息刷到屏幕上
echo 1 > /proc/sys/kernel/printk 2>/dev/null

# start app
# 日志文件若曾被 root/sudo 创建, 普通用户无法追加, 重定向失败会导致启动失败
LOG="./userdata/duy.log"
if ! ( : >> "$LOG" ) 2>/dev/null; then
        LOG="$HOME/duy.log"
        mkdir -p "$HOME" 2>/dev/null
        echo "[mystart.sh] userdata/duy.log 不可写, 日志改到 $LOG"
fi
nohup ./DuySorter >> "$LOG" 2>&1 &
sleep 3
if pgrep -x DuySorter >/dev/null 2>&1; then
        echo "[mystart.sh] DuySorter 已启动 (PID=$(pgrep -x DuySorter | tr '\n' ' '))"
        echo "[mystart.sh] 日志: $LOG"
else
        echo "[mystart.sh] ❌ DuySorter 启动失败, 最后 20 行日志:"
        tail -20 "$LOG" 2>/dev/null
        exit 1
fi
