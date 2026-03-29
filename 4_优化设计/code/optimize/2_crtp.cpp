/*
    奇异递归模板模式（CRTP，The Curiously Recurring Template Pattern）
    CRTP 的实现手法是将派生类作为基类的模板参数，其本质而言是将类型传递给父类，父类可以使用这个类型。从而避免虚函数，或者做一些其他操作等。
    一般而言有这些目的：
    1. 在编译时实现静态多态，性能更好；
    2. 处理operator的重载问题，特别是迭代器情况最为常见；
    3. 作为facade实现的一种，和impl一块实现facade；
*/

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace RegularPolymorphism {

    class Shape {
      public:
        virtual ~Shape() = default;
        virtual void draw() const = 0;
    };

    class Circle : public Shape {
      public:
        void draw() const override {
            std::cout << "Drawing a Circle" << std::endl;
        }
    };

    class Square : public Shape {
      public:
        void draw() const override {
            std::cout << "Drawing a Square" << std::endl;
        }
    };

    void task() {
        Circle circle;
        Square square;

        Shape* shape1 = &circle;
        Shape* shape2 = &square;
        shape1->draw();
        shape2->draw();
    }

}  // namespace RegularPolymorphism

namespace CompilerPolymorphism {

    // 基类模板
    template <typename Derived>
    class Shape {
      public:
        void draw() const {
            // 调用派生类的 draw_impl 方法
            static_cast<const Derived*>(this)->draw_impl();
        }
    };

    // 派生类：Circle
    class Circle : public Shape<Circle> {
      public:
        void draw_impl() const { std::cout << "Drawing a Circle" << std::endl; }
    };

    // 派生类：Square
    class Square : public Shape<Square> {
      public:
        void draw_impl() const { std::cout << "Drawing a Square" << std::endl; }
    };

    void task() {
        Circle circle;
        Square square;

        // 静态多态：在编译时确定调用的具体方法
        circle.draw();  // 输出：Drawing a Circle
        square.draw();  // 输出：Drawing a Square
    }
}  // namespace CompilerPolymorphism

namespace OperatorOverloadingByCRTP {

    template <typename T>
    class Base {
        friend bool operator!=(const T& lhs, const T& rhs) {
            return !(lhs == rhs);
        }
    };

    class Derived : public Base<Derived> {
        friend bool operator==(const Derived& lhs, const Derived& rhs) {
            return lhs.i_ == rhs.i_;
        }

      public:
        Derived(int i) : i_(i) {}

      private:
        int i_ = 0;
    };

    void task() {
        Derived a{1};
        Derived b{2};
        assert(a != b);
    }
}  // namespace OperatorOverloadingByCRTP

namespace RegularOperatorOverloading {

    class Base {
      public:
        virtual ~Base() = default;

        virtual bool isEqual(const Base& other) const = 0;

        friend bool operator==(const Base& lhs, const Base& rhs) {
            return lhs.isEqual(rhs);
        }

        friend bool operator!=(const Base& lhs, const Base& rhs) {
            return !(lhs == rhs);
        }
    };

    class Derived : public Base {
      public:
        Derived(int i) : i_(i) {}

        bool isEqual(const Base& other) const override {
            const Derived* otherDerived = dynamic_cast<const Derived*>(&other);
            if (otherDerived) {
                return i_ == otherDerived->i_;
            }
            return false;
        }

      private:
        int i_ = 0;
    };

    void task() {
        Derived a{1};
        Derived b{2};
        assert(a != b);
    }
}  // namespace RegularOperatorOverloading

namespace OperatorOverloadingByCrtp2 {

    template <typename Derived, typename Value, typename Category>
    class IteratorFacade {
      public:
        using value_type = std::remove_const_t<Value>;
        using reference = Value&;
        using pointer = Value*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = Category;

      public:
        reference operator*() const { return as_derived().dereference(); }

        Derived& operator++() {
            as_derived().increment();
            return as_derived();
        }

        Derived operator++(int) {
            Derived tmp(as_derived());
            as_derived().increment();
            return tmp;
        }

        friend bool operator==(const IteratorFacade& lhs,
                               const IteratorFacade& rhs) {
            return lhs.as_derived().equals(rhs.as_derived());
        }

        friend bool operator!=(const IteratorFacade& lhs,
                               const IteratorFacade& rhs) {
            return !operator==(lhs, rhs);
        }

      private:
        Derived& as_derived() { return *static_cast<Derived*>(this); }

        const Derived& as_derived() const {
            return *static_cast<const Derived*>(this);
        }
    };

    template <typename T>
    struct ListNode {
        ListNode(T x) : value(x) {}

        T value;
        ListNode<T>* next = nullptr;
    };

    template <typename T>
    class ListNodeIterator : public IteratorFacade<ListNodeIterator<T>, T,
                                                   std::forward_iterator_tag> {
      public:
        ListNodeIterator(ListNode<T>* t = nullptr) : t_(t) {}
        T& dereference() const { return t_->value; }
        void increment() { t_ = t_->next; }
        bool equals(const ListNodeIterator& rhs) const { return t_ == rhs.t_; }

      private:
        ListNode<T>* t_ = nullptr;
    };

    void task() {
        auto a = std::make_unique<ListNode<int>>(1);
        auto b = std::make_unique<ListNode<int>>(2);
        auto c = std::make_unique<ListNode<int>>(3);

        a->next = b.get();
        a->next->next = c.get();

        auto first = ListNodeIterator{a.get()};
        auto last = ++ListNodeIterator<int>{nullptr};

        std::vector<int> v;
        for (auto it = first; it != last; ++it) {
            v.emplace_back(*it);
        }
        assert((v == std::vector<int>{1, 2, 3}));
    }
}  // namespace OperatorOverloadingByCrtp2

namespace RegularOperatorOverloading2 {

    template <typename T>
    struct ListNode {
        ListNode(T x) : value(x) {}

        T value;
        ListNode<T>* next = nullptr;
    };

    //
    template <typename T>
    class ListNodeIterator {
      public:
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

      public:
        ListNodeIterator(ListNode<T>* t = nullptr) : t_(t) {}

        reference operator*() const { return t_->value; }

        ListNodeIterator& operator++() {
            t_ = t_->next;
            return *this;
        }

        ListNodeIterator operator++(int) {
            ListNodeIterator tmp(*this);
            t_ = t_->next;
            return tmp;
        }

        friend bool operator==(const ListNodeIterator& lhs,
                               const ListNodeIterator& rhs) {
            return lhs.t_ == rhs.t_;
        }

        friend bool operator!=(const ListNodeIterator& lhs,
                               const ListNodeIterator& rhs) {
            return !(lhs == rhs);
        }

      private:
        ListNode<T>* t_ = nullptr;
    };

    void task() {
        auto a = std::make_unique<ListNode<int>>(1);
        auto b = std::make_unique<ListNode<int>>(2);
        auto c = std::make_unique<ListNode<int>>(3);

        a->next = b.get();
        a->next->next = c.get();

        auto first = ListNodeIterator{a.get()};
        auto last = ++ListNodeIterator<int>{nullptr};

        std::vector<int> v;
        for (auto it = first; it != last; ++it) {
            v.emplace_back(*it);
        }
        assert((v == std::vector<int>{1, 2, 3}));
    }
}  // namespace RegularOperatorOverloading2

namespace TemplateInitializationCount {

    template <typename T>
    class Base {
      public:
        static int count() { return i; }

      protected:
        Base() { ++i; }
        Base(const Base<T>&) { ++i; }
        Base(Base<T>&&) noexcept { ++i; }
        ~Base() { --i; }

      private:
        inline static int i = 0;
    };

    template <typename T>
    class Derived : public Base<Derived<T>> {};

    void task() {
        Derived<int> a, b;
        Derived<char> c;
        assert(Derived<int>::count() == 2);
        assert(Derived<char>::count() == 1);
    }
}  // namespace TemplateInitializationCount

int main() {
    RegularPolymorphism::task();
    CompilerPolymorphism::task();
    OperatorOverloadingByCRTP::task();
    RegularOperatorOverloading::task();
    TemplateInitializationCount::task();
    return 0;
}