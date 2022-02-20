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
  ml_test(the_nil->tag == ML_OBJECT_POINTER);
  ml_test(the_nil->ptr == NULL);

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

  /* name */
  {
    ml_object *str = ml_object_new_name("");
    ml_test(str->tag == ML_OBJECT_NAME);
    ml_test(strcmp(str->str.str, "") == 0);
  }
  {
    ml_object *str = ml_object_new_name("fox");
    ml_test(str->tag == ML_OBJECT_NAME);
    ml_test(strcmp(str->str.str, "fox") == 0);
  }

  /* cons */
  {
    ml_object *car = ml_object_new_number(42);
    ml_object *cdr = ml_object_new_bool(0);
    ml_object *cons = ml_object_new_cons(car, cdr);
    uintptr_t ptrs[3] = {(uintptr_t)cons->cons.car, (uintptr_t)cons->cons.cdr,
                         (uintptr_t)cons};
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
    uintptr_t ptrs[3] = {(uintptr_t)cons->cons.car, (uintptr_t)cons->cons.cdr,
                         (uintptr_t)cons};
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

  /* comment */
  {
    ml_parser parser = ml_parser_new_str("; comment...\n"
                                         "(+ 1 2)\n");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
  }
  {
    /* EOF */
    ml_parser parser = ml_parser_new_str("(+ 1 2)\n"
                                         "; comment...");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
  }
  {
    ml_parser parser = ml_parser_new_str("3; comment...\n");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
  }
  {
    ml_parser parser = ml_parser_new_str("(+ 1 3); comment...\n");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
  }
  {
    ml_parser parser = ml_parser_new_str("(+; add\n1; a\n3; b\n); end\n");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
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
    ml_object *result = ml_machine_xeval(&machine, list1);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 3);
  }

  /* simple mod() */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(mod 45 15)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 0);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(mod 45 10)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 5);
  }

  /* cons */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(cons 0 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 1);
  }

  /* ref */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(& 42)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_POINTER);
    ml_test(result->ptr != NULL);
  }

  /* deref */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(-> (& 42))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 42);
  }

  /* eq */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(= 2 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(= 2 3)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(= 3 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* gt */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(> 2 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(> 2 3)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(> 3 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }

  /* lt */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(< 2 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(< 2 3)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(< 3 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* ge */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(>= 2 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(>= 2 3)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(>= 3 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }

  /* le */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(<= 2 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(<= 2 3)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(<= 3 2)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* and */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(and false false)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(and true false)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(and false true)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(and true true)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }

  /* or */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(or false false)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(or true false)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(or false true)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(or true true)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }

  /* not */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(not false)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(not true)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* is_cons */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(cons? (cons 1 2))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(cons? 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* is_bool */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(bool? false)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(bool? 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* is_num */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(num? 0)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(num? true)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* is_name */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(name? (quote x))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(name? 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* is_func */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(fn? (fn nil nil))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(fn? 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* is_ptr */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(ptr? (& 1))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(ptr? nil)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(result->boolean);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(ptr? 1)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_BOOL);
    ml_test(!result->boolean);
  }

  /* list */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(list 1 2 (+ 1 2))");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.car->num == 1);
    result = result->cons.cdr;
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.car->num == 2);
    result = result->cons.cdr;
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.car->num == 3);
    result = result->cons.cdr;
    ml_test(result == the_nil);
  }

  /* unique */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str(
        "(def my-max\n"
        "  (macro (a b)\n"
        "    (do\n"
        "      (def a_u (unique))\n"
        "      (def b_u (unique))\n"
        "      (list\n"
        "        (quote do)\n"
        "        (list (quote def) a_u a)\n"
        "        (list (quote def) b_u b)\n"
        "        (list (quote if) (list (quote >) a_u b_u) a_u b_u)))))\n"
        "(my-max 3 1)\n");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 3);
  }

  /* eval error */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(eval-error)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_POINTER);
    ml_test(result->cons.car->ptr != the_nil);
    result = result->cons.cdr;
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->cons.car->str.str, "eval-error") == 0);
    result = result->cons.cdr;
    ml_test(result == the_nil);
  }

  /* nil error */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(nil-error)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_POINTER);
    ml_test(result->cons.car->ptr != the_nil);
    result = result->cons.cdr;
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->cons.car->str.str, "nil-error") == 0);
    result = result->cons.cdr;
    ml_test(result == the_nil);
  }

  /* noname error */
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(noname-error)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_POINTER);
    ml_test(result->cons.car->ptr != the_nil);
    result = result->cons.cdr;
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->cons.car->str.str, "noname-error") == 0);
    result = result->cons.cdr;
    ml_test(result == the_nil);
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
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 20);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(if false 20 50)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 50);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code =
        "(if (car (cons true false)) (car (cons 10 20)) (cdr (cons 30 40)))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 10);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code =
        "(if (cdr (cons true false)) (car (cons 10 20)) (cdr (cons 30 40)))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 40);
  }

  /* def */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def g1 25) g1";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 25);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def g1 25) (+ g1 15)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 40);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def g1 (car (cons 15 35))) g1";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 15);
  }

  /* fn */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "((fn (x) (+ x 1)) 9)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 10);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def f1 (fn (x) (+ x 1))) (f1 9)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 10);
  }
  {
    /* simple closure */
    ml_machine machine = ml_machine_new();
    const char *code = "(def f1 (fn (x) (fn (y) (+ x y)))) ((f1 9) 1)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 10);
  }
  {
    /* complex closure */
    ml_machine machine = ml_machine_new();
    const char *code = "(def f1 (fn (x) (+ v1 x))) (def v1 3) (f1 0)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 3);
  }
  {
    /* empty list (nil) */
    ml_machine machine = ml_machine_new();
    const char *code = "(def f1 (fn nil (+ 3 2))) (f1)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 5);
  }
  {
    /* direct apply */
    ml_machine machine = ml_machine_new();
    const char *code = "((fn nil 3))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 3);
  }
  {
    /* restore table */
    ml_machine machine = ml_machine_new();
    const char *code =
        "(def f1 (fn (x) x)) (def f2 (fn (x) (do (f1 3) x))) (f2 10)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 10);
  }

  /* do */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(do)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result == the_nil);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(do 2 4 6)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 6);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def f (fn (x) x)) (do (f 1) (f 2) (f 3))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 3);
  }

  /* quote */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(quote x)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->str.str, "x") == 0);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(quote (x))";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->cons.car->str.str, "x") == 0);
    ml_test(result->cons.cdr == the_nil);
  }

  /* set */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def x 1) (set x 8) x";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 8);
  }

  /* setcar */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def x (cons 1 2)) (setcar x 9) x";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.car->num == 9);
    ml_test(result->cons.cdr->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.cdr->num == 2);
  }

  /* setcdr */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def x (cons 1 2)) (setcdr x 9) x";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_CONS);
    ml_test(result->cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.car->num == 1);
    ml_test(result->cons.cdr->tag == ML_OBJECT_NUMBER);
    ml_test(result->cons.cdr->num == 9);
  }

  /* macro */
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def my-when\n"
                       "  (macro (cond body)\n"
                       "    (list (quote if) cond body nil)))\n"
                       "(my-when (or true false) (+ 40 2))\n";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->num == 42);
  }
  {
    ml_machine machine = ml_machine_new();
    const char *code = "(def my-when\n"
                       "  (macro (cond body)\n"
                       "    (list (quote if) cond body nil)))\n"
                       "(my-when (and true false) (+ 40 2))\n";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result == the_nil);
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
  if (strcmp(testname, "nil") == 0) {
    ml_object_debug_dump(the_nil);
  } else if (strcmp(testname, "number") == 0) {
    ml_object *num = ml_object_new_number(42);
    ml_object_debug_dump(num);
  } else if (strcmp(testname, "bool") == 0) {
    ml_object *b = ml_object_new_bool(1);
    ml_object_debug_dump(b);
  } else if (strcmp(testname, "name") == 0) {
    ml_object *str = ml_object_new_name("dog");
    ml_object_debug_dump(str);
  } else if (strcmp(testname, "cons") == 0) {
    ml_object *car = ml_object_new_number(-42);
    ml_object *cdr = ml_object_new_bool(0);
    ml_object *cons = ml_object_new_cons(car, cdr);
    ml_object_debug_dump(cons);
  } else if (strcmp(testname, "builtin-function") == 0) {
    ml_machine machine = ml_machine_new();
    const char *code = "+";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_object_debug_dump(result);
  } else if (strcmp(testname, "normal-function") == 0) {
    ml_machine machine = ml_machine_new();
    const char *code = "(fn (x) x)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_object_debug_dump(result);
  } else if (strcmp(testname, "pointer") == 0) {
    ml_object *fake = ml_object_new_number(42);
    ml_object *str = ml_object_new_pointer(fake);
    ml_object_debug_dump(str);
  } else if (strcmp(testname, "macro") == 0) {
    ml_machine machine = ml_machine_new();
    const char *code = "(macro (x) x)";
    ml_parser parser = ml_parser_new_str(code);
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_object_debug_dump(result);
  } else if (strcmp(testname, "same-reference") == 0) {
    ml_object *b = ml_object_new_bool(1);
    ml_object *cons = ml_object_new_cons(b, b);
    ml_object_debug_dump(cons);
  } else if (strcmp(testname, "circular-list") == 0) {
    ml_object *list3 = ml_object_new_cons(ml_object_new_name("3rd"), the_nil);
    ml_object *list2 = ml_object_new_cons(ml_object_new_name("2nd"), list3);
    ml_object *list1 = ml_object_new_cons(ml_object_new_name("1st"), list2);
    list3->cons.cdr = list1;
    ml_object_debug_dump(list1);
  } else {
    fatal("unknown testname: %s", testname);
  }
}

void test_unique(void) {
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(unique)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->str.str, "$0.0") == 0);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(unique)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->str.str, "$0.1") == 0);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(unique)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->str.str, "$0.2") == 0);
  }
  {
    ml_machine machine = ml_machine_new();
    ml_parser parser = ml_parser_new_str("(unique)");
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_xeval_top(&machine, root);
    ml_test(result->tag == ML_OBJECT_NAME);
    ml_test(strcmp(result->str.str, "$0.3") == 0);
  }
}
