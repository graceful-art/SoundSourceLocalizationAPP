/*
 * File: sph2cart.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Dec-2021 11:09:20
 */

/* Include Files */
#include "sph2cart.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : const double az[5041]
 *                const double elev[5041]
 *                double x[5041]
 *                double y[5041]
 *                double z[5041]
 * Return Type  : void
 */
void sph2cart(const double az[5041], const double elev[5041], double x[5041],
              double y[5041], double z[5041])
{
  double d;
  double d1;
  int k;
  for (k = 0; k < 5041; k++) {
    d = elev[k];
    z[k] = sin(d);
    d = cos(d);
    d1 = az[k];
    x[k] = d * cos(d1);
    d *= sin(d1);
    y[k] = d;
  }
}

/*
 * File trailer for sph2cart.c
 *
 * [EOF]
 */
