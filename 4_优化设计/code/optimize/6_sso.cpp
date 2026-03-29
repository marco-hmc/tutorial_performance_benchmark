/*
 短字符串优化（SSO, Small String Optimization）是一种优化技术，用于在字符串长度较小时避免动态内存分配，从而提高性能。以下是 SSOString 类中的短字符串优化如何提高性能的详细解释：
 1. **减少动态内存分配**：
    - 动态内存分配（如使用new通常是一个昂贵的操作，因为它涉及到操作系统的内存管理。
    - 通过在对象内部存储短字符串，可以避免动态内存分配，从而减少内存分配和释放的开销。
 2. **提高缓存命中率**：
    - 短字符串存储在对象内部的固定大小缓冲区中，这样可以提高缓存命中率。
    - 由于短字符串和对象的其他成员变量存储在一起，访问这些数据时更有可能命中 CPU 缓存，从而提高访问速度。
 3. **减少内存碎片**：
    - 动态内存分配可能导致内存碎片化，尤其是在频繁分配和释放内存的情况下。
    - 通过在对象内部存储短字符串，可以减少动态内存分配，从而减少内存碎片化的可能性。
*/

#include <cstring>
#include <iostream>

class SSOString {
  public:
    SSOString() : size_(0), is_long_(false) { buffer_.short_str[0] = '\0'; }

    explicit SSOString(const char* str) : size_(std::strlen(str)) {
        if (size_ < short_max_size) {
            std::strcpy(buffer_.short_str, str);
            is_long_ = false;
        } else {
            buffer_.long_str.ptr = new char[size_ + 1];
            std::strcpy(buffer_.long_str.ptr, str);
            buffer_.long_str.capacity = size_;
            is_long_ = true;
        }
    }

    SSOString(const SSOString& other)
        : size_(other.size_), is_long_(other.is_long_) {
        if (is_long_) {
            buffer_.long_str.ptr = new char[size_ + 1];
            std::strcpy(buffer_.long_str.ptr, other.buffer_.long_str.ptr);
            buffer_.long_str.capacity = size_;
        } else {
            std::strcpy(buffer_.short_str, other.buffer_.short_str);
        }
    }

    SSOString& operator=(const SSOString& other) {
        if (this != &other) {
            if (is_long_) {
                delete[] buffer_.long_str.ptr;
            }
            size_ = other.size_;
            is_long_ = other.is_long_;
            if (is_long_) {
                buffer_.long_str.ptr = new char[size_ + 1];
                std::strcpy(buffer_.long_str.ptr, other.buffer_.long_str.ptr);
                buffer_.long_str.capacity = size_;
            } else {
                std::strcpy(buffer_.short_str, other.buffer_.short_str);
            }
        }
        return *this;
    }

    ~SSOString() {
        if (is_long_) {
            delete[] buffer_.long_str.ptr;
        }
    }

    const char* c_str() const {
        return is_long_ ? buffer_.long_str.ptr : buffer_.short_str;
    }

    size_t size() const { return size_; }

  private:
    static const size_t short_max_size = 15;

    union Buffer {
        char short_str[short_max_size + 1];
        struct {
            char* ptr;
            size_t capacity;
        } long_str;
    } buffer_{};

    size_t size_;
    bool is_long_;
};

int main() {
    SSOString short_str("short");
    SSOString long_str("this is a very long string");

    std::cout << "Short string: " << short_str.c_str()
              << " (size: " << short_str.size() << ")\n";
    std::cout << "Long string: " << long_str.c_str()
              << " (size: " << long_str.size() << ")\n";

    SSOString copy_str = long_str;
    std::cout << "Copied long string: " << copy_str.c_str()
              << " (size: " << copy_str.size() << ")\n";

    return 0;
}