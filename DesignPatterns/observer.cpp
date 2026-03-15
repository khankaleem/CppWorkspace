#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>

// Observer Interface
class Observer {
public:
  Observer() = default;
  ~Observer() = default;
  virtual void update(const std::string& symbol, double price) = 0;
};

// Subject or Observable
template<typename ObserverT>
class Stock {
  std::string m_symbol{};
  double m_price{};
  std::vector<std::weak_ptr<ObserverT>> m_observers;

public:
  Stock(std::string symbol, double price_ = 0) : m_symbol{std::move(symbol)}, m_price{price_} {};

  void attach(std::shared_ptr<ObserverT> observer) {
    m_observers.push_back(std::weak_ptr<ObserverT>(observer));
  }

  void update(double price_) {
    m_price = price_;
    notify_observers(m_price);
  }
  void notify_observers(double price_) {
    for (auto& observer: m_observers) {
      auto ptr = observer.lock();
      if (ptr != nullptr) {
        ptr->update(m_symbol, price_);
      }
    }
  }
};

class DisplayPrice : public Observer {
public:
  DisplayPrice() = default;
  virtual void update(const std::string& symbol, double price) override {
    std::cout << "Symbol:" <<  symbol << "|Price:" << price << '\n';
  }
};

int main() {
  Stock<Observer> stk{"TSLA", 125};
  {
    std::shared_ptr<DisplayPrice> display = std::make_shared<DisplayPrice>();
    stk.attach(display);
    stk.update(150);
    stk.update(250);
  }
  stk.update(300);
}