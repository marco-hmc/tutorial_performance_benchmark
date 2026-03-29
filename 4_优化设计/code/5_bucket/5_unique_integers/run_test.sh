#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
echo "编译 unique_integers..."
g++ -std=c++17 -O2 unique_integers.cpp -o unique_integers
echo
./unique_integers sample_numbers.txt
echo "测试完成。"
#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
echo "编译 unique_integers..."
g++ -std=c++17 -O2 unique_integers.cpp -o unique_integers
echo
./unique_integers sample_numbers.txt
echo "测试完成。"
