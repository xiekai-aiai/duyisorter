#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
make_train2.py
基于 make_train.py，但采样方式从「手动采样点」改为「自动前景提取」：

1. 蓝色通道阈值提取前景 mask（背景偏蓝 B 高，物料 B 低）
2. 排除图像左右边缘像素（边缘可能有截断/干扰，不作为样本）
3. 腐蚀处理：收缩前景，去掉物料边缘像素，只保留物料内部像素
4. findContours 加轮廓：提取连通域，过滤过小/过大面积的噪声
5. 对轮廓内的前景像素提取 9 维特征，生成 LIBSVM train 文件

特征顺序与 make_train.py 一致：
    label 1:r 2:g 3:b 4:rg 5:rb 6:gb 7:rr 8:gg 9:bb

依赖：pip install opencv-python numpy
"""

import os
import sys

import cv2
import numpy as np


def build_train(images, labels, output_file,
                threshold=200, erode_iter=1, min_area=0, max_area=2000, border=3):
    labels = [int(x) for x in labels]

    if len(images) != len(labels):
        sys.exit("错误：images/labels 数量不一致：%d/%d"
                 % (len(images), len(labels)))

    total = 0
    with open(output_file, "w", encoding="utf-8") as out:
        for img_path, label in zip(images, labels):
            if label not in (-1, 1):
                sys.exit("错误：非法标签 %d（只允许 -1 或 1）" % label)

            img = cv2.imread(img_path)  # BGR
            if img is None:
                sys.exit("图片读取失败: %s" % img_path)

            # 1. 蓝色通道阈值提取前景（背景偏蓝 B 高，物料 B 低）
            mask = (img[:, :, 0] < threshold).astype(np.uint8) * 255

            # 2. 排除图像左右边缘像素（边缘可能有截断/干扰，不作为样本）
            if border > 0:
                h, w = mask.shape[:2]
                mask[:, :border] = 0
                mask[:, w - border:] = 0

            # 3. 腐蚀：收缩前景，去掉物料边缘像素，只保留物料内部像素
            if erode_iter > 0:
                kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
                mask = cv2.erode(mask, kernel, iterations=erode_iter)

            # 4. 加轮廓：提取连通域，过滤过小/过大面积的噪声
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL,
                                           cv2.CHAIN_APPROX_SIMPLE)
            cleaned = np.zeros_like(mask)
            for c in contours:
                area = cv2.contourArea(c)
                print(area)
                if area < min_area:
                    continue
                if max_area > 0 and area >= max_area:
                    continue
                cv2.drawContours(cleaned, [c], -1, 255, -1)
            mask = cleaned

            # 5. 提取前景像素的 9 维特征
            ys, xs = np.where(mask > 0)
            n_fg = len(ys)
            if n_fg == 0:
                print("  警告: %s 无前景像素，跳过" % img_path)
                continue

            b = img[ys, xs, 0].astype(np.int64)
            g = img[ys, xs, 1].astype(np.int64)
            r = img[ys, xs, 2].astype(np.int64)

            for i in range(n_fg):
                out.write("%d\t1:%d\t2:%d\t3:%d\t4:%d\t5:%d\t6:%d\t7:%d\t8:%d\t9:%d\n"
                          % (label, r[i], g[i], b[i],
                             r[i] * g[i], r[i] * b[i], g[i] * b[i],
                             r[i] * r[i], g[i] * g[i], b[i] * b[i]))
                total += 1

            # 可视化：把前景像素涂成红色保存
            vis = img.copy()
            vis[mask > 0] = (0, 0, 255)
            base, _ = os.path.splitext(img_path)
            cv2.imwrite(base + "_mask.png", vis)
            print("  %s: 前景 %d 像素" % (img_path, n_fg))

    print("完成：共 %d 张图，写入 %d 条样本 -> %s"
          % (len(images), total, output_file))


def main():
    images = ['../data/0.png', '../data/1.png', '../data/2.png']
    labels = [-1, 1, 1]
    output = '../data/train2.txt'
    build_train(images, labels, output)


if __name__ == "__main__":
    main()
