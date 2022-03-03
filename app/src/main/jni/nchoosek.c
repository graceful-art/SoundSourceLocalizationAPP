/*
 * File: nchoosek.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 12:17:08
 */

/* Include Files */
#include "nchoosek.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const double x[16]
 *                double y[240]
 * Return Type  : void
 */
void nchoosek(const double x[16], double y[240])
{
  int comb[2];
  int a;
  int combj;
  int icomb;
  int nmkpi;
  int row;
  comb[0] = 1;
  comb[1] = 2;
  icomb = 1;
  nmkpi = 16;
  for (row = 0; row < 120; row++) {
    y[row] = x[comb[0] - 1];
    y[row + 120] = x[comb[1] - 1];
    if (icomb + 1 > 0) {
      a = comb[icomb];
      combj = comb[icomb] + 1;
      comb[icomb]++;
      if (a + 1 < nmkpi) {
        icomb += 2;
        for (nmkpi = icomb; nmkpi < 3; nmkpi++) {
          combj++;
          comb[1] = combj;
        }
        icomb = 1;
        nmkpi = 16;
      } else {
        icomb--;
        nmkpi--;
      }
    }
  }
}

/*
 * File trailer for nchoosek.c
 *
 * [EOF]
 */
