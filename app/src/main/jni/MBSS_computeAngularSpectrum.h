/*
 * File: MBSS_computeAngularSpectrum.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
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
void MBSS_computeAngularSpectrum(const double alpha[3931320],
                                 const cell_wrap_2 alphaSampled[120],
                                 const cell_wrap_2 tauGrid[120],
                                 const creal_T X[131072],
                                 double specInst[65522]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for MBSS_computeAngularSpectrum.h
 *
 * [EOF]
 */
