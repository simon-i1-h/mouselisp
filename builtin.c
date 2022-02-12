#include "mouselisp.h"

ml_object *ml_add(ml_object *args) {
  ml_object *curr = args;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid args");
  ml_object *a = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr->tag != ML_OBJECT_CONS)
    fatal("invalid args");
  ml_object *b = curr->cons.car;
  curr = curr->cons.cdr;

  if (curr != the_nil)
    fatal("invalid args");

  if (a->tag != ML_OBJECT_NUMBER)
    fatal("invalid args");

  if (b->tag != ML_OBJECT_NUMBER)
    fatal("invalid args");

  return ml_object_new_number(a->num + b->num);
}
