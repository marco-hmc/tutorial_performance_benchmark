#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Simple demo of 2-bit bitmap approach for "numbers that occur exactly once".
// For production on full 32-bit space, memory allocation would be ~1 GiB.

int main(int argc, char** argv) {
    std::string filename = "test_input.txt";
    uint64_t maxValue = 1000;  // default small range for testing

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "-f" || a == "--file") && i + 1 < argc) {
            filename = argv[++i];
        } else if ((a == "-m" || a == "--max") && i + 1 < argc) {
            maxValue = std::stoull(argv[++i]);
        }
    }

    // allocate 2 bits per value: pack into bytes -> need ceil(2*maxValue/8) bytes
    uint64_t bits = maxValue * 2;
    uint64_t bytes = (bits + 7) / 8;
    std::vector<uint8_t> bitmap(bytes);  // initialized to 0

    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Failed to open input file: " << filename << "\n";
        return 2;
    }

    uint64_t val;
    while (ifs >> val) {
        if (val >= maxValue) continue;  // skip out-of-range values in this demo
        uint64_t bitIndex = val * 2;
        uint64_t byteIndex = bitIndex / 8;
        uint8_t bitOffset = bitIndex % 8;
        uint8_t mask = 0x3 << bitOffset;  // two bits mask
        uint8_t cur = (bitmap[byteIndex] & mask) >> bitOffset;
        if (cur == 0) {
            // 00 -> 01
            bitmap[byteIndex] =
                (bitmap[byteIndex] & ~mask) | (0x1 << bitOffset);
        } else if (cur == 1) {
            // 01 -> 10
            bitmap[byteIndex] =
                (bitmap[byteIndex] & ~mask) | (0x2 << bitOffset);
        } else {
            // 10 -> stay 10; 11 unused
        }
    }

    // output values that are exactly 01
    for (uint64_t x = 0; x < maxValue; ++x) {
        uint64_t bitIndex = x * 2;
        uint64_t byteIndex = bitIndex / 8;
        uint8_t bitOffset = bitIndex % 8;
        uint8_t mask = 0x3 << bitOffset;
        uint8_t cur = (bitmap[byteIndex] & mask) >> bitOffset;
        if (cur == 1) std::cout << x << "\n";
    }

    return 0;
}
