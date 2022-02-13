#ifndef MOUSELISP_H
#define MOUSELISP_H

#include <gc/gc.h>

#include <stdarg.h>
#include <stddef.h>

/* util.c */

#define stringify2(s) #s
#define stringify(s) stringify2(s)

#define ATTR_FMT_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#define ATTR_NORETURN __attribute__((noreturn))
#define ATTR_FALLTHROUGH __attribute__((fallthrough))

#define countof(a) (sizeof(a) / sizeof(a[0]))

void vlogmsgx(const char *filename, const char *line, int newline,
              const char *fmt, va_list ap);

#define logmsg(...) logmsg2(__FILE__, stringify(__LINE__), __VA_ARGS__)
#define rlogmsg(...) logmsg2(NULL, NULL, __VA_ARGS__)

ATTR_FMT_PRINTF(3, 4)
void logmsg2(const char *filename, const char *line, const char *fmt, ...);

#define logmsgf(...) logmsgf2(__FILE__, stringify(__LINE__), __VA_ARGS__)
#define rlogmsgf(...) logmsgf2(NULL, NULL, __VA_ARGS__)

ATTR_FMT_PRINTF(3, 4)
void logmsgf2(const char *filename, const char *line, const char *fmt, ...);

/* shorthand of early return */
#define bug(err) bug2(__FILE__, stringify(__LINE__), err)

void bug2(const char *filename, const char *line, int err);

#define fatal(...) fatal2(__FILE__, stringify(__LINE__), __VA_ARGS__)

ATTR_NORETURN
ATTR_FMT_PRINTF(3, 4)
void fatal2(const char *filename, const char *line, const char *fmt, ...);

void *xgcmalloc(size_t size);
void *xgcrealloc(void *ptr, size_t size);

/* file.c */

typedef struct ml_file {
  const char *str;
  size_t pos;

  /* for unread */
  char buf[2];
  size_t buf_pos;
} ml_file;

typedef struct ml_read_char {
  char c;
  int eof;
  size_t pos;
} ml_read_char;

ml_read_char ml_file_read(ml_file *f);
void ml_file_unread(ml_file *f, char c);

/* string.c */

typedef struct ml_string {
  /*
   * nul文字終端。
   *
   * 文字列型を変化させるような関数呼び出しを行うと参照が無効に
   * なる可能性がある。reallocにより確保し直される可能性があるた
   * め。
   * https://cpprefjp.github.io/reference/string/basic_string/c_str.html
   */
  char *str;

  size_t len;
  size_t capacity;
} ml_string;

ml_string ml_string_new_str(const char *str);
void ml_string_concat_char(ml_string *s, int c);
void ml_string_concat_string(ml_string *s, const char *str);
int ml_string_strcmp(ml_string *a, ml_string *b);

/* arith.c */

int chk_muli(int *dst, int a, int b);
int chk_addi(int *dst, int a, int b);

/* object.c */

typedef struct ml_object ml_object;

typedef struct ml_cons {
  ml_object *car;
  ml_object *cdr;
} ml_cons;

typedef enum ml_function_tag {
  ML_FUNCTION_NORMAL,
  ML_FUNCTION_BUILTIN
} ml_function_tag;

typedef ml_object *(ml_builtin)(ml_object *);

typedef struct ml_function {
  ml_function_tag tag;
  union {
    ml_object *root;
    ml_builtin *builtin;
  };
} ml_function;

/*
 * TODO: byte (array)
 */
typedef enum ml_object_tag {
  ML_OBJECT_NIL,
  ML_OBJECT_CONS,
  ML_OBJECT_BOOL,
  ML_OBJECT_NUMBER,
  ML_OBJECT_STRING,
  ML_OBJECT_NAME,
  ML_OBJECT_FUNCTION
} ml_object_tag;

typedef struct ml_object {
  ml_object_tag tag;
  union {
    ml_cons cons;
    int boolean;
    int num; /* TODO double? */
    ml_string str; /* string or name */
    ml_function func;
  };
} ml_object;

ml_object *ml_object_new_nil(void);
ml_object *ml_object_new_cons(ml_object *car, ml_object *cdr);
ml_object *ml_object_new_bool(int boolean);
ml_object *ml_object_new_number(int num);
ml_object *ml_object_new_string(const char *str);
ml_object *ml_object_new_name(const char *str);
ml_object *ml_object_new_builtin_function(ml_builtin *builtin);
int ml_list_exists(ml_object *list, ml_object *ptr);
void ml_object_debug_dump_recur(ml_object *obj, ml_object **known_objs,
                                int depth);

#define ml_object_debug_dump(obj)                                            \
  ml_object_debug_dump2(__FILE__, stringify(__LINE__), obj)

void ml_object_debug_dump2(const char *filename, const char *line,
                           ml_object *root);

/* parser.c */

typedef struct ml_parser {
  ml_file file;
} ml_parser;

ml_parser ml_parser_new_str(const char *str);
ml_object *ml_parser_parse(ml_parser *p);

/* machine.c */

typedef struct ml_machine {
  ml_object *named_objs;
} ml_machine;

ml_machine ml_machine_new(void);
ml_object *ml_machine_eval(ml_machine *m, ml_object *root);

/* builtin.c */

ml_object *ml_add(ml_object *args);
ml_object *ml_sub(ml_object *args);
ml_object *ml_mul(ml_object *args);
ml_object *ml_div(ml_object *args);
ml_object *ml_car(ml_object *args);
ml_object *ml_cdr(ml_object *args);
ml_object *ml_cons_(ml_object *args); /* TODO naming */

/* init.c */

/* singleton */
extern ml_object *the_nil;

void mouselisp_init(void);

/* t/test.c */

void test_main(void);
void test_object_dump_main(const char *testname);

#endif /* MOUSELISP_H */
