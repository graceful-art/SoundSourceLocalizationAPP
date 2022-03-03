/*
 * File: shiftdim.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:35:04
 */

/* Include Files */
#include "shiftdim.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double x[604920]
 *                double b[604920]
 * Return Type  : void
 */
void shiftdim(const double x[604920], double b[604920])
{
  int k;
  for (k = 0; k < 5041; k++) {
    memcpy(&b[k * 120], &x[k * 120], 120U * sizeof(double));
  }
}

/*
 * File trailer for shiftdim.c
 *
 * [EOF]
 */
