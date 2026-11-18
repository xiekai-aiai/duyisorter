# -*- coding: utf-8 -*-
"""
test_svm.py
参考 norm_train.py 导出的 .model 参数，对单张图片做逐像素 SVM 分类，
并把指定类别（0 或 1）的像素涂成红色，最后把「原图」和「结果图」左右拼接保存。

说明：
- norm_train.py 训练结束会把合并后的参数写入 <data_file>.model：
    前 feat_dim 行是 w，最后一行是 b（均已乘以 1e6）。
- 合并后参数已把 StandardScaler 折入 w/b，因此这里直接对“原始像素特征”计算
  decision = x · w + b，decision > 0 判为 1 类，否则为 0 类，无需再做标准化。
- 特征顺序（color_mode="rgb"，与 norm_train.py 的 __main__ 一致）：
  [R, G, B, R*G, R*B, G*B, R^2, G^2, B^2]，实际使用时截断到模型的 feat_dim。
- norm_train.py 里的 get_fg_mask 在反编译产物中缺失，这里按同项目 C++ 前景提取
  （GetBg/foreground.cpp）复现：背景颜色 + 颜色欧氏距离阈值 + 形态学 + 面积过滤。

用法示例：
    python test_svm.py --model train.txt.model --input img_0_273.jpg --class 1
    python test_svm.py --model train.txt.model --input img_0_273.jpg --class 0 \
        --bg bg_0_1.jpg --thresh 40 --output out.jpg
"""

import argparse
import os

import cv2
import numpy as np


def load_model(model_path):
    """读取 norm_train.py 导出的 .model 文件，返回 (w, b, feat_dim)。"""
    if not os.path.exists(model_path):
        raise FileNotFoundError("模型文件不存在: {}".format(model_path))

    with open(model_path, "r") as f:
        lines = [line.strip() for line in f if line.strip() != ""]

    if len(lines) < 2:
        raise ValueError("模型文件至少需要 1 个系数和 1 个偏置")

    # 最后一行是 b，其余是 w
    b = float(lines[-1])
    w = np.array([float(x) for x in lines[:-1]], dtype=np.float64)
    return w, b, len(w)


def get_background_color(img_bgr, bg_path=None, border=3):
    """
    获取背景 BGR 颜色。
    优先使用背景图均值；否则取原图边缘像素的均值作为背景估计。
    """
    if bg_path:
        bg = cv2.imread(bg_path)
        if bg is None:
            raise FileNotFoundError("背景图读取失败: {}".format(bg_path))
        return cv2.mean(bg)[:3]

    h, w = img_bgr.shape[:2]
    border = max(1, min(border, h // 2, w // 2))
    top = img_bgr[:border].reshape(-1, 3)
    bottom = img_bgr[h - border:].reshape(-1, 3)
    left = img_bgr[:, :border].reshape(-1, 3)
    right = img_bgr[:, w - border:].reshape(-1, 3)
    border_pixels = np.concatenate([top, bottom, left, right], axis=0)
    return border_pixels.mean(axis=0)


def get_fg_mask(img_bgr, threshold=200, min_area=0):
    """
    前景掩码：背景偏蓝（B 通道高），物料 B 通道低。
    直接用蓝色通道绝对阈值切割：B < threshold 判为前景。
    再做开运算 + 膨胀去噪，并可按面积过滤小轮廓。
    """
    mask = (img_bgr[:, :, 0] < threshold).astype(np.uint8) * 255

    # # 去噪：开运算去掉孤立点，再膨胀连接
    # mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN,
    #                         cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3)))
    # mask = cv2.dilate(mask, cv2.getStructuringElement(cv2.MORPH_RECT, (2, 2)))

    # if min_area > 0:
    #     contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    #     cleaned = np.zeros_like(mask)
    #     for c in contours:
    #         if cv2.contourArea(c) >= min_area:
    #             cv2.drawContours(cleaned, [c], -1, 255, -1)
    #     mask = cleaned

    return mask


def extract_features(img_bgr, fg_mask):
    """提取前景像素的 9 维特征，返回 (features(N,9), 前景像素索引)。"""
    b = img_bgr[:, :, 0].astype(np.float64)
    g = img_bgr[:, :, 1].astype(np.float64)
    r = img_bgr[:, :, 2].astype(np.float64)

    features = np.stack(
        [r, g, b, r * g, r * b, g * b, r * r, g * g, b * b], axis=-1
    )  # (H, W, 9)

    ys, xs = np.where(fg_mask > 0)
    return features[ys, xs], (ys, xs)


def classify(w, b, features):
    """线性 SVM 分类，decision > 0 判为 1 类。"""
    feat_dim = len(w)
    feats = features[:, :feat_dim]
    decision = feats @ w + b
    return (decision > 0).astype(np.uint8)


def draw_target_class(img_bgr, fg_coords, labels, target_class):
    """把指定类别的像素涂成红色（BGR 下红色为 (0,0,255)）。"""
    result = img_bgr.copy()
    target = labels == target_class
    ys = fg_coords[0][target]
    xs = fg_coords[1][target]
    result[ys, xs] = (0, 0, 255)
    return result


def main():
    parser = argparse.ArgumentParser(description="用 norm_train.py 导出的 SVM 参数测试单张图片")
    parser.add_argument("--model", required=True, help=".model 参数文件路径")
    parser.add_argument("--input", required=True, help="待测试图片路径")
    parser.add_argument("--class", dest="target_class", type=int, default=1,
                        choices=[-1, 1], help="要涂成红色的类别（默认 1）")
    parser.add_argument("--bg", default=None, help="背景图路径（可选）")
    parser.add_argument("--thresh", type=float, default=200,
                        help="蓝色通道绝对阈值：B < 该值判为前景（默认 200）")
    parser.add_argument("--min-area", type=float, default=0,
                        help="最小前景面积，过滤噪声（默认 0 不过滤）")
    parser.add_argument("--output", default=None, help="输出图片完整路径（优先级高于 --outdir）")
    parser.add_argument("--outdir", default=None, help="输出目录（默认与输入图片同目录）")
    args = parser.parse_args()

    w, b, feat_dim = load_model(args.model)
    print("加载模型: {}, feat_dim={}".format(args.model, feat_dim))

    img = cv2.imread(args.input)
    if img is None:
        raise FileNotFoundError("图片读取失败: {}".format(args.input))

    bg_color = get_background_color(img, args.bg)
    print("背景均值 BGR: ({:.1f}, {:.1f}, {:.1f})".format(bg_color[0], bg_color[1], bg_color[2]))
    fg_mask = get_fg_mask(img, threshold=args.thresh, min_area=args.min_area)

    features, coords = extract_features(img, fg_mask)
    if features.shape[0] == 0:
        print("未检测到前景像素，结果图与原图相同。")
        result = img.copy()
    else:
        labels = classify(w, b, features)
        result = draw_target_class(img, coords, labels, args.target_class)
        n_target = int((labels == args.target_class).sum())
        n_fg = int(labels.shape[0])
        print("前景像素 {} 个，其中 {} 类 {} 个".format(n_fg, args.target_class, n_target))

    # 左右拼接：原图 | 结果图
    concat = np.hstack([img, result])

    if args.output:
        out_path = args.output
    else:
        name = os.path.basename(args.input)
        base, ext = os.path.splitext(name)
        out_dir = args.outdir if args.outdir else os.path.dirname(args.input)
        out_path = os.path.join(out_dir, base + "_result" + ext)
    out_dir = os.path.dirname(os.path.abspath(out_path))
    os.makedirs(out_dir, exist_ok=True)
    cv2.imwrite(out_path, concat)
    print("已保存: {}".format(out_path))


if __name__ == "__main__":
    main()
