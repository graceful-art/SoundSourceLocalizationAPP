/*
 * File: MBSS_preprocess.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
 */

#ifndef MBSS_PREPROCESS_H
#define MBSS_PREPROCESS_H

/* Include Files */
#include "main5_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void MBSS_preprocess(const double thetaGrid[32761], const double phiGrid[32761],
                     double alpha[3931320], cell_wrap_2 alphaSampled[120],
                     cell_wrap_2 tauGrid[120]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for MBSS_preprocess.h
 *
 * [EOF]
 */
