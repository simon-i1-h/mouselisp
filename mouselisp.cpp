#include <cstdio>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <gc/gc_allocator.h>
#include <gc/gc_cpp.h>

#include "mouselisp.hpp"

namespace mouselisp {

using String =
    std::basic_string<char, std::char_traits<char>, gc_allocator<char>>;

struct Object;

struct Cons : public gc_cleanup {
  Object *car;
  Object *cdr;
};

struct Nil {
  int fake;
};

struct Object : public std::variant<Cons, double, String, std::byte, Nil>,
                public gc_cleanup {};

class Machine {
  gc_allocator<int> alloc_int;
  std::vector<int *> ints;

  int *new_int(int x);

public:
  void allocate_ints(void);
  void print_ints(void);
};

int *Machine::new_int(int x) {
  int *ret = this->alloc_int.allocate(1);
  *ret = x;
  return ret;
}

void Machine::allocate_ints(void) {
  for (int i = 0; i < 10; i++) {
    ints.push_back(this->new_int(i));
  }
}

void Machine::print_ints(void) {
  logmsgf("");
  for (const auto &i : this->ints)
    rlogmsgf("%d ", *i);
  rlogmsg("");
}

} // namespace mouselisp

int main(void) {
  GC_INIT();

  mouselisp::Machine the_machine;
  the_machine.allocate_ints();
  the_machine.print_ints();

  return 0;
}
