#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

// 小规模演示：
// 1) 2-bit 位图演示（仅支持非负小范围值，用 vector<uint8_t> 模拟每个数的两位状态）
// 2) 分桶（hash% B）演示，保证相同数落在同一桶，逐桶统计并汇总不重复数

void demo_bitmap(const string &infile) {
    cout << "---- 2-bit 位图 演示 ----\n";
    // 假设样例数据范围在 [0, 255]，使用长度为 256 的 2-bit 位图
    const int RANGE = 256;
    vector<uint8_t> bits(RANGE,
                         0);  // 2 bit per entry stored in low bits of byte
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        return;
    }
    int x;
    while (ifs >> x) {
        if (x < 0 || x >= RANGE) continue;
        uint8_t v = bits[x] & 0x3;
        if (v == 0)
            bits[x] = (bits[x] & ~0x3) | 1;  // 00 -> 01
        else if (v == 1)
            bits[x] = (bits[x] & ~0x3) | 2;  // 01 -> 10
        // else 10 stay 10
    }
    cout << "不重复的数字: ";
    for (int i = 0; i < RANGE; ++i)
        if ((bits[i] & 0x3) == 1) cout << i << " ";
    cout << "\n";
}
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

// unique_integers.cpp
// 提供两种演示方法：
// 1) 2-bit 位图（packed）演示（小范围）
// 2) 分桶分治法演示（hash % B -> 每桶计数 -> 汇总）

void bitmap_demo(const string &infile) {
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        return;
    }
    vector<uint32_t> nums;
    int x;
    int maxv = 0;
    while (ifs >> x) {
        nums.push_back(static_cast<uint32_t>(x));
        if (x > maxv) maxv = x;
    }
    if (nums.empty()) {
        cout << "bitmap_demo: no data\n";
        return;
    }
    if (maxv > 1000000) {
        cout << "bitmap_demo: "
                "值域太大（>1e6），跳过位图演示，请使用分桶方法。\n";
        return;
    }

    size_t nbits = static_cast<size_t>(maxv) + 1;
    size_t nbytes = (nbits * 2 + 7) / 8;
    vector<uint8_t> bits(nbytes, 0);

    auto get2 = [&](uint32_t v) -> int {
        size_t pos = static_cast<size_t>(v) * 2;
        size_t idx = pos / 8;
        int off = static_cast<int>(pos % 8);
        return (bits[idx] >> off) & 0x3;
    };
    auto set2 = [&](uint32_t v, int val) {
        size_t pos = static_cast<size_t>(v) * 2;
        size_t idx = pos / 8;
        int off = static_cast<int>(pos % 8);
        bits[idx] &= static_cast<uint8_t>(~(0x3u << off));
        bits[idx] |=
            static_cast<uint8_t>((static_cast<uint8_t>(val & 0x3u) << off));
    };

    for (auto v : nums) {
        int cur = get2(v);
        if (cur == 0)
            set2(v, 1);
        else if (cur == 1)
            set2(v, 2);
    }

    cout << "bitmap_demo: values appearing exactly once:\n";
    for (uint32_t v = 0; v <= static_cast<uint32_t>(maxv); ++v)
        if (get2(v) == 1) cout << v << '\n';
}

void partition_demo(const string &infile, int B) {
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        return;
    }
    pid_t pid = getpid();
    string tmpdir = string("/tmp/unique_ints_") + to_string(pid);
    (void)mkdir(tmpdir.c_str(), 0700);
    vector<ofstream> ofs(B);
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/b" + to_string(i) + ".txt";
        ofs[i].open(fname);
    }
    int x;
    while (ifs >> x) {
        size_t h = std::hash<int>{}(x) % static_cast<size_t>(B);
        ofs[h] << x << '\n';
    }
    for (int i = 0; i < B; ++i) ofs[i].close();

    cout << "partition_demo: values appearing exactly once (merged results):\n";
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/b" + to_string(i) + ".txt";
        ifstream bifs(fname);
        unordered_map<int, int> cnt;
        int v;
        while (bifs >> v) ++cnt[v];
        for (auto &kv : cnt)
            if (kv.second == 1) cout << kv.first << '\n';
        bifs.close();
        (void)remove(fname.c_str());
    }
    (void)rmdir(tmpdir.c_str());
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "usage: unique_integers <sample_numbers.txt>\n";
        return 1;
    }
    string infile = argv[1];
    cout << "=== bitmap demo ===\n";
    bitmap_demo(infile);
    cout << "=== partition demo (B=4) ===\n";
    partition_demo(infile, 4);
    return 0;
}
