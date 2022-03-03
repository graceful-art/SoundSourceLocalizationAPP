/*
 * File: colon.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:23:14
 */

/* Include Files */
#include "colon.h"
#include "main5_emxutil.h"
#include "main5_types.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : double a
 *                double b
 *                emxArray_real_T *y
 * Return Type  : void
 */
void eml_float_colon(double a, double b, emxArray_real_T *y)
{
  double apnd;
  double cdiff;
  double ndbl;
  double u0;
  double u1;
  int k;
  int n;
  int nm1d2;
  ndbl = floor((b - a) / 5.0 + 0.5);
  apnd = a + ndbl * 5.0;
  cdiff = apnd - b;
  u0 = fabs(a);
  u1 = fabs(b);
  if ((u0 > u1) || rtIsNaN(u1)) {
    u1 = u0;
  }
  if (fabs(cdiff) < 4.4408920985006262E-16 * u1) {
    ndbl++;
    apnd = b;
  } else if (cdiff > 0.0) {
    apnd = a + (ndbl - 1.0) * 5.0;
  } else {
    ndbl++;
  }
  if (ndbl >= 0.0) {
    n = (int)ndbl;
  } else {
    n = 0;
  }
  nm1d2 = y->size[0] * y->size[1];
  y->size[0] = 1;
  y->size[1] = n;
  emxEnsureCapacity_real_T(y, nm1d2);
  if (n > 0) {
    y->data[0] = a;
    if (n > 1) {
      y->data[n - 1] = apnd;
      nm1d2 = (n - 1) / 2;
      for (k = 0; k <= nm1d2 - 2; k++) {
        ndbl = ((double)k + 1.0) * 5.0;
        y->data[k + 1] = a + ndbl;
        y->data[(n - k) - 2] = apnd - ndbl;
      }
      if (nm1d2 << 1 == n - 1) {
        y->data[nm1d2] = (a + apnd) / 2.0;
      } else {
        ndbl = (double)nm1d2 * 5.0;
        y->data[nm1d2] = a + ndbl;
        y->data[nm1d2 + 1] = apnd - ndbl;
      }
    }
  }
}

/*
 * File trailer for colon.c
 *
 * [EOF]
 */
