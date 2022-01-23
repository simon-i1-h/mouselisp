#include <stdint.h>
#include <string.h>

#include "mouselisp.h"

/* machine */

typedef struct ml_machine {
  int fake;
} ml_machine;

ml_object *ml_machine_eval(ml_machine *m, ml_object *root) {
  if (root->tag == ML_OBJECT_NUMBER)
    return root;

  if (root->tag == ML_OBJECT_CONS &&
      root->u.cons.cdr->tag == ML_OBJECT_CONS &&
      root->u.cons.cdr->u.cons.cdr->tag == ML_OBJECT_CONS) {
    ml_object *f = root->u.cons.car;
    ml_object *a = ml_machine_eval(m, root->u.cons.cdr->u.cons.car);
    ml_object *b =
        ml_machine_eval(m, root->u.cons.cdr->u.cons.cdr->u.cons.car);
    if (f->tag == ML_OBJECT_NAME) {
      /* apply builtin functions */
      if (strcmp(f->u.str.str, "+") == 0 && a->tag == ML_OBJECT_NUMBER &&
          b->tag == ML_OBJECT_NUMBER)
        return ml_object_new_number(a->u.num + b->u.num);
    }
  }

  ml_object_debug_dump(root);
  fatal("evaluate failed.");
}

/*
 * parser
 *
 * TODO: レキサーとパーサーを分離。いやしないかも。
 */

typedef struct ml_parser {
  ml_file file;
} ml_parser;

inline static int is_whitespace(unsigned char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
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
      if (c.eof || is_whitespace(c.c) || c.c == ')')
        goto valid;
      goto invalid;
    case STATE_INTERGER:
      if (c.eof || is_whitespace(c.c) || c.c == ')')
        goto valid;
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
  ml_file_unread(&p->file, c.c);
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

/* main */

int main(void) {
  GC_INIT();
  the_nil = ml_object_new_nil();

  ml_object_debug_dump(the_nil);

  ml_object *t2_car = ml_object_new_number(42.0);
  ml_object *t2_cdr = ml_object_new_bool(0);
  ml_object *t2_cons = ml_object_new_cons(t2_car, t2_cdr);
  ml_object_debug_dump(t2_cons);

  ml_object *t3_str = ml_object_new_string(ml_string_new_str("fox"));
  ml_object *t3_cons = ml_object_new_cons(t3_str, t3_str);
  ml_object_debug_dump(t3_cons);

  ml_object *t4_list_1 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("foo")), the_nil);
  ml_object *t4_list_2 = ml_object_new_cons(the_nil, t4_list_1);
  ml_object *t4_list_3 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("baz")), t4_list_2);
  ml_object *t4_list_4 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("qux")), t4_list_3);
  ml_object *t4_list_5 = ml_object_new_cons(
      ml_object_new_string(ml_string_new_str("quux")), t4_list_4);
  t4_list_2->u.cons.car = t4_list_4;
  ml_object_debug_dump(t4_list_5);

  ml_machine t5_machine = (ml_machine){0};
  ml_object *t5_f = ml_object_new_name(ml_string_new_str("+"));
  ml_object *t5_a = ml_object_new_number(32);
  ml_object *t5_b = ml_object_new_number(-0.5);
  ml_object *t5_list_1 = ml_object_new_cons(t5_b, the_nil);
  ml_object *t5_list_2 = ml_object_new_cons(t5_a, t5_list_1);
  ml_object *t5_list_3 = ml_object_new_cons(t5_f, t5_list_2);
  ml_object_debug_dump(ml_machine_eval(&t5_machine, t5_list_3));

  ml_object *t6_nil = ml_object_new_nil();
  ml_object_debug_dump(t6_nil);

  ml_machine t7_machine = (ml_machine){0};
  ml_object *t7_f = ml_object_new_name(ml_string_new_str("+"));
  ml_object *t7_a = ml_object_new_number(25.5);
  ml_object *t7_b = t5_list_3;
  ml_object *t7_list_1 = ml_object_new_cons(t7_b, the_nil);
  ml_object *t7_list_2 = ml_object_new_cons(t7_a, t7_list_1);
  ml_object *t7_list_3 = ml_object_new_cons(t7_f, t7_list_2);
  ml_object_debug_dump(ml_machine_eval(&t7_machine, t7_list_3));

  ml_machine t8_machine = (ml_machine){0};
  ml_parser t8_parser = (ml_parser){.file = (ml_file){.str = "(+ 3 5)"}};
  ml_object *t8_root = ml_parser_parse(&t8_parser);
  if (t8_root == NULL)
    fatal("t8");
  ml_object_debug_dump(t8_root);
  ml_object_debug_dump(ml_machine_eval(&t8_machine, t8_root));

  ml_machine t9_machine = (ml_machine){0};
  ml_parser t9_parser = (ml_parser){.file = (ml_file){.str = "(+ 1 (+ 2 3))"}};
  ml_object *t9_root = ml_parser_parse(&t9_parser);
  if (t9_root == NULL)
    fatal("t9");
  ml_object_debug_dump(t9_root);
  ml_object_debug_dump(ml_machine_eval(&t9_machine, t9_root));

  return 0;
}
