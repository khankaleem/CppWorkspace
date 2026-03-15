#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>
#include<cmath>

class PointFactory;

class Point {
  double x;
  double y;
  Point(double x_, double y_) : x{x_}, y{y_} {}

  //friend class PointFactory;
  class PointFactory {
      PointFactory() = default;
    public:
      static Point newPolar(double r, double theta) {
        return Point(r * cos(theta), r * sin(theta));
      }
      static Point newCartesian(double x, double y) {
        return Point(x, y);
      }
  };

public:

  static PointFactory Factory;

  friend std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "(" << p.x << ' ' << p.y << ")";
    return os;
  }
};

// class PointFactory {
// public:
//   PointFactory() = default;

//   static Point newPolar(double r, double theta) {
//     return Point(r * cos(theta), r * sin(theta));
//   }
//   static Point newCartesian(double x, double y) {
//     return Point(x, y);
//   }
// };


// Abstract Factory
class Button {
public:
  virtual ~Button() = default;
  virtual void paint() = 0;
};
class CheckBox {
public:
  virtual ~CheckBox() = default;
  virtual void render() = 0;
};
class WindowsButton : public Button {
public:
  WindowsButton() = default;
  virtual void paint() override {}
};
class WindowsCheckBox : public CheckBox {
public:
  WindowsCheckBox() = default;
  virtual void render() override {}
};
class GuiFactory {
public:
  ~GuiFactory() = default;
  virtual std::unique_ptr<Button> createButton() const = 0;
  virtual std::unique_ptr<CheckBox> createCheckBox() const = 0;
};
class WindowsFactory : public GuiFactory {
public:
  virtual std::unique_ptr<Button> createButton() const override {
    return std::make_unique<WindowsButton>();
  }
  virtual std::unique_ptr<CheckBox> createCheckBox() const override {
    return std::make_unique<WindowsCheckBox>();
  }
};
class Application {
  std::unique_ptr<Button> m_button;
  std::unique_ptr<CheckBox> m_checkBox;
public:
  Application(const GuiFactory& guiFactory_) : 
              m_button{guiFactory_.createButton()}, m_checkBox{guiFactory_.createCheckBox()} {} 
};




// Function Factory
class HotDrink {
public:
  virtual ~HotDrink() = default;
  virtual void prepare(int vol_) = 0;
};
class Tea : public HotDrink {
public:
  virtual void prepare(int vol_) override {
    std::cout << "Preparing tea for " << vol_ << " ml!";
  }
};
class Coffee : public HotDrink {
public:
  virtual void prepare(int vol_) override {
    std::cout << "Preparing coffee for " << vol_ << " ml!";
  }
};
class HotDrinkFactory {
public:
  virtual ~HotDrinkFactory() = default;
  virtual std::unique_ptr<HotDrink> make() = 0;
};
class TeaFactory : public HotDrinkFactory {
public:
  ~TeaFactory() = default;
  virtual std::unique_ptr<HotDrink> make() {
    return std::make_unique<Tea>();
  }
};
class CoffeeFactory : public HotDrinkFactory {
public:
  ~CoffeeFactory() = default;
  virtual std::unique_ptr<HotDrink> make() {
    return std::make_unique<Tea>();
  }
};
class DrinkFactory {
  std::unordered_map<std::string, std::unique_ptr<HotDrinkFactory>> m_factories;
public:
  DrinkFactory() {
    m_factories["Tea"] = std::make_unique<TeaFactory>();
    m_factories["Coffee"] = std::make_unique<CoffeeFactory>();
  }
  std::unique_ptr<HotDrink> make(const std::string& type, int vol_) {
    std::unique_ptr<HotDrink> ptr = m_factories[type]->make();
    ptr->prepare(vol_);
    return ptr;
  }
};
class DrinkFactoryFn {
  using CallBackType = std::unique_ptr<HotDrink>(*)(int);
  std::unordered_map<std::string, CallBackType> m_factories;
public:
  DrinkFactoryFn() {
    m_factories["Tea"] = [](int vol_) { 
      std::unique_ptr<HotDrink> ptr = std::make_unique<TeaFactory>()->make();
      ptr->prepare(vol_);
      return ptr;
    };
    m_factories["Coffee"] = [](int vol_) { 
      std::unique_ptr<HotDrink> ptr = std::make_unique<TeaFactory>()->make();
      ptr->prepare(vol_);
      return ptr;
    };
  }
  std::unique_ptr<HotDrink> make(const std::string& type, int vol_) {
    return m_factories[type](vol_);
  }
};




int main() {
  Point p = Point::Factory.newCartesian(100, 200);
  std::cout << p << '\n';
}
