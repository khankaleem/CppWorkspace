/*
    Some varidic template implementation
*/

// Power
template<int base, int exponent>
class Power {
  public:
    static constexpr int value = base * Power<base, exponent-1>::value;
};

template<int base>
class Power<base, 0> {
public:
  static constexpr int value = 1;
};



// Square Root
template<bool pred, typename U, typename V>
class IfThenElse {
public:
  using type = U;
};

template<typename U, typename V>
class IfThenElse<false, U, V> {
public:
  using type = V;
};

template<int N, int L = 1, int H = N>
class FastSquareRoot {
  static constexpr int M = ((L + H)>>1);
  using type = typename IfThenElse< M*M >= N , FastSquareRoot<N, L, M >, FastSquareRoot<N, M + 1, H > >::type;
public:
  static constexpr int value = type::value;
};

template<int N, int L>
class FastSquareRoot<N, L, L> {
public:
  static constexpr int value = L;
};

// Iterative Square Root
template<int I>
class ValueWrapper {
public:
  static constexpr int value = I;
};
template<int N, int I=1>
class IterativeSquareRoot {
  using type = typename IfThenElse< I*I < N , IterativeSquareRoot<N, I+1>, ValueWrapper<I> >::type;
public:
  static constexpr int value = type::value;
};


//Compile time nCr computation
template<int N, int R>
constexpr int nCr() {
    static_assert(N >= 0, "nCr: N must be >= 0");
    static_assert(R >= 0, "nCr: R must be >= 0");
    if constexpr (R > N) {
        return 0;
    } else if constexpr(N == R || R == 0) {
        return 1;
    } else {
        return nCr<N-1, R-1>() + nCr<N-1, R>();
    }
}


int main() {
    static_assert(nCr<6, 2>() == 15);
}
