/*
 * File: sph2cart.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

/* Include Files */
#include "sph2cart.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : const double az[1296]
 *                const double elev[1296]
 *                double x[1296]
 *                double y[1296]
 *                double z[1296]
 * Return Type  : void
 */
void sph2cart(const double az[1296], const double elev[1296], double x[1296],
              double y[1296], double z[1296])
{
  double d;
  double d1;
  int k;
  for (k = 0; k < 1296; k++) {
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
