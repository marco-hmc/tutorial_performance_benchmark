#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

// 小型演示实现：支持两种模式
// 1) memory 模式：直接读取文件到内存 hash_map 统计（仅用于样例）
// 2) bucket 模式：按照 hash(word)%B 分桶，对每个桶局部统计并取前K，再合并所有桶的候选

static size_t str_hash(const string &s) { return std::hash<string>{}(s); }

vector<pair<string, int>> topk_from_map(unordered_map<string, int> &mp,
                                        size_t K) {
    // 小根堆
    auto cmp = [](const pair<int, string> &a, const pair<int, string> &b) {
        return a.first > b.first;
    };
    priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(cmp)>
        pq(cmp);
    for (auto &kv : mp) {
        int cnt = kv.second;
        if (pq.size() < K)
            pq.emplace(cnt, kv.first);
        else if (cnt > pq.top().first) {
            pq.pop();
            pq.emplace(cnt, kv.first);
        }
    }
    vector<pair<string, int>> res;
    while (!pq.empty()) {
        res.emplace_back(pq.top().second, pq.top().first);
        pq.pop();
    }
    reverse(res.begin(), res.end());
    return res;
}

void mode_memory(const string &infile, size_t K) {
    unordered_map<string, int> mp;
    mp.reserve(1 << 20);
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        exit(1);
    }
    string w;
    while (getline(ifs, w)) {
        if (w.empty()) continue;
        ++mp[w];
    }
    auto top = topk_from_map(mp, K);
    cout << "---- memory 模式 Top-" << K << " ----\n";
    for (auto &p : top) cout << p.first << " " << p.second << "\n";
}

void mode_bucket(const string &infile, size_t K, int B) {
    pid_t pid = getpid();
    string tmpdir = "/tmp/top100_" + to_string(pid);
    mkdir(tmpdir.c_str(), 0700);
    vector<ofstream> bfs(B);
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/b" + to_string(i) + ".txt";
        bfs[i].open(fname);
    }
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        exit(1);
    }
    string w;
    while (getline(ifs, w)) {
        if (w.empty()) continue;
        size_t h = str_hash(w) % B;
        bfs[h] << w << '\n';
    }
    for (int i = 0; i < B; ++i) bfs[i].close();

    // 全局小根堆，存 (count, word)
    auto cmp = [](const pair<int, string> &a, const pair<int, string> &b) {
        return a.first > b.first;
    };
    priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(cmp)>
        global_pq(cmp);

    // 逐个桶处理
    for (int i = 0; i < B; ++i) {
        string fname = tmpdir + "/b" + to_string(i) + ".txt";
        ifstream bfsin(fname);
        unordered_map<string, int> mp;
        mp.reserve(1 << 10);
        string line;
        while (getline(bfsin, line)) {
            if (line.empty()) continue;
            ++mp[line];
        }
        auto cand = topk_from_map(mp, K);
        for (auto &p : cand) {
            int cnt = p.second;
            string w = p.first;
            if (global_pq.size() < K)
                global_pq.emplace(cnt, w);
            else if (cnt > global_pq.top().first) {
                global_pq.pop();
                global_pq.emplace(cnt, w);
            }
        }
        bfsin.close();
        remove(fname.c_str());
    }
    rmdir(tmpdir.c_str());

    vector<pair<string, int>> res;
    while (!global_pq.empty()) {
        res.emplace_back(global_pq.top().second, global_pq.top().first);
        global_pq.pop();
    }
    reverse(res.begin(), res.end());
    cout << "---- bucket 模式 Top-" << K << " (B=" << B << ") ----\n";
    for (auto &p : res) cout << p.first << " " << p.second << "\n";
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (argc < 2) {
        cerr << "usage: top100_words <sample.txt>\n";
        return 1;
    }
    string infile = argv[1];
    size_t K = 100;
    cout << "运行示例，K=" << K << " (样例使用较小数据)\n";
    mode_memory(infile, 10);  // 小样例演示 Top-10
    mode_bucket(infile, 10, 4);
    return 0;
}
