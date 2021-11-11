/*
 * File: sph2cart.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
 */

/* Include Files */
#include "sph2cart.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : const double az[32761]
 *                const double elev[32761]
 *                double x[32761]
 *                double y[32761]
 *                double z[32761]
 * Return Type  : void
 */
void sph2cart(const double az[32761], const double elev[32761], double x[32761],
              double y[32761], double z[32761])
{
  double d;
  double d1;
  int k;
  for (k = 0; k < 32761; k++) {
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
