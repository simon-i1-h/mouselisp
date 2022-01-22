#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "mouselisp.hpp"

namespace mouselisp {

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

} // namespace mouselisp
