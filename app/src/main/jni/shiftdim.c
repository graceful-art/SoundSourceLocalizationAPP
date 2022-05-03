/*
 * File: shiftdim.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

/* Include Files */
#include "shiftdim.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double x[434520]
 *                double b[434520]
 * Return Type  : void
 */
void shiftdim(const double x[434520], double b[434520])
{
  int k;
  for (k = 0; k < 3621; k++) {
    memcpy(&b[k * 120], &x[k * 120], 120U * sizeof(double));
  }
}

/*
 * File trailer for shiftdim.c
 *
 * [EOF]
 */
