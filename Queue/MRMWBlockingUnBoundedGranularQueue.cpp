#include<cassert>
#include<exception>
#include<mutex>
#include<condition_variable>
#include<stack>
#include<queue>
#include<iostream>

// Locked thread safe multireader multi writer unbounded queue
namespace ds {

template<typename T>
class Queue {

  struct Node {
    std::shared_ptr<T> m_data{};
    std::unique_ptr<Node> m_next{};
    Node() = default;
    Node(T&& data_) : m_data{std::make_shared<T>(std::move(data_))}{}
    Node(const T& data_) : m_data{std::make_shared<T>(data_)}{}
  };

  std::unique_ptr<Node> m_head{}; // pop at front
  Node* m_tail{}; // push at tail

  mutable std::mutex m_headMutex;
  mutable std::mutex m_tailMutex;
  std::condition_variable m_cond;

  Node* getTail() const {
    std::lock_guard<std::mutex> guard{m_tailMutex};
    return m_tail;
  }

  std::unique_ptr<Node> pop_head_unsafe() noexcept {
    std::unique_ptr<Node> oldHead{std::move(m_head)};
    m_head = std::move(oldHead->m_next);
    return oldHead;
  }

  std::unique_ptr<Node> try_pop_head() {
    std::lock_guard<std::mutex> guard{m_headMutex};
    if (m_head.get() == getTail()) {
      return std::unique_ptr<Node>{};
    }
    return pop_head_unsafe();
  }

  std::unique_ptr<Node> try_pop_head(T& fill) {
    std::lock_guard<std::mutex> guard{m_headMutex};
    if (m_head.get() == getTail()) {
      return std::unique_ptr<Node>{};
    }
    if constexpr(std::is_nothrow_move_assignable_v<T>) {
      fill = std::move(*m_head->m_data);
    }
    else if constexpr(std::is_copy_assignable_v<T>){
      fill = (*m_head->m_data);
    }
    else {
      static_assert(std::is_nothrow_move_assignable_v<T>, "Type must be movable or copiable");
    }
    return pop_head_unsafe();
  }

  std::unique_ptr<Node> wait_and_pop_head() {
    std::unique_lock<std::mutex> lock{m_headMutex};
    m_cond.wait(lock,
                [this](){ return m_head.get() != getTail();});
    return pop_head_unsafe();
  }

  std::unique_ptr<Node> wait_and_pop_head(T& fill) {
    std::unique_lock<std::mutex> lock{m_headMutex};
    m_cond.wait(lock,
                [this](){ return m_head.get() != getTail();});
    if constexpr(std::is_nothrow_move_assignable_v<T>) {
      fill = std::move(*m_head->m_data);
    }
    else if constexpr(std::is_copy_assignable_v<T>){
      fill = (*m_head->m_data);
    }
    else {
      static_assert(std::is_nothrow_move_assignable_v<T>, "Type must be movable or copiable");
    }
    return pop_head_unsafe();
  }

public:
  Queue() : m_head{std::make_unique<Node>()}, m_tail{m_head.get()}{}
  Queue(const Queue& other) = delete;
  Queue& operator=(const Queue& other) = delete;

  void push(const T& data) {
    std::unique_ptr<Node> newTail{std::make_unique<Node>()};
    std::shared_ptr<T> newData = std::make_shared<T>(data);
    Node* newTailRaw = newTail.get();
    {
      std::lock_guard<std::mutex> guard{m_tailMutex};
      m_tail->m_data = std::move(newData);
      m_tail->m_next = std::move(newTail);
      m_tail = newTailRaw;
    }
    m_cond.notify_one();
  }

  std::shared_ptr<T> try_pop() {
    std::unique_ptr<Node> oldHead = try_pop_head();
    return oldHead ? oldHead->m_data : std::shared_ptr<T>{};
  }

  bool try_pop(T& fill) {
    std::unique_ptr<Node> oldHead = try_pop_head(fill);
    return oldHead != nullptr;
  }

  std::shared_ptr<T> wait_and_pop() {
    std::unique_ptr<Node> oldHead = wait_and_pop_head();
    return oldHead ? oldHead->m_data : std::shared_ptr<T>{};
  }

  void wait_and_pop(T& fill) {
    std::unique_ptr<Node> oldHead = wait_and_pop_head(fill);
  }

  bool empty() const {
    std::unique_lock<std::mutex> lock{m_headMutex};
    return m_head.get() == getTail();
  }
};

template class Queue<int>;

}
