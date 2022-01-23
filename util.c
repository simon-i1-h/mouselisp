/* utilities */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "mouselisp.h"

void vlogmsgx(const char *filename, const char *line, int newline,
              const char *fmt, va_list ap) {
  if (filename != NULL)
    fprintf(stderr, "%s: ", filename);
  if (line != NULL)
    fprintf(stderr, "%s: ", line);

  vfprintf(stderr, fmt, ap);
  if (newline)
    fprintf(stderr, "\n");
}

ATTR_FMT_PRINTF(3, 4)
void logmsg2(const char *filename, const char *line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vlogmsgx(filename, line, 1, fmt, ap);
  va_end(ap);
}

ATTR_FMT_PRINTF(3, 4)
void logmsgf2(const char *filename, const char *line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vlogmsgx(filename, line, 0, fmt, ap);
  va_end(ap);
}

void bug2(const char *filename, const char *line, int err) {
  if (err) {
    logmsg2(filename, line, "bug");
    exit(1);
  }
}

ATTR_NORETURN
ATTR_FMT_PRINTF(3, 4)
void fatal2(const char *filename, const char *line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vlogmsgx(filename, line, 1, fmt, ap);
  va_end(ap);
  exit(1);
}

void *xgcmalloc(size_t size) {
  void *ret = GC_MALLOC(size);
  if (ret == NULL)
    fatal("GC_MALLOC");

  return ret;
}

/*
 * 常に有効なアドレスを返すので、呼び出し元は一時変数を使わずに次のよ
 * うに呼び出せる。
 *
 *   ptr = xgcrealloc(ptr, newsize)
 *
 */
void *xgcrealloc(void *ptr, size_t size) {
  void *ret = GC_REALLOC(ptr, size);
  if (ret == NULL)
    fatal("GC_REALLOC");

  return ret;
}
