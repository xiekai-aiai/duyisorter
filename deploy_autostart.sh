#!/bin/bash
#
# deploy_autostart.sh
# ----------------------------------------------------------------------------
# DuySorter 开机自启动一键部署脚本 (topeet 用户 + 触摸屏保护)
#
# 功能:
#   1) LightDM 自动登录 topeet  (正确 seat 名 + timeout=1)
#   2) 保护已有触摸屏驱动配置   (52-egalax-lightdm.conf 不删)
#   3) 修复 sudoers 免密        (避免 sudo 语法错误连带脚本中断)
#   4) 修复 /opt/app 目录属主    (topeet 可写 userdata/)
#   5) 放置系统级 autostart     (/etc/xdg/autostart/duysorter.desktop)
#
# 使用:
#   sudo bash deploy_autostart.sh
#   执行完 sudo systemctl restart lightdm 或 reboot
#
# 幂等:
#   可重复执行，不会破坏已有正确配置
# ----------------------------------------------------------------------------

set -e

# ---- 颜色输出 ----
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'  # No Color
ok()   { echo -e "${GREEN}✅${NC} $*"; }
warn() { echo -e "${YELLOW}⚠️${NC} $*"; }
err()  { echo -e "${RED}❌${NC} $*"; exit 1; }
info() { echo -e "${CYAN}ℹ️${NC} $*"; }

# ---- 必须 root 执行 ----
[ "$(id -u)" = "0" ] || err "请用 sudo bash $0 执行"

APPDIR="/opt/app"
APPUSER="topeet"
APPGROUP="topeet"

echo "============================================================"
echo "  DuySorter 开机自启动部署 (用户=$APPUSER)"
echo "============================================================"
echo

# =============================================================
# Step 1: LightDM 自动登录 (topeet) + 保护触摸屏配置
# =============================================================
info "Step 1/5: 配置 LightDM 自动登录 $APPUSER ..."

# 1a. 备份现有 lightdm.conf（非首次时保留历史）
if [ -f /etc/lightdm/lightdm.conf ]; then
    cp -f /etc/lightdm/lightdm.conf \
          /etc/lightdm/lightdm.conf.bak.$(date +%Y%m%d%H%M%S)
fi

# 1b. 写主配置 (SeatDefaults 是正确的 seat 名; timeout 必须 >=1)
cat > /etc/lightdm/lightdm.conf <<'EOF'
[LightDM]
[SeatDefaults]
autologin-user=topeet
autologin-user-timeout=1
user-session=xfce
[Greeter]
allow-guest=false
hide-users=true
EOF

# 1c. 保留 conf.d 里的触摸屏驱动 (52-egalax)，只清掉写坏的 99-autologin
if [ -d /etc/lightdm/lightdm.conf.d ]; then
    # 删掉之前写坏的那个 (seat:* 是 LightDM 1.30 不识别的语法)
    rm -f /etc/lightdm/lightdm.conf.d/99-autologin.conf
    # 触摸屏配置必须保留
    if [ -f /etc/lightdm/lightdm.conf.d/52-egalax-lightdm.conf ]; then
        ok "触摸屏配置已存在: 52-egalax-lightdm.conf"
    else
        warn "52-egalax-lightdm.conf 不存在，自动创建"
        mkdir -p /etc/lightdm/lightdm.conf.d
        cat > /etc/lightdm/lightdm.conf.d/52-egalax-lightdm.conf <<'EOF'
[SeatDefaults]
session-setup-script=/usr/bin/eGTouchD -f
greeter-setup-script=/usr/bin/eGTouchD -f
EOF
    fi
fi

# 1d. 验证 eGTouchD 二进制
if [ -x /usr/bin/eGTouchD ]; then
    ok "eGTouchD 触摸驱动: /usr/bin/eGTouchD"
else
    warn "/usr/bin/eGTouchD 不存在，触摸屏可能不工作！"
fi

ok "LightDM 配置完成"
echo

# =============================================================
# Step 2: 修复 sudoers (topeet 免密)
# =============================================================
info "Step 2/5: 配置 sudo 免密 ($APPUSER NOPASSWD) ..."

# 2a. 覆盖写入 (避免之前被破坏的 ALtopeet ALLL)
echo "$APPUSER ALL=(ALL:ALL) NOPASSWD: ALL" \
    > /etc/sudoers.d/topeet
chmod 440 /etc/sudoers.d/topeet

# 2b. 语法校验 (visudo -c 会读 /etc/sudoers + /etc/sudoers.d/*)
if visudo -c >/dev/null 2>&1; then
    ok "sudoers 语法校验通过"
else
    err "sudoers 语法错误！检查 /etc/sudoers.d/topeet"
fi

# 2c. 验证免密
if sudo -n -u "$APPUSER" true 2>/dev/null; then
    ok "sudo -n 免密验证通过"
else
    warn "sudo -n 免密未生效 (可能需要重登 topeet 会话)"
fi

echo

# =============================================================
# Step 3: 修复 /opt/app 属主
# =============================================================
info "Step 3/5: 修复 $APPDIR 目录属主 ..."

# 3a. 属主
chown -R "$APPUSER:$APPGROUP" "$APPDIR"

# 3b. DuySorter 可执行
if [ -f "$APPDIR/DuySorter" ]; then
    chmod +x "$APPDIR/DuySorter"
fi

# 3c. userdata 必须可写 (nohup >> duy.log 重定向到这里)
mkdir -p "$APPDIR/userdata/i18n" "$APPDIR/userdata/db"
chmod -R u+rwX "$APPDIR/userdata"

# 3d. 验证
OWNER=$(stat -c '%U:%G' "$APPDIR")
if [ "$OWNER" = "$APPUSER:$APPGROUP" ]; then
    ok "$APPDIR 属主 = $OWNER"
else
    err "$APPDIR 属主应为 $APPUSER:$APPGROUP 实际为 $OWNER"
fi

if [ -x "$APPDIR/DuySorter" ]; then
    ok "$APPDIR/DuySorter 可执行"
else
    err "$APPDIR/DuySorter 不可执行"
fi

echo

# =============================================================
# Step 4: 系统级 XFCE/KDE autostart
# =============================================================
info "Step 4/5: 配置系统级 autostart ..."

AUTOSTART_DIR="/etc/xdg/autostart"
mkdir -p "$AUTOSTART_DIR"

# Exec 里必须 cd 到 /opt/app，因为 mystart.sh 全是相对路径
cat > "$AUTOSTART_DIR/duysorter.desktop" <<'EOF'
[Desktop Entry]
Version=1.0.0
Name=DuySorter
Comment=Auto start DuySorter on boot
Exec=sh -c 'cd /opt/app && bash mystart.sh'
Type=Application
NoDisplay=true
X-GNOME-Autostart-enabled=true
X-XFCE-Autostart-Phase=Applications
EOF
chmod 644 "$AUTOSTART_DIR/duysorter.desktop"

ok "$AUTOSTART_DIR/duysorter.desktop 已部署"
echo

# =============================================================
# Step 5: 立即手动拉起 eGTouchD (当前会话立刻恢复触控)
# =============================================================
info "Step 5/5: 当前会话立即拉起触摸屏驱动 ..."

if [ -x /usr/bin/eGTouchD ]; then
    pkill -f eGTouchD 2>/dev/null || true
    sleep 1
    /usr/bin/eGTouchD -f &
    sleep 2
    if pgrep -f eGTouchD >/dev/null; then
        ok "eGTouchD 已启动 (PID=$(pgrep -f eGTouchD | tr '\n' ' '))"
    else
        warn "eGTouchD 启动失败，触摸屏可能不工作"
    fi
fi

echo
echo "============================================================"
echo -e "  ${GREEN}全部部署完成！${NC}"
echo "============================================================"
echo
echo "现在可以执行:"
echo "  sudo systemctl restart lightdm     # 重启显示管理器 (推荐)"
echo "  # 或"
echo "  sudo reboot                         # 完全重启"
echo
echo "重启后验证 (以 $APPUSER 登录):"
echo "  whoami                              # → $APPUSER"
echo "  pgrep -a DuySorter                  # → ./DuySorter"
echo "  tail -20 $APPDIR/userdata/duy.log   # → 启动日志"
echo
