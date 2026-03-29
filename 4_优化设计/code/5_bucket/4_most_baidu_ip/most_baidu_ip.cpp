#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

// 简化日志解析：假设每行的格式为 "<ip> <rest of line>"，并且访问百度的行包含 "www.baidu.com" 子串。

bool line_has_baidu(const string &line) {
    return line.find("www.baidu.com") != string::npos;
}

string extract_ip(const string &line) {
    // 取第一个空格前的字段为 IP
    auto pos = line.find(' ');
    if (pos == string::npos) return line;
    return line.substr(0, pos);
}

pair<string, int> mode_memory(const string &infile) {
    unordered_map<string, int> cnt;
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        exit(1);
    }
    string line;
    while (getline(ifs, line)) {
        if (!line_has_baidu(line)) continue;
        string ip = extract_ip(line);
        ++cnt[ip];
    }
    string best;
    int bestc = 0;
    for (auto &kv : cnt)
        if (kv.second > bestc) {
            best = kv.first;
            bestc = kv.second;
        }
    return {best, bestc};
}

pair<string, int> mode_bucket(const string &infile, int B) {
    pid_t pid = getpid();
    string tmpdir = "/tmp/most_baidu_" + to_string(pid);
    mkdir(tmpdir.c_str(), 0700);
    vector<ofstream> ofs(B);
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/b" + to_string(i) + ".log";
        ofs[i].open(fname);
    }
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        exit(1);
    }
    string line;
    while (getline(ifs, line)) {
        if (!line_has_baidu(line)) continue;
        string ip = extract_ip(line);
        size_t h = std::hash<string>{}(ip) % B;
        ofs[h] << ip << '\n';
    }
    for (int i = 0; i < B; ++i) ofs[i].close();

    string best;
    int bestc = 0;
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/b" + to_string(i) + ".log";
        ifstream bifs(fname);
        unordered_map<string, int> cnt;
        string ip;
        while (getline(bifs, ip)) {
            if (ip.empty()) continue;
            ++cnt[ip];
        }
        for (auto &kv : cnt) {
            if (kv.second > bestc) {
                best = kv.first;
                bestc = kv.second;
            }
        }
        bifs.close();
        remove(fname.c_str());
    }
    rmdir(tmpdir.c_str());
    return {best, bestc};
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "usage: most_baidu_ip <logfile>\n";
        return 1;
    }
    string logfile = argv[1];
    cout << "---- memory 模式 ----\n";
    auto m = mode_memory(logfile);
    cout << "best: " << m.first << " " << m.second << "\n";
    cout << "---- bucket 模式 (B=10) ----\n";
    auto b = mode_bucket(logfile, 10);
    cout << "best: " << b.first << " " << b.second << "\n";
    return 0;
}
