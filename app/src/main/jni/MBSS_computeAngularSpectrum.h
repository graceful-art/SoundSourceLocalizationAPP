/*
 * File: MBSS_computeAngularSpectrum.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 22-Dec-2021 14:18:57
 */

#ifndef MBSS_COMPUTEANGULARSPECTRUM_H
#define MBSS_COMPUTEANGULARSPECTRUM_H

/* Include Files */
#include "main5_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void MBSS_computeAngularSpectrum(const double alpha[604920],
                                 const cell_wrap_0 alphaSampled[120],
                                 const cell_wrap_0 tauGrid[120],
                                 const creal_T X[393216],
                                 double specInst[15123]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for MBSS_computeAngularSpectrum.h
 *
 * [EOF]
 */
