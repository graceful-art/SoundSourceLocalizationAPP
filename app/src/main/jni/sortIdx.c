/*
 * File: sortIdx.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:35:04
 */

/* Include Files */
#include "sortIdx.h"
#include "rt_nonfinite.h"

/* Function Declarations */
static void merge(int idx[5041], double x[5041], int offset, int np, int nq,
                  int iwork[5041], double xwork[5041]);

/* Function Definitions */
/*
 * Arguments    : int idx[5041]
 *                double x[5041]
 *                int offset
 *                int np
 *                int nq
 *                int iwork[5041]
 *                double xwork[5041]
 * Return Type  : void
 */
static void merge(int idx[5041], double x[5041], int offset, int np, int nq,
                  int iwork[5041], double xwork[5041])
{
  int exitg1;
  int iout;
  int j;
  int n_tmp;
  int p;
  int q;
  if (nq != 0) {
    n_tmp = np + nq;
    for (j = 0; j < n_tmp; j++) {
      iout = offset + j;
      iwork[j] = idx[iout];
      xwork[j] = x[iout];
    }
    p = 0;
    q = np;
    iout = offset - 1;
    do {
      exitg1 = 0;
      iout++;
      if (xwork[p] >= xwork[q]) {
        idx[iout] = iwork[p];
        x[iout] = xwork[p];
        if (p + 1 < np) {
          p++;
        } else {
          exitg1 = 1;
        }
      } else {
        idx[iout] = iwork[q];
        x[iout] = xwork[q];
        if (q + 1 < n_tmp) {
          q++;
        } else {
          q = iout - p;
          for (j = p + 1; j <= np; j++) {
            iout = q + j;
            idx[iout] = iwork[j - 1];
            x[iout] = xwork[j - 1];
          }
          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);
  }
}

/*
 * Arguments    : int idx[5041]
 *                double x[5041]
 *                int offset
 *                int n
 *                int preSortLevel
 *                int iwork[5041]
 *                double xwork[5041]
 * Return Type  : void
 */
void merge_block(int idx[5041], double x[5041], int offset, int n,
                 int preSortLevel, int iwork[5041], double xwork[5041])
{
  int bLen;
  int nPairs;
  int nTail;
  int tailOffset;
  nPairs = n >> preSortLevel;
  bLen = 1 << preSortLevel;
  while (nPairs > 1) {
    if ((nPairs & 1) != 0) {
      nPairs--;
      tailOffset = bLen * nPairs;
      nTail = n - tailOffset;
      if (nTail > bLen) {
        merge(idx, x, offset + tailOffset, bLen, nTail - bLen, iwork, xwork);
      }
    }
    tailOffset = bLen << 1;
    nPairs >>= 1;
    for (nTail = 0; nTail < nPairs; nTail++) {
      merge(idx, x, offset + nTail * tailOffset, bLen, bLen, iwork, xwork);
    }
    bLen = tailOffset;
  }
  if (n > bLen) {
    merge(idx, x, offset, bLen, n - bLen, iwork, xwork);
  }
}

/*
 * File trailer for sortIdx.c
 *
 * [EOF]
 */
