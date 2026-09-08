#!/bin/sh
#
# qtmakeApp.sh
#
# Build or clean the project

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
MYSVN_TMP=`svn info | awk 'BEGIN{FS=": "} $1=="Revision" || $1=="版本" {print $2}'`

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
rm -rf tar_app
mkdir -p tar_app

cp -rf $APP_DIR/DuySorter      tar_app/
cp -rf $APP_DIR/DuySorter.conf tar_app/
cp -rf $APP_DIR/mystart.sh     tar_app/
cp -rf $APP_DIR/env.sh         tar_app/
cp -rf $APP_DIR/myrestart.sh   tar_app/
cp -rf $APP_DIR/wifi.sh        tar_app/
cp -rf $APP_DIR/check_and_mount_usb.sh tar_app/
cp -rf $APP_DIR/norm_train     tar_app/
cp -rf $APP_DIR/i18n           tar_app/
cp -rf $APP_DIR/lib            tar_app/
cp -rf $APP_DIR/OpenVPN        tar_app/

# 打包 OpenCV 依赖库（aarch64）到 opencvlib/，库随项目分发在 src/3rdparty/opencv/arm/lib
mkdir -p tar_app/opencvlib
cp -rf src/3rdparty/opencv/arm/lib/*.so* tar_app/opencvlib/

find tar_app -name .svn -print0 | xargs -0 rm -rf 2>/dev/null

mkdir -p $REL_DIR
rm -f $REL_DIR/duyiSorter_v1.00.tar.xz
tar -cJf $REL_DIR/duyiSorter_v1.00.tar.xz tar_app
rm -rf tar_app

exit 0


#压缩 tar -cvf archive.tar.xz ./资料/ 得到 archive.tar.xz
#解压 tar -xvf archive.tar.xz -C ~/       -C 解压到 ~/ 中

