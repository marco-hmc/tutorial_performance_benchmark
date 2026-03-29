#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++17 -O2 top100_100w.cpp -o top100_100w
echo "编译完成。运行示例..."
./top100_100w sample_numbers.txt
