/*
 * File: sort.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 12-Feb-2022 13:36:32
 */

/* Include Files */
#include "sort.h"
#include "rt_nonfinite.h"
#include "sortIdx.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : double x[5041]
 *                int idx[5041]
 * Return Type  : void
 */
void sort(double x[5041], int idx[5041])
{
  double xwork[5041];
  double b_xwork[256];
  double x4[4];
  double d;
  double d1;
  int iwork[5041];
  int b;
  int bLen;
  int bLen2;
  int b_b;
  int exitg1;
  int i1;
  int i2;
  int i3;
  int i4;
  int ib;
  int k;
  int nBlocks;
  int nNaNs;
  int nNonNaN;
  int nPairs;
  short b_iwork[256];
  short idx4[4];
  signed char perm[4];
  x4[0] = 0.0;
  idx4[0] = 0;
  x4[1] = 0.0;
  idx4[1] = 0;
  x4[2] = 0.0;
  idx4[2] = 0;
  x4[3] = 0.0;
  idx4[3] = 0;
  memset(&idx[0], 0, 5041U * sizeof(int));
  memset(&xwork[0], 0, 5041U * sizeof(double));
  nNaNs = 0;
  ib = 0;
  for (k = 0; k < 5041; k++) {
    if (rtIsNaN(x[k])) {
      idx[5040 - nNaNs] = k + 1;
      xwork[5040 - nNaNs] = x[k];
      nNaNs++;
    } else {
      ib++;
      idx4[ib - 1] = (short)(k + 1);
      x4[ib - 1] = x[k];
      if (ib == 4) {
        ib = k - nNaNs;
        if (x4[0] >= x4[1]) {
          i1 = 1;
          i2 = 2;
        } else {
          i1 = 2;
          i2 = 1;
        }
        if (x4[2] >= x4[3]) {
          i3 = 3;
          i4 = 4;
        } else {
          i3 = 4;
          i4 = 3;
        }
        d = x4[i1 - 1];
        d1 = x4[i3 - 1];
        if (d >= d1) {
          d = x4[i2 - 1];
          if (d >= d1) {
            perm[0] = (signed char)i1;
            perm[1] = (signed char)i2;
            perm[2] = (signed char)i3;
            perm[3] = (signed char)i4;
          } else if (d >= x4[i4 - 1]) {
            perm[0] = (signed char)i1;
            perm[1] = (signed char)i3;
            perm[2] = (signed char)i2;
            perm[3] = (signed char)i4;
          } else {
            perm[0] = (signed char)i1;
            perm[1] = (signed char)i3;
            perm[2] = (signed char)i4;
            perm[3] = (signed char)i2;
          }
        } else {
          d1 = x4[i4 - 1];
          if (d >= d1) {
            if (x4[i2 - 1] >= d1) {
              perm[0] = (signed char)i3;
              perm[1] = (signed char)i1;
              perm[2] = (signed char)i2;
              perm[3] = (signed char)i4;
            } else {
              perm[0] = (signed char)i3;
              perm[1] = (signed char)i1;
              perm[2] = (signed char)i4;
              perm[3] = (signed char)i2;
            }
          } else {
            perm[0] = (signed char)i3;
            perm[1] = (signed char)i4;
            perm[2] = (signed char)i1;
            perm[3] = (signed char)i2;
          }
        }
        idx[ib - 3] = idx4[perm[0] - 1];
        idx[ib - 2] = idx4[perm[1] - 1];
        idx[ib - 1] = idx4[perm[2] - 1];
        idx[ib] = idx4[perm[3] - 1];
        x[ib - 3] = x4[perm[0] - 1];
        x[ib - 2] = x4[perm[1] - 1];
        x[ib - 1] = x4[perm[2] - 1];
        x[ib] = x4[perm[3] - 1];
        ib = 0;
      }
    }
  }
  if (ib > 0) {
    perm[1] = 0;
    perm[2] = 0;
    perm[3] = 0;
    if (ib == 1) {
      perm[0] = 1;
    } else if (ib == 2) {
      if (x4[0] >= x4[1]) {
        perm[0] = 1;
        perm[1] = 2;
      } else {
        perm[0] = 2;
        perm[1] = 1;
      }
    } else if (x4[0] >= x4[1]) {
      if (x4[1] >= x4[2]) {
        perm[0] = 1;
        perm[1] = 2;
        perm[2] = 3;
      } else if (x4[0] >= x4[2]) {
        perm[0] = 1;
        perm[1] = 3;
        perm[2] = 2;
      } else {
        perm[0] = 3;
        perm[1] = 1;
        perm[2] = 2;
      }
    } else if (x4[0] >= x4[2]) {
      perm[0] = 2;
      perm[1] = 1;
      perm[2] = 3;
    } else if (x4[1] >= x4[2]) {
      perm[0] = 2;
      perm[1] = 3;
      perm[2] = 1;
    } else {
      perm[0] = 3;
      perm[1] = 2;
      perm[2] = 1;
    }
    for (k = 0; k < ib; k++) {
      i2 = perm[k] - 1;
      i1 = ((k - nNaNs) - ib) + 5041;
      idx[i1] = idx4[i2];
      x[i1] = x4[i2];
    }
  }
  i1 = (nNaNs >> 1) + 5041;
  for (k = 0; k <= i1 - 5042; k++) {
    ib = (k - nNaNs) + 5041;
    i2 = idx[ib];
    idx[ib] = idx[5040 - k];
    idx[5040 - k] = i2;
    x[ib] = xwork[5040 - k];
    x[5040 - k] = xwork[ib];
  }
  if ((nNaNs & 1) != 0) {
    ib = i1 - nNaNs;
    x[ib] = xwork[ib];
  }
  memset(&iwork[0], 0, 5041U * sizeof(int));
  nNonNaN = 5041 - nNaNs;
  ib = 2;
  if (5041 - nNaNs > 1) {
    nBlocks = (5041 - nNaNs) >> 8;
    if (nBlocks > 0) {
      for (b = 0; b < nBlocks; b++) {
        i4 = (b << 8) - 1;
        for (b_b = 0; b_b < 6; b_b++) {
          bLen = 1 << (b_b + 2);
          bLen2 = bLen << 1;
          nPairs = 256 >> (b_b + 3);
          for (k = 0; k < nPairs; k++) {
            i2 = (i4 + k * bLen2) + 1;
            for (i1 = 0; i1 < bLen2; i1++) {
              ib = i2 + i1;
              b_iwork[i1] = (short)idx[ib];
              b_xwork[i1] = x[ib];
            }
            i3 = 0;
            i1 = bLen;
            ib = i2 - 1;
            do {
              exitg1 = 0;
              ib++;
              if (b_xwork[i3] >= b_xwork[i1]) {
                idx[ib] = b_iwork[i3];
                x[ib] = b_xwork[i3];
                if (i3 + 1 < bLen) {
                  i3++;
                } else {
                  exitg1 = 1;
                }
              } else {
                idx[ib] = b_iwork[i1];
                x[ib] = b_xwork[i1];
                if (i1 + 1 < bLen2) {
                  i1++;
                } else {
                  ib -= i3;
                  for (i1 = i3 + 1; i1 <= bLen; i1++) {
                    i2 = ib + i1;
                    idx[i2] = b_iwork[i1 - 1];
                    x[i2] = b_xwork[i1 - 1];
                  }
                  exitg1 = 1;
                }
              }
            } while (exitg1 == 0);
          }
        }
      }
      ib = nBlocks << 8;
      i1 = 5041 - (nNaNs + ib);
      if (i1 > 0) {
        merge_block(idx, x, ib, i1, 2, iwork, xwork);
      }
      ib = 8;
    }
    merge_block(idx, x, 0, 5041 - nNaNs, ib, iwork, xwork);
  }
  if ((nNaNs > 0) && (5041 - nNaNs > 0)) {
    for (k = 0; k < nNaNs; k++) {
      ib = (k - nNaNs) + 5041;
      xwork[k] = x[ib];
      iwork[k] = idx[ib];
    }
    for (k = nNonNaN; k >= 1; k--) {
      ib = (nNaNs + k) - 1;
      x[ib] = x[k - 1];
      idx[ib] = idx[k - 1];
    }
    if (0 <= nNaNs - 1) {
      memcpy(&x[0], &xwork[0], nNaNs * sizeof(double));
      memcpy(&idx[0], &iwork[0], nNaNs * sizeof(int));
    }
  }
}

/*
 * File trailer for sort.c
 *
 * [EOF]
 */
