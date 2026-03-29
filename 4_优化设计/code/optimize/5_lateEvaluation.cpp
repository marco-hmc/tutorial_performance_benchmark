#include <functional>
#include <iostream>
#include <vector>

// 示例 1: 使用 std::function 和 Lambda 表达式
namespace Example1 {
    class LateEvaluation {
      public:
        void addTask(const std::function<int()>& task) {
            tasks.push_back(task);
        }

        std::vector<int> evaluate() {
            std::vector<int> results;
            for (const auto& task : tasks) {
                results.push_back(task());
            }
            return results;
        }

      private:
        std::vector<std::function<int()>> tasks;
    };

    void task() {
        LateEvaluation evaluator;
        int a = 10, b = 20, c = 30;
        evaluator.addTask([&]() { return a + b; });
        evaluator.addTask([&]() { return b * c; });
        evaluator.addTask([&]() { return c - a; });
        a = 15;
        b = 25;
        c = 35;
        auto results = evaluator.evaluate();
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "Example1 Task " << i + 1 << " result: " << results[i]
                      << std::endl;
        }
    }
}  // namespace Example1

// 示例 2: 使用惰性对象（Lazy Object）
namespace Example2 {
    class Lazy {
      public:
        Lazy(std::function<int()> func)
            : func(func), evaluated(false), value(0) {}

        int getValue() {
            if (!evaluated) {
                value = func();
                evaluated = true;
            }
            return value;
        }

      private:
        std::function<int()> func;
        bool evaluated;
        int value;
    };

    void task() {
        int x = 10, y = 20;
        Lazy lazySum([&]() { return x + y; });
        x = 15;
        y = 25;
        std::cout << "Example2 Lazy result: " << lazySum.getValue()
                  << std::endl;
    }
}  // namespace Example2

// 示例 3: 使用生成器（Generator）
namespace Example3 {
    class Generator {
      public:
        void addTask(const std::function<int()>& task) {
            tasks.push_back(task);
        }

        void generate() {
            for (const auto& task : tasks) {
                std::cout << "Generated value: " << task() << std::endl;
            }
        }

      private:
        std::vector<std::function<int()>> tasks;
    };

    void task() {
        Generator generator;
        int x = 5, y = 6;
        generator.addTask([&]() { return x + y; });
        generator.addTask([&]() { return x * y; });
        generator.generate();
    }
}  // namespace Example3

// 示例 4: 使用模板和函数指针
namespace Example4 {
    class LateEvaluationTemplate {
      public:
        template <typename Func>
        void addTask(Func task) {
            tasks.push_back([task]() { return task(); });
        }

        std::vector<int> evaluate() {
            std::vector<int> results;
            for (const auto& task : tasks) {
                results.push_back(task());
            }
            return results;
        }

      private:
        std::vector<std::function<int()>> tasks;
    };

    void task() {
        LateEvaluationTemplate evaluator;
        int p = 5, q = 10;
        evaluator.addTask([&]() { return p + q; });
        evaluator.addTask([&]() { return p * q; });
        p = 7;
        q = 15;
        auto results = evaluator.evaluate();
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "Example4 Template Task " << i + 1
                      << " result: " << results[i] << std::endl;
        }
    }
}  // namespace Example4

int main() {
    Example1::task();
    Example2::task();
    Example3::task();
    Example4::task();
    return 0;
}
