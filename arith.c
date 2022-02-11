/* arithmetic operators */

#include <limits.h>

#include "mouselisp.h"

/* return: error number */
int chk_muli(int *dst, int a, int b) {
  if (a == 0 || b == 0) {
    *dst = 0;
    return 0;
  }

  if ((a > 0 && b > 0 && (a > (INT_MAX / b))) ||
      (a > 0 && b < 0 && (b < (INT_MIN / a))) ||
      (a < 0 && b > 0 && (a < (INT_MIN / b))) ||
      (a < 0 && b < 0 && (b < (INT_MAX / a))))
    return 1;

  *dst = a * b;
  return 0;
}

/* return: error number */
int chk_addi(int *dst, int a, int b) {
  if ((b > 0 && a > (INT_MAX - b)) || (b < 0 && a < (INT_MIN - b)))
    return 1;

  *dst = a + b;
  return 0;
}
