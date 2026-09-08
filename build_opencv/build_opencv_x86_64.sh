#!/bin/bash
# ============================================================
# 本机 x86_64 编译 OpenCV 4.5.5（Ubuntu 虚拟机调试用）
# 与目标机 aarch64 版本保持一致：只编 core/imgproc/imgcodecs，
# 关掉 GDCM/OpenEXR/GDAL 等冷门后端，避免调试与发布行为不一致。
# 用法:
#   ./build_opencv_x86_64.sh [opencv源码目录]
#   不传参数时默认 ~/Desktop/opencv-4.5.5
# ============================================================
set -e

SRC_DIR=${1:-$HOME/Desktop/opencv-4.5.5}
INSTALL_DIR=$HOME/Desktop/opencv-x86_64-install
BUILD_DIR=${SRC_DIR}/build-x86_64
JOBS=${JOBS:-$(nproc)}

if [ ! -f "$SRC_DIR/CMakeLists.txt" ]; then
    echo "[错误] 找不到 OpenCV 源码: $SRC_DIR"
    exit 1
fi

echo "====================================================="
echo " SRC     = $SRC_DIR"
echo " INSTALL = $INSTALL_DIR"
echo " JOBS    = $JOBS"
echo "====================================================="

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"----

cmake .. \
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

make -j"$JOBS"
make install

echo "====================================================="
echo "[完成] 产物目录: $INSTALL_DIR/lib"
echo "[头文件] $INSTALL_DIR/include/opencv4/opencv2"
echo "====================================================="
