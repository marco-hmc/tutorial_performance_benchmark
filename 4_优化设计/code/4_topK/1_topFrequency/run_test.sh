#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
echo "编译 top100_words..."
g++ -std=c++17 -O2 top100_words.cpp -o top100_words
echo
./top100_words sample.txt

echo "测试完成。"
