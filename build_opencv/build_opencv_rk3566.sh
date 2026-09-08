#!/bin/bash
# ============================================================
# 交叉编译 OpenCV 4.5.5 for RK3566 (aarch64)
# 与 DuySorter 工程解耦，可放在任意目录运行
# 用法:
#   ./build_opencv_rk3566.sh [opencv源码目录]
#   不传参数时默认 ~/Desktop/opencv-4.5.5
# ============================================================
set -e

# ---------------- 交叉编译 GCC 工具链（核心配置，按需修改） ----------------
TOOLCHAIN_DIR=/home/mcgs/sdk/rk3568-arm64-Qt5.15.15   # SDK 根目录
SYSROOT=${TOOLCHAIN_DIR}/sysroot                       # 交叉 sysroot
CC=${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc          # C 编译器
CXX=${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++         # C++ 编译器
READELF=${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-readelf # 依赖校验工具

# ---------------- 源码 / 安装 / 编译目录 ----------------
SRC_DIR=${1:-$HOME/Desktop/opencv-4.5.5}
INSTALL_DIR=$HOME/Desktop/opencv-rk3566-install
BUILD_DIR=${SRC_DIR}/build-rk3566
JOBS=${JOBS:-$(nproc)}                                 # 并行数，可用 JOBS=4 覆盖

# ---------------- 校验工具链是否存在 ----------------
for f in "$CC" "$CXX" "$READELF"; do
    if [ ! -x "$f" ]; then
        echo "[错误] 找不到工具: $f"
        exit 1
    fi
done

# ---------------- 校验源码目录 ----------------
if [ ! -f "$SRC_DIR/CMakeLists.txt" ]; then
    echo "[错误] 源码目录不存在或不是 OpenCV 源码: $SRC_DIR"
    exit 1
fi

echo "====================================================="
echo " 工具链:"
echo "   CC       = $CC"
echo "   CXX      = $CXX"
echo "   SYSROOT  = $SYSROOT"
echo "   SRC      = $SRC_DIR"
echo "   INSTALL  = $INSTALL_DIR"
echo "   JOBS     = $JOBS"
echo "====================================================="

# ---------------- 生成 toolchain.cmake ----------------
TOOLCHAIN_FILE=${SRC_DIR}/toolchain-rk3566.cmake
cat > "$TOOLCHAIN_FILE" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_SYSROOT ${SYSROOT})
set(CMAKE_C_COMPILER   ${CC})
set(CMAKE_CXX_COMPILER ${CXX})

# 关键：显式把 --sysroot 传给编译器，确保 cmake 各类 try_compile 检测能定位目标头文件
set(CMAKE_C_FLAGS_INIT   "--sysroot=${SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${SYSROOT}")

# 链接阶段也要带 --sysroot + 显式 -L，否则 ld 找不到 sysroot 里的 libdl/libpthread/librt
set(CMAKE_EXE_LINKER_FLAGS_INIT    "--sysroot=${SYSROOT} -L${SYSROOT}/usr/lib/aarch64-linux-gnu -L${SYSROOT}/lib/aarch64-linux-gnu -L${SYSROOT}/usr/lib")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--sysroot=${SYSROOT} -L${SYSROOT}/usr/lib/aarch64-linux-gnu -L${SYSROOT}/lib/aarch64-linux-gnu -L${SYSROOT}/usr/lib")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "--sysroot=${SYSROOT} -L${SYSROOT}/usr/lib/aarch64-linux-gnu -L${SYSROOT}/lib/aarch64-linux-gnu -L${SYSROOT}/usr/lib")

# 交叉编译时 try_run 无法在宿主机运行 aarch64 程序，改为只编译静态库来推断类型大小/字节序
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH ${SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
echo "[OK] 已生成 toolchain: $TOOLCHAIN_FILE"

# ---------------- cmake 配置 ----------------
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_LIST=core,imgproc,imgcodecs \
  -DWITH_GDCM=OFF -DWITH_OPENEXR=OFF -DWITH_GDAL=OFF \
  -DWITH_JASPER=OFF -DWITH_OPENJPEG=OFF \
  -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF \
  -DWITH_IPP=OFF -DWITH_OPENCL=OFF -DWITH_TBB=OFF -DWITH_1394=OFF \
  -DWITH_JPEG=ON -DWITH_PNG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF \
  -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_opencv_apps=OFF

# ---------------- 编译 & 安装 ----------------
echo "[开始编译] make -j$JOBS"
make -j"$JOBS"
make install

# ---------------- 校验依赖（应只剩 jpeg/zlib） ----------------
echo "====================================================="
echo "[完成] 产物目录: $INSTALL_DIR/lib"
echo "[依赖校验] libopencv_imgcodecs 的 NEEDED 如下:"
"$READELF" -d "$INSTALL_DIR/lib/libopencv_imgcodecs.so.405" 2>/dev/null | grep NEEDED \
  || "$READELF" -d "$INSTALL_DIR/lib/libopencv_imgcodecs.so" 2>/dev/null | grep NEEDED \
  || echo "  未找到 imgcodecs 库，请检查 $INSTALL_DIR/lib"
echo "====================================================="
