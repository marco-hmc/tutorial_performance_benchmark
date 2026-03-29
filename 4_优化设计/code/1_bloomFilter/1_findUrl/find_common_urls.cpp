// 找出 a、b 两个文件共同的 URL
// 支持两种模式：
// 1) memory: 将文件 A 全部读入内存 unordered_set，再扫描文件 B（适合小文件或测试）
// 2) bucket: 将 A 和 B 按 hash(url)%M 分为 M 个桶（临时文件），逐个桶读入内存计算交集（适合大文件）
//
// 用法：
//  1) 内存模式（快速测试）:
//     ./find_common_urls memory a.txt b.txt
//  2) 分桶模式（指定桶数 M）:
//     ./find_common_urls bucket M a.txt b.txt

#include <bits/stdc++.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

static inline uint64_t str_hash(const string &s) {
    // 使用 std::hash 作为简易哈希
    return (uint64_t)std::hash<string>{}(s);
}

int mode_memory(const string &a, const string &b) {
    // 将 a 文件读入内存哈希集合
    ifstream fa(a);
    if (!fa) {
        cerr << "无法打开文件: " << a << "\n";
        return 1;
    }
    unordered_set<string> S;
    string line;
    while (getline(fa, line)) {
        if (!line.empty()) S.insert(line);
    }
    fa.close();
    // 扫描 b 文件，输出交集（去重）
    ifstream fb(b);
    if (!fb) {
        cerr << "无法打开文件: " << b << "\n";
        return 1;
    }
    unordered_set<string> out;
    while (getline(fb, line)) {
        if (!line.empty() && S.find(line) != S.end()) out.insert(line);
    }
    fb.close();
    for (const auto &s : out) cout << s << "\n";
    return 0;
}

int mode_bucket(int M, const string &a, const string &b) {
    if (M <= 0) {
        cerr << "桶数 M 必须为正整数\n";
        return 1;
    }
    // 创建临时目录
    string tmpdir = "/tmp/find_urls_" + to_string(getpid());
    if (mkdir(tmpdir.c_str(), 0700) != 0) {
        cerr << "无法创建临时目录: " << tmpdir << "\n";
        return 1;
    }
    vector<string> fa_names(M), fb_names(M);
    for (int i = 0; i < M; ++i) {
        fa_names[i] = tmpdir + "/a_" + to_string(i) + ".txt";
        fb_names[i] = tmpdir + "/b_" + to_string(i) + ".txt";
    }
    // 分桶写入
    auto split_file = [&](const string &infile, const vector<string> &names) {
        ifstream fin(infile);
        if (!fin) {
            cerr << "无法打开文件: " << infile << "\n";
            return false;
        }
        vector<ofstream> outs(M);
        for (int i = 0; i < M; ++i)
            outs[i].open(names[i], ios::out | ios::binary);
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            uint64_t h = str_hash(line);
            int idx = (int)(h % (uint64_t)M);
            outs[idx] << line << '\n';
        }
        fin.close();
        for (int i = 0; i < M; ++i) outs[i].close();
        return true;
    };
    if (!split_file(a, fa_names)) return 1;
    if (!split_file(b, fb_names)) return 1;
    // 逐个桶处理并输出交集（去重）
    unordered_set<string> result;
    for (int i = 0; i < M; ++i) {
        ifstream fa_i(fa_names[i]);
        ifstream fb_i(fb_names[i]);
        if (!fa_i || !fb_i) continue;  // 空桶或打开失败
        unordered_set<string> Sa;
        string line;
        while (getline(fa_i, line))
            if (!line.empty()) Sa.insert(line);
        while (getline(fb_i, line))
            if (!line.empty() && Sa.find(line) != Sa.end()) result.insert(line);
        fa_i.close();
        fb_i.close();
    }
    for (const auto &s : result) cout << s << "\n";
    // 清理临时文件
    for (int i = 0; i < M; ++i) {
        remove(fa_names[i].c_str());
        remove(fb_names[i].c_str());
    }
    rmdir(tmpdir.c_str());
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cerr << "Usage:\n  memory: ./find_common_urls memory a.txt b.txt\n  "
                "bucket: ./find_common_urls bucket M a.txt b.txt\n";
        return 1;
    }
    string mode = argv[1];
    if (mode == "memory") {
        string a = argv[2], b = argv[3];
        return mode_memory(a, b);
    } else if (mode == "bucket") {
        if (argc < 5) {
            cerr << "bucket 模式需要桶数 M 和两个文件名\n";
            return 1;
        }
        int M = stoi(argv[2]);
        string a = argv[3], b = argv[4];
        return mode_bucket(M, a, b);
    } else {
        cerr << "未知模式: " << mode << "\n";
        return 1;
    }
}
