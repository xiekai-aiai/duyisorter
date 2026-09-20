#!/bin/sh
#
# qtmakeApp.sh
#
# Build or clean the project

path_cur=$(cd `dirname $0`; pwd)
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
		make distclean;
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

# debug mode: exit
#if [ $MYMODEOPT = "debug" ]
#then
#	exit 0
#fi


# release mode: 打包升级包
# 包顶层为 tar_app/，供目标机 mystart.sh 解压后 "mv tar_app/* ." 使用
#

# ------ 第一步：把所有运行时 .so 统一复制到 $APP_DIR/lib/ ------

libdirs=`ls $lib_base_path`
for lib in $libdirs
do
  if [ -d "$lib_base_path/$lib/arm/lib" ]; then
     cp $lib_base_path/$lib/arm/lib/* $path_cur/$APP_DIR/lib/
  fi
done

# ------ 第二步：组装 tar_app/ ------
rm -rf tar_app
mkdir -p tar_app/userdata/db

cp -rf $APP_DIR/DuySorter      tar_app/
cp -rf $APP_DIR/DuySorter.conf tar_app/
cp -rf $APP_DIR/mystart.sh     tar_app/
cp -rf $APP_DIR/env.sh         tar_app/
cp -rf $APP_DIR/myrestart.sh   tar_app/
cp -rf $APP_DIR/wifi.sh        tar_app/
cp -rf $APP_DIR/check_and_mount_usb.sh tar_app/
cp -rf $APP_DIR/log.properties tar_app/
cp -rf $APP_DIR/norm_train     tar_app/
cp -rf $APP_DIR/i18n           tar_app/
cp -rf $APP_DIR/lib            tar_app/
cp -rf $APP_DIR/OpenVPN        tar_app/
cp -rf $APP_DIR/db/*           tar_app/userdata/db/


# 确保所有启动脚本有可执行权限（源文件权限可能不是 755）
chmod +x tar_app/DuySorter tar_app/mystart.sh tar_app/myrestart.sh \
         tar_app/env.sh tar_app/wifi.sh tar_app/check_and_mount_usb.sh

mkdir -p $REL_DIR
TAR_NAME="duyiSorter_${MYPRDOPT}_${MYVERSION_RELEASE}_${MYSVN_TMP}.tar.xz"
rm -f $REL_DIR/$TAR_NAME
tar -cJf $REL_DIR/$TAR_NAME tar_app
rm -rf tar_app

echo "✅ 打包完成: $REL_DIR/$TAR_NAME"
ls -lh $REL_DIR/$TAR_NAME


#压缩 tar -cvf archive.tar.xz ./资料/ 得到 archive.tar.xz
#解压 tar -xvf archive.tar.xz -C ~/       -C 解压到 ~/ 中

