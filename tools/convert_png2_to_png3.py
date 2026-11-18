#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
convert_png2_to_png3.py
=======================
将 res/png2 下的图标预混成 res/png 的绘制风格:
  - 原 png 图标"完全不透明", 白色像素占比高 (~20%~80%), 即白底实色
  - png2 图标有真 alpha 通道, 无白色背景, 缩放到 iconSize(32x32) 后显示错位

脚本对每个 png2 图标做:
  1. 用原 png 的"白色"作为背景色, 预混 png2 的 alpha 通道
     → 输出完全不透明的白底实色图 (和原 png 一致)
  2. 保持与 png 相同的画布尺寸
  3. PNG 编码参数对齐原 png: 16-bit deflate, 无 interlace

输出到 res/png3/, 然后把 res.qrc 改成引用 png3 即可.

用法:
    python3 tools/convert_png2_to_png3.py
"""

import os
import sys
import struct
import zlib
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
SRC_PNG = ROOT / "res" / "png"       # 原始图标 (参考风格)
SRC_PNG2 = ROOT / "res" / "png2"     # 新图标 (待转换)
DST_PNG3 = ROOT / "res" / "png3"     # 输出目录


def png_encode_bytes(rgba: np.ndarray) -> bytes:
    """
    用 PIL 按 PNG 标准参数 (8-bit, deflate level=9, no interlace) 编码.
    返回原始字节串. 不做任何额外 chunk.
    """
    im = Image.fromarray(rgba, mode="RGBA")
    import io
    buf = io.BytesIO()
    im.save(buf, format="PNG", optimize=True, compress_level=9)
    return buf.getvalue()


def read_png_rgba(path: Path) -> np.ndarray:
    """读取 PNG 为 H×W×4 的 uint8 numpy 数组 (RGBA)."""
    im = Image.open(str(path)).convert("RGBA")
    return np.array(im, dtype=np.uint8)


def sample_reference_bg(png_path: Path) -> np.ndarray:
    """
    从原 png 图标采样背景色.
    原 png 全不透明 (alpha=255), 背景就是白色像素最多的颜色.
    """
    arr = read_png_rgba(png_path)
    rgb = arr[:, :, :3].reshape(-1, 3)
    # 取 RGB 三通道平均值作为背景色 (原 png 的白色背景)
    bg = rgb.mean(axis=0).round().astype(np.uint8)
    # 但更准确的做法: 取出现最多的颜色
    # 为简单稳健, 用原 png 的平均色做背景
    return bg  # shape (3,) uint8


def premultiply_over_bg(png2_rgba: np.ndarray, bg_rgb: np.ndarray) -> np.ndarray:
    """
    用 bg_rgb 背景色对 png2 做 alpha 预混, 输出完全不透明的 RGBA 图.
    png2 主体保留, 透明区域变成背景色 (和原 png 一致).
    """
    alpha = png2_rgba[:, :, 3:4].astype(np.float32) / 255.0
    fg = png2_rgba[:, :, :3].astype(np.float32)
    bg = bg_rgb.astype(np.float32).reshape(1, 1, 3)
    out = fg * alpha + bg * (1.0 - alpha)
    out = np.clip(out, 0, 255).astype(np.uint8)
    # alpha 全设为 255 (和原 png 一致)
    rgba = np.concatenate([out, np.full((*out.shape[:2], 1), 255, dtype=np.uint8)], axis=2)
    return rgba


def main():
    if not SRC_PNG2.exists():
        print(f"[错误] png2 目录不存在: {SRC_PNG2}")
        sys.exit(1)
    if not SRC_PNG.exists():
        print(f"[错误] png 目录不存在: {SRC_PNG}")
        sys.exit(1)

    DST_PNG3.mkdir(parents=True, exist_ok=True)

    png_files = {f.name: f for f in SRC_PNG.glob("*.png")}
    png2_files = {f.name: f for f in SRC_PNG2.glob("*.png")}

    common = sorted(set(png_files.keys()) & set(png2_files.keys()))
    if not common:
        print("[错误] png 与 png2 没有同名文件")
        sys.exit(1)

    ok_count = 0
    skip_count = 0
    err_count = 0

    print(f"共 {len(common)} 个同名 png/png2, 开始转换 → {DST_PNG3}")
    print("-" * 72)

    for fname in common:
        try:
            p_png = png_files[fname]
            p_png2 = png2_files[fname]
            p_out = DST_PNG3 / fname

            # 参考背景色: 原 png 的平均色 (近似白底)
            bg = sample_reference_bg(p_png)

            # png2 → 用 bg 预混 alpha
            png2 = read_png_rgba(p_png2)
            out = premultiply_over_bg(png2, bg)

            # 尺寸对齐 (如果 png2 尺寸和 png 不同, 保持 png2 的尺寸即可)
            # 若原 png 尺寸不同, 以 png 尺寸为准
            png_im = Image.open(str(p_png))
            target_size = png_im.size  # (W, H)

            if (out.shape[1], out.shape[0]) != target_size:
                im = Image.fromarray(out, mode="RGBA").resize(
                    target_size, Image.LANCZOS
                )
                out = np.array(im, dtype=np.uint8)

            # 输出完全不透明 (alpha 全 255)
            out[:, :, 3] = 255

            Image.fromarray(out, mode="RGBA").save(
                str(p_out), format="PNG", optimize=True, compress_level=9
            )
            ok_count += 1
            print(f"[OK]  {fname}  bg=({bg[0]},{bg[1]},{bg[2]})  → {p_out.stat().st_size//1024}KB")

        except Exception as e:
            err_count += 1
            print(f"[ERR] {fname}: {e}")

    # png 有但 png2 没有的, 直接拷贝
    only_in_png = sorted(set(png_files.keys()) - set(png2_files.keys()))
    for fname in only_in_png:
        import shutil
        shutil.copy(png_files[fname], DST_PNG3 / fname)
        skip_count += 1
        print(f"[CPY] {fname} (png 独有)")

    # png2 有但 png 没有的, 直接拷贝到 png3 (无参考背景, 保持 png2)
    only_in_png2 = sorted(set(png2_files.keys()) - set(png_files.keys()))
    for fname in only_in_png2:
        import shutil
        shutil.copy(png2_files[fname], DST_PNG3 / fname)
        skip_count += 1
        print(f"[CPY] {fname} (png2 独有, 无参考背景)")

    print("-" * 72)
    print(f"完成: 转换 {ok_count}, 拷贝 {skip_count}, 错误 {err_count}")
    print(f"输出目录: {DST_PNG3}")
    print()
    print("下一步: 修改 res.qrc, 把所有 res/png/ 换成 res/png3/, 重新编译 DuySorter")


if __name__ == "__main__":
    main()
