#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

// 演示位图（小范围模拟）和逐位分治（按位切分文件并递归查找）

// 小范围位图示例：range <= MAXV
void bitmap_check(const string &infile, unsigned int query, unsigned int MAXV) {
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开: " << infile << '\n';
        return;
    }
    if (query > MAXV) {
        cout << "查询值超出模拟位图范围（0.." << MAXV << ")，无法判断\n";
        return;
    }
    vector<uint8_t> bits(MAXV + 1, 0);
    unsigned int x;
    while (ifs >> x) {
        if (x <= MAXV) bits[x] = 1;
    }
    cout << "---- Bitmap 模拟检查 ----\n";
    cout << query << (bits[query] ? " 存在" : " 不存在") << '\n';
}

// 逐位分治的演示（在内存中实现，按位分组并递归查找）
bool bitwise_partition_check(vector<unsigned int> &arr, unsigned int query,
                             int bit) {
    if (arr.empty()) return false;
    if (bit < 0) return false;
    vector<unsigned int> group;
    unsigned int mask = 1u << bit;
    int want = (query & mask) ? 1 : 0;
    for (auto v : arr) {
        int b = (v & mask) ? 1 : 0;
        if (b == want) group.push_back(v);
    }
    if (group.empty()) return false;
    // 如果只剩少量元素，直接线性查找
    if ((int)group.size() <= 16) {
        for (auto v : group)
            if (v == query) return true;
        return false;
    }
    return bitwise_partition_check(group, query, bit - 1);
}

int main(int argc, char **argv) {
    string infile = "sample_presence.txt";
    if (argc >= 2) infile = argv[1];
    unsigned int query = 100;
    if (argc >= 3) query = (unsigned int)stoul(argv[2]);

    cout << "演示位图（小范围）与逐位分治查找（小规模内存示例）\n";
    bitmap_check(infile, query, 1000);

    // 逐位分治演示：读入全部数据（仅用于示例）
    vector<unsigned int> arr;
    ifstream ifs(infile);
    unsigned int v;
    while (ifs >> v) arr.push_back(v);
    int maxbit = 0;
    unsigned int mx = 0;
    for (auto a : arr) mx = max(mx, a);
    while ((1u << maxbit) <= mx) ++maxbit;
    if (maxbit > 0) --maxbit;
    bool found = bitwise_partition_check(arr, query, maxbit);
    cout << "---- 逐位分治查找（演示）----\n";
    cout << query << (found ? " 存在" : " 不存在") << '\n';
    return 0;
}
