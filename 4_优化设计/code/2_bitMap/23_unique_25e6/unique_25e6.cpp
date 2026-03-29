#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

// 演示 2-bit 位图与分桶法。注意：真实 2^32 × 2bit 位图在本机不可行，本示例用小范围模拟。

// 2-bit 位图模拟（range <= 1e6）
void bitmap_demo(const string &infile, int MAXV) {
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开: " << infile << '\n';
        return;
    }
    // 使用 vector<uint8_t> 每个元素存 2 bit（用一个字节保存4个数的状态），为了简单使用 vector<uint8_t> 每个数一个字节
    if (MAXV <= 0) {
        cerr << "MAXV 必须 >0\n";
        return;
    }
    vector<uint8_t> state(MAXV + 1, 0);
    int x;
    while (ifs >> x) {
        if (x < 0 || x > MAXV) continue;  // 跳过超出模拟范围的数
        if (state[x] == 0)
            state[x] = 1;  // 01 表示出现一次
        else if (state[x] == 1)
            state[x] = 2;  // 10 表示多次
    }
    cout << "---- Bitmap 模拟：出现一次的数 ----\n";
    for (int i = 0; i <= MAXV; ++i)
        if (state[i] == 1) cout << i << ' ';
    cout << '\n';
}

// 分桶法：按 hash% B 分桶，逐桶统计并输出只出现一次的元素
void bucket_demo(const string &infile, int B) {
    pid_t pid = getpid();
    string tmpdir = "/tmp/unique25e6_" + to_string(pid);
    mkdir(tmpdir.c_str(), 0700);

    vector<ofstream> outs(B);
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/bucket_" + to_string(i) + ".txt";
        outs[i].open(fname);
    }
    ifstream ifs(infile);
    int x;
    while (ifs >> x) {
        int idx = (std::hash<int>()(x) & 0x7fffffff) % B;
        outs[idx] << x << '\n';
    }
    for (int i = 0; i < B; ++i) outs[i].close();

    cout << "---- Bucket 模式：每桶统计，输出出现一次的数 ----\n";
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/bucket_" + to_string(i) + ".txt";
        ifstream bis(fname);
        unordered_map<int, int> cnt;
        int v;
        while (bis >> v) ++cnt[v];
        for (auto &p : cnt)
            if (p.second == 1) cout << p.first << ' ';
        bis.close();
        remove(fname.c_str());
    }
    rmdir(tmpdir.c_str());
    cout << '\n';
}

int main(int argc, char **argv) {
    string infile = "sample_numbers.txt";
    if (argc >= 2) infile = argv[1];

    cout << "演示 2-bit Bitmap（小范围模拟）与分桶法\n";
    // Bitmap 模拟范围设置为 0..1000
    bitmap_demo(infile, 1000);
    // Bucket 模式 B=4
    bucket_demo(infile, 4);
    cout << "测试完成。\n";
    return 0;
}
