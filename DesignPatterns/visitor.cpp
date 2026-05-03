#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>

// overloaded function set
template<typename... FunctorsT> 
struct OverloadedSet : public FunctorsT...  {

  template<typename... U>
  OverloadedSet(U&&... u) : FunctorsT(std::forward<U>(u))... {}
  
  using FunctorsT::operator()...;
};
// dduciton guidea s llambda have aninymous ttypes
template<typename... U>
OverloadedSet(U&&... u) -> OverloadedSet<U...>;



// visitor interface
template<typename... Head>
class VisitorBase;

template<typename Head>
class VisitorBase<Head> {
  public:
    virtual void visit(Head*) = 0;
};

template<typename Head, typename... Tail>
class VisitorBase<Head, Tail...> : public VisitorBase<Tail...> {
  public:
    using Base = VisitorBase<Tail...>;
    using Base::visit;
    virtual void visit(Head*) = 0;
};
using PetVisitor = VisitorBase<class Cat, class Dog>;



// visitor implemetnation using lambda overloads
template<typename...>
class LambdaVisitor;

template<typename Base, typename HeadT, typename... TailT, typename FunctorHead, typename... FunctorTail>
class LambdaVisitor<Base, VisitorBase<HeadT, TailT...>, FunctorHead, FunctorTail...> : 
  private FunctorHead, public LambdaVisitor<Base, VisitorBase<TailT...>, FunctorTail...> {
public:
    using Base1 = FunctorHead;
    using Base2 = LambdaVisitor<Base, VisitorBase<TailT...>, FunctorTail...>;

    LambdaVisitor(const FunctorHead& fhead_, const FunctorTail&... ftail_) :
          FunctorHead(fhead_),
          Base2(ftail_...)
    {}

    void visit(HeadT* p) override {
      return FunctorHead::operator()(p);
    }
};

template<typename Base, typename HeadT, typename FunctorHead>
class LambdaVisitor<Base, VisitorBase<HeadT>, FunctorHead> : 
  private FunctorHead, public Base {
public:
    using Base1 = FunctorHead;
    using Base2 = Base;

    LambdaVisitor(const FunctorHead& fhead_) :
          FunctorHead(fhead_),
          Base2()
    {}

    void visit(HeadT* p) override {
      return FunctorHead::operator()(p);
    }
};

template<typename... FunctorHead>
auto make_pet_visitor(const FunctorHead&... fhead_) {
  return LambdaVisitor<PetVisitor, PetVisitor, FunctorHead...>{fhead_...};
}





class Pet {
  public:
    virtual void accept(PetVisitor* visitor_impl)=0;
    virtual ~Pet() = default;
};

template<typename Derived>
class Visitable : public Pet {
public:
  virtual void accept(PetVisitor* visitor_impl) {
    visitor_impl->visit(static_cast<Derived*>(this));
  }
};

class Cat : public Visitable<Cat> {

};

class Dog : public Visitable<Dog>  {

};



int main() {
  Pet* p = new Cat{};
  auto visitor = make_pet_visitor([](Cat* cat){ std::cout << "Cat lambda\n"; }, [](Dog* dog){ std::cout << "Dog lambda\n"; });
  p->accept(&visitor);
}
