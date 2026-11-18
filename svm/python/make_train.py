#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
仿照 AIMainWidget::saveDataFromImg()（RGB 9 维交叉项 / MODE_MULTIPLE 多分类模式），
把「图片 + 采样点坐标 + 标签」转换成 LIBSVM 格式的 train 文件。

输入：三个列表，一一对应
    images  : 图片路径列表
    labels  : 标签列表，-1 = 好料，1 = 坏料
    samples : 采样点坐标文件路径列表（对应上面同一张图片）

采样点坐标文件：每行两个整数 "x y"，表示该图上一个采样像素的坐标。
    （好图只会在其采样点文件里放好料的点，坏图只放坏料的点）

输出：train 文件（LIBSVM 稀疏格式，每行 9 维特征：
    label 1:r 2:g 3:b 4:rg 5:rb 6:gb 7:rr 8:gg 9:bb）

依赖：pip install pillow
"""

#import argparse
import sys

from PIL import Image, ImageDraw


def build_train(images, labels, samples, output_file):
    labels = [int(x) for x in labels]

    if not (len(images) == len(labels) == len(samples)):
        sys.exit("错误：images/labels/samples 数量不一致："
                 "%d/%d/%d" % (len(images), len(labels), len(samples)))

    total = 0
    with open(output_file, "w", encoding="utf-8") as out:
        for img_path, label, sample_path in zip(images, labels, samples):
            if label not in (-1, 1):
                sys.exit("错误：非法标签 %d（只允许 -1 或 1）" % label)

            img = Image.open(img_path).convert("RGB")
            w, h = img.size

            points = []
            with open(sample_path, encoding="utf-8") as sp:
                for line in sp:
                    line = line.strip()
                    if not line:
                        continue
                    # 用 : 分割
                    parts = line.split(':')
                    if len(parts) < 2:
                        continue
                    try:
                        x, y = int(parts[0]), int(parts[1])
                    except ValueError:
                        continue
                    # 越界点跳过（sample 可能来自不同尺寸的图）
                    if x < 0 or y < 0 or x >= w or y >= h:
                        continue

                    points.append((x, y))
                    r, g, b = img.getpixel((x, y))
                    out.write("%d\t1:%d\t2:%d\t3:%d\t4:%d\t5:%d\t6:%d\t7:%d\t8:%d\t9:%d\n"
                              % (label, r, g, b,
                                 r * g, r * b, g * b,
                                 r * r, g * g, b * b))
                    total += 1

            # 把采样点画到图上并保存为 sampleN.png
            draw_img = img.copy()
            draw = ImageDraw.Draw(draw_img)
            for (x, y) in points:
                draw.ellipse([x - 2, y - 2, x + 2, y + 2], fill=(255, 0, 0))
            draw_img.save(sample_path + ".png")

    print("完成：共 %d 张图，写入 %d 条样本 -> %s"
          % (len(images), total, output_file))


def main():
    images = ['./data/0.png','./data/1.png','./data/2.png']
    labels = [-1,1,1]
    samples = ['./data/sample0','./data/sample1','./data/sample2']
    output = './data/train.txt'
    build_train(images, labels, samples, output)


if __name__ == "__main__":
    main()
