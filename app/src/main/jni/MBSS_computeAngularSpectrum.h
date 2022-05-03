/*
 * File: MBSS_computeAngularSpectrum.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
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
void MBSS_computeAngularSpectrum(
    const double d[120], const double alpha[434520],
    const cell_wrap_2 alphaSampled[120], const cell_wrap_2 tauGrid[120],
    const creal_T X[24576], const double freqBins_data[],
    const int freqBins_size[2], double specInst[10863]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for MBSS_computeAngularSpectrum.h
 *
 * [EOF]
 */
