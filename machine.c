#include <string.h>

#include "mouselisp.h"

ml_object *ml_machine_eval(ml_machine *m, ml_object *root) {
  if (root->tag == ML_OBJECT_NUMBER)
    return root;

  if (root->tag == ML_OBJECT_CONS &&
      root->cons.cdr->tag == ML_OBJECT_CONS &&
      root->cons.cdr->cons.cdr->tag == ML_OBJECT_CONS) {
    ml_object *f = root->cons.car;
    ml_object *a = ml_machine_eval(m, root->cons.cdr->cons.car);
    ml_object *b =
        ml_machine_eval(m, root->cons.cdr->cons.cdr->cons.car);
    if (f->tag == ML_OBJECT_NAME) {
      /* apply builtin functions */
      if (strcmp(f->str.str, "+") == 0 && a->tag == ML_OBJECT_NUMBER &&
          b->tag == ML_OBJECT_NUMBER)
        return ml_object_new_number(a->num + b->num);
    }
  }

  ml_object_debug_dump(root);
  fatal("evaluate failed."); /* TODO */
}
