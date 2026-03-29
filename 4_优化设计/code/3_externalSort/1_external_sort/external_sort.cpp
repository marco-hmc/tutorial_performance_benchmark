#include <bits/stdc++.h>
using namespace std;

// 简化演示的外部排序：
// 1. 将输入按 CHUNK_SIZE 个整数一块拆分到临时文件
// 2. 对每个块在内存中排序并写回临时有序文件
// 3. 使用多路归并（优先队列）合并所有有序临时文件到最终输出

// 注意：本示例用的是小规模参数，真实场景需按内存大小和 I/O 优化

int main(int argc, char** argv) {
    string infile = "sample_large.txt";
    if (argc >= 2) infile = argv[1];
    string out = "sorted_out.txt";
    if (argc >= 3) out = argv[2];

    // 演示时把每个 chunk 大小设置为较小值
    const size_t CHUNK_SIZE = 20;  // 每块 20 个整数（示例）

    ifstream ifs(infile);
    if (!ifs) {
        cerr << "无法打开输入文件: " << infile << "\n";
        return 1;
    }

    vector<string> chunk_files;
    size_t chunk_id = 0;
    while (true) {
        vector<int> buf;
        buf.reserve(CHUNK_SIZE);
        int x;
        for (size_t i = 0; i < CHUNK_SIZE && (ifs >> x); ++i) buf.push_back(x);
        if (buf.empty()) break;
        sort(buf.begin(), buf.end());
        string fname = "/tmp/extsort_chunk_" + to_string(chunk_id++) + ".txt";
        ofstream ofs(fname);
        for (int v : buf) ofs << v << '\n';
        ofs.close();
        chunk_files.push_back(fname);
    }
    ifs.close();

    cout << "生成 " << chunk_files.size() << " 个有序子文件。开始多路归并...\n";

    // 多路归并：维护每个文件的当前值，使用最小堆
    struct Node {
        int val;
        int idx;
    };
    struct Cmp {
        bool operator()(const Node& a, const Node& b) const {
            return a.val > b.val;
        }
    };
    priority_queue<Node, vector<Node>, Cmp> pq;

    // 打开所有子文件并读入首元素
    vector<ifstream> inputs;
    for (auto& f : chunk_files) {
        inputs.emplace_back(f);
    }
    for (int i = 0; i < (int)inputs.size(); ++i) {
        int v;
        if (inputs[i] >> v) pq.push({v, i});
    }

    ofstream ofs(out);
    while (!pq.empty()) {
        Node t = pq.top();
        pq.pop();
        ofs << t.val << '\n';
        int idx = t.idx;
        int v;
        if (inputs[idx] >> v) pq.push({v, idx});
    }
    ofs.close();

    // 关闭并删除临时文件
    for (int i = 0; i < (int)inputs.size(); ++i) {
        inputs[i].close();
        remove(chunk_files[i].c_str());
    }

    cout << "归并完成，结果写入: " << out << "\n";
    return 0;
}
