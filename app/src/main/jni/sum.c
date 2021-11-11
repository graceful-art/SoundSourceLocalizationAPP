/*
 * File: sum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
 */

/* Include Files */
#include "sum.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double x[11793960]
 *                double y[3931320]
 * Return Type  : void
 */
void sum(const double x[11793960], double y[3931320])
{
  int xi;
  int xpageoffset;
  for (xi = 0; xi < 3931320; xi++) {
    xpageoffset = xi * 3;
    y[xi] = (x[xpageoffset] + x[xpageoffset + 1]) + x[xpageoffset + 2];
  }
}

/*
 * File trailer for sum.c
 *
 * [EOF]
 */
