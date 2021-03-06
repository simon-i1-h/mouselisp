#include <inttypes.h>
#include <stdint.h>

#include "mouselisp.h"

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

ml_object *ml_object_new_number(int num) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_NUMBER, .num = num};
  return ret;
}

ml_object *ml_object_new_name(const char *str) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  ml_string mlstr = ml_string_new_str(str);
  *ret = (ml_object){.tag = ML_OBJECT_NAME, .str = mlstr};
  return ret;
}

ml_object *ml_object_new_builtin_function(ml_builtin *builtin) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  ml_function func = {.tag = ML_FUNCTION_BUILTIN,
                      .builtin = builtin,
                      .closure = NULL /* TODO NULLじゃない？ */};
  *ret = (ml_object){.tag = ML_OBJECT_FUNCTION, .func = func};
  return ret;
}

ml_object *ml_object_new_normal_function(ml_object *closure, ml_object *args,
                                         ml_object *body) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  ml_function func = {.tag = ML_FUNCTION_NORMAL,
                      .normal = {.args = args, .body = body},
                      .closure = closure};
  *ret = (ml_object){.tag = ML_OBJECT_FUNCTION, .func = func};
  return ret;
}

ml_object *ml_object_new_pointer(ml_object *ptr) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  *ret = (ml_object){.tag = ML_OBJECT_POINTER, .ptr = ptr};
  return ret;
}

ml_object *ml_object_new_macro(ml_object *args, ml_object *body) {
  ml_object *ret = xgcmalloc(sizeof(ml_object));
  ml_macro macro = {.args = args, .body = body};
  *ret = (ml_object){.tag = ML_OBJECT_MACRO, .macro = macro};
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
  case ML_OBJECT_BOOL:
    rlogmsg("BOOL: %s", obj->boolean ? "true" : "false");
    break;
  case ML_OBJECT_NUMBER:
    rlogmsg("NUMBER: %d", obj->num);
    break;
  case ML_OBJECT_NAME:
    rlogmsg("NAME: %s", obj->str.str);
    break;
  case ML_OBJECT_CONS:
    rlogmsg("CONS:");
    ml_object_debug_dump_recur(obj->cons.car, known_objs, depth + 1);
    ml_object_debug_dump_recur(obj->cons.cdr, known_objs, depth + 1);
    break;
  case ML_OBJECT_FUNCTION:
    switch (obj->func.tag) {
    case ML_FUNCTION_NORMAL:
      rlogmsg("NORMAL FUNCTION (ARGS, BODY):");
      ml_object_debug_dump_recur(obj->func.normal.args, known_objs,
                                 depth + 1);
      ml_object_debug_dump_recur(obj->func.normal.body, known_objs,
                                 depth + 1);
      break;
    case ML_FUNCTION_BUILTIN:
      rlogmsg("BUILTIN FUNCTION: %" PRIxPTR, (uintptr_t)obj->func.builtin);
      break;
    }
    break;
  case ML_OBJECT_POINTER:
    if (obj == the_nil)
      rlogmsg("NIL");
    else
      rlogmsg("POINTER: %" PRIxPTR, (uintptr_t)obj->ptr);
    break;
  case ML_OBJECT_MACRO:
    rlogmsg("MACRO (ARGS, BODY):");
    ml_object_debug_dump_recur(obj->macro.args, known_objs, depth + 1);
    ml_object_debug_dump_recur(obj->macro.body, known_objs, depth + 1);
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
