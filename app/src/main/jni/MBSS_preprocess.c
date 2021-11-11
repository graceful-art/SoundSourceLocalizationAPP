/*
 * File: MBSS_preprocess.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Nov-2021 11:11:15
 */

/* Include Files */
#include "MBSS_preprocess.h"
#include "acosd.h"
#include "colon.h"
#include "main5_emxutil.h"
#include "main5_types.h"
#include "minOrMax.h"
#include "permute.h"
#include "repmat.h"
#include "rt_nonfinite.h"
#include "shiftdim.h"
#include "sph2cart.h"
#include "sum.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Function MBSS_preprocess
 *  This function preprocess values needed to compute and aggregate
 *  the angular spectrum over all microphone pairs:
 *  - Combination of all microphone pair indexes
 *  - Angles corresponding to each direction {azimuth, elevation} for each microphone pair
 *  - Sampled angles array for each microphone pair
 *  - Corresponding TDOAs for each microphone pair
 *  This function depends only on the microphone positions and the search
 *  angle grid boundaries & resolution (array of potential direction for sources).
 *
 *  INPUT:
 *  c         : 1x1 , Speed of sound
 *  micPos    : N x 3 , cartesian coordinates of the N microphones
 *  thetaGrid : 1 x nDirection : Azimuth grid
 *  phiGrid   : 1 x nDirection : Elevation grid
 *  alphaRes  : 1x1, interpolation resolution
 *
 *  OUTPUT:
 *  pairId       : nMicPair x 2, All microphone pair indexes
 *  dMic         : nMicPair x 1, distance between microphones for each pair
 *  alpha        : nMicPair x nDirection : Array of angles for each
 *                 microphone pair corresponding to all {theta, phi} to be
 *                 tested.
 *  alphaSampled : 1 x nMicPair cell array, each cell element contains the
 *                 uniformly distributed angles to be tested for the
 *                 corresponding pair
 *  tauGrid      : 1 x nMicPair cell array, each cell element contains the
 *                 TDOA corresponding to the alphaSampled for each pair
 *
 *
 * Arguments    : const double thetaGrid[32761]
 *                const double phiGrid[32761]
 *                double alpha[3931320]
 *                cell_wrap_2 alphaSampled[120]
 *                cell_wrap_2 tauGrid[120]
 * Return Type  : void
 */
void MBSS_preprocess(const double thetaGrid[32761], const double phiGrid[32761],
                     double alpha[3931320], cell_wrap_2 alphaSampled[120],
                     cell_wrap_2 tauGrid[120])
{
  static creal_T dcv[3931320];
  static double dv3[11793960];
  static double dv4[11793960];
  static double dv5[3931320];
  static double dv6[3931320];
  static double Pjk[98283];
  static double b_dv[32761];
  static double b_phiGrid[32761];
  static double b_thetaGrid[32761];
  static double dv1[32761];
  static double dv2[32761];
  emxArray_real_T *x;
  double a;
  double b;
  int k;
  int nx;

  /*  Number of theta/phi combinations */
  /*  Find all microphone pair indexes */
  /*  Microphone direction vector (in xyz) for each pair */
  /*  Microphone distance for each pair */
  /*  Convert all tuple {theta,phi} on the sphere grid in cartesian coordinates */
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = thetaGrid[k] * 3.1415926535897931 / 180.0;
    b_phiGrid[k] = phiGrid[k] * 3.1415926535897931 / 180.0;
  }

  sph2cart(b_thetaGrid, b_phiGrid, b_dv, dv1, dv2);
  for (k = 0; k < 32761; k++) {
    Pjk[3 * k] = b_dv[k];
    Pjk[3 * k + 1] = dv1[k];
    Pjk[3 * k + 2] = dv2[k];
  }

  /*  Note : Matlab specific method of computation to obtain alpha without loop - uses lots of RAM */
  /*  Duplicate all {theta,phi} coordinates for each pair of microphone  */
  /*  Microphone direction vector duplicate for each {theta,phi} combination */
  /*  alpha for one pair and one {theta,phi} is the angle formed between the microphone */
  /*  direction and the {theta,phi} direction - computation with dot product approach. */
  repmat(Pjk, dv3);
  b_permute(dv3, dv4);
  b_repmat(dv3);
  for (k = 0; k < 11793960; k++) {
    dv4[k] *= dv3[k];
  }

  sum(dv4, dv5);
  shiftdim(dv5, dv6);
  c_repmat(dv5);
  for (k = 0; k < 3931320; k++) {
    dcv[k].re = dv6[k] / dv5[k];
    dcv[k].im = 0.0;
  }

  b_acosd(dcv);
  for (k = 0; k < 3931320; k++) {
    alpha[k] = dcv[k].re;
  }

  /*  Compute 1D angles search grids and associated TDOA (Tau) search grids for each microphone pair   */
  /*  following search grid boundaries for each microphone pair is driven by */
  /*  the following fact : basic boundaries [0� 180] for each pair could be */
  /*  adapted when the global search grid does not cover the entire space */
  /*  (leading to avoid useless curves computation and saving CPU time) */
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[0].f1->size[0] * alphaSampled[0].f1->size[1];
    alphaSampled[0].f1->size[0] = 1;
    alphaSampled[0].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[0].f1, k);
    alphaSampled[0].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[0].f1->size[0] = 1;
    alphaSampled[0].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[0].f1->size[0] * alphaSampled[0].f1->size[1];
    alphaSampled[0].f1->size[0] = 1;
    alphaSampled[0].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[0].f1, k);
    alphaSampled[0].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[0].f1->size[0] * alphaSampled[0].f1->size[1];
    alphaSampled[0].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[0].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[0].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[0].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[0].f1);
  }

  emxInit_real_T(&x, 2);
  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[0].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[0].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[0].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[0].f1->size[0] * tauGrid[0].f1->size[1];
  tauGrid[0].f1->size[0] = 1;
  tauGrid[0].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[0].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[0].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 1];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 1];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[1].f1->size[0] * alphaSampled[1].f1->size[1];
    alphaSampled[1].f1->size[0] = 1;
    alphaSampled[1].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[1].f1, k);
    alphaSampled[1].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[1].f1->size[0] = 1;
    alphaSampled[1].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[1].f1->size[0] * alphaSampled[1].f1->size[1];
    alphaSampled[1].f1->size[0] = 1;
    alphaSampled[1].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[1].f1, k);
    alphaSampled[1].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[1].f1->size[0] * alphaSampled[1].f1->size[1];
    alphaSampled[1].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[1].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[1].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[1].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[1].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[1].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[1].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[1].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[1].f1->size[0] * tauGrid[1].f1->size[1];
  tauGrid[1].f1->size[0] = 1;
  tauGrid[1].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[1].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[1].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 2];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 2];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[2].f1->size[0] * alphaSampled[2].f1->size[1];
    alphaSampled[2].f1->size[0] = 1;
    alphaSampled[2].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[2].f1, k);
    alphaSampled[2].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[2].f1->size[0] = 1;
    alphaSampled[2].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[2].f1->size[0] * alphaSampled[2].f1->size[1];
    alphaSampled[2].f1->size[0] = 1;
    alphaSampled[2].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[2].f1, k);
    alphaSampled[2].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[2].f1->size[0] * alphaSampled[2].f1->size[1];
    alphaSampled[2].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[2].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[2].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[2].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[2].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[2].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[2].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[2].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[2].f1->size[0] * tauGrid[2].f1->size[1];
  tauGrid[2].f1->size[0] = 1;
  tauGrid[2].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[2].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[2].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 3];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 3];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[3].f1->size[0] * alphaSampled[3].f1->size[1];
    alphaSampled[3].f1->size[0] = 1;
    alphaSampled[3].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[3].f1, k);
    alphaSampled[3].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[3].f1->size[0] = 1;
    alphaSampled[3].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[3].f1->size[0] * alphaSampled[3].f1->size[1];
    alphaSampled[3].f1->size[0] = 1;
    alphaSampled[3].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[3].f1, k);
    alphaSampled[3].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[3].f1->size[0] * alphaSampled[3].f1->size[1];
    alphaSampled[3].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[3].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[3].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[3].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[3].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[3].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[3].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[3].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[3].f1->size[0] * tauGrid[3].f1->size[1];
  tauGrid[3].f1->size[0] = 1;
  tauGrid[3].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[3].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[3].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 4];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 4];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[4].f1->size[0] * alphaSampled[4].f1->size[1];
    alphaSampled[4].f1->size[0] = 1;
    alphaSampled[4].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[4].f1, k);
    alphaSampled[4].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[4].f1->size[0] = 1;
    alphaSampled[4].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[4].f1->size[0] * alphaSampled[4].f1->size[1];
    alphaSampled[4].f1->size[0] = 1;
    alphaSampled[4].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[4].f1, k);
    alphaSampled[4].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[4].f1->size[0] * alphaSampled[4].f1->size[1];
    alphaSampled[4].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[4].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[4].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[4].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[4].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[4].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[4].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[4].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[4].f1->size[0] * tauGrid[4].f1->size[1];
  tauGrid[4].f1->size[0] = 1;
  tauGrid[4].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[4].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[4].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 5];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 5];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[5].f1->size[0] * alphaSampled[5].f1->size[1];
    alphaSampled[5].f1->size[0] = 1;
    alphaSampled[5].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[5].f1, k);
    alphaSampled[5].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[5].f1->size[0] = 1;
    alphaSampled[5].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[5].f1->size[0] * alphaSampled[5].f1->size[1];
    alphaSampled[5].f1->size[0] = 1;
    alphaSampled[5].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[5].f1, k);
    alphaSampled[5].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[5].f1->size[0] * alphaSampled[5].f1->size[1];
    alphaSampled[5].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[5].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[5].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[5].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[5].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[5].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[5].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[5].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[5].f1->size[0] * tauGrid[5].f1->size[1];
  tauGrid[5].f1->size[0] = 1;
  tauGrid[5].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[5].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[5].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 6];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 6];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[6].f1->size[0] * alphaSampled[6].f1->size[1];
    alphaSampled[6].f1->size[0] = 1;
    alphaSampled[6].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[6].f1, k);
    alphaSampled[6].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[6].f1->size[0] = 1;
    alphaSampled[6].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[6].f1->size[0] * alphaSampled[6].f1->size[1];
    alphaSampled[6].f1->size[0] = 1;
    alphaSampled[6].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[6].f1, k);
    alphaSampled[6].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[6].f1->size[0] * alphaSampled[6].f1->size[1];
    alphaSampled[6].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[6].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[6].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[6].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[6].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[6].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[6].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[6].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[6].f1->size[0] * tauGrid[6].f1->size[1];
  tauGrid[6].f1->size[0] = 1;
  tauGrid[6].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[6].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[6].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 7];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 7];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[7].f1->size[0] * alphaSampled[7].f1->size[1];
    alphaSampled[7].f1->size[0] = 1;
    alphaSampled[7].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[7].f1, k);
    alphaSampled[7].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[7].f1->size[0] = 1;
    alphaSampled[7].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[7].f1->size[0] * alphaSampled[7].f1->size[1];
    alphaSampled[7].f1->size[0] = 1;
    alphaSampled[7].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[7].f1, k);
    alphaSampled[7].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[7].f1->size[0] * alphaSampled[7].f1->size[1];
    alphaSampled[7].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[7].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[7].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[7].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[7].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[7].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[7].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[7].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[7].f1->size[0] * tauGrid[7].f1->size[1];
  tauGrid[7].f1->size[0] = 1;
  tauGrid[7].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[7].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[7].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 8];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 8];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[8].f1->size[0] * alphaSampled[8].f1->size[1];
    alphaSampled[8].f1->size[0] = 1;
    alphaSampled[8].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[8].f1, k);
    alphaSampled[8].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[8].f1->size[0] = 1;
    alphaSampled[8].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[8].f1->size[0] * alphaSampled[8].f1->size[1];
    alphaSampled[8].f1->size[0] = 1;
    alphaSampled[8].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[8].f1, k);
    alphaSampled[8].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[8].f1->size[0] * alphaSampled[8].f1->size[1];
    alphaSampled[8].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[8].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[8].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[8].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[8].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[8].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[8].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[8].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[8].f1->size[0] * tauGrid[8].f1->size[1];
  tauGrid[8].f1->size[0] = 1;
  tauGrid[8].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[8].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[8].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 9];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 9];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[9].f1->size[0] * alphaSampled[9].f1->size[1];
    alphaSampled[9].f1->size[0] = 1;
    alphaSampled[9].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[9].f1, k);
    alphaSampled[9].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[9].f1->size[0] = 1;
    alphaSampled[9].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[9].f1->size[0] * alphaSampled[9].f1->size[1];
    alphaSampled[9].f1->size[0] = 1;
    alphaSampled[9].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[9].f1, k);
    alphaSampled[9].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[9].f1->size[0] * alphaSampled[9].f1->size[1];
    alphaSampled[9].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[9].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[9].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[9].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[9].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[9].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[9].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[9].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[9].f1->size[0] * tauGrid[9].f1->size[1];
  tauGrid[9].f1->size[0] = 1;
  tauGrid[9].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[9].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[9].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 10];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 10];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[10].f1->size[0] * alphaSampled[10].f1->size[1];
    alphaSampled[10].f1->size[0] = 1;
    alphaSampled[10].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[10].f1, k);
    alphaSampled[10].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[10].f1->size[0] = 1;
    alphaSampled[10].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[10].f1->size[0] * alphaSampled[10].f1->size[1];
    alphaSampled[10].f1->size[0] = 1;
    alphaSampled[10].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[10].f1, k);
    alphaSampled[10].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[10].f1->size[0] * alphaSampled[10].f1->size[1];
    alphaSampled[10].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[10].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[10].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[10].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[10].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[10].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[10].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[10].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[10].f1->size[0] * tauGrid[10].f1->size[1];
  tauGrid[10].f1->size[0] = 1;
  tauGrid[10].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[10].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[10].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 11];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 11];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[11].f1->size[0] * alphaSampled[11].f1->size[1];
    alphaSampled[11].f1->size[0] = 1;
    alphaSampled[11].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[11].f1, k);
    alphaSampled[11].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[11].f1->size[0] = 1;
    alphaSampled[11].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[11].f1->size[0] * alphaSampled[11].f1->size[1];
    alphaSampled[11].f1->size[0] = 1;
    alphaSampled[11].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[11].f1, k);
    alphaSampled[11].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[11].f1->size[0] * alphaSampled[11].f1->size[1];
    alphaSampled[11].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[11].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[11].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[11].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[11].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[11].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[11].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[11].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[11].f1->size[0] * tauGrid[11].f1->size[1];
  tauGrid[11].f1->size[0] = 1;
  tauGrid[11].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[11].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[11].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 12];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 12];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[12].f1->size[0] * alphaSampled[12].f1->size[1];
    alphaSampled[12].f1->size[0] = 1;
    alphaSampled[12].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[12].f1, k);
    alphaSampled[12].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[12].f1->size[0] = 1;
    alphaSampled[12].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[12].f1->size[0] * alphaSampled[12].f1->size[1];
    alphaSampled[12].f1->size[0] = 1;
    alphaSampled[12].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[12].f1, k);
    alphaSampled[12].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[12].f1->size[0] * alphaSampled[12].f1->size[1];
    alphaSampled[12].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[12].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[12].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[12].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[12].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[12].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[12].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[12].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[12].f1->size[0] * tauGrid[12].f1->size[1];
  tauGrid[12].f1->size[0] = 1;
  tauGrid[12].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[12].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[12].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 13];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 13];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[13].f1->size[0] * alphaSampled[13].f1->size[1];
    alphaSampled[13].f1->size[0] = 1;
    alphaSampled[13].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[13].f1, k);
    alphaSampled[13].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[13].f1->size[0] = 1;
    alphaSampled[13].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[13].f1->size[0] * alphaSampled[13].f1->size[1];
    alphaSampled[13].f1->size[0] = 1;
    alphaSampled[13].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[13].f1, k);
    alphaSampled[13].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[13].f1->size[0] * alphaSampled[13].f1->size[1];
    alphaSampled[13].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[13].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[13].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[13].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[13].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[13].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[13].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[13].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[13].f1->size[0] * tauGrid[13].f1->size[1];
  tauGrid[13].f1->size[0] = 1;
  tauGrid[13].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[13].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[13].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 14];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 14];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[14].f1->size[0] * alphaSampled[14].f1->size[1];
    alphaSampled[14].f1->size[0] = 1;
    alphaSampled[14].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[14].f1, k);
    alphaSampled[14].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[14].f1->size[0] = 1;
    alphaSampled[14].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[14].f1->size[0] * alphaSampled[14].f1->size[1];
    alphaSampled[14].f1->size[0] = 1;
    alphaSampled[14].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[14].f1, k);
    alphaSampled[14].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[14].f1->size[0] * alphaSampled[14].f1->size[1];
    alphaSampled[14].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[14].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[14].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[14].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[14].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[14].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[14].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[14].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[14].f1->size[0] * tauGrid[14].f1->size[1];
  tauGrid[14].f1->size[0] = 1;
  tauGrid[14].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[14].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[14].f1->data[k] = 0.10182337649086284 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 15];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 15];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[15].f1->size[0] * alphaSampled[15].f1->size[1];
    alphaSampled[15].f1->size[0] = 1;
    alphaSampled[15].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[15].f1, k);
    alphaSampled[15].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[15].f1->size[0] = 1;
    alphaSampled[15].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[15].f1->size[0] * alphaSampled[15].f1->size[1];
    alphaSampled[15].f1->size[0] = 1;
    alphaSampled[15].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[15].f1, k);
    alphaSampled[15].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[15].f1->size[0] * alphaSampled[15].f1->size[1];
    alphaSampled[15].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[15].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[15].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[15].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[15].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[15].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[15].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[15].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[15].f1->size[0] * tauGrid[15].f1->size[1];
  tauGrid[15].f1->size[0] = 1;
  tauGrid[15].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[15].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[15].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 16];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 16];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[16].f1->size[0] * alphaSampled[16].f1->size[1];
    alphaSampled[16].f1->size[0] = 1;
    alphaSampled[16].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[16].f1, k);
    alphaSampled[16].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[16].f1->size[0] = 1;
    alphaSampled[16].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[16].f1->size[0] * alphaSampled[16].f1->size[1];
    alphaSampled[16].f1->size[0] = 1;
    alphaSampled[16].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[16].f1, k);
    alphaSampled[16].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[16].f1->size[0] * alphaSampled[16].f1->size[1];
    alphaSampled[16].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[16].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[16].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[16].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[16].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[16].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[16].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[16].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[16].f1->size[0] * tauGrid[16].f1->size[1];
  tauGrid[16].f1->size[0] = 1;
  tauGrid[16].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[16].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[16].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 17];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 17];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[17].f1->size[0] * alphaSampled[17].f1->size[1];
    alphaSampled[17].f1->size[0] = 1;
    alphaSampled[17].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[17].f1, k);
    alphaSampled[17].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[17].f1->size[0] = 1;
    alphaSampled[17].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[17].f1->size[0] * alphaSampled[17].f1->size[1];
    alphaSampled[17].f1->size[0] = 1;
    alphaSampled[17].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[17].f1, k);
    alphaSampled[17].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[17].f1->size[0] * alphaSampled[17].f1->size[1];
    alphaSampled[17].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[17].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[17].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[17].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[17].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[17].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[17].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[17].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[17].f1->size[0] * tauGrid[17].f1->size[1];
  tauGrid[17].f1->size[0] = 1;
  tauGrid[17].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[17].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[17].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 18];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 18];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[18].f1->size[0] * alphaSampled[18].f1->size[1];
    alphaSampled[18].f1->size[0] = 1;
    alphaSampled[18].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[18].f1, k);
    alphaSampled[18].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[18].f1->size[0] = 1;
    alphaSampled[18].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[18].f1->size[0] * alphaSampled[18].f1->size[1];
    alphaSampled[18].f1->size[0] = 1;
    alphaSampled[18].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[18].f1, k);
    alphaSampled[18].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[18].f1->size[0] * alphaSampled[18].f1->size[1];
    alphaSampled[18].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[18].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[18].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[18].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[18].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[18].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[18].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[18].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[18].f1->size[0] * tauGrid[18].f1->size[1];
  tauGrid[18].f1->size[0] = 1;
  tauGrid[18].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[18].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[18].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 19];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 19];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[19].f1->size[0] * alphaSampled[19].f1->size[1];
    alphaSampled[19].f1->size[0] = 1;
    alphaSampled[19].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[19].f1, k);
    alphaSampled[19].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[19].f1->size[0] = 1;
    alphaSampled[19].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[19].f1->size[0] * alphaSampled[19].f1->size[1];
    alphaSampled[19].f1->size[0] = 1;
    alphaSampled[19].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[19].f1, k);
    alphaSampled[19].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[19].f1->size[0] * alphaSampled[19].f1->size[1];
    alphaSampled[19].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[19].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[19].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[19].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[19].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[19].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[19].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[19].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[19].f1->size[0] * tauGrid[19].f1->size[1];
  tauGrid[19].f1->size[0] = 1;
  tauGrid[19].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[19].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[19].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 20];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 20];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[20].f1->size[0] * alphaSampled[20].f1->size[1];
    alphaSampled[20].f1->size[0] = 1;
    alphaSampled[20].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[20].f1, k);
    alphaSampled[20].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[20].f1->size[0] = 1;
    alphaSampled[20].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[20].f1->size[0] * alphaSampled[20].f1->size[1];
    alphaSampled[20].f1->size[0] = 1;
    alphaSampled[20].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[20].f1, k);
    alphaSampled[20].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[20].f1->size[0] * alphaSampled[20].f1->size[1];
    alphaSampled[20].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[20].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[20].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[20].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[20].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[20].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[20].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[20].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[20].f1->size[0] * tauGrid[20].f1->size[1];
  tauGrid[20].f1->size[0] = 1;
  tauGrid[20].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[20].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[20].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 21];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 21];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[21].f1->size[0] * alphaSampled[21].f1->size[1];
    alphaSampled[21].f1->size[0] = 1;
    alphaSampled[21].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[21].f1, k);
    alphaSampled[21].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[21].f1->size[0] = 1;
    alphaSampled[21].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[21].f1->size[0] * alphaSampled[21].f1->size[1];
    alphaSampled[21].f1->size[0] = 1;
    alphaSampled[21].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[21].f1, k);
    alphaSampled[21].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[21].f1->size[0] * alphaSampled[21].f1->size[1];
    alphaSampled[21].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[21].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[21].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[21].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[21].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[21].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[21].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[21].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[21].f1->size[0] * tauGrid[21].f1->size[1];
  tauGrid[21].f1->size[0] = 1;
  tauGrid[21].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[21].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[21].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 22];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 22];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[22].f1->size[0] * alphaSampled[22].f1->size[1];
    alphaSampled[22].f1->size[0] = 1;
    alphaSampled[22].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[22].f1, k);
    alphaSampled[22].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[22].f1->size[0] = 1;
    alphaSampled[22].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[22].f1->size[0] * alphaSampled[22].f1->size[1];
    alphaSampled[22].f1->size[0] = 1;
    alphaSampled[22].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[22].f1, k);
    alphaSampled[22].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[22].f1->size[0] * alphaSampled[22].f1->size[1];
    alphaSampled[22].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[22].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[22].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[22].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[22].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[22].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[22].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[22].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[22].f1->size[0] * tauGrid[22].f1->size[1];
  tauGrid[22].f1->size[0] = 1;
  tauGrid[22].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[22].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[22].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 23];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 23];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[23].f1->size[0] * alphaSampled[23].f1->size[1];
    alphaSampled[23].f1->size[0] = 1;
    alphaSampled[23].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[23].f1, k);
    alphaSampled[23].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[23].f1->size[0] = 1;
    alphaSampled[23].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[23].f1->size[0] * alphaSampled[23].f1->size[1];
    alphaSampled[23].f1->size[0] = 1;
    alphaSampled[23].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[23].f1, k);
    alphaSampled[23].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[23].f1->size[0] * alphaSampled[23].f1->size[1];
    alphaSampled[23].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[23].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[23].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[23].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[23].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[23].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[23].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[23].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[23].f1->size[0] * tauGrid[23].f1->size[1];
  tauGrid[23].f1->size[0] = 1;
  tauGrid[23].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[23].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[23].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 24];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 24];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[24].f1->size[0] * alphaSampled[24].f1->size[1];
    alphaSampled[24].f1->size[0] = 1;
    alphaSampled[24].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[24].f1, k);
    alphaSampled[24].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[24].f1->size[0] = 1;
    alphaSampled[24].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[24].f1->size[0] * alphaSampled[24].f1->size[1];
    alphaSampled[24].f1->size[0] = 1;
    alphaSampled[24].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[24].f1, k);
    alphaSampled[24].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[24].f1->size[0] * alphaSampled[24].f1->size[1];
    alphaSampled[24].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[24].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[24].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[24].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[24].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[24].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[24].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[24].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[24].f1->size[0] * tauGrid[24].f1->size[1];
  tauGrid[24].f1->size[0] = 1;
  tauGrid[24].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[24].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[24].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 25];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 25];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[25].f1->size[0] * alphaSampled[25].f1->size[1];
    alphaSampled[25].f1->size[0] = 1;
    alphaSampled[25].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[25].f1, k);
    alphaSampled[25].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[25].f1->size[0] = 1;
    alphaSampled[25].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[25].f1->size[0] * alphaSampled[25].f1->size[1];
    alphaSampled[25].f1->size[0] = 1;
    alphaSampled[25].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[25].f1, k);
    alphaSampled[25].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[25].f1->size[0] * alphaSampled[25].f1->size[1];
    alphaSampled[25].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[25].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[25].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[25].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[25].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[25].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[25].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[25].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[25].f1->size[0] * tauGrid[25].f1->size[1];
  tauGrid[25].f1->size[0] = 1;
  tauGrid[25].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[25].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[25].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 26];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 26];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[26].f1->size[0] * alphaSampled[26].f1->size[1];
    alphaSampled[26].f1->size[0] = 1;
    alphaSampled[26].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[26].f1, k);
    alphaSampled[26].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[26].f1->size[0] = 1;
    alphaSampled[26].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[26].f1->size[0] * alphaSampled[26].f1->size[1];
    alphaSampled[26].f1->size[0] = 1;
    alphaSampled[26].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[26].f1, k);
    alphaSampled[26].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[26].f1->size[0] * alphaSampled[26].f1->size[1];
    alphaSampled[26].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[26].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[26].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[26].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[26].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[26].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[26].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[26].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[26].f1->size[0] * tauGrid[26].f1->size[1];
  tauGrid[26].f1->size[0] = 1;
  tauGrid[26].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[26].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[26].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 27];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 27];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[27].f1->size[0] * alphaSampled[27].f1->size[1];
    alphaSampled[27].f1->size[0] = 1;
    alphaSampled[27].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[27].f1, k);
    alphaSampled[27].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[27].f1->size[0] = 1;
    alphaSampled[27].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[27].f1->size[0] * alphaSampled[27].f1->size[1];
    alphaSampled[27].f1->size[0] = 1;
    alphaSampled[27].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[27].f1, k);
    alphaSampled[27].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[27].f1->size[0] * alphaSampled[27].f1->size[1];
    alphaSampled[27].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[27].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[27].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[27].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[27].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[27].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[27].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[27].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[27].f1->size[0] * tauGrid[27].f1->size[1];
  tauGrid[27].f1->size[0] = 1;
  tauGrid[27].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[27].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[27].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 28];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 28];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[28].f1->size[0] * alphaSampled[28].f1->size[1];
    alphaSampled[28].f1->size[0] = 1;
    alphaSampled[28].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[28].f1, k);
    alphaSampled[28].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[28].f1->size[0] = 1;
    alphaSampled[28].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[28].f1->size[0] * alphaSampled[28].f1->size[1];
    alphaSampled[28].f1->size[0] = 1;
    alphaSampled[28].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[28].f1, k);
    alphaSampled[28].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[28].f1->size[0] * alphaSampled[28].f1->size[1];
    alphaSampled[28].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[28].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[28].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[28].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[28].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[28].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[28].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[28].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[28].f1->size[0] * tauGrid[28].f1->size[1];
  tauGrid[28].f1->size[0] = 1;
  tauGrid[28].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[28].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[28].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 29];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 29];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[29].f1->size[0] * alphaSampled[29].f1->size[1];
    alphaSampled[29].f1->size[0] = 1;
    alphaSampled[29].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[29].f1, k);
    alphaSampled[29].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[29].f1->size[0] = 1;
    alphaSampled[29].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[29].f1->size[0] * alphaSampled[29].f1->size[1];
    alphaSampled[29].f1->size[0] = 1;
    alphaSampled[29].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[29].f1, k);
    alphaSampled[29].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[29].f1->size[0] * alphaSampled[29].f1->size[1];
    alphaSampled[29].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[29].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[29].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[29].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[29].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[29].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[29].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[29].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[29].f1->size[0] * tauGrid[29].f1->size[1];
  tauGrid[29].f1->size[0] = 1;
  tauGrid[29].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[29].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[29].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 30];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 30];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[30].f1->size[0] * alphaSampled[30].f1->size[1];
    alphaSampled[30].f1->size[0] = 1;
    alphaSampled[30].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[30].f1, k);
    alphaSampled[30].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[30].f1->size[0] = 1;
    alphaSampled[30].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[30].f1->size[0] * alphaSampled[30].f1->size[1];
    alphaSampled[30].f1->size[0] = 1;
    alphaSampled[30].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[30].f1, k);
    alphaSampled[30].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[30].f1->size[0] * alphaSampled[30].f1->size[1];
    alphaSampled[30].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[30].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[30].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[30].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[30].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[30].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[30].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[30].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[30].f1->size[0] * tauGrid[30].f1->size[1];
  tauGrid[30].f1->size[0] = 1;
  tauGrid[30].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[30].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[30].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 31];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 31];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[31].f1->size[0] * alphaSampled[31].f1->size[1];
    alphaSampled[31].f1->size[0] = 1;
    alphaSampled[31].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[31].f1, k);
    alphaSampled[31].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[31].f1->size[0] = 1;
    alphaSampled[31].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[31].f1->size[0] * alphaSampled[31].f1->size[1];
    alphaSampled[31].f1->size[0] = 1;
    alphaSampled[31].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[31].f1, k);
    alphaSampled[31].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[31].f1->size[0] * alphaSampled[31].f1->size[1];
    alphaSampled[31].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[31].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[31].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[31].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[31].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[31].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[31].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[31].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[31].f1->size[0] * tauGrid[31].f1->size[1];
  tauGrid[31].f1->size[0] = 1;
  tauGrid[31].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[31].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[31].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 32];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 32];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[32].f1->size[0] * alphaSampled[32].f1->size[1];
    alphaSampled[32].f1->size[0] = 1;
    alphaSampled[32].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[32].f1, k);
    alphaSampled[32].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[32].f1->size[0] = 1;
    alphaSampled[32].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[32].f1->size[0] * alphaSampled[32].f1->size[1];
    alphaSampled[32].f1->size[0] = 1;
    alphaSampled[32].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[32].f1, k);
    alphaSampled[32].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[32].f1->size[0] * alphaSampled[32].f1->size[1];
    alphaSampled[32].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[32].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[32].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[32].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[32].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[32].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[32].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[32].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[32].f1->size[0] * tauGrid[32].f1->size[1];
  tauGrid[32].f1->size[0] = 1;
  tauGrid[32].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[32].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[32].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 33];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 33];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[33].f1->size[0] * alphaSampled[33].f1->size[1];
    alphaSampled[33].f1->size[0] = 1;
    alphaSampled[33].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[33].f1, k);
    alphaSampled[33].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[33].f1->size[0] = 1;
    alphaSampled[33].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[33].f1->size[0] * alphaSampled[33].f1->size[1];
    alphaSampled[33].f1->size[0] = 1;
    alphaSampled[33].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[33].f1, k);
    alphaSampled[33].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[33].f1->size[0] * alphaSampled[33].f1->size[1];
    alphaSampled[33].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[33].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[33].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[33].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[33].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[33].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[33].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[33].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[33].f1->size[0] * tauGrid[33].f1->size[1];
  tauGrid[33].f1->size[0] = 1;
  tauGrid[33].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[33].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[33].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 34];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 34];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[34].f1->size[0] * alphaSampled[34].f1->size[1];
    alphaSampled[34].f1->size[0] = 1;
    alphaSampled[34].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[34].f1, k);
    alphaSampled[34].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[34].f1->size[0] = 1;
    alphaSampled[34].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[34].f1->size[0] * alphaSampled[34].f1->size[1];
    alphaSampled[34].f1->size[0] = 1;
    alphaSampled[34].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[34].f1, k);
    alphaSampled[34].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[34].f1->size[0] * alphaSampled[34].f1->size[1];
    alphaSampled[34].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[34].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[34].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[34].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[34].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[34].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[34].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[34].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[34].f1->size[0] * tauGrid[34].f1->size[1];
  tauGrid[34].f1->size[0] = 1;
  tauGrid[34].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[34].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[34].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 35];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 35];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[35].f1->size[0] * alphaSampled[35].f1->size[1];
    alphaSampled[35].f1->size[0] = 1;
    alphaSampled[35].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[35].f1, k);
    alphaSampled[35].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[35].f1->size[0] = 1;
    alphaSampled[35].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[35].f1->size[0] * alphaSampled[35].f1->size[1];
    alphaSampled[35].f1->size[0] = 1;
    alphaSampled[35].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[35].f1, k);
    alphaSampled[35].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[35].f1->size[0] * alphaSampled[35].f1->size[1];
    alphaSampled[35].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[35].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[35].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[35].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[35].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[35].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[35].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[35].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[35].f1->size[0] * tauGrid[35].f1->size[1];
  tauGrid[35].f1->size[0] = 1;
  tauGrid[35].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[35].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[35].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 36];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 36];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[36].f1->size[0] * alphaSampled[36].f1->size[1];
    alphaSampled[36].f1->size[0] = 1;
    alphaSampled[36].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[36].f1, k);
    alphaSampled[36].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[36].f1->size[0] = 1;
    alphaSampled[36].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[36].f1->size[0] * alphaSampled[36].f1->size[1];
    alphaSampled[36].f1->size[0] = 1;
    alphaSampled[36].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[36].f1, k);
    alphaSampled[36].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[36].f1->size[0] * alphaSampled[36].f1->size[1];
    alphaSampled[36].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[36].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[36].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[36].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[36].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[36].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[36].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[36].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[36].f1->size[0] * tauGrid[36].f1->size[1];
  tauGrid[36].f1->size[0] = 1;
  tauGrid[36].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[36].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[36].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 37];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 37];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[37].f1->size[0] * alphaSampled[37].f1->size[1];
    alphaSampled[37].f1->size[0] = 1;
    alphaSampled[37].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[37].f1, k);
    alphaSampled[37].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[37].f1->size[0] = 1;
    alphaSampled[37].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[37].f1->size[0] * alphaSampled[37].f1->size[1];
    alphaSampled[37].f1->size[0] = 1;
    alphaSampled[37].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[37].f1, k);
    alphaSampled[37].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[37].f1->size[0] * alphaSampled[37].f1->size[1];
    alphaSampled[37].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[37].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[37].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[37].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[37].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[37].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[37].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[37].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[37].f1->size[0] * tauGrid[37].f1->size[1];
  tauGrid[37].f1->size[0] = 1;
  tauGrid[37].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[37].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[37].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 38];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 38];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[38].f1->size[0] * alphaSampled[38].f1->size[1];
    alphaSampled[38].f1->size[0] = 1;
    alphaSampled[38].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[38].f1, k);
    alphaSampled[38].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[38].f1->size[0] = 1;
    alphaSampled[38].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[38].f1->size[0] * alphaSampled[38].f1->size[1];
    alphaSampled[38].f1->size[0] = 1;
    alphaSampled[38].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[38].f1, k);
    alphaSampled[38].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[38].f1->size[0] * alphaSampled[38].f1->size[1];
    alphaSampled[38].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[38].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[38].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[38].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[38].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[38].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[38].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[38].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[38].f1->size[0] * tauGrid[38].f1->size[1];
  tauGrid[38].f1->size[0] = 1;
  tauGrid[38].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[38].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[38].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 39];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 39];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[39].f1->size[0] * alphaSampled[39].f1->size[1];
    alphaSampled[39].f1->size[0] = 1;
    alphaSampled[39].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[39].f1, k);
    alphaSampled[39].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[39].f1->size[0] = 1;
    alphaSampled[39].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[39].f1->size[0] * alphaSampled[39].f1->size[1];
    alphaSampled[39].f1->size[0] = 1;
    alphaSampled[39].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[39].f1, k);
    alphaSampled[39].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[39].f1->size[0] * alphaSampled[39].f1->size[1];
    alphaSampled[39].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[39].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[39].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[39].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[39].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[39].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[39].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[39].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[39].f1->size[0] * tauGrid[39].f1->size[1];
  tauGrid[39].f1->size[0] = 1;
  tauGrid[39].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[39].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[39].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 40];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 40];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[40].f1->size[0] * alphaSampled[40].f1->size[1];
    alphaSampled[40].f1->size[0] = 1;
    alphaSampled[40].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[40].f1, k);
    alphaSampled[40].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[40].f1->size[0] = 1;
    alphaSampled[40].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[40].f1->size[0] * alphaSampled[40].f1->size[1];
    alphaSampled[40].f1->size[0] = 1;
    alphaSampled[40].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[40].f1, k);
    alphaSampled[40].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[40].f1->size[0] * alphaSampled[40].f1->size[1];
    alphaSampled[40].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[40].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[40].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[40].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[40].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[40].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[40].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[40].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[40].f1->size[0] * tauGrid[40].f1->size[1];
  tauGrid[40].f1->size[0] = 1;
  tauGrid[40].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[40].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[40].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 41];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 41];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[41].f1->size[0] * alphaSampled[41].f1->size[1];
    alphaSampled[41].f1->size[0] = 1;
    alphaSampled[41].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[41].f1, k);
    alphaSampled[41].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[41].f1->size[0] = 1;
    alphaSampled[41].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[41].f1->size[0] * alphaSampled[41].f1->size[1];
    alphaSampled[41].f1->size[0] = 1;
    alphaSampled[41].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[41].f1, k);
    alphaSampled[41].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[41].f1->size[0] * alphaSampled[41].f1->size[1];
    alphaSampled[41].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[41].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[41].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[41].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[41].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[41].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[41].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[41].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[41].f1->size[0] * tauGrid[41].f1->size[1];
  tauGrid[41].f1->size[0] = 1;
  tauGrid[41].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[41].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[41].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 42];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 42];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[42].f1->size[0] * alphaSampled[42].f1->size[1];
    alphaSampled[42].f1->size[0] = 1;
    alphaSampled[42].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[42].f1, k);
    alphaSampled[42].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[42].f1->size[0] = 1;
    alphaSampled[42].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[42].f1->size[0] * alphaSampled[42].f1->size[1];
    alphaSampled[42].f1->size[0] = 1;
    alphaSampled[42].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[42].f1, k);
    alphaSampled[42].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[42].f1->size[0] * alphaSampled[42].f1->size[1];
    alphaSampled[42].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[42].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[42].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[42].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[42].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[42].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[42].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[42].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[42].f1->size[0] * tauGrid[42].f1->size[1];
  tauGrid[42].f1->size[0] = 1;
  tauGrid[42].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[42].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[42].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 43];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 43];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[43].f1->size[0] * alphaSampled[43].f1->size[1];
    alphaSampled[43].f1->size[0] = 1;
    alphaSampled[43].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[43].f1, k);
    alphaSampled[43].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[43].f1->size[0] = 1;
    alphaSampled[43].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[43].f1->size[0] * alphaSampled[43].f1->size[1];
    alphaSampled[43].f1->size[0] = 1;
    alphaSampled[43].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[43].f1, k);
    alphaSampled[43].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[43].f1->size[0] * alphaSampled[43].f1->size[1];
    alphaSampled[43].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[43].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[43].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[43].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[43].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[43].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[43].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[43].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[43].f1->size[0] * tauGrid[43].f1->size[1];
  tauGrid[43].f1->size[0] = 1;
  tauGrid[43].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[43].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[43].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 44];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 44];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[44].f1->size[0] * alphaSampled[44].f1->size[1];
    alphaSampled[44].f1->size[0] = 1;
    alphaSampled[44].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[44].f1, k);
    alphaSampled[44].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[44].f1->size[0] = 1;
    alphaSampled[44].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[44].f1->size[0] * alphaSampled[44].f1->size[1];
    alphaSampled[44].f1->size[0] = 1;
    alphaSampled[44].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[44].f1, k);
    alphaSampled[44].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[44].f1->size[0] * alphaSampled[44].f1->size[1];
    alphaSampled[44].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[44].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[44].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[44].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[44].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[44].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[44].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[44].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[44].f1->size[0] * tauGrid[44].f1->size[1];
  tauGrid[44].f1->size[0] = 1;
  tauGrid[44].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[44].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[44].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 45];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 45];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[45].f1->size[0] * alphaSampled[45].f1->size[1];
    alphaSampled[45].f1->size[0] = 1;
    alphaSampled[45].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[45].f1, k);
    alphaSampled[45].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[45].f1->size[0] = 1;
    alphaSampled[45].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[45].f1->size[0] * alphaSampled[45].f1->size[1];
    alphaSampled[45].f1->size[0] = 1;
    alphaSampled[45].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[45].f1, k);
    alphaSampled[45].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[45].f1->size[0] * alphaSampled[45].f1->size[1];
    alphaSampled[45].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[45].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[45].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[45].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[45].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[45].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[45].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[45].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[45].f1->size[0] * tauGrid[45].f1->size[1];
  tauGrid[45].f1->size[0] = 1;
  tauGrid[45].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[45].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[45].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 46];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 46];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[46].f1->size[0] * alphaSampled[46].f1->size[1];
    alphaSampled[46].f1->size[0] = 1;
    alphaSampled[46].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[46].f1, k);
    alphaSampled[46].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[46].f1->size[0] = 1;
    alphaSampled[46].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[46].f1->size[0] * alphaSampled[46].f1->size[1];
    alphaSampled[46].f1->size[0] = 1;
    alphaSampled[46].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[46].f1, k);
    alphaSampled[46].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[46].f1->size[0] * alphaSampled[46].f1->size[1];
    alphaSampled[46].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[46].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[46].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[46].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[46].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[46].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[46].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[46].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[46].f1->size[0] * tauGrid[46].f1->size[1];
  tauGrid[46].f1->size[0] = 1;
  tauGrid[46].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[46].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[46].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 47];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 47];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[47].f1->size[0] * alphaSampled[47].f1->size[1];
    alphaSampled[47].f1->size[0] = 1;
    alphaSampled[47].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[47].f1, k);
    alphaSampled[47].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[47].f1->size[0] = 1;
    alphaSampled[47].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[47].f1->size[0] * alphaSampled[47].f1->size[1];
    alphaSampled[47].f1->size[0] = 1;
    alphaSampled[47].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[47].f1, k);
    alphaSampled[47].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[47].f1->size[0] * alphaSampled[47].f1->size[1];
    alphaSampled[47].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[47].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[47].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[47].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[47].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[47].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[47].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[47].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[47].f1->size[0] * tauGrid[47].f1->size[1];
  tauGrid[47].f1->size[0] = 1;
  tauGrid[47].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[47].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[47].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 48];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 48];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[48].f1->size[0] * alphaSampled[48].f1->size[1];
    alphaSampled[48].f1->size[0] = 1;
    alphaSampled[48].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[48].f1, k);
    alphaSampled[48].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[48].f1->size[0] = 1;
    alphaSampled[48].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[48].f1->size[0] * alphaSampled[48].f1->size[1];
    alphaSampled[48].f1->size[0] = 1;
    alphaSampled[48].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[48].f1, k);
    alphaSampled[48].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[48].f1->size[0] * alphaSampled[48].f1->size[1];
    alphaSampled[48].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[48].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[48].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[48].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[48].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[48].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[48].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[48].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[48].f1->size[0] * tauGrid[48].f1->size[1];
  tauGrid[48].f1->size[0] = 1;
  tauGrid[48].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[48].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[48].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 49];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 49];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[49].f1->size[0] * alphaSampled[49].f1->size[1];
    alphaSampled[49].f1->size[0] = 1;
    alphaSampled[49].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[49].f1, k);
    alphaSampled[49].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[49].f1->size[0] = 1;
    alphaSampled[49].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[49].f1->size[0] * alphaSampled[49].f1->size[1];
    alphaSampled[49].f1->size[0] = 1;
    alphaSampled[49].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[49].f1, k);
    alphaSampled[49].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[49].f1->size[0] * alphaSampled[49].f1->size[1];
    alphaSampled[49].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[49].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[49].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[49].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[49].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[49].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[49].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[49].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[49].f1->size[0] * tauGrid[49].f1->size[1];
  tauGrid[49].f1->size[0] = 1;
  tauGrid[49].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[49].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[49].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 50];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 50];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[50].f1->size[0] * alphaSampled[50].f1->size[1];
    alphaSampled[50].f1->size[0] = 1;
    alphaSampled[50].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[50].f1, k);
    alphaSampled[50].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[50].f1->size[0] = 1;
    alphaSampled[50].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[50].f1->size[0] * alphaSampled[50].f1->size[1];
    alphaSampled[50].f1->size[0] = 1;
    alphaSampled[50].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[50].f1, k);
    alphaSampled[50].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[50].f1->size[0] * alphaSampled[50].f1->size[1];
    alphaSampled[50].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[50].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[50].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[50].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[50].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[50].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[50].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[50].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[50].f1->size[0] * tauGrid[50].f1->size[1];
  tauGrid[50].f1->size[0] = 1;
  tauGrid[50].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[50].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[50].f1->data[k] = 0.10182337649086284 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 51];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 51];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[51].f1->size[0] * alphaSampled[51].f1->size[1];
    alphaSampled[51].f1->size[0] = 1;
    alphaSampled[51].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[51].f1, k);
    alphaSampled[51].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[51].f1->size[0] = 1;
    alphaSampled[51].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[51].f1->size[0] * alphaSampled[51].f1->size[1];
    alphaSampled[51].f1->size[0] = 1;
    alphaSampled[51].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[51].f1, k);
    alphaSampled[51].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[51].f1->size[0] * alphaSampled[51].f1->size[1];
    alphaSampled[51].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[51].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[51].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[51].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[51].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[51].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[51].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[51].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[51].f1->size[0] * tauGrid[51].f1->size[1];
  tauGrid[51].f1->size[0] = 1;
  tauGrid[51].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[51].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[51].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 52];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 52];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[52].f1->size[0] * alphaSampled[52].f1->size[1];
    alphaSampled[52].f1->size[0] = 1;
    alphaSampled[52].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[52].f1, k);
    alphaSampled[52].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[52].f1->size[0] = 1;
    alphaSampled[52].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[52].f1->size[0] * alphaSampled[52].f1->size[1];
    alphaSampled[52].f1->size[0] = 1;
    alphaSampled[52].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[52].f1, k);
    alphaSampled[52].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[52].f1->size[0] * alphaSampled[52].f1->size[1];
    alphaSampled[52].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[52].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[52].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[52].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[52].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[52].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[52].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[52].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[52].f1->size[0] * tauGrid[52].f1->size[1];
  tauGrid[52].f1->size[0] = 1;
  tauGrid[52].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[52].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[52].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 53];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 53];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[53].f1->size[0] * alphaSampled[53].f1->size[1];
    alphaSampled[53].f1->size[0] = 1;
    alphaSampled[53].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[53].f1, k);
    alphaSampled[53].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[53].f1->size[0] = 1;
    alphaSampled[53].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[53].f1->size[0] * alphaSampled[53].f1->size[1];
    alphaSampled[53].f1->size[0] = 1;
    alphaSampled[53].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[53].f1, k);
    alphaSampled[53].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[53].f1->size[0] * alphaSampled[53].f1->size[1];
    alphaSampled[53].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[53].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[53].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[53].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[53].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[53].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[53].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[53].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[53].f1->size[0] * tauGrid[53].f1->size[1];
  tauGrid[53].f1->size[0] = 1;
  tauGrid[53].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[53].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[53].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 54];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 54];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[54].f1->size[0] * alphaSampled[54].f1->size[1];
    alphaSampled[54].f1->size[0] = 1;
    alphaSampled[54].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[54].f1, k);
    alphaSampled[54].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[54].f1->size[0] = 1;
    alphaSampled[54].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[54].f1->size[0] * alphaSampled[54].f1->size[1];
    alphaSampled[54].f1->size[0] = 1;
    alphaSampled[54].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[54].f1, k);
    alphaSampled[54].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[54].f1->size[0] * alphaSampled[54].f1->size[1];
    alphaSampled[54].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[54].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[54].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[54].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[54].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[54].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[54].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[54].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[54].f1->size[0] * tauGrid[54].f1->size[1];
  tauGrid[54].f1->size[0] = 1;
  tauGrid[54].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[54].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[54].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 55];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 55];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[55].f1->size[0] * alphaSampled[55].f1->size[1];
    alphaSampled[55].f1->size[0] = 1;
    alphaSampled[55].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[55].f1, k);
    alphaSampled[55].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[55].f1->size[0] = 1;
    alphaSampled[55].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[55].f1->size[0] * alphaSampled[55].f1->size[1];
    alphaSampled[55].f1->size[0] = 1;
    alphaSampled[55].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[55].f1, k);
    alphaSampled[55].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[55].f1->size[0] * alphaSampled[55].f1->size[1];
    alphaSampled[55].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[55].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[55].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[55].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[55].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[55].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[55].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[55].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[55].f1->size[0] * tauGrid[55].f1->size[1];
  tauGrid[55].f1->size[0] = 1;
  tauGrid[55].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[55].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[55].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 56];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 56];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[56].f1->size[0] * alphaSampled[56].f1->size[1];
    alphaSampled[56].f1->size[0] = 1;
    alphaSampled[56].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[56].f1, k);
    alphaSampled[56].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[56].f1->size[0] = 1;
    alphaSampled[56].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[56].f1->size[0] * alphaSampled[56].f1->size[1];
    alphaSampled[56].f1->size[0] = 1;
    alphaSampled[56].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[56].f1, k);
    alphaSampled[56].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[56].f1->size[0] * alphaSampled[56].f1->size[1];
    alphaSampled[56].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[56].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[56].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[56].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[56].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[56].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[56].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[56].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[56].f1->size[0] * tauGrid[56].f1->size[1];
  tauGrid[56].f1->size[0] = 1;
  tauGrid[56].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[56].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[56].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 57];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 57];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[57].f1->size[0] * alphaSampled[57].f1->size[1];
    alphaSampled[57].f1->size[0] = 1;
    alphaSampled[57].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[57].f1, k);
    alphaSampled[57].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[57].f1->size[0] = 1;
    alphaSampled[57].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[57].f1->size[0] * alphaSampled[57].f1->size[1];
    alphaSampled[57].f1->size[0] = 1;
    alphaSampled[57].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[57].f1, k);
    alphaSampled[57].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[57].f1->size[0] * alphaSampled[57].f1->size[1];
    alphaSampled[57].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[57].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[57].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[57].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[57].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[57].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[57].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[57].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[57].f1->size[0] * tauGrid[57].f1->size[1];
  tauGrid[57].f1->size[0] = 1;
  tauGrid[57].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[57].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[57].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 58];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 58];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[58].f1->size[0] * alphaSampled[58].f1->size[1];
    alphaSampled[58].f1->size[0] = 1;
    alphaSampled[58].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[58].f1, k);
    alphaSampled[58].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[58].f1->size[0] = 1;
    alphaSampled[58].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[58].f1->size[0] * alphaSampled[58].f1->size[1];
    alphaSampled[58].f1->size[0] = 1;
    alphaSampled[58].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[58].f1, k);
    alphaSampled[58].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[58].f1->size[0] * alphaSampled[58].f1->size[1];
    alphaSampled[58].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[58].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[58].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[58].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[58].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[58].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[58].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[58].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[58].f1->size[0] * tauGrid[58].f1->size[1];
  tauGrid[58].f1->size[0] = 1;
  tauGrid[58].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[58].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[58].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 59];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 59];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[59].f1->size[0] * alphaSampled[59].f1->size[1];
    alphaSampled[59].f1->size[0] = 1;
    alphaSampled[59].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[59].f1, k);
    alphaSampled[59].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[59].f1->size[0] = 1;
    alphaSampled[59].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[59].f1->size[0] * alphaSampled[59].f1->size[1];
    alphaSampled[59].f1->size[0] = 1;
    alphaSampled[59].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[59].f1, k);
    alphaSampled[59].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[59].f1->size[0] * alphaSampled[59].f1->size[1];
    alphaSampled[59].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[59].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[59].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[59].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[59].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[59].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[59].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[59].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[59].f1->size[0] * tauGrid[59].f1->size[1];
  tauGrid[59].f1->size[0] = 1;
  tauGrid[59].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[59].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[59].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 60];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 60];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[60].f1->size[0] * alphaSampled[60].f1->size[1];
    alphaSampled[60].f1->size[0] = 1;
    alphaSampled[60].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[60].f1, k);
    alphaSampled[60].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[60].f1->size[0] = 1;
    alphaSampled[60].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[60].f1->size[0] * alphaSampled[60].f1->size[1];
    alphaSampled[60].f1->size[0] = 1;
    alphaSampled[60].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[60].f1, k);
    alphaSampled[60].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[60].f1->size[0] * alphaSampled[60].f1->size[1];
    alphaSampled[60].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[60].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[60].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[60].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[60].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[60].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[60].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[60].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[60].f1->size[0] * tauGrid[60].f1->size[1];
  tauGrid[60].f1->size[0] = 1;
  tauGrid[60].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[60].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[60].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 61];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 61];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[61].f1->size[0] * alphaSampled[61].f1->size[1];
    alphaSampled[61].f1->size[0] = 1;
    alphaSampled[61].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[61].f1, k);
    alphaSampled[61].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[61].f1->size[0] = 1;
    alphaSampled[61].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[61].f1->size[0] * alphaSampled[61].f1->size[1];
    alphaSampled[61].f1->size[0] = 1;
    alphaSampled[61].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[61].f1, k);
    alphaSampled[61].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[61].f1->size[0] * alphaSampled[61].f1->size[1];
    alphaSampled[61].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[61].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[61].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[61].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[61].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[61].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[61].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[61].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[61].f1->size[0] * tauGrid[61].f1->size[1];
  tauGrid[61].f1->size[0] = 1;
  tauGrid[61].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[61].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[61].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 62];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 62];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[62].f1->size[0] * alphaSampled[62].f1->size[1];
    alphaSampled[62].f1->size[0] = 1;
    alphaSampled[62].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[62].f1, k);
    alphaSampled[62].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[62].f1->size[0] = 1;
    alphaSampled[62].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[62].f1->size[0] * alphaSampled[62].f1->size[1];
    alphaSampled[62].f1->size[0] = 1;
    alphaSampled[62].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[62].f1, k);
    alphaSampled[62].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[62].f1->size[0] * alphaSampled[62].f1->size[1];
    alphaSampled[62].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[62].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[62].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[62].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[62].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[62].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[62].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[62].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[62].f1->size[0] * tauGrid[62].f1->size[1];
  tauGrid[62].f1->size[0] = 1;
  tauGrid[62].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[62].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[62].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 63];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 63];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[63].f1->size[0] * alphaSampled[63].f1->size[1];
    alphaSampled[63].f1->size[0] = 1;
    alphaSampled[63].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[63].f1, k);
    alphaSampled[63].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[63].f1->size[0] = 1;
    alphaSampled[63].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[63].f1->size[0] * alphaSampled[63].f1->size[1];
    alphaSampled[63].f1->size[0] = 1;
    alphaSampled[63].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[63].f1, k);
    alphaSampled[63].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[63].f1->size[0] * alphaSampled[63].f1->size[1];
    alphaSampled[63].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[63].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[63].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[63].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[63].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[63].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[63].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[63].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[63].f1->size[0] * tauGrid[63].f1->size[1];
  tauGrid[63].f1->size[0] = 1;
  tauGrid[63].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[63].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[63].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 64];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 64];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[64].f1->size[0] * alphaSampled[64].f1->size[1];
    alphaSampled[64].f1->size[0] = 1;
    alphaSampled[64].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[64].f1, k);
    alphaSampled[64].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[64].f1->size[0] = 1;
    alphaSampled[64].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[64].f1->size[0] * alphaSampled[64].f1->size[1];
    alphaSampled[64].f1->size[0] = 1;
    alphaSampled[64].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[64].f1, k);
    alphaSampled[64].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[64].f1->size[0] * alphaSampled[64].f1->size[1];
    alphaSampled[64].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[64].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[64].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[64].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[64].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[64].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[64].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[64].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[64].f1->size[0] * tauGrid[64].f1->size[1];
  tauGrid[64].f1->size[0] = 1;
  tauGrid[64].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[64].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[64].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 65];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 65];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[65].f1->size[0] * alphaSampled[65].f1->size[1];
    alphaSampled[65].f1->size[0] = 1;
    alphaSampled[65].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[65].f1, k);
    alphaSampled[65].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[65].f1->size[0] = 1;
    alphaSampled[65].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[65].f1->size[0] * alphaSampled[65].f1->size[1];
    alphaSampled[65].f1->size[0] = 1;
    alphaSampled[65].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[65].f1, k);
    alphaSampled[65].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[65].f1->size[0] * alphaSampled[65].f1->size[1];
    alphaSampled[65].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[65].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[65].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[65].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[65].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[65].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[65].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[65].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[65].f1->size[0] * tauGrid[65].f1->size[1];
  tauGrid[65].f1->size[0] = 1;
  tauGrid[65].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[65].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[65].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 66];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 66];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[66].f1->size[0] * alphaSampled[66].f1->size[1];
    alphaSampled[66].f1->size[0] = 1;
    alphaSampled[66].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[66].f1, k);
    alphaSampled[66].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[66].f1->size[0] = 1;
    alphaSampled[66].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[66].f1->size[0] * alphaSampled[66].f1->size[1];
    alphaSampled[66].f1->size[0] = 1;
    alphaSampled[66].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[66].f1, k);
    alphaSampled[66].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[66].f1->size[0] * alphaSampled[66].f1->size[1];
    alphaSampled[66].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[66].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[66].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[66].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[66].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[66].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[66].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[66].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[66].f1->size[0] * tauGrid[66].f1->size[1];
  tauGrid[66].f1->size[0] = 1;
  tauGrid[66].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[66].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[66].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 67];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 67];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[67].f1->size[0] * alphaSampled[67].f1->size[1];
    alphaSampled[67].f1->size[0] = 1;
    alphaSampled[67].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[67].f1, k);
    alphaSampled[67].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[67].f1->size[0] = 1;
    alphaSampled[67].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[67].f1->size[0] * alphaSampled[67].f1->size[1];
    alphaSampled[67].f1->size[0] = 1;
    alphaSampled[67].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[67].f1, k);
    alphaSampled[67].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[67].f1->size[0] * alphaSampled[67].f1->size[1];
    alphaSampled[67].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[67].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[67].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[67].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[67].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[67].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[67].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[67].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[67].f1->size[0] * tauGrid[67].f1->size[1];
  tauGrid[67].f1->size[0] = 1;
  tauGrid[67].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[67].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[67].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 68];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 68];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[68].f1->size[0] * alphaSampled[68].f1->size[1];
    alphaSampled[68].f1->size[0] = 1;
    alphaSampled[68].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[68].f1, k);
    alphaSampled[68].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[68].f1->size[0] = 1;
    alphaSampled[68].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[68].f1->size[0] * alphaSampled[68].f1->size[1];
    alphaSampled[68].f1->size[0] = 1;
    alphaSampled[68].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[68].f1, k);
    alphaSampled[68].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[68].f1->size[0] * alphaSampled[68].f1->size[1];
    alphaSampled[68].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[68].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[68].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[68].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[68].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[68].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[68].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[68].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[68].f1->size[0] * tauGrid[68].f1->size[1];
  tauGrid[68].f1->size[0] = 1;
  tauGrid[68].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[68].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[68].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 69];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 69];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[69].f1->size[0] * alphaSampled[69].f1->size[1];
    alphaSampled[69].f1->size[0] = 1;
    alphaSampled[69].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[69].f1, k);
    alphaSampled[69].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[69].f1->size[0] = 1;
    alphaSampled[69].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[69].f1->size[0] * alphaSampled[69].f1->size[1];
    alphaSampled[69].f1->size[0] = 1;
    alphaSampled[69].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[69].f1, k);
    alphaSampled[69].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[69].f1->size[0] * alphaSampled[69].f1->size[1];
    alphaSampled[69].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[69].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[69].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[69].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[69].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[69].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[69].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[69].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[69].f1->size[0] * tauGrid[69].f1->size[1];
  tauGrid[69].f1->size[0] = 1;
  tauGrid[69].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[69].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[69].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 70];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 70];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[70].f1->size[0] * alphaSampled[70].f1->size[1];
    alphaSampled[70].f1->size[0] = 1;
    alphaSampled[70].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[70].f1, k);
    alphaSampled[70].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[70].f1->size[0] = 1;
    alphaSampled[70].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[70].f1->size[0] * alphaSampled[70].f1->size[1];
    alphaSampled[70].f1->size[0] = 1;
    alphaSampled[70].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[70].f1, k);
    alphaSampled[70].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[70].f1->size[0] * alphaSampled[70].f1->size[1];
    alphaSampled[70].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[70].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[70].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[70].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[70].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[70].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[70].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[70].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[70].f1->size[0] * tauGrid[70].f1->size[1];
  tauGrid[70].f1->size[0] = 1;
  tauGrid[70].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[70].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[70].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 71];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 71];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[71].f1->size[0] * alphaSampled[71].f1->size[1];
    alphaSampled[71].f1->size[0] = 1;
    alphaSampled[71].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[71].f1, k);
    alphaSampled[71].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[71].f1->size[0] = 1;
    alphaSampled[71].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[71].f1->size[0] * alphaSampled[71].f1->size[1];
    alphaSampled[71].f1->size[0] = 1;
    alphaSampled[71].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[71].f1, k);
    alphaSampled[71].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[71].f1->size[0] * alphaSampled[71].f1->size[1];
    alphaSampled[71].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[71].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[71].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[71].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[71].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[71].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[71].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[71].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[71].f1->size[0] * tauGrid[71].f1->size[1];
  tauGrid[71].f1->size[0] = 1;
  tauGrid[71].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[71].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[71].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 72];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 72];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[72].f1->size[0] * alphaSampled[72].f1->size[1];
    alphaSampled[72].f1->size[0] = 1;
    alphaSampled[72].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[72].f1, k);
    alphaSampled[72].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[72].f1->size[0] = 1;
    alphaSampled[72].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[72].f1->size[0] * alphaSampled[72].f1->size[1];
    alphaSampled[72].f1->size[0] = 1;
    alphaSampled[72].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[72].f1, k);
    alphaSampled[72].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[72].f1->size[0] * alphaSampled[72].f1->size[1];
    alphaSampled[72].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[72].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[72].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[72].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[72].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[72].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[72].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[72].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[72].f1->size[0] * tauGrid[72].f1->size[1];
  tauGrid[72].f1->size[0] = 1;
  tauGrid[72].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[72].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[72].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 73];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 73];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[73].f1->size[0] * alphaSampled[73].f1->size[1];
    alphaSampled[73].f1->size[0] = 1;
    alphaSampled[73].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[73].f1, k);
    alphaSampled[73].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[73].f1->size[0] = 1;
    alphaSampled[73].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[73].f1->size[0] * alphaSampled[73].f1->size[1];
    alphaSampled[73].f1->size[0] = 1;
    alphaSampled[73].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[73].f1, k);
    alphaSampled[73].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[73].f1->size[0] * alphaSampled[73].f1->size[1];
    alphaSampled[73].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[73].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[73].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[73].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[73].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[73].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[73].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[73].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[73].f1->size[0] * tauGrid[73].f1->size[1];
  tauGrid[73].f1->size[0] = 1;
  tauGrid[73].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[73].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[73].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 74];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 74];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[74].f1->size[0] * alphaSampled[74].f1->size[1];
    alphaSampled[74].f1->size[0] = 1;
    alphaSampled[74].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[74].f1, k);
    alphaSampled[74].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[74].f1->size[0] = 1;
    alphaSampled[74].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[74].f1->size[0] * alphaSampled[74].f1->size[1];
    alphaSampled[74].f1->size[0] = 1;
    alphaSampled[74].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[74].f1, k);
    alphaSampled[74].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[74].f1->size[0] * alphaSampled[74].f1->size[1];
    alphaSampled[74].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[74].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[74].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[74].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[74].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[74].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[74].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[74].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[74].f1->size[0] * tauGrid[74].f1->size[1];
  tauGrid[74].f1->size[0] = 1;
  tauGrid[74].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[74].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[74].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 75];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 75];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[75].f1->size[0] * alphaSampled[75].f1->size[1];
    alphaSampled[75].f1->size[0] = 1;
    alphaSampled[75].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[75].f1, k);
    alphaSampled[75].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[75].f1->size[0] = 1;
    alphaSampled[75].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[75].f1->size[0] * alphaSampled[75].f1->size[1];
    alphaSampled[75].f1->size[0] = 1;
    alphaSampled[75].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[75].f1, k);
    alphaSampled[75].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[75].f1->size[0] * alphaSampled[75].f1->size[1];
    alphaSampled[75].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[75].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[75].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[75].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[75].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[75].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[75].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[75].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[75].f1->size[0] * tauGrid[75].f1->size[1];
  tauGrid[75].f1->size[0] = 1;
  tauGrid[75].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[75].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[75].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 76];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 76];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[76].f1->size[0] * alphaSampled[76].f1->size[1];
    alphaSampled[76].f1->size[0] = 1;
    alphaSampled[76].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[76].f1, k);
    alphaSampled[76].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[76].f1->size[0] = 1;
    alphaSampled[76].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[76].f1->size[0] * alphaSampled[76].f1->size[1];
    alphaSampled[76].f1->size[0] = 1;
    alphaSampled[76].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[76].f1, k);
    alphaSampled[76].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[76].f1->size[0] * alphaSampled[76].f1->size[1];
    alphaSampled[76].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[76].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[76].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[76].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[76].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[76].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[76].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[76].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[76].f1->size[0] * tauGrid[76].f1->size[1];
  tauGrid[76].f1->size[0] = 1;
  tauGrid[76].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[76].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[76].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 77];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 77];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[77].f1->size[0] * alphaSampled[77].f1->size[1];
    alphaSampled[77].f1->size[0] = 1;
    alphaSampled[77].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[77].f1, k);
    alphaSampled[77].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[77].f1->size[0] = 1;
    alphaSampled[77].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[77].f1->size[0] * alphaSampled[77].f1->size[1];
    alphaSampled[77].f1->size[0] = 1;
    alphaSampled[77].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[77].f1, k);
    alphaSampled[77].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[77].f1->size[0] * alphaSampled[77].f1->size[1];
    alphaSampled[77].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[77].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[77].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[77].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[77].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[77].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[77].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[77].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[77].f1->size[0] * tauGrid[77].f1->size[1];
  tauGrid[77].f1->size[0] = 1;
  tauGrid[77].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[77].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[77].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 78];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 78];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[78].f1->size[0] * alphaSampled[78].f1->size[1];
    alphaSampled[78].f1->size[0] = 1;
    alphaSampled[78].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[78].f1, k);
    alphaSampled[78].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[78].f1->size[0] = 1;
    alphaSampled[78].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[78].f1->size[0] * alphaSampled[78].f1->size[1];
    alphaSampled[78].f1->size[0] = 1;
    alphaSampled[78].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[78].f1, k);
    alphaSampled[78].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[78].f1->size[0] * alphaSampled[78].f1->size[1];
    alphaSampled[78].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[78].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[78].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[78].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[78].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[78].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[78].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[78].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[78].f1->size[0] * tauGrid[78].f1->size[1];
  tauGrid[78].f1->size[0] = 1;
  tauGrid[78].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[78].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[78].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 79];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 79];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[79].f1->size[0] * alphaSampled[79].f1->size[1];
    alphaSampled[79].f1->size[0] = 1;
    alphaSampled[79].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[79].f1, k);
    alphaSampled[79].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[79].f1->size[0] = 1;
    alphaSampled[79].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[79].f1->size[0] * alphaSampled[79].f1->size[1];
    alphaSampled[79].f1->size[0] = 1;
    alphaSampled[79].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[79].f1, k);
    alphaSampled[79].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[79].f1->size[0] * alphaSampled[79].f1->size[1];
    alphaSampled[79].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[79].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[79].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[79].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[79].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[79].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[79].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[79].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[79].f1->size[0] * tauGrid[79].f1->size[1];
  tauGrid[79].f1->size[0] = 1;
  tauGrid[79].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[79].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[79].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 80];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 80];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[80].f1->size[0] * alphaSampled[80].f1->size[1];
    alphaSampled[80].f1->size[0] = 1;
    alphaSampled[80].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[80].f1, k);
    alphaSampled[80].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[80].f1->size[0] = 1;
    alphaSampled[80].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[80].f1->size[0] * alphaSampled[80].f1->size[1];
    alphaSampled[80].f1->size[0] = 1;
    alphaSampled[80].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[80].f1, k);
    alphaSampled[80].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[80].f1->size[0] * alphaSampled[80].f1->size[1];
    alphaSampled[80].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[80].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[80].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[80].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[80].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[80].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[80].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[80].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[80].f1->size[0] * tauGrid[80].f1->size[1];
  tauGrid[80].f1->size[0] = 1;
  tauGrid[80].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[80].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[80].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 81];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 81];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[81].f1->size[0] * alphaSampled[81].f1->size[1];
    alphaSampled[81].f1->size[0] = 1;
    alphaSampled[81].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[81].f1, k);
    alphaSampled[81].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[81].f1->size[0] = 1;
    alphaSampled[81].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[81].f1->size[0] * alphaSampled[81].f1->size[1];
    alphaSampled[81].f1->size[0] = 1;
    alphaSampled[81].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[81].f1, k);
    alphaSampled[81].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[81].f1->size[0] * alphaSampled[81].f1->size[1];
    alphaSampled[81].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[81].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[81].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[81].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[81].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[81].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[81].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[81].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[81].f1->size[0] * tauGrid[81].f1->size[1];
  tauGrid[81].f1->size[0] = 1;
  tauGrid[81].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[81].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[81].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 82];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 82];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[82].f1->size[0] * alphaSampled[82].f1->size[1];
    alphaSampled[82].f1->size[0] = 1;
    alphaSampled[82].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[82].f1, k);
    alphaSampled[82].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[82].f1->size[0] = 1;
    alphaSampled[82].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[82].f1->size[0] * alphaSampled[82].f1->size[1];
    alphaSampled[82].f1->size[0] = 1;
    alphaSampled[82].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[82].f1, k);
    alphaSampled[82].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[82].f1->size[0] * alphaSampled[82].f1->size[1];
    alphaSampled[82].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[82].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[82].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[82].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[82].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[82].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[82].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[82].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[82].f1->size[0] * tauGrid[82].f1->size[1];
  tauGrid[82].f1->size[0] = 1;
  tauGrid[82].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[82].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[82].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 83];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 83];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[83].f1->size[0] * alphaSampled[83].f1->size[1];
    alphaSampled[83].f1->size[0] = 1;
    alphaSampled[83].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[83].f1, k);
    alphaSampled[83].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[83].f1->size[0] = 1;
    alphaSampled[83].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[83].f1->size[0] * alphaSampled[83].f1->size[1];
    alphaSampled[83].f1->size[0] = 1;
    alphaSampled[83].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[83].f1, k);
    alphaSampled[83].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[83].f1->size[0] * alphaSampled[83].f1->size[1];
    alphaSampled[83].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[83].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[83].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[83].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[83].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[83].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[83].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[83].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[83].f1->size[0] * tauGrid[83].f1->size[1];
  tauGrid[83].f1->size[0] = 1;
  tauGrid[83].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[83].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[83].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 84];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 84];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[84].f1->size[0] * alphaSampled[84].f1->size[1];
    alphaSampled[84].f1->size[0] = 1;
    alphaSampled[84].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[84].f1, k);
    alphaSampled[84].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[84].f1->size[0] = 1;
    alphaSampled[84].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[84].f1->size[0] * alphaSampled[84].f1->size[1];
    alphaSampled[84].f1->size[0] = 1;
    alphaSampled[84].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[84].f1, k);
    alphaSampled[84].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[84].f1->size[0] * alphaSampled[84].f1->size[1];
    alphaSampled[84].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[84].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[84].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[84].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[84].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[84].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[84].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[84].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[84].f1->size[0] * tauGrid[84].f1->size[1];
  tauGrid[84].f1->size[0] = 1;
  tauGrid[84].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[84].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[84].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 85];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 85];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[85].f1->size[0] * alphaSampled[85].f1->size[1];
    alphaSampled[85].f1->size[0] = 1;
    alphaSampled[85].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[85].f1, k);
    alphaSampled[85].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[85].f1->size[0] = 1;
    alphaSampled[85].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[85].f1->size[0] * alphaSampled[85].f1->size[1];
    alphaSampled[85].f1->size[0] = 1;
    alphaSampled[85].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[85].f1, k);
    alphaSampled[85].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[85].f1->size[0] * alphaSampled[85].f1->size[1];
    alphaSampled[85].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[85].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[85].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[85].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[85].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[85].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[85].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[85].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[85].f1->size[0] * tauGrid[85].f1->size[1];
  tauGrid[85].f1->size[0] = 1;
  tauGrid[85].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[85].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[85].f1->data[k] = 0.053665631459994957 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 86];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 86];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[86].f1->size[0] * alphaSampled[86].f1->size[1];
    alphaSampled[86].f1->size[0] = 1;
    alphaSampled[86].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[86].f1, k);
    alphaSampled[86].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[86].f1->size[0] = 1;
    alphaSampled[86].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[86].f1->size[0] * alphaSampled[86].f1->size[1];
    alphaSampled[86].f1->size[0] = 1;
    alphaSampled[86].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[86].f1, k);
    alphaSampled[86].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[86].f1->size[0] * alphaSampled[86].f1->size[1];
    alphaSampled[86].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[86].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[86].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[86].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[86].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[86].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[86].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[86].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[86].f1->size[0] * tauGrid[86].f1->size[1];
  tauGrid[86].f1->size[0] = 1;
  tauGrid[86].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[86].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[86].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 87];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 87];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[87].f1->size[0] * alphaSampled[87].f1->size[1];
    alphaSampled[87].f1->size[0] = 1;
    alphaSampled[87].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[87].f1, k);
    alphaSampled[87].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[87].f1->size[0] = 1;
    alphaSampled[87].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[87].f1->size[0] * alphaSampled[87].f1->size[1];
    alphaSampled[87].f1->size[0] = 1;
    alphaSampled[87].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[87].f1, k);
    alphaSampled[87].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[87].f1->size[0] * alphaSampled[87].f1->size[1];
    alphaSampled[87].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[87].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[87].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[87].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[87].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[87].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[87].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[87].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[87].f1->size[0] * tauGrid[87].f1->size[1];
  tauGrid[87].f1->size[0] = 1;
  tauGrid[87].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[87].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[87].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 88];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 88];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[88].f1->size[0] * alphaSampled[88].f1->size[1];
    alphaSampled[88].f1->size[0] = 1;
    alphaSampled[88].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[88].f1, k);
    alphaSampled[88].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[88].f1->size[0] = 1;
    alphaSampled[88].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[88].f1->size[0] * alphaSampled[88].f1->size[1];
    alphaSampled[88].f1->size[0] = 1;
    alphaSampled[88].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[88].f1, k);
    alphaSampled[88].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[88].f1->size[0] * alphaSampled[88].f1->size[1];
    alphaSampled[88].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[88].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[88].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[88].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[88].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[88].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[88].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[88].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[88].f1->size[0] * tauGrid[88].f1->size[1];
  tauGrid[88].f1->size[0] = 1;
  tauGrid[88].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[88].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[88].f1->data[k] = 0.086533230611135742 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 89];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 89];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[89].f1->size[0] * alphaSampled[89].f1->size[1];
    alphaSampled[89].f1->size[0] = 1;
    alphaSampled[89].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[89].f1, k);
    alphaSampled[89].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[89].f1->size[0] = 1;
    alphaSampled[89].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[89].f1->size[0] * alphaSampled[89].f1->size[1];
    alphaSampled[89].f1->size[0] = 1;
    alphaSampled[89].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[89].f1, k);
    alphaSampled[89].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[89].f1->size[0] * alphaSampled[89].f1->size[1];
    alphaSampled[89].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[89].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[89].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[89].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[89].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[89].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[89].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[89].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[89].f1->size[0] * tauGrid[89].f1->size[1];
  tauGrid[89].f1->size[0] = 1;
  tauGrid[89].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[89].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[89].f1->data[k] = 0.067882250993908558 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 90];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 90];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[90].f1->size[0] * alphaSampled[90].f1->size[1];
    alphaSampled[90].f1->size[0] = 1;
    alphaSampled[90].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[90].f1, k);
    alphaSampled[90].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[90].f1->size[0] = 1;
    alphaSampled[90].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[90].f1->size[0] * alphaSampled[90].f1->size[1];
    alphaSampled[90].f1->size[0] = 1;
    alphaSampled[90].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[90].f1, k);
    alphaSampled[90].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[90].f1->size[0] * alphaSampled[90].f1->size[1];
    alphaSampled[90].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[90].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[90].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[90].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[90].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[90].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[90].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[90].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[90].f1->size[0] * tauGrid[90].f1->size[1];
  tauGrid[90].f1->size[0] = 1;
  tauGrid[90].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[90].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[90].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 91];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 91];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[91].f1->size[0] * alphaSampled[91].f1->size[1];
    alphaSampled[91].f1->size[0] = 1;
    alphaSampled[91].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[91].f1, k);
    alphaSampled[91].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[91].f1->size[0] = 1;
    alphaSampled[91].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[91].f1->size[0] * alphaSampled[91].f1->size[1];
    alphaSampled[91].f1->size[0] = 1;
    alphaSampled[91].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[91].f1, k);
    alphaSampled[91].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[91].f1->size[0] * alphaSampled[91].f1->size[1];
    alphaSampled[91].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[91].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[91].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[91].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[91].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[91].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[91].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[91].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[91].f1->size[0] * tauGrid[91].f1->size[1];
  tauGrid[91].f1->size[0] = 1;
  tauGrid[91].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[91].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[91].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 92];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 92];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[92].f1->size[0] * alphaSampled[92].f1->size[1];
    alphaSampled[92].f1->size[0] = 1;
    alphaSampled[92].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[92].f1, k);
    alphaSampled[92].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[92].f1->size[0] = 1;
    alphaSampled[92].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[92].f1->size[0] * alphaSampled[92].f1->size[1];
    alphaSampled[92].f1->size[0] = 1;
    alphaSampled[92].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[92].f1, k);
    alphaSampled[92].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[92].f1->size[0] * alphaSampled[92].f1->size[1];
    alphaSampled[92].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[92].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[92].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[92].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[92].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[92].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[92].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[92].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[92].f1->size[0] * tauGrid[92].f1->size[1];
  tauGrid[92].f1->size[0] = 1;
  tauGrid[92].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[92].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[92].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 93];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 93];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[93].f1->size[0] * alphaSampled[93].f1->size[1];
    alphaSampled[93].f1->size[0] = 1;
    alphaSampled[93].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[93].f1, k);
    alphaSampled[93].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[93].f1->size[0] = 1;
    alphaSampled[93].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[93].f1->size[0] * alphaSampled[93].f1->size[1];
    alphaSampled[93].f1->size[0] = 1;
    alphaSampled[93].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[93].f1, k);
    alphaSampled[93].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[93].f1->size[0] * alphaSampled[93].f1->size[1];
    alphaSampled[93].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[93].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[93].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[93].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[93].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[93].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[93].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[93].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[93].f1->size[0] * tauGrid[93].f1->size[1];
  tauGrid[93].f1->size[0] = 1;
  tauGrid[93].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[93].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[93].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 94];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 94];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[94].f1->size[0] * alphaSampled[94].f1->size[1];
    alphaSampled[94].f1->size[0] = 1;
    alphaSampled[94].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[94].f1, k);
    alphaSampled[94].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[94].f1->size[0] = 1;
    alphaSampled[94].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[94].f1->size[0] * alphaSampled[94].f1->size[1];
    alphaSampled[94].f1->size[0] = 1;
    alphaSampled[94].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[94].f1, k);
    alphaSampled[94].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[94].f1->size[0] * alphaSampled[94].f1->size[1];
    alphaSampled[94].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[94].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[94].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[94].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[94].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[94].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[94].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[94].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[94].f1->size[0] * tauGrid[94].f1->size[1];
  tauGrid[94].f1->size[0] = 1;
  tauGrid[94].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[94].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[94].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 95];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 95];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[95].f1->size[0] * alphaSampled[95].f1->size[1];
    alphaSampled[95].f1->size[0] = 1;
    alphaSampled[95].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[95].f1, k);
    alphaSampled[95].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[95].f1->size[0] = 1;
    alphaSampled[95].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[95].f1->size[0] * alphaSampled[95].f1->size[1];
    alphaSampled[95].f1->size[0] = 1;
    alphaSampled[95].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[95].f1, k);
    alphaSampled[95].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[95].f1->size[0] * alphaSampled[95].f1->size[1];
    alphaSampled[95].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[95].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[95].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[95].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[95].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[95].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[95].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[95].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[95].f1->size[0] * tauGrid[95].f1->size[1];
  tauGrid[95].f1->size[0] = 1;
  tauGrid[95].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[95].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[95].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 96];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 96];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[96].f1->size[0] * alphaSampled[96].f1->size[1];
    alphaSampled[96].f1->size[0] = 1;
    alphaSampled[96].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[96].f1, k);
    alphaSampled[96].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[96].f1->size[0] = 1;
    alphaSampled[96].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[96].f1->size[0] * alphaSampled[96].f1->size[1];
    alphaSampled[96].f1->size[0] = 1;
    alphaSampled[96].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[96].f1, k);
    alphaSampled[96].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[96].f1->size[0] * alphaSampled[96].f1->size[1];
    alphaSampled[96].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[96].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[96].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[96].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[96].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[96].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[96].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[96].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[96].f1->size[0] * tauGrid[96].f1->size[1];
  tauGrid[96].f1->size[0] = 1;
  tauGrid[96].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[96].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[96].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 97];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 97];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[97].f1->size[0] * alphaSampled[97].f1->size[1];
    alphaSampled[97].f1->size[0] = 1;
    alphaSampled[97].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[97].f1, k);
    alphaSampled[97].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[97].f1->size[0] = 1;
    alphaSampled[97].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[97].f1->size[0] * alphaSampled[97].f1->size[1];
    alphaSampled[97].f1->size[0] = 1;
    alphaSampled[97].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[97].f1, k);
    alphaSampled[97].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[97].f1->size[0] * alphaSampled[97].f1->size[1];
    alphaSampled[97].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[97].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[97].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[97].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[97].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[97].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[97].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[97].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[97].f1->size[0] * tauGrid[97].f1->size[1];
  tauGrid[97].f1->size[0] = 1;
  tauGrid[97].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[97].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[97].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 98];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 98];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[98].f1->size[0] * alphaSampled[98].f1->size[1];
    alphaSampled[98].f1->size[0] = 1;
    alphaSampled[98].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[98].f1, k);
    alphaSampled[98].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[98].f1->size[0] = 1;
    alphaSampled[98].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[98].f1->size[0] * alphaSampled[98].f1->size[1];
    alphaSampled[98].f1->size[0] = 1;
    alphaSampled[98].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[98].f1, k);
    alphaSampled[98].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[98].f1->size[0] * alphaSampled[98].f1->size[1];
    alphaSampled[98].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[98].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[98].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[98].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[98].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[98].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[98].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[98].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[98].f1->size[0] * tauGrid[98].f1->size[1];
  tauGrid[98].f1->size[0] = 1;
  tauGrid[98].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[98].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[98].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 99];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 99];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[99].f1->size[0] * alphaSampled[99].f1->size[1];
    alphaSampled[99].f1->size[0] = 1;
    alphaSampled[99].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[99].f1, k);
    alphaSampled[99].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[99].f1->size[0] = 1;
    alphaSampled[99].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[99].f1->size[0] * alphaSampled[99].f1->size[1];
    alphaSampled[99].f1->size[0] = 1;
    alphaSampled[99].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[99].f1, k);
    alphaSampled[99].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[99].f1->size[0] * alphaSampled[99].f1->size[1];
    alphaSampled[99].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[99].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[99].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[99].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[99].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[99].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[99].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[99].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[99].f1->size[0] * tauGrid[99].f1->size[1];
  tauGrid[99].f1->size[0] = 1;
  tauGrid[99].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[99].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[99].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 100];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 100];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[100].f1->size[0] * alphaSampled[100].f1->size[1];
    alphaSampled[100].f1->size[0] = 1;
    alphaSampled[100].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[100].f1, k);
    alphaSampled[100].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[100].f1->size[0] = 1;
    alphaSampled[100].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[100].f1->size[0] * alphaSampled[100].f1->size[1];
    alphaSampled[100].f1->size[0] = 1;
    alphaSampled[100].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[100].f1, k);
    alphaSampled[100].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[100].f1->size[0] * alphaSampled[100].f1->size[1];
    alphaSampled[100].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[100].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[100].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[100].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[100].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[100].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[100].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[100].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[100].f1->size[0] * tauGrid[100].f1->size[1];
  tauGrid[100].f1->size[0] = 1;
  tauGrid[100].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[100].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[100].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 101];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 101];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[101].f1->size[0] * alphaSampled[101].f1->size[1];
    alphaSampled[101].f1->size[0] = 1;
    alphaSampled[101].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[101].f1, k);
    alphaSampled[101].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[101].f1->size[0] = 1;
    alphaSampled[101].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[101].f1->size[0] * alphaSampled[101].f1->size[1];
    alphaSampled[101].f1->size[0] = 1;
    alphaSampled[101].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[101].f1, k);
    alphaSampled[101].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[101].f1->size[0] * alphaSampled[101].f1->size[1];
    alphaSampled[101].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[101].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[101].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[101].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[101].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[101].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[101].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[101].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[101].f1->size[0] * tauGrid[101].f1->size[1];
  tauGrid[101].f1->size[0] = 1;
  tauGrid[101].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[101].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[101].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 102];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 102];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[102].f1->size[0] * alphaSampled[102].f1->size[1];
    alphaSampled[102].f1->size[0] = 1;
    alphaSampled[102].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[102].f1, k);
    alphaSampled[102].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[102].f1->size[0] = 1;
    alphaSampled[102].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[102].f1->size[0] * alphaSampled[102].f1->size[1];
    alphaSampled[102].f1->size[0] = 1;
    alphaSampled[102].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[102].f1, k);
    alphaSampled[102].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[102].f1->size[0] * alphaSampled[102].f1->size[1];
    alphaSampled[102].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[102].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[102].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[102].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[102].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[102].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[102].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[102].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[102].f1->size[0] * tauGrid[102].f1->size[1];
  tauGrid[102].f1->size[0] = 1;
  tauGrid[102].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[102].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[102].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 103];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 103];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[103].f1->size[0] * alphaSampled[103].f1->size[1];
    alphaSampled[103].f1->size[0] = 1;
    alphaSampled[103].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[103].f1, k);
    alphaSampled[103].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[103].f1->size[0] = 1;
    alphaSampled[103].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[103].f1->size[0] * alphaSampled[103].f1->size[1];
    alphaSampled[103].f1->size[0] = 1;
    alphaSampled[103].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[103].f1, k);
    alphaSampled[103].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[103].f1->size[0] * alphaSampled[103].f1->size[1];
    alphaSampled[103].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[103].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[103].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[103].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[103].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[103].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[103].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[103].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[103].f1->size[0] * tauGrid[103].f1->size[1];
  tauGrid[103].f1->size[0] = 1;
  tauGrid[103].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[103].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[103].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 104];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 104];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[104].f1->size[0] * alphaSampled[104].f1->size[1];
    alphaSampled[104].f1->size[0] = 1;
    alphaSampled[104].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[104].f1, k);
    alphaSampled[104].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[104].f1->size[0] = 1;
    alphaSampled[104].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[104].f1->size[0] * alphaSampled[104].f1->size[1];
    alphaSampled[104].f1->size[0] = 1;
    alphaSampled[104].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[104].f1, k);
    alphaSampled[104].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[104].f1->size[0] * alphaSampled[104].f1->size[1];
    alphaSampled[104].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[104].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[104].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[104].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[104].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[104].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[104].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[104].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[104].f1->size[0] * tauGrid[104].f1->size[1];
  tauGrid[104].f1->size[0] = 1;
  tauGrid[104].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[104].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[104].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 105];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 105];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[105].f1->size[0] * alphaSampled[105].f1->size[1];
    alphaSampled[105].f1->size[0] = 1;
    alphaSampled[105].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[105].f1, k);
    alphaSampled[105].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[105].f1->size[0] = 1;
    alphaSampled[105].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[105].f1->size[0] * alphaSampled[105].f1->size[1];
    alphaSampled[105].f1->size[0] = 1;
    alphaSampled[105].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[105].f1, k);
    alphaSampled[105].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[105].f1->size[0] * alphaSampled[105].f1->size[1];
    alphaSampled[105].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[105].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[105].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[105].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[105].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[105].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[105].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[105].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[105].f1->size[0] * tauGrid[105].f1->size[1];
  tauGrid[105].f1->size[0] = 1;
  tauGrid[105].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[105].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[105].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 106];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 106];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[106].f1->size[0] * alphaSampled[106].f1->size[1];
    alphaSampled[106].f1->size[0] = 1;
    alphaSampled[106].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[106].f1, k);
    alphaSampled[106].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[106].f1->size[0] = 1;
    alphaSampled[106].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[106].f1->size[0] * alphaSampled[106].f1->size[1];
    alphaSampled[106].f1->size[0] = 1;
    alphaSampled[106].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[106].f1, k);
    alphaSampled[106].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[106].f1->size[0] * alphaSampled[106].f1->size[1];
    alphaSampled[106].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[106].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[106].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[106].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[106].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[106].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[106].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[106].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[106].f1->size[0] * tauGrid[106].f1->size[1];
  tauGrid[106].f1->size[0] = 1;
  tauGrid[106].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[106].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[106].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 107];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 107];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[107].f1->size[0] * alphaSampled[107].f1->size[1];
    alphaSampled[107].f1->size[0] = 1;
    alphaSampled[107].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[107].f1, k);
    alphaSampled[107].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[107].f1->size[0] = 1;
    alphaSampled[107].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[107].f1->size[0] * alphaSampled[107].f1->size[1];
    alphaSampled[107].f1->size[0] = 1;
    alphaSampled[107].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[107].f1, k);
    alphaSampled[107].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[107].f1->size[0] * alphaSampled[107].f1->size[1];
    alphaSampled[107].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[107].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[107].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[107].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[107].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[107].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[107].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[107].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[107].f1->size[0] * tauGrid[107].f1->size[1];
  tauGrid[107].f1->size[0] = 1;
  tauGrid[107].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[107].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[107].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 108];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 108];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[108].f1->size[0] * alphaSampled[108].f1->size[1];
    alphaSampled[108].f1->size[0] = 1;
    alphaSampled[108].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[108].f1, k);
    alphaSampled[108].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[108].f1->size[0] = 1;
    alphaSampled[108].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[108].f1->size[0] * alphaSampled[108].f1->size[1];
    alphaSampled[108].f1->size[0] = 1;
    alphaSampled[108].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[108].f1, k);
    alphaSampled[108].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[108].f1->size[0] * alphaSampled[108].f1->size[1];
    alphaSampled[108].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[108].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[108].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[108].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[108].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[108].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[108].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[108].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[108].f1->size[0] * tauGrid[108].f1->size[1];
  tauGrid[108].f1->size[0] = 1;
  tauGrid[108].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[108].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[108].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 109];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 109];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[109].f1->size[0] * alphaSampled[109].f1->size[1];
    alphaSampled[109].f1->size[0] = 1;
    alphaSampled[109].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[109].f1, k);
    alphaSampled[109].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[109].f1->size[0] = 1;
    alphaSampled[109].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[109].f1->size[0] * alphaSampled[109].f1->size[1];
    alphaSampled[109].f1->size[0] = 1;
    alphaSampled[109].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[109].f1, k);
    alphaSampled[109].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[109].f1->size[0] * alphaSampled[109].f1->size[1];
    alphaSampled[109].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[109].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[109].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[109].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[109].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[109].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[109].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[109].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[109].f1->size[0] * tauGrid[109].f1->size[1];
  tauGrid[109].f1->size[0] = 1;
  tauGrid[109].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[109].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[109].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 110];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 110];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[110].f1->size[0] * alphaSampled[110].f1->size[1];
    alphaSampled[110].f1->size[0] = 1;
    alphaSampled[110].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[110].f1, k);
    alphaSampled[110].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[110].f1->size[0] = 1;
    alphaSampled[110].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[110].f1->size[0] * alphaSampled[110].f1->size[1];
    alphaSampled[110].f1->size[0] = 1;
    alphaSampled[110].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[110].f1, k);
    alphaSampled[110].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[110].f1->size[0] * alphaSampled[110].f1->size[1];
    alphaSampled[110].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[110].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[110].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[110].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[110].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[110].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[110].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[110].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[110].f1->size[0] * tauGrid[110].f1->size[1];
  tauGrid[110].f1->size[0] = 1;
  tauGrid[110].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[110].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[110].f1->data[k] = 0.0758946638440411 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 111];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 111];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[111].f1->size[0] * alphaSampled[111].f1->size[1];
    alphaSampled[111].f1->size[0] = 1;
    alphaSampled[111].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[111].f1, k);
    alphaSampled[111].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[111].f1->size[0] = 1;
    alphaSampled[111].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[111].f1->size[0] * alphaSampled[111].f1->size[1];
    alphaSampled[111].f1->size[0] = 1;
    alphaSampled[111].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[111].f1, k);
    alphaSampled[111].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[111].f1->size[0] * alphaSampled[111].f1->size[1];
    alphaSampled[111].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[111].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[111].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[111].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[111].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[111].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[111].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[111].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[111].f1->size[0] * tauGrid[111].f1->size[1];
  tauGrid[111].f1->size[0] = 1;
  tauGrid[111].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[111].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[111].f1->data[k] = 0.05366563145999495 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 112];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 112];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[112].f1->size[0] * alphaSampled[112].f1->size[1];
    alphaSampled[112].f1->size[0] = 1;
    alphaSampled[112].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[112].f1, k);
    alphaSampled[112].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[112].f1->size[0] = 1;
    alphaSampled[112].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[112].f1->size[0] * alphaSampled[112].f1->size[1];
    alphaSampled[112].f1->size[0] = 1;
    alphaSampled[112].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[112].f1, k);
    alphaSampled[112].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[112].f1->size[0] * alphaSampled[112].f1->size[1];
    alphaSampled[112].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[112].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[112].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[112].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[112].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[112].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[112].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[112].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[112].f1->size[0] * tauGrid[112].f1->size[1];
  tauGrid[112].f1->size[0] = 1;
  tauGrid[112].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[112].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[112].f1->data[k] = 0.033941125496954279 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 113];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 113];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[113].f1->size[0] * alphaSampled[113].f1->size[1];
    alphaSampled[113].f1->size[0] = 1;
    alphaSampled[113].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[113].f1, k);
    alphaSampled[113].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[113].f1->size[0] = 1;
    alphaSampled[113].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[113].f1->size[0] * alphaSampled[113].f1->size[1];
    alphaSampled[113].f1->size[0] = 1;
    alphaSampled[113].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[113].f1, k);
    alphaSampled[113].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[113].f1->size[0] * alphaSampled[113].f1->size[1];
    alphaSampled[113].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[113].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[113].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[113].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[113].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[113].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[113].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[113].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[113].f1->size[0] * tauGrid[113].f1->size[1];
  tauGrid[113].f1->size[0] = 1;
  tauGrid[113].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[113].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[113].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 114];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 114];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[114].f1->size[0] * alphaSampled[114].f1->size[1];
    alphaSampled[114].f1->size[0] = 1;
    alphaSampled[114].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[114].f1, k);
    alphaSampled[114].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[114].f1->size[0] = 1;
    alphaSampled[114].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[114].f1->size[0] * alphaSampled[114].f1->size[1];
    alphaSampled[114].f1->size[0] = 1;
    alphaSampled[114].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[114].f1, k);
    alphaSampled[114].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[114].f1->size[0] * alphaSampled[114].f1->size[1];
    alphaSampled[114].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[114].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[114].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[114].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[114].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[114].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[114].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[114].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[114].f1->size[0] * tauGrid[114].f1->size[1];
  tauGrid[114].f1->size[0] = 1;
  tauGrid[114].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[114].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[114].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 115];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 115];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[115].f1->size[0] * alphaSampled[115].f1->size[1];
    alphaSampled[115].f1->size[0] = 1;
    alphaSampled[115].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[115].f1, k);
    alphaSampled[115].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[115].f1->size[0] = 1;
    alphaSampled[115].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[115].f1->size[0] * alphaSampled[115].f1->size[1];
    alphaSampled[115].f1->size[0] = 1;
    alphaSampled[115].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[115].f1, k);
    alphaSampled[115].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[115].f1->size[0] * alphaSampled[115].f1->size[1];
    alphaSampled[115].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[115].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[115].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[115].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[115].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[115].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[115].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[115].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[115].f1->size[0] * tauGrid[115].f1->size[1];
  tauGrid[115].f1->size[0] = 1;
  tauGrid[115].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[115].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[115].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 116];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 116];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[116].f1->size[0] * alphaSampled[116].f1->size[1];
    alphaSampled[116].f1->size[0] = 1;
    alphaSampled[116].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[116].f1, k);
    alphaSampled[116].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[116].f1->size[0] = 1;
    alphaSampled[116].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[116].f1->size[0] * alphaSampled[116].f1->size[1];
    alphaSampled[116].f1->size[0] = 1;
    alphaSampled[116].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[116].f1, k);
    alphaSampled[116].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[116].f1->size[0] * alphaSampled[116].f1->size[1];
    alphaSampled[116].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[116].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[116].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[116].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[116].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[116].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[116].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[116].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[116].f1->size[0] * tauGrid[116].f1->size[1];
  tauGrid[116].f1->size[0] = 1;
  tauGrid[116].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[116].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[116].f1->data[k] = 0.072 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 117];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 117];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[117].f1->size[0] * alphaSampled[117].f1->size[1];
    alphaSampled[117].f1->size[0] = 1;
    alphaSampled[117].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[117].f1, k);
    alphaSampled[117].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[117].f1->size[0] = 1;
    alphaSampled[117].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[117].f1->size[0] * alphaSampled[117].f1->size[1];
    alphaSampled[117].f1->size[0] = 1;
    alphaSampled[117].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[117].f1, k);
    alphaSampled[117].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[117].f1->size[0] * alphaSampled[117].f1->size[1];
    alphaSampled[117].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[117].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[117].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[117].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[117].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[117].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[117].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[117].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[117].f1->size[0] * tauGrid[117].f1->size[1];
  tauGrid[117].f1->size[0] = 1;
  tauGrid[117].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[117].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[117].f1->data[k] = 0.024 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 118];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 118];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[118].f1->size[0] * alphaSampled[118].f1->size[1];
    alphaSampled[118].f1->size[0] = 1;
    alphaSampled[118].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[118].f1, k);
    alphaSampled[118].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[118].f1->size[0] = 1;
    alphaSampled[118].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[118].f1->size[0] * alphaSampled[118].f1->size[1];
    alphaSampled[118].f1->size[0] = 1;
    alphaSampled[118].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[118].f1, k);
    alphaSampled[118].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[118].f1->size[0] * alphaSampled[118].f1->size[1];
    alphaSampled[118].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[118].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[118].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[118].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[118].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[118].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[118].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[118].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[118].f1->size[0] * tauGrid[118].f1->size[1];
  tauGrid[118].f1->size[0] = 1;
  tauGrid[118].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[118].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[118].f1->data[k] = 0.048 * x->data[k] / 343.0;
  }

  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 119];
  }

  a = floor(minimum(b_thetaGrid) / 5.0) * 5.0;
  for (k = 0; k < 32761; k++) {
    b_thetaGrid[k] = alpha[120 * k + 119];
  }

  b = ceil(maximum(b_thetaGrid) / 5.0) * 5.0;
  if (rtIsNaN(a) || rtIsNaN(b)) {
    k = alphaSampled[119].f1->size[0] * alphaSampled[119].f1->size[1];
    alphaSampled[119].f1->size[0] = 1;
    alphaSampled[119].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[119].f1, k);
    alphaSampled[119].f1->data[0] = rtNaN;
  } else if (b < a) {
    alphaSampled[119].f1->size[0] = 1;
    alphaSampled[119].f1->size[1] = 0;
  } else if ((rtIsInf(a) || rtIsInf(b)) && (a == b)) {
    k = alphaSampled[119].f1->size[0] * alphaSampled[119].f1->size[1];
    alphaSampled[119].f1->size[0] = 1;
    alphaSampled[119].f1->size[1] = 1;
    emxEnsureCapacity_real_T(alphaSampled[119].f1, k);
    alphaSampled[119].f1->data[0] = rtNaN;
  } else if (a == a) {
    k = alphaSampled[119].f1->size[0] * alphaSampled[119].f1->size[1];
    alphaSampled[119].f1->size[0] = 1;
    nx = (int)floor((b - a) / 5.0);
    alphaSampled[119].f1->size[1] = nx + 1;
    emxEnsureCapacity_real_T(alphaSampled[119].f1, k);
    for (k = 0; k <= nx; k++) {
      alphaSampled[119].f1->data[k] = a + 5.0 * (double)k;
    }
  } else {
    eml_float_colon(a, b, alphaSampled[119].f1);
  }

  k = x->size[0] * x->size[1];
  x->size[0] = 1;
  x->size[1] = alphaSampled[119].f1->size[1];
  emxEnsureCapacity_real_T(x, k);
  nx = alphaSampled[119].f1->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = alphaSampled[119].f1->data[k] * 3.1415926535897931 / 180.0;
  }

  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = cos(x->data[k]);
  }

  k = tauGrid[119].f1->size[0] * tauGrid[119].f1->size[1];
  tauGrid[119].f1->size[0] = 1;
  tauGrid[119].f1->size[1] = x->size[1];
  emxEnsureCapacity_real_T(tauGrid[119].f1, k);
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    tauGrid[119].f1->data[k] = 0.023999999999999997 * x->data[k] / 343.0;
  }

  emxFree_real_T(&x);
}

/*
 * File trailer for MBSS_preprocess.c
 *
 * [EOF]
 */
