#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++17 -O2 unique_25e6.cpp -o unique_25e6
echo "编译完成。运行示例..."
./unique_25e6 sample_numbers.txt
