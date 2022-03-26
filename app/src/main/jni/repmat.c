/*
 * File: repmat.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 26-Mar-2022 15:53:05
 */

/* Include Files */
#include "repmat.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double a[360]
 *                double b[1814760]
 * Return Type  : void
 */
void b_repmat(const double a[360], double b[1814760])
{
  int iacol;
  int ibmat;
  int ibtile;
  int jcol;
  int jtilecol;
  for (jtilecol = 0; jtilecol < 5041; jtilecol++) {
    ibtile = jtilecol * 360 - 1;
    for (jcol = 0; jcol < 120; jcol++) {
      iacol = jcol * 3;
      ibmat = ibtile + jcol * 3;
      b[ibmat + 1] = a[iacol];
      b[ibmat + 2] = a[iacol + 1];
      b[ibmat + 3] = a[iacol + 2];
    }
  }
}

/*
 * Arguments    : const double a[120]
 *                double b[604920]
 * Return Type  : void
 */
void c_repmat(const double a[120], double b[604920])
{
  int ibtile;
  int jtilecol;
  for (jtilecol = 0; jtilecol < 5041; jtilecol++) {
    ibtile = jtilecol * 120;
    memcpy(&b[ibtile], &a[0], 120U * sizeof(double));
  }
}

/*
 * Arguments    : const double a[15123]
 *                double b[1814760]
 * Return Type  : void
 */
void repmat(const double a[15123], double b[1814760])
{
  int iacol;
  int ibmat;
  int ibtile;
  int jcol;
  int jtilecol;
  for (jtilecol = 0; jtilecol < 120; jtilecol++) {
    ibtile = jtilecol * 15123 - 1;
    for (jcol = 0; jcol < 5041; jcol++) {
      iacol = jcol * 3;
      ibmat = ibtile + jcol * 3;
      b[ibmat + 1] = a[iacol];
      b[ibmat + 2] = a[iacol + 1];
      b[ibmat + 3] = a[iacol + 2];
    }
  }
}

/*
 * File trailer for repmat.c
 *
 * [EOF]
 */
