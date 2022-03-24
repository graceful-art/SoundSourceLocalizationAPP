/*
 * File: shiftdim.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

/* Include Files */
#include "shiftdim.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double x[155520]
 *                double b[155520]
 * Return Type  : void
 */
void shiftdim(const double x[155520], double b[155520])
{
  int k;
  for (k = 0; k < 1296; k++) {
    memcpy(&b[k * 120], &x[k * 120], 120U * sizeof(double));
  }
}

/*
 * File trailer for shiftdim.c
 *
 * [EOF]
 */
