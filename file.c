#include <stdio.h>

#include "mouselisp.h"

ml_file ml_file_new_str(const char *str) {
  return (ml_file){
      .tag = ML_FILE_STRING, .str = str, .pos = 0, .buf = {0}, .buf_pos = 0};
}

ml_file ml_file_new_file(FILE *file) {
  return (ml_file){
      .tag = ML_FILE_FILE, .file = file, .pos = 0, .buf = {0}, .buf_pos = 0};
}

static inline char readc(FILE *file) {
  int c = fgetc(file);
  return ferror(file) || feof(file) ? '\0' : c;
}

ml_read_char ml_file_read(ml_file *f) {
  char c;

  if (f->buf_pos > 0) {
    bug(f->buf_pos > countof(f->buf));
    c = f->buf[--f->buf_pos];
  } else {
    switch (f->tag) {
    case ML_FILE_STRING:
      c = f->str[f->pos];
      break;
    case ML_FILE_FILE:
      c = readc(f->file);
      break;
    }
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
