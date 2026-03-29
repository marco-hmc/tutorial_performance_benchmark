#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

class DataBuffer {
  private:
    std::vector<int> data;
    int* raw_ptr;
    size_t size;

  public:
    DataBuffer(size_t sz) : data(sz), size(sz) {
        raw_ptr = data.data();
        // 初始化数据
        for (size_t i = 0; i < sz; ++i) {
            data[i] = static_cast<int>(i);
        }
    }

    // 性能读 - 直接内存访问，无边界检查
    inline int performance_read(size_t index) const noexcept {
        return raw_ptr[index];  // 直接访问，最快但不安全
    }

    // 安全读 - 边界检查和异常处理
    int safe_read(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];  // 使用vector的边界检查
    }

    // 性能写 - 直接内存访问，无边界检查
    inline void performance_write(size_t index, int value) noexcept {
        raw_ptr[index] = value;  // 直接写入，最快但不安全
    }

    // 安全写 - 边界检查和异常处理
    void safe_write(size_t index, int value) {
        if (index >= size) {
            throw std::out_of_range("Index out of bounds");
        }
        data[index] = value;  // 使用vector的边界检查
    }

    size_t get_size() const { return size; }
};

// 性能测试函数
void performance_test() {
    const size_t buffer_size = 10000000;  // 1000万个元素
    const size_t iterations = 100000000;  // 1亿次操作

    DataBuffer buffer(buffer_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, buffer_size - 1);

    std::cout << "=== 性能测试 (缓冲区大小: " << buffer_size
              << ", 操作次数: " << iterations << ") ===" << std::endl;

    // 测试性能读
    {
        auto start = std::chrono::high_resolution_clock::now();
        volatile int sum = 0;  // volatile防止编译器优化

        for (size_t i = 0; i < iterations; ++i) {
            size_t index = dist(gen);
            sum += buffer.performance_read(index);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "性能读: " << duration.count() << " ms" << std::endl;
    }

    // 测试安全读
    {
        auto start = std::chrono::high_resolution_clock::now();
        volatile int sum = 0;

        try {
            for (size_t i = 0; i < iterations; ++i) {
                size_t index = dist(gen);
                sum += buffer.safe_read(index);
            }
        } catch (const std::exception& e) {
            std::cout << "安全读异常: " << e.what() << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "安全读: " << duration.count() << " ms" << std::endl;
    }

    // 测试性能写
    {
        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < iterations; ++i) {
            size_t index = dist(gen);
            buffer.performance_write(index, static_cast<int>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "性能写: " << duration.count() << " ms" << std::endl;
    }

    // 测试安全写
    {
        auto start = std::chrono::high_resolution_clock::now();

        try {
            for (size_t i = 0; i < iterations; ++i) {
                size_t index = dist(gen);
                buffer.safe_write(index, static_cast<int>(i));
            }
        } catch (const std::exception& e) {
            std::cout << "安全写异常: " << e.what() << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "安全写: " << duration.count() << " ms" << std::endl;
    }
}

// 安全性演示
void safety_demo() {
    std::cout << "\n=== 安全性演示 ===" << std::endl;

    DataBuffer buffer(10);

    // 正常访问
    std::cout << "正常读取 buffer[5]: " << buffer.safe_read(5) << std::endl;

    // 越界访问演示
    try {
        std::cout << "尝试越界读取 buffer[15]: ";
        int value = buffer.safe_read(15);  // 这会抛出异常
        std::cout << value << std::endl;
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }

    try {
        std::cout << "尝试越界写入 buffer[20]: ";
        buffer.safe_write(20, 999);  // 这会抛出异常
        std::cout << "写入成功" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }

    // 性能访问的风险演示（注释掉，因为会导致未定义行为）
    /*
    std::cout << "性能读越界访问(危险): " << buffer.performance_read(15) << std::endl;
    buffer.performance_write(20, 999);  // 可能导致程序崩溃
    */
    std::cout << "注意: 性能读写不检查边界，越界访问可能导致程序崩溃或数据损坏"
              << std::endl;
}

int main() {
    std::cout << "性能 vs 安全 - 读写操作对比测试" << std::endl;
    std::cout << "==============================" << std::endl;

    // 运行安全性演示
    safety_demo();

    // 运行性能测试
    performance_test();

    std::cout << "\n=== 总结 ===" << std::endl;
    std::cout << "性能读写: 速度最快，但无边界检查，可能导致缓冲区溢出"
              << std::endl;
    std::cout << "安全读写: 速度较慢，但有边界检查和异常处理，更安全可靠"
              << std::endl;
    std::cout << "建议: 在性能关键且能保证索引有效的内部循环中使用性能读写"
              << std::endl;
    std::cout << "      在用户输入或不确定边界的场景中使用安全读写"
              << std::endl;

    return 0;
}
