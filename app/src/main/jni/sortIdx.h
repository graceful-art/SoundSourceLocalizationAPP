/*
 * File: sortIdx.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

#ifndef SORTIDX_H
#define SORTIDX_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void merge_block(int idx[1296], double x[1296], int offset, int n,
                 int preSortLevel, int iwork[1296], double xwork[1296]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for sortIdx.h
 *
 * [EOF]
 */
