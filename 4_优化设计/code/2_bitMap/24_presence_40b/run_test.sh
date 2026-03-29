#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++17 -O2 presence_40b.cpp -o presence_40b
echo "编译完成。运行示例（查询 100）..."
./presence_40b sample_presence.txt 100
