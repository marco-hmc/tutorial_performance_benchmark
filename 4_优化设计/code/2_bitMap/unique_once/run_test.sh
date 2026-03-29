#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++17 -O2 unique_once.cpp -o unique_once
./unique_once -f test_input.txt -m 100
