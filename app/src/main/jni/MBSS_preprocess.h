/*
 * File: MBSS_preprocess.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 26-Mar-2022 15:53:05
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
void MBSS_preprocess(const double micPos[48], double dMic[120],
                     double alpha[604920], cell_wrap_0 alphaSampled[120],
                     cell_wrap_0 tauGrid[120]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for MBSS_preprocess.h
 *
 * [EOF]
 */
