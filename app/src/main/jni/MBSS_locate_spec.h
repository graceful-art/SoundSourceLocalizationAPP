/*
 * File: MBSS_locate_spec.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 26-Mar-2022 15:53:05
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
void MBSS_findPeaks2D(const double ppfSpec[5041],
                      const double piAzimutGrid[5041],
                      const double piElevationGrid[5041],
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
