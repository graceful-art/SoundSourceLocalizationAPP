/*
 * File: sum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

/* Include Files */
#include "sum.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double x[466560]
 *                double y[155520]
 * Return Type  : void
 */
void sum(const double x[466560], double y[155520])
{
  int xi;
  int xpageoffset;
  for (xi = 0; xi < 155520; xi++) {
    xpageoffset = xi * 3;
    y[xi] = (x[xpageoffset] + x[xpageoffset + 1]) + x[xpageoffset + 2];
  }
}

/*
 * File trailer for sum.c
 *
 * [EOF]
 */
