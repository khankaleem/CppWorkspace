#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>

class CacheConfig {
public:
  std::string m_name;
  size_t m_size{};
  bool m_enableLogging{};
  double m_evictionThreshold{};

  CacheConfig() = default;

  void display() const {
    std::ostringstream oss;
    oss << "|Name:" << m_name 
        << "|Size:" << m_size 
        << "|EnableLogging:" << m_enableLogging 
        << "|EvictionThreshold:" << m_evictionThreshold;
    std::cout << oss.str() << '\n';
  }
};

class CacheConfigBuilder {
  std::unique_ptr<CacheConfig> m_config;
public:
  CacheConfigBuilder() : m_config{std::make_unique<CacheConfig>()} {
  }

  CacheConfigBuilder& set_name(std::string name_) {
    m_config->m_name = std::move(name_);
    return *this;
  }

  CacheConfigBuilder& set_size(size_t size_) {
    m_config->m_size = size_;
    return *this;
  }

  CacheConfigBuilder& enable_logging(bool input_) {
    m_config->m_enableLogging = input_;
    return *this;
  }

  CacheConfigBuilder& set_eviction_threshold(int input_) {
    m_config->m_evictionThreshold = input_;
    return *this;
  }

  std::unique_ptr<CacheConfig> build() {
    return std::move(m_config);
  }
};


int main() {
  auto cacheConfig = CacheConfigBuilder()
                      .set_name("kcrcvf")
                      .enable_logging(true)
                      .set_size(1024)
                      .set_eviction_threshold(1024)
                      .build();
}
