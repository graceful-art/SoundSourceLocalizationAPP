/*
 * File: sortIdx.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 22-Dec-2021 15:50:26
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
void merge_block(int idx[5041], double x[5041], int offset, int n,
                 int preSortLevel, int iwork[5041], double xwork[5041]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for sortIdx.h
 *
 * [EOF]
 */
