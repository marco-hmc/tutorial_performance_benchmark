#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++17 -O2 external_sort.cpp -o external_sort
echo "编译完成。运行示例..."
./external_sort sample_large.txt sorted_out.txt
echo "输出前 50 行（示例）:"
head -n 50 sorted_out.txt || true
