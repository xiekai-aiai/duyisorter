#!/bin/bash

# ====================== root 权限自提升 ======================
# 以 topeet 等普通用户启动时, 以下操作全部会报权限错误:
#   ifconfig wlan0 up/down   → SIOCSIFFLAGS: 不允许的操作
#   kill wpa_supplicant      → kill: (xxx) - 不允许的操作
#   wpa_cli 访问控制 socket  → Failed to connect to non-global ctrl_ifname: Permission denied
#   killall hostapd、写 /etc/wpa_supplicant.conf 同样需要 root
# 板卡已配置 topeet NOPASSWD sudo, 因此在此用 sudo 重新执行自身
if [ "$(id -u)" != "0" ]; then
	exec sudo -n "$0" "$@"
fi

cnt1=`ps aux | grep hostapd | grep -v grep  | wc -l`
if [ "$cnt1" != "0" ];then
        killall hostapd > /dev/null
fi

#/etc/init.d/S80dnsmasq stop > /dev/null


function usage()
{
    echo "Usage: -i <wifi> -s <ssid> -p <password>"
    echo " -i : mlan0 or mlan1"
    echo " -s : wifi ssid"
    echo " -p : wifi password or NONE"
}

function parse_args()
{
    while true; do
        case "$1" in
            -i ) wifi=$2;echo wifi $wifi;shift 2 ;;
            -s ) ssid=$2;echo ssid $ssid;shift 2 ;;
            -p ) pasw=$2;echo pasw $pasw;shift 2 ;;
            -h ) usage; exit 1 ;;
            * ) break ;;
        esac
    done
}

if [ $# != 6 ]
then
    usage;
    exit 1;
fi

parse_args $@

# ====================== 有 NetworkManager 时交给 nmcli ======================
# 桌面 XFCE 的 NetworkManager 一直在管理 wlan0, 系统里的 wpa_supplicant 是
# NM 以 root 拉起的. 脚本若再 ifconfig wlan0 down / kill wpa_supplicant /
# 另起一个 wpa_supplicant, 就会出现:
#   SIOCSIFFLAGS: 不允许的操作
#   kill: (xxx) - 不允许的操作
#   Failed to connect to non-global ctrl_ifname: wlan0 error: Permission denied
# 所以 NM 在运行时统一用 nmcli 连接, 不再和 NM 抢网卡.
nm_running()
{
	command -v nmcli >/dev/null 2>&1 || return 1
	nmcli -t -f RUNNING general 2>/dev/null | grep -qi running
}

# 等 DHCP 拿到 IPv4 地址, 最多 20 秒
wait_ip()
{
	local n=0
	while [ $n -lt 20 ]
	do
		if ip -4 addr show "$1" 2>/dev/null | grep -q 'inet '; then
			return 0
		fi
		sleep 1
		n=$((n+1))
	done
	return 1
}

if nm_running; then
	# 同一 SSID 的旧配置先删掉, 保证本次输入的新密码生效
	nmcli connection delete id "$ssid" >/dev/null 2>&1
	nmcli device set "$wifi" managed yes >/dev/null 2>&1
	echo "waiting..."
	if [ "$pasw" == NONE ]; then
		nmcli device wifi connect "$ssid" ifname "$wifi" hidden yes >/dev/null 2>&1
	else
		nmcli device wifi connect "$ssid" password "$pasw" ifname "$wifi" hidden yes >/dev/null 2>&1
	fi
	rc=$?
	if [ "$rc" = "0" ] && wait_ip "$wifi"; then
		echo "Finshed!"
		exit 0
	fi
	echo "************************************************"
	echo "connect faild,please check the passward and ssid"
	exit 1
fi

if [ -e /etc/wpa_supplicant.conf ]
then
    rm /etc/wpa_supplicant.conf
fi
    echo \#PSK/TKIP >> /etc/wpa_supplicant.conf
	echo ctrl_interface=/var/run/wpa_supplicant >>/etc/wpa_supplicant.conf
	echo ctrl_interface_group=0 >>/etc/wpa_supplicant.conf
	echo update_config=1 >>/etc/wpa_supplicant.conf
	echo network={ >>/etc/wpa_supplicant.conf
    echo ssid=\"$ssid\" >>/etc/wpa_supplicant.conf
	echo scan_ssid=1 >>/etc/wpa_supplicant.conf
    if [ $pasw == NONE ]
	then
		echo key_mgmt=NONE >>/etc/wpa_supplicant.conf
	else
		echo psk=\"$pasw\" >>/etc/wpa_supplicant.conf
		echo key_mgmt=WPA-EAP WPA-PSK IEEE8021X NONE >>/etc/wpa_supplicant.conf
		echo group=CCMP TKIP WEP104 WEP40 >>/etc/wpa_supplicant.conf
	fi
    echo } >>/etc/wpa_supplicant.conf

ifconfig -a|grep mlan0 |grep -v grep  > /dev/null
if [ $? -eq 0 ]
then
	ifconfig mlan0 down > /dev/null
fi

#ifconfig -a|grep mlan1 |grep -v grep  > /dev/null
#if [ $? -eq 0 ]
#then
#	ifconfig mlan1 down > /dev/null
#fi

#ifconfig -a|grep eth0 |grep -v grep  > /dev/null
#if [ $? -eq 0 ]
#then
#	ifconfig eth0 down > /dev/null
#fi

#ifconfig -a|grep eth1 |grep -v grep  > /dev/null
#if [ $? -eq 0 ]
#then
#	ifconfig eth1 down > /dev/null
#fi

ps -fe|grep wpa_supplicant |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
	kill -9 $(pidof wpa_supplicant)
fi

sleep 1
ifconfig $wifi up > /dev/null 
sleep 1

(wpa_supplicant -Dnl80211,wext -i$wifi -c/etc/wpa_supplicant.conf  >/dev/null) &
echo "waiting..."
sleep 3
wpa_cli -i$wifi status |grep COMPLETED |grep -v grep >/dev/null
if [ $? -eq 0 ]
then
	udhcpc -i $wifi
	echo "Finshed!" 
else
	echo "try to connect again..."
	sleep 3
	wpa_cli -i$wifi status |grep COMPLETED |grep -v grep >/dev/null
		if [ $? -eq 0 ]
		then
        		udhcpc -i $wifi
				#echo "nameserver 114.114.114.114" > /etc/resolv.conf
       			echo "Finshed!"
		else
        		echo "************************************************"
       	 		echo "connect faild,please check the passward and ssid"
  		      	kill -9 $(pidof wpa_supplicant)
			exit 1
		fi
fi
