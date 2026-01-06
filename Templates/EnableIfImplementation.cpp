#include<type_traits>
#include<cassert>

// if Predicate is false then type exists
template<bool Predicate, typename T = void>
struct EnableIf {
    using type = T;
};
// if Predicate is false then type does not exist
template<typename T>
struct EnableIf<false, T> {};

// Use EnableIf to generate specialized overload
template<typename U>
typename EnableIf<std::is_same_v<U, uint64_t>, void>::type  
foo(U u) {
    static_assert(sizeof(U) == 8);
}

// Use EnableIf to generate specialized overload
template<typename U>
typename EnableIf<std::is_same_v<U, char>, void>::type  
foo(U u) {
    static_assert(sizeof(U) == 1);
}

int main() {
    char c = 'a';
    foo(c);
    uint64_t x = 102;
    foo(x);
}
