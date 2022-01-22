#include <string>

#include "mouselisp.hpp"

namespace mouselisp {

read_char File::read(File &f) {
  read_char ret = {.c = f.str.at(f.pos), .eof = 0, .pos = f.pos};

  if (f.str.at(f.pos) == '\0')
    ret.eof = 1;
  else
    f.pos++;

  return ret;
}

} // namespace mouselisp
