#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>


// Dynamic Decorator
class DataSource {
public:
  DataSource() = default;
  virtual void write_data() = 0;
  virtual void read_data() = 0;
};

class FileDataSource : public DataSource {
public:
  FileDataSource() = default;
  virtual void write_data() override {
    std::cout << "write data\n";
  }
  virtual void read_data() override {
    std::cout << "read data\n";
  }
};

class EncryptedFileDataSource : public DataSource {
  DataSource* m_underlying;
  void encrypt() {
  }
  void decrypt() {
  }
public:
  EncryptedFileDataSource(DataSource* underlying_) : m_underlying{underlying_} {}
  virtual void write_data() override {
    std::cout << "write encrypted data\n";
    encrypt();
    m_underlying->write_data();
  }
  virtual void read_data() override {
    m_underlying->read_data();
    decrypt();
    std::cout << "read decrypted data data\n";
  }
};




namespace two {

  class Shape {
  public:
    virtual double area() = 0;
    virtual double volume() = 0;
    virtual std::string str() = 0;
    ~Shape() = default;
  };

  class Circle : public Shape {
    int m_radius;
  public:
    using Base = Shape;
    Circle(int radius_) : Base{}, m_radius{radius_} {}
    void resize(int radius_) {
      m_radius = radius_;
    }
    double area() override {
      return m_radius * m_radius * 3.14;
    }
    double volume() override {
      return 0;
    }
    std::string str() override {
      std::ostringstream oss;
      oss << "|Shape:Circle|";
      return oss.str();
    }
  };

  enum class Color : uint8_t {
    RED=0,
    BLUE=1,
    GREEN=2
  };
  std::vector<std::string> ColorStr = { "RED", "BLUE", "GREEN" };

  class ColoredShape : public Shape {
    Color m_color;
    Shape* m_underlying;
  public:
    ColoredShape(Color color, Shape* underlying) : m_color{color}, m_underlying{underlying} {}
    double area() override {
      return m_underlying->area();
    }
    double volume() override {
      return m_underlying->volume();
    }
    std::string str() override {
      std::ostringstream oss;
      oss << m_underlying->str() << "Color: " << ColorStr[(int)m_color] << "|";
      return oss.str();
    }
  };

};

namespace one {

  class Shape {
  public:
    virtual double area() = 0;
    virtual double volume() = 0;
    virtual std::string str() = 0;
    ~Shape() = default;
  };

  class Circle : public Shape {
    int m_radius;
  public:
    using Base = Shape;
    Circle(int radius_) : Base{}, m_radius{radius_} {}
    void resize(int radius_) {
      m_radius = radius_;
    }
    double area() override {
      return m_radius * m_radius * 3.14;
    }
    double volume() override {
      return 0;
    }
    std::string str() override {
      std::ostringstream oss;
      oss << "|Shape:Circle|";
      return oss.str();
    }
  };

  enum class Color : uint8_t {
    RED=0,
    BLUE=1,
    GREEN=2
  };
  std::vector<std::string> ColorStr = { "RED", "BLUE", "GREEN" };

  template<typename ShapeT>
  class ColoredShape : public ShapeT {
    static_assert(std::is_base_of_v<Shape, ShapeT>, 
                    "ColoredShape: ShapeT must derive from Shape base class.");
    Color m_color;
  public:
    using Base = ShapeT;
    template<typename... Args>
    explicit ColoredShape(Color color, Args&&... args) : Base{std::forward<Args>(args)...}, m_color{color} {}
    std::string str() override {
      std::ostringstream oss;
      oss << Base::str() << "Color: " << ColorStr[(int)m_color] << "|";
      return oss.str();
    }
  };
}


namespace three {

// static function decorator
template<typename Function>
class FunctionDecorator {
  Function m_func;
public:
  template<typename T>
  FunctionDecorator(T&& func_) : m_func{std::forward<T>(func_)} {}

  template<typename... Args>
  auto operator()(Args&&... args) {
    std::cout << "Starting Function...\n";
    using ReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));
    if constexpr(std::is_same_v<ReturnType, void>) {
      m_func(std::forward<Args>(args)...);    
      std::cout << "Finished Function.\n";
    }
    else {
      auto ret = m_func(std::forward<Args>(args)...);      
      std::cout << "\nFinished Function";
      return ret;
    }
  }
};

};


int main() {
  auto lambda = []() -> double { std::cout << "called!"; return 3.14; };
  three::FunctionDecorator<decltype(lambda)> dec{lambda};
  std::cout << dec();
}
