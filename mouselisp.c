#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include "mouselisp.h"

/* object */

typedef struct ml_object ml_object;

typedef struct ml_cons {
  ml_object *car;
  ml_object *cdr;
} ml_cons;

/*
 * TODO: byte (array)
 */
typedef enum ml_object_tag {
  ML_OBJECT_NIL,
  ML_OBJECT_CONS,
  ML_OBJECT_BOOL,
  ML_OBJECT_NUMBER,
  ML_OBJECT_STRING,
  ML_OBJECT_NAME
} ml_object_tag;

typedef struct ml_object {
  ml_object_tag tag;
  union {
    ml_cons cons;
    int boolean;
    double num;
    ml_string str; /* string or name */
  } u;
} ml_object;

/* singleton */
ml_object *the_nil;

ml_object *ml_object_new_nil(void) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NIL};
  return ret;
}

ml_object *ml_object_new_cons(ml_object *car, ml_object *cdr) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  ml_cons cons = {.car = car, .cdr = cdr};
  *ret = (ml_object){.tag = ML_OBJECT_CONS, .u.cons = cons};
  return ret;
}

ml_object *ml_object_new_bool(int boolean) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_BOOL, .u.boolean = boolean};
  return ret;
}

ml_object *ml_object_new_number(double num) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NUMBER, .u.num = num};
  return ret;
}

ml_object *ml_object_new_string(ml_string str) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_STRING, .u.str = str};
  return ret;
}

ml_object *ml_object_new_name(ml_string str) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NAME, .u.str = str};
  return ret;
}

int ml_list_exists(ml_object *list, ml_object *ptr) {
  ml_object *o = list;
  while (o->tag == ML_OBJECT_CONS) {
    if (o->u.cons.car == ptr)
      return 1;
    else
      o = o->u.cons.cdr;
  }
  return 0;
}

void ml_object_debug_dump_recur(ml_object *obj, ml_object **known_objs,
                                int depth) {
  for (int i = 0; i < depth; i++)
    rlogmsgf("  ");

  rlogmsgf("%" PRIxPTR " ", (uintptr_t)obj);
  if (ml_list_exists(*known_objs, obj)) {
    rlogmsg("(dup)");
    return;
  }

  *known_objs = ml_object_new_cons(obj, *known_objs);

  switch (obj->tag) {
  case ML_OBJECT_NIL:
    if (obj != the_nil)
      rlogmsgf("INVALID ");
    rlogmsg("NIL");
    break;
  case ML_OBJECT_BOOL:
    rlogmsg("BOOL: %s", obj->u.boolean ? "true" : "false");
    break;
  case ML_OBJECT_NUMBER:
    rlogmsg("NUMBER: %f", obj->u.num);
    break;
  case ML_OBJECT_STRING:
    rlogmsg("STRING: %s", obj->u.str.str);
    break;
  case ML_OBJECT_NAME:
    rlogmsg("NAME: %s", obj->u.str.str);
    break;
  case ML_OBJECT_CONS:
    rlogmsg("CONS:");
    ml_object_debug_dump_recur(obj->u.cons.car, known_objs, depth + 1);
    ml_object_debug_dump_recur(obj->u.cons.cdr, known_objs, depth + 1);
    break;
  }
}

#define ml_object_debug_dump(obj)                                            \
  ml_object_debug_dump2(__FILE__, stringify(__LINE__), obj)

void ml_object_debug_dump2(const char *filename, const char *line,
                           ml_object *root) {
  ml_object *known_objs = the_nil;

  logmsg2(filename, line, "dump object:");
  rlogmsg("--------------------");
  ml_object_debug_dump_recur(root, &known_objs, 0);
  rlogmsg("--------------------");
}

/* machine */

typedef struct ml_machine {
  int fake;
} ml_machine;

ml_object *ml_machine_eval(ml_machine *machine, ml_object *root) {
  if (root->tag == ML_OBJECT_NUMBER)
    return root;

  if (root->tag == ML_OBJECT_CONS &&
      root->u.cons.cdr->tag == ML_OBJECT_CONS &&
      root->u.cons.cdr->u.cons.cdr->tag == ML_OBJECT_CONS) {
    ml_object *f = root->u.cons.car;
    ml_object *a = ml_machine_eval(machine, root->u.cons.cdr->u.cons.car);
    ml_object *b =
        ml_machine_eval(machine, root->u.cons.cdr->u.cons.cdr->u.cons.car);
    if (f->tag == ML_OBJECT_NAME) {
      /* apply builtin functions */
      if (strcmp(f->u.str.str, "+") == 0 && a->tag == ML_OBJECT_NUMBER &&
          b->tag == ML_OBJECT_NUMBER)
        return ml_object_new_number(a->u.num + b->u.num);
    }
  }

  ml_object_debug_dump(root);
  fatal("evaluate failed.");
}

/* main */

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

  return 0;
}
