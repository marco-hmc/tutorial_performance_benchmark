#!/usr/bin/env bash
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
cd "$HERE"

echo "编译 find_common_urls..."
g++ -std=c++17 -O2 -o find_common_urls find_common_urls.cpp

echo
echo "---- 内存模式 (memory) 测试 ----"
./find_common_urls memory a.txt b.txt

echo
echo "---- 分桶模式 (bucket, M=3) 测试 ----"
./find_common_urls bucket 3 a.txt b.txt

echo
echo "测试完成。"
