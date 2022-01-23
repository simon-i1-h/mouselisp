#include "mouselisp.h"

ml_object *the_nil;

void
mouselisp_init(void)
{
  GC_INIT();
  the_nil = ml_object_new_nil();
}
