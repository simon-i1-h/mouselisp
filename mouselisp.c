#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include "mouselisp.h"

char *ml_read(void) {
  ml_string ret = ml_string_new_str("");
  int c;

  for (;;) {
    c = fgetc(stdin);
    if (feof(stdin))
      return NULL;
    if (c == '\n')
      return ret.str;
    ml_string_concat_char(&ret, c);
  }
}

void ml_repl(void) {
  ml_machine machine = ml_machine_new();

  for (;;) {
    printf("ml> ");

    char *str = ml_read();
    /* EOF */
    if (str == NULL)
      return;
    ml_parser parser = ml_parser_new_str(str);
    ml_object *root;

    /* top level error handling */
    ml_object *prev_named_objs = machine.named_objs;
    jmp_buf curr_jmpbuf;
    machine.last_exc_handler = &curr_jmpbuf;
    machine.exc = the_nil;
    if (setjmp(curr_jmpbuf) == 0) {
      root = ml_parser_parse(&parser, &machine);
    } else {
      machine.named_objs = prev_named_objs;
      logmsg("error: %s", machine.exc->cons.cdr->cons.car->str.str);
      continue;
    }

    for (ml_object *list = root; list != the_nil; list = list->cons.cdr) {
      /* top level error handling */
      ml_object *prev_named_objs = machine.named_objs;
      jmp_buf curr_jmpbuf;
      machine.last_exc_handler = &curr_jmpbuf;
      machine.exc = the_nil;
      if (setjmp(curr_jmpbuf) == 0) {
        ml_object_debug_dump(ml_machine_eval(&machine, list->cons.car));
      } else {
        machine.named_objs = prev_named_objs;
        logmsg("error: %s", machine.exc->cons.cdr->cons.car->str.str);
        continue;
      }
    }
  }
}

int main(int argc, char **argv) {
  mouselisp_init();

  if (argc == 1) {
    ml_repl();
    return 0;
  }

  if (argc > 1 && strcmp(argv[1], "--test") == 0) {
    test_main();
    return 0;
  }

  if (argc > 2 && strcmp(argv[1], "--test-object-dump") == 0) {
    test_object_dump_main(argv[2]);
    return 0;
  }

  if (argc > 1 && strcmp(argv[1], "--test-unique") == 0) {
    test_unique();
    return 0;
  }

  fprintf(stderr, "unknown options.");
  return 1;
}
