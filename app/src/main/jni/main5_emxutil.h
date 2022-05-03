/*
 * File: main5_emxutil.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

#ifndef MAIN5_EMXUTIL_H
#define MAIN5_EMXUTIL_H

/* Include Files */
#include "main5_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void emxEnsureCapacity_real_T(emxArray_real_T *emxArray, int oldNumel);

extern void emxFreeMatrix_cell_wrap_2(cell_wrap_2 pMatrix[120]);

extern void emxFreeStruct_cell_wrap_2(cell_wrap_2 *pStruct);

extern void emxFree_real_T(emxArray_real_T **pEmxArray);

extern void emxInitMatrix_cell_wrap_2(cell_wrap_2 pMatrix[120]);

extern void emxInitStruct_cell_wrap_2(cell_wrap_2 *pStruct);

extern void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for main5_emxutil.h
 *
 * [EOF]
 */
