#ifndef MOUSELISP_H
#define MOUSELISP_H

#include <gc/gc.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

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

ATTR_FMT_PRINTF(1, 2)
char *xgcsprintf(const char *fmt, ...);

/* file.c */

typedef enum ml_file_tag { ML_FILE_STRING, ML_FILE_FILE } ml_file_tag;

typedef struct ml_file {
  ml_file_tag tag;
  union {
    const char *str;
    FILE *file;
  };

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

ml_file ml_file_new_str(const char *str);
ml_file ml_file_new_file(FILE *file);
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

/* arith.c */

typedef struct ml_machine ml_machine;

int chk_muli(ml_machine *m, int a, int b);
int chk_addi(ml_machine *m, int a, int b);
int chk_subi(ml_machine *m, int a, int b);
int chk_divi(ml_machine *m, int a, int b);
int chk_modi(ml_machine *m, int a, int b);

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

typedef ml_object *(ml_builtin)(ml_machine *, ml_object *);

typedef struct ml_function {
  ml_function_tag tag;
  union {
    struct {
      ml_object *args;
      ml_object *body;
    } normal;
    ml_builtin *builtin;
  };
  ml_object *closure;
} ml_function;

typedef struct {
  ml_object *args;
  ml_object *body;
} ml_macro;

typedef enum ml_object_tag {
  ML_OBJECT_CONS,
  ML_OBJECT_BOOL,
  ML_OBJECT_NUMBER,
  ML_OBJECT_NAME,
  ML_OBJECT_FUNCTION,
  ML_OBJECT_POINTER,
  ML_OBJECT_MACRO
} ml_object_tag;

typedef struct ml_object {
  ml_object_tag tag;
  union {
    ml_cons cons;
    int boolean;
    int num;
    ml_string str; /* name */
    ml_function func;
    ml_object *ptr;
    ml_macro macro;
  };
} ml_object;

ml_object *ml_object_new_nil(void);
ml_object *ml_object_new_cons(ml_object *car, ml_object *cdr);
ml_object *ml_object_new_bool(int boolean);
ml_object *ml_object_new_number(int num);
ml_object *ml_object_new_name(const char *str);
ml_object *ml_object_new_builtin_function(ml_builtin *builtin);
ml_object *ml_object_new_normal_function(ml_object *closure, ml_object *args,
                                         ml_object *body);
ml_object *ml_object_new_pointer(ml_object *ptr);
ml_object *ml_object_new_macro(ml_object *args, ml_object *body);
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
ml_parser ml_parser_new_file(FILE *file);
ml_object *ml_parser_parse(ml_parser *p, ml_machine *m);
#define ml_parser_xparse(...)                                                \
  ml_parser_xparse2(__FILE__, stringify(__LINE__), __VA_ARGS__)
ml_object *ml_parser_xparse2(const char *filename, const char *line,
                             ml_parser *p, ml_machine *m);

/* machine.c */

typedef struct ml_machine {
  ml_object *named_objs;
  jmp_buf *last_exc_handler;
  /* エラーオブジェクトの形式: (error-func-ptr error-func-name ...) */
  ml_object *exc;
} ml_machine;

ml_machine ml_machine_new(void);
ml_object *ml_find_named_object(ml_machine *m, const char *name);
ml_object *ml_machine_eval(ml_machine *m, ml_object *root);
#define ml_machine_xeval(...)                                                \
  ml_machine_xeval2(__FILE__, stringify(__LINE__), __VA_ARGS__)
ml_object *ml_machine_xeval2(const char *filename, const char *line,
                             ml_machine *m, ml_object *root);
#define ml_machine_xeval_top(...)                                            \
  ml_machine_xeval_top2(__FILE__, stringify(__LINE__), __VA_ARGS__)
ml_object *ml_machine_xeval_top2(const char *filename, const char *line,
                                 ml_machine *m, ml_object *root);

/* builtin.c */

ml_object *ml_add(ml_machine *m, ml_object *args);
ml_object *ml_sub(ml_machine *m, ml_object *args);
ml_object *ml_mul(ml_machine *m, ml_object *args);
ml_object *ml_div(ml_machine *m, ml_object *args);
ml_object *ml_mod(ml_machine *m, ml_object *args);
ml_object *ml_car(ml_machine *m, ml_object *args);
ml_object *ml_cdr(ml_machine *m, ml_object *args);
ml_object *ml_cons_(ml_machine *m, ml_object *args);
ml_object *ml_ref(ml_machine *m, ml_object *args);
ml_object *ml_deref(ml_machine *m, ml_object *args);
ml_object *ml_eq(ml_machine *m, ml_object *args);
ml_object *ml_gt(ml_machine *m, ml_object *args);
ml_object *ml_lt(ml_machine *m, ml_object *args);
ml_object *ml_ge(ml_machine *m, ml_object *args);
ml_object *ml_le(ml_machine *m, ml_object *args);
ml_object *ml_and(ml_machine *m, ml_object *args);
ml_object *ml_or(ml_machine *m, ml_object *args);
ml_object *ml_not(ml_machine *m, ml_object *args);
ml_object *ml_is_cons(ml_machine *m, ml_object *args);
ml_object *ml_is_bool(ml_machine *m, ml_object *args);
ml_object *ml_is_num(ml_machine *m, ml_object *args);
ml_object *ml_is_name(ml_machine *m, ml_object *args);
ml_object *ml_is_func(ml_machine *m, ml_object *args);
ml_object *ml_is_ptr(ml_machine *m, ml_object *args);
ml_object *ml_list(ml_machine *m, ml_object *args);
ml_object *ml_unique(ml_machine *m, ml_object *args);
ml_object *ml_eval_error(ml_machine *m, ml_object *args);
ml_object *ml_nil_error(ml_machine *m, ml_object *args);
ml_object *ml_noname_error(ml_machine *m, ml_object *args);
ml_object *ml_syntax_error(ml_machine *m, ml_object *args);
ml_object *ml_arith_error(ml_machine *m, ml_object *args);
ml_object *ml_throw_(ml_machine *m, ml_object *args);
ml_object *ml_debug_dump(ml_machine *m, ml_object *args);

/* init.c */

/* singleton */
extern ml_object *the_nil;

extern int unique_tid;
extern int unique_seq;

extern int exc_log;

void mouselisp_init(void);

/* exception.c */

#define ml_throw(...) ml_throw2(__FILE__, stringify(__LINE__), __VA_ARGS__)

ATTR_NORETURN
void ml_throw2(const char *filename, const char *line, ml_machine *m,
               ml_object *err);

/* t/test.c */

void test_main(void);
void test_object_dump_main(const char *testname);
void test_unique(void);

#endif /* MOUSELISP_H */
