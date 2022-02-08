#include "mouselisp.h"

/* TODO: レキサーとパーサーを分離。いやしないかも。 */

inline static int is_whitespace(unsigned char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline static int is_end_value(unsigned char c) {
  return is_whitespace(c) || c == '(' || c == ')';
}

/* TODO: 小数も扱えるようにしたい。 */
ml_object *ml_parser_parse_number(ml_parser *p) {
  enum { STATE_START, STATE_ZERO, STATE_INTERGER } st = STATE_START;
  ml_read_char c;
  int sign = 1;
  int ret = 0;
  int digit, shift;

  for (;;) {
    c = ml_file_read(&p->file);

    switch (st) {
    case STATE_START:
      if (c.c == '0') {
        st = STATE_ZERO;
        break;
      }
      if (c.c >= '1' && c.c <= '9') {
        ret += c.c - '0';
        st = STATE_INTERGER;
        break;
      }
      if (c.c == '-') {
        sign = -1;
        st = STATE_INTERGER;
        break;
      }
      goto invalid;
    case STATE_ZERO:
      if (c.eof)
        goto valid;
      if (is_end_value(c.c)) {
        ml_file_unread(&p->file, c.c);
        goto valid;
      }
      goto invalid;
    case STATE_INTERGER:
      if (c.eof)
        goto valid;
      if (is_end_value(c.c)) {
        ml_file_unread(&p->file, c.c);
        goto valid;
      }
      if (c.c >= '0' && c.c <= '9') {
        digit = sign * (c.c - '0');
        if (chk_muli(&shift, 10, ret) != 0)
          goto invalid;
        if (chk_addi(&ret, shift, digit) != 0)
          goto invalid;
        break;
      }
      goto invalid;
    }
  }

invalid:
  return NULL;
valid:
  return ml_object_new_number(ret);
}

ml_object *ml_parser_parse_expr(ml_parser *p);

ml_object *ml_parser_parse_list(ml_parser *p) {
  enum { STATE_START, STATE_LIST } st = STATE_START;
  ml_read_char c;
  ml_object *o;
  ml_object *root = the_nil;
  ml_object *tail = root;

  for (;;) {
    c = ml_file_read(&p->file);
    if (c.eof)
      return NULL;
    if (is_whitespace(c.c))
      continue;

    switch (st) {
    case STATE_START:
      if (c.c != '(')
        return NULL;
      st = STATE_LIST;
      break;
    case STATE_LIST:
      if (c.c == ')')
        return root == the_nil ? NULL : root;

      ml_file_unread(&p->file, c.c);
      o = ml_parser_parse_expr(p);
      if (o == NULL)
        return NULL;
      if (root == the_nil) {
        root = tail = ml_object_new_cons(o, the_nil);
      } else {
        tail->u.cons.cdr = ml_object_new_cons(o, the_nil);
        tail = tail->u.cons.cdr;
      }
      break;
    }
  }
}

/* TODO: NULLを使わず例外を使うかも */
ml_object *ml_parser_parse_expr(ml_parser *p) {

  enum { STATE_START, STATE_LITERAL, STATE_NAME } st = STATE_START;

  ml_read_char c;
  char first;
  ml_string strbuf = (ml_string){0};

  for (;;) {
    c = ml_file_read(&p->file);

    switch (st) {
    case STATE_START:
      if (c.eof)
        return NULL;
      if (is_whitespace(c.c))
        break;
      if (c.c == '(') {
        ml_file_unread(&p->file, c.c);
        return ml_parser_parse_list(p);
      }
      if ((c.c >= '0' && c.c <= '9') || (c.c == '-')) {
        first = c.c;
        st = STATE_LITERAL;
        break;
      }

      ml_string_concat_char(&strbuf, c.c);
      st = STATE_NAME;
      break;
    case STATE_LITERAL:
      if (c.eof) {
        if (first >= '0' && first <= '9') {
          ml_file_unread(&p->file, first);
          return ml_parser_parse_number(p);
        }
        ml_string_concat_char(&strbuf, first);
        return ml_object_new_name(strbuf);
      }

      if ((first >= '0' && first <= '9') ||
          (first == '-' && (c.c >= '1' && c.c <= '9'))) {
        ml_file_unread(&p->file, c.c);
        ml_file_unread(&p->file, first);
        return ml_parser_parse_number(p);
      }

      ml_string_concat_char(&strbuf, first);
      ml_string_concat_char(&strbuf, c.c);
      st = STATE_NAME;
      break;
    case STATE_NAME:
      if (c.eof || is_whitespace(c.c))
        return ml_object_new_name(strbuf);
      ml_string_concat_char(&strbuf, c.c);
      break;
    }
  }
}

ml_object *ml_parser_parse(ml_parser *p) { return ml_parser_parse_expr(p); }
