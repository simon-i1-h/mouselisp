#ifndef MOUSELISP_HPP
#define MOUSELISP_HPP

#include <cstdarg>
#include <string>

namespace mouselisp {

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

/* file */

struct ReadChar {
  char c;
  bool eof;
  size_t pos;
};

struct File {
  std::string str;
  size_t pos;

  File(std::string str) : str(str), pos(0) {}

  ReadChar read(File &f);
};

} // namespace mouselisp

#endif /* MOUSELISP_HPP */
