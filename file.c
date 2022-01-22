#include "mouselisp.h"

ml_read_char ml_file_read(ml_file *f) {
  ml_read_char ret = {.c = f->str[f->pos], .eof = 0, .pos = f->pos};

  if (f->str[f->pos] == '\0')
    ret.eof = 1;
  else
    f->pos++;

  return ret;
}
