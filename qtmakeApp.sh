#!/bin/sh
#
# qtmakeApp.sh
#
# Build or clean the project

path_cur=$(cd `dirname $0`; pwd)
cd $path_cur
lib_base_path="$path_cur/src/3rdparty"

# global param #
PLATFORM_335X=1
# machine type
MACHINE_CG=15

# product type
OPT_PRODUCT_ANCOO_GENERAL=1

# custom req
MYCUSTOM_REQ_TMP=0

# global function
print_usage()
{
	echo "----------------------------------------"
	echo ""
	echo "Please use parameters as follow"
	echo "Clean   Mode: ./qtmakeApp.sh clean   "
	echo "Debug   Mode: ./qtmakeApp.sh debug   [Product List]"
	echo "Release Mode: ./qtmakeApp.sh release [Product List]"
	echo ""
	echo "***** Product List *****"
	echo "6SXZ-600G_335X            	CG	1024x768(IPC335x Serial TS)"
	echo "----------------------------------------"
}

print_conf()
{
	if [ $# -ne 10 ]
	then
		echo "****************************************"
		echo ""
		echo "error : failed to create configure file"
		echo ""
		echo "****************************************"
		exit 1
	fi

	cat <<-!EOF > $1
	[Lcd]
	MYLCD_WID=$2
	MYLCD_HEI=$3

	[Touch]
	MYTSMODE=$4

	[Machine]
	MYMACHINE=$5

	[Product]
	MYPRODUCT=$6

	[Version]
	MYAPP_NAME=$7
	MYAPP_VERSION=$8
	MYAPP_SVN=$9

	[Custom]
	MYCUSTOM_REQ=$MYCUSTOM_REQ_TMP
	!EOF

	until [ -z "$1" ]
	do
		echo "$1"
		shift
	done
}

# check parameter
if [ $# -gt 2 ]
then
	print_usage
	exit 1
fi

# debug && release && clean
case "$1" in
	"clean")
		make distclean;
		exit 1;;
	"debug")
		MYMODEOPT="debug";
		break;;
	"release")
		MYMODEOPT="release";
		if [ "$NO_BUILD" != "1" ]; then
		  make distclean;
		fi
		break;;
	*)
		print_usage
		exit 1;;
esac

# product list
case "$2" in
        "6SXZ-600G_335X")
		MYLCD_WID_TMP=1024;
		MYLCD_HEI_TMP=768;
		MYVERSION_RELEASE="V1.06";
		MYVERSION_TMP="V1.06";
		MYMACHINE_TMP=$MACHINE_CG;
		MYPRODUCT_TMP=$OPT_PRODUCT_ANCOO_GENERAL;
		MYPRDOPT=$2;
        MYCUSTOM_REQ_TMP=0;
		MYAIMODE=0;
		MYTSMODE=0;
		break;;
	*)
		print_usage
		exit 1;;
esac


# platform
#if [[ $2 == *335X* ]]; then
	PATH=/home/mcgs/sdk/rk3568-arm64-Qt5.15.15/sysroot/usr/local/qt5.15.15/bin/:$PATH
	STRIP=/home/mcgs/sdk/rk3568-arm64-Qt5.15.15/bin/aarch64-linux-gnu-strip
	QMAKE=/home/mcgs/sdk/rk3568-arm64-Qt5.15.15/sysroot/usr/local/qt5.15.15/bin/qmake
	APP_DIR=app
	REL_DIR=rel
	MYPLATFORM=$PLATFORM_335X
export PATH

# ========== 编译加速配置 ==========
# ccache: 二次编译命中缓存直接跳过
export CXX="ccache /home/mcgs/sdk/rk3568-arm64-Qt5.15.15/bin/aarch64-linux-gnu-g++"
export CC="ccache /home/mcgs/sdk/rk3568-arm64-Qt5.15.15/bin/aarch64-linux-gnu-gcc"
export CACHEFILE=/home/mcgs/.ccache
export CCACHE_MAXSIZE=5G
# 并行编译: 用全部可用核数
MYJOBS=$(nproc)
echo "🚀 编译加速: ccache=开, 并行作业=${MYJOBS}"

# set global parameters for qmake
$QMAKE -set MYAPP_DIR  $APP_DIR
$QMAKE -set MYLCD_WID  $MYLCD_WID_TMP
$QMAKE -set MYLCD_HEI  $MYLCD_HEI_TMP
$QMAKE -set MYAIMODE   $MYAIMODE
$QMAKE -set MYPLATFORM $MYPLATFORM

# ------ 备份 app/ 关键文件 ------
# distclean 只删 app/DuySorter 二进制, 其他脚本配置会保留
# NO_BUILD=1 时跳过备份恢复 (根本不会跑 distclean)
if [ "$NO_BUILD" != "1" ]; then
  APP_BACKUP=/tmp/duyi_app_backup_$$
  mkdir -p $APP_BACKUP
  for f in DuySorter.conf mystart.sh myrestart.sh env.sh wifi.sh check_and_mount_usb.sh log.properties norm_train i18n OpenVPN db; do
    if [ -e "$APP_DIR/$f" ]; then
      cp -rf "$APP_DIR/$f" $APP_BACKUP/ 2>/dev/null
    fi
  done
  echo ">> 关键文件已备份到 $APP_BACKUP"
fi

# set global parameters for app
MYCONFFILE="$APP_DIR/DuySorter.conf"
#MYSVN_TMP=`svn info | awk 'BEGIN{FS=": "} $1=="Revision" || $1=="版本" {print $2}'`
MYSVN_TMP=35

MYCONF=`cat <<EOF
${MYCONFFILE}
${MYLCD_WID_TMP}
${MYLCD_HEI_TMP}
${MYTSMODE}
${MYMACHINE_TMP}
${MYPRODUCT_TMP}
${MYPRDOPT}
${MYVERSION_RELEASE}
${MYSVN_TMP}
${MYCUSTOM_REQ_TMP}
EOF`


print_conf $MYCONF

if [ "$NO_BUILD" = "1" ]; then
  echo ">> NO_BUILD=1, 跳过编译, 直接打包"
else
  echo 1
  # build
  $QMAKE
  make $MYMODEOPT -j${MYJOBS}
  # check compile 
  if [ $? -ne 0 ]
  then
  	echo "complie erro ,Please check program"
  	exit 1
  fi
  
  # strip executable file
  $STRIP $APP_DIR/DuySorter
fi

# ------ 恢复 app/ 关键文件 ------
if [ "$NO_BUILD" != "1" ]; then
  for f in $APP_BACKUP/*; do
    name=$(basename "$f")
    if [ ! -e "$APP_DIR/$name" ]; then
      echo "   恢复: $name"
      cp -rf "$f" "$APP_DIR/"
    fi
  done
  rm -rf $APP_BACKUP
  echo ">> 关键文件已恢复"
fi

# debug mode: exit
#if [ $MYMODEOPT = "debug" ]
#then
#	exit 0
#fi


# release mode: 打包升级包
# 包顶层为 tar_app/，供目标机 mystart.sh 解压后 "mv tar_app/* ." 使用
#

# ------ 第一步：把所有运行时 .so 统一复制到 $APP_DIR/lib/ ------
# 规范: 所有 ARM .so 必须放 src/3rdparty/<name>/arm/lib/
# 现支持: opencv / openssl / libssh2 / unilog / spdlog / libts
# tun.ko / fonts 等非 .so 运行时资源保持在 app/lib/ 不动

echo ">> [1/3] 同步 3rdparty ARM .so -> $APP_DIR/lib/"

# 清空旧 .so, 避免版本残留 (不删 tun.ko/fonts/)
find $path_cur/$APP_DIR/lib -maxdepth 1 -type l -delete
find $path_cur/$APP_DIR/lib -maxdepth 1 -name '*.so*' -type f -delete

libdirs=`ls $lib_base_path`
for lib in $libdirs
do
  libdir="$lib_base_path/$lib/arm/lib"
  if [ -d "$libdir" ]; then
     # -L: 跟随软链拷实体, 避免断裂软链进入包
     # 注意: libdir 里可能有 cmake/ 子目录 (opencv), 用通配符只拷文件
     echo "   - $lib"
     cp -Lf $libdir/*.so* $path_cur/$APP_DIR/lib/ 2>/dev/null || true
  fi
done

# 重建软链 (cp -L 后软链信息丢失, 必须按 SOVERSION 重建)
# 规则: 对每个实体 .so 文件, 自动推断:
#   libssh2.so.1.0.1 → libssh2.so.1 (MAJOR=1) → libssh2.so
#   libcrypto.so.1.1 → libcrypto.so.1 (MAJOR=1) → libcrypto.so
#   libts.so.0.10.1  → libts.so.0  (MAJOR=0)  → libts.so
#   libopencv_core.so.405 → libopencv_core.so (两段式, 数字本身就是兼容版本)
cd $path_cur/$APP_DIR/lib

# 先删所有旧软链, 保证干净
find . -maxdepth 1 -type l -delete

# 对每个实体 .so 文件, 自动按 SOVERSION 重建软链
for sofile in $(find . -maxdepth 1 -type f -name '*.so*' -printf '%f\n'); do
  base=$(echo "$sofile" | sed -E 's/\.so\..*$//')   # libssh2, libcrypto, ...

  # 三段+式: libssh2.so.1.0.1 / libcrypto.so.1.1 / libts.so.0.10.1
  # → 先建 libX.so.MAJOR -> 实体, 再建 libX.so -> libX.so.MAJOR
  if echo "$sofile" | grep -qE '\.so\.[0-9]+\.[0-9]+(\.[0-9]+)?$'; then
    major=$(echo "$sofile" | sed -E 's/.*\.so\.([0-9]+)\..*/\1/')
    ln -sf "$sofile"   "${base}.so.${major}"
    ln -sf "${base}.so.${major}" "${base}.so"
    echo "   + ${base}.so.${major} -> ${sofile},  ${base}.so -> ${base}.so.${major}"

  # 两段式: libopencv_core.so.405 / libts.so.0
  # → 只建 libX.so -> 实体 (数字本身就是兼容版本号)
  elif echo "$sofile" | grep -qE '\.so\.[0-9]+$'; then
    ln -sf "$sofile" "${base}.so"
  fi
done

# 校验: 所有 .so 软链必须能 resolve
echo ">> [1.5/3] 软链完整性校验"
LINK_BAD=0
cd $path_cur/$APP_DIR/lib
for f in $(ls -1 *.so* 2>/dev/null); do
  if [ -L "$f" ]; then
    target=$(readlink "$f")
    if [ ! -e "$target" ] && [ ! -e "$(dirname $f)/$target" ]; then
      echo "   !! BROKEN LINK: $f -> $target"
      LINK_BAD=$((LINK_BAD+1))
    fi
  fi
done
if [ $LINK_BAD -eq 0 ]; then
  echo "   ✅ 所有软链 OK"
else
  echo "   ❌ 发现 $LINK_BAD 个断裂软链, 请检查 src/3rdparty!"
  exit 1
fi

echo ">> lib 概览:"
ls -la $path_cur/$APP_DIR/lib/ | grep -E '\.so|tun\.ko'

# ------ 第二步：组装 tar_app/ ------
cd $path_cur   # 回到项目根目录 (lib 处理环节 cd 过 lib/ 子目录)
rm -rf tar_app
mkdir -p tar_app/userdata/db

cp -rf $path_cur/$APP_DIR/DuySorter      tar_app/
cp -rf $path_cur/$APP_DIR/DuySorter.conf tar_app/
cp -rf $path_cur/$APP_DIR/mystart.sh     tar_app/
cp -rf $path_cur/$APP_DIR/env.sh         tar_app/
cp -rf $path_cur/$APP_DIR/myrestart.sh   tar_app/
cp -rf $path_cur/$APP_DIR/wifi.sh        tar_app/
cp -rf $path_cur/$APP_DIR/check_and_mount_usb.sh tar_app/
cp -rf $path_cur/$APP_DIR/log.properties tar_app/
cp -rf $path_cur/$APP_DIR/norm_train     tar_app/
cp -rf $path_cur/$APP_DIR/i18n           tar_app/
cp -rf $path_cur/$APP_DIR/lib            tar_app/
cp -rf $path_cur/$APP_DIR/OpenVPN        tar_app/
cp -rf $path_cur/$APP_DIR/db/*           tar_app/userdata/db/

# 校验: 关键文件不能缺 (cp 失败时立刻退出, 避免出坏包)
MISSING=""
for f in DuySorter mystart.sh env.sh myrestart.sh lib/DuySorter.conf; do
  # 注意 DuySorter.conf 在 lib/ 里？不在！是 $APP_DIR/DuySorter.conf 直接 cp 到 tar_app/
  case "$f" in
    DuySorter) [ -e "tar_app/DuySorter" ] || MISSING="$MISSING $f" ;;
    mystart.sh) [ -e "tar_app/mystart.sh" ] || MISSING="$MISSING $f" ;;
    env.sh) [ -e "tar_app/env.sh" ] || MISSING="$MISSING $f" ;;
    myrestart.sh) [ -e "tar_app/myrestart.sh" ] || MISSING="$MISSING $f" ;;
  esac
done
if [ -n "$MISSING" ]; then
  echo "❌ 关键文件缺失:$MISSING"
  echo "   请确认 $APP_DIR/ 目录完整!"
  rm -rf tar_app
  exit 1
fi

# 确保所有启动脚本有可执行权限（源文件权限可能不是 755）
chmod +x tar_app/DuySorter tar_app/mystart.sh tar_app/myrestart.sh \
         tar_app/env.sh tar_app/wifi.sh tar_app/check_and_mount_usb.sh

mkdir -p $REL_DIR
TAR_NAME="duyiSorter_${MYPRDOPT}_${MYVERSION_RELEASE}_${MYSVN_TMP}.tar.xz"
rm -f $REL_DIR/$TAR_NAME
tar -cJf $REL_DIR/$TAR_NAME tar_app
rm -rf tar_app

# 最终校验: 包不能太小 (正常 > 5MB, 小于 1MB 肯定是坏包)
PKG_SIZE=$(stat -c%s $REL_DIR/$TAR_NAME 2>/dev/null || stat -f%z $REL_DIR/$TAR_NAME 2>/dev/null)
if [ "$PKG_SIZE" -lt 1000000 ]; then
  echo "❌ tar.xz 只有 $PKG_SIZE 字节, 坏包! (正常 > 5MB)"
  exit 1
fi

echo "✅ 打包完成: $REL_DIR/$TAR_NAME ($(( PKG_SIZE / 1024 )) KB)"


#压缩 tar -cvf archive.tar.xz ./资料/ 得到 archive.tar.xz
#解压 tar -xvf archive.tar.xz -C ~/       -C 解压到 ~/ 中

