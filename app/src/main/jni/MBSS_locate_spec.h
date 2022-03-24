/*
 * File: MBSS_locate_spec.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

#ifndef MBSS_LOCATE_SPEC_H
#define MBSS_LOCATE_SPEC_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void MBSS_findPeaks2D(const double ppfSpec[1296],
                      const double piAzimutGrid[1296],
                      const double piElevationGrid[1296],
                      double pfEstAngles[2]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for MBSS_locate_spec.h
 *
 * [EOF]
 */
