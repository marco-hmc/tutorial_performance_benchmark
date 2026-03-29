#include <bits/stdc++.h>
using namespace std;

// 方案1：局部淘汰法（适用于 K 很小的场景）
vector<int> local_elimination(vector<int> a, int K) {
    if ((int)a.size() <= K) {
        sort(a.begin(), a.end(), greater<int>());
        return a;
    }
    vector<int> L(a.begin(), a.begin() + K);
    sort(L.begin(), L.end());  // 升序，L[0] 为最小
    for (size_t i = K; i < a.size(); ++i) {
        int x = a[i];
        if (x > L[0]) {
            // 替换并将 x 插入到合适位置（插入排序方式）
            L[0] = x;
            // 将新的最小元素向右移动以恢复有序
            int j = 0;
            while (j + 1 < K && L[j] > L[j + 1]) {
                swap(L[j], L[j + 1]);
                ++j;
            }
        }
    }
    // 返回降序结果
    sort(L.begin(), L.end(), greater<int>());
    return L;
}

// 方案2：快速选择（QuickSelect） — 找前 K 大的元素（无序），最后对这部分排序
int partition_desc(vector<int>& a, int l, int r) {
    int pivot = a[r];
    int i = l;
    for (int j = l; j < r; ++j) {
        if (a[j] > pivot) {  // 大元素放左
            swap(a[i++], a[j]);
        }
    }
    swap(a[i], a[r]);
    return i;
}

void quickselect_topk(vector<int>& a, int l, int r, int K) {
    if (l >= r) return;
    int idx = partition_desc(a, l, r);
    int left_count = idx - l + 1;  // 包含 idx 本身
    if (left_count == K)
        return;
    else if (left_count > K)
        quickselect_topk(a, l, idx - 1, K);
    else
        quickselect_topk(a, idx + 1, r, K - left_count);
}

vector<int> quickselect_method(vector<int> a, int K) {
    if ((int)a.size() <= K) {
        sort(a.begin(), a.end(), greater<int>());
        return a;
    }
    quickselect_topk(a, 0, (int)a.size() - 1, K);
    vector<int> topk(a.begin(), a.begin() + K);
    sort(topk.begin(), topk.end(), greater<int>());
    return topk;
}

// 方案3：最小堆（推荐，时间复杂度 O(n log K)）
vector<int> min_heap_method(const vector<int>& a, int K) {
    if ((int)a.size() <= K) {
        vector<int> b = a;
        sort(b.begin(), b.end(), greater<int>());
        return b;
    }
    priority_queue<int, vector<int>, greater<int>> pq;
    for (int x : a) {
        if ((int)pq.size() < K)
            pq.push(x);
        else if (x > pq.top()) {
            pq.pop();
            pq.push(x);
        }
    }
    vector<int> out;
    while (!pq.empty()) {
        out.push_back(pq.top());
        pq.pop();
    }
    sort(out.begin(), out.end(), greater<int>());
    return out;
}

int main(int argc, char** argv) {
    // 为演示方便，读取 sample_numbers.txt，如果传入文件参数则使用该文件
    string infile = "sample_numbers.txt";
    if (argc >= 2) infile = argv[1];

    vector<int> data;
    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开输入文件: " << infile << "\n";
        return 1;
    }
    int x;
    while (ifs >> x) data.push_back(x);
    cout << "总数: " << data.size() << "\n";

    int K = 10;  // 演示 Top-10，可修改为 100 或通过命令行传入
    cout << "演示 Top-" << K << "（可在源码中调整）\n";

    auto r1 = local_elimination(data, K);
    cout << "---- 局部淘汰法 Top-" << K << " ----\n";
    for (int v : r1) cout << v << " ";
    cout << "\n";

    auto r2 = quickselect_method(data, K);
    cout << "---- QuickSelect Top-" << K << " ----\n";
    for (int v : r2) cout << v << " ";
    cout << "\n";

    auto r3 = min_heap_method(data, K);
    cout << "---- MinHeap Top-" << K << " ----\n";
    for (int v : r3) cout << v << " ";
    cout << "\n";

    cout << "测试完成。\n";
    return 0;
}
