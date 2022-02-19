#include <string.h>

#include "mouselisp.h"

ml_object *ml_prelude(void) {
  ml_object *ret = ml_object_new_cons(the_nil, the_nil);

  {
    ml_object *name = ml_object_new_name("+");
    ml_object *value = ml_object_new_builtin_function(ml_add);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("-");
    ml_object *value = ml_object_new_builtin_function(ml_sub);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("*");
    ml_object *value = ml_object_new_builtin_function(ml_mul);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("/");
    ml_object *value = ml_object_new_builtin_function(ml_div);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("car");
    ml_object *value = ml_object_new_builtin_function(ml_car);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("cdr");
    ml_object *value = ml_object_new_builtin_function(ml_cdr);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("cons");
    ml_object *value = ml_object_new_builtin_function(ml_cons_);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }

  return ret;
}

ml_machine ml_machine_new(void) {
  return (ml_machine){.named_objs = ml_prelude()};
}

ml_object *ml_def(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *name = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *value = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    fatal("invalid body");

  if (name->tag != ML_OBJECT_NAME)
    fatal("invalid body");

  ml_object *evaled = ml_machine_eval(m, value);
  ml_object *named = ml_object_new_cons(name, evaled);
  m->named_objs->cons.cdr = ml_object_new_cons(named, m->named_objs->cons.cdr);
  return name;
}

ml_object *ml_if(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *cond = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *then = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *else_ = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    fatal("invalid body");

  ml_object *evaled = ml_machine_eval(m, cond);
  if (evaled->tag != ML_OBJECT_BOOL)
    fatal("invalid body");
  return ml_machine_eval(m, evaled->boolean ? then : else_);
}

ml_object *ml_find_named_object(ml_machine *m, const char *name) {
  for (ml_object *elem = m->named_objs; elem != the_nil;
       elem = elem->cons.cdr) {
    ml_object *pair = elem->cons.car;
    if (pair == the_nil)
      continue;
    if (strcmp(pair->cons.car->str.str, name) == 0)
      return pair->cons.cdr;
  }
  return NULL;
}

ml_object *ml_fn(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *fn_args = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid body");
  ml_object *fn_body = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    fatal("invalid body");

  /* validate arg list */
  for (ml_object *l = fn_args; l != the_nil; l = l->cons.cdr)
    if (l->tag != ML_OBJECT_CONS || l->cons.car->tag != ML_OBJECT_NAME)
      fatal("invalid body");

  return ml_object_new_normal_function(m->named_objs, fn_args, fn_body);
}

ml_object *ml_machine_eval_list(ml_machine *m, ml_object *root) {
  ml_object *car = root->cons.car;
  ml_object *cdr = root->cons.cdr;

  if (car->tag == ML_OBJECT_NAME) {
    ml_string name = car->str;

    /* special forms */
    if (strcmp(name.str, "if") == 0)
      return ml_if(m, cdr);
    else if (strcmp(name.str, "def") == 0)
      return ml_def(m, cdr);
    else if (strcmp(name.str, "fn") == 0)
      return ml_fn(m, cdr);
  }

  ml_object *value = ml_machine_eval(m, car);

  /* apply a function */

  if (value->tag != ML_OBJECT_FUNCTION)
    fatal("invalid form");
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
    return func.builtin(m, args);
  case ML_FUNCTION_NORMAL: {
    /* bind */
    ml_object *new_table = func.closure;
    ml_object *name = func.normal.args;
    ml_object *arg = args;
    for (; name != the_nil && arg != the_nil;
         name = name->cons.cdr, arg = arg->cons.cdr) {
      if (name == the_nil || arg == the_nil)
        fatal("mismatched number of args");
      ml_object *named = ml_object_new_cons(name->cons.car, arg->cons.car);
      new_table = ml_object_new_cons(named, new_table);
    }
    new_table = ml_object_new_cons(the_nil, new_table);

    /* apply */
    ml_object *curr_table = m->named_objs;
    m->named_objs = new_table;
    ml_object *ret = ml_machine_eval(m, func.normal.body);
    m->named_objs = curr_table;
    return ret;
  }
  }
}

ml_object *ml_machine_eval(ml_machine *m, ml_object *root) {
  ml_object *ret;

  switch (root->tag) {
  case ML_OBJECT_NIL:
    if (root != the_nil)
      fatal("invalid nil");
    ATTR_FALLTHROUGH;
  case ML_OBJECT_NUMBER:
  case ML_OBJECT_BOOL:
  case ML_OBJECT_STRING:
  case ML_OBJECT_FUNCTION:
    return root;
  case ML_OBJECT_NAME:
    ret = ml_find_named_object(m, root->str.str);
    if (ret == NULL)
      fatal("%s is not defined.", root->str.str);
    return ret;
  case ML_OBJECT_CONS:
    return ml_machine_eval_list(m, root);
  }
}
