#!/bin/bash
set -e

# ===== 部署 SVM 训练程序到板端 =====
# 用法:
#   ./deploy.sh                 # 拷贝 svm_train 和测试数据 data/train.txt
#   ./deploy.sh svm_train svm.h train.cpp   # 拷贝指定文件
#   HOST=... USER=... PASS=... ./deploy.sh   # 用环境变量覆盖默认连接信息

cd "$(dirname "$0")"

HOST="${HOST:-192.168.0.200}"
USER="${USER:-root}"
PASS="${PASS:-flexem}"
REMOTE_DIR="${REMOTE_DIR:-/userdata/svm}"

# 要拷贝的文件，默认 svm_train + 测试数据 train.txt
FILES=("$@")
if [ ${#FILES[@]} -eq 0 ]; then
    FILES=(svm_train ../data/train.txt)
fi

if ! command -v sshpass >/dev/null 2>&1; then
    echo "错误: 需要 sshpass，请先安装:" >&2
    echo "  Ubuntu: sudo apt install -y sshpass" >&2
    echo "  macOS:  brew install sshpass" >&2
    exit 1
fi

for f in "${FILES[@]}"; do
    if [ ! -f "$f" ]; then
        echo "错误: 找不到文件 $f（请先 ./build.sh 交叉编译）" >&2
        exit 1
    fi
done

export SSHPASS="$PASS"
SSHOPT="-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o LogLevel=ERROR"

echo "==> $USER@$HOST 创建目录 $REMOTE_DIR"
sshpass -e ssh $SSHOPT "$USER@$HOST" "mkdir -p $REMOTE_DIR"

for f in "${FILES[@]}"; do
    base=$(basename "$f")
    echo "==> 拷贝 $f -> $REMOTE_DIR/$base"
    # 板端无 scp，用 ssh + cat 通过 stdin 传输（二进制安全）
    sshpass -e ssh $SSHOPT "$USER@$HOST" "cat > $REMOTE_DIR/$base" < "$f"
    # 无扩展名的文件视为可执行文件，设置执行权限
    case "$base" in
        *.*) ;;
        *) sshpass -e ssh $SSHOPT "$USER@$HOST" "chmod +x $REMOTE_DIR/$base" ;;
    esac
done

echo "完成。"
