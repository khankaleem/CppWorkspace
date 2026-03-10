#include<cassert>
#include<exception>
#include<mutex>
#include<condition_variable>
#include<stack>
#include<queue>
#include<iostream>
#include<shared_mutex>
#include<list>

template<typename KeyT, typename ValueT, typename HashT = std::hash<KeyT>>
class ConcurrentHashMap : public HashT {
public:
  using Key = KeyT;
  using Value = ValueT;
  using Hash = HashT;

private: 
  struct Bucket {
    public:
      using Elem = std::pair<Key, Value>;
      using Chain = std::list<Elem>;
      using Iterator = typename Chain::iterator;
      using ConstIterator = typename Chain::const_iterator;

    private:
      Chain m_chain;
      mutable std::shared_mutex m_mutex;

      ConstIterator find_unsafe(const Key& key_) const {
        return std::find_if(m_chain.begin(), 
                            m_chain.end(), 
                            [&key_](const Elem& elem_) {
                              return elem_.first == key_;
                            });
      }
      Iterator find_unsafe(const Key& key_) {
        return std::find_if(m_chain.begin(), 
                            m_chain.end(), 
                            [&key_](const Elem& elem_) {
                              return elem_.first == key_;
                            });
      }

    public:
      Bucket() = default;

      void insert_or_update(const Key& key_, const Value& val_) {
        std::unique_lock<std::shared_mutex> lock{m_mutex};
        auto it = find_unsafe(key_);
        if (it == m_chain.end()) {
          m_chain.push_front(Elem(key_, val_));
        }
        else {
          it->second = val_;
        }
      }
      void erase(const Key& key_) {
        std::unique_lock<std::shared_mutex> lock{m_mutex};
        auto it = find_unsafe(key_);
        if (it != m_chain.end()) {
          m_chain.erase(it);
        }
      }
      std::optional<Value> get_value(const Key& key_) const {
        std::shared_lock<std::shared_mutex> lock{m_mutex};
        auto it = find_unsafe(key_);
        if ( it != m_chain.end() ) {
          return it->second;
        }
        return std::nullopt; 
      }
  };
  using BucketPtr = std::unique_ptr<Bucket>;
  Hash& getHash() {
    return static_cast<Hash&>(*this);
  }  
  const Hash& getHash() const {
    return static_cast<const Hash&>(*this);
  }

  std::unique_ptr<BucketPtr[]> m_bucketptr{};
  size_t m_cap{};

  Bucket& getBucket(const Key& key_) {
    return *m_bucketptr[getHash()(key_) & (m_cap - 1)];
  }  
  const Bucket& getBucket(const Key& key_) const {
    return *m_bucketptr[getHash()(key_) & (m_cap - 1)];
  }

public:
  explicit ConcurrentHashMap(size_t cap_ = (1<<8)) : m_cap{cap_}, m_bucketptr{std::make_unique<BucketPtr[]>(cap_)} {
    for (size_t i = 0; i < m_cap; i++) {
      m_bucketptr[i] = std::make_unique<Bucket>();
    }
  }

  ConcurrentHashMap(const ConcurrentHashMap& other) = delete;
  ConcurrentHashMap& operator=(const ConcurrentHashMap& other) = delete;

  void insert_or_update(const Key& key_, const Value& val_) {
    getBucket(key_).insert_or_update(key_, val_);
  }
  std::optional<Value> get_value(const Key& key_) const {
    return getBucket(key_).get_value(key_);
  }
  void erase(const Key& key_) {
    getBucket(key_).erase(key_);
  }

};
template class ConcurrentHashMap<int, int>;
