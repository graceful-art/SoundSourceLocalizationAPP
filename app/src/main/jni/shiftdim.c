/*
 * File: shiftdim.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
 */

/* Include Files */
#include "shiftdim.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double x[3931320]
 *                double b[3931320]
 * Return Type  : void
 */
void shiftdim(const double x[3931320], double b[3931320])
{
  int k;
  for (k = 0; k < 32761; k++) {
    memcpy(&b[k * 120], &x[k * 120], 120U * sizeof(double));
  }
}

/*
 * File trailer for shiftdim.c
 *
 * [EOF]
 */
