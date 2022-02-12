#include <inttypes.h>

#include "mouselisp.h"

ml_object *ml_object_new_nil(void) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NIL};
  return ret;
}

ml_object *ml_object_new_cons(ml_object *car, ml_object *cdr) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  ml_cons cons = {.car = car, .cdr = cdr};
  *ret = (ml_object){.tag = ML_OBJECT_CONS, .cons = cons};
  return ret;
}

ml_object *ml_object_new_bool(int boolean) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_BOOL, .boolean = boolean};
  return ret;
}

ml_object *ml_object_new_number(double num) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NUMBER, .num = num};
  return ret;
}

ml_object *ml_object_new_string(ml_string str) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_STRING, .str = str};
  return ret;
}

ml_object *ml_object_new_name(ml_string str) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NAME, .str = str};
  return ret;
}

int ml_list_exists(ml_object *list, ml_object *ptr) {
  ml_object *o = list;
  while (o->tag == ML_OBJECT_CONS) {
    if (o->cons.car == ptr)
      return 1;
    else
      o = o->cons.cdr;
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
    rlogmsg("BOOL: %s", obj->boolean ? "true" : "false");
    break;
  case ML_OBJECT_NUMBER:
    rlogmsg("NUMBER: %f", obj->num);
    break;
  case ML_OBJECT_STRING:
    rlogmsg("STRING: %s", obj->str.str);
    break;
  case ML_OBJECT_NAME:
    rlogmsg("NAME: %s", obj->str.str);
    break;
  case ML_OBJECT_CONS:
    rlogmsg("CONS:");
    ml_object_debug_dump_recur(obj->cons.car, known_objs, depth + 1);
    ml_object_debug_dump_recur(obj->cons.cdr, known_objs, depth + 1);
    break;
  }
}

void ml_object_debug_dump2(const char *filename, const char *line,
                           ml_object *root) {
  ml_object *known_objs = the_nil;

  logmsg2(filename, line, "dump object:");
  rlogmsg("--------------------");
  ml_object_debug_dump_recur(root, &known_objs, 0);
  rlogmsg("--------------------");
}
