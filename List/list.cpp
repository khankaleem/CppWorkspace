#include<iostream>

template<typename ElemT>
class List {
  
  struct Node {
    ElemT val;
    Node* prev{};
    Node* next{};

    template<typename U>
    explicit Node(U&& val_) : val{std::forward<U>(val_)} {}

    template<typename U>
    explicit Node(U&& val_, Node* prev_, Node* next_) : val{std::forward<U>(val_)}, prev{prev_}, next{next_} {}

    template<typename... ArgsT>
    explicit Node(Node* prev_, Node* next_, ArgsT&&... args_) : val{std::forward<ArgsT>(args_)...},  prev{prev_}, next{next_} {}
  };

  Node* m_head{};
  Node* m_tail{};
  size_t m_size{};

  void release(Node* ptr) {
    ptr->next = nullptr;
    ptr->prev = nullptr;
    delete ptr;
  }

  void remove(Node* ptr) {
    if (ptr->prev != nullptr) {
      ptr->prev->next = ptr->next;
    }
    else {
      m_head = ptr->next;
    }
    if (ptr->next != nullptr) {
      ptr->next->prev = ptr->prev;
    }
    else {
      m_tail = ptr->prev;
    }
    release(ptr);
  }

public:

  template<bool IsConst>
  class IteratorBase {
    using PointerT = std::conditional_t<IsConst, const ElemT*, ElemT*>;
    using referenceT = std::conditional_t<IsConst, const ElemT&, ElemT&>;

    Node* m_cur;
    friend class List; // Allows List to access m_cur for erase/insert

    public:
    // required for <algorithm> functions
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = ElemT;
    using pointer           = PointerT;
    using reference         = referenceT;

    explicit IteratorBase(Node* node_) : m_cur{node_} {}
    
    template<bool OtherIsConstT, typename = std::enable_if_t<IsConst >= OtherIsConstT, void>>
    IteratorBase (const IteratorBase<OtherIsConstT>& other) : m_cur{other.m_cur} {}

    
    IteratorBase& operator++() {
      m_cur = m_cur->next;
      return *this;
    }
    IteratorBase& operator--() {
      m_cur = m_cur->prev;
      return *this;
    }
    reference operator*() const { // non const in reality : shallow constness
      return m_cur->val;
    }
    pointer operator->() const {
      return &(m_cur->val);
    }
    friend bool operator==(const IteratorBase& f, const IteratorBase& s) {
      return f.m_cur == s.m_cur;    
    }
    friend bool operator!=(const IteratorBase& f, const IteratorBase& s) {
      return f.m_cur != s.m_cur;
    }
  };
  using Iterator = IteratorBase<false>;
  using ConstIterator = IteratorBase<true>;

  Iterator begin() {
    return Iterator(m_head);
  }
  Iterator end() {
    return Iterator(nullptr);
  }
  ConstIterator begin() const {
    return ConstIterator(m_head);
  }
  ConstIterator end() const {
    return ConstIterator(nullptr);
  }

  List() = default;
  List(std::initializer_list<ElemT> initlist_) { // read only list
    for (auto &x: initlist_) {
      push_back(x); // always copies
    }
  }

  // Rule of 5
  List(const List& other) { // Copy Ctor
    const Node* ptr = other.m_head;
    while (ptr != nullptr) {
      push_back(ptr->val);
      ptr = ptr->next;
    }
  }
  List(List&& other) noexcept : m_head{other.m_head}, m_tail{other.m_tail}, m_size{other.m_size} { //Move Ctor
    other.m_head = nullptr;
    other.m_tail = nullptr;
    other.m_size = 0;
  }
  List& operator=(const List& other) { //copy assignment
    if (&other != this) {
      // clear this list first
      clear();
      // copy
      const Node* ptr = other.m_head;
      while (ptr != nullptr) {
        push_back(ptr->val);
        ptr = ptr->next;
      }
    }
    return *this;
  }
  List& operator=(List&& other) { // Move assignemnt
    if (&other != this) {
      // clear this list first
      clear();

      m_head = other.m_head;
      m_tail = other.m_tail;
      m_size = other.m_size;

      other.m_head = nullptr;
      other.m_tail = nullptr;
      other.m_size = 0;
    }
    return *this;
  }
  ~List() {
    clear();
  }
  // Rule of 5 end

  // push_back lvalue
  void push_back(const ElemT& val) {
    m_size++;
    if (m_head == nullptr) {
      m_head = m_tail = new Node{val};
      return;
    }
    m_tail->next = new Node{val, m_tail, nullptr};
    m_tail = m_tail->next;
  }

  // push_back rvalue
  void push_back(ElemT&& val) {
    m_size++;
    if (m_head == nullptr) {
      m_head = m_tail = new Node{std::move(val)};
      return;
    }
    m_tail->next = new Node{std::move(val), m_tail, nullptr};
    m_tail = m_tail->next;
  }

  template<typename... ArgsT>
  void emplace_back(ArgsT&&... args_) {
    m_size++;
    if (m_head == nullptr) {
        m_head = m_tail = new Node{nullptr, nullptr, std::forward<ArgsT>(args_)...};
        return;
    }
    m_tail->next = new Node{m_tail, nullptr, std::forward<ArgsT>(args_)...};
    m_tail = m_tail->next;
  }

  Iterator insert(ConstIterator pos, const ElemT& val) {
    Node* curr = pos.m_cur;
    if (curr == nullptr) {
      push_back(val);
      return Iterator(m_tail);
    }
    m_size++;
    Node* newNode = new Node(val, curr->prev, curr);
    if (curr->prev != nullptr) {
      curr->prev->next = newNode;
    } else {
      m_head = newNode; // Inserting at the very front
    }
    curr->prev = newNode;
    return Iterator(newNode);
  }

  // erase
  void erase(const ElemT& val) {
    auto ptr = m_head;
    while(ptr != nullptr && ptr->val != val) {
      ptr = ptr->next;
    }
    if (ptr == nullptr) { return; }

    m_size--;
    remove(ptr);
  }

  Iterator erase(ConstIterator it) {
    Node* ptr = it.m_cur; 
    if (ptr == nullptr) return end();
    Node* nextNode = ptr->next;
    remove(ptr);
    return Iterator(nextNode);
  }

  // empty
  bool empty() const {
    return m_size == 0;
  }

  // size
  size_t size() const {
    return m_size;
  }

  // clear
  void clear() {
    while(m_head != nullptr) {
      auto next = m_head->next;
      release(m_head);
      m_head = next;
    }
    m_tail = nullptr;
  }
};


int main() {
  List<int> l;
  l.push_back(100);
  l.erase(100);
  l.push_back(200);
  List<int> r;
  r = l;
  r.push_back(100);

  for (auto& x: r) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << *std::max_element(r.begin(), r.end()) << '\n';
}


