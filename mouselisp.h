#ifndef MOUSELISP_H
#define MOUSELISP_H

#include <gc/gc.h>

#include <stdarg.h>
#include <stddef.h>

/* utilities */

#define stringify2(s) #s
#define stringify(s) stringify2(s)

#define ATTR_FMT_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#define ATTR_NORETURN __attribute__((noreturn))

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

/* file */

typedef struct ml_file {
  char *str;
  size_t pos;
} ml_file;

typedef struct ml_read_char {
  char c;
  int eof;
  size_t pos;
} ml_read_char;

ml_file ml_file_new_str(char *str);
ml_read_char ml_file_read(ml_file *file);

/* string */

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

ml_string ml_string_new(void);
ml_string ml_string_new_str(const char *str);
void ml_string_concat_char(ml_string *s, int c);
void ml_string_concat_string(ml_string *s, const char *str);

#endif /* MOUSELISP_H */
