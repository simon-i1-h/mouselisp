#include <setjmp.h>

#include "mouselisp.h"

ATTR_NORETURN
void ml_throw2(const char *filename, const char *line, ml_machine *m,
               ml_object *err) {
  if (exc_log)
    logmsg("%s: %s: %s", filename, line, err->cons.cdr->cons.car->str.str);
  m->exc = err;
  longjmp(m->last_exc_handler, 1);
}
