/*
  题目：使用 2-bit 位图来标记整数出现次数（00=未出现,01=出现一次,10=多次）。
  说明：此示例用于展示位图概念，输入为范围较小的非负整数，输出只出现一次的数。
  用法：./two_bit_bitmap numbers_input.txt
*/
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " numbers.txt\n";
        return 1;
    }
    string file = argv[1];
    ifstream fin(file);
    vector<int> nums;
    int x, maxv = 0;
    while (fin >> x) {
        nums.push_back(x);
        if (x > maxv) maxv = x;
    }
    int n = maxv + 1;
    vector<uint8_t> bits((n + 3) / 4,
                         0);  // 2 bits per number => pack 4 numbers per byte
    auto get2 = [&](int idx) -> int {
        int byte = idx / 4;
        int off = (idx % 4) * 2;
        return (bits[byte] >> off) & 0x3;
    };
    auto set2 = [&](int idx, int val) {
        int byte = idx / 4;
        int off = (idx % 4) * 2;
        bits[byte] &= ~(0x3 << off);
        bits[byte] |= ((val & 0x3) << off);
    };
    for (int v : nums) {
        int cur = get2(v);
        if (cur == 0)
            set2(v, 1);
        else if (cur == 1)
            set2(v, 2);
        else { /* already >=2 */
        }
    }
    // output numbers that appeared exactly once
    for (int i = 0; i <= maxv; i++)
        if (get2(i) == 1) cout << i << "\n";
    return 0;
}
