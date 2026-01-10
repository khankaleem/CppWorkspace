#include<iostream>
#include<type_traits>

template<typename T>
struct referenceWrapper {
    using type = T;
    referenceWrapper(T& elem_) : m_elem{&elem_} {}

    // cast operator
    operator T&() const { return *m_elem; }
    T& get() const { return *m_elem; }
    // T is a function
    template<typename... ArgsT>
    auto operator() (ArgsT&&... args) {
        return (*m_elem)(std::forward<ArgsT>(args)...);
    }
private:
    T* m_elem;
};

template<typename T>
void f(T t) {

}

int main() {
    struct A {
        A() { std::cout << "Constructing...\n";}
        A(A&&) = delete;
        A(const A&) {std::cout << "Copy Constructing...\n";}
    };  
    A a;
    f(referenceWrapper(a));
}