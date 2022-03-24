/*
 * File: MBSS_locate_spec.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 24-Mar-2022 14:32:31
 */

/* Include Files */
#include "MBSS_locate_spec.h"
#include "rt_nonfinite.h"
#include "sort.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double ppfSpec[1296]
 *                const double piAzimutGrid[1296]
 *                const double piElevationGrid[1296]
 *                double pfEstAngles[2]
 * Return Type  : void
 */
void MBSS_findPeaks2D(const double ppfSpec[1296],
                      const double piAzimutGrid[1296],
                      const double piElevationGrid[1296], double pfEstAngles[2])
{
  double ppfPadpeakFilter[1444];
  double b_ppfSpec2D[1296];
  double ppfSpec2D[1296];
  double minval[36];
  double d;
  double ex;
  int iidx[1296];
  int b_i;
  int b_ppfSpec2D_tmp;
  int i;
  int idx;
  int ppfSpec2D_tmp;
  boolean_T exitg1;
  boolean_T p;
  /*  PEAKS FINDING METHODS */
  /*  Convert angular spectrum in 2D */
  for (i = 0; i < 36; i++) {
    for (b_i = 0; b_i < 36; b_i++) {
      ppfSpec2D[b_i + 36 * i] = ppfSpec[i + 36 * b_i];
    }
  }
  /*  All values of flat peaks are detected as peaks with this implementation :
   */
  for (i = 0; i < 1444; i++) {
    ppfPadpeakFilter[i] = rtMinusInf;
  }
  /*  Find peaks : compare values with their neighbours */
  /*  % top */
  /*  % bottom */
  /*  % right */
  /*  % left */
  /*  % top/left */
  /*  % top/right */
  /*  % bottom/left */
  /*  bottom/right */
  /*  number of local maxima */
  /*  local maxima with corrresponding values */
  for (idx = 0; idx < 36; idx++) {
    memcpy(&ppfPadpeakFilter[idx * 38 + 39], &ppfSpec2D[idx * 36],
           36U * sizeof(double));
    minval[idx] = ppfSpec2D[36 * idx];
    for (b_i = 0; b_i < 35; b_i++) {
      d = ppfSpec2D[(b_i + 36 * idx) + 1];
      if (rtIsNaN(d)) {
        p = false;
      } else if (rtIsNaN(minval[idx])) {
        p = true;
      } else {
        p = (minval[idx] > d);
      }
      if (p) {
        minval[idx] = d;
      }
    }
  }
  if (!rtIsNaN(minval[0])) {
    idx = 1;
  } else {
    idx = 0;
    b_i = 2;
    exitg1 = false;
    while ((!exitg1) && (b_i <= 36)) {
      if (!rtIsNaN(minval[b_i - 1])) {
        idx = b_i;
        exitg1 = true;
      } else {
        b_i++;
      }
    }
  }
  if (idx == 0) {
    ex = minval[0];
  } else {
    ex = minval[idx - 1];
    i = idx + 1;
    for (b_i = i; b_i < 37; b_i++) {
      d = minval[b_i - 1];
      if (ex > d) {
        ex = d;
      }
    }
  }
  /*  substract min value : avoid issues (when sorting peaks) if some peaks
   * values are negatives */
  /*  sort values of local maxima */
  for (i = 0; i < 36; i++) {
    for (b_i = 0; b_i < 36; b_i++) {
      idx = i + 38 * (b_i + 1);
      d = ppfPadpeakFilter[idx + 1];
      ppfSpec2D_tmp = i + 38 * b_i;
      b_ppfSpec2D_tmp = i + 38 * (b_i + 2);
      b_ppfSpec2D[b_i + 36 * i] =
          (ppfSpec2D[i + 36 * b_i] - ex) *
          (double)((d >= ppfPadpeakFilter[idx]) &&
                   (d >= ppfPadpeakFilter[idx + 2]) &&
                   (d >= ppfPadpeakFilter[ppfSpec2D_tmp + 1]) &&
                   (d >= ppfPadpeakFilter[b_ppfSpec2D_tmp + 1]) &&
                   (d >= ppfPadpeakFilter[ppfSpec2D_tmp]) &&
                   (d >= ppfPadpeakFilter[b_ppfSpec2D_tmp]) &&
                   (d >= ppfPadpeakFilter[ppfSpec2D_tmp + 2]) &&
                   (d >= ppfPadpeakFilter[b_ppfSpec2D_tmp + 2]));
    }
  }
  sort(b_ppfSpec2D, iidx);
  for (i = 0; i < 1296; i++) {
    ppfSpec2D[i] = iidx[i];
  }
  /*  first source is the global maximum (first one in piSortedPeaksIndex1D) */
  /*  search index in piSortedPeaksIndex1D */
  /*  set to one as global maximum is already selected as source */
  /* Filter the list of peaks found with respect to minAngle parameter */
  idx = (int)ppfSpec2D[0] - 1;
  pfEstAngles[0] = piAzimutGrid[idx];
  pfEstAngles[1] = piElevationGrid[idx];
}

/*
 * File trailer for MBSS_locate_spec.c
 *
 * [EOF]
 */
