/*
 * File: permute.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 25-Apr-2022 17:17:50
 */

/* Include Files */
#include "permute.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double a[1303560]
 *                double b[1303560]
 * Return Type  : void
 */
void b_permute(const double a[1303560], double b[1303560])
{
  int b_b_tmp;
  int b_k;
  int b_tmp;
  int k;
  for (k = 0; k < 120; k++) {
    for (b_k = 0; b_k < 3621; b_k++) {
      b_tmp = 3 * b_k + 10863 * k;
      b_b_tmp = 3 * k + 360 * b_k;
      b[b_b_tmp] = a[b_tmp];
      b[b_b_tmp + 1] = a[b_tmp + 1];
      b[b_b_tmp + 2] = a[b_tmp + 2];
    }
  }
}

/*
 * Arguments    : const creal_T a[24576]
 *                creal_T b[24576]
 * Return Type  : void
 */
void permute(const creal_T a[24576], creal_T b[24576])
{
  int b_k;
  int c_k;
  int d_k;
  int k;
  for (k = 0; k < 3; k++) {
    for (b_k = 0; b_k < 32; b_k++) {
      for (c_k = 0; c_k < 16; c_k++) {
        for (d_k = 0; d_k < 16; d_k++) {
          b[((b_k + (k << 5)) + 96 * d_k) + 1536 * c_k] =
              a[((d_k + (c_k << 4)) + (b_k << 8)) + (k << 13)];
        }
      }
    }
  }
}

/*
 * File trailer for permute.c
 *
 * [EOF]
 */
