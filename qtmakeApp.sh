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
# 最终 /opt/app 目录结构（与参考 /Users/thinklic/Desktop/opt/app 对齐）:
#   DuySorter                    ← 可执行文件
#   DuySorter.conf               ← 运行时配置
#   env.sh / mystart.sh / myrestart.sh / wifi.sh / check_and_mount_usb.sh
#   log.properties               ← unilog 日志配置
#   norm_train                   ← AI 模型
#   i18n/                        ← 翻译 .qm
#   OpenVPN/                     ← VPN 证书
#   lib/
#     ├── fonts/                 ← 所有 .ttf（DejaVu + HarmonyOS + 文泉驿 + Tahoma）
#     ├── libts.so -> libts.so.0 -> libts.so.0.10.1   ← 触摸库（符号链接）
#     ├── libts.so.0.10.1
#     ├── tun.ko
#     ├── libopencv_core.so      ← OpenCV 4.5.5
#     ├── libopencv_core.so.405
#     ├── libopencv_core.so.4.5.5
#     ├── libopencv_imgproc.so
#     ├── libopencv_imgproc.so.405
#     ├── libopencv_imgproc.so.4.5.5
#     ├── libopencv_imgcodecs.so
#     ├── libopencv_imgcodecs.so.405
#     ├── libopencv_imgcodecs.so.4.5.5
#     ├── libcrypto.so           ← OpenSSL 1.1
#     ├── libcrypto.so.1.1
#     ├── libssl.so
#     ├── libssl.so.1.1
#     ├── libunilog.so           ← 自研日志库
#     ├── libssh2.so             ← SFTP 客户端（符号链接 + 实文件）
#     └── libssh2.so.1.0.1
#   userdata/                    ← 运行时数据（不打包，由 mystart.sh 创建）

# ------ 第一步：把所有运行时 .so 统一复制到 $APP_DIR/lib/ ------
OPENCV_LIB=src/3rdparty/opencv/arm/lib
OPENSSL_LIB=src/3rdparty/openssl/arm/lib
SSH2_LIB=src/3rdparty/libssh2/arm/lib
UNILOG_LIB=src/3rdparty/unilog/arm/lib

cp -f $OPENCV_LIB/libopencv_core.so*      $APP_DIR/lib/
cp -f $OPENCV_LIB/libopencv_imgproc.so*   $APP_DIR/lib/
cp -f $OPENCV_LIB/libopencv_imgcodecs.so* $APP_DIR/lib/

cp -f $OPENSSL_LIB/libcrypto.so* $APP_DIR/lib/
cp -f $OPENSSL_LIB/libssl.so*    $APP_DIR/lib/

cp -f $SSH2_LIB/libssh2.so* $APP_DIR/lib/

cp -f $UNILOG_LIB/libunilog.so $APP_DIR/lib/

# 字体补全（HarmonyOS 系列 + DejaVuSansMono-Bold 从 res/fonts/ 合入 lib/fonts/）
cp -f res/fonts/*.ttf $APP_DIR/lib/fonts/

# libts 符号链接修复（确保 .so -> .so.0 -> .so.0.10.1 链路完整）
( cd $APP_DIR/lib && rm -f libts.so libts.so.0 && \
  ln -s libts.so.0.10.1 libts.so.0 && \
  ln -s libts.so.0      libts.so )

# 所有 .so 的 SONAME 符号链接必须显式创建
# 应用加载时用 SONAME（如 libssh2.so.1），cp glob 只拷源文件里已有的链接
( cd $APP_DIR/lib
  rm -f libssh2.so.1 && ln -sf libssh2.so.1.0.1 libssh2.so.1
  rm -f libcrypto.so.1.1 && ln -sf libcrypto.so.1.1 libcrypto.so   # 已存在则无害
  rm -f libssl.so.1.1    && ln -sf libssl.so.1.1    libssl.so      # 同上
  rm -f libopencv_core.so.405      && ln -sf libopencv_core.so.4.5.5      libopencv_core.so.405
  rm -f libopencv_imgproc.so.405   && ln -sf libopencv_imgproc.so.4.5.5   libopencv_imgproc.so.405
  rm -f libopencv_imgcodecs.so.405 && ln -sf libopencv_imgcodecs.so.4.5.5 libopencv_imgcodecs.so.405
  rm -f libopencv_core.so          && ln -sf libopencv_core.so.405      libopencv_core.so
  rm -f libopencv_imgproc.so       && ln -sf libopencv_imgproc.so.405   libopencv_imgproc.so
  rm -f libopencv_imgcodecs.so     && ln -sf libopencv_imgcodecs.so.405 libopencv_imgcodecs.so
  rm -f libcrypto.so               && ln -sf libcrypto.so.1.1 libcrypto.so
  rm -f libssl.so                  && ln -sf libssl.so.1.1    libssl.so
  rm -f libssh2.so                 && ln -sf libssh2.so.1     libssh2.so
)

# ------ 第二步：组装 tar_app/ ------
rm -rf tar_app
mkdir -p tar_app

cp -f $APP_DIR/DuySorter      tar_app/
cp -f $APP_DIR/DuySorter.conf tar_app/
cp -f $APP_DIR/mystart.sh     tar_app/
cp -f $APP_DIR/env.sh         tar_app/
cp -f $APP_DIR/myrestart.sh   tar_app/
cp -f $APP_DIR/wifi.sh        tar_app/
cp -f $APP_DIR/check_and_mount_usb.sh tar_app/
cp -f $APP_DIR/log.properties tar_app/
cp -rf $APP_DIR/norm_train    tar_app/
cp -rf $APP_DIR/i18n          tar_app/
cp -rf $APP_DIR/lib           tar_app/
cp -rf $APP_DIR/OpenVPN       tar_app/

# 确保所有启动脚本有可执行权限（源文件权限可能不是 755）
chmod +x tar_app/DuySorter tar_app/mystart.sh tar_app/myrestart.sh \
         tar_app/env.sh tar_app/wifi.sh tar_app/check_and_mount_usb.sh

# ------ 第三步：打包 tar.xz（文件名带版本号）------
find tar_app -name .svn -print0 | xargs -0 rm -rf 2>/dev/null

mkdir -p $REL_DIR
TAR_NAME="duyiSorter_${MYPRDOPT}_${MYVERSION_RELEASE}_${MYSVN_TMP}.tar.xz"
rm -f $REL_DIR/$TAR_NAME
tar -cJf $REL_DIR/$TAR_NAME tar_app
rm -rf tar_app

echo "✅ 打包完成: $REL_DIR/$TAR_NAME"
ls -lh $REL_DIR/$TAR_NAME


#压缩 tar -cvf archive.tar.xz ./资料/ 得到 archive.tar.xz
#解压 tar -xvf archive.tar.xz -C ~/       -C 解压到 ~/ 中

