#include <cstdio>
#include <memory>
#include <vector>

#include <gc/gc_allocator.h>

#include "mouselisp.hpp"

namespace mouselisp {

class Machine {
  std::allocator<int> alloc_int;
  std::vector<int *> ints;

  int *new_int(int x);

public:
  void allocate_ints(void);
  void print_ints(void);
};

int *Machine::new_int(int x)
{
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
  for (const auto& i : this->ints)
    printf("%d ", *i);
  printf("\n");
}

} // namespace mouselisp

int main(int argc, char **argv) {
  GC_INIT();

  mouselisp::Machine the_machine;
  the_machine.allocate_ints();
  the_machine.print_ints();

  return 0;
}
