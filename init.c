#include "mouselisp.h"

ml_object *the_nil;

/* TODO プラットフォームに応じたスレッドローカルストレージを使う必要がある */
int unique_tid;
int unique_seq;

int exc_log = 1;

void mouselisp_init(void) {
  /* TODO GC効いてるのかよく分からん */
  GC_INIT();
  the_nil = ml_object_new_pointer(NULL);
}
