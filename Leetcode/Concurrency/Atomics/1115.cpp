#undef asm         // This removes the "error" definition
class FooBar {
private:
    int n;
    atomic<int> turn{0};
public:
    FooBar(int n) {
        this->n = n;
    }

    void foo(function<void()> printFoo) {
        for (int i = 0; i < n; i++) {
            while(turn.load(std::memory_order_acquire) != 0) {
                asm volatile("pause" ::: "memory");
            }
            printFoo();
            turn.store(1, std::memory_order_release);
        }
    }

    void bar(function<void()> printBar) {
        for (int i = 0; i < n; i++) {
            while(turn.load(std::memory_order_acquire) != 1) {
                asm volatile("pause" ::: "memory");
            }
            printBar();
            turn.store(0, std::memory_order_release);
        }
    }

};
