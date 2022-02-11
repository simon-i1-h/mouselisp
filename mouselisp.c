#include <stdio.h>
#include <string.h>

#include "mouselisp.h"

int main(int argc, char **argv) {
  mouselisp_init();

  if (argc > 1 && strcmp(argv[1], "--test") == 0) {
    test_main();
    return 0;
  }

  if (argc > 2 && strcmp(argv[1], "--test-object-dump") == 0) {
    test_object_dump_main(argv[2]);
    return 0;
  }

  fprintf(stderr, "unknown options.");
  return 1;
}
