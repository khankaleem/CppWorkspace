#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>

class Singleton {
  Singleton() = default;
  ~Singleton() = default;

  std::atomic<uint64_t> m_count{0};

  void update_impl() {
    m_count.fetch_add(1, std::memory_order_relaxed);
  }

public:
  Singleton(const Singleton& other) = delete;
  Singleton& operator=(const Singleton& other) = delete;

  static Singleton& Get() {
    // Initialized thread-safely on the first call
    static Singleton instance{};
    return instance;
  }
  static void update() {
    Get().update_impl();
  }
  static uint64_t get_cnt() {
    return Get().m_count.load(std::memory_order_relaxed);
  }
};

int main() {
  Singleton::update();
}