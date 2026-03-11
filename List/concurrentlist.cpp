#include<cassert>
#include<exception>
#include<mutex>
#include<condition_variable>
#include<stack>
#include<queue>
#include<iostream>
#include<shared_mutex>
#include<list>
#include<map>



template<typename ElemT>
class ConcurrentList {
private:
  
  struct Node {
    std::shared_ptr<ElemT> m_data{};
    std::unique_ptr<Node> m_next{};
    mutable std::mutex m_mutex;
    Node() = default;

    template<typename E,
             typename = std::enable_if_t< std::is_constructible_v<ElemT, E&&> > >
    Node(E&& data_) : m_data{std::make_shared<ElemT>(std::forward<E>(data_))} {}
  };
  
  std::unique_ptr<Node> m_head{};

public:
  ConcurrentList() : m_head{std::make_unique<Node>()} {};
  ConcurrentList(const ConcurrentList& other) = delete;
  ConcurrentList& operator=(const ConcurrentList& other) = delete;

  template<typename E>
  std::enable_if_t< std::is_constructible_v<ElemT, E&&> > push_front(E&& data_) {
    std::unique_ptr<Node> new_node{std::make_unique<Node>(std::forward<E>(data_))};
    std::lock_guard<std::mutex> guard{m_head->m_mutex};
    new_node->m_next = std::move(m_head->m_next);
    m_head->m_next = std::move(new_node);
  }

  template<typename Visitor>
  void for_each(Visitor&& visitor) const {
    std::unique_lock<std::mutex> prev_lock{m_head->m_mutex};
    Node* prev = m_head.get();
    Node* cur; 
    while((cur = prev->m_next.get())) {
      std::unique_lock<std::mutex> cur_lock{cur->m_mutex};
      prev_lock.unlock();
      visitor(*cur->m_data);
      prev = cur;
      prev_lock = std::move(cur_lock);
    }
  }

  template<typename Predicate>
  std::shared_ptr<ElemT> find_if(Predicate&& pred) const {
    std::unique_lock<std::mutex> prev_lock{m_head->m_mutex};
    Node* prev = m_head.get();
    Node* cur; 
    while((cur = prev->m_next.get())) {
      std::unique_lock<std::mutex> cur_lock{cur->m_mutex};
      prev_lock.unlock();
      if (pred(static_cast<const ElemT&>(*cur->m_data))) {
        return cur->m_data;
      }
      prev = cur;
      prev_lock = std::move(cur_lock);
    }
    return std::shared_ptr<ElemT>();
  }

  template<typename Predicate>
  void erase_if(Predicate&& pred) {
    std::unique_lock<std::mutex> prev_lock{m_head->m_mutex};
    Node* prev = m_head.get();
    Node* cur; 
    while((cur = prev->m_next.get())) {
      std::unique_lock<std::mutex> cur_lock{cur->m_mutex};
      if (pred(static_cast<const ElemT&>(*cur->m_data))) {
        std::unique_ptr<Node> dead{std::move(prev->m_next)};
        prev->m_next = std::move(cur->m_next);
        cur_lock.unlock();
      }
      else {
        prev = cur;
        prev_lock = std::move(cur_lock);
      }
    }
  }

};
template class ConcurrentList<int>;
