#include <type_traits>

template <typename T>
constexpr std::remove_reference_t<T>&& MyMove(T&& arg) noexcept {
    return static_cast< std::remove_reference_t<T>&& > (arg);
}

/*

    T&& wrongMove(T&& arg) {
        return (T&&)arg;
    }
    lval
    T = string&
    return ->  string& && -> string&

    rval
    T = string
    return =>  string&& ->  fine

    const lval
    T = const string&

    const rval
    T = const string
*/


int main() {

}