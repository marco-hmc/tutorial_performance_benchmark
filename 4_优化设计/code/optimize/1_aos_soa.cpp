#include <chrono>
#include <execution>
#include <iostream>
#include <vector>

constexpr size_t N = 1'000'000;  // 粒子数量

// AoS: Array of Structures
namespace AoS {
    struct Particle {
        float x, y, z;     // 位置
        float vx, vy, vz;  // 速度
    };

    namespace SingleThread {
        void update(std::vector<Particle>& particles) {
            for (auto& p : particles) {
                p.x += p.vx;
                p.y += p.vy;
                p.z += p.vz;
            }
        }
    }  // namespace SingleThread

    namespace MultiThread {
        void update(std::vector<Particle>& particles) {
            std::for_each(std::execution::par_unseq, particles.begin(),
                          particles.end(), [](Particle& p) {
                              p.x += p.vx;
                              p.y += p.vy;
                              p.z += p.vz;
                          });
        }
    }  // namespace MultiThread
}  // namespace AoS

// SoA: Structure of Arrays
namespace SoA {
    struct Particles {
        std::vector<float> x, y, z;     // 位置
        std::vector<float> vx, vy, vz;  // 速度
    };

    namespace SingleThread {
        void update(Particles& particles) {
            for (size_t i = 0; i < N; ++i) {
                particles.x[i] += particles.vx[i];
                particles.y[i] += particles.vy[i];
                particles.z[i] += particles.vz[i];
            }
        }
    }  // namespace SingleThread

    namespace MultiThread {
        void update(Particles& particles) {
            std::for_each(std::execution::par_unseq, particles.x.begin(),
                          particles.x.end(), [&](float& x) {
                              size_t i =
                                  &x - &particles.x[0];  // 计算当前元素的索引
                              x += particles.vx[i];
                              particles.y[i] += particles.vy[i];
                              particles.z[i] += particles.vz[i];
                          });
        }
    }  // namespace MultiThread
}  // namespace SoA

int main() {
    // 初始化 AoS 粒子
    std::vector<AoS::Particle> aos(N,
                                   {0, 0, 0, 1, 1, 1});  // 初始位置为0，速度为1

    // 初始化 SoA 粒子
    SoA::Particles soa;
    soa.x.resize(N, 0);
    soa.y.resize(N, 0);
    soa.z.resize(N, 0);
    soa.vx.resize(N, 1);
    soa.vy.resize(N, 1);
    soa.vz.resize(N, 1);

    // 测试 AoS 单线程性能
    auto start = std::chrono::high_resolution_clock::now();
    AoS::SingleThread::update(aos);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "AoS 单线程耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 测试 AoS 多线程性能
    start = std::chrono::high_resolution_clock::now();
    AoS::MultiThread::update(aos);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "AoS 多线程耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 测试 SoA 单线程性能
    start = std::chrono::high_resolution_clock::now();
    SoA::SingleThread::update(soa);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "SoA 单线程耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    // 测试 SoA 多线程性能
    start = std::chrono::high_resolution_clock::now();
    SoA::MultiThread::update(soa);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "SoA 多线程耗时: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                       start)
                     .count()
              << " μs\n";

    return 0;
}