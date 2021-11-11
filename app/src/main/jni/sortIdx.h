/*
 * File: sortIdx.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
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
void merge_block(int idx[32761], double x[32761], int offset, int n,
                 int preSortLevel, int iwork[32761], double xwork[32761]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for sortIdx.h
 *
 * [EOF]
 */
