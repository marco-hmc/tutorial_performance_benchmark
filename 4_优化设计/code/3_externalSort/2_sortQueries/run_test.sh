#!/usr/bin/env bash
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
cd "$HERE"

echo "编译 sort_queries..."
g++ -std=c++17 -O2 -o sort_queries sort_queries.cpp

echo
echo "---- memory 模式 Top-5 测试（合并 10 个文件） ----"
./sort_queries memory 5 file0.txt file1.txt file2.txt file3.txt file4.txt file5.txt file6.txt file7.txt file8.txt file9.txt

echo
echo "---- bucket 模式 M=4, Top-5 测试 ----"
./sort_queries bucket 4 5 file0.txt file1.txt file2.txt file3.txt file4.txt file5.txt file6.txt file7.txt file8.txt file9.txt

echo
echo "测试完成。"
