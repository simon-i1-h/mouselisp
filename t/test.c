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

/* TODO: debug dumpテストはPythonスクリプトで行う。 */

/* object.c */
void test_object(void) {
  /* nil */
  ml_test(the_nil->tag == ML_OBJECT_NIL);
  // ml_object_debug_dump(the_nil);

  /* number */
  {
    ml_object *num = ml_object_new_number(0.0);
    ml_test(num->tag == ML_OBJECT_NUMBER);
    ml_test(num->u.num == 0.0);
  }
  {
    ml_object *num = ml_object_new_number(42.0);
    ml_test(num->tag == ML_OBJECT_NUMBER);
    ml_test(num->u.num == 42.0);
  }
  {
    ml_object *num = ml_object_new_number(-42.0);
    ml_test(num->tag == ML_OBJECT_NUMBER);
    ml_test(num->u.num == -42.0);
  }

  /* bool */
  {
    ml_object *num = ml_object_new_bool(0);
    ml_test(num->tag == ML_OBJECT_BOOL);
    ml_test(num->u.boolean == 0);
  }
  {
    ml_object *num = ml_object_new_bool(1);
    ml_test(num->tag == ML_OBJECT_BOOL);
    ml_test(num->u.boolean == 1);
  }

  /* string */
  {
    ml_object *str = ml_object_new_string(ml_string_new_str(""));
    ml_test(str->tag == ML_OBJECT_STRING);
    ml_string expected = ml_string_new_str("");
    ml_test(ml_string_strcmp(&str->u.str, &expected) == 0);
  }
  {
    ml_object *str = ml_object_new_string(ml_string_new_str("fox"));
    ml_test(str->tag == ML_OBJECT_STRING);
    ml_string expected = ml_string_new_str("fox");
    ml_test(ml_string_strcmp(&str->u.str, &expected) == 0);
  }

  /* cons */
  {
    ml_object *car = ml_object_new_number(42.0);
    ml_object *cdr = ml_object_new_bool(0);
    ml_object *cons = ml_object_new_cons(car, cdr);
    uintptr_t ptrs[3] = {(uintptr_t)cons->u.cons.car,
                         (uintptr_t)cons->u.cons.cdr, (uintptr_t)cons};
    ml_test(cons->tag == ML_OBJECT_CONS);
    ml_test(cons->u.cons.car->tag == ML_OBJECT_NUMBER);
    ml_test(cons->u.cons.car->u.num == 42.0);
    ml_test(cons->u.cons.cdr->tag == ML_OBJECT_BOOL);
    ml_test(cons->u.cons.cdr->u.boolean == 0);
    ml_test(ptrarr_in((uintptr_t)car, ptrs, countof(ptrs), 1));
    ml_test(ptrarr_in((uintptr_t)cdr, ptrs, countof(ptrs), 1));
    ml_test(ptrarr_in((uintptr_t)cons, ptrs, countof(ptrs), 1));
    // ml_object_debug_dump(cons);
  }

  /* same reference */
  {
    ml_object *b = ml_object_new_bool(1);
    ml_object *cons = ml_object_new_cons(b, b);
    uintptr_t ptrs[3] = {(uintptr_t)cons->u.cons.car,
                         (uintptr_t)cons->u.cons.cdr, (uintptr_t)cons};
    ml_test(ptrarr_in((uintptr_t)b, ptrs, countof(ptrs), 2));
    ml_test(ptrarr_in((uintptr_t)cons, ptrs, countof(ptrs), 1));
    ml_test(cons->u.cons.car == cons->u.cons.cdr);
    // ml_object_debug_dump(t3_cons);
  }

  /* circular list */
// TODO
// これが重要なのはちゃんとダンプできるのかということなので、このテストからは除いていいかもしれない。
#if 0
  ml_object *t4_list_1 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("foo")), the_nil);
  ml_object *t4_list_2 = ml_object_new_cons(the_nil, t4_list_1);
  ml_object *t4_list_3 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("baz")), t4_list_2);
  ml_object *t4_list_4 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("qux")), t4_list_3);
  ml_object *t4_list_5 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("quux")), t4_list_4);
  t4_list_2->u.cons.car = t4_list_4;
  // ml_object_debug_dump(t4_list_5);
#endif
}

/* machine.c */
void test_machine(void) {
  /* simple add() */
  {
    ml_machine machine = (ml_machine){0};
    ml_object *f = ml_object_new_name(ml_string_new_str("+"));
    ml_object *a = ml_object_new_number(32);
    ml_object *b = ml_object_new_number(-0.5);
    ml_object *list3 = ml_object_new_cons(b, the_nil);
    ml_object *list2 = ml_object_new_cons(a, list3);
    ml_object *list1 = ml_object_new_cons(f, list2);
    ml_object *result = ml_machine_eval(&machine, list1);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->u.num == 31.5);
    // ml_object_debug_dump(result);
  }
}

void test_top(void) {
  {
    ml_machine machine = (ml_machine){0};
    ml_parser parser = (ml_parser){.file = (ml_file){.str = "(+ 3 5)"}};
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->u.num == 8.0);
    // ml_object_debug_dump(root);
    // ml_object_debug_dump(result);
  }

  /* nested */
  {
    ml_machine machine = (ml_machine){0};
    ml_parser parser = (ml_parser){.file = (ml_file){.str = "(+ 1 (+ 2 3))"}};
    ml_object *root = ml_parser_parse(&parser);
    if (root == NULL)
      fatal("parse");
    ml_object *result = ml_machine_eval(&machine, root);
    ml_test(result->tag == ML_OBJECT_NUMBER);
    ml_test(result->u.num == 6.0);
    // ml_object_debug_dump(root);
    // ml_object_debug_dump(result);
  }
}

void test_main(void) {
// TODO これはダンプのテストなのでPythonでって思ったけど、invalid nil
// だと難しいな。どうしようか
#if 0
  ml_object *t6_nil = ml_object_new_nil();
  // ml_object_debug_dump(t6_nil);
#endif

  test_object();
  test_machine();
  test_top();
}
