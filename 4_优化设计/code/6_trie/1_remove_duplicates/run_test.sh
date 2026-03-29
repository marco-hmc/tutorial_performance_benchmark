#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
echo "编译 remove_duplicates..."
g++ -std=c++17 -O2 remove_duplicates.cpp -o remove_duplicates
echo
./remove_duplicates sample_strings.txt
echo "测试完成。"
