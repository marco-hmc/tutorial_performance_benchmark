#include <chrono>
#include <functional>
#include <iostream>
#include <optional>
#include <vector>

class VecWrapper {
  public:
    VecWrapper(const std::vector<int>& vec) : vec_(vec) {}

    std::optional<std::reference_wrapper<int>> get(size_t index) {
        if (index < vec_.size()) {
            return std::optional<std::reference_wrapper<int>>(vec_[index]);
        } else {
            return std::nullopt;
        }
    }

  private:
    std::vector<int> vec_;
};

int main() {
    std::vector<int> data(1'000'000, 42);  // 初始化大数据量
    VecWrapper vec(data);

    constexpr size_t test_index = 500'000;       // 测试中间索引
    constexpr size_t invalid_index = 2'000'000;  // 测试无效索引

    // 性能读测试
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1'000'000; ++i) {
        volatile int value = vec.get(test_index).value().get();  // 防止优化
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "性能读耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 安全读测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1'000'000; ++i) {
        if (auto opt = vec.get(test_index); opt.has_value()) {
            volatile int value = opt.value().get();  // 防止优化
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "安全读耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 性能写测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1'000'000; ++i) {
        vec.get(test_index).value().get() = 84;  // 写入新值
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "性能写耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 安全写测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1'000'000; ++i) {
        if (auto opt = vec.get(test_index); opt.has_value()) {
            opt.value().get() = 84;  // 写入新值
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "安全写耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 无效索引安全读测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1'000'000; ++i) {
        if (auto opt = vec.get(invalid_index); opt.has_value()) {
            volatile int value = opt.value().get();  // 防止优化
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "无效索引安全读耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    return 0;
}