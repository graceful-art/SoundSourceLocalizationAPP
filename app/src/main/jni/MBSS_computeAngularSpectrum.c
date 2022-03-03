/*
 * File: MBSS_computeAngularSpectrum.c
 *
 * MATLAB Coder version            : 5.2
 * C/C++ source code generated on  : 03-Mar-2022 13:35:04
 */

/* Include Files */
#include "MBSS_computeAngularSpectrum.h"
#include "main5_emxutil.h"
#include "main5_types.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <string.h>

/* Function Definitions */
/*
 * Function MBSS_computeAngularSpectrum
 *
 *  This function call the localization method designed by functionName with
 *  aggregationParam, X and f as static argument + varargin arguments
 *
 *  INPUT (mandatory):
 *  functionName     : string, the multi-channel angular spectrum method
 *  debugMode        : 1 x 1, flag to display local angular spectra
 *  aggregationParam : struct, containning the 2-channel angular spectra
 *                     aggregation parameter. These parameters are the following
 * :
 *    * pairId       : nPairs x 2, All microphone pair indexes
 *    * d            : nPairs x 1, For each pair, distance (in meters)
 *                     between microphones
 *    * alpha        : nPairs x nGrid : Array of angles for each
 *                     microphone pair corresponding to all {azimut, elevation}
 *                     to be tested.
 *    * alphaSampled : 1 x nPairs cell array, each cell element contains the
 *                     uniformly distributed angles to be tested for the
 *                     corresponding pair
 *    * azimuth      : 1 x nAz, vector of azimuth values
 *    * elevation    : 1 x nEl, vector of elevation values
 *    * tauGrid      : 1 x nMicPair cell array, each cell element contains the
 *                     TDOA corresponding to the alphaSampled for each pair
 *    * nGrid        : Size of the global grid
 *    * nPairs       : 1 x 1, number of pairs
 *    * c            : 1 x 1, speed of sound (m/s)
 *  X                : nfreq x nfram x N : N multichannel time-frequency
 * transformed signals OR nfreq x nfram x N x N : spatial covariance matrices in
 * all time-frequency bins f                : nfreq x 1, frequency axis in Hertz
 *  freqBins         : 1 x K containing the index of frequency bins used for the
 * aggregation
 *
 * Arguments    : const double alpha[604920]
 *                const cell_wrap_0 alphaSampled[120]
 *                const cell_wrap_0 tauGrid[120]
 *                const creal_T X[1572864]
 *                double specInst[15123]
 * Return Type  : void
 */
void MBSS_computeAngularSpectrum(const double alpha[604920],
                                 const cell_wrap_0 alphaSampled[120],
                                 const cell_wrap_0 tauGrid[120],
                                 const creal_T X[1572864],
                                 double specInst[15123])
{
  static creal_T hatRxx[14328];
  static double specCurrentPair[15123];
  static const signed char pairId[240] = {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,
      6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,
      8,  8,  9,  9,  9,  9,  9,  9,  9,  10, 10, 10, 10, 10, 10, 11, 11, 11,
      11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 15, 2,  3,  4,  5,  6,  7,
      8,  9,  10, 11, 12, 13, 14, 15, 16, 3,  4,  5,  6,  7,  8,  9,  10, 11,
      12, 13, 14, 15, 16, 4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
      5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 6,  7,  8,  9,  10, 11,
      12, 13, 14, 15, 16, 7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 8,  9,  10,
      11, 12, 13, 14, 15, 16, 9,  10, 11, 12, 13, 14, 15, 16, 10, 11, 12, 13,
      14, 15, 16, 11, 12, 13, 14, 15, 16, 12, 13, 14, 15, 16, 13, 14, 15, 16,
      14, 15, 16, 15, 16, 16};
  emxArray_real_T *SNR;
  emxArray_real_T *b;
  emxArray_real_T *specSampledgrid;
  emxArray_real_T *x;
  emxArray_real_T *y;
  creal_T EXP[3582];
  creal_T b_y[1194];
  double TR[3582];
  double im;
  double r;
  double re;
  double y_im;
  double y_re;
  int b_i;
  int i;
  int i1;
  int ibtile;
  int k;
  int low_ip1;
  int mid_i;
  int npages;
  signed char hatRxx_tmp[2];
  /*  OUTPUT: */
  /*  specInst : nGrid x nFrames, angular spectrum for each frame */
  /*  */
  /*  Version: v2.0 */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  Copyright 2018 Ewen Camberlein and Romain Lebarbenchon */
  /*  This software is distributed under the terms of the GNU Public License */
  /*  version 3 (http://www.gnu.org/licenses/gpl.txt) */
  /*  If you find it useful, please cite the following reference: */
  /*  http://bass-db.gforge.inria.fr/bss_locate/ */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  */
  /*  Function MVDR_MULTI */
  /*  */
  /*  Compute the MVDR_SPEC algorithm for all pairs of microphones. */
  /*  */
  /*  INPUT: */
  /*  aggregationParam: structure (see the description above) */
  /*  hatRxx:           nfreq x nfram x N x N , spatial covariance matrices in
   * all */
  /*                    time-frequency bins */
  /*  f:                1 x nfreq, frequency axis */
  /*  freqBins:         1 x K containing the index of frequency bins used for */
  /*                    the aggregation */
  /*  debugMode:        1 x 1, flag to display local angular spectra */
  /*  */
  /*  OUTPUT: */
  /*  specInst:         1 x nDirection, angular spectrum */
  /*  */
  /*  Version : v2.0 */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  Copyright 2018 Ewen Camberlein and Romain Lebarbenchon */
  /*  This software is distributed under the terms of the GNU Public License */
  /*  version 3 (http://www.gnu.org/licenses/gpl.txt) */
  /*  If you find it useful, please cite the following reference: */
  /*  http://bass-db.gforge.inria.fr/bss_locate/ */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   */
  /*  Computing the angular spectrum */
  /*  nbin x nFrames x 2 x 2 */
  memset(&specInst[0], 0, 15123U * sizeof(double));
  emxInit_real_T(&specSampledgrid, 2);
  emxInit_real_T(&SNR, 3);
  emxInit_real_T(&y, 3);
  emxInit_real_T(&b, 2);
  emxInit_real_T(&x, 1);
  for (i = 0; i < 120; i++) {
    hatRxx_tmp[0] = pairId[i];
    hatRxx_tmp[1] = pairId[i + 120];
    for (b_i = 0; b_i < 2; b_i++) {
      for (i1 = 0; i1 < 2; i1++) {
        for (low_ip1 = 0; low_ip1 < 3; low_ip1++) {
          for (mid_i = 0; mid_i < 1194; mid_i++) {
            hatRxx[((mid_i + 1194 * low_ip1) + 3582 * i1) + 7164 * b_i] =
                X[(((mid_i + (low_ip1 << 11)) + 6144 * (hatRxx_tmp[i1] - 1)) +
                   98304 * (hatRxx_tmp[b_i] - 1)) +
                  853];
          }
        }
      }
    }
    /*  MVDR_SPEC Computes the SNR in all directions using the MVDR beamformer
     */
    /*  */
    /*  spec = mvdr_spec(hatRxx, f, tauGrid) */
    /*  */
    /*  Inputs: */
    /*  hatRxx : nbin x nFrames x 2 x 2 array containing the spatial covariance
     */
    /*      matrices of the input signal in all time-frequency bins */
    /*  f: nbin x 1 vector containing the center frequency of each frequency bin
     */
    /*      in Hz */
    /*  tauGrid: 1 x ngrid vector of possible TDOAs in seconds */
    /*  */
    /*  Output: */
    /*  spec: nbin x nFrames x ngrid array of SNR values */
    /*  */
    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
     */
    /*  Copyright 2010-2011 Charles Blandin and Emmanuel Vincent */
    /*  This software is distributed under the terms of the GNU Public License
     */
    /*  version 3 (http://www.gnu.org/licenses/gpl.txt) */
    /*  If you find it useful, please cite the following reference: */
    /*  Charles Blandin, Emmanuel Vincent and Alexey Ozerov, "Multi-source TDOA
     */
    /*  estimation in reverberant audio using angular spectra and clustering",
     */
    /*  Signal Processing 92, pp. 1950-1960, 2012. */
    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
     */
    for (b_i = 0; b_i < 3; b_i++) {
      for (i1 = 0; i1 < 1194; i1++) {
        ibtile = i1 + 1194 * b_i;
        TR[ibtile] = hatRxx[ibtile].re + hatRxx[ibtile + 10746].re;
      }
    }
    b_i = SNR->size[0] * SNR->size[1] * SNR->size[2];
    SNR->size[0] = 1194;
    SNR->size[1] = 3;
    i1 = tauGrid[i].f1->size[1];
    SNR->size[2] = tauGrid[i].f1->size[1];
    emxEnsureCapacity_real_T(SNR, b_i);
    for (low_ip1 = 0; low_ip1 < i1; low_ip1++) {
      y_re = tauGrid[i].f1->data[low_ip1] * -0.0;
      y_im = tauGrid[i].f1->data[low_ip1] * -6.2831853071795862;
      for (k = 0; k < 1194; k++) {
        r = 23.4375 * (double)k + 20015.625;
        re = r * y_re;
        im = r * y_im;
        b_y[k].re = re;
        b_y[k].im = im;
        if (im == 0.0) {
          b_y[k].re = exp(re);
          b_y[k].im = 0.0;
        } else {
          r = exp(re / 2.0);
          b_y[k].re = r * (r * cos(im));
          b_y[k].im = r * (r * sin(im));
        }
      }
      for (mid_i = 0; mid_i < 3; mid_i++) {
        ibtile = mid_i * 1194;
        for (k = 0; k < 1194; k++) {
          EXP[ibtile + k] = b_y[k];
          npages = k + 1194 * mid_i;
          re = hatRxx[npages + 7164].re;
          im = hatRxx[npages + 7164].im;
          y_re = TR[npages];
          r = ((hatRxx[npages].re * hatRxx[npages + 10746].re -
                hatRxx[npages].im * hatRxx[npages + 10746].im) -
               (re * hatRxx[npages + 3582].re -
                im * hatRxx[npages + 3582].im)) /
              (y_re - 2.0 * (re * EXP[npages].re - im * EXP[npages].im));
          SNR->data[npages + 3582 * low_ip1] = r / (0.5 * y_re - r);
        }
      }
    }
    /*  */
    /*  sum on frequencies */
    if (SNR->size[2] == 0) {
      y->size[0] = 1;
      y->size[1] = 3;
      y->size[2] = 0;
    } else {
      npages = 1;
      k = 3;
      if (SNR->size[2] == 1) {
        k = 2;
      }
      for (low_ip1 = 2; low_ip1 <= k; low_ip1++) {
        npages *= SNR->size[low_ip1 - 1];
      }
      b_i = y->size[0] * y->size[1] * y->size[2];
      y->size[0] = 1;
      y->size[1] = 3;
      y->size[2] = SNR->size[2];
      emxEnsureCapacity_real_T(y, b_i);
      for (low_ip1 = 0; low_ip1 < npages; low_ip1++) {
        ibtile = low_ip1 * 1194;
        y->data[low_ip1] = SNR->data[ibtile];
        for (k = 0; k < 1023; k++) {
          y->data[low_ip1] += SNR->data[(ibtile + k) + 1];
        }
        r = SNR->data[ibtile + 1024];
        for (k = 0; k < 169; k++) {
          r += SNR->data[(ibtile + k) + 1025];
        }
        y->data[low_ip1] += r;
      }
    }
    if (y->size[2] == 0) {
      b->size[0] = 3;
      b->size[1] = 0;
    } else {
      b_i = b->size[0] * b->size[1];
      b->size[0] = 3;
      b->size[1] = y->size[2];
      emxEnsureCapacity_real_T(b, b_i);
      b_i = y->size[2];
      for (k = 0; k < b_i; k++) {
        b->data[3 * k] = y->data[3 * k];
        i1 = 3 * k + 1;
        b->data[i1] = y->data[i1];
        i1 = 3 * k + 2;
        b->data[i1] = y->data[i1];
      }
    }
    b_i = specSampledgrid->size[0] * specSampledgrid->size[1];
    specSampledgrid->size[0] = b->size[1];
    specSampledgrid->size[1] = 3;
    emxEnsureCapacity_real_T(specSampledgrid, b_i);
    ibtile = b->size[1];
    for (b_i = 0; b_i < 3; b_i++) {
      for (i1 = 0; i1 < ibtile; i1++) {
        specSampledgrid->data[i1 + specSampledgrid->size[0] * b_i] =
            b->data[b_i + 3 * i1];
      }
    }
    /*  Order 1 interpolation on the entire grid */
    ibtile = alphaSampled[i].f1->size[1];
    b_i = x->size[0];
    x->size[0] = alphaSampled[i].f1->size[1];
    emxEnsureCapacity_real_T(x, b_i);
    for (b_i = 0; b_i < ibtile; b_i++) {
      x->data[b_i] = alphaSampled[i].f1->data[b_i];
    }
    for (b_i = 0; b_i < 15123; b_i++) {
      specCurrentPair[b_i] = rtNaN;
    }
    for (k = 0; k < 5041; k++) {
      y_re = alpha[i + 120 * k];
      if ((y_re >= x->data[0]) && (y_re <= x->data[x->size[0] - 1])) {
        ibtile = x->size[0];
        npages = 1;
        low_ip1 = 2;
        while (ibtile > low_ip1) {
          mid_i = (npages >> 1) + (ibtile >> 1);
          if (((npages & 1) == 1) && ((ibtile & 1) == 1)) {
            mid_i++;
          }
          if (y_re >= x->data[mid_i - 1]) {
            npages = mid_i;
            low_ip1 = mid_i + 1;
          } else {
            ibtile = mid_i;
          }
        }
        r = x->data[npages - 1];
        r = (y_re - r) / (x->data[npages] - r);
        y_re = specSampledgrid->data[npages - 1];
        if (y_re == specSampledgrid->data[npages]) {
          specCurrentPair[k] = specSampledgrid->data[npages - 1];
        } else {
          specCurrentPair[k] =
              (1.0 - r) * y_re + r * specSampledgrid->data[npages];
        }
        y_re = specSampledgrid->data[(npages + specSampledgrid->size[0]) - 1];
        if (y_re == specSampledgrid->data[npages + specSampledgrid->size[0]]) {
          specCurrentPair[k + 5041] =
              specSampledgrid->data[(npages + specSampledgrid->size[0]) - 1];
        } else {
          specCurrentPair[k + 5041] =
              (1.0 - r) * y_re +
              r * specSampledgrid->data[npages + specSampledgrid->size[0]];
        }
        y_re =
            specSampledgrid->data[(npages + specSampledgrid->size[0] * 2) - 1];
        if (y_re ==
            specSampledgrid->data[npages + specSampledgrid->size[0] * 2]) {
          specCurrentPair[k + 10082] = y_re;
        } else {
          specCurrentPair[k + 10082] =
              (1.0 - r) * y_re +
              r * specSampledgrid->data[npages + specSampledgrid->size[0] * 2];
        }
      }
    }
    /* Aggregation */
    for (b_i = 0; b_i < 15123; b_i++) {
      specInst[b_i] += specCurrentPair[b_i];
    }
  }
  emxFree_real_T(&x);
  emxFree_real_T(&b);
  emxFree_real_T(&y);
  emxFree_real_T(&SNR);
  emxFree_real_T(&specSampledgrid);
  /*  X is a hatRxx in this case */
}

/*
 * File trailer for MBSS_computeAngularSpectrum.c
 *
 * [EOF]
 */
