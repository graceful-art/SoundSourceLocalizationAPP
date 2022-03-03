/*
 * File: mtimes.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:23:14
 */

/* Include Files */
#include "mtimes.h"
#include "main5_types.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const emxArray_creal_T *A
 *                const emxArray_creal_T *B
 *                creal_T C[256]
 * Return Type  : void
 */
void mtimes(const emxArray_creal_T *A, const emxArray_creal_T *B,
            creal_T C[256])
{
  double B_im;
  double B_re;
  double s_im;
  double s_re;
  int B_re_tmp;
  int B_re_tmp_tmp;
  int coffset;
  int i;
  int inner;
  int j;
  int k;
  inner = A->size[1];
  for (j = 0; j < 16; j++) {
    coffset = j << 4;
    for (i = 0; i < 16; i++) {
      s_re = 0.0;
      s_im = 0.0;
      for (k = 0; k < inner; k++) {
        B_re_tmp_tmp = k << 4;
        B_re_tmp = B_re_tmp_tmp + j;
        B_re = B->data[B_re_tmp].re;
        B_im = -B->data[B_re_tmp].im;
        B_re_tmp_tmp += i;
        s_re +=
            A->data[B_re_tmp_tmp].re * B_re - A->data[B_re_tmp_tmp].im * B_im;
        s_im +=
            A->data[B_re_tmp_tmp].re * B_im + A->data[B_re_tmp_tmp].im * B_re;
      }
      B_re_tmp_tmp = coffset + i;
      C[B_re_tmp_tmp].re = s_re;
      C[B_re_tmp_tmp].im = s_im;
    }
  }
}

/*
 * File trailer for mtimes.c
 *
 * [EOF]
 */
