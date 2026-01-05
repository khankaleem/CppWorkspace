#include <cassert>
#include <new> 
#include <utility> 
#include <cstddef> 

template<typename T>
class CArray {
    T* m_arr{nullptr};
    size_t m_size{0};
    T* allocRaw(size_t size) {
        if (size == 0) return nullptr;
        return reinterpret_cast<T*>(::operator new[](size * sizeof(T), std::align_val_t(alignof(T))));
    }
    void deallocRaw() {
        if (m_arr == nullptr) return;
        ::operator delete[](m_arr, std::align_val_t(alignof(T)));
    }
    void destroyRange(size_t N) {
        if (m_arr == nullptr) return;
        for (size_t i = 0; i < N; i++) {
            (m_arr + i)->~T();
        }
    }
public:
    CArray() = default;
    CArray(size_t size_) : m_arr{allocRaw(size_)}, m_size(size_) {
        size_t i = 0;
        try {
            for(; i < m_size; i++) {
                new (m_arr + i) T(); // default construct
            }
        }
        catch(...) {
            destroyRange(i);
            deallocRaw();
            m_arr = nullptr;
            m_size = 0;
            throw;
        }
    }
    // Rule of 5 using copy and swap idiom
    CArray(const CArray& other)  : m_arr{allocRaw(other.m_size)}, m_size{other.m_size} {
        size_t i = 0;
        try {
            for(; i < m_size; i++) {
                new (m_arr + i) T(other.m_arr[i]); // copy construct
            }
        }
        catch(...) {
            destroyRange(i);
            deallocRaw();
            m_arr = nullptr;
            m_size = 0;
            throw;
        }
    }
    CArray(CArray&& other) noexcept : m_arr{other.m_arr}, m_size(other.m_size) {
        other.m_arr = nullptr;
        other.m_size = 0;
    }
    CArray& operator=(const CArray& other) { // can unify move and copy assignment as well
        CArray tmp{other}; 
        swap(*this, tmp);
        return *this;    
    }
    CArray& operator=(CArray&& other) noexcept {
        CArray tmp{std::move(other)};
        swap(*this, tmp);
        return *this;    
    }
    ~CArray() {
        destroyRange(m_size);
        deallocRaw();
    }
    // Rule of 5 end

    // swap function
    friend void swap(CArray& first, CArray& second) noexcept {
        using std::swap; // Use std::swap as an overload candidate for swap: ADL
        swap(first.m_arr, second.m_arr);
        swap(first.m_size, second.m_size);
    }

    // operator: user needs to ensure bounds
    T& operator[](size_t idx) {
        return m_arr[idx];
    }
    const T& operator[](size_t idx) const {
        return m_arr[idx];
    }

    size_t size() const {
        return m_size;
    }
};

int main() {
    struct A {
        int x{100};
    };
    CArray<A> one{3};
    assert(one[0].x == 100);
    CArray<A> two{4};
    two[0].x = 1;
    assert(two[0].x == 1);

    //check copy assignment
    one = two;
    assert(one[0].x == 1);
    assert(one.size() == 4);
    assert(two[0].x == 1);
    assert(two.size() == 4);

    //check move assignment
    one = CArray<A>{5};
    assert(one.size() == 5);

    one = std::move(two);
    assert(one.size() == 4);
    assert(two.size() == 0);

    //check move construction
    CArray<A> three{CArray<A>{6}};
    assert(three.size() == 6);

    //check copy construction
    CArray<A> four{three};
    assert(three.size() == 6);
    assert(four.size() == 6);
}
