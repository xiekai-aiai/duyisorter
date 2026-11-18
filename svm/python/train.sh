#!/bin/bash
# 训练 SVM：读取 ../data/train.txt，输出 ../data/train.txt.model
cd "$(dirname "$0")"
python3 norm_train.py ../data/train2.txt
