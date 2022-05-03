/*
 * File: sum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

/* Include Files */
#include "sum.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double x[1303560]
 *                double y[434520]
 * Return Type  : void
 */
void b_sum(const double x[1303560], double y[434520])
{
  int xi;
  int xpageoffset;
  for (xi = 0; xi < 434520; xi++) {
    xpageoffset = xi * 3;
    y[xi] = (x[xpageoffset] + x[xpageoffset + 1]) + x[xpageoffset + 2];
  }
}

/*
 * Arguments    : const double x[360]
 *                double y[120]
 * Return Type  : void
 */
void sum(const double x[360], double y[120])
{
  int k;
  int xj;
  int xoffset;
  memcpy(&y[0], &x[0], 120U * sizeof(double));
  for (k = 0; k < 2; k++) {
    xoffset = (k + 1) * 120;
    for (xj = 0; xj < 120; xj++) {
      y[xj] += x[xoffset + xj];
    }
  }
}

/*
 * File trailer for sum.c
 *
 * [EOF]
 */
