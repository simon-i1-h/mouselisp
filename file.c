#include "mouselisp.h"

ml_read_char ml_file_read(ml_file *f) {
  char c = f->str[f->pos];

  if (f->buf_pos > 0) {
    bug(f->buf_pos > countof(f->buf));
    c = f->buf[--f->buf_pos];
  }

  ml_read_char ret = {.c = c, .eof = 0, .pos = f->pos};

  if (c == '\0')
    ret.eof = 1;
  else
    f->pos++;

  return ret;
}

void ml_file_unread(ml_file *f, char c) {
  bug(f->buf_pos >= countof(f->buf));
  bug(f->pos == 0);
  f->buf[f->buf_pos++] = c;
  f->pos--;
}
