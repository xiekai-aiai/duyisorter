#!/bin/bash
# 测试 SVM：对三张测试图片（0/1/2.png）分类并把指定类别像素涂红后与原图拼接保存
# 用法：./test.sh [模型文件] [类别] [输出目录]
#   默认：模型 ../data/train2.txt.model，类别 1，输出目录 ../data/train2_result
cd "$(dirname "$0")"

MODEL="${1:-../data/train2.txt.model}"
CLASS="${2:-1}"
OUTDIR="${3:-../data/train2_result}"

for INPUT in ../data/0.png ../data/1.png ../data/2.png; do
    echo "===== 测试 $INPUT ====="
    python3 test_svm.py --model "$MODEL" --input "$INPUT" --class "$CLASS" --outdir "$OUTDIR"
done
