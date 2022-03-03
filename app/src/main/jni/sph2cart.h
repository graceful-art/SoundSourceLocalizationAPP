/*
 * File: sph2cart.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:35:04
 */

#ifndef SPH2CART_H
#define SPH2CART_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void sph2cart(const double az[5041], const double elev[5041], double x[5041],
              double y[5041], double z[5041]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for sph2cart.h
 *
 * [EOF]
 */
