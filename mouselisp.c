#include <string.h>

#include "mouselisp.h"

int main(int argc, char **argv) {
  mouselisp_init();

  if (argc > 1 && strcmp(argv[1], "--test") == 0) {
    test_main();
    return 0;
  }

  return 0;
}
