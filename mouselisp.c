#include <string.h>

#include "mouselisp.h"

int main(int argc, char **argv) {
  mouselisp_init();

  if (argc > 1 && strcmp(argv[1], "--test") == 0) {
    test_main();
    return 0;
  }

  if (argc > 2 && strcmp(argv[1], "--dump") == 0) {
    ml_parser parser =
        (ml_parser){.file = (ml_file){.str = argv[2]}};
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object_debug_dump(root);
    return 0;
  }

  return 1;
}
