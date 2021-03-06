#include "clion_bugs.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Slice {
 public:
  Slice() = default;
  template <size_t N>
  // todo:
  Slice(const char (&a)[N]) : size(N) {  // Can't resolve variable N
  }
  size_t size;
};
void f_slice(Slice slice) {
}
void bug_slice() {
  // https://youtrack.jetbrains.com/issue/CPP-7860#comment=27-1677997
  f_slice("hello");  // Parameter type mismatch: Types 'Slice' and 'const char[6]' are not compatible
}

enum class ScopeExit {};
template <class FunctionT>
void operator+(ScopeExit, FunctionT &&func) {
}
void bug_scope_exit() {
  // https://youtrack.jetbrains.com/issue/CPP-3585
  ScopeExit() + [&] {};  // Binary operator '+' can't be applied to the expression of type ScopeExit and 'void (*)()'
}

// https://youtrack.jetbrains.com/issue/CPP-5883
class SfinaeBug {
 public:
  template <class T>
  explicit SfinaeBug(T s, std::enable_if_t<std::is_same<int, T>::value, int> = 0) {
  }
  template <class T>
  explicit SfinaeBug(T s, std::enable_if_t<!std::is_same<int, T>::value, int> =
                              0)  // Duplicated declaration of constructor 'SfinaeBug'
  {
  }
};

// todo:
void bug_class_in_function_() {
  class A {
   public:
    A() = default;  // explicit type is required here
  };
  class B {
   public:
    void f() {
      std::make_unique<A>();  // Can't resolve variable A
    }
  };
}

// https://youtrack.jetbrains.com/issue/CPP-9277
class BugClassInFunctionInClass {
 public:
 private:
  void f() {
  }
  void g() {
    class C {
      C(BugClassInFunctionInClass *b) {
        b->f();  // Private 'BugClassInFunctionInClass::f()' is inaccessible
      }
    };
  }
};

// Declarations of A::Object and B::Object must be in the header
namespace A {
class Object {};
}  // namespace A
namespace B {
class Object {};
}  // namespace B
void bug_with_header(const A::Object &) {  // function 'bug_with_header' was redeclared with different type ...
}
void bug_with_header(const B::Object &) {  // function 'bug_with_header' was redeclared with different type ...
}

class ClassWithoutDefaultConstructor {
 public:
  ClassWithoutDefaultConstructor(std::unique_ptr<int> x) : x_(std::move(x)) {
  }

 private:
  std::unique_ptr<int> x_;
};

class ClassFieldWithoutDefaultConstructor {
 public:
  // https://youtrack.jetbrains.com/issue/CPP-2168
  ClassFieldWithoutDefaultConstructor() {  // field without_default must be initialized
  }

 private:
  ClassWithoutDefaultConstructor without_default{nullptr};
};

// https://youtrack.jetbrains.com/issue/CPP-1547
void bug_lamda_with_default_argument() {
  auto f = [](int a, int b = 1) {};
  f(1);  // Too few arguments, expected 2
};

// https://youtrack.jetbrains.com/issue/CPP-9587 (?)
void bug_lambda_return_string() {
  auto f = [] {
    return std::string("hello");  // Returning 'std::basic_string...' from lambda returning 'void'
  };
}

// https://youtrack.jetbrains.com/issue/CPP-8649 (fixed in master)
void bug_vector() {
  std::vector<int> a(1);
  std::vector<int> b;
  b.push_back(a.back());  // Parameter type mismatch: expression must be rvalue
};

int main() {
  return 0;
}
