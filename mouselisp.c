#include "mouselisp.h"

int main(void) {
  GC_INIT();
  the_nil = ml_object_new_nil();

  ml_object_debug_dump(the_nil);

  ml_object *t2_car = ml_object_new_number(42.0);
  ml_object *t2_cdr = ml_object_new_bool(0);
  ml_object *t2_cons = ml_object_new_cons(t2_car, t2_cdr);
  ml_object_debug_dump(t2_cons);

  ml_object *t3_str = ml_object_new_string(ml_string_new_str("fox"));
  ml_object *t3_cons = ml_object_new_cons(t3_str, t3_str);
  ml_object_debug_dump(t3_cons);

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
  ml_object_debug_dump(t4_list_5);

  ml_machine t5_machine = (ml_machine){0};
  ml_object *t5_f = ml_object_new_name(ml_string_new_str("+"));
  ml_object *t5_a = ml_object_new_number(32);
  ml_object *t5_b = ml_object_new_number(-0.5);
  ml_object *t5_list_1 = ml_object_new_cons(t5_b, the_nil);
  ml_object *t5_list_2 = ml_object_new_cons(t5_a, t5_list_1);
  ml_object *t5_list_3 = ml_object_new_cons(t5_f, t5_list_2);
  ml_object_debug_dump(ml_machine_eval(&t5_machine, t5_list_3));

  ml_object *t6_nil = ml_object_new_nil();
  ml_object_debug_dump(t6_nil);

  ml_machine t7_machine = (ml_machine){0};
  ml_object *t7_f = ml_object_new_name(ml_string_new_str("+"));
  ml_object *t7_a = ml_object_new_number(25.5);
  ml_object *t7_b = t5_list_3;
  ml_object *t7_list_1 = ml_object_new_cons(t7_b, the_nil);
  ml_object *t7_list_2 = ml_object_new_cons(t7_a, t7_list_1);
  ml_object *t7_list_3 = ml_object_new_cons(t7_f, t7_list_2);
  ml_object_debug_dump(ml_machine_eval(&t7_machine, t7_list_3));

  ml_machine t8_machine = (ml_machine){0};
  ml_parser t8_parser = (ml_parser){.file = (ml_file){.str = "(+ 3 5)"}};
  ml_object *t8_root = ml_parser_parse(&t8_parser);
  if (t8_root == NULL)
    fatal("t8");
  ml_object_debug_dump(t8_root);
  ml_object_debug_dump(ml_machine_eval(&t8_machine, t8_root));

  ml_machine t9_machine = (ml_machine){0};
  ml_parser t9_parser = (ml_parser){.file = (ml_file){.str = "(+ 1 (+ 2 3))"}};
  ml_object *t9_root = ml_parser_parse(&t9_parser);
  if (t9_root == NULL)
    fatal("t9");
  ml_object_debug_dump(t9_root);
  ml_object_debug_dump(ml_machine_eval(&t9_machine, t9_root));

  return 0;
}
