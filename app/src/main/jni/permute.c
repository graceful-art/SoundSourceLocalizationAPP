/*
 * File: permute.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:35:04
 */

/* Include Files */
#include "permute.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double a[1814760]
 *                double b[1814760]
 * Return Type  : void
 */
void b_permute(const double a[1814760], double b[1814760])
{
  int b_b_tmp;
  int b_k;
  int b_tmp;
  int k;
  for (k = 0; k < 120; k++) {
    for (b_k = 0; b_k < 5041; b_k++) {
      b_tmp = 3 * b_k + 15123 * k;
      b_b_tmp = 3 * k + 360 * b_k;
      b[b_b_tmp] = a[b_tmp];
      b[b_b_tmp + 1] = a[b_tmp + 1];
      b[b_b_tmp + 2] = a[b_tmp + 2];
    }
  }
}

/*
 * Arguments    : const creal_T a[1572864]
 *                creal_T b[1572864]
 * Return Type  : void
 */
void permute(const creal_T a[1572864], creal_T b[1572864])
{
  int b_k;
  int c_k;
  int d_k;
  int k;
  for (k = 0; k < 3; k++) {
    for (b_k = 0; b_k < 2048; b_k++) {
      for (c_k = 0; c_k < 16; c_k++) {
        for (d_k = 0; d_k < 16; d_k++) {
          b[((b_k + (k << 11)) + 6144 * d_k) + 98304 * c_k] =
              a[((d_k + (c_k << 4)) + (b_k << 8)) + (k << 19)];
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
