#include <string.h>

#include "mouselisp.h"

ml_object *ml_prelude(void) {
  ml_object *ret = the_nil;

  {
    ml_object *name = ml_object_new_name("+");
    ml_object *value = ml_object_new_builtin_function(ml_add);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }
  {
    ml_object *name = ml_object_new_name("-");
    ml_object *value = ml_object_new_builtin_function(ml_sub);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }
  {
    ml_object *name = ml_object_new_name("*");
    ml_object *value = ml_object_new_builtin_function(ml_mul);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }
  {
    ml_object *name = ml_object_new_name("/");
    ml_object *value = ml_object_new_builtin_function(ml_div);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }
  {
    ml_object *name = ml_object_new_name("car");
    ml_object *value = ml_object_new_builtin_function(ml_car);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }
  {
    ml_object *name = ml_object_new_name("cdr");
    ml_object *value = ml_object_new_builtin_function(ml_cdr);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }
  {
    ml_object *name = ml_object_new_name("cons");
    ml_object *value = ml_object_new_builtin_function(ml_cons_);
    ml_object *named = ml_object_new_cons(name, value);
    ret = ml_object_new_cons(named, ret);
  }

  return ret;
}

ml_machine ml_machine_new(void) {
  return (ml_machine){.named_objs = ml_prelude()};
}

ml_object *ml_find_named_object(ml_machine *m, const char *name) {
  for (ml_object *elem = m->named_objs; elem != the_nil;
       elem = elem->cons.cdr) {
    ml_object *pair = elem->cons.car;
    if (strcmp(pair->cons.car->str.str, name) == 0)
      return pair->cons.cdr;
  }
  return NULL;
}

ml_object *ml_machine_eval_list(ml_machine *m, ml_object *root) {
  ml_object *car = root->cons.car;
  ml_object *cdr = root->cons.cdr;

  if (car->tag != ML_OBJECT_NAME)
    fatal("invalid args");

  ml_string name = car->str;
  ml_object *value = ml_find_named_object(m, name.str);

  if (value->tag != ML_OBJECT_FUNCTION)
    fatal("invalid args");
  ml_function func = value->func;

  ml_object *args = the_nil;
  ml_object *tail = args;
  for (ml_object *elem = cdr; elem != the_nil; elem = elem->cons.cdr) {
    ml_object *evaled = ml_machine_eval(m, elem->cons.car);
    ml_object *arg = ml_object_new_cons(evaled, the_nil);
    if (tail == the_nil)
      args = tail = arg;
    else
      tail = tail->cons.cdr = arg;
  }

  switch (func.tag) {
  case ML_FUNCTION_BUILTIN:
    return func.builtin(args);
  case ML_FUNCTION_NORMAL:
    /* TODO */
    fatal("unimplemented");
  }
}

ml_object *ml_machine_eval(ml_machine *m, ml_object *root) {
  if (root->tag == ML_OBJECT_NUMBER)
    return root;

  if (root->tag == ML_OBJECT_CONS)
    return ml_machine_eval_list(m, root);

  ml_object_debug_dump(root);
  fatal("evaluate failed."); /* TODO */
}
