#include<type_traits>
#include<cassert>

// if Predicate is false then type 
template<bool Predicate, typename T = void>
struct EnableIf {
    using type = T;
};
// if Predicate is false then type does not exist
template<typename T>
struct EnableIf<false, T> {};


// return type is part of function signature, so these can coexist
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


template<typename U>
struct X {
    // Enable only if prefdicate is true
    typename EnableIf<std::is_same_v<U, int64_t>>::type foo() {
        static_assert(sizeof(U) == 8);
    }
};

template<typename U>
struct Y {
    // specialize non template functions using Dummy value
    template<typename Dummy = void>
    typename EnableIf<std::is_same_v<U, int64_t>, Dummy>::type foo() {
        static_assert(sizeof(U) == 8);
    }

    template<typename Dummy = void>
    typename EnableIf<std::is_same_v<U, char>, Dummy>::type foo() {
        static_assert(sizeof(U) == 1);
    }
};


int main() {
    char c = 'a';
    foo(c);
    uint64_t x = 102;
    foo(x);

    X<int64_t> y;
    y.foo();

    Y<char> z;
    z.foo();
}
