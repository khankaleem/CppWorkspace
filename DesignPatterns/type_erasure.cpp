#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>



class Circle {
};
class Square {
};

void draw(const Circle&) {
  std::cout << "circle drawn!\n";
}
void draw(const Square&) {
  std::cout << "square drawn!\n";
}

class Shape {
  private:

        // External Polymorphism : make any type behave polymprphically
        class Concept {
          public:
            virtual ~Concept() = default;
            virtual void draw() const = 0;
            virtual std::unique_ptr<Concept> clone() const = 0;
        };
        template<typename ElemT>
        class Model : public Concept {
          public:
            Model(ElemT&& elem_) : m_elem(std::move(elem_)) {}

            // Prototype
            std::unique_ptr<Concept> clone() const override {
              return std::make_unique<Model<ElemT>>(*this);
            }
            // Operations
            void draw() const override {
              ::draw(m_elem);
            }

          private:
            ElemT m_elem;
        };

  public:

    // Ctor
    template<typename ElemT, typename = std::enable_if_t<!std::is_same_v<std::decay_t<ElemT>, Shape>>>
    Shape(ElemT&& elem_) : m_pimpl{std::make_unique<Model<ElemT>>(std::move(elem_))} {}

    // Rule of 5
    Shape(Shape&& other) = default;
    Shape& operator=(Shape&& other) = default;
    Shape(const Shape& other) : m_pimpl{other.m_pimpl->clone()} {
    }
    Shape& operator=(const Shape& other) {
      m_pimpl = other.m_pimpl->clone();
      return *this;
    }

    friend void draw(const Shape& shape_) {
      shape_.m_pimpl->draw();
    }

  private:
    // Bridge
    std::unique_ptr<Concept> m_pimpl;

};




int main() {

  std::vector<Shape> vec;
  vec.push_back(Circle{});
  vec.push_back(Square{});
  for (auto& elt: vec) {
    draw(elt);
  }
  for (int i = 0; i < 2; i++) {
    Shape s{vec[i]};
    draw(s);
  }



}