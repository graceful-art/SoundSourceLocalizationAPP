/*
 * File: sum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 22-Dec-2021 15:50:26
 */

/* Include Files */
#include "sum.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double x[1814760]
 *                double y[604920]
 * Return Type  : void
 */
void sum(const double x[1814760], double y[604920])
{
  int xi;
  int xpageoffset;
  for (xi = 0; xi < 604920; xi++) {
    xpageoffset = xi * 3;
    y[xi] = (x[xpageoffset] + x[xpageoffset + 1]) + x[xpageoffset + 2];
  }
}

/*
 * File trailer for sum.c
 *
 * [EOF]
 */
