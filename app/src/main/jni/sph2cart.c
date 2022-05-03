/*
 * File: sph2cart.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

/* Include Files */
#include "sph2cart.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : const double az[3621]
 *                const double elev[3621]
 *                double x[3621]
 *                double y[3621]
 *                double z[3621]
 * Return Type  : void
 */
void sph2cart(const double az[3621], const double elev[3621], double x[3621],
              double y[3621], double z[3621])
{
  double d;
  double d1;
  int k;
  for (k = 0; k < 3621; k++) {
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
