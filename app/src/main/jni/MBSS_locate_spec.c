/*
 * File: MBSS_locate_spec.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
 */

/* Include Files */
#include "MBSS_locate_spec.h"
#include "rt_nonfinite.h"
#include "sort.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double ppfSpec[32761]
 *                const double piAzimutGrid[32761]
 *                const double piElevationGrid[32761]
 *                double pfEstAngles[2]
 * Return Type  : void
 */
void MBSS_findPeaks2D(const double ppfSpec[32761],
                      const double piAzimutGrid[32761],
                      const double piElevationGrid[32761],
                      double pfEstAngles[2])
{
  static double ppfPadpeakFilter[33489];
  static double b_ppfSpec2D[32761];
  static double ppfSpec2D[32761];
  static int iidx[32761];
  double minval[181];
  double d;
  double ex;
  int b_i;
  int b_ppfSpec2D_tmp;
  int i;
  int idx;
  int ppfSpec2D_tmp;
  boolean_T exitg1;
  boolean_T p;
  /*  PEAKS FINDING METHODS */
  /*  Convert angular spectrum in 2D */
  for (i = 0; i < 181; i++) {
    for (b_i = 0; b_i < 181; b_i++) {
      ppfSpec2D[b_i + 181 * i] = ppfSpec[i + 181 * b_i];
    }
  }
  /*  All values of flat peaks are detected as peaks with this implementation :
   */
  for (i = 0; i < 33489; i++) {
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
  for (idx = 0; idx < 181; idx++) {
    memcpy(&ppfPadpeakFilter[idx * 183 + 184], &ppfSpec2D[idx * 181],
           181U * sizeof(double));
    minval[idx] = ppfSpec2D[181 * idx];
    for (b_i = 0; b_i < 180; b_i++) {
      d = ppfSpec2D[(b_i + 181 * idx) + 1];
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
    while ((!exitg1) && (b_i <= 181)) {
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
    for (b_i = i; b_i < 182; b_i++) {
      d = minval[b_i - 1];
      if (ex > d) {
        ex = d;
      }
    }
  }
  /*  substract min value : avoid issues (when sorting peaks) if some peaks
   * values are negatives */
  /*  sort values of local maxima */
  for (i = 0; i < 181; i++) {
    for (b_i = 0; b_i < 181; b_i++) {
      idx = i + 183 * (b_i + 1);
      d = ppfPadpeakFilter[idx + 1];
      ppfSpec2D_tmp = i + 183 * b_i;
      b_ppfSpec2D_tmp = i + 183 * (b_i + 2);
      b_ppfSpec2D[b_i + 181 * i] =
          (ppfSpec2D[i + 181 * b_i] - ex) *
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
  for (i = 0; i < 32761; i++) {
    ppfSpec2D[i] = iidx[i];
  }
  /*  first source is the global maximum (first one in piSortedPeaksIndex1D) */
  /*  search index in piSortedPeaksIndex1D */
  /*  set to one as global maximum is already selected as source */
  idx = (int)ppfSpec2D[0] - 1;
  pfEstAngles[0] = piAzimutGrid[idx];
  pfEstAngles[1] = piElevationGrid[idx];
}

/*
 * File trailer for MBSS_locate_spec.c
 *
 * [EOF]
 */
