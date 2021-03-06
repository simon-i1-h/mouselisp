#include <limits.h>
#include <setjmp.h>
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
    ml_object *name = ml_object_new_name("mod");
    ml_object *value = ml_object_new_builtin_function(ml_mod);
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
  {
    ml_object *name = ml_object_new_name("&");
    ml_object *value = ml_object_new_builtin_function(ml_ref);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("->");
    ml_object *value = ml_object_new_builtin_function(ml_deref);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("=");
    ml_object *value = ml_object_new_builtin_function(ml_eq);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name(">");
    ml_object *value = ml_object_new_builtin_function(ml_gt);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("<");
    ml_object *value = ml_object_new_builtin_function(ml_lt);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name(">=");
    ml_object *value = ml_object_new_builtin_function(ml_ge);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("<=");
    ml_object *value = ml_object_new_builtin_function(ml_le);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("and");
    ml_object *value = ml_object_new_builtin_function(ml_and);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("or");
    ml_object *value = ml_object_new_builtin_function(ml_or);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("not");
    ml_object *value = ml_object_new_builtin_function(ml_not);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("cons?");
    ml_object *value = ml_object_new_builtin_function(ml_is_cons);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("bool?");
    ml_object *value = ml_object_new_builtin_function(ml_is_bool);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("num?");
    ml_object *value = ml_object_new_builtin_function(ml_is_num);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("name?");
    ml_object *value = ml_object_new_builtin_function(ml_is_name);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("fn?");
    ml_object *value = ml_object_new_builtin_function(ml_is_func);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("ptr?");
    ml_object *value = ml_object_new_builtin_function(ml_is_ptr);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("list");
    ml_object *value = ml_object_new_builtin_function(ml_list);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("unique");
    ml_object *value = ml_object_new_builtin_function(ml_unique);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("eval-error");
    ml_object *value = ml_object_new_builtin_function(ml_eval_error);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("nil-error");
    ml_object *value = ml_object_new_builtin_function(ml_nil_error);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("noname-error");
    ml_object *value = ml_object_new_builtin_function(ml_noname_error);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("syntax-error");
    ml_object *value = ml_object_new_builtin_function(ml_syntax_error);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("arith-error");
    ml_object *value = ml_object_new_builtin_function(ml_arith_error);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("throw");
    ml_object *value = ml_object_new_builtin_function(ml_throw_);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("num-min");
    ml_object *value = ml_object_new_number(INT_MIN);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("num-max");
    ml_object *value = ml_object_new_number(INT_MAX);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }
  {
    ml_object *name = ml_object_new_name("debug-dump");
    ml_object *value = ml_object_new_builtin_function(ml_debug_dump);
    ml_object *named = ml_object_new_cons(name, value);
    ret->cons.cdr = ml_object_new_cons(named, ret->cons.cdr);
  }

  return ret;
}

ml_machine ml_machine_new(void) {
  return (ml_machine){.named_objs = ml_prelude()};
}

ml_object *ml_machine_eval(ml_machine *m, ml_object *root);

ml_object *ml_def(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *name = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *value = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (name->tag != ML_OBJECT_NAME)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *evaled = ml_machine_eval(m, value);
  ml_object *named = ml_object_new_cons(name, evaled);
  m->named_objs->cons.cdr =
      ml_object_new_cons(named, m->named_objs->cons.cdr);
  return name;
}

ml_object *ml_if(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *cond = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *then = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *else_ = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *evaled = ml_machine_eval(m, cond);
  if (evaled->tag != ML_OBJECT_BOOL)
    ml_throw(m, ml_eval_error(m, the_nil));
  return ml_machine_eval(m, evaled->boolean ? then : else_);
}

ml_object *ml_find_named(ml_machine *m, const char *name) {
  for (ml_object *elem = m->named_objs; elem != the_nil;
       elem = elem->cons.cdr) {
    ml_object *pair = elem->cons.car;
    if (pair == the_nil)
      continue;
    if (strcmp(pair->cons.car->str.str, name) == 0)
      return pair;
  }
  ml_throw(m, ml_noname_error(m, the_nil));
}

ml_object *ml_find_named_object(ml_machine *m, const char *name) {
  ml_object *named = ml_find_named(m, name);
  return named->cons.cdr;
}

ml_object *ml_fn(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *fn_args = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *fn_body = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  /* validate arg list */
  for (ml_object *l = fn_args; l != the_nil; l = l->cons.cdr)
    if (l->tag != ML_OBJECT_CONS || l->cons.car->tag != ML_OBJECT_NAME)
      ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_normal_function(m->named_objs, fn_args, fn_body);
}

ml_object *ml_do(ml_machine *m, ml_object *body) {
  ml_object *ret = the_nil;

  for (ml_object *list = body; list != the_nil; list = list->cons.cdr) {
    if (list->tag != ML_OBJECT_CONS)
      ml_throw(m, ml_eval_error(m, the_nil));
    ret = ml_machine_eval(m, list->cons.car);
  }

  return ret;
}

ml_object *ml_quote(ml_machine *m, ml_object *body) {
  (void)m;
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *list = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return list;
}

ml_object *ml_set(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *name = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *value = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (name->tag != ML_OBJECT_NAME)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *named = ml_find_named(m, name->str.str);
  ml_object *evaled = ml_machine_eval(m, value);
  named->cons.cdr = evaled;

  return evaled;
}

ml_object *ml_setcar(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *name = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *value = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (name->tag != ML_OBJECT_NAME)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *old_value = ml_find_named_object(m, name->str.str);
  if (old_value->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *evaled = ml_machine_eval(m, value);
  old_value->cons.car = evaled;

  return evaled;
}

ml_object *ml_setcdr(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *name = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *value = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (name->tag != ML_OBJECT_NAME)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *old_value = ml_find_named_object(m, name->str.str);
  if (old_value->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));

  ml_object *evaled = ml_machine_eval(m, value);
  old_value->cons.cdr = evaled;

  return evaled;
}

ml_object *ml_macro_(ml_machine *m, ml_object *body) {
  (void)m;
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *macro_args = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *macro_body = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  /* validate arg list */
  for (ml_object *l = macro_args; l != the_nil; l = l->cons.cdr)
    if (l->tag != ML_OBJECT_CONS || l->cons.car->tag != ML_OBJECT_NAME)
      ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_macro(macro_args, macro_body);
}

ml_object *ml_machine_eval_function(ml_machine *m, ml_object *body) {
  ml_function func = body->cons.car->func;
  ml_object *cdr = body->cons.cdr;

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
        ml_throw(m, ml_eval_error(m, the_nil));
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

ml_object *ml_machine_eval_macro(ml_machine *m, ml_object *body) {
  ml_macro macro = body->cons.car->macro;

  /* bind */
  ml_object *new_table = m->named_objs;
  ml_object *name = macro.args;
  ml_object *arg = body->cons.cdr;
  for (; name != the_nil && arg != the_nil;
       name = name->cons.cdr, arg = arg->cons.cdr) {
    if (name == the_nil || arg == the_nil)
      ml_throw(m, ml_eval_error(m, the_nil));
    ml_object *named = ml_object_new_cons(name->cons.car, arg->cons.car);
    new_table = ml_object_new_cons(named, new_table);
  }
  new_table = ml_object_new_cons(the_nil, new_table);

  /* expand */
  ml_object *curr_table = m->named_objs;
  m->named_objs = new_table;
  ml_object *expanded = ml_machine_eval(m, macro.body);
  m->named_objs = curr_table;

  return ml_machine_eval(m, expanded);
}

ml_object *ml_try(ml_machine *m, ml_object *body) {
  ml_object *curr = body;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *try = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *kw_catch = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *error = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *catch = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (kw_catch->tag != ML_OBJECT_NAME ||
      strcmp(kw_catch->str.str, "catch") != 0)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (error->tag != ML_OBJECT_NAME)
    ml_throw(m, ml_eval_error(m, the_nil));

  /* error handling */
  ml_object *ret;
  ml_object *prev_named_objs = m->named_objs;
  jmp_buf *prev_jmpbuf = m->last_exc_handler;
  jmp_buf curr_jmpbuf;
  m->last_exc_handler = &curr_jmpbuf;
  m->exc = the_nil;
  if (setjmp(curr_jmpbuf) == 0) {
    ret = ml_machine_eval(m, try);
    m->last_exc_handler = prev_jmpbuf;
  } else {
    m->last_exc_handler = prev_jmpbuf;

    ml_object *named = ml_object_new_cons(error, m->exc);
    ml_object *new_table = ml_object_new_cons(
        the_nil, ml_object_new_cons(named, prev_named_objs));
    m->named_objs = new_table;
    ret = ml_machine_eval(m, catch);
    m->named_objs = prev_named_objs;
  }
  return ret;
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
    else if (strcmp(name.str, "do") == 0)
      return ml_do(m, cdr);
    else if (strcmp(name.str, "quote") == 0)
      return ml_quote(m, cdr);
    else if (strcmp(name.str, "set") == 0)
      return ml_set(m, cdr);
    else if (strcmp(name.str, "setcar") == 0)
      return ml_setcar(m, cdr);
    else if (strcmp(name.str, "setcdr") == 0)
      return ml_setcdr(m, cdr);
    else if (strcmp(name.str, "macro") == 0)
      return ml_macro_(m, cdr);
    else if (strcmp(name.str, "try") == 0)
      return ml_try(m, cdr);
  }

  ml_object *evaled = ml_machine_eval(m, car);
  ml_object *body = ml_object_new_cons(evaled, cdr);

  if (evaled->tag == ML_OBJECT_FUNCTION)
    return ml_machine_eval_function(m, body);
  else if (evaled->tag == ML_OBJECT_MACRO)
    return ml_machine_eval_macro(m, body);

  ml_throw(m, ml_eval_error(m, the_nil));
}

ml_object *ml_machine_eval(ml_machine *m, ml_object *root) {
  switch (root->tag) {
  case ML_OBJECT_NUMBER:
  case ML_OBJECT_BOOL:
  case ML_OBJECT_FUNCTION:
  case ML_OBJECT_POINTER:
  case ML_OBJECT_MACRO:
    return root;
  case ML_OBJECT_NAME:
    return ml_find_named_object(m, root->str.str);
  case ML_OBJECT_CONS:
    return ml_machine_eval_list(m, root);
  }
}

ml_object *ml_machine_eval_top(ml_machine *m, ml_object *root) {
  ml_object *ret = the_nil;
  for (ml_object *list = root; list != the_nil; list = list->cons.cdr)
    ret = ml_machine_eval(m, list->cons.car);
  return ret;
}

ml_object *ml_machine_xeval2(const char *filename, const char *line,
                             ml_machine *m, ml_object *root) {
  /* top level error handling */
  jmp_buf curr_jmpbuf;
  m->last_exc_handler = &curr_jmpbuf;
  m->exc = the_nil;
  if (setjmp(curr_jmpbuf) == 0)
    return ml_machine_eval(m, root);
  else
    fatal("%s: %s: error: %s", filename, line,
          m->exc->cons.cdr->cons.car->str.str);
}

ml_object *ml_machine_xeval_top2(const char *filename, const char *line,
                                 ml_machine *m, ml_object *root) {
  /* top level error handling */
  jmp_buf curr_jmpbuf;
  m->last_exc_handler = &curr_jmpbuf;
  m->exc = the_nil;
  if (setjmp(curr_jmpbuf) == 0)
    return ml_machine_eval_top(m, root);
  else
    fatal("%s: %s: error: %s", filename, line,
          m->exc->cons.cdr->cons.car->str.str);
}
