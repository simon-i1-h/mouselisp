#include <stdint.h>
#include <string.h>

#include "../mouselisp.h"

#define ml_test(b) ml_test2(__FILE__, stringify(__LINE__), stringify(b), b)
void ml_test2(const char *filename, const char *line, const char *strb,
              int b) {
  if (!b)
    fatal2(filename, line, "test failed: %s", strb);
}

int ptrarr_in(uintptr_t x, uintptr_t *array, size_t len, int nxs) {
  int count = 0;

  for (size_t i = 0; i < len; i++)
    if (array[i] == x)
      count++;
  return count == nxs;
}

/* object.c */
void test_object(void) {
  /* nil */
  ml_test(the_nil->tag == ML_OBJECT_NIL);

  /* number */
  {
    ml_object *num = ml_object_new_number(0);
    ml_test(num->tag == ML_OBJECT_NUMBER);
    ml_test(num->num == 0);
  }
  {
    ml_object *num = ml_object_new_number(42);
    ml_test(num->tag == ML_OBJECT_NUMBER);
    ml_test(num->num == 42);
  }
  {
    ml_object *num = ml_object_new_number(-42);
    ml_test(num->tag == ML_OBJECT_NUMBER);
    ml_test(num->num == -42);
  }

  /* bool */
  {
    ml_object *b = ml_object_new_bool(0);
    ml_test(b->tag == ML_OBJECT_BOOL);
    ml_test(b->boolean == 0);
  }
  {
    ml_object *b = ml_object_new_bool(1);
    ml_test(b->tag == ML_OBJECT_BOOL);
    ml_test(b->boolean == 1);
  }

  /* string */
  {
    ml_object *str = ml_object_new_string("");
    ml_test(str->tag == ML_OBJECT_STRING);
    ml_string expected = ml_string_new_str("");
    ml_test(ml_string_strcmp(&str->str, &expected) == 0);
  }
  {
    ml_object *str = ml_object_new_string("fox");
    ml_test(str->tag == ML_OBJECT_STRING);
    ml_string expected = ml_string_new_str("fox");
    ml_test(ml_string_strcmp(&str->str, &expected) == 0);
  }

  /* cons */
  {
    ml_object *car = ml_object_new_number(42);
    ml_object *cdr = ml_object_new_bool(0);
    ml_object *cons = ml_object_new_cons(car, cdr);
    uintptr_t ptrs[3] = {(uintptr_t)cons->cons.car,
                         (uintptr_t)cons->cons.cdr, (uintptr_t)cons};
    ml_test(cons->tag == ML_OBJECT_CONS);
    ml_test(cons->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(cons->cons.car->num == 42);
    ml_test(cons->cons.cdr->tag == ML_OBJECT_BOOL);
    ml_test(cons->cons.cdr->boolean == 0);
    ml_test(ptrarr_in((uintptr_t)car, ptrs, countof(ptrs), 1));
    ml_test(ptrarr_in((uintptr_t)cdr, ptrs, countof(ptrs), 1));
    ml_test(ptrarr_in((uintptr_t)cons, ptrs, countof(ptrs), 1));
  }

  /* same reference */
  {
    ml_object *b = ml_object_new_bool(1);
    ml_object *cons = ml_object_new_cons(b, b);
    uintptr_t ptrs[3] = {(uintptr_t)cons->cons.car,
                         (uintptr_t)cons->cons.cdr, (uintptr_t)cons};
    ml_test(ptrarr_in((uintptr_t)b, ptrs, countof(ptrs), 2));
    ml_test(ptrarr_in((uintptr_t)cons, ptrs, countof(ptrs), 1));
    ml_test(cons->cons.car == cons->cons.cdr);
  }
}

/* parser.c */
void test_parser(void) {
  /* nil literal */
  {
    ml_parser parser = ml_parser_new_str("nil");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = root->cons.car;
    ml_test(result == the_nil);
  }

  /* bool literal */
  {
    ml_parser parser = ml_parser_new_str("true");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = root->cons.car;
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_parser parser = ml_parser_new_str("false");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = root->cons.car;
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
}

/* machine.c */
void test_machine(void) {
  /* simple add() */
  {
    ml_machine machine = ml_machine_new();
    ml_object *f = ml_object_new_name("+");
    ml_object *a = ml_object_new_number(32);
    ml_object *b = ml_object_new_number(-2);
    ml_object *list3 = ml_object_new_cons(b, the_nil);
    ml_object *list2 = ml_object_new_cons(a, list3);
    ml_object *list1 = ml_object_new_cons(f, list2);
    ml_object *result = ml_machine_eval(&machine, list1);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 30);
  }
}

void test_top(void) {
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(+ 3 5)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 8);
  }

  /* nested */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(+ 1 (+ 2 3))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 6);
  }
}

/* builtin.c */
void test_builtin(void) {
  /* simple add() */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(+ 45 15)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 60);
  }

  /* simple sub() */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(- 45 15)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 30);
  }

  /* simple mul() */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(* 45 15)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 675);
  }

  /* simple div() */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(/ 45 15)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 3);
  }

  /* cons */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(cons 0 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.car->num == 0);
    ml_test(result->cons.cdr->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.cdr->num == 1);
  }

  /* car */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(car (cons 0 1))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 0);
  }

  /* cdr */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(cdr (cons 0 1))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 1);
  }
}

void test_special_forms(void) {
  /* if */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(if true 20 50)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 20);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(if false 20 50)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 50);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(if (car (cons true false)) (car (cons 10 20)) (cdr (cons 30 40)))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 10);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(if (cdr (cons true false)) (car (cons 10 20)) (cdr (cons 30 40)))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 40);
  }

  /* define */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(define g1 25) g1";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result;
    for (ml_object *elem = root; elem != the_nil; elem = elem->cons.cdr)
      result = ml_machine_eval(&machine, elem->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 25);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(define g1 25) (+ g1 15)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result;
    for (ml_object *elem = root; elem != the_nil; elem = elem->cons.cdr)
      result = ml_machine_eval(&machine, elem->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 40);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(define g1 (car (cons 15 35))) g1";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result;
    for (ml_object *elem = root; elem != the_nil; elem = elem->cons.cdr)
      result = ml_machine_eval(&machine, elem->cons.car);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 15);
  }
}

void test_main(void) {
  test_object();
  test_parser();
  test_machine();
  test_top();
  test_builtin();
  test_special_forms();
}

void test_object_dump_main(const char *testname) {
  if (strcmp(testname, "valid-nil") == 0) {
    ml_object_debug_dump(the_nil);
  } else if (strcmp(testname, "invalid-nil") == 0) {
    ml_object *invalid_nil = ml_object_new_nil();
    ml_object_debug_dump(invalid_nil);
  } else if (strcmp(testname, "number") == 0) {
    ml_object *num = ml_object_new_number(42);
    ml_object_debug_dump(num);
  } else if (strcmp(testname, "bool") == 0) {
    ml_object *b = ml_object_new_bool(1);
    ml_object_debug_dump(b);
  } else if (strcmp(testname, "string") == 0) {
    ml_object *str = ml_object_new_string("dog");
    ml_object_debug_dump(str);
  } else if (strcmp(testname, "cons") == 0) {
    ml_object *car = ml_object_new_number(-42);
    ml_object *cdr = ml_object_new_bool(0);
    ml_object *cons = ml_object_new_cons(car, cdr);
    ml_object_debug_dump(cons);
  } else if (strcmp(testname, "same-reference") == 0) {
    ml_object *b = ml_object_new_bool(1);
    ml_object *cons = ml_object_new_cons(b, b);
    ml_object_debug_dump(cons);
  } else if (strcmp(testname, "circular-list") == 0) {
    ml_object *list3 = ml_object_new_cons(
        ml_object_new_string("3rd"), the_nil);
    ml_object *list2 = ml_object_new_cons(
        ml_object_new_string("2nd"), list3);
    ml_object *list1 = ml_object_new_cons(
        ml_object_new_string("1st"), list2);
    list3->cons.cdr = list1;
    ml_object_debug_dump(list1);
  } else {
    fatal("unknown testname: %s", testname);
  }
}
