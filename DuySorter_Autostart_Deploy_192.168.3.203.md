# DuySorter 开机自启动部署方案（板卡 192.168.3.203）

> 目标机：RK3568 / Ubuntu 22.04.4 LTS (aarch64) / Xubuntu(XFCE 4.16) / LightDM 1.30.0
> 登录账号：`topeet`（密码 `topeet`，属 `sudo` 组，`NOPASSWD: ALL`）
> 编写日期：2026-11-16
> 参考机型：192.168.3.157（Ubuntu 18.04 + Lubuntu/LXDE，已验证的 root 自启动方案）

---

## 1. 方案结论

**可以落地，但不能原样照搬 3.157 的方案。**

3.157 的链路是：

```
lightdm 自动登录 root → Lubuntu(lxsession) → /etc/xdg/autostart/duysorter.desktop → /opt/app/mystart.sh → DuySorter
```

3.203 的链路调整为：

```
lightdm 自动登录 topeet（已配置好）→ Xubuntu(xfce4-session) → /etc/xdg/autostart/duysorter.desktop → /opt/app/mystart.sh → DuySorter
```

核心差异只有两点：

1. **自动登录用户是 `topeet` 而不是 `root`**，脚本里需要 root 权限的几条命令必须加 `sudo`；
2. **桌面从 LXDE 换成 XFCE**，但 XFCE 的 `xfce4-session` 同样遵循 XDG autostart 规范（已实测：`/etc/xdg/autostart` 下的 `blueman.desktop`、`nm-applet.desktop` 已被会话自动拉起），机制完全兼容。

不建议把自动登录用户改成 root：本机 LightDM 1.30 未找到 `allow-root` 选项支持，且 PAM/桌面会话对 root 图形登录限制更多，风险高于收益。

---

## 2. 目标机现状（已实测）

| 检查项 | 结果 | 说明 |
|---|---|---|
| 系统 | Ubuntu 22.04.4 LTS，内核 4.19.232，aarch64 | 与 3.157 的 18.04 不同 |
| 显示管理器 | LightDM 1.30.0，`active (running)` | 已安装并运行 |
| 自动登录 | `/etc/lightdm/lightdm.conf`：`autologin-user=topeet`、`autologin-user-timeout=0` | **已配置，无需修改** |
| 桌面会话 | `Desktop=xubuntu`、`Type=x11`、`Service=lightdm-autologin` | XFCE 会话 |
| XDG autostart | `/etc/xdg/autostart/` 存在且生效 | 已有 `blueman`、`nm-applet` 由 xfce4-session 拉起 |
| duysorter.desktop | **不存在** | 需新建 |
| DuySorter 进程 | **未运行** | 尚未接入自启动 |
| /opt/app | 完整：DuySorter(84M)、mystart.sh、env.sh、myrestart.sh、lib/tun.ko、norm_train、userdata、i18n | 属主 `topeet:topeet`，可写 |
| Qt | `/opt/qt5.15.15` 存在，`env.sh` 已指向 | 无需配置 |
| sudo | `topeet` 属 `sudo` 组，`%sudo ALL=(ALL) NOPASSWD: ALL` | 脚本可免密提权 |
| 串口 | `/dev/ttyS3` 属 `root:dialout`，topeet 在 dialout 组 | 可访问 |
| 相机 | `/dev/video0`~`video4` 存在，topeet 在 video 组 | 可访问 |
| 屏保 | `xfce4-screensaver 4.16` 正在运行 | 空闲会锁屏，需关闭 |
| /udisk | 软链到 `/mnt/udisk`，目录属主 root | mount 需 sudo |
| 磁盘 | `/` 12G，已用 49% | 空间充足 |

---

## 3. 实施前检查

在板卡上以 `topeet` 登录后执行，全部通过再继续：

```bash
# 1) 确认图形会话正常
loginctl show-session c1 | grep -E "Service|Desktop|Type|Active"

# 2) 确认免密 sudo 可用
sudo -n true && echo "sudo OK"

# 3) 确认程序与脚本齐全
ls -l /opt/app/DuySorter /opt/app/mystart.sh /opt/app/env.sh
```

预期输出：

```
Service=lightdm-autologin
Desktop=xubuntu
Type=x11
Active=yes
sudo OK
-rwxr-xr-x 1 topeet topeet ... /opt/app/DuySorter
-rwxr-xr-x 1 topeet topeet ... /opt/app/mystart.sh
-rwxr-xr-x 1 topeet topeet ... /opt/app/env.sh
```

---

## 4. 实施步骤

### 步骤 1：备份现有启动脚本

```bash
cd /opt/app
cp -a mystart.sh mystart.sh.bak.20261116
ls -l mystart.sh*
```

### 步骤 2：修改 `mystart.sh`

共 6 处改动：

| # | 原内容 | 修改后 | 原因 |
|---|---|---|---|
| 1 | `insmod /opt/app/lib/tun.ko` | `sudo insmod /opt/app/lib/tun.ko` | 加载内核模块需 root |
| 2 | `ifconfig wlan0 up 2>/dev/null` | `sudo ifconfig wlan0 up 2>/dev/null` | 网卡 up 需 CAP_NET_ADMIN |
| 3 | `mount /dev/sda1 /udisk` | `sudo mount /dev/sda1 /udisk` | 挂载需 root |
| 4 | `cp /udisk/upgrade/logo.bmp /dev/mmcblk0p8` | `sudo cp ... /dev/mmcblk0p8` | 写块设备需 root |
| 5 | `cp lib/libt* /usr/lib/aarch64-linux-gnu/` | `sudo cp lib/libt* ...` | 写系统库目录需 root |
| 6 | `./DuySorter &` | `nohup ./DuySorter >> ./userdata/duy.log 2>&1 &` | 保留运行日志（3.157 行为） |

另外新增 2 行关闭屏保（见下方脚本 `# 关闭屏保` 处）。

> 注意：其余内容（U 盘升级、数据库备份、解压逻辑等）保持原样，不要改动。

**方式 A（推荐，直接覆盖）**：执行以下命令，粘贴整段即可（heredoc 用单引号，变量不会被展开）：

```bash
cat > /opt/app/mystart.sh <<'EOF'
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

mkdir -p /opt/app/userdata/i18n
cp -f i18n/* /opt/app/userdata/i18n/ 2>/dev/null

chmod u+x DuySorter

# 关闭屏保与 DPMS，避免空闲锁屏遮挡界面（本机新增）
pkill -f xfce4-screensaver 2>/dev/null
xset s off s noblank -dpms 2>/dev/null

# start app
nohup ./DuySorter >> ./userdata/duy.log 2>&1 &
EOF
chmod 755 /opt/app/mystart.sh
```

**方式 B（手工编辑）**：用 `vi /opt/app/mystart.sh` 按上表逐处修改，注意第 6 处末尾的 `&`。

**语法自检**：

```bash
bash -n /opt/app/mystart.sh && echo "语法 OK"
grep -n "sudo\|nohup\|xset" /opt/app/mystart.sh
```

预期能看到 6 处 `sudo`、1 处 `nohup`、1 处 `xset`。

### 步骤 3：创建开机自启动项

```bash
sudo tee /etc/xdg/autostart/duysorter.desktop >/dev/null <<'EOF'
[Desktop Entry]
Version=1.0.0
Name=DuySorter
Comment=Auto start DuySorter on boot
Exec=sh -c '/opt/app/mystart.sh'
Type=Application
NoDisplay=true
X-GNOME-Autostart-enabled=true
EOF
sudo chmod 644 /etc/xdg/autostart/duysorter.desktop
cat /etc/xdg/autostart/duysorter.desktop
```

说明：

- `X-GNOME-Autostart-enabled=true` 同时被 GNOME 与 XFCE 识别，确保不会被会话忽略；
- 不要在同名路径 `/home/topeet/.config/autostart/duysorter.desktop` 再放一份，避免重复启动。

### 步骤 4：手动验证（先不重启）

```bash
export DISPLAY=:0
/opt/app/mystart.sh
sleep 8
pgrep -a DuySorter
tail -n 30 /opt/app/userdata/duy.log
```

预期：

- 板卡屏幕上出现 DuySorter 界面；
- `pgrep -a DuySorter` 输出进程号与 `./DuySorter`；
- `duy.log` 有新日志；
- `~/.xsession-errors`（或终端输出）可见 `tun.ko加载成功`、`U盘...`、`软件无需升级` 等提示。

> 若终端提示 `mount: /udisk: special device /dev/sda1 does not exist`，属正常（当前没插 U 盘），不影响启动。

### 步骤 5：重启验收

```bash
sudo reboot
```

等待约 60~90 秒后重新 SSH 登录，执行第 5 节验收命令。

---

## 5. 验收标准

逐项打勾：

```bash
# 1) 进程存在且 PPID=1（由会话拉起后被 init 收养）
ps -o pid,ppid,lstart,cmd -C DuySorter

# 2) 可执行文件与工作目录正确
readlink /proc/$(pgrep -x DuySorter)/exe     # 期望 /opt/app/DuySorter
readlink /proc/$(pgrep -x DuySorter)/cwd     # 期望 /opt/app

# 3) 会话信息
loginctl show-session c1 | grep -E "Service|Desktop|Type|Active"

# 4) 运行日志持续写入
tail -n 30 /opt/app/userdata/duy.log
ls -l /opt/app/userdata/duy.log

# 5) 自启动项与脚本
cat /etc/xdg/autostart/duysorter.desktop
grep -n "sudo\|nohup" /opt/app/mystart.sh

# 6) 界面与业务（人工确认）
#    - 全屏界面正常显示，无黑屏/白屏
#    - 相机画面、串口通信正常
#    - 空闲 10 分钟不锁屏、不黑屏
```

| 验收项 | 通过标准 | 结果 |
|---|---|---|
| 开机自动进入桌面 | 无需输入密码，自动进入 XFCE 会话 | ☐ |
| DuySorter 自动启动 | 开机 90 秒内出现界面 | ☐ |
| 进程归属 | `PPID=1`，exe 指向 `/opt/app/DuySorter` | ☐ |
| 日志 | `/opt/app/userdata/duy.log` 有本次启动日志 | ☐ |
| 硬件 | 串口、相机、AI 功能正常 | ☐ |
| 无锁屏 | 空闲 10 分钟界面不被遮挡 | ☐ |
| 手动重启脚本 | `/opt/app/myrestart.sh` 可正常重启程序 | ☐ |

---

## 6. 回滚

```bash
# 1) 移除自启动项
sudo rm -f /etc/xdg/autostart/duysorter.desktop

# 2) 恢复原启动脚本
cp -a /opt/app/mystart.sh.bak.20261116 /opt/app/mystart.sh
chmod 755 /opt/app/mystart.sh

# 3) 停止当前实例（可选）
kill $(pgrep -x DuySorter) 2>/dev/null

# 4) 重启确认
sudo reboot
```

回滚后系统恢复为"自动登录到桌面、不自动启动 DuySorter"。

---

## 7. 故障排查

| 现象 | 排查命令 | 常见原因 / 处理 |
|---|---|---|
| 重启后没有 DuySorter | `tail -n 80 ~/.xsession-errors` | 脚本报错，看最后几行；`bash -n` 自检 |
| 同上 | `journalctl -b \| grep -iE "duy\|xfce4-session\|lightdm"` | 会话未启动或 desktop 文件格式错误 |
| 进程存在但界面黑屏 | `DISPLAY=:0 xrandr` | X 未就绪或 `QT_QPA_PLATFORM` 不对，检查 `env.sh` |
| 终端提示 sudo 需要密码 | `sudo -n true; id` | topeet 不在 sudo 组或 sudoers 被改 |
| `insmod` / `mount` 失败 | `sudo -n insmod ...` 单独试 | sudo 生效但内核模块与内核版本不匹配 |
| tun.ko 加载失败 | `uname -r; modinfo /opt/app/lib/tun.ko` | 模块与 4.19.232 不匹配，业务若不依赖 VPN 可忽略 |
| `duy.log` 不增长 | `ls -l /opt/app/userdata/duy.log` | 目录/文件权限被 root 改过，`sudo chown topeet:topeet` |
| 界面被锁屏遮挡 | `DISPLAY=:0 xset q \| grep -i "screen saver"` | 确认 `mystart.sh` 中 `xset` 两行已执行；或在桌面设置里关闭屏保 |
| U 盘升级不生效 | `ls -l /udisk/upgrade/` | `/dev/sda1` 未识别、或 U 盘不是第一个分区 |
| 重复启动两个实例 | `pgrep -a DuySorter` | 用户级 autostart 也放了一份，删掉 `~/.config/autostart/duysorter.desktop` |

日志位置汇总：

| 日志 | 路径 |
|---|---|
| 主程序运行日志 | `/opt/app/userdata/duy.log` |
| 会话启动输出（含 mystart.sh 的 echo） | `~/.xsession-errors` |
| 系统/会话日志 | `journalctl -b` |
| LightDM 日志 | `/var/log/lightdm/lightdm.log` |

---

## 8. 风险与注意事项

1. **强依赖图形会话**：与 3.157 一致，若 LightDM 或 XFCE 会话启动失败，DuySorter 不会启动。如需更高可靠性，可后续评估 systemd + X11 依赖方案，但改动面大，本次不做。
2. **以 topeet 身份运行**：串口（dialout）、相机（video）、数据库与配置目录均可用；若程序内部有直接写 `/dev/mmcblk*`、`/sys/class/gpio`、`/sys/class/pwm` 等操作，需要再补 sudo 或 udev 规则。建议上板实测全部业务功能。
3. **U 盘升级逻辑沿用原脚本**：升级包无签名/校验，升级过程中断电有风险，与 3.157 行为一致。
4. **`mount /dev/sda1` 可能报错**：无 U 盘时正常报错但不影响后续启动（脚本未启用 `set -e`）。
5. **不要同时保留用户级 autostart**：`/etc/xdg/autostart` 与 `~/.config/autostart` 同名文件会覆盖而非叠加，但若文件名不同会导致启动两次。
6. **XFCE 与 LXDE 的窗口行为差异**：主程序使用 `showFullScreen()`，XFCE 下一般正常；若出现窗口未全屏或被任务栏遮挡，需在 XFCE 会话保存一次窗口设置或调整窗口管理器。

---

## 9. 附录：备选方案（整脚本 sudo 运行）

仅当步骤 4 验证发现 topeet 权限不足（例如程序必须写 `/dev/mmcblk*`、GPIO）时使用。思路是保持脚本零改动，让整个脚本以 root 运行，同时把 X 显示权限透传给 root。

`/etc/xdg/autostart/duysorter.desktop` 改为：

```ini
[Desktop Entry]
Version=1.0.0
Name=DuySorter
Comment=Auto start DuySorter as root
Exec=sh -c 'xhost +SI:localuser:root >/dev/null 2>&1; sudo -n env DISPLAY=:0 XAUTHORITY=/home/topeet/.Xauthority /opt/app/mystart.sh'
Type=Application
NoDisplay=true
X-GNOME-Autostart-enabled=true
```

使用该方案前必须先手动验证：

```bash
export DISPLAY=:0
xhost +SI:localuser:root
sudo -n env DISPLAY=:0 XAUTHORITY=/home/topeet/.Xauthority /opt/app/mystart.sh
```

确认 root 能正常拉起 Qt 界面、且 `ps -o user,cmd -C DuySorter` 显示 `root` 再重启。若界面起不来，回退到方案 A。

---

## 10. 实施记录

| 时间 | 操作 | 执行人 | 结果 |
|---|---|---|---|
|  | 备份 mystart.sh |  |  |
|  | 修改 mystart.sh（6 处） |  |  |
|  | 创建 duysorter.desktop |  |  |
|  | 手动验证 |  |  |
|  | 重启验收 |  |  |
