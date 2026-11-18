#!/bin/bash
NUM_PICS="${1:-1000}"
WAIT_SEC="${2:-15}"
cd "$(dirname "$0")"
source /opt/app/env.sh
unset QT_QPA_PLATFORM
./test_arm 7 "$NUM_PICS" "$WAIT_SEC" 2>&1