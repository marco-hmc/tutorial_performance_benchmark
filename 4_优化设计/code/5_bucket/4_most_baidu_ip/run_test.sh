#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
echo "编译 most_baidu_ip..."
g++ -std=c++17 -O2 most_baidu_ip.cpp -o most_baidu_ip
echo
./most_baidu_ip logs.txt
echo "测试完成。"
