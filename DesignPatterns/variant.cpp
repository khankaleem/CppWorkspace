#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>
#include<variant>

class Circle;
class Square;
class ShapeVisitorBase {
public:
    virtual ~ShapeVisitorBase() = default;
    virtual void visit(Circle& circle) = 0;
    virtual void visit(Square& circle) = 0; 
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitorBase* visitor) = 0;
};

class Circle : public Shape {
public:
    void accept(ShapeVisitorBase* visitor) override {
        visitor->visit(*this);
    }
};
class Square : public Shape {
public:
    void accept(ShapeVisitorBase* visitor) override {
        visitor->visit(*this);
    }
};

class Printer : public ShapeVisitorBase {
public:
    void visit(Circle& circle) override {
        std::cout << "Circle\n";
    }
    void visit(Square& circle) override {
        std::cout << "Square\n";
    }    
};

int main() {
    Circle c;
    Square s;

    // decoupling operations from types
    std::vector<Shape*> shapes{&c, &s};
    Printer printer{};
    for (auto shape: shapes) {
        shape->accept(&printer);
    }



    // open set of operations over a closed set of types 
    std::vector<std::variant<Circle, Square>> shapes2;
    shapes2.push_back(Circle{});
    shapes2.push_back(Square{});

    struct Printer2 {
        void operator()(Circle& circle) {
            std::cout << "Circle\n";
        }
        void operator()(Square& square) {
            std::cout << "Square\n";
        }
    };
    for (auto& elt: shapes2) {
        std::visit(Printer2{}, elt);
    }
}