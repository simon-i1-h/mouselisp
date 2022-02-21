/* arithmetic operators */

#include <limits.h>

#include "mouselisp.h"

int chk_muli(ml_machine *m, int a, int b) {
  if ((a > 0 && b > 0 && a > (INT_MAX / b)) ||
      (a > 0 && b < 0 && b < (INT_MIN / a)) ||
      (a < 0 && b > 0 && a < (INT_MIN / b)) ||
      (a < 0 && b < 0 && b < (INT_MAX / a)))
    ml_throw(m, ml_arith_error(m, the_nil));

  return a * b;
}

int chk_addi(ml_machine *m, int a, int b) {
  if ((a > 0 && b > (INT_MAX - a)) || (a < 0 && b < (INT_MIN - a)))
    ml_throw(m, ml_arith_error(m, the_nil));

  return a + b;
}

int chk_subi(ml_machine *m, int a, int b) {
  if ((b > 0 && a < (INT_MIN + b)) || (b < 0 && a > (INT_MAX + b)))
    ml_throw(m, ml_arith_error(m, the_nil));

  return a - b;
}

int chk_divi(ml_machine *m, int a, int b) {
  if ((b == 0) || (b == -1 && a == INT_MIN))
    ml_throw(m, ml_arith_error(m, the_nil));

  return a / b;
}

int chk_modi(ml_machine *m, int a, int b) {
  if ((b == 0) || (b == -1 && a == INT_MIN))
    ml_throw(m, ml_arith_error(m, the_nil));

  return a % b;
}
