/*
 * File: MBSS_preprocess.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
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
                     double alpha[434520], cell_wrap_2 alphaSampled[120],
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
