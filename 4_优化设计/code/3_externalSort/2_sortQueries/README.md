# 题目 2：对 10 个文件中的 query 按频度排序

完整题目说明：

给定 10 个大的文本文件，每行存放一个用户的 query（字符串），可能有大量重复。目标是按 query 的出现频度对所有文件中的 query 进行排序（或取 Top-K）。

实现说明（示例代码包含两种模式）：

- memory 模式：直接将所有文件的 query 读入内存的 `unordered_map` 进行计数，最后按频率排序并输出 Top-K（适合数据量能放进内存的场景）。
- bucket 模式：将输入按 `hash(query) % M` 分成 M 个桶（临时文件），保证相同 query 落在同一桶；对每个桶在内存中统计频率，并用全局小根堆抽取 Top-K（适合海量数据的分布式/外部处理）。

**方案 1（外排序）**：

- 按照 `hash(query)%10` 将 query 重新分布到 10 个文件中
- 对每个文件用 `hash_map(query, query_count)` 统计频次并排序
- 多路归并得到最终结果

**方案 2（内存充足）**：

- 如果去重后的 query 总量能放入内存，直接用 trie 树或 hash_map 统计

使用方法示例（见 `run_test.sh`）：

./sort_queries memory K file1 file2 ... ./sort_queries bucket M K file1 file2 ...

示例目录包含 10 个小输入文件用于演示。请在更大规模测试时增大桶数 M 并分配足够的磁盘空间用于临时文件。
