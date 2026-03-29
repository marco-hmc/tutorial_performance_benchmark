// 对多个文件中的 query 按频度排序（或取 Top-K）
// 支持两种模式：
// 1) memory: 将所有 query 加入内存哈希表统计（适合能放入内存时）
//    用法： ./sort_queries memory K file1 file2 ...
// 2) bucket: 将输入按 hash(query)%M 分桶，逐桶统计并合并 Top-K（适合海量数据）
//    用法： ./sort_queries bucket M K file1 file2 ...

#include <bits/stdc++.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

static inline uint64_t str_hash(const string& s) {
    return (uint64_t)std::hash<string>{}(s);
}

void topk_from_map(const unordered_map<string, int>& cnt, int K) {
    auto cmp = [](const pair<int, string>& a, const pair<int, string>& b) {
        if (a.first != b.first) return a.first > b.first;  // min-heap by count
        return a.second < b.second;
    };
    priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(cmp)>
        pq(cmp);
    for (auto& p : cnt) {
        pq.emplace(p.second, p.first);
        if ((int)pq.size() > K) pq.pop();
    }
    vector<pair<int, string>> res;
    while (!pq.empty()) {
        res.push_back(pq.top());
        pq.pop();
    }
    reverse(res.begin(), res.end());
    for (auto& p : res) cout << p.second << " " << p.first << "\n";
}

int mode_memory(int K, const vector<string>& files) {
    unordered_map<string, int> cnt;
    for (auto& f : files) {
        ifstream fin(f);
        if (!fin) {
            cerr << "无法打开文件: " << f << "\n";
            return 1;
        }
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            ++cnt[line];
        }
    }
    topk_from_map(cnt, K);
    return 0;
}

int mode_bucket(int M, int K, const vector<string>& files) {
    if (M <= 0) {
        cerr << "桶数 M 必须>0\n";
        return 1;
    }
    string tmpdir = "/tmp/sort_queries_" + to_string(getpid());
    if (mkdir(tmpdir.c_str(), 0700) != 0) {
        cerr << "无法创建临时目录: " << tmpdir << "\n";
        return 1;
    }
    vector<string> names(M);
    for (int i = 0; i < M; i++)
        names[i] = tmpdir + "/bucket_" + to_string(i) + ".txt";
    // 分桶
    vector<ofstream> outs(M);
    for (int i = 0; i < M; i++) outs[i].open(names[i], ios::out | ios::binary);
    for (auto& f : files) {
        ifstream fin(f);
        if (!fin) {
            cerr << "无法打开文件: " << f << "\n";
            return 1;
        }
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            uint64_t h = str_hash(line);
            int idx = (int)(h % (uint64_t)M);
            outs[idx] << line << "\n";
        }
    }
    for (int i = 0; i < M; i++) outs[i].close();
    // 每个桶统计并用全局小根堆合并 Top-K
    auto cmp = [](const pair<int, string>& a, const pair<int, string>& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second < b.second;
    };
    priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(cmp)>
        global_pq(cmp);
    for (int i = 0; i < M; i++) {
        ifstream fin(names[i]);
        if (!fin) continue;
        unordered_map<string, int> cnt;
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            ++cnt[line];
        }
        // 对该桶取 Top-K 并合并到 global_pq
        for (auto& p : cnt) {
            global_pq.emplace(p.second, p.first);
            if ((int)global_pq.size() > K) global_pq.pop();
        }
        fin.close();
        remove(names[i].c_str());
    }
    rmdir(tmpdir.c_str());
    vector<pair<int, string>> res;
    while (!global_pq.empty()) {
        res.push_back(global_pq.top());
        global_pq.pop();
    }
    reverse(res.begin(), res.end());
    for (auto& p : res) cout << p.second << " " << p.first << "\n";
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage:\n  memory: ./sort_queries memory K file1 file2 ...\n  "
                "bucket: ./sort_queries bucket M K file1 file2 ...\n";
        return 1;
    }
    string mode = argv[1];
    if (mode == "memory") {
        int K = stoi(argv[2]);
        vector<string> files;
        for (int i = 3; i < argc; i++) files.push_back(argv[i]);
        return mode_memory(K, files);
    } else if (mode == "bucket") {
        int M = stoi(argv[2]);
        int K = stoi(argv[3]);
        vector<string> files;
        for (int i = 4; i < argc; i++) files.push_back(argv[i]);
        return mode_bucket(M, K, files);
    } else {
        cerr << "未知模式: " << mode << "\n";
        return 1;
    }
}
