#include <bits/stdc++.h>
using namespace std;

// 方法1: 使用 unordered_set 直接去重（内存足够时）
void dedup_hashmap(const string &infile) {
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        return;
    }
    unordered_set<string> seen;
    string s;
    cout << "---- hashmap 去重结果 ----\n";
    while (getline(ifs, s)) {
        if (s.empty()) continue;
        if (seen.insert(s).second) cout << s << "\n";
    }
}

// 方法2: 简化 Trie（只演示插入和是否首次出现）
struct TrieNode {
    bool end = false;
    unordered_map<char, TrieNode *> next;
};

class SimpleTrie {
  public:
    SimpleTrie() { root = new TrieNode(); }
    ~SimpleTrie() { clear(root); }
    // 返回 true 表示这是第一次插入（第一次出现）
    bool insert_once(const string &s) {
        TrieNode *p = root;
        for (char c : s) {
            if (!p->next.count(c)) p->next[c] = new TrieNode();
            p = p->next[c];
        }
        if (p->end) return false;
        p->end = true;
        return true;
    }

  private:
    TrieNode *root;
    void clear(TrieNode *p) {
        if (!p) return;
        for (auto &kv : p->next) clear(kv.second);
        delete p;
    }
};

void dedup_trie(const string &infile) {
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开文件: " << infile << "\n";
        return;
    }
    SimpleTrie trie;
    string s;
    cout << "---- trie 去重结果 ----\n";
    while (getline(ifs, s)) {
        if (s.empty()) continue;
        if (trie.insert_once(s)) cout << s << "\n";
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "usage: remove_duplicates <sample_strings.txt>\n";
        return 1;
    }
    string infile = argv[1];
    dedup_hashmap(infile);
    cout << "\n";
    dedup_trie(infile);
    return 0;
}
