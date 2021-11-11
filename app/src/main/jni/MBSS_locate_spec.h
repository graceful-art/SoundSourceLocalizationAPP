/*
 * File: MBSS_locate_spec.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
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
void MBSS_findPeaks2D(const double ppfSpec[32761],
                      const double piAzimutGrid[32761],
                      const double piElevationGrid[32761],
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
