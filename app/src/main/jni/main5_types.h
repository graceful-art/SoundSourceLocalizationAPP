/*
 * File: main5_types.h
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 17-Nov-2021 10:34:49
 */

#ifndef MAIN5_TYPES_H
#define MAIN5_TYPES_H

/* Include Files */
#include "rtwtypes.h"

/* Type Definitions */
#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T
struct emxArray_real_T {
  double *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};
#endif /* struct_emxArray_real_T */
#ifndef typedef_emxArray_real_T
#define typedef_emxArray_real_T
typedef struct emxArray_real_T emxArray_real_T;
#endif /* typedef_emxArray_real_T */

#ifndef typedef_cell_wrap_2
#define typedef_cell_wrap_2
typedef struct {
  emxArray_real_T *f1;
} cell_wrap_2;
#endif /* typedef_cell_wrap_2 */

#ifndef typedef_emxArray_creal_T
#define typedef_emxArray_creal_T
typedef struct {
  creal_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
} emxArray_creal_T;
#endif /* typedef_emxArray_creal_T */

#endif
/*
 * File trailer for main5_types.h
 *
 * [EOF]
 */
