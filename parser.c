#include <string.h>

#include "mouselisp.h"

inline static int is_whitespace(unsigned char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline static int is_end_value(unsigned char c) {
  return is_whitespace(c) || c == '(' || c == ')';
}

inline static int is_sign(unsigned char c) {
  return (c == '+') || (c == '-');
}

inline static int is_name(unsigned char c) {
  return (c == '!') || (c == '$') || (c == '%') || (c == '&') || (c == '*') ||
         (c == '/') || (c == ':') || (c == '<') || (c == '=') || (c == '>') ||
         (c == '?') || (c == '^') || (c == '_') || (c == '~') ||
         (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline static int is_number(unsigned char c) {
  return c >= '0' && c <= '9';
}

inline static int is_name_rest(unsigned char c) {
  return is_sign(c) || is_name(c) || is_number(c);
}

ml_parser ml_parser_new_str(const char *str) {
  return (ml_parser){.file = (ml_file){.str = str}};
}

/* TODO: 小数も扱えるようにしたい。 */
ml_object *ml_parser_parse_literal_number(ml_parser *p) {
  enum { STATE_START, STATE_ZERO, STATE_INTERGER } st = STATE_START;
  ml_read_char c;
  int sign = 1;
  int ret = 0;
  int digit, shift;

  for (;;) {
    c = ml_file_read(&p->file);

    switch (st) {
    case STATE_START:
      if (c.eof)
        goto invalid;

      if (c.c == '0') {
        st = STATE_ZERO;
        break;
      }

      if (c.c >= '1' && c.c <= '9') {
        ret += c.c - '0';
        st = STATE_INTERGER;
        break;
      }

      if (c.c == '+') {
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
      if (c.eof || is_end_value(c.c)) {
        if (!c.eof)
          ml_file_unread(&p->file, c.c);
        return ml_object_new_number(ret);
      }

      goto invalid;
    case STATE_INTERGER:
      if (c.eof || is_end_value(c.c)) {
        if (!c.eof)
          ml_file_unread(&p->file, c.c);
        return ml_object_new_number(ret);
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
}

ml_object *ml_parser_parse_expr(ml_parser *p);

ml_object *ml_parser_parse_list(ml_parser *p) {
  enum { STATE_START, STATE_LIST } st = STATE_START;
  ml_read_char c;
  ml_object *expr;
  ml_object *root = the_nil;
  ml_object *tail = root;

  for (;;) {
    c = ml_file_read(&p->file);

    switch (st) {
    case STATE_START:
      if (c.eof)
        goto invalid;

      if (c.c == '(') {
        st = STATE_LIST;
        break;
      }

      goto invalid;
    case STATE_LIST:
      if (c.eof)
        goto invalid;
      if (is_whitespace(c.c))
        continue;

      if (c.c == ')') {
        if (root != the_nil)
          return root;
        goto invalid;
      }

      ml_file_unread(&p->file, c.c);
      expr = ml_parser_parse_expr(p);
      if (expr != NULL) {
        if (root == the_nil) {
          root = tail = ml_object_new_cons(expr, the_nil);
        } else {
          tail->cons.cdr = ml_object_new_cons(expr, the_nil);
          tail = tail->cons.cdr;
        }
        break;
      }

      goto invalid;
    }
  }

invalid:
  return NULL;
}

/* TODO: NULLを使わず例外を使うかも */
ml_object *ml_parser_parse_expr(ml_parser *p) {
  enum { STATE_START, STATE_LITERAL_NUMBER, STATE_NAME } st = STATE_START;

  ml_read_char c;
  char first;
  ml_string strbuf = (ml_string){0};

  for (;;) {
    c = ml_file_read(&p->file);

    switch (st) {
    case STATE_START:
      if (c.eof)
        goto invalid;
      if (is_whitespace(c.c))
        break;

      if (c.c == '(') {
        ml_file_unread(&p->file, c.c);
        return ml_parser_parse_list(p);
      }
      if (is_number(c.c) || is_sign(c.c)) {
        first = c.c;
        st = STATE_LITERAL_NUMBER;
        break;
      }

      if (is_name(c.c)) {
        ml_string_concat_char(&strbuf, c.c);
        st = STATE_NAME;
        break;
      }

      goto invalid;
    case STATE_LITERAL_NUMBER:
      if (c.eof || is_end_value(c.c)) {
        if (!c.eof)
          ml_file_unread(&p->file, c.c);
        if (is_sign(first)) {
          ml_string_concat_char(&strbuf, first);
          goto name_end;
        } else {
          ml_file_unread(&p->file, first);
          return ml_parser_parse_literal_number(p);
        }
      }

      if (is_number(c.c)) {
        ml_file_unread(&p->file, c.c);
        ml_file_unread(&p->file, first);
        return ml_parser_parse_literal_number(p);
      }

      if (is_name(c.c) || is_sign(c.c)) {
        if (is_sign(first)) {
          ml_string_concat_char(&strbuf, first);
          ml_string_concat_char(&strbuf, c.c);
          st = STATE_NAME;
          break;
        }
      }

      goto invalid;
    case STATE_NAME:
      if (c.eof || is_end_value(c.c)) {
        if (!c.eof)
          ml_file_unread(&p->file, c.c);
        goto name_end;
      }

      if (is_name_rest(c.c)) {
        ml_string_concat_char(&strbuf, c.c);
        break;
      }

      goto invalid;
    }
  }

invalid:
  return NULL;
name_end:
  if (strcmp(strbuf.str, "nil") == 0)
    return the_nil;
  else if (strcmp(strbuf.str, "true") == 0)
    return ml_object_new_bool(1);
  else if (strcmp(strbuf.str, "false") == 0)
    return ml_object_new_bool(0);
  else
    return ml_object_new_name(strbuf.str);
}

ml_object *ml_parser_parse(ml_parser *p) {
  ml_read_char c;
  ml_object *expr;
  ml_object *root = the_nil;
  ml_object *tail = root;

  for (;;) {
    c = ml_file_read(&p->file);
    if (c.eof)
      return root;
    if (is_whitespace(c.c))
      continue;

    ml_file_unread(&p->file, c.c);
    expr = ml_parser_parse_expr(p);
    if (expr != NULL) {
      if (root == the_nil) {
        root = tail = ml_object_new_cons(expr, the_nil);
      } else {
        tail->cons.cdr = ml_object_new_cons(expr, the_nil);
        tail = tail->cons.cdr;
      }
      continue;
    }

    goto invalid;
  }

invalid:
  return NULL;
}
