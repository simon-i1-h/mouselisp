#include "mouselisp.h"

ml_object *ml_add(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_number(chk_addi(m, a->num, b->num));
}

ml_object *ml_sub(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_number(chk_subi(m, a->num, b->num));
}

ml_object *ml_mul(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_number(chk_muli(m, a->num, b->num));
}

ml_object *ml_div(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_number(chk_divi(m, a->num, b->num));
}

ml_object *ml_mod(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_number(chk_modi(m, a->num, b->num));
}

ml_object *ml_car(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *list = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (list->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));

  return list->cons.car;
}

ml_object *ml_cdr(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *list = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (list->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));

  return list->cons.cdr;
}

ml_object *ml_cons_(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *car = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *cdr = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_cons(car, cdr);
}

ml_object *ml_ref(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *x = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_pointer(x);
}

ml_object *ml_deref(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *x = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (x->tag != ML_OBJECT_POINTER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (x == the_nil)
    ml_throw(m, ml_nil_error(m, the_nil));

  return x->ptr;
}

ml_object *ml_eq(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag == ML_OBJECT_NUMBER && b->tag == ML_OBJECT_NUMBER)
    return ml_object_new_bool(a->num == b->num);
  else if (a->tag == ML_OBJECT_POINTER && b->tag == ML_OBJECT_POINTER)
    return ml_object_new_bool(a->ptr == b->ptr);
  ml_throw(m, ml_eval_error(m, the_nil));
}

ml_object *ml_gt(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->num > b->num);
}

ml_object *ml_lt(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->num < b->num);
}

ml_object *ml_ge(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->num >= b->num);
}

ml_object *ml_le(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_NUMBER)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->num <= b->num);
}

ml_object *ml_and(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_BOOL)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_BOOL)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->boolean && b->boolean);
}

ml_object *ml_or(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_BOOL)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (b->tag != ML_OBJECT_BOOL)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->boolean || b->boolean);
}

ml_object *ml_not(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  if (a->tag != ML_OBJECT_BOOL)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(!a->boolean);
}

ml_object *ml_is_cons(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->tag == ML_OBJECT_CONS);
}

ml_object *ml_is_bool(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->tag == ML_OBJECT_BOOL);
}

ml_object *ml_is_num(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->tag == ML_OBJECT_NUMBER);
}

ml_object *ml_is_name(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->tag == ML_OBJECT_NAME);
}

ml_object *ml_is_func(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->tag == ML_OBJECT_FUNCTION);
}

ml_object *ml_is_ptr(ml_machine *m, ml_object *args) {
  (void)m;
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    ml_throw(m, ml_eval_error(m, the_nil));
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    ml_throw(m, ml_eval_error(m, the_nil));

  return ml_object_new_bool(a->tag == ML_OBJECT_POINTER);
}

ml_object *ml_list(ml_machine *m, ml_object *args) {
  (void)m;
  return args;
}

ml_object *ml_unique(ml_machine *m, ml_object *args) {
  (void)m;
  (void)args;
  char *str = xgcsprintf("$%d.%d", unique_tid, unique_seq);
  unique_seq++;
  return ml_object_new_name(str);
}

ml_object *ml_eval_error(ml_machine *m, ml_object *args) {
  (void)args;
  const char *name_str = "eval-error";
  ml_object *errfunc = ml_find_named_object(m, name_str);
  ml_object *ptr = ml_object_new_pointer(errfunc);
  ml_object *name = ml_object_new_name(name_str);
  return ml_object_new_cons(ptr, ml_object_new_cons(name, the_nil));
}

ml_object *ml_nil_error(ml_machine *m, ml_object *args) {
  (void)args;
  const char *name_str = "nil-error";
  ml_object *errfunc = ml_find_named_object(m, name_str);
  ml_object *ptr = ml_object_new_pointer(errfunc);
  ml_object *name = ml_object_new_name(name_str);
  return ml_object_new_cons(ptr, ml_object_new_cons(name, the_nil));
}

ml_object *ml_noname_error(ml_machine *m, ml_object *args) {
  (void)args;
  const char *name_str = "noname-error";
  ml_object *errfunc = ml_find_named_object(m, name_str);
  ml_object *ptr = ml_object_new_pointer(errfunc);
  ml_object *name = ml_object_new_name(name_str);
  return ml_object_new_cons(ptr, ml_object_new_cons(name, the_nil));
}

ml_object *ml_syntax_error(ml_machine *m, ml_object *args) {
  (void)args;
  const char *name_str = "syntax-error";
  ml_object *errfunc = ml_find_named_object(m, name_str);
  ml_object *ptr = ml_object_new_pointer(errfunc);
  ml_object *name = ml_object_new_name(name_str);
  return ml_object_new_cons(ptr, ml_object_new_cons(name, the_nil));
}

ml_object *ml_arith_error(ml_machine *m, ml_object *args) {
  (void)args;
  const char *name_str = "arith-error";
  ml_object *errfunc = ml_find_named_object(m, name_str);
  ml_object *ptr = ml_object_new_pointer(errfunc);
  ml_object *name = ml_object_new_name(name_str);
  return ml_object_new_cons(ptr, ml_object_new_cons(name, the_nil));
}
